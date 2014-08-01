/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyView/MboyViewDigiMaker.h"
#include "MboyEDMToolInterfaces/IMboyGetPrepDataFromEventTool.h"
#include "MboyEDMToolInterfaces/IMboyPrepDataToDigiStoreTool.h"
#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include <vector>
#include <sstream>
#include <cmath>
using namespace std;

MboyViewDigiMaker::MboyViewDigiMaker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  p_IMboyGetPrepDataFromEventTool("MboyGetPrepDataFromEventTool/MboyRecMboyGetPrepDataFromEventTool"),
  p_IMboyPrepDataToDigiStoreTool("MboyPrepDataToDigiStoreTool/ConfiguredMboyPrepDataToDigiStoreTool") {

  m_mdtId = 0 ;

  declareProperty("DigiStoreLocation", m_DigiStoreLocation = "Muonboy_DigiStore");
  declareProperty("ApproximateRsLine", m_ApproximateRsLine = false);
  declareProperty("SkipUnfoundHits", m_SkipUnfoundHits = false);
  declareProperty("SegmentCollection", m_segmentCollection="ConvertedMBoySegments");

  declareProperty("MboyGetPrepDataFromEventTool", p_IMboyGetPrepDataFromEventTool);
  declareProperty("MboyPrepDataToDigiStoreTool", p_IMboyPrepDataToDigiStoreTool);
}

MboyViewDigiMaker::~MboyViewDigiMaker() {

}

StatusCode MboyViewDigiMaker::initialize() {
  if (p_IMboyGetPrepDataFromEventTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << p_IMboyGetPrepDataFromEventTool);
    return StatusCode::FAILURE;
  }

  if (p_IMboyPrepDataToDigiStoreTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << p_IMboyPrepDataToDigiStoreTool);
    return StatusCode::FAILURE;
  }

  if (detStore()->retrieve(m_mdtId,"MDTIDHELPER").isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve MDTIDHELPER");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MboyViewDigiMaker::execute(){

  StatusCode sc = StatusCode::SUCCESS;


  // Collect PrepRawData
  std::vector<const Trk::PrepRawData*> VectorOfpPrepData ;

  sc = p_IMboyGetPrepDataFromEventTool->CollectPrepData(VectorOfpPrepData);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("CollectPrepData failed");
    return sc;
  }

  // Get approximate radiuses for these prep data, and optionnally remove the
  // prep data which don't have a radius
  std::multimap<Identifier,double> radiusMap;
  std::map<const Muon::MdtPrepData*, double> radiusPrdMap;
  if (m_ApproximateRsLine) {
    sc = makeRadiusMap(radiusMap);
    if (sc != StatusCode::SUCCESS) return sc;
    sc = makeRadiusPrdMap(radiusPrdMap, VectorOfpPrepData, radiusMap);
    if (sc != StatusCode::SUCCESS) return sc;
    sc = removeUnfoundMdtHits(VectorOfpPrepData, radiusPrdMap);
    if (sc != StatusCode::SUCCESS) return sc;
  }

  // Create MuonRecDigiStoreTDS
  MuonRecDigiStoreTDS* pMuonRecDigiStoreTDS = new MuonRecDigiStoreTDS;
  MuonRecDigiStoreAth* pMuonRecDigiStoreAth = &(pMuonRecDigiStoreTDS->m_MuonRecDigiStore);
  pMuonRecDigiStoreAth->ResetMuonRecDigiStoreAth();

  // Fill MuonRecDigiStoreAth and update Digis
  const DataHandle<EventInfo> pEventInfo;
  sc = evtStore()->retrieve(pEventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Retrieve pEventInfo failed");
    return( StatusCode::FAILURE );
  }

  pMuonRecDigiStoreAth->SetRunNber(pEventInfo->event_ID()->run_number());
  pMuonRecDigiStoreAth->SetEventNber(pEventInfo->event_ID()->event_number());

  p_IMboyPrepDataToDigiStoreTool
    ->FillMuonRecDigiStoreWithVectorOfPrepData_Base
    (VectorOfpPrepData, pMuonRecDigiStoreAth);

  if (m_ApproximateRsLine) {
    sc = makeRsLine(pMuonRecDigiStoreAth, radiusPrdMap);
    if (sc != StatusCode::SUCCESS) return sc;
  }

  // Save MuonRecDigiStoreTDS
  sc = evtStore()->record(pMuonRecDigiStoreTDS, m_DigiStoreLocation);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not save pMuonRecDigiStoreTDS at " << m_DigiStoreLocation);
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode MboyViewDigiMaker::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode MboyViewDigiMaker::makeRadiusMap(std::multimap<Identifier,double>& radiusMap) {
  StatusCode sc;

  // Get the list of segments from event
  const Trk::SegmentCollection* segments;
  sc = sgSvc()->retrieve(segments, m_segmentCollection);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve list of segments");
    return sc;
  }

  // Loop on the list of segments
  for (Trk::SegmentCollection::const_iterator
	 it=segments->begin(), end=segments->end();
       it!=end; it++) {
    const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*>(*it);
    if (!seg) continue;

    // Loop on the MDT hits in the segment
    const vector<const Trk::RIO_OnTrack*>& pRIOSet = seg->containedROTs();
    for (vector<const Trk::RIO_OnTrack*>::const_iterator
	   it=pRIOSet.begin(), end=pRIOSet.end();
	 it!=end; it++) {
      const Trk::RIO_OnTrack* rot = *it;
      const Identifier& id = rot->identify();
      if (m_mdtId->is_mdt(id)) {
	// Set the value of the radius in the map
	double driftRadius = rot->localParameters()[Trk::driftRadius];
	radiusMap.insert(make_pair(id,abs(driftRadius)));
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MboyViewDigiMaker::makeRadiusPrdMap (std::map<const Muon::MdtPrepData*, double>& radiusPrdMap,
    const std::vector<const Trk::PrepRawData*>& prepData,
    const std::multimap<Identifier,double>& radiusMap) {
  for (std::vector<const Trk::PrepRawData*>::const_iterator it(prepData.begin()), end(prepData.end());
      it!=end; ++it) {
    const Muon::MdtPrepData* prd = dynamic_cast<const Muon::MdtPrepData*> (*it);
    if (!prd) continue;

    // Look if this ID is in the radius map
    multimap<Identifier,double>::const_iterator rmit =
      radiusMap.find(prd->identify());

    if (rmit != radiusMap.end()) {
      radiusPrdMap[prd] = rmit->second;
      continue;
    }

    if (m_SkipUnfoundHits) continue;

    radiusPrdMap[prd] = 7.5;
  }
  return StatusCode::SUCCESS;
}

StatusCode MboyViewDigiMaker::removeUnfoundMdtHits (std::vector<const Trk::PrepRawData*>& prepData,
    const std::map<const Muon::MdtPrepData*, double>& radiusPrdMap) {
  std::vector<const Trk::PrepRawData*> ret;
  ret.reserve(prepData.size());

  for (std::vector<const Trk::PrepRawData*>::const_iterator it(prepData.begin()), end(prepData.end());
      it!=end; ++it) {
    const Trk::PrepRawData* prd = *it;
    const Muon::MdtPrepData* mdtPrd = dynamic_cast<const Muon::MdtPrepData*> (prd);
    if (!mdtPrd) {
      ret.push_back(prd);
      continue;
    }
    std::map<const Muon::MdtPrepData*, double>::const_iterator
      mit = radiusPrdMap.find(mdtPrd);

    // Don't include in the list, MDT PRDs with no known radius
    if (mit == radiusPrdMap.end()) continue;

    ret.push_back(prd);
  }
  prepData = ret;
  return StatusCode::SUCCESS;
}


StatusCode MboyViewDigiMaker::makeRsLine(MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
    const std::map<const Muon::MdtPrepData*, double> radiusPrdMap) {
  // For each MDT digit i, look for a segment which includes i, and
  // set the digit's RS line with a radius equal to the segment's
  // measurement. If there is no segment holding the digit, set the
  // radius to a default value (7.5)

  int TotalNberOfDigi = pMuonRecDigiStoreAth->NberOfDigi();
  for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++) {
    int Ident = ItemNber;

    MuonRecDigitMDT* pMuonRecDigitMDT = pMuonRecDigiStoreAth->NonCte_LiIdGetMuonRecDigitMDT(ItemNber);
    const Muon::MdtPrepData* pMdtPrd = pMuonRecDigiStoreAth->LiIdGetMdtPrepData(ItemNber);

    if (pMuonRecDigitMDT) {
      if (!pMdtPrd) {
	ATH_MSG_ERROR("MDT PRD not saved in MuonRecDigiStoreAth");
	return StatusCode::FAILURE;
      }
      
      map<const Muon::MdtPrepData*, double>::const_iterator
       	it = radiusPrdMap.find(pMdtPrd);
      if (it == radiusPrdMap.end()) {
	ATH_MSG_ERROR("Cannot find radius for MDT PRD");
	return StatusCode::FAILURE;
      }

      double driftRadius = it->second;

      ostringstream rsline;
      rsline << "RS2 " << Ident << " " << driftRadius;
      pMuonRecDigitMDT->SetRsLine(rsline.str());
    }
  }

  return StatusCode::SUCCESS;
}

