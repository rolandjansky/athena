/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: AthenaBarCodeAssociationTool.cxx
//
/**
   @class AthenaBarCodeAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date March 2010

   @brief Tool to match an object to every object from an input container
   within a given DeltaR range.

*/
//=============================================================================

// This classes header
#include "AthenaBarCodeAssociationTool.h"

// STL includes
#include <string>
#include <vector>
#include <utility>
#include <cfloat>
#include <climits>

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/INav4MomAssocs.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h" 



//=============================================================================
// Constructor
//=============================================================================
AthenaBarCodeAssociationTool::AthenaBarCodeAssociationTool( const std::string& type, 
                                                            const std::string& name,
                                                            const IInterface* parent ) :
  AssociationBaseTool( type, name, parent ),
  m_nCalls(0)
{
  declareProperty("requireSameAthenaBarCodeVersion",   m_requireSameAthenaBarCode = false,
                  "Require that both objects have an identical AthenaBarCode, including having the same version (default=false)" );
}


//=============================================================================
// Destructor
//=============================================================================
AthenaBarCodeAssociationTool::~AthenaBarCodeAssociationTool()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode AthenaBarCodeAssociationTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  ATH_MSG_DEBUG ( " using inputAssociateToCollection      = " << m_matchToCollKey );
  ATH_MSG_DEBUG ( " using requireSameAthenaBarCodeVersion = " << m_requireSameAthenaBarCode );

  // Do some sanity checks on the user configuration
  if ( m_matchToCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty inputAssociateToCollection! Please configure it properly!"
                      << " This has to be a collection that inherits from INavigable4Momentum "
                      << "(i.e., pretty much any offline object collection)" 
                      << " OR a INav4MomLinkContainer"
                      << "(= std::vector< ElementLink< INavigable4MomentumCollection> > >) " );
      return StatusCode::FAILURE ;
    }

  return StatusCode::SUCCESS;
}



//=============================================================================
// The standard method to be called for each particle/inav4mom
//=============================================================================
StatusCode AthenaBarCodeAssociationTool::calculateAssociations( const INavigable4MomentumCollection* objectContainer,
                                                                const size_t objectIndex,
                                                                INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );

  // Update counter
  m_nCalls += 1;


  //-----------------------------------------
  // Get the current object
  //-----------------------------------------
  const INavigable4Momentum* object(NULL);
  object = (*objectContainer)[objectIndex];


  // -----------------------------------------
  // Retrieve the input container where the matches are searched in
  // -----------------------------------------
  unsigned long tmpInContSize(0);
  const INavigable4MomentumCollection* inContainer(NULL);
  const INav4MomLinkContainer* inLinkContainer(NULL);
  //if ( evtStore()->contains< INav4MomLinkContainer >( m_matchToCollKey ) ) // For some strange reason, this DOESN'T WORK!!
  const CLID inav4momLinkClid = ClassID_traits< INav4MomLinkContainer >::ID();
  const CLID inav4momClid = ClassID_traits< INavigable4MomentumCollection >::ID();
  if ( evtStore()->contains( inav4momLinkClid,  m_matchToCollKey ) )
    {
      m_inputIsLinkColl = true;
      CHECK( evtStore()->retrieve( inLinkContainer, m_matchToCollKey ) );
      if ( !inLinkContainer ) return StatusCode::SUCCESS;
      ATH_MSG_DEBUG ( "Input link collection = '" << m_matchToCollKey << "' retrieved from StoreGate" );
      tmpInContSize = inLinkContainer->size();
    }
  else if ( evtStore()->contains( inav4momClid,  m_matchToCollKey ) )
    {
      m_inputIsLinkColl = false;
      CHECK( evtStore()->retrieve( inContainer, m_matchToCollKey ) );
      if ( !inContainer ) return StatusCode::SUCCESS;
      ATH_MSG_DEBUG ( "Input collection = '" << m_matchToCollKey << "' retrieved from StoreGate" );
      tmpInContSize = inContainer->size();
    }
  else
    {
      if ( m_nCalls <= 10 )
        {
          ATH_MSG_WARNING ( "Input (link)collection = '" << m_matchToCollKey
                            << "' could not be retrieved from StoreGate! "
                            << " This message will only be repeated 10 times..." );
        }
      else
        {
          ATH_MSG_DEBUG ( "Input (link)collection = '" << m_matchToCollKey << "' could not be retrieved from StoreGate! " );
        }
      return StatusCode::SUCCESS;
    }
  const unsigned long inContSize(tmpInContSize);



  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( object != NULL )
    {
      // Now, loop over the objects to be matched to 
      for ( unsigned long i=0; i < inContSize; ++i )
        {
          // Get the object for this iterator and cast it to the templated type
          const INavigable4Momentum* nav4momMatchTo(NULL);
          INav4MomLink nav4momMatchToLink;
          if ( m_inputIsLinkColl )
            {
              nav4momMatchToLink = (*inLinkContainer)[i];
              if ( nav4momMatchToLink.isValid() )
                {
                  nav4momMatchTo = (*nav4momMatchToLink);
                }
            }
          else
            {
              nav4momMatchTo = (*inContainer)[i];
              nav4momMatchToLink = INav4MomLink( *inContainer, i ) ;
            }
             
          // Check that the pointer is valid
          if ( nav4momMatchTo != NULL )
            {
              if ( ( !m_requireSameAthenaBarCode && object->hasSameAthenaBarCodeExceptVersion(*nav4momMatchTo) )
                   || ( m_requireSameAthenaBarCode && object->hasSameAthenaBarCode(*nav4momMatchTo) )
                   )
                {
                  // Make sure we have a valid link
                  if ( nav4momMatchToLink.isValid() )
                    {
                      // Get the pointer to the container that containst the positive matches
                      const INavigable4MomentumCollection* matchedObjectsContainer = nav4momMatchToLink.getStorableObjectPointer();
                      
                      // Get the index of the matched object in the container that containst the positive matches
                      const unsigned int matchedIdx = nav4momMatchToLink.index();
                      
                      // Add the current match to the association map
                      inav4MomAssocs.addAssociation( objectContainer,
                                                     objectIndex,
                                                     matchedObjectsContainer,
                                                     matchedIdx );
                      
                      ATH_MSG_DEBUG ( "Found a match with AthenaBarCode = " << object->getAthenaBarCode() );
                    } // End: if ( nav4momMatchToLink.isValid() )

                } // End: Check the AthenaBarCode match

            } // End: Check that the pointer is valid
          else
            {
              ATH_MSG_WARNING ( "No valid pointer for the INavigable4Momentum to match to!" );
            }
        } // End: loop over matchToLinkContainer

    } // End: if ( object != NULL )

  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode AthenaBarCodeAssociationTool::finalize()
{
  ATH_MSG_DEBUG ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );
  return StatusCode::SUCCESS;
}



