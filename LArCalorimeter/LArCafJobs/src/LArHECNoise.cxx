/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArID_Exception.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"

#include "LArIdentifier/LArOnlineID.h"

#include "LArRecEvent/LArEventBitInfo.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
 
#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "StoreGate/DataHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "xAODEventInfo/EventInfo.h"

#include "CaloEvent/CaloCellContainer.h"

#include "LArCafJobs/LArHECNoise.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

using namespace std;
using xAOD::EventInfo;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor


LArHECNoise::LArHECNoise(const std::string& name,
			 ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_thistSvc(0),
    m_tree(0),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_LArOnlineIDHelper(0),
    m_caloIdMgr(0),
    m_calodetdescrmgr(0),
    m_calocell_id(nullptr),
    m_nt_run(0),
    m_nt_evtId(0),
    m_nt_evtCount(0),
    m_nt_evtTime(0),
    m_nt_evtTime_ns(0),
    m_nt_lb(0),
    m_nt_bcid(0),
    m_nt_gain(0),
    m_nt_side(0),
    m_nt_samp(0),
    m_nt_reg(0),
    m_nt_ieta(0),
    m_nt_iphi(0),
    m_nt_quality(0),
    m_nt_digi(),
    m_nt_max(0),
    m_nt_min(0),
    m_nt_OID(0),
    m_nt_avgMu(0),
    m_nt_actMu(0),
    m_nt_e(0),
    m_nt_t(0),
    m_nt_eta(0),
    m_nt_phi(0),
    m_nt_z(0),
    m_nt_r(0),
    m_nt_ped(0),
    m_nt_pedRMS(0)
 {

   // Trigger
   declareProperty( "TrigDecisionTool", m_trigDec );
   
   /** switches to control the analysis through job options */
   
   declareProperty("TriggerLines",m_TriggerLines={"L1_J5", "L1_J10", "L1_J12", "L1_J30", "L1_TAU5", "L1_TAU8", "L1_J5_EMPTY", "L1_J10_EMPTY", "L1_J12_EMPTY", "L1_J30_EMPTY", "L1_TAU5_EMPTY", "L1_TAU8_EMPTY", "L1_J5_FIRSTEMPTY", "L1_J10_FIRSTEMPTY", "L1_J12_FIRSTEMPTY", "L1_J30_FIRSTEMPTY", "L1_TAU5_FIRSTEMPTY", "L1_TAU8_FIRSTEMPTY"});

   declareProperty("MinDigitADC",m_MinDigitADC=20);
   declareProperty("MaxDeltaT",m_MaxDeltaT=5);

    m_nt_prescale = new float(m_TriggerLines.size());
    m_nt_trigger = new bool(m_TriggerLines.size());
 }


LArHECNoise::~LArHECNoise() {}


StatusCode LArHECNoise::initialize() {

  ATH_MSG_DEBUG ( "Initializing LArHECNoise" );
 
  // Trigger Decision Tool
  ATH_CHECK(m_trigDec.retrieve());
  
  ATH_CHECK( m_cablingKey.initialize() );

  // Retrieve online ID helper
  const DataHandle<LArOnlineID> LArOnlineIDHelper;
  ATH_CHECK( detStore()->retrieve(LArOnlineIDHelper, "LArOnlineID") );
  m_LArOnlineIDHelper = LArOnlineIDHelper;
  ATH_MSG_DEBUG( " Found LArOnline Helper");

  // Retrieve ID helpers
  ATH_CHECK( detStore()->retrieve(m_caloIdMgr) );
  m_calocell_id  = m_caloIdMgr->getCaloCell_ID();

  m_calodetdescrmgr = CaloDetDescrManager::instance();
  /** get a handle on the NTuple and histogramming service */
  ATH_CHECK( service("THistSvc", m_thistSvc) );
 
  /** Prepare TTree **/
  m_tree = new TTree( "HECNoise", "HECNoise" );

  // General properties of events
  m_tree->Branch("iRun",&m_nt_run,"iRun/I");// Event ID
  m_tree->Branch("iEvent",&m_nt_evtId,"iEvent/I");// Event ID
  m_tree->Branch("iEventCount",&m_nt_evtCount,"iEventCount/I");
  m_tree->Branch("iTime",&m_nt_evtTime,"iTime/I");// Event time
  m_tree->Branch("iTime_ns",&m_nt_evtTime_ns,"iTime_ns/I");// Event time in nanosecond
  m_tree->Branch("iLB",&m_nt_lb,"iLB/I"); // LB
  m_tree->Branch("iBCID",&m_nt_bcid,"iBCID/I"); // BCID
  // prescale and trigger here
  //const std::vector<float> &tp = m_nt_prescale;
  //const std::vector<bool> &tt = m_nt_trigger;
  for(unsigned i=0; i<m_TriggerLines.size(); ++i) {
     //m_tree->Branch((m_TriggerLines[i]+"_Prescale").c_str(),&(tp[i]),(m_TriggerLines[i]+"_Prescale/F").c_str());
     //m_tree->Branch((m_TriggerLines[i]+"_Trigger").c_str(),tt[i],(m_TriggerLines[i]+"_Trigger/O").c_str());
     m_tree->Branch((m_TriggerLines[i]+"_Prescale").c_str(),&(m_nt_prescale[i]),(m_TriggerLines[i]+"_Prescale/F").c_str());
     m_tree->Branch((m_TriggerLines[i]+"_Trigger").c_str(),&(m_nt_trigger[i]),(m_TriggerLines[i]+"_Trigger/I").c_str());
  }
  m_tree->Branch("avgMu",&m_nt_avgMu,"avgMu/F"); 
  m_tree->Branch("actMu",&m_nt_actMu,"actMu/F"); 
  m_tree->Branch("iGain",&m_nt_gain,"iGain/I"); 
  m_tree->Branch("iOID",&m_nt_OID,"iOID/L"); 
  m_tree->Branch("iSide",&m_nt_side,"iSide/I"); 
  m_tree->Branch("iSamp",&m_nt_samp,"iSamp/I"); 
  m_tree->Branch("iReg",&m_nt_reg,"iReg/I"); 
  m_tree->Branch("iEta",&m_nt_ieta,"iEta/I"); 
  m_tree->Branch("iPhi",&m_nt_iphi,"iPhi/I"); 
  m_tree->Branch("iQuality",&m_nt_quality,"iQuality/I"); 
  m_tree->Branch("e",&m_nt_e,"e/F"); 
  m_tree->Branch("t",&m_nt_t,"t/F"); 
  m_tree->Branch("eta",&m_nt_eta,"eta/F"); 
  m_tree->Branch("phi",&m_nt_phi,"phi/F"); 
  m_tree->Branch("z",&m_nt_z,"z/F"); 
  m_tree->Branch("r",&m_nt_r,"r/F"); 
  m_tree->Branch("Ped",&m_nt_ped,"Ped/F"); 
  m_tree->Branch("PedRMS",&m_nt_pedRMS,"PedRMS/F"); 
  m_tree->Branch("iDigi",m_nt_digi,"iDigi[32]/S"); 
  m_tree->Branch("iMax",&m_nt_max,"iMax/I"); 
  m_tree->Branch("iMin",&m_nt_min,"iMin/I"); 

  std::string treeName =  "/HEC/HECNoise" ;
  ATH_CHECK( m_thistSvc->regTree(treeName, m_tree) );


  ATH_MSG_DEBUG ( "End of Initializing LArHECNoise" );
 
  return StatusCode::SUCCESS;
}

StatusCode LArHECNoise::finalize() {
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}


StatusCode LArHECNoise::execute() {

  ATH_MSG_DEBUG ( "in execute()" );

  for(unsigned i=0; i<m_TriggerLines.size(); ++i){
         m_nt_prescale[i] = m_trigDec->getPrescale(m_TriggerLines[i]);
         if (m_trigDec->isPassed(m_TriggerLines[i])){
                   m_nt_trigger[i] = true;
         } else {
                  m_nt_trigger[i] = false;
         }
  }

  const EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  m_nt_evtCount = 0;
  const CaloCellContainer* cc = 0;
  const LArRawChannelContainer* lraw = 0;
  if (evtStore()->contains<CaloCellContainer>("AllCalo")) {
     ATH_CHECK(evtStore()->retrieve(cc, "AllCalo"));
  } else if (evtStore()->contains<LArRawChannelContainer>("LArRawChannels")){
     ATH_CHECK(evtStore()->retrieve(lraw, "LArRawChannels"));
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  // retrieve pedestals
  ATH_CHECK( detStore()->retrieve(m_ped,"Pedestal") );

  const LArDigitContainer* ld = 0;
  if (evtStore()->contains<LArDigitContainer>("LArDigitContainer")) {
      ATH_CHECK(evtStore()->retrieve(ld, "LArDigitContainer"));
  } else if (evtStore()->contains<LArDigitContainer>("LArDigitContainer_Thinned")) {
      ATH_CHECK(evtStore()->retrieve(ld, "LArDigitContainer_Thinned"));
  } else if (evtStore()->contains<LArDigitContainer>("FREE")) {
      ATH_CHECK(evtStore()->retrieve(ld, "FREE"));
  } else {
        msg(MSG::WARNING) << "Neither LArDigitContainer nor LArDigitContainer_Thinned nor FREE present, not filling anything "<<endmsg;
        return StatusCode::SUCCESS;
  }
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = ld->begin(); 
  LArDigitContainer::const_iterator itDig_e= ld->end(); 
  const LArDigit* pLArDigit;

   for ( ; itDig!=itDig_e;++itDig) {
           pLArDigit = *itDig;
           HWIdentifier hid = pLArDigit->hardwareID();
        if(! m_LArOnlineIDHelper->isHECchannel(hid)) continue; //and ld.gain() == 2:
        int sigmax=0;
        int sigmin=0;
        int imax=0;
        int imin=0;
        short samp0= pLArDigit->samples()[0];
        for(unsigned i=0; i<(pLArDigit->samples()).size(); ++i){
              short samp=pLArDigit->samples()[i];
              if( i < 32) m_nt_digi[i] = samp;
              if(samp-samp0 > sigmax) {
                 sigmax = samp-samp0;
                 imax=i;
              }
              if( samp-samp0 < sigmin) {
                 sigmin = samp-samp0;
                 imin=i;
              } 
        }
        if( sigmax > m_MinDigitADC && sigmin < -m_MinDigitADC && ( (imin-imax) < m_MaxDeltaT || imin < imax)) {
              m_nt_run        = eventInfo->runNumber();
              m_nt_evtId      = eventInfo->eventNumber();
              m_nt_evtTime    = eventInfo->timeStamp();
              m_nt_evtTime_ns = eventInfo->timeStampNSOffset();
              m_nt_lb         = eventInfo->lumiBlock();
              m_nt_bcid       = eventInfo->bcid();
              m_nt_avgMu      = eventInfo->averageInteractionsPerCrossing();
              m_nt_actMu      = eventInfo->actualInteractionsPerCrossing();

              m_nt_evtCount += 1;
              m_nt_gain = pLArDigit->gain();
              Identifier oid = cabling->cnvToIdentifier(hid);
              m_nt_OID = pLArDigit->channelID().get_compact();
              m_nt_ped = m_ped->pedestal(pLArDigit->channelID(),pLArDigit->gain());
              m_nt_pedRMS = m_ped->pedestalRMS(pLArDigit->channelID(),pLArDigit->gain());
              m_nt_side = m_calocell_id->pos_neg(oid);
              m_nt_samp = m_calocell_id->sampling(oid);
              m_nt_reg = m_calocell_id->region(oid);
              m_nt_ieta = m_calocell_id->eta(oid);
              m_nt_iphi = m_calocell_id->phi(oid);
              m_nt_max = imax;
              m_nt_min = imin;
              IdentifierHash ihash = m_calocell_id->calo_cell_hash(oid);
              m_nt_e = 0.0;
              m_nt_t = 0.0;
              m_nt_quality = 0;
              if(cc) {
                 const CaloCell *rcell = cc->findCell(ihash);
                 if(rcell->ID() != oid) {
                     msg(MSG::WARNING) <<"Cell iHash does not match ..."<<endmsg;
                 }else{
                     m_nt_e = rcell->e();
                     m_nt_t = rcell->time();
                     m_nt_quality = rcell->quality();
                 } 
              } else if(lraw) {
                 for(unsigned l=0; l<lraw->size(); ++l) {
                    if((*lraw)[l].identify() == hid) {
                       m_nt_e = (*lraw)[l].energy();
                       m_nt_t = (*lraw)[l].time()*1e-3;
                       m_nt_quality = (*lraw)[l].quality();
                       break;
                    }
                 }
              }
              const CaloDetDescrElement *cdde =  m_calodetdescrmgr->get_element(oid);
              m_nt_eta = cdde->eta();
              m_nt_phi = cdde->phi();
              m_nt_z = cdde->z();
              m_nt_r = cdde->r();
              m_tree->Fill();
        }//found our digit
   }//over digits
 
  return StatusCode::SUCCESS;
}

