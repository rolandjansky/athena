/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"
#include "TrigHTTSGInput/TrigHTTInputUtils.h"

#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "TrigHTTObjects/HTTTowerInputHeader.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTOptionalEventInfo.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTOfflineHit.h"
#include "TrigHTTObjects/HTTTruthTrack.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"

#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"



HTTRawToLogicalHitsTool::HTTRawToLogicalHitsTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc){}


StatusCode HTTRawToLogicalHitsTool::initialize(){
  ATH_CHECK(m_HTTMapping.retrieve());
  ATH_CHECK(m_EvtSel.retrieve());

  ATH_MSG_INFO("SaveOptional="<<m_saveOptional);
  // remove deferensed m_towersToMap.value() in newer releases!
  if( m_towersToMap.value().empty() ){
    int maxNtowers = m_HTTMapping->RegionMap_2nd()->getNRegions();
    for (int ireg=0;ireg!=maxNtowers;++ireg) m_towers.push_back(ireg);
  }else{
    m_towers = m_towersToMap.value();
  }

  ATH_MSG_DEBUG ("Configured to process "<<m_towers.size() << " towers");
  std::stringstream listOfTowers;
  for (int ireg: m_towers){
    listOfTowers<<", "<<ireg;
  }
  ATH_MSG_DEBUG ("List of Towers: "<<listOfTowers.str());
  return StatusCode::SUCCESS;
}


StatusCode HTTRawToLogicalHitsTool::convert(unsigned stage, const HTTEventInputHeader& eventHeader, HTTLogicalEventInputHeader& logicEventHeader){

  ATH_MSG_DEBUG ("Mapping " << eventHeader.nHits() << " hits using stage " << stage);
  HTTEventInfo eventinfo = eventHeader.event();
  ATH_MSG_DEBUG ("Getting Event " << eventinfo);
  logicEventHeader.newEvent(eventinfo);//this also reset all varaibles

  // Get correct maps
  const HTTPlaneMap *pmap = nullptr;
  const HTTRegionMap *rmap = nullptr;
  if (stage == 1)
  {
      pmap = m_HTTMapping->PlaneMap_1st();
      rmap = m_HTTMapping->RegionMap_1st();
  }
  else if (stage == 2)
  {
      pmap = m_HTTMapping->PlaneMap_2nd();
      rmap = m_HTTMapping->RegionMap_2nd();
  }
  else
  {
      ATH_MSG_FATAL("convert() must have stage == 1 or 2");
  }

  logicEventHeader.reserveTowers(m_towers.size());
  for (int ireg: m_towers){
    HTTTowerInputHeader tower = HTTTowerInputHeader(ireg);//default header, can eventually set eta/phi/deta/dphi
    logicEventHeader.addTower( tower);
  }

  ATH_MSG_DEBUG ("Created "<<logicEventHeader.nTowers()<<" towers. Now map Hits");

  m_missing_hits.clear(); // reset vector of missing hits
  m_missing_hit_codes.clear();


  // fill the towers with mapped hits
  for (auto hit: eventHeader.hits()) { // hit loop
      // map to the logical hit, copying across the truth
      pmap->map(hit);

      // In the ITk geometry, some of the plane IDs are -1 if the layers are not yet being used.
      // This causes the code in this hit loop to crash. As a workaround for the moment, we currently
      // skip over hits in layers that are not included in the HTT geometry, with plane = -1
      if (pmap->getLayerSection(hit.getDetType(), hit.getDetectorZone(), hit.getPhysLayer()).layer == -1)
      {
          // m_missing_hits.push_back(hit); // comment this out so hits in unused layers aren't considered "unmapped"
          continue;
      }

      bool mapped=false;
      for (unsigned int ireg=0;ireg!=m_towers.size();++ireg){
	        if (rmap->isInRegion(ireg, hit)) {
              // if the equivalent hit is compatible with this tower the hit is saved                            
              logicEventHeader.getTower( ireg )->addHit(hit);
              ATH_MSG_VERBOSE ("Hit mapped to tower="<<ireg << ", nHits now=" << logicEventHeader.getTower(ireg)->nHits());
              mapped=true;
          }
      }

      if (!mapped) {
          // get unmapped hit's error codes
          ATH_MSG_DEBUG("Unmapped hit: pixel="<<hit.isPixel()<<" Barrel="<<hit.isBarrel() <<" Layer="<<hit.getLayer() <<" PLayer="<<hit.getPhysLayer() << " section="<<hit.getSection());
          for (unsigned int ireg=0;ireg!=m_towers.size();++ireg){
              int code = rmap->getUnmappedID(m_towers[ireg], hit);
              if (code != 111111) m_missing_hit_codes.push_back(code);
          }
          hit.setHitType(HitType::unmapped);
          m_missing_hits.push_back(hit);
      }
  } // end hit loop

  unsigned int notStoredHits = m_missing_hits.size();
  if (notStoredHits != 0){
      ATH_MSG_DEBUG ("Found " << notStoredHits << " missing mapped hits in this event");
      for (unsigned iter = 0; iter < m_missing_hit_codes.size(); iter+=100) {
        ATH_MSG_VERBOSE ("Missing Hit Code: " << m_missing_hit_codes[iter]);
      }
  }

  if (stage == 1) {
    HTTOptionalEventInfo op = eventHeader.optional();
    if (m_saveOptional == 2) {
      logicEventHeader.setOptional(op);
    }
    else if (m_saveOptional == 1) {
      // save offline tracks  
      HTTOptionalEventInfo newop;
      newop.reserveOfflineTracks(op.nOfflineTracks());
      for (HTTOfflineTrack const & offline_t : op.getOfflineTracks()) newop.addOfflineTrack(offline_t);
      // save truth in region
      for (HTTTruthTrack const & truth_t : op.getTruthTracks()) {
        if (m_EvtSel->passMatching(truth_t)) newop.addTruthTrack(truth_t);
      }
      ATH_MSG_DEBUG("Selected " << newop.nTruthTracks() << " truth tracks");
      logicEventHeader.setOptional(newop);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode HTTRawToLogicalHitsTool::getUnmapped(std::vector<HTTHit>& missing_hits) {
    std::copy(m_missing_hits.begin(), m_missing_hits.end(), back_inserter(missing_hits));    
    return StatusCode::SUCCESS;
}

const HTTPlaneMap* HTTRawToLogicalHitsTool::getPlaneMap_1st() {
  return m_HTTMapping->PlaneMap_1st();
}

