/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#include "MuonTruthAlgs/MuonPRD_MultiTruthMaker.h"

#include "Identifier/Identifier.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonGeoModel/DetectorElement.h"

#include <iterator>
#include <typeinfo>

//#include <ext/functional>

//================================================================
MuonPRD_MultiTruthMaker::MuonPRD_MultiTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
  // Input
    declareProperty("MDT_PrepRawDataContainer",   m_MDT_ContainerName="MDT_DriftCircles");
    declareProperty("CSC_PrepRawDataContainer",   m_CSC_ContainerName="CSC_Clusters");
    declareProperty("RPC_PrepRawDataContainer",   m_RPC_ContainerName="RPC_Measurements");
    declareProperty("TGC_PrepRawDataContainer",   m_TGC_ContainerName="TGC_Measurements");
    declareProperty("STGC_PrepRawDataContainer",  m_STGC_ContainerName="STGC_Measurements");
    declareProperty("MM_PrepRawDataContainer",    m_MM_ContainerName="MM_Measurements");

    declareProperty("MDT_SDO_Container",          m_MDT_SimDataMapName="MDT_SDO");
    declareProperty("CSC_SDO_Container",          m_CSC_SimDataMapName="CSC_SDO");
    declareProperty("RPC_SDO_Container",          m_RPC_SimDataMapName="RPC_SDO");
    declareProperty("TGC_SDO_Container",          m_TGC_SimDataMapName="TGC_SDO");
    declareProperty("STGC_SDO_Container",         m_STGC_SimDataMapName="STGC_SDO");
    declareProperty("MM_SDO_Container",           m_MM_SimDataMapName="MM_SDO");

  // Output
    declareProperty("MDT_PRD_TruthContainer",     m_MDT_PRD_TruthName="MDT_TruthMap");
    declareProperty("CSC_PRD_TruthContainer",     m_CSC_PRD_TruthName="CSC_TruthMap");
    declareProperty("RPC_PRD_TruthContainer",     m_RPC_PRD_TruthName="RPC_TruthMap");
    declareProperty("TGC_PRD_TruthContainer",     m_TGC_PRD_TruthName="TGC_TruthMap");
    declareProperty("STGC_PRD_TruthContainer",    m_STGC_PRD_TruthName="STGC_TruthMap");
    declareProperty("MM_PRD_TruthContainer",      m_MM_PRD_TruthName="MM_TruthMap");

    //technology flags
    declareProperty("UseNSW",                     m_useNSW=false);
    declareProperty("UseCSC",                     m_useCSC=true);
}

//================================================================
StatusCode MuonPRD_MultiTruthMaker::initialize()
{
  ATH_MSG_DEBUG( "MuonPRD_MultiTruthMaker::initialize()" );
  ATH_CHECK(m_MDT_ContainerName.initialize());
  ATH_CHECK(m_CSC_ContainerName.initialize(m_useCSC));
  ATH_CHECK(m_RPC_ContainerName.initialize());
  ATH_CHECK(m_TGC_ContainerName.initialize());
  ATH_CHECK(m_STGC_ContainerName.initialize(m_useNSW));
  ATH_CHECK(m_MM_ContainerName.initialize(m_useNSW));
  ATH_CHECK(m_MDT_SimDataMapName.initialize());
  ATH_CHECK(m_CSC_SimDataMapName.initialize(m_useCSC));
  ATH_CHECK(m_RPC_SimDataMapName.initialize());
  ATH_CHECK(m_TGC_SimDataMapName.initialize());
  ATH_CHECK(m_STGC_SimDataMapName.initialize(m_useNSW));
  ATH_CHECK(m_MM_SimDataMapName.initialize(m_useNSW));
  ATH_CHECK(m_MDT_PRD_TruthName.initialize());
  ATH_CHECK(m_CSC_PRD_TruthName.initialize(m_useCSC));
  ATH_CHECK(m_RPC_PRD_TruthName.initialize());
  ATH_CHECK(m_TGC_PRD_TruthName.initialize());
  ATH_CHECK(m_STGC_PRD_TruthName.initialize(m_useNSW));
  ATH_CHECK(m_MM_PRD_TruthName.initialize(m_useNSW));
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
    if(m_useCSC) retvals.push_back(buildPRD_Truth<Muon::CscPrepDataContainer, CscSimDataCollection> (m_CSC_ContainerName, m_CSC_SimDataMapName, m_CSC_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::RpcPrepDataContainer, MuonSimDataCollection>(m_RPC_ContainerName, m_RPC_SimDataMapName, m_RPC_PRD_TruthName));
    retvals.push_back(buildPRD_Truth<Muon::TgcPrepDataContainer, MuonSimDataCollection>(m_TGC_ContainerName, m_TGC_SimDataMapName, m_TGC_PRD_TruthName));
    if(m_useNSW){
      retvals.push_back(buildPRD_Truth<Muon::sTgcPrepDataContainer, MuonSimDataCollection>(m_STGC_ContainerName, m_STGC_SimDataMapName, m_STGC_PRD_TruthName));
      retvals.push_back(buildPRD_Truth<Muon::MMPrepDataContainer, MuonSimDataCollection>(m_MM_ContainerName, m_MM_SimDataMapName, m_MM_PRD_TruthName));
    }

    bool ok = true;
    for(std::vector<StatusCode>::const_iterator i = retvals.begin(); i!=retvals.end(); i++) {
      if( i->isFailure() )  ok = false;
    }
    if( ok ) return StatusCode::SUCCESS;
    return StatusCode::FAILURE;
}

//================================================================
template<class PrepDataContainer, class SIMDATACOLLECTION>
StatusCode MuonPRD_MultiTruthMaker::buildPRD_Truth(SG::ReadHandleKey<PrepDataContainer> prepDataKey, 
						   SG::ReadHandleKey<SIMDATACOLLECTION> sdoKey, 
						   SG::WriteHandleKey<PRD_MultiTruthCollection> outputKey) {
      
  SG::ReadHandle<SIMDATACOLLECTION> simDataMap(sdoKey);
  if( !simDataMap.isPresent() ) {
    ATH_MSG_DEBUG( "SimDataCollection for key="<<sdoKey.key() << " not in storegate, not adding it ");
    return StatusCode::SUCCESS;
  }

  if(!simDataMap.isValid()){
    ATH_MSG_ERROR("Could not read "<<sdoKey.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<PrepDataContainer> prdContainer(prepDataKey);
  if(!prdContainer.isValid()){
    ATH_MSG_ERROR("Could not read "<<prepDataKey.key());
    return StatusCode::FAILURE;
  }

  // Create and fill the PRD truth structure
  ATH_MSG_DEBUG("make PRD truth for "<<outputKey.key());
  SG::WriteHandle<PRD_MultiTruthCollection> output(outputKey);
  ATH_CHECK(output.record(std::make_unique<PRD_MultiTruthCollection>()));
  addPRDCollections(output, prdContainer->begin(), prdContainer->end(), simDataMap);

  return StatusCode::SUCCESS;
}

//================================================================
template<class PRD_Container_Iterator, class SIMDATACOLLECTION>
void MuonPRD_MultiTruthMaker::addPRDCollections(SG::WriteHandle<PRD_MultiTruthCollection> prdTruth,
						PRD_Container_Iterator collections_begin,
						PRD_Container_Iterator collections_end,
						SG::ReadHandle<SIMDATACOLLECTION> simDataMap)
{
    for (PRD_Container_Iterator colNext=collections_begin; colNext != collections_end; ++colNext) {
        addPRDRange(prdTruth, (*colNext)->begin(), (*colNext)->end(), simDataMap);
    }
}

//================================================================
// Adds PRDs in the range to prdTruth.
template<class PRD_Collection_Iterator, class SIMDATACOLLECTION>
void MuonPRD_MultiTruthMaker::addPRDRange (SG::WriteHandle<PRD_MultiTruthCollection> prdTruth,
					   PRD_Collection_Iterator range_begin,
					   PRD_Collection_Iterator range_end,
					   SG::ReadHandle<SIMDATACOLLECTION> simDataMap) 
{
    for (PRD_Collection_Iterator nextDatum = range_begin; nextDatum != range_end; nextDatum++){
        addPrepRawDatum(prdTruth, *nextDatum, simDataMap);
    }
}

//================================================================
template <class SIMDATACOLLECTION>
void MuonPRD_MultiTruthMaker::addPrepRawDatum(SG::WriteHandle<PRD_MultiTruthCollection> prdTruth,
                                              const Trk::PrepRawData* prd,
                                              SG::ReadHandle<SIMDATACOLLECTION> simDataMap
                                              )
{
  ATH_MSG_VERBOSE( "addPrepRawDatum(): new PRD "<<prd<<", id="<<prd->identify()<<", number of RDOs: " << prd->rdoList().size() );

  bool gotSDO = false;
  bool gotValidParticle = false;

  // loop over RDOs
  for (const auto& nextRDO : prd->rdoList()) {
    typename SIMDATACOLLECTION::const_iterator iter(simDataMap->find(nextRDO));

    if(iter != simDataMap->end() )  {
      gotSDO = true;
      // Got an SDO.  Try to associate the PRD to MC particles we have info about.
      typedef typename SIMDATACOLLECTION::mapped_type SIMDATA;
      const SIMDATA& sdo = iter->second;
      const std::vector<   typename SIMDATA::Deposit >& deposits = sdo.getdeposits();
      if (deposits.empty()) { continue; }
      for (const auto& [particleLink, mcData] : deposits) {
        ATH_MSG_VERBOSE("addPrepRawDatum(): particleLink.isValid() " << particleLink.isValid() );
        ATH_MSG_VERBOSE("addPrepRawDatum(): Barcode " << particleLink.barcode()<< " evt " << particleLink.eventIndex() );
        if (particleLink.isValid()) {
          gotValidParticle = true;
          // Associate the particle to the PRD. But don't add duplicates.
          // Note: it may be more efficient to filter out duplicates among particles for the current PRD, then check-and-add the reduced set to the large multimap.
          // But may be not for the typically small RDO/PRD ratio.
          typedef PRD_MultiTruthCollection::iterator truthiter;
          std::pair<truthiter, truthiter> r = prdTruth->equal_range(prd->identify());
          if(r.second == std::find_if(r.first, r.second,
                                      [ particleLink ](const PRD_MultiTruthCollection::value_type &prd_to_truth) {
                                        return prd_to_truth.second == particleLink;
                                      } )) {
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
