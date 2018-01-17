/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: DeltaRAssociationTool.cxx
//
/**
   @class DeltaRAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date March 2010

   @brief Tool to match an object to every object from an input container
   within a given DeltaR range.

*/
//=============================================================================

// This classes header
#include "DeltaRAssociationTool.h"

// STL includes
#include <string>
#include <vector>
#include <utility>
#include <cfloat>
#include <climits>

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/I4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/INav4MomAssocs.h"
#include "FourMomUtils/P4Helpers.h"

// Particle includes
#include "muonEvent/Muon.h"
#include "egammaEvent/egamma.h"
#include "CaloEvent/CaloCluster.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h" 


using namespace CLHEP;


//=============================================================================
// Constructor
//=============================================================================
DeltaRAssociationTool::DeltaRAssociationTool( const std::string& type, 
                                              const std::string& name,
                                              const IInterface* parent ) :
  AssociationBaseTool( type, name, parent ),
  m_resultVecPair(0),
  m_resultVecDeltaR(0),
  m_nCalls(0)
{
  declareProperty("userDataMatchDeltaRName",  m_userDataMatchDeltaRName = "match_deltaR",
                  "The name of the UserData variable that contains the vector of the resulting DeltaR" );

  declareProperty("storeOnlyBestMatch",       m_storeOnlyBestMatch = false,
                  "Store only the best match instead of all allowed matches" );

  declareProperty("useMatchFromCaloClusterPosition", m_useMatchFromCaloClusterPosition = false,
                  "use the calorimeter cluster position of electrons and photons of the object for which a match is searched (default=false)" );
  declareProperty("useMatchToCaloClusterPosition",   m_useMatchToCaloClusterPosition = false,
                  "use the calorimeter cluster position of electrons and photons of the objects to be matched to (default=false)" );

  declareProperty("deltaRMin",                m_deltaRMin = -DBL_MAX, "Min deltaR" );
  declareProperty("deltaRMax",                m_deltaRMax = DBL_MAX,  "Max deltaR" );

  declareProperty("specialMuElFSRMatchCut",   m_trackMatchCut = DBL_MAX,  "e/mu track match cut" );
  declareProperty("doSpecialMuElFSRMatch",    m_doFsr = false,  "Find Fsr candidates" );

}

//=============================================================================
// Destructor
//=============================================================================
DeltaRAssociationTool::~DeltaRAssociationTool()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode DeltaRAssociationTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  ATH_MSG_DEBUG ( " using inputAssociateToCollection      = " << m_matchToCollKey );
  ATH_MSG_DEBUG ( " using userDataPrefix                  = " << m_userDataPrefix );
  ATH_MSG_DEBUG ( " using userDataMatchDeltaRName         = " << m_userDataMatchDeltaRName );
  ATH_MSG_DEBUG ( " using storeOnlyBestMatch              = " << m_storeOnlyBestMatch );
  ATH_MSG_DEBUG ( " using useMatchFromCaloClusterPosition = " << m_useMatchFromCaloClusterPosition );
  ATH_MSG_DEBUG ( " using useMatchToCaloClusterPosition   = " << m_useMatchToCaloClusterPosition );
  ATH_MSG_DEBUG ( " using deltaRMin                       = " << m_deltaRMin );
  ATH_MSG_DEBUG ( " using deltaRMax                       = " << m_deltaRMax );
  ATH_MSG_DEBUG ( " using specialMuElFSRMatchCut          = " << m_trackMatchCut );
  ATH_MSG_DEBUG ( " using doSpecialMuElFSRMatch           = " << m_doFsr );

  // Do some sanity checks on the user configuration
  if (m_writeUserData) {
    ATH_MSG_WARNING ("Writing to UserData requested, but UserDataSvc is obsolete.");
  }

  if ( m_matchToCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty inputAssociateToCollection! Please configure it properly!"
                      << " This has to be a collection that inherits from INavigable4Momentum "
                      << "(i.e., pretty much any offline object collection)" 
                      << " OR a INav4MomLinkContainer"
                      << "(= std::vector< ElementLink< INavigable4MomentumCollection> > >) " );
      return StatusCode::FAILURE ;
    }

  // Create new vectors for this run.
  // Also, reserve some minimum memory capacity to avoid the first few reallocations
  delete m_resultVecPair; m_resultVecPair = 0;
  m_resultVecPair = new std::vector< std::pair< double, INav4MomLink > >;
  m_resultVecPair->reserve(5);
  delete m_resultVecDeltaR; m_resultVecDeltaR = 0;
  m_resultVecDeltaR = new std::vector< double >;
  m_resultVecDeltaR->reserve(5);

  // Get the names for the UserData output
  m_userDataMatchDeltaRName.insert( 0, m_userDataPrefix );

  return StatusCode::SUCCESS;
}



//=============================================================================
// The standard method to be called for each particle/inav4mom
//=============================================================================

StatusCode DeltaRAssociationTool::calculateAssociations( const INavigable4MomentumCollection* objectContainer,
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
      // Clear the vectors from the previous matching
      m_resultVecPair->clear();
      m_resultVecDeltaR->clear();


      // Get the LorentzVector from the object for with matches are searched
      HepLorentzVector matchFromHLV( object->hlv() );

      // Get the I4Momentum from the object for with matches are searched
      const I4Momentum* matchFromI4Mom(NULL);

      // Get the cluster I4Momentum instead, if wanted
      if ( m_useMatchFromCaloClusterPosition )
        {
          const egamma* eg = dynamic_cast< const egamma* >(object);
          if ( eg )
            {
              ATH_MSG_DEBUG ( "Could dynamic_cast to an electron or photon. Using its cluster for matching!" );
              matchFromI4Mom = eg->cluster();
            }
        }
      else
        {
          matchFromI4Mom = object;
        } // End: if ( m_useClusterPosition )


      // Check that the pointer is valid
      if ( matchFromI4Mom != NULL && matchFromI4Mom->p() != 0 )
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
              if ( nav4momMatchTo )
                {
                  
                  // Get the I4Momentum from the object to with to match to
                  const I4Momentum* matchToI4Mom(NULL);
                  
                  // Get the cluster I4Momentum instead, if wanted
                  if ( m_useMatchToCaloClusterPosition )
                    {
                      const egamma* eg = dynamic_cast< const egamma* >(nav4momMatchTo);
                      if ( eg )
                        {
                          ATH_MSG_DEBUG ( "Could dynamic_cast to an electron or photon. Using its cluster for matching!" );
                          matchToI4Mom = eg->cluster();
                        }
                    }
                  else
                    {
                      matchToI4Mom = nav4momMatchTo;
                    } // End: if ( m_useMatchToCaloClusterPosition )

                  // Check that the pointer is valid
                  if ( matchToI4Mom != NULL && matchToI4Mom->p() != 0. )
                    {

                      // If running in FSR mode and using muon-electron match
                      if ( m_doFsr ) 
                        {
                          // Get the track of the associated object
                          const Analysis::Muon* muon = dynamic_cast< const Analysis::Muon* >(matchFromI4Mom);
                          const egamma* eg = dynamic_cast< const egamma* >(matchToI4Mom);
              
                          if ( eg != NULL && muon != NULL ) 
                            { 
                              bool foundMatch = false;
                              bool doMatching = false; 
                              if ( eg->author(egammaParameters::AuthorElectron) )
                                {
                                  doMatching = true;
                                  foundMatch = findElectronMuonMatch(muon, eg);
                                }
                              
                              // no track matching  no fsr for electrons  
                              if ( doMatching && !foundMatch ) continue;
                            } 
                          else
                            {
                              ATH_MSG_WARNING ( "No valid pointer for egamma and/or muon object!" );
                            }  // check eg NULL pointer
                        } // end of Fsr 


                      // Calculate the match
                      double deltaR(-99.9);
                      deltaR = P4Helpers::deltaR( matchFromI4Mom, matchToI4Mom );
                      
                      // Store the match if deltaR is within the allowed range
                      if ( deltaR > m_deltaRMin && deltaR < m_deltaRMax )
                        {
                          m_resultVecPair->push_back( std::make_pair( deltaR, nav4momMatchToLink ) );
                        } // End: Check if deltaR is within allowed range
              
                    } // End: Check that the pointer is valid
                  else
                    {
                      ATH_MSG_WARNING ( "No valid pointer for the INavigable4Momentum to match to!" );
                    }
                } // End: Check that the pointer is valid
              else
                {
                  ATH_MSG_WARNING ( "No valid pointer for the I4Momentum to match to!" );
                }

            } // End: loop over matchToLinkContainer
          
        } // End: Check that the pointer is valid
      else
        {
          ATH_MSG_WARNING ( "No valid pointer for the I4Momentum to match from!" );
        }
          


      // Now, sort the results from smallest DeltaR to largest DeltaR
      std::sort( m_resultVecPair->begin(), m_resultVecPair->end() );


      // Fill the resulting sorted vector of ElementLinks
      for( unsigned int idx(0); idx < m_resultVecPair->size(); ++idx )
        {
          if ( !m_storeOnlyBestMatch || ( m_storeOnlyBestMatch && idx<1 ) )
            {
              // Get the current link
              const INav4MomLink thisMatchLink = m_resultVecPair->at(idx).second;
              
              // Make sure we have a valid link
              if ( thisMatchLink.isValid() )
                {
                  // Get the pointer to the container that containst the positive matches
                  const INavigable4MomentumCollection* matchedObjectsContainer = thisMatchLink.getStorableObjectPointer();
                  
                  // Get the index of the matched object in the container that containst the positive matches
                  const unsigned int matchedIdx = thisMatchLink.index();
                  
                  // Add the current match to the association map
                  inav4MomAssocs.addAssociation( objectContainer,
                                                 objectIndex,
                                                 matchedObjectsContainer,
                                                 matchedIdx );

                  ATH_MSG_DEBUG ( "Found a match with DeltaR = " << m_resultVecPair->at(idx).first );
                } // End: if ( thisMatchLink.isValid() )
            }
        } // End: Loop over all sorted pairs
    } // End: if ( object != NULL )

  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode DeltaRAssociationTool::finalize()
{
  // Clean up everything that is not needed any more
  delete m_resultVecPair; m_resultVecPair = 0;
  delete m_resultVecDeltaR; m_resultVecDeltaR = 0;

  ATH_MSG_DEBUG ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );

  return StatusCode::SUCCESS;
}



bool DeltaRAssociationTool::findElectronMuonMatch(const Analysis::Muon* muon , const egamma* eg)
{

  bool foundTrackMatch = false;

  const Rec::TrackParticle* mutrk = muon->track();
  const Rec::TrackParticle* egtrk = eg->trackParticle();
  if ( mutrk  &&  egtrk )
    {
      double trackMatchTheta = fabs( mutrk->measuredPerigee()->parameters()[Trk::theta]
                                     - egtrk->measuredPerigee()->parameters()[Trk::theta] );
      double trackMatchPhi = fabs( mutrk->measuredPerigee()->parameters()[Trk::phi]
                                   - egtrk->measuredPerigee()->parameters()[Trk::phi] );
      if ( (trackMatchTheta < m_trackMatchCut) && (trackMatchPhi < m_trackMatchCut) )
        {
          foundTrackMatch = true;
        }
    }

  return foundTrackMatch;
}

