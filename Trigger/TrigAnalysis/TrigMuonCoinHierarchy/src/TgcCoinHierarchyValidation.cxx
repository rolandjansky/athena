/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyValidation.h"

#include "TrigMuonCoinHierarchy/ITgcCoinHierarchySvc.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchyClassifyTool.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "muonEvent/MuonContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include <iomanip>
#include <string>
#include <climits>

#include <EventInfo/EventInfo.h>
#include <EventInfo/EventID.h>


using Muon::TgcCoinData;
using Muon::TgcCoinDataCollection;
using Muon::TgcCoinDataContainer;
using Muon::TgcPrepData;
using Muon::TgcPrepDataCollection;
using Muon::TgcPrepDataContainer;

namespace Trigger { 
  TgcCoinHierarchyValidation::TgcCoinHierarchyValidation(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
     ,m_tgcCoinSvc(0)
     ,m_tgcCabling(0)
     ,m_timingName("UNKNOWN")
     ,m_bc(UINT_MAX)
     ,m_ofs(0)
  {
    declareProperty("timing",m_timingName="CURR");
    declareProperty("doCoin",m_doCoin=true);
    declareProperty("doHit" ,m_doHit=true);
    declareProperty("logName",m_logName);
  }

  StatusCode TgcCoinHierarchyValidation::initialize() {   
    ATH_MSG_INFO("initialize()");

    // check timing
    if     (m_timingName.compare("PRIOR")==0)m_bc = 0;
    else if(m_timingName.compare("CURR" )==0)m_bc = 1;
    else if(m_timingName.compare("NEXT" )==0)m_bc = 2;
    else                                     m_bc = UINT_MAX;
    std::cout << __FILE__ << " set timing:" << m_timingName << ":" << m_bc << std::endl;

    if(m_bc!=0 and m_bc!=1 and m_bc!=2){
      ATH_MSG_FATAL("Invalid Timing is set." << m_bc << ":" << m_timingName);
      return StatusCode::FAILURE;
    }

    // Retrieve Trigger::TgcCoinHierarchySvc 
    StatusCode sc = service("Trigger::TgcCoinHierarchySvc", m_tgcCoinSvc);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Could not find TgcCoinHierarchySvc");
      return sc;
    }

    // Prepare hit analysis //
    ATH_CHECK( m_idHelperSvc.retrieve() );

    // Prepare log maker //
    if(!m_logName.empty()){
      m_ofs = new std::ofstream();
      m_ofs->open(m_logName.c_str());
      if(!*m_ofs){
        ATH_MSG_FATAL("Could not open log file " << m_logName.c_str());
        return StatusCode::FAILURE;
      }
      makeLog("Start\n");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyValidation::execute() {   
    ATH_MSG_DEBUG("execute()");

    if(m_doCoin){
      if(!doAnalysis()){
        ATH_MSG_FATAL("Something problem is happend in doAnalysis()");
        return StatusCode::FAILURE;
      }
    }

    if(m_doHit){
      if(!doAnalysisHit()){
        ATH_MSG_FATAL("Something problem is happend in doAnalysisHit()");
        return StatusCode::FAILURE;
      }
    }


    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyValidation::finalize() {   
    ATH_MSG_INFO("finalize()");
    if(!m_logName.empty()){
      makeLog("Finish\n");
      m_ofs->close();
      delete m_ofs;
    }
    return StatusCode::SUCCESS;
  }

  bool TgcCoinHierarchyValidation::doAnalysis(){
    std::vector<const TgcCoinHierarchy*> vh;
    StatusCode sc;
    sc = m_tgcCoinSvc->getHierarchy(&vh);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("getHierarchy failed in doAnalysis().");
      return false;
    }

    const EventInfo *p_evt = 0;
    sc = evtStore()->retrieve(p_evt);
    int runNumber = p_evt->event_ID()->run_number();
    int lumiBlock = p_evt->event_ID()->lumi_block();
    int eventNumber = p_evt->event_ID()->event_number();

    for(unsigned int ii=0;ii<vh.size();ii++){
      if(vh.at(ii)->uintToTiming(m_bc)!=vh.at(ii)->getTimingCoin())continue;
      unsigned int trgFlag = 0;
      char message[127];
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_TRACKLET,false)) trgFlag |= 0x01;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_TRACKLET,true )) trgFlag |= 0x02;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_HIPT,false)) trgFlag |= 0x04;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_HIPT,true )) trgFlag |= 0x08;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_SL)) trgFlag |= 0x10;

     sprintf(message
             ,"run#:%d,LB:%d,event#:%d,BC:%d,trg:%x\n"
             ,runNumber
             ,lumiBlock
             ,eventNumber
             ,m_bc
             ,trgFlag
             );
      makeLog(message);

      if((trgFlag&0x1)!=0){
        sprintf(message
               ,"trkw:isA=%d,isF=%d,phi=%d,trkId=%d,delta=%d\n"
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,false)->isAside()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,false)->isForward()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,false)->phi()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,false)->trackletId()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,false)->delta()
               );
        makeLog(message);
      }
      if((trgFlag&0x2)!=0){
        sprintf(message
               ,"trks:isA=%d,isF=%d,phi=%d,trkId=%d,delta=%d\n"
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,true)->isAside()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,true)->isForward()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,true)->phi()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,true)->trackletId()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_TRACKLET,true)->delta()
               );
        makeLog(message);
      }
      if((trgFlag&0x4)!=0){
        sprintf(message
               ,"hptw:isA=%d,isF=%d,phi=%d,trkId=%d,delta=%d\n"
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,false)->isAside()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,false)->isForward()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,false)->phi()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,false)->trackletId()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,false)->delta()
               );
        makeLog(message);
      }
      if((trgFlag&0x8)!=0){
        sprintf(message
               ,"hpts:isA=%d,isF=%d,phi=%d,trkId=%d,delta=%d\n"
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,true)->isAside()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,true)->isForward()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,true)->phi()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,true)->trackletId()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_HIPT,true)->delta()
               );
        makeLog(message);
      }
      if((trgFlag&0x10)!=0){
        sprintf(message
               ,"SL  :isA=%d,isF=%d,phi=%d,roi=%d,pt=%d\n"
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_SL)->isAside()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_SL)->isForward()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_SL)->phi()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_SL)->roi()
               ,vh.at(ii)->getCoincidence(TgcCoinData::TYPE_SL)->pt()
               );
        makeLog(message);
      }

    }

    return true;
  }

  bool TgcCoinHierarchyValidation::doAnalysisHit(){
    std::vector<const TgcCoinHierarchy*> vh;
    std::vector<const TgcPrepData*> vp;
    StatusCode sc;
    sc = m_tgcCoinSvc->getHierarchy(&vh);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("getHierarchy failed in doAnalysis().");
      return false;
    }

    /* TGCcablingSvc */
    const ITGCcablingServerSvc *TgcCabGet = 0;
    sc = service("TGCcablingServerSvc",TgcCabGet,true);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("TgcCablingServiceSvc failed in doAnalysisHit().");
      return false;
    }

    sc = TgcCabGet->giveCabling(m_tgcCabling);
    if(!sc.isSuccess()){
      ATH_MSG_FATAL("TgcCabGet is failed in doAnalysisHit().");
      return false;
    }

    /* Hits */
    unsigned int numHits = 0;

    /* Event Info */
    const EventInfo *p_evt = 0;
    sc = evtStore()->retrieve(p_evt);
    int runNumber = p_evt->event_ID()->run_number();
    int lumiBlock = p_evt->event_ID()->lumi_block();
    int eventNumber = p_evt->event_ID()->event_number();

    for(unsigned int ii=0;ii<vh.size();ii++){
      if(vh.at(ii)->uintToTiming(m_bc)!=vh.at(ii)->getTimingCoin())continue;
      unsigned int trgFlag = 0;
      char message[255];

      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_TRACKLET,false)) trgFlag |= 0x01;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_TRACKLET,true )) trgFlag |= 0x02;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_HIPT,false)) trgFlag |= 0x04;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_HIPT,true )) trgFlag |= 0x08;
      if(vh.at(ii)->hasCoincidence(TgcCoinData::TYPE_SL)) trgFlag |= 0x10;

      if(trgFlag==0) continue;

      sprintf(message
             ,"HIT :: run#:%d,LB:%d,event#:%d,BC:%d,trg:%x\n"
             ,runNumber
             ,lumiBlock
             ,eventNumber
             ,m_bc
             ,trgFlag
             );
      makeLog(message);

      for(unsigned int iST=TgcCoinHierarchy::TGC1; iST<=TgcCoinHierarchy::TGC3; iST++){
        bool isStrip = false;
        TgcCoinHierarchy::STATION station = (iST==TgcCoinHierarchy::TGC1 ? TgcCoinHierarchy::TGC1 :
                                            (iST==TgcCoinHierarchy::TGC2 ? TgcCoinHierarchy::TGC2 :
                                                                           TgcCoinHierarchy::TGC3));
        // wire part //
        numHits = vh.at(ii)->numHits(isStrip,station);
        if(numHits!=0 and (trgFlag&0x03)!=0){
          const std::vector<const Muon::TgcPrepData*> *vp = vh.at(ii)->getHits(isStrip,station);
          for(unsigned int hh = 0;hh < vp->size();hh++){
            Identifier id = vp->at(hh)->identify();
            unsigned int timing = vh.at(ii)->uintToTiming(vh.at(ii)->getTimingHit());
            int sta = getSTATION(id);
            int eta = getStEta(id);
            int phi = getStPhi(id);
            int gap = getGasgap(id);
            int channel  = getChannel(id);
            char isS[7];
            if(getIsStrip(id))sprintf(isS,"strip");
            else              sprintf(isS,"wire");
            sprintf(message
                   ,"entry:%u,timing=%u,%s,station=%u,stationName=%d,steta=%d,stphi=%d,gasgap=%d,channel=%d\n"
                   ,hh
                   ,timing
                   ,isS
                   ,iST
                   ,sta
                   ,eta
                   ,phi
                   ,gap
                   ,channel
                   );
            makeLog(message);

          }

        }
      }
    }

    return true;

  }

  bool TgcCoinHierarchyValidation::makeLog(const char *msg){
    if(m_logName.empty()) return false;
    *m_ofs << msg;
    return true;
  }

  int TgcCoinHierarchyValidation::getSTATION(const Identifier id){
    return  m_idHelperSvc->tgcIdHelper().stationName(id);
  }

  int TgcCoinHierarchyValidation::getStEta(const Identifier id){
    return m_idHelperSvc->tgcIdHelper().stationEta(id);
  }

  int TgcCoinHierarchyValidation::getStPhi(const Identifier id){
    return m_idHelperSvc->tgcIdHelper().stationPhi(id);
  }

  int TgcCoinHierarchyValidation::getGasgap(const Identifier id){
    return m_idHelperSvc->tgcIdHelper().gasGap(id);
  }

  int TgcCoinHierarchyValidation::getChannel(const Identifier id){
    return m_idHelperSvc->tgcIdHelper().channel(id);
  }

  int TgcCoinHierarchyValidation::getIsStrip(const Identifier id){
    return m_idHelperSvc->tgcIdHelper().isStrip(id);
  }



} // end of namespace Trigger
