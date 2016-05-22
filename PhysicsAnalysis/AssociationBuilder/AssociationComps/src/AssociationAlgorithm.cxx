/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: AssociationAlgorithm.cxx
//
/**
   @class AssociationAlgorithm

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date January 2010

   @brief AthAlgorithm to match one object container to another using AssociationTools

*/
//=============================================================================

// Preprocessor magic for debugging
#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This classes header
#include "AssociationAlgorithm.h"

// STL includes
#include <string>
#include <vector>
#include <utility>
#include <cfloat>
#include <climits>

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h" 





//=============================================================================
// Constructor
//=============================================================================
AssociationAlgorithm::AssociationAlgorithm( const std::string& name,
                                            ISvcLocator* pSvcLocator ) :
  AthAlgorithm ( name, pSvcLocator ),
  m_assoTools(this),
  m_nEventsProcessed(0)
{
  declareProperty("inputCollection",                m_matchFromCollKey = "", "Input collection to match from" );

  declareProperty("outputAssociationContainerList", m_outCollKeys,
                  "A list of the names (i.e. StoreGate keys) of the output containers/collections that hold the maps of associations" );

  declareProperty("associationTools",               m_assoTools,
                  "List of AssociationTools to be used for object match/association" );

  declareProperty("minNumberPassed",                m_minNumberPassed = 0,
                  "Min number of successfully matched objects to accept the event" );
  declareProperty("maxNumberPassed",                m_maxNumberPassed = INT_MAX,
                  "Max number of successfully matched objects to accept the event" );

  declareProperty("dumpStoreGate",                  m_dumpStoreGate = false, 
                  "Flag to dump StoreGate content after each event" );
}

//=============================================================================
// Destructor
//=============================================================================
AssociationAlgorithm::~AssociationAlgorithm()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode AssociationAlgorithm::initialize()
{
  // Print the used configuration
  if (msgLvl(MSG::INFO))
    {
      msg(MSG::INFO) << "==> initialize " << name() << "..." << endreq ;
      
      // Print out the used configuration
      msg(MSG::INFO) << " using inputCollection                = " << m_matchFromCollKey << endreq;

      msg(MSG::INFO) << " using outputAssociationContainerList = " << m_outCollKeys << endreq;

      msg(MSG::INFO) << " using associationTools               = " << m_assoTools << endreq;

      msg(MSG::INFO) << " using minNumberPassed                = " << m_minNumberPassed << endreq;
      msg(MSG::INFO) << " using maxNumberPassed                = " << m_maxNumberPassed << endreq;

      msg(MSG::INFO) << " using dumpStoreGate                  = " << m_dumpStoreGate << endreq;
    }

   
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;



  // Get all the ISelector tools
  sc = m_assoTools.retrieve();
  if ( sc.isFailure() ) 
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR)
            << "Unable to retrieve all AssociationTools."
            << endreq;
        }
      return sc ;
    }



  // Do some sanity checks on the user configuration
  bool problemFound(false);
  if ( m_matchFromCollKey == "" )
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR)
            << "Empty input collection! Please configure it properly!"
            << " This has to be a collection that inherits from INavigable4Momentum, "
            << "i.e., pretty much any offline object collection." 
            << endreq;
        }
      sc = StatusCode::FAILURE ;
      problemFound = true;
    }
  if ( m_outCollKeys.size() != m_assoTools.size() )
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR)
            << "The outputAssociationCollections has size = " << m_outCollKeys.size()
            << " and the associationTools has size = " << m_assoTools.size()
            << " but they have to have the same size!"
            << " Please configure it properly..."
            << endreq;
        }
      sc = StatusCode::FAILURE ;
      problemFound = true;
    }
  if ( problemFound )
    {
      return sc;
    }


  // Reserve the memory for the needed objects
  m_vecINav4MomAssocs.reserve( m_assoTools.size() );


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
  
  return sc ;

}



//=============================================================================
// Athena execute method
//=============================================================================

StatusCode AssociationAlgorithm::execute()
{
  //-----------------------------------------
  // Increase the event counter
  //-----------------------------------------
  ++m_nEventsProcessed ;


  //-----------------------------------------
  // Simple status message at the beginning of each event execute,
  //-----------------------------------------
  if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) 
        << "==> execute " << name() 
        << " on " << m_nEventsProcessed << ". event..."
        << endreq ;
    }


  //-----------------------------------------
  // Declare the simple StatusCode
  //-----------------------------------------
  StatusCode sc = StatusCode::SUCCESS ;


  //-----------------------------------------
  //Retrieve the container whos objects will be used to match from 
  //-----------------------------------------
  const INavigable4MomentumCollection* inMatchFromContainer(0);
  if ( evtStore()->contains< INavigable4MomentumCollection >( m_matchFromCollKey ) )
    {
      sc = evtStore()->retrieve( inMatchFromContainer, m_matchFromCollKey );
      if ( sc.isFailure() || !inMatchFromContainer )
        {
          if (msgLvl(MSG::WARNING))
            {
              msg(MSG::WARNING) 
                << "Input MatchFrom collection = '" << m_matchFromCollKey
                << "' could not be retrieved from StoreGate! "
                << endreq;
            }
          sc = StatusCode::SUCCESS;
          return sc ;
        } 
      else
        {
          if (msgLvl(MSG::DEBUG))
            {
              msg(MSG::DEBUG) 
                << "Input MatchFrom collection = '" << m_matchFromCollKey
                << "' retrieved from StoreGate" 
                << endreq ;
            } 
        }
    }
  else
    {
      if (msgLvl(MSG::ERROR))
        {
          msg(MSG::ERROR) 
            << "No collection that inherits from INavigable4MomentumCollection with key = '"
            << m_matchFromCollKey
            << "' could be found in StoreGate! "
            << endreq;
        }
      sc = StatusCode::FAILURE;
      return sc ;
    } // End: if/else ( evtStore()->contains< INavigable4MomentumCollection >( m_matchFromCollKey ) )


    



  //-----------------------------------------
  // Create the output INav4MomAssocs map container
  //-----------------------------------------
  // Empty the vector from the previous event
  m_vecINav4MomAssocs.clear();

  // Create the same amount of INav4MomAssocs maps as we have AssociationTools
  for ( unsigned int toolIdx=0; toolIdx < m_assoTools.size(); ++toolIdx )
    {
      INav4MomAssocs* inav4MomAssocs = new INav4MomAssocs;
      if ( m_outCollKeys[toolIdx] != "" )
        {
          sc = evtStore()->record ( inav4MomAssocs, m_outCollKeys[toolIdx] );
          if ( sc.isFailure() || !inav4MomAssocs )
            {
              if (msgLvl(MSG::WARNING))
                {
                  msg(MSG::WARNING)
                    << "Output association map collection = '" << m_outCollKeys[toolIdx]
                    << "' could not be recorded into StoreGate! "
                    << endreq;
                }
              sc = StatusCode::SUCCESS;
              return sc ;
            }
          else
            {
              if (msgLvl(MSG::DEBUG))
                {
                  msg(MSG::DEBUG)
                    << "Output association map collection = '" << m_outCollKeys[toolIdx]
                    << "' recorded into StoreGate" 
                    << endreq ;
                }
            } // End: if/else sc failure
        } // End: if ( m_outCollKey != "" )
      m_vecINav4MomAssocs.push_back( inav4MomAssocs );
    } // End: Add all needed INav4MomAssocs to a vector



  //----------------------------------------------------------
  // Loop over both containers and find the matches
  //----------------------------------------------------------
  unsigned int nObjects(0);
  unsigned int nObjectsWithMatch(0);
  // Loop over the match from container
  if ( inMatchFromContainer )
    {
      // Get the input match-from container and loop over it
      for( unsigned int inIdx = 0; inIdx < inMatchFromContainer->size(); ++inIdx )
        {
          ++nObjects;

          // Loop over the scheduled association tools, try to find associations and store them
          for ( unsigned int toolIdx(0); toolIdx < m_assoTools.size(); ++toolIdx )
            {
              sc = m_assoTools[toolIdx]->calculateAssociations( inMatchFromContainer,
                                                                inIdx,
                                                                *(m_vecINav4MomAssocs[toolIdx]) );
              if ( !sc.isSuccess() )
                {
                  if (msgLvl(MSG::WARNING))
                    {
                      msg(MSG::WARNING) 
                        << "The calculateAssociations method of the AssociationBaseTool"
                        << m_assoTools[toolIdx]->name()
                        << " returned " << sc
                        << endreq;
                    }
                  return sc ;
                } 
              else
                {
                  if (msgLvl(MSG::DEBUG))
                    {
                      msg(MSG::DEBUG) 
                        << "The calculateAssociations method of the AssociationBaseTool"
                        << m_assoTools[toolIdx]->name()
                        << " returned " << sc
                        << endreq ;
                    }
                } // End: if/else StatusCode Failure

              // Increment the counter if at least one match was found
              if ( (m_vecINav4MomAssocs[toolIdx])->getNumberOfAssociations( const_cast< INavigable4Momentum* > ((*inMatchFromContainer)[inIdx]) ) >= 1 )
                {
                  ++nObjectsWithMatch;
                }
              
            } // End: Loop over association tools

        } // End: Loop over inMatchFromContainer
    } // End: if ( inMatchFromContainer )
 


  //-----------------------------------------
  // Set the output container of selected objects as const
  //-----------------------------------------
  for ( unsigned int toolIdx=0; toolIdx < m_assoTools.size(); ++toolIdx )
    {
      if ( m_outCollKeys[toolIdx] != "" )
        {
          sc = evtStore()->setConst( (m_vecINav4MomAssocs[toolIdx]) );
          if ( sc.isFailure() || !(m_vecINav4MomAssocs[toolIdx]) )
            {
              if (msgLvl(MSG::WARNING))
                {
                  msg(MSG::WARNING)
                    << "Output association map collection = '" << m_outCollKeys[toolIdx]
                    << "' could not be set to const in StoreGate! "
                    << endreq;
                }
              return sc ;
            }
          else
            {
              if (msgLvl(MSG::DEBUG))
                {
                  msg(MSG::DEBUG)
                    << "Output association map collection = '" << m_outCollKeys[toolIdx]
                    << "' set to const in StoreGate" 
                    << endreq ;
                }
            } // End: if/else sc failure
        } // End: Loop over all association maps and set them as const in StoreGate
    } // End: if ( m_outCollKey != "" )



  //-----------------------------------------
  // Determine if this event is accepted
  //-----------------------------------------
  if ( nObjectsWithMatch >= m_minNumberPassed 
       && nObjectsWithMatch <= m_maxNumberPassed )
    {
      if (msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) 
            << " Event " << m_nEventsProcessed
            << " passed, used " << nObjects
            << " objects and found " << nObjectsWithMatch
            << " objects with a successful match. Required were minNumber=" << m_minNumberPassed
            << " and maxNumber=" << m_maxNumberPassed
            << endreq;
        }
      setFilterPassed(true);
    }
  else
    {
      if (msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) 
            << " Event " << m_nEventsProcessed
            << " failed, used " << nObjects
            << " objects and found " << nObjectsWithMatch
            << " objects with a successful match. Required were minNumber=" << m_minNumberPassed
            << " and maxNumber=" << m_maxNumberPassed
            << endreq;
        }
      setFilterPassed(false);
    }


  // Writing out all containers in StoreGate to the log file
  if ( m_dumpStoreGate )
    {
      if (msgLvl(MSG::INFO))
        {
          msg(MSG::INFO)
            << "StoreGateDump: \n"
            << evtStore()->dump()
            << endreq ;
        }
    } // End: if ( m_dumpStoreGate )

  
  return sc ;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode AssociationAlgorithm::finalize()
{
  //-----------------------------------------
  // Declare the simple StatusCode
  //-----------------------------------------
  StatusCode sc = StatusCode::SUCCESS ;


  if (msgLvl(MSG::INFO))
    {
      msg(MSG::INFO)
        << "FINALIZING AFTER ALL EVENTS ARE PROCESSED"
        << endreq;
    }

  return sc;
}


