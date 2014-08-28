/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonPRD_MultiTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "Identifier/Identifier.h"

#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"

#include <iterator>
#include <typeinfo>

#include <ext/functional>

//================================================================
MuonPRD_MultiTruthMaker::MuonPRD_MultiTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_MDT_ContainerName("MDT_DriftCircles"),
  m_CSC_ContainerName("CSC_Clusters"),
  m_RPC_ContainerName("RPC_Measurements"),
  m_TGC_ContainerName("TGC_Measurements"),
  m_STGC_ContainerName("STGC_Measurements"),
  m_MM_ContainerName("MM_Measurements"),
  m_MDT_SimDataMapName("MDT_SDO"),
  m_CSC_SimDataMapName("CSC_SDO"),
  m_RPC_SimDataMapName("RPC_SDO"),
  m_TGC_SimDataMapName("TGC_SDO"),
  m_STGC_SimDataMapName("STGC_SDO"),
  m_MM_SimDataMapName("MM_SDO"),
  m_MDT_PRD_TruthName("MDT_TruthMap"),
  m_CSC_PRD_TruthName("CSC_TruthMap"),
  m_RPC_PRD_TruthName("RPC_TruthMap"),
  m_TGC_PRD_TruthName("TGC_TruthMap"),
  m_STGC_PRD_TruthName("STGC_TruthMap"),
  m_MM_PRD_TruthName("MM_TruthMap")
{
  // Input
    declareProperty("MDT_PrepRawDataContainer",   m_MDT_ContainerName);
    declareProperty("CSC_PrepRawDataContainer",   m_CSC_ContainerName);
    declareProperty("RPC_PrepRawDataContainer",   m_RPC_ContainerName);
    declareProperty("TGC_PrepRawDataContainer",   m_TGC_ContainerName);
    declareProperty("STGC_PrepRawDataContainer",  m_STGC_ContainerName);
    declareProperty("MM_PrepRawDataContainer",    m_MM_ContainerName);

    declareProperty("MDT_SDO_Container",          m_MDT_SimDataMapName);
    declareProperty("CSC_SDO_Container",          m_CSC_SimDataMapName);
    declareProperty("RPC_SDO_Container",          m_RPC_SimDataMapName);
    declareProperty("TGC_SDO_Container",          m_TGC_SimDataMapName);
    declareProperty("STGC_SDO_Container",         m_STGC_SimDataMapName);
    declareProperty("MM_SDO_Container",           m_MM_SimDataMapName);

  // Output
    declareProperty("MDT_PRD_TruthContainer",     m_MDT_PRD_TruthName);
    declareProperty("CSC_PRD_TruthContainer",     m_CSC_PRD_TruthName);
    declareProperty("RPC_PRD_TruthContainer",     m_RPC_PRD_TruthName);
    declareProperty("TGC_PRD_TruthContainer",     m_TGC_PRD_TruthName);
    declareProperty("STGC_PRD_TruthContainer",    m_STGC_PRD_TruthName);
    declareProperty("MM_PRD_TruthContainer",      m_MM_PRD_TruthName);
}

//================================================================
StatusCode MuonPRD_MultiTruthMaker::initialize()
{
  ATH_MSG_DEBUG( "MuonPRD_MultiTruthMaker::initialize()" );
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MuonPRD_MultiTruthMaker::finalize() 
{
  ATH_MSG_DEBUG( "MuonPRD_MultiTruthMaker::finalize()" );
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode MuonPRD_MultiTruthMaker::execute() {
  ATH_MSG_DEBUG( "MuonPRD_MultiTruthMaker::execute()" );

    std::vector<StatusCode> retvals;
    retvals.push_back(buildPRD_Truth<Muon::MdtPrepDataContainer, MuonSimDataCollection>(m_MDT_ContainerName, m_MDT_SimDataMapName, m_MDT_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::CscPrepDataContainer, CscSimDataCollection> (m_CSC_ContainerName, m_CSC_SimDataMapName, m_CSC_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::RpcPrepDataContainer, MuonSimDataCollection>(m_RPC_ContainerName, m_RPC_SimDataMapName, m_RPC_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::TgcPrepDataContainer, MuonSimDataCollection>(m_TGC_ContainerName, m_TGC_SimDataMapName, m_TGC_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::sTgcPrepDataContainer, MuonSimDataCollection>(m_STGC_ContainerName, m_STGC_SimDataMapName, m_STGC_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::MMPrepDataContainer, MuonSimDataCollection>(m_MM_ContainerName, m_MM_SimDataMapName, m_MM_PRD_TruthName));

    bool ok = true;
    for(std::vector<StatusCode>::const_iterator i = retvals.begin(); i!=retvals.end(); i++) {
      if( i->isFailure() )  ok = false;
    }
    if( ok ) return StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

//================================================================
template<class PrepDataContainer, class SIMDATACOLLECTION>
StatusCode MuonPRD_MultiTruthMaker::buildPRD_Truth(const std::string& prepDataKey, const std::string& sdoKey, const std::string& outputKey) {
    if(!prepDataKey.empty() && !sdoKey.empty() && !outputKey.empty()) {
      
      if( !evtStore()->contains<SIMDATACOLLECTION>(sdoKey) ) {
	ATH_MSG_DEBUG( "SimDataCollection for key="<<sdoKey << " not in storegate, not adding it ");
	return StatusCode::SUCCESS;
      }
        const PrepDataContainer *prdContainer(0);

        if( ! evtStore()->retrieve(prdContainer, prepDataKey).isSuccess() ) {
             ATH_MSG_DEBUG( "Prep data container NOT found: key="<<prepDataKey<<", type "<<typeid(PrepDataContainer).name());
        } else{
             ATH_MSG_DEBUG( "Retrieved prep data container: key="<<prepDataKey<<", type "<<typeid(PrepDataContainer).name());

      // Retrieve SDO map SimDataCollection this event
            const SIMDATACOLLECTION *simDataMap(0);
            if(!evtStore()->retrieve(simDataMap, sdoKey).isSuccess()) {
                 ATH_MSG_DEBUG( "Could NOT find the MuonSimDataMap map key="<<sdoKey);
            } else {
                 ATH_MSG_DEBUG( "Retrieved SimDataCollection for key="<<sdoKey);

    // Create and fill the PRD truth structure
                PRD_MultiTruthCollection *output = new PRD_MultiTruthCollection;
                addPRDCollections(output, prdContainer->begin(), prdContainer->end(), simDataMap);

    // And register it with the StoreGate
                bool allow_modifications;
                if(! evtStore()->record(output, outputKey, allow_modifications=false).isSuccess()) {
                     ATH_MSG_WARNING( "PRD truth structure '" << outputKey
                        << "' could not be registered in StoreGate !" );
                    return StatusCode::SUCCESS;
                } else {
                     ATH_MSG_DEBUG( "PRD truth structure '" << outputKey
                        << "' is registered in StoreGate, size="<<output->size() );
                }
            }
        } 
    }

    return StatusCode::SUCCESS;
}

//================================================================
template<class PRD_Container_Iterator, class SIMDATACOLLECTION>
void MuonPRD_MultiTruthMaker::addPRDCollections(PRD_MultiTruthCollection* prdTruth,
    PRD_Container_Iterator collections_begin,
    PRD_Container_Iterator collections_end,
    const SIMDATACOLLECTION* simDataMap)
{
    for (PRD_Container_Iterator colNext=collections_begin; colNext != collections_end; ++colNext) {
        addPRDRange(prdTruth, (*colNext)->begin(), (*colNext)->end(), simDataMap);
    }
}

//================================================================
// Adds PRDs in the range to prdTruth.
template<class PRD_Collection_Iterator, class SIMDATACOLLECTION>
void MuonPRD_MultiTruthMaker::addPRDRange (PRD_MultiTruthCollection* prdTruth,
    PRD_Collection_Iterator range_begin,
    PRD_Collection_Iterator range_end,
    const SIMDATACOLLECTION* simDataMap) 
{
    for (PRD_Collection_Iterator nextDatum = range_begin; nextDatum != range_end; nextDatum++){
        addPrepRawDatum(prdTruth, *nextDatum, simDataMap);
    }
}

//================================================================
template <class SIMDATACOLLECTION>
void MuonPRD_MultiTruthMaker::addPrepRawDatum(PRD_MultiTruthCollection *prdTruth,
    const Trk::PrepRawData* prd,
    const SIMDATACOLLECTION* simDataMap
    )
{
    if(!prdTruth || !prd || !simDataMap) {
        return;
    }
    ATH_MSG_VERBOSE( "addPrepRawDatum(): new PRD "<<prd<<", id="<<prd->identify()<<", number of RDOs: " << prd->rdoList().size() );

    bool gotSDO = false;
    bool gotValidParticle = false;

  // loop over RDOs
    std::vector<Identifier>::const_iterator nextRDO = prd->rdoList().begin();
    std::vector<Identifier>::const_iterator lastRDO = prd->rdoList().end();   
    for (; nextRDO!=lastRDO; ++nextRDO) {
        typename SIMDATACOLLECTION::const_iterator iter(simDataMap->find(*nextRDO));

        if(iter != simDataMap->end() )  {
            gotSDO = true;
      // Got an SDO.  Try to associate the PRD to MC particles we have info about.
            typedef typename SIMDATACOLLECTION::mapped_type SIMDATA;
            const SIMDATA& sdo = iter->second; 
            const std::vector<   typename SIMDATA::Deposit >& deposits = sdo.getdeposits();
            typename std::vector< typename SIMDATA::Deposit >::const_iterator nextdeposit = deposits.begin();
            typename std::vector< typename SIMDATA::Deposit >::const_iterator lastdeposit = deposits.end();
            for( ; nextdeposit!=lastdeposit; ++nextdeposit) {
                const HepMcParticleLink& particleLink = nextdeposit->first;

                ATH_MSG_VERBOSE("addPrepRawDatum(): Barcode " << particleLink.barcode()<< " evt " << particleLink.eventIndex() );

                if (particleLink.isValid()) {
                    gotValidParticle = true;
      // Associate the particle to the PRD. But don't add duplicates.
      // Note: it may be more efficient to filter out duplicates among particles for the current PRD, then check-and-add the reduced set to the large multimap.
      // But may be not for the typically small RDO/PRD ratio.
                    typedef PRD_MultiTruthCollection::iterator truthiter;
                    std::pair<truthiter, truthiter> r = prdTruth->equal_range(prd->identify());
      // FIXME: Is it OK to use the gcc (SGI) extensions of the STL?
                    if(r.second == std::find_if(r.first, r.second, 
                        __gnu_cxx::compose1(std::bind2nd(std::equal_to<HepMcParticleLink>(), particleLink), 
                        __gnu_cxx::select2nd<PRD_MultiTruthCollection::value_type>() 
                        )
                        )
                        )
                    {
                        prdTruth->insert(std::make_pair(prd->identify(), particleLink));
                    }
                }
            }
        }
    }

    if(gotSDO && !gotValidParticle) {
    // Looked at all the deposits from all the SDOs, but did not find any valid particle link.
    //prdTruth->insert(std::make_pair(prd, particleLinkUnknown));
        ATH_MSG_DEBUG("addPrepRawDatum(): got SDO but no particles");
    }
}

//================================================================
//EOF
