/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// PRD_PlanarTruthTrajectoryBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// package include
#include "ISF_FatrasRecoTools/PRD_PlanarTruthTrajectoryBuilder.h"

// Trk
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkToolInterfaces/IPRD_Provider.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryManipulator.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasDetectorID.h"

// HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

/** Constructor **/
iFatras::PRD_PlanarTruthTrajectoryBuilder::PRD_PlanarTruthTrajectoryBuilder(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_idHelper(0),
  m_idPrdPlanarClusterProvider(""),
  m_minPt(400.),
  m_geantinos(false)
{
    declareInterface<Trk::IPRD_TruthTrajectoryBuilder>(this);
    // the PRD multi truth collections this builder works on
    declareProperty("PRD_PlanarMultiTruthCollections", m_prdPlanarMultiTruthCollectionNames);
    // the PRD providers that turn Identifier -> IdentiferHash and get the PRD
    declareProperty("InDetPRD_PlanarClusterProvider",  m_idPrdPlanarClusterProvider);
    // the PRD manipulators
    declareProperty("PRD_TruthTrajectoryManipulators", m_prdTruthTrajectoryManipulators);
    // Minimum PT cut
    declareProperty("MinimumPt",         m_minPt);
    // Track geantinos
    declareProperty("Geantinos",         m_geantinos);
}

// Athena algtool's Hooks - initialize
StatusCode  iFatras::PRD_PlanarTruthTrajectoryBuilder::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");
    // Set up ATLAS ID helper to be able to identify the PRD's det-subsystem
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
         ATH_MSG_ERROR ("Could not get AtlasDetectorID helper. Arborting ...");
         return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG ("AtlasDetectorID helper retrieved.");
    
    // get the ID PRD Provider
    if ( !m_idPrdPlanarClusterProvider.empty() && m_idPrdPlanarClusterProvider.retrieve().isFailure()){
      ATH_MSG_ERROR ("Could not get " << m_idPrdPlanarClusterProvider << ". Arborting ..." );
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG ("PRD Provider for PlanarClusters retrieved.");
    
    // get the manipulators
    if ( m_prdTruthTrajectoryManipulators.size()  && m_prdTruthTrajectoryManipulators.retrieve().isFailure()){
      ATH_MSG_ERROR ("Could not get configured " << m_prdTruthTrajectoryManipulators << ". Arborting ..." );
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG ("TruthTrajectoryManipulator retrieved.");
    
    ATH_MSG_VERBOSE("Initialization successfully finished!");

    return StatusCode::SUCCESS;
}

StatusCode iFatras::PRD_PlanarTruthTrajectoryBuilder::refreshEvent()  {

   ATH_MSG_VERBOSE("Calling refreshEvent() to reset cache and retrieve collections");
   // clear the cache & reserve
   m_gpPrdTruthTrajectories.clear();
   m_prdPlanarMultiTruthCollections.clear();
   m_prdPlanarMultiTruthCollections.reserve(m_prdPlanarMultiTruthCollectionNames.size());
   
   // load the PRD collections from SG for PlanarClusters
   std::vector< std::string >::iterator pmtPlanarCollNameIter  = m_prdPlanarMultiTruthCollectionNames.begin();
   std::vector< std::string >::iterator pmtPlanarCollNameIterE = m_prdPlanarMultiTruthCollectionNames.end();
   for ( ;  pmtPlanarCollNameIter != pmtPlanarCollNameIterE; ++pmtPlanarCollNameIter ) {
       // try to retrieve the PRD multi truth collection for PlanarClusters
       const PRD_MultiTruthCollection* curColl = 0;
       if ( evtStore()->retrieve(curColl,(*pmtPlanarCollNameIter)).isFailure() )
           ATH_MSG_WARNING("Could not retrieve " << (*pmtPlanarCollNameIter) << ". Ignoring ... ");
       else if (curColl){
           ATH_MSG_VERBOSE("Added " << (*pmtPlanarCollNameIter) << " to collection list for truth track creation.");
           m_prdPlanarMultiTruthCollections.push_back(curColl);
       }
   }   
   
   // retrieve collection call to the PRD_PlanarClusterProviders
   if (!m_idPrdPlanarClusterProvider.empty() && m_idPrdPlanarClusterProvider->retrieveCollection().isFailure()){
     ATH_MSG_ERROR ("Failure in collection retrieval of " << m_idPrdPlanarClusterProvider << ". Arborting ..." );
     return StatusCode::FAILURE; 
   }
   else {
     ATH_MSG_INFO ("collection retrieval of " << m_idPrdPlanarClusterProvider << ". Done!" );
   }
   
   // all good
   return StatusCode::SUCCESS;
   
}

const std::map< const HepMC::GenParticle*, Trk::PRD_TruthTrajectory >& iFatras::PRD_PlanarTruthTrajectoryBuilder::truthTrajectories() const {
    // ndof
    size_t ndofTotal = 0;
    size_t ndof      = 0;
    
    // PART 1 for Planar clusters--------------------------------------------------------------------------------------------------------
    // loop over the PRD_MultiTruthCollection, search for the PRD and create (if necessary and entry in the return map)
    std::vector<const PRD_MultiTruthCollection*>::const_iterator pmtPlanarCollIter  = m_prdPlanarMultiTruthCollections.begin();
    std::vector<const PRD_MultiTruthCollection*>::const_iterator pmtPlanarCollIterE = m_prdPlanarMultiTruthCollections.end();
    int first_count = 0;
    int second_count = 0;
    for ( ; pmtPlanarCollIter != pmtPlanarCollIterE; ++pmtPlanarCollIter ){
        // loop over the map and get the identifier, GenParticle relation
        PRD_MultiTruthCollection::const_iterator prdMtCIter  = (*pmtPlanarCollIter)->begin();
        PRD_MultiTruthCollection::const_iterator prdMtCIterE = (*pmtPlanarCollIter)->end();
	ATH_MSG_VERBOSE("  1-Count " << first_count++ );
	ATH_MSG_VERBOSE("  Coll size =  " << (*pmtPlanarCollIter)->size() );
        for ( ; prdMtCIter != prdMtCIterE; ++ prdMtCIter ){
	  ATH_MSG_VERBOSE("  2-Count " << second_count++ );
            // check if entry exists and if   
            const HepMC::GenParticle* curGenP       = (*prdMtCIter).second;
            Identifier                curIdentifier = (*prdMtCIter).first;
            // apply the min pT cut 
            if ( curGenP->momentum().perp() < m_minPt ) continue;
            // skip geantinos if required
            if (!m_geantinos && std::abs(curGenP->pdg_id())==999) continue;
            // get the associated PRD from the provider
            const Trk::PrepRawData* prd = 0;
	    if (m_idHelper->is_indet(curIdentifier))
	      prd = m_idPrdPlanarClusterProvider->prdFromIdentifier(curIdentifier,ndof);
	    // stuff it into the trajectory if you found a PRD
            if (prd){
	      ATH_MSG_VERBOSE("  I got the PRD for Id = " << curIdentifier );
                // try to find the entry for this GenParticle 
                std::map< const HepMC::GenParticle*, Trk::PRD_TruthTrajectory >::iterator prdTrajIter = m_gpPrdTruthTrajectories.find(curGenP);
                if ( prdTrajIter ==  m_gpPrdTruthTrajectories.end() ){
                    // first PRD associated to this: create PRD_TruthTrajectory object
                    Trk::PRD_TruthTrajectory newPrdTruthTrajectory;
                    newPrdTruthTrajectory.prds.push_back(prd);
                    newPrdTruthTrajectory.nDoF = ndof-5;
                    // register the GenParticle only once
                    newPrdTruthTrajectory.genParticle = curGenP;
                    // fill into map
                    m_gpPrdTruthTrajectories[curGenP] = newPrdTruthTrajectory;
                    ndofTotal = ndof;
                } else {
                    // this PRD_TruthTrajectory already exists
                    (prdTrajIter->second).prds.push_back(prd);
                    (prdTrajIter->second).nDoF += ndof;
                    ndofTotal = (prdTrajIter->second).nDoF;
                }
                ATH_MSG_VERBOSE("  Associating PRD for Planar Cluster with " << ndof << " degrees of freedom, total N.d.o.F : " << ndofTotal );
                ATH_MSG_VERBOSE("  Associating Identifier " << curIdentifier << " with particle at [ " << curGenP << " ]." );
            } else ATH_MSG_VERBOSE("  NO PRD for Id = " << curIdentifier );
        }        
    }

    // PART 2 --------------------------------------------------------------------------------------------------------
    // loop through the provided list of manipulators ( sorter is included )
    std::map< const HepMC::GenParticle*, Trk::PRD_TruthTrajectory >::iterator prdTruthTrajIter  = m_gpPrdTruthTrajectories.begin();
    std::map< const HepMC::GenParticle*, Trk::PRD_TruthTrajectory >::iterator prdTruthTrajIterE = m_gpPrdTruthTrajectories.end();
    for ( ; prdTruthTrajIter != prdTruthTrajIterE; ++prdTruthTrajIter ){
    //std::cout << "sorting, barcode: " << prdTruthTrajIter->first->barcode() << std::endl;
        if ( m_prdTruthTrajectoryManipulators.size() ){
            ToolHandleArray<Trk::IPRD_TruthTrajectoryManipulator>::const_iterator prdTTMIter  = m_prdTruthTrajectoryManipulators.begin();
            ToolHandleArray<Trk::IPRD_TruthTrajectoryManipulator>::const_iterator prdTTMIterE = m_prdTruthTrajectoryManipulators.end();
            for ( ; prdTTMIter != prdTTMIterE; ++prdTTMIter ){
                if ((*prdTTMIter)->manipulateTruthTrajectory((*prdTruthTrajIter).second))
                    ATH_MSG_VERBOSE("PRD truth trajectory got manipulated by: " << (*prdTTMIter).name() );
            }
        }
    }
    // return the truth trajectories and leave it to the TruthTrack creation to proceed further
    return m_gpPrdTruthTrajectories;
}
                                    
StatusCode  iFatras::PRD_PlanarTruthTrajectoryBuilder::finalize()
{
    // clear the cache a last time
    m_gpPrdTruthTrajectories.clear();
    //m_prdPlanarMultiTruthCollections.clear();    
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

