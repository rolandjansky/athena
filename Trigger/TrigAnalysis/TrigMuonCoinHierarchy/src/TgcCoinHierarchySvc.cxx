/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchySvc.h"

#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IIncidentSvc.h"

#include "TrkTrack/Track.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

using Muon::TgcCoinData;
using Muon::TgcPrepData;
using Muon::MuonCluster;

namespace Trigger {

  TgcCoinHierarchySvc::TgcCoinHierarchySvc(const std::string& name, ISvcLocator* svc) : 
    AthService(name, svc),
    m_classifyTool("Trigger::TgcCoinHierarchyClassifyTool"), 
    m_findTool("Trigger::TgcCoinHierarchyFindTool"),
    m_debug(false), 
    m_newEvent(true) 
  {
    // Flag of ContainedROTS treatment Trk::Track with CompetingRIOsOnTrack
    declareProperty("loopOverContainedROTS", m_loopOverContainedROTS=false);
  }
  
  TgcCoinHierarchySvc::~TgcCoinHierarchySvc(void) 
  {}

  StatusCode TgcCoinHierarchySvc::queryInterface(const InterfaceID& riid,
                                                 void** ppvIF) 
  {
    if(IIncidentListener::interfaceID().versionMatch(riid)) { 
      *ppvIF = dynamic_cast<IIncidentListener*>(this); 
    } else if(ITgcCoinHierarchySvc::interfaceID().versionMatch(riid)) {
      *ppvIF = dynamic_cast<ITgcCoinHierarchySvc*>(this); 
    } else {
      return AthService::queryInterface(riid, ppvIF);
    }

    addRef();
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchySvc::initialize(void) {
    m_debug = outputLevel() <= MSG::DEBUG;

    ATH_MSG_DEBUG("initialize()");

    // Retrieve TgcCoinHierarchyClassifyTool 
    StatusCode sc = m_classifyTool.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("ITgcCoinHierarchyClassifyTool could not be retrieved");
      return sc;
    }

    // Retrieve TgcCoinHierarchyFindTool 
    sc = m_findTool.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("ITgcCoinHierarchyFindTool could not be retrieved");
      return sc;
    }

    // Retrieve IncidentSvc 
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name()); 
    sc = incidentSvc.retrieve(); 
    if(sc.isFailure()) { 
      ATH_MSG_FATAL("IncidentSvc not found.");
      return StatusCode::FAILURE; 
    } 
    // BeginEvent incident is registered 
    incidentSvc->addListener(this, IncidentType::BeginEvent);

    return sc;
  }

  StatusCode TgcCoinHierarchySvc::finalize(void) {
    ATH_MSG_DEBUG("finalize()");
 
    // Delete memories 
    prepareForNewEvent();

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchySvc::execute(void) {
    ATH_MSG_DEBUG("execute()");

    if(!m_newEvent) {
      // This event is already processed and TgcCoinHierarchy's are already found 
      ATH_MSG_DEBUG("This is not a new event.");
      return StatusCode::SUCCESS; 
    }

    // Delete memories 
    prepareForNewEvent();

    // Get TgcCoinHierarchyTriggerSector's
    std::vector<TgcCoinHierarchyTriggerSector*> trigSectorCombVector;
    StatusCode sc = m_classifyTool->classify(&trigSectorCombVector);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("ITgcCoinHierarchyClassifyTool" << " classify() failed");
      return sc;
    }

    // Loop over all the TgcCoinHierarchyTriggerSector's
    ATH_MSG_DEBUG("trigSectorCombVector size : " << trigSectorCombVector.size());
    while(!trigSectorCombVector.empty()) {
      // Get TgcCoinHierarchy's from each TgcCoinHierarchyTriggerSector 
      std::vector<TgcCoinHierarchy*> hierarchyVector;
      sc = m_findTool->find(trigSectorCombVector.back(), &hierarchyVector);
      if(sc.isFailure()) {
        ATH_MSG_FATAL("ITgcCoinHierarchyFindTool->find() failed.");
        return StatusCode::FAILURE;
      }

      if(m_debug) showHierarchyVector(&hierarchyVector);
      
      // Record TgcCoinHierarchy's into m_hierarchyVector from temporary hierarchyVector 
      while(!hierarchyVector.empty()) {
        m_hierarchyVector.push_back(hierarchyVector.back());
        hierarchyVector.pop_back();
      }
      delete trigSectorCombVector.back();
      trigSectorCombVector.pop_back();
    }

    // Make maps from trigger sector to TgcCoinHierarchy's numbers and existence of TgcCoinHierarchy's 
    sc = makeFoundHierarchyTables();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("makeFoundHierarchyTables() failed.");
      return sc;
    }

    // Make maps from Identifier to TgcCoinHierarchy's numbers 
    sc = makeIndexIdentifierToHierarchy();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("makeIndexIdentifierToHierarchy() failed.");
      return sc;
    }

    ATH_MSG_DEBUG("m_hierarchyVector size is " << m_hierarchyVector.size());

    // The search of TgcCoinHierarchy's for this event has been done 
    m_newEvent = false;
    
    return sc;
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const Identifier identify) {
    if(!hierarchies) {
      ATH_MSG_FATAL("Given std::vector<const TgcCoinHierarchy*>* pointer is NULL. getHierarchy failed.");
      return StatusCode::FAILURE;
    }

    // We don't need old information
    hierarchies->clear();

    // If this call is the first time for this event, execute is performed 
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
        ATH_MSG_FATAL("execute() failed in " << 
                      "getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Identifier identify)");
        return sc;
      }
    }

    // Get TgcCoinHierarchy's numbers from Identifier  
    std::vector<unsigned int>* index = getIndexIdentifierToHierarchy(identify);
    if(!index) {
      return StatusCode::SUCCESS;
    }

    // Get pointers from numbers 
    unsigned int size = index->size();
    for(unsigned int i=0; i<size; i++) {
      hierarchies->push_back(m_hierarchyVector.at(index->at(i)));
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const TgcPrepData* hit) {
    if(!hit) {
      ATH_MSG_WARNING("const TgcPrepData* hit is null pointer"); 
      return StatusCode::SUCCESS;
    }
    
    return getHierarchy(hierarchies, hit->identify());
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const MuonCluster* hit) {
    if(!hit) {
      ATH_MSG_WARNING("const MuonCluster* hit is null pointer");
      return StatusCode::SUCCESS;
    }
    const TgcPrepData* tgcHit = dynamic_cast<const TgcPrepData*>(hit);
    if(!tgcHit) {
      ATH_MSG_DEBUG("dynamic_cast<TgcPrepData*> failed");
      return StatusCode::SUCCESS;
    }

    return getHierarchy(hierarchies, tgcHit->identify());
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const Trk::PrepRawData* hit) {
    if(!hit) {
      ATH_MSG_WARNING("const Trk::PrepRawData* hit is null pointer");
      return StatusCode::SUCCESS;
    }
    const TgcPrepData* tgcHit = dynamic_cast<const TgcPrepData*>(hit);
    if(!tgcHit) {
      ATH_MSG_DEBUG("dynamic_cast<TgcPrepData*> failed");
      return StatusCode::SUCCESS;
    }

    return getHierarchy(hierarchies, tgcHit->identify());
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const Trk::RIO_OnTrack* rio) {
    if(!rio) {
      ATH_MSG_WARNING("const Trk::RIO_OnTrack* rio is null pointer");
      return StatusCode::SUCCESS;
    }

    /*
      const TgcPrepData* tgcHit = dynamic_cast<const TgcPrepData*>(rio->prepRawData());
      if(!tgcHit) {
      ATH_MSG_DEBUG("dynamic_cast<TgcPrepData*> failed");
      return StatusCode::SUCCESS;
      }
    */

    return getHierarchy(hierarchies, rio->identify());
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const Trk::Track* track) {
    if(!track) {
      ATH_MSG_WARNING("const Trk::Track* track is null pointer");
      return StatusCode::SUCCESS;
    }

    if(!hierarchies) {
      ATH_MSG_FATAL("Given std::vector<const TgcCoinHierarchy*>* pointer is NULL. getHierarchy failed.");
      return StatusCode::FAILURE;
    }

    // We don't need old information
    hierarchies->clear();    

    // Use only one Trk::RIO_OnTrack or all Trk::RIO_OnTrack's
    if(!m_loopOverContainedROTS) { 
      std::vector<const Trk::RIO_OnTrack*> rioOnTracks;
      // Get the representative Trk::RIO_OnTrack of each chamber  
      Trk::RoT_Extractor::extract(rioOnTracks, track->measurementsOnTrack()->stdcont());
      std::vector<const Trk::RIO_OnTrack*>::const_iterator it   = rioOnTracks.begin();
      std::vector<const Trk::RIO_OnTrack*>::const_iterator it_e = rioOnTracks.end();
      for(; it!=it_e; it++) {
        std::vector<const TgcCoinHierarchy*> tmpHierarchies;
        // Get TgcCoinHierarchy's by an RIO_OnTrack 
        StatusCode sc = getHierarchy(&tmpHierarchies, *it);
        if(sc.isFailure()) {
          ATH_MSG_FATAL("getHierarchy() failed in " << 
                        "getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Trk::Track* track)");
          return sc;
        }
        
        std::vector<const TgcCoinHierarchy*>::const_iterator jt   = tmpHierarchies.begin(); 
        std::vector<const TgcCoinHierarchy*>::const_iterator jt_e = tmpHierarchies.end(); 
        for(; jt!=jt_e; jt++) {
          bool isDuplicated = false;
          std::vector<const TgcCoinHierarchy*>::const_iterator kt   = hierarchies->begin();
          std::vector<const TgcCoinHierarchy*>::const_iterator kt_e = hierarchies->end();
          for(; kt!=kt_e; kt++) { 
            // Duplication is checked by pointer of TgcCoinHierarchy
            // This is OK because both pointers point elements of m_hierarchyVector of TgcCoinHierarchySvc
            if((*kt)==(*jt)) {
              isDuplicated = true;
              break;
            }
          }
          if(!isDuplicated) {
            hierarchies->push_back(*jt);
          }
        }
      }
    } else { // m_loopOverContainedROTS==true
      const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
      if(!trackStates) {
        ATH_MSG_WARNING("track->trackStateOnSurfaces() is null pointer");
        return StatusCode::SUCCESS;
      }

      DataVector<const Trk::TrackStateOnSurface>::const_iterator its   = trackStates->begin() ;
      DataVector<const Trk::TrackStateOnSurface>::const_iterator its_e = trackStates->end() ;
      for(; its != its_e ; ++its) {
        const Trk::MeasurementBase* measurement = (*its)->measurementOnTrack();
        if(!measurement) continue;

        // Loop over all the ContainedROTs. 
        // This is possible if the Track has CompetingRIOsOnTrack's 
	const Trk::CompetingRIOsOnTrack* rotc = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
        if(!rotc) continue;

        unsigned int size = rotc->numberOfContainedROTs();
        for(unsigned int icc=0; icc<size; icc++) {
          std::vector<const TgcCoinHierarchy*> tmpHierarchies;
          // Get TgcCoinHierarchy's by an Identifier 
          StatusCode sc = getHierarchy(&tmpHierarchies, rotc->rioOnTrack(icc).identify());
          if(sc.isFailure()) {
            ATH_MSG_FATAL("getHierarchy() failed in " << 
                          "getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Trk::Track* track)");
            return sc;
          }

          ATH_MSG_DEBUG("Hierarchy size associated to RIO is " << tmpHierarchies.size()); 
          std::vector<const TgcCoinHierarchy*>::const_iterator jt   = tmpHierarchies.begin();
          std::vector<const TgcCoinHierarchy*>::const_iterator jt_e = tmpHierarchies.end();
          for(; jt!=jt_e; jt++) {
            std::vector<const TgcCoinHierarchy*>::const_iterator kt   = hierarchies->begin();
            std::vector<const TgcCoinHierarchy*>::const_iterator kt_e = hierarchies->end();
            bool isDuplicated = false;
            for(; kt!=kt_e; kt++) {
              // Duplication is checked by pointer of TgcCoinHierarchy
              // This is OK because both pointers point elements of m_hierarchyVector of TgcCoinHierarchySvc
              if((*kt)==(*jt)) {
                isDuplicated = true;
                break;
              }
            }
            if(!isDuplicated) {
              hierarchies->push_back(*jt);
            }
          }
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const TgcCoinData* coin) { 
    if(!hierarchies) {
      ATH_MSG_FATAL("Given std::vector<const TgcCoinHierarchy*>* pointer is NULL. getHierarchy failed.");
      return StatusCode::FAILURE;
    }

    // We don't need old information
    hierarchies->clear();

    if(!coin) {
      ATH_MSG_WARNING("const TgcCoinData* coin is null pointer"); 
      return StatusCode::SUCCESS;
    }
    
    // Check that the trigger sector is valid
    unsigned int isAside = static_cast<unsigned int>(coin->isAside());
    unsigned int isForward = static_cast<unsigned int>(coin->isForward());
    unsigned int phi = static_cast<unsigned int>(coin->phi());
    if(phi==0 || phi>=TgcCoinHierarchy::NPHIS) {
      ATH_MSG_WARNING("phi of TgcCoinData is " << phi << ". This should be between 1 to 48.");
      return StatusCode::SUCCESS;
    }

    // If this call is the first time for this event, execute is performed
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
        ATH_MSG_FATAL("execute() failed in " << 
                      "getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Identifier identify)");
        return sc;
      }
    }

    // Check that there is at least one TgcCoinHierarchy in the trigger sector
    if(!m_foundHierarchy[isAside][isForward][phi]) return StatusCode::SUCCESS;

    const TgcCoinData::CoinDataType type = coin->type();
    const bool isStrip = coin->isStrip();
    
    unsigned int sizeHierarchy = m_indexTriggerSectorToHierarchy[isAside][isForward][phi].size();
    for(unsigned int iHierarchy=0; iHierarchy<sizeHierarchy; iHierarchy++) {
      const unsigned int idHierarchy = m_indexTriggerSectorToHierarchy[isAside][isForward][phi].at(iHierarchy);
      const TgcCoinHierarchy* tmpHierarchy = m_hierarchyVector.at(idHierarchy);
      if(!tmpHierarchy) {
        ATH_MSG_FATAL("Null pointer to TgcCoinHierarchy. This should never happen.");
        return StatusCode::FAILURE;
      }

      if(!tmpHierarchy->hasCoincidence(type, isStrip)) continue;

      const TgcCoinData* tmpCoin = tmpHierarchy->getCoincidence(type, isStrip);
      if(!tmpCoin) {
        ATH_MSG_FATAL("const TgcCoinData* coin is null pointer. This should never happen."); 
        return StatusCode::FAILURE;
      }
      
      if(type==TgcCoinData::TYPE_TRACKLET || type==TgcCoinData::TYPE_HIPT) {
        // Check that TgcCoinData's are the same by comparing channelIdIn and channelIdOut for Tracklet and HiPt
        if(coin->channelIdIn()==tmpCoin->channelIdIn() && 
           coin->channelIdOut()==tmpCoin->channelIdOut()) {
          hierarchies->push_back(m_hierarchyVector.at(idHierarchy));
        }
      } else if(type==TgcCoinData::TYPE_SL) {
        // Check that TgcCoinData's are the same by comparing several variables for SL
        if(coin->channelIdOut()==tmpCoin->channelIdOut() &&
           coin->identifyHash()==tmpCoin->identifyHash() &&
           coin->trackletId()==tmpCoin->trackletId() &&
           coin->trackletIdStrip()==tmpCoin->trackletIdStrip() &&
           coin->roi()==tmpCoin->roi() &&
           coin->pt()==tmpCoin->pt() &&
           coin->isPositiveDeltaR()==tmpCoin->isPositiveDeltaR()) {
	  hierarchies->push_back(m_hierarchyVector.at(idHierarchy));
        }
      } 
    }

    return StatusCode::SUCCESS; 
  }

  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, 
                                               const bool isAside, const bool isForward, const int phi) {
    if(!hierarchies) {
      ATH_MSG_FATAL("Given std::vector<const TgcCoinHierarchy*>* pointer is NULL. getHierarchy failed.");
      return StatusCode::FAILURE;
    }

    // We don't need old information
    hierarchies->clear();    

    if(phi<=0 || phi>=TgcCoinHierarchy::NPHIS) {
      ATH_MSG_WARNING("phi of TgcCoinData is " << phi << ". This should be between 1 to 48.");
      return StatusCode::SUCCESS;
    }

    // If this call is the first time for this event, execute is performed
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
        ATH_MSG_FATAL("execute() failed in " << 
                      "getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Identifier identify)");
        return sc;
      }
    }

    const unsigned int uiIsAside = static_cast<unsigned int>(isAside);
    const unsigned int uiIsForward = static_cast<unsigned int>(isForward);
    const unsigned int uiPhi = static_cast<unsigned int>(phi);

    if(!m_foundHierarchy[uiIsAside][uiIsForward][uiPhi]) {
      return StatusCode::SUCCESS;
    } 

    unsigned int sizeHierarchy = m_indexTriggerSectorToHierarchy[uiIsAside][uiIsForward][uiPhi].size();
    for(unsigned int iHierarchy=0; iHierarchy<sizeHierarchy; iHierarchy++) {
      const unsigned int idHierarchy = m_indexTriggerSectorToHierarchy[uiIsAside][uiIsForward][uiPhi].at(iHierarchy);
      hierarchies->push_back(m_hierarchyVector.at(idHierarchy));
    }
    
    return StatusCode::SUCCESS; 
  }

  // This method returs only TgcCoinHierarchies which have Sector Logic.
  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const bool isAside, 
                                               const bool isForward, const int phi, const int roi) {
    if(!hierarchies) {
      ATH_MSG_FATAL("Given std::vector<const TgcCoinHierarchy*>* pointer is NULL. getHierarchy failed.");
      return StatusCode::FAILURE;
    }

    // We don't need old information
    hierarchies->clear();    

    // Get all the TgcCoinHierarchy's in the trigger sector 
    std::vector<const TgcCoinHierarchy*> tmpHierarchies;
    StatusCode sc = getHierarchy(&tmpHierarchies, isAside, isForward, phi);
    if(!sc.isSuccess()) return sc;
    if(!tmpHierarchies.size()) return StatusCode::SUCCESS;

    std::vector<const TgcCoinHierarchy*>::const_iterator jt   = tmpHierarchies.begin();
    std::vector<const TgcCoinHierarchy*>::const_iterator jt_e = tmpHierarchies.end();
    for(; jt!=jt_e; jt++) {
      // Select TgcCoinHierarchy's which have SL's
      if(!(*jt)->hasCoincidence(TgcCoinData::TYPE_SL)) continue;

      const TgcCoinData* tmpCoin = (*jt)->getCoincidence(TgcCoinData::TYPE_SL);
      if(!tmpCoin) {
        ATH_MSG_FATAL("const TgcCoinData* coin is null pointer. This should never happen."); 
        return StatusCode::FAILURE;
      }
      // Compare ROI numbers 
      if(tmpCoin->roi()==roi) {
        hierarchies->push_back(*jt);
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode TgcCoinHierarchySvc::getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies) { 
    if(!hierarchies) {
      ATH_MSG_FATAL("Given std::vector<const TgcCoinHierarchy*>* pointer is NULL. getHierarchy failed.");
      return StatusCode::FAILURE;
    }

    // We don't need old information
    hierarchies->clear();    
    
    // If this call is the first time for this event, execute is performed
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
        ATH_MSG_FATAL("execute() failed in " << 
                      "getHierarchy(std::vector<const TgcCoinHierarchy*>* hierarchies, const Identifier identify)");
        return sc;
      }
    }

    // Copy m_hierarchyVector to hierarchies
    unsigned int sizeHierarchy = m_hierarchyVector.size();
    for(unsigned int iHierarchy=0; iHierarchy<sizeHierarchy; iHierarchy++) {
      hierarchies->push_back(m_hierarchyVector.at(iHierarchy));
    }
    
    return StatusCode::SUCCESS; 
  }

  void TgcCoinHierarchySvc::handle(const Incident& inc) { 
    // When processing for a new run is started, m_newEvent will become true   
    if(inc.type()==IncidentType::BeginEvent) { 
      ATH_MSG_DEBUG("handle() BeginEvent");
    }

    m_newEvent = true;
  }

  StatusCode TgcCoinHierarchySvc::makeIndexIdentifierToHierarchy() {
    // Loop over m_hierarchyVector 
    unsigned int sizeHierarchyVector = m_hierarchyVector.size();
    for(unsigned int iHierarchy=0; iHierarchy<sizeHierarchyVector; iHierarchy++) {
      const TgcCoinHierarchy* hierarchy = m_hierarchyVector.at(iHierarchy);
      // Loop over all wire hits and strip hits
      for(unsigned int iWS=TgcCoinHierarchy::WIRE; iWS<=TgcCoinHierarchy::STRIP; iWS++) {
        bool isStrip = (iWS==TgcCoinHierarchy::STRIP ? true : false);
        // Loop over TGC1, TGC2 and TGC3 stations  
        for(unsigned int iST=TgcCoinHierarchy::TGC1; iST<=TgcCoinHierarchy::TGC3; iST++) {
          TgcCoinHierarchy::STATION station = (iST==TgcCoinHierarchy::TGC1 ? TgcCoinHierarchy::TGC1 : 
					       (iST==TgcCoinHierarchy::TGC2 ? TgcCoinHierarchy::TGC2 : 
						TgcCoinHierarchy::TGC3));
          // Loop over all hits
          unsigned int numHits = hierarchy->numHits(isStrip, station);
          if(!numHits) continue;
          const std::vector<const TgcPrepData*>* hits = hierarchy->getHits(isStrip, station);
          for(unsigned int iHit=0; iHit<numHits; iHit++) {
            Identifier identify = hits->at(iHit)->identify();
            std::map<const Identifier, std::vector<unsigned int>*>::iterator it 
              = m_indexIdentifierToHierarchy[iWS][station].find(identify);
            if(it!=m_indexIdentifierToHierarchy[iWS][station].end()) { 
              // identify is found in m_indexIdentifierToHierarchy
              // identify is already associated to at least one TgcCoinHierarchy 
              std::vector<unsigned int>* ids = (*it).second;
              unsigned int sizeIds = ids->size();
              // Check that TgcCoinHierarchy is duplicated by the TgcCoinHierarchy numbers in m_hierarchyVector
              bool isDuplicated = false;
              for(unsigned int iId=0; iId<sizeIds; iId++) {
                if(ids->at(iId)==iHierarchy) {
                  isDuplicated = true;
                  break;
                }
              }
              // If the TgcCoinHierarchy is new one, 
              // the TgcCoinHierarchy number is added in std::vector<unsigned int> of m_indexIdentifierToHierarchy
              if(!isDuplicated) {
                ids->push_back(iHierarchy);
              }
            } else { // identify is not found in m_indexIdentifierToHierarchy
              // Create a new std::vector<unsigned int> 
              std::vector<unsigned int>* newIds = new std::vector<unsigned int>;
              // The TgcCoinHierarchy number is added in the new std::vector<unsigned int>
              newIds->push_back(iHierarchy);
              // The new std::vector<unsigned int> is added into m_indexIdentifierToHierarchy
              m_indexIdentifierToHierarchy[iWS][station].insert(std::pair<const Identifier, 
                                                                std::vector<unsigned int>*>(identify, newIds));
            }
          }
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  std::vector<unsigned int>* TgcCoinHierarchySvc::getIndexIdentifierToHierarchy(const Identifier identify) const {
    // Get TgcCoinHierarchy::STATION enum from Identifier 
    TgcCoinHierarchy::STATION station = m_classifyTool->getSTATION(identify);
    if(station==TgcCoinHierarchy::NOTSTATION) return 0;

    // Get trigger sector from Identifier
    unsigned int isAside = 0;
    unsigned int isForward = 0;
    unsigned int phi = 0;
    bool isOK = m_classifyTool->getTriggerSector(identify, isAside, isForward, phi);
    if(!isOK) return 0;
    
    // Check that there is at least one TgcCoinHierarchy in the trigger sector
    if(!m_foundHierarchy[isAside][isForward][phi]) return 0;

    // Get isStrip from Identifier
    bool isStrip = m_classifyTool->isStrip(identify);

    // Find the TgcCoinHierarchy numbers associated to Identifier using m_indexIdentifierToHierarchy  
    std::map<const Identifier, std::vector<unsigned int>*>::const_iterator it 
      = m_indexIdentifierToHierarchy[isStrip][station].find(identify);
    if(it!=m_indexIdentifierToHierarchy[isStrip][station].end()) { 
      return (*it).second;
    }

    return 0;
  }

  void TgcCoinHierarchySvc::showHierarchyVector(const std::vector<TgcCoinHierarchy*>* hierarchyVector) const {
    unsigned int sizeHierarchyVector = hierarchyVector->size();
    ATH_MSG_DEBUG("The number of TgcCoinHierarchy's = " << sizeHierarchyVector); 

    // Loop over hierarchyVector 
    for(unsigned int iC=0; iC<sizeHierarchyVector; iC++) {
      TgcCoinHierarchy* hierarchy = hierarchyVector->at(iC);
      // Show the details of the highest coincidence of the TgcCoinHierarchy 
      TgcCoinHierarchy::COINCIDENCE coincidence = hierarchy->highestCoincidence();
      ATH_MSG_DEBUG("COINCINDENCE="     << coincidence << 
                    " isAside="         << (coincidence>=TgcCoinHierarchy::TRACKLETSTRIP ? 
                                            hierarchy->getCoincidence(coincidence)->isAside()         : -1) << 
                    " isForward="       << (coincidence>=TgcCoinHierarchy::TRACKLETSTRIP ? 
                                            hierarchy->getCoincidence(coincidence)->isForward()       : -1) << 
                    " phi="             << (coincidence>=TgcCoinHierarchy::TRACKLETSTRIP ? 
                                            hierarchy->getCoincidence(coincidence)->phi()             : -1) << 
                    " trackletId="      << (coincidence>=TgcCoinHierarchy::TRACKLETSTRIP ? 
                                            hierarchy->getCoincidence(coincidence)->trackletId()      : -1) <<
                    " trackletIdStrip=" << (coincidence==TgcCoinHierarchy::SL            ? 
                                            hierarchy->getCoincidence(coincidence)->trackletIdStrip() : -1)); 

      // For duplicated coincidences
      if(hierarchy->hasAnyDuplicatedCoincidence()) {
        ATH_MSG_DEBUG("TgcCoinHierarchy headed by " << 
                      *(hierarchy->getCoincidence(hierarchyVector->back()->highestCoincidence())) 
                      << " has duplicated : " << 
                      (hierarchy->hasDuplicatedCoincidence(TgcCoinData::TYPE_HIPT,     false) ? "HiPt Wire" : "") << 
                      (hierarchy->hasDuplicatedCoincidence(TgcCoinData::TYPE_HIPT,     true)  ? "HiPt Strip" : "") << 
                      (hierarchy->hasDuplicatedCoincidence(TgcCoinData::TYPE_TRACKLET, false) ? "Traclet Wire" : "") << 
                      (hierarchy->hasDuplicatedCoincidence(TgcCoinData::TYPE_TRACKLET, true)  ? "Traclet Strip" : ""));
      }
    }
  }

  void TgcCoinHierarchySvc::prepareForNewEvent() {
    // Delete TgcCoinHierarchy's in m_hierarchyVector
    while(!m_hierarchyVector.empty()) {
      delete m_hierarchyVector.back();
      m_hierarchyVector.pop_back();
    }

    // Reset m_foundHierarchy and m_indexTriggerSectorToHierarchy maps 
    for(int iSIDE=0; iSIDE<TgcCoinHierarchy::NSIDES; iSIDE++) {
      for(int iREGION=0; iREGION<TgcCoinHierarchy::NREGIONS; iREGION++) {
        for(int iPHI=0; iPHI<TgcCoinHierarchy::NPHIS; iPHI++) {
          m_foundHierarchy[iSIDE][iREGION][iPHI] = false;
          m_indexTriggerSectorToHierarchy[iSIDE][iREGION][iPHI].clear();
        }
      }
    }

    // Reset m_indexIdentifierToHierarchy maps
    for(unsigned int iWS=TgcCoinHierarchy::WIRE; iWS<=TgcCoinHierarchy::STRIP; iWS++) {
      for(unsigned int iST=TgcCoinHierarchy::TGC1; iST<=TgcCoinHierarchy::TGC3; iST++) {
        std::map<const Identifier, std::vector<unsigned int>*>::iterator it   
          = m_indexIdentifierToHierarchy[iWS][iST].begin();
        std::map<const Identifier, std::vector<unsigned int>*>::iterator it_e 
          = m_indexIdentifierToHierarchy[iWS][iST].end();
        for(; it!=it_e; it++) delete ((*it).second);
        m_indexIdentifierToHierarchy[iWS][iST].clear();
      }
    }
  }

  StatusCode TgcCoinHierarchySvc::makeFoundHierarchyTables() {
    // Make maps from trigger sector to TgcCoinHierarchy's numbers and existence of TgcCoinHierarchy's 
    unsigned int sizeHierarchyVector = m_hierarchyVector.size();
    for(unsigned int iHierarchy=0; iHierarchy<sizeHierarchyVector; iHierarchy++) {
      // Retrieve TgcCoinHierarchy from m_hierarchyVector
      const TgcCoinHierarchy* hierarchy = m_hierarchyVector.at(iHierarchy);
      if(!hierarchy) {
        ATH_MSG_FATAL("Null pointer to TgcCoinHierarchy. This should never happen.");
        return StatusCode::FAILURE;
      }
      // The highest coincidence will be used for retrieving the trigger sector of the TgcCoinHierarchy
      const TgcCoinData* coinData = hierarchy->getCoincidence(hierarchy->highestCoincidence());
      if(!coinData) {
        ATH_MSG_FATAL("Null pointer to highest TgcCoinData. This should never happen.");
        return StatusCode::FAILURE;
      }
      
      int phi = coinData->phi();
      if(phi<=0 || phi>=TgcCoinHierarchy::NPHIS) continue;
      unsigned int isAside = static_cast<unsigned int>(coinData->isAside());
      unsigned int isForward = static_cast<unsigned int>(coinData->isForward());
      // The trigger sector has at least one TgcCoinHierarchy
      m_foundHierarchy[isAside][isForward][phi] = true;
      // Push back the TgcCoinHierarchy
      m_indexTriggerSectorToHierarchy[isAside][isForward][phi].push_back(iHierarchy);
    }

    return StatusCode::SUCCESS;
  }

} // end of namespace Trigger
