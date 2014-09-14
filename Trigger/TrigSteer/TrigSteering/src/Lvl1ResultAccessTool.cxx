/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl1ResultAccessTool
 *
 * @brief
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Lvl1ResultAccessTool.cxx,v 1.45 2009-05-11 13:56:23 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Lvl1ResultAccessTool.h"

#include "TrigT1Result/RoIBResult.h"

#include "TrigT1Interfaces/TriggerTypeWord.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/RoIFormatUpdater.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "TrigSteeringEvent/Lvl1Result.h"

#include "TrigInterfaces/AlgoConfig.h"

using namespace HLT;
using namespace std;
using namespace TrigConf;

Lvl1ResultAccessTool::Lvl1ResultAccessTool(const std::string& name, const std::string& type,
                                           const IInterface* parent) :
   AlgTool(name, type, parent),
   m_typeConverter { new LVL1::JEPRoIDecoder() },
   m_lvl1ConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_store("StoreGateSvc", name)
{
   declareProperty( "LVL1ConfigSvc", m_lvl1ConfigSvc, "LVL1 Config Service");
   declareProperty( "ignorePrescales", m_ignorePrescales=false, "Allows to set prescales but ignore them");
   declareProperty( "muonCommissioningStep", m_muonCommissioningStep=0, "Set thresholods creation policty. 0 means default, i.e. thresholds fully inclusive");

   declareInterface<HLT::ILvl1ResultAccessTool>( this );
}

Lvl1ResultAccessTool::~Lvl1ResultAccessTool() {
   clearLvl1Items();
   clearDecisionItems();
   delete m_typeConverter; 
}


void Lvl1ResultAccessTool::clearDecisionItems() {
   for (LVL1CTP::Lvl1Item* item : m_decisionItems)
      delete item;
   m_decisionItems.clear();
}

void Lvl1ResultAccessTool::clearLvl1Items() {
   for( LVL1CTP::Lvl1Item* item : m_lvl1ItemConfig)
      delete item;
   m_lvl1ItemConfig.clear();
}


StatusCode Lvl1ResultAccessTool::initialize() {
   m_log = new MsgStream( msgSvc(), name() );
   m_logLvl = m_log->level();

   // Get LVL1 Config Svc handle:
   if (m_lvl1ConfigSvc.empty()) {
      (*m_log) << MSG::FATAL << "no TrigConfigSvc set in the jobOptions-> abort" << endreq;
      return StatusCode::FAILURE;
   }
   (*m_log) << MSG::DEBUG << "retrieving TrigConfigSvc." << endreq;
   if ( m_lvl1ConfigSvc.retrieve().isFailure() ) {
      (*m_log) << MSG::ERROR << "Failed to retreive LvlConverter Tool: " << m_lvl1ConfigSvc << endreq;
      return StatusCode::FAILURE;
   } else {
      (*m_log) << MSG::INFO << "Retrieved " << m_lvl1ConfigSvc << endreq;
   }

   if(m_ignorePrescales)
      (*m_log) << MSG::INFO << "Prescales will be ignored." << endreq; 

   return StatusCode::SUCCESS;
}


StatusCode Lvl1ResultAccessTool::finalize() {
   delete m_log; m_log = 0;
   return StatusCode::SUCCESS;
}


StatusCode Lvl1ResultAccessTool::updateItemsConfigOnly() {

   // Get CTP and Threshold configurations from the LVL1ConfigSvc
   const TrigConf::CTPConfig* ctpConfig = m_lvl1ConfigSvc->ctpConfig();

   // =============================================================
   // Prepare LVL1 items, i.e. name -> hashID, put everything into
   // a vector where the position corresponds to the LVL1 item
   // position in the CTP result!
   // =============================================================


   clearLvl1Items();

   const vector<int> & prescales = ctpConfig->prescaleSet().prescales();

   for(const TrigConf::TriggerItem * item : ctpConfig->menu().items() ) {

      unsigned int pos = item->ctpId();

      //std::cout << "LVL1 item: name=" << (*itemIt)->name() << " CTP id=" << pos << std::endl;

      // make sure, vector is big enough
      //    while(m_lvl1ItemConfig.size() <= pos) m_lvl1ItemConfig.push_back(0);
      if ( m_lvl1ItemConfig.size() <= pos )
         m_lvl1ItemConfig.resize(pos+1);

      if (m_lvl1ItemConfig[pos] != 0 && item->name() != m_lvl1ItemConfig[pos]->name()  ) {
         (*m_log) << MSG::ERROR << "LVL1 item: " << item->name() << " uses a CTP id: "
                  << pos << " that is already used!! --> ignore this LVL1 item! " << endreq;
      } else {
         m_lvl1ItemConfig[pos] =  new LVL1CTP::Lvl1Item( item->name(),
                                                         TrigConf::HLTUtils::string2hash(item->name()),
                                                         0, 0, 0, prescales[pos]);
      }
   }

   // some debug output
   (*m_log) << MSG::DEBUG << "CTP -> Configured LVL1 items:" << endreq;

   //std::cout << "m_decisionItems.size() = " << m_decisionItems.size() << " m_lvl1ItemConfig.size() = " << m_lvl1ItemConfig.size() << std::endl;

   int xtra = m_decisionItems.size() - m_lvl1ItemConfig.size();

   if (xtra > 0)
      for (unsigned int i = 0; i < (unsigned int)xtra; ++i) {
         delete m_decisionItems.back();
         m_decisionItems.pop_back();
      }

   for (unsigned int i = 0; i < m_lvl1ItemConfig.size(); ++i) {

      // make sure we have a non-zero object:
      if (!m_lvl1ItemConfig[i]) continue;

      if (i < m_decisionItems.size()) {
         m_decisionItems[i]->setName(m_lvl1ItemConfig[i]->name());
         m_decisionItems[i]->setHashId(m_lvl1ItemConfig[i]->hashId());
         m_decisionItems[i]->setPrescaleFactor(m_lvl1ItemConfig[i]->prescaleFactor());
      }

      (*m_log) << MSG::DEBUG << "name " << m_lvl1ItemConfig[i]->name()
               << " (and hashID " << m_lvl1ItemConfig[i]->hashId() << ") position " << i << endreq;
   }

   return StatusCode::SUCCESS;
}


StatusCode Lvl1ResultAccessTool::updateConfig( bool useL1Muon, bool useL1Calo,
                                               bool useL1JetEnergy)
{
  StatusCode sc = updateItemsConfigOnly();
  if (!sc.isSuccess()) return sc;

  const TrigConf::ThresholdConfig* thresholdConfig = m_lvl1ConfigSvc->thresholdConfig();

  // =============================================================
  // Prepare LVL1 RoI thresholds:
  // - for a fast execution, we save the threholds names (hashIDs)
  //   here
  // - matching these hashIDs with HLT sequence TriggerElements,
  //   we can flag the thresholds that are used in HLT in order to
  //   save time in the execute: create only those TriggerElements
  // =============================================================

  // ------------------------------
  //            Muon RoIs
  // ------------------------------

  if (useL1Muon) {
     (*m_log) << MSG::DEBUG << "Going through configured LVL1 MUON thresholds ... " << endreq;
     const std::vector<TrigConf::TriggerThreshold*>& muonThresholdConfig = thresholdConfig->getThresholdVector(L1DataDef::MUON);

     m_muonCfg.clear();
     m_useL1Muon = false; // care about muons in execute only if at least one muon TriggerElement is used in HLT !

     // make sure, the vector is filled up to a size of MAX MUON NUMBER (should be 6):
     while(m_muonCfg.size() <= 6)
        m_muonCfg.push_back( ConfigThreshold() ); // default entries are not active !

     for (std::vector<TrigConf::TriggerThreshold*>::const_iterator muonTT = muonThresholdConfig.begin();
          muonTT != muonThresholdConfig.end(); ++muonTT) {

        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1 MUON threshold: "
                 << (*muonTT)->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( (*muonTT)->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash((*muonTT)->name());
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<<(*muonTT)->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }
        int tn = (*muonTT)->thresholdNumber()+1;
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else if (tn > 6)  {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is greater 6"
                    << ", go on w/o this threshold!" <<endreq;
        } else{
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << endreq;
           m_muonCfg.at(tn) =  ConfigThreshold((*muonTT)->name(), teId, activeHLT) ;
           m_useL1Muon = true;
        }
     }


  }

  // ------------------------------
  //          EMTau RoIs
  // ------------------------------
  if (useL1Calo) {

     (*m_log) << MSG::DEBUG << "Going through configured LVL1 CALO thresholds ... " << endreq;

     m_useL1Calo = false; // care about calo RoIs in execute only if at least one calo TriggerElement is used in HLT !
     m_emCfg.clear();
     m_tauCfg.clear();
     m_emtauThresholds.clear();
     
     for( L1DataDef::TriggerType caloType : vector<L1DataDef::TriggerType>{ L1DataDef::EM, L1DataDef::TAU} ) {

        for (TriggerThreshold * clusterTT : thresholdConfig->getThresholdVector( caloType ) ) {

           if (clusterTT == nullptr) continue;
           
           // Needed to find thresholds passed by Run 2 RoI
           m_emtauThresholds.push_back(clusterTT);
           
           (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1  EM/TAU threshold: " << clusterTT->name();

           unsigned int teId = 0;
           bool activeHLT = TrigConf::HLTTriggerElement::getId( clusterTT->name().c_str(), teId);
           teId = TrigConf::HLTUtils::string2hash( clusterTT->name() );
           if (activeHLT) {
              (*m_log) << MSG::DEBUG << " => OK (found TE)"<<endreq;
              (*m_log) << MSG::DEBUG << " => TE=(name="<< clusterTT->name() << ", hashID="<<teId << ")"<<endreq;
           } else {
              (*m_log) << MSG::DEBUG << " => NO" << endreq;
           }

           int tn = clusterTT->thresholdNumber();
           if (tn < 0) {
              (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                       << ", go on w/o this threshold!" <<endreq;
           } else {
              uint32_t ibit= 0x1 << tn;
              (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                       << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
              if (caloType == L1DataDef::EM)
                 m_emCfg.push_back( ConfigThreshold( clusterTT->name(), teId, activeHLT, ibit ));
              else if (caloType == L1DataDef::TAU)
                 m_tauCfg.push_back( ConfigThreshold( clusterTT->name(), teId, activeHLT, ibit ));
              m_useL1Calo = true;
           }

        }

     }
  }



  // ------------------------------
  //         JetEnergy RoIs
  // ------------------------------
  if (useL1JetEnergy) {

     (*m_log) << MSG::DEBUG << "Going through configured LVL1 JetEnergy thresholds ... " << endreq;

     m_useL1JetEnergy = false;// care about jetEnergy RoIs in execute only if at least one jetEnergy TriggerElement is used in HLT !
     m_jetCfg.clear();
     
     // In case we need to calculate thresholds passed: copy jet thresholds into vector RecJetRoI can use
     m_jetThresholds = thresholdConfig->getJetThresholdVector();
     std::vector<TrigConf::TriggerThreshold*> fwdJetThresholds = thresholdConfig->getFJetThresholdVector();
     for (std::vector<TrigConf::TriggerThreshold*>::iterator it = fwdJetThresholds.begin(); it != fwdJetThresholds.end(); ++it) m_jetThresholds.push_back((*it));

     // Jet threholds:
     for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getJetThresholdVector() ) {

        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1 JET threshold: "
                 << thr->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash( thr->name() );
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<< thr->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }

        int tn = thr->thresholdNumber();
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else {
           uint32_t ibit= 0x1 << tn; // 8 bits in Run 1, up to 25 in Run 2
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                    << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
           m_jetCfg.push_back( ConfigJetEThreshold( thr->name(), teId, activeHLT, ibit, JetRoI) );
           m_useL1JetEnergy = true;
        }

     }


     // Forward Jet threholds (only Run 1):
     for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getFJetThresholdVector() ) {

        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1  ForwardJET (FJET) threshold: "
                 << thr->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash(thr->name());
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<<thr->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }
        int tn = thr->thresholdNumber();
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else {
           uint32_t ibit= 0x1;
           ibit <<= 8; // first 8 bits are for jet thresholds (this code will never be reached in Run 2)
           ibit <<= tn; // move to threshold number (4 bits)
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                    << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
           m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT,  ibit, ForwardJetRoI) );
           m_useL1JetEnergy = true;
           //ibit <<= 1;
        }
        
     }


     //    ibit=1;
     // Total Energy threholds:
     for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getTotEtVector() ) {

        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1  TotalEt (ET) threshold: "
                 << thr->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash(thr->name());
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<<thr->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }
        int tn = thr->thresholdNumber();
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else {
           uint32_t ibit= 0x1;
           ibit <<= 16; // first 15 bits are EnergyY, 16th bit is overflow
           ibit <<= tn; // 8 bits here (change at beginning of 2011)
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                    << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
           m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT, ibit, TotalEtRoI) );
           m_useL1JetEnergy = true;
        }
     }


     // Jet Energy threholds:
     for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getJetEtVector() ) {
        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1 JetEt (JE) threshold: "
                 << thr->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash(thr->name());
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<<thr->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }
        int tn = thr->thresholdNumber();
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else {
           uint32_t ibit= 0x1 << tn; // 4 bits here !
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                    << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
           m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT,ibit, JetEtRoI) );
           m_useL1JetEnergy = true;
        }
     }

     //    ibit=1;
     // Missing Energy threholds:
     for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getMissEtVector() ) {

        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1  MissingEt (XE) threshold: "
                 << thr->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash(thr->name());
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<<thr->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }
        int tn = thr->thresholdNumber();
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else {
           uint32_t ibit= 0x1;
           ibit <<= 16; // first 15 bits are EnergyY, 16th bit is overflow
           ibit <<= tn; // 8 bits here
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                    << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
           m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT, ibit, MissingEtRoI) );
           m_useL1JetEnergy = true;
           //      ibit <<= 1;
        }
     }


     for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getMissEtSignVector() ) {

        (*m_log) << MSG::DEBUG << "Searching TE that matches LVL1  MissingEtSignificance (XS) threshold: "
                 << thr->name();

        unsigned int teId = 0;
        bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
        teId = TrigConf::HLTUtils::string2hash(thr->name()); 
        if (activeHLT) {
           (*m_log) << MSG::DEBUG << " => OK (found TE)" << endreq;
           (*m_log) << MSG::DEBUG << " => TE=(name="<<thr->name() << ", hashID="<<teId << ")"<<endreq;
        } else {
           (*m_log) << MSG::DEBUG << " => NO" << endreq;
        }
        int tn = thr->thresholdNumber();
        if (tn < 0) {
           (*m_log) << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
                    << ", go on w/o this threshold!" <<endreq;
        } else {
           uint32_t ibit= 0x1;
           ibit <<= 16; // first 15 bits are EnergyY, 16th bit is overflow
           ibit <<= tn; // 8 bits here
           (*m_log) << MSG::DEBUG << "thresholdNumber = " << tn << " .. bit position = 0x"
                    << std::hex << std::setw( 8 ) << std::setfill( '0' ) << (ibit) << std::dec <<  endreq;
           m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT, ibit, METSignificanceRoI) );
           m_useL1JetEnergy = true;
        }
     }
  } // end of Jet/Energy part
  //  std::cout << "7" << std::endl;

  (*m_log) << MSG::DEBUG << " Initialize done" << endreq;
  return StatusCode::SUCCESS;
}



StatusCode Lvl1ResultAccessTool::updateResult(const ROIB::RoIBResult& result,
                                              bool updateCaloRoIs)
{
  // set L1 items:
  createL1Items(result);

  createMuonThresholds(result);
  createEMTauThresholds(result, updateCaloRoIs);
  createJetEnergyThresholds(result,  updateCaloRoIs);

  return StatusCode::SUCCESS;
}


StatusCode Lvl1ResultAccessTool::updateResult(const LVL1CTP::Lvl1Result& result)
{
  clearDecisionItems();

  // set L1 items:
  for (unsigned int i = 0; i < result.nItems(); ++i)
    //    m_decisionItems.push_back(new LVL1CTP::Lvl1Item("", 0, i,
    m_decisionItems.push_back(new LVL1CTP::Lvl1Item("", 0,
                                                    result.isPassedBeforePrescale(i),
                                                    result.isPassedAfterPrescale(i),
                                                    result.isPassedAfterVeto(i) ));

  return StatusCode::SUCCESS;
}

// check for calo calibration bits  
// get the calibration triggers, 253, 254 and 255 ie, bits 
// 29, 30 and 31 from word 7				  
bool Lvl1ResultAccessTool::isCalibrationEvent(const ROIB::RoIBResult& result) {
  std::vector<ROIB::CTPRoI> ctpRoIVec(result.cTPResult().TAV());
  if ( ctpRoIVec.size() >= 8 && (ctpRoIVec[7].roIWord() &  0xE0000000) )
    return true;
  else
    return false;
}

bool getBit(const std::vector<ROIB::CTPRoI>& words, unsigned position) {
  unsigned w  = position/32;  
  unsigned b  = position%32;  
  if ( words.size() > w) {
    uint32_t roIWord = words[w].roIWord();
    return (roIWord >> b) & 0x1;
  } 
  return false;
}

// ==============================
//       create LVL1 items
// ==============================
const std::vector< const LVL1CTP::Lvl1Item* >& Lvl1ResultAccessTool::createL1Items( const ROIB::RoIBResult& result,
                                                                                    bool makeLvl1Result ) {

  m_items.clear();
  std::vector<ROIB::CTPRoI> bitsBP = result.cTPResult().TBP();
  std::vector<ROIB::CTPRoI> bitsAP = result.cTPResult().TAP();
  std::vector<ROIB::CTPRoI> bitsAV = result.cTPResult().TAV();


  bool calib_flag = isCalibrationEvent(result);
  if(m_logLvl <= MSG::DEBUG) {
    (*m_log) << MSG::DEBUG << "Calibration event? " << calib_flag  << endreq;
  }  
  
  
  
  // loop over all configured items if no calibration, else only 3 last
  unsigned first_item = calib_flag ? 253: 0;  
  
  for ( unsigned i = first_item; i < m_lvl1ItemConfig.size(); i++ ) {
    if ( !m_lvl1ItemConfig[ i ] ) continue; // empty slot

    LVL1CTP::Lvl1Item* item =  m_lvl1ItemConfig[ i ];


    *item = LVL1CTP::Lvl1Item( item->name(), item->hashId(), 
			       getBit(bitsBP, i), 
			       getBit(bitsAP, i), 
			       getBit(bitsAV, i), 
			       item->prescaleFactor() );  
    m_items.push_back(item);
  }



  // Fill TBP, TAP in case we're creating the Lvl1Result
  if (makeLvl1Result) {
    m_lvl1Result = new LVL1CTP::Lvl1Result(true);
    // 1.) TAV
    const std::vector<ROIB::CTPRoI> ctpRoIVecAV = result.cTPResult().TAV();
    for (unsigned int iWord = 0; iWord < ctpRoIVecAV.size(); ++iWord) {
      uint32_t roIWord = ctpRoIVecAV[iWord].roIWord();
      m_lvl1Result->itemsAfterVeto().push_back(roIWord);

      if(m_logLvl <= MSG::DEBUG) {
	(*m_log) << MSG::DEBUG << "TAV word #" << iWord << " is 0x" << MSG::hex
		 << std::setw( 8 ) << std::setfill( '0' ) << roIWord << MSG::dec <<  endreq;
      }

    }

    // 1.) TBP
    const std::vector<ROIB::CTPRoI> ctpRoIVecBP = result.cTPResult().TBP();
    for (unsigned int iWord=0; iWord < ctpRoIVecBP.size(); ++iWord) {
      uint32_t roIWord = ctpRoIVecBP[iWord].roIWord();
      m_lvl1Result->itemsBeforePrescale().push_back(roIWord);

      if(m_logLvl <= MSG::DEBUG) {
	(*m_log) << MSG::DEBUG << "TBP word #" << iWord << " is 0x" << MSG::hex
		 << std::setw( 8 ) << std::setfill( '0' ) << roIWord << MSG::dec <<  endreq;
      }
    }

    // 2.) TAP
    const std::vector<ROIB::CTPRoI> ctpRoIVecAP = result.cTPResult().TAP();
    for (unsigned int iWord=0; iWord < ctpRoIVecAP.size(); ++iWord) {
      uint32_t roIWord = ctpRoIVecAP[iWord].roIWord();
      m_lvl1Result->itemsAfterPrescale().push_back(roIWord);

      if(m_logLvl <= MSG::DEBUG) {
	(*m_log) << MSG::DEBUG << "TAP word #" << iWord << " is 0x" << MSG::hex
		 << std::setw( 8 ) << std::setfill( '0' ) << roIWord << MSG::dec <<  endreq;
      }
    }
    // make sure TBP, TAP, TAV all have 8 entries !
    while (m_lvl1Result->itemsBeforePrescale().size() < 8) m_lvl1Result->itemsBeforePrescale().push_back(0);
    while (m_lvl1Result->itemsAfterPrescale().size() < 8) m_lvl1Result->itemsAfterPrescale().push_back(0);
    while (m_lvl1Result->itemsAfterVeto().size() < 8) m_lvl1Result->itemsAfterVeto().push_back(0);
    m_store->record(m_lvl1Result, "Lvl1Result").ignore();

  }

  
  return m_items;
}

// ==============================
//        create Muon RoIs
// ==============================
const std::vector< MuonRoI>& Lvl1ResultAccessTool::createMuonThresholds(const ROIB::RoIBResult& result)
{
   m_muonRoIs.clear();

   if (m_useL1Muon) {

      const std::vector<ROIB::MuCTPIRoI>& muonRoIVec = result.muCTPIResult().roIVec();

      // some Debug output
      if(m_logLvl <= MSG::DEBUG) {
         (*m_log) << MSG::DEBUG << "Looking for Muon RoIs"
                  << endreq;
         (*m_log) << MSG::DEBUG << "---------------------"
                  << endreq;
      }

      for (std::vector<ROIB::MuCTPIRoI>::const_iterator itMuon  = muonRoIVec.begin();
           itMuon != muonRoIVec.end(); ++itMuon) {

         MuonRoI roi(*itMuon);
         uint32_t roIWord = itMuon->roIWord();

         if(m_logLvl <= MSG::DEBUG) {
            (*m_log) << MSG::DEBUG << "RoI word: 0x" << MSG::hex
                     << std::setw(8) << roIWord  << MSG::dec << endreq;
         }

         // the Threshold Number (1 to 6)  associated with this Muon RoI
         unsigned int thresholdNumber = itMuon->pt();

         // this should never happen ... but who knows
         if (thresholdNumber > 6 || thresholdNumber < 1) {
            (*m_log) << MSG::WARNING << "Muon threshold number is not between 1 and 6 but ="
                     << thresholdNumber << " !!! Go on with a value of 1" << endreq;
            thresholdNumber = 1;
         }


         bool tgc = (itMuon->getSectorLocation() != MuCTPI_RDO::BARREL);

         std::vector<int> thToFire;
         thToFire.reserve(6);


         if ( m_muonCommissioningStep == 0 ) {
            // corresponding to "Menu 1E31" 
            for ( unsigned int th = 1; th <= thresholdNumber; ++th )
               thToFire.push_back(th);
         }  else if ( m_muonCommissioningStep == 1 ) { 

            // corresponding to "Step 1." in the table //
	
            switch (thresholdNumber) {
	  
            case 1:  // (5) , 1
               thToFire.push_back(1);
               if ( tgc ) 
                  thToFire.push_back(5);
               break; 
            case 2:  // (5) , 1 , 2
               thToFire.push_back(1); thToFire.push_back(2);
               if ( tgc ) 
                  thToFire.push_back(5);
               break;
            case 3:  // (5) , 1 , 2 , 3
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3);
               if ( tgc ) 
                  thToFire.push_back(5);

               break;
            case 4:  // 1 , 2 , 3 , 4
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3); thToFire.push_back(4);
               break;
            case 5:  // 5
               thToFire.push_back(5);
               break;
            case 6:  // 1, 2, 3, (4 or 5) , 6
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3); thToFire.push_back(6);
               if ( tgc )
                  thToFire.push_back(5);
               else
                  thToFire.push_back(4);
               break;
            default:
               break;
            }
         } else if ( m_muonCommissioningStep == 2 || m_muonCommissioningStep == 3 ) { 	
            // corresponds to "Step.2 & Step.3" commonly //	  
            switch (thresholdNumber) {
            case 1:  // 1
               thToFire.push_back(1);
               break;
            case 2:  // 1 , 2
               thToFire.push_back(1); thToFire.push_back(2);
               break;
            case 3:  // 1 , 2 , 3
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3);
               break;
            case 4:  // 1 , 2 , 3 , 4
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3); thToFire.push_back(4);
               break;
            case 5:  // 1 , 2 , 3 , 5
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3); thToFire.push_back(5);
               break;
            case 6:  // 1 , 2 , 3 , (4 or 5) , 6
               thToFire.push_back(1); thToFire.push_back(2); thToFire.push_back(3); thToFire.push_back(6);
               if ( tgc )
                  thToFire.push_back(5);
               else
                  thToFire.push_back(4);
               break;
            default:
               break;
            }
         }
      
      
      
         // produce all TEs that have an equal or lower threshold number AND are needed by HLT
         for (std::vector<int>::const_iterator th = thToFire.begin(); th != thToFire.end(); ++th) {
	
            roi.addThreshold( & m_muonCfg[*th] );
	
            if(m_logLvl <= MSG::DEBUG)
               (*m_log) << MSG::DEBUG << "found Muon Threshold " << m_muonCfg[*th].name
                        << " threshold number: " << *th << endreq;
         }
      
         m_muonRoIs.push_back(roi);
      }    
   }  
   return m_muonRoIs;
}



// ==============================
//          EMTau RoIs
// ==============================
const std::vector< EMTauRoI >& Lvl1ResultAccessTool::createEMTauThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs)
{
  m_emTauRoIs.clear();

  if (m_useL1Calo) {

    // reconstruct RoIs
    if(m_logLvl <= MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "Looking for EMTau RoIs" << endreq;
      (*m_log) << MSG::DEBUG << "----------------------"
	       << endreq;
    }

    const std::vector<ROIB::EMTauResult>& emTauResult = result.eMTauResult();
    std::vector<ROIB::EMTauRoI> emTauRoIVec;

    for (std::vector<ROIB::EMTauResult>::const_iterator itResult  = emTauResult.begin();
	 itResult != emTauResult.end(); ++itResult) {

      const std::vector<ROIB::EMTauRoI>& roIVec = itResult->roIVec();

      for (std::vector<ROIB::EMTauRoI>::const_iterator itEMTau  = roIVec.begin();
	   itEMTau != roIVec.end(); ++itEMTau) {

	uint32_t roIWord = itEMTau->roIWord();

	// Fix for change of RoI format
	if (updateCaloRoIs) roIWord = LVL1::RoIFormatUpdater::UpdateEMTauRoI(roIWord);
	HLT::EMTauRoI roi = HLT::EMTauRoI( ROIB::EMTauRoI(roIWord) );
        
        // Set threshold bits in a way that work for Run 1 or Run 2
        LVL1::RecEmTauRoI recRoI( roIWord, &m_emtauThresholds );
        roi.setThresholdMask(recRoI.thresholdPattern());

	if(m_logLvl <= MSG::DEBUG) {
	  (*m_log) << MSG::DEBUG << "RoI word: 0x" << MSG::hex
		   << std::setw(8) << roIWord << ", threshold pattern " << roi.thresholdMask() << MSG::dec <<  endreq;
	}

        if (recRoI.roiType() == LVL1::TrigT1CaloDefs::EMRoIWordType || recRoI.roiType() == LVL1::TrigT1CaloDefs::CpRoIWordType) {
            for ( std::vector< ConfigThreshold >::const_iterator threshold = m_emCfg.begin();
                  threshold != m_emCfg.end(); ++threshold) {

               // check whether this LVL1 threshold is used by HLT:
               if ( !(threshold->mask & roi.thresholdMask()) ) continue;

               roi.addThreshold( &(*threshold) );

               if (m_logLvl <= MSG::DEBUG)
                  (*m_log) << MSG::DEBUG << "creating EM Theshold: "  << threshold->name << endreq;
            }
        }
        if (recRoI.roiType() == LVL1::TrigT1CaloDefs::TauRoIWordType || recRoI.roiType() == LVL1::TrigT1CaloDefs::CpRoIWordType) {
            for ( std::vector< ConfigThreshold >::const_iterator threshold = m_tauCfg.begin();
                  threshold != m_tauCfg.end(); ++threshold) {

               // check whether this LVL1 threshold is used by HLT:
               if ( !(threshold->mask & roi.thresholdMask()) ) continue;

               roi.addThreshold( &(*threshold) );

               if (m_logLvl <= MSG::DEBUG)
                  (*m_log) << MSG::DEBUG << "creating TAU Theshold: "  << threshold->name << endreq;
            }
        }
	m_emTauRoIs.push_back(roi);
      }
    }
  }
  return m_emTauRoIs;
}



// ==============================
//         JetEnergy RoIs
// ==============================

const std::vector< JetEnergyRoI >& Lvl1ResultAccessTool::createJetEnergyThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs)
{
  m_jetRoIs.clear();

  if (m_useL1JetEnergy) {

    // reconstruct RoIs
    if(m_logLvl <= MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "Looking for JetEnergy RoIs" << endreq;
      (*m_log) << MSG::DEBUG << "--------------------------" << endreq;
    }


    const std::vector<ROIB::JetEnergyResult>& jetEnergyResult = result.jetEnergyResult();

    for (std::vector<ROIB::JetEnergyResult>::const_iterator itResult  = jetEnergyResult.begin();
	 itResult != jetEnergyResult.end(); ++itResult) {
      
      const std::vector<ROIB::JetEnergyRoI>& roIVec = itResult->roIVec();
      if (m_logLvl <= MSG::DEBUG) {
	(*m_log) << MSG::DEBUG << "#RoI words " << roIVec.size() << endreq;
      }
      for (std::vector<ROIB::JetEnergyRoI>::const_iterator itJetEn = roIVec.begin();
	   itJetEn != roIVec.end(); ++itJetEn) {

	uint32_t roIWord = itJetEn->roIWord();
	// Fix for change of RoI format - long obsolete
	//if (updateCaloRoIs) roIWord = LVL1::RoIFormatUpdater::UpdateJetEnergyRoI( roIWord );
        
        /// RoI Type
	LVL1::TrigT1CaloDefs::RoIType roiType = m_typeConverter->roiType(roIWord);

        /// Create RoI
	HLT::JetEnergyRoI roi = HLT::JetEnergyRoI( ROIB::JetEnergyRoI(roIWord) );

	// create a second roi container, since the 3 roiWords contain all SumET & all MissingET thresholds!
	// use roi for SumET  and  roi2 for MissingET
	HLT::JetEnergyRoI roiET   = HLT::JetEnergyRoI( ROIB::JetEnergyRoI(roIWord) );


	if(m_logLvl <= MSG::DEBUG) {
	  (*m_log) << MSG::DEBUG << "RoI word: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' )
		   << roIWord << std::dec << endreq;
	}

	// roiWords 2 and 3 for the SumET & MissingET RoIs
	uint32_t roIWord1 = 0, roIWord2 = 0;

	/*
	std::cout << "RoI Type = " << roiType << " where JetRoIWordType="
		  <<  LVL1::TrigT1CaloDefs::JetRoIWordType << ", JetEtRoIWordType="
		  <<  LVL1::TrigT1CaloDefs::JetEtRoIWordType << ", EnergyRoIWordType0="
		  <<  LVL1::TrigT1CaloDefs::EnergyRoIWordType0 << std::endl;
	*/

	// check whether this roIWord is of type Energy RoI => get the next two words as well
	if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0  && itJetEn->etSumType() == 0) {
	//if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 ) {
          // Energy RoI consists of 3 RoI words !
          ++itJetEn;
	  if (itJetEn == roIVec.end()) break; 

	  roIWord1 = itJetEn->roIWord();
          // Fix for change of RoI format
          //if (updateCaloRoIs) roIWord1 = LVL1::RoIFormatUpdater::UpdateJetEnergyRoI( roIWord1 );

          ++itJetEn;
 	  if (itJetEn == roIVec.end()) break; 

	  roIWord2 = itJetEn->roIWord();

          // Fix for change of RoI format
          //if (updateCaloRoIs) roIWord2 = LVL1::RoIFormatUpdater::UpdateJetEnergyRoI( roIWord2 );

	  if ( !roiET.setExtraWords(roIWord, roIWord1, roIWord2)) {
	    (*m_log) << MSG::ERROR << "Error setting Total/Missing ET RoI extra words" << endreq;
	    return m_jetRoIs;
	  }

	  if(m_logLvl <= MSG::DEBUG) {
            (*m_log) << MSG::DEBUG << "RoI triplet = 0x" <<  std::hex << std::setw( 8 ) << std::setfill( '0' )
		     << roIWord << " 0x" << roIWord1 << " 0x" << roIWord2 << std::dec << endreq;
          }
	}

        // For Jet RoIs, fill thresholdMask word, used later for setting Configured Thresholds
        // Do it here so that it only needs to be done once/RoI

        else if ( roiType == LVL1::TrigT1CaloDefs::JetRoIWordType ) {        
           LVL1::RecJetRoI recRoI( roIWord, &m_jetThresholds );
           roi.setThresholdMask(recRoI.thresholdPattern());
        }
 
	// Loop over possible Jet thresholds
	for (std::vector< ConfigJetEThreshold >::const_iterator threshold = m_jetCfg.begin();
	     threshold != m_jetCfg.end(); ++threshold) {

	  if ( roiType == LVL1::TrigT1CaloDefs::JetRoIWordType &&
	       (threshold->type == JetRoI || threshold->type == ForwardJetRoI) ) {
	    if(m_logLvl <= MSG::DEBUG) 
	      (*m_log) << MSG::DEBUG << threshold->name << " is of type JetRoI or ForwardJetRoI" << endreq;

	    // plain Jet & ForwardJet RoI
	    //---------------------
	    if ( ! addJetThreshold(roi, &*threshold) ) {
	      if(m_logLvl <= MSG::WARNING) {
		(*m_log) << MSG::WARNING << "Problem while adding threshold: " << threshold->name << " returning prematurely" << endreq;
	      }
	      return  m_jetRoIs;
	    }
	  } else if ( roiType == LVL1::TrigT1CaloDefs::JetEtRoIWordType &&
		      threshold->type == JetEtRoI ) {
	    if(m_logLvl <= MSG::DEBUG) 
	      (*m_log) << MSG::DEBUG << threshold->name << " is of type JetEtRoI " << endreq;

	    // JetEt ET RoI (total transv. energy from jets)
	    //----------------------------------------------
	    if ( ! addJetThreshold(roi, &*threshold) ) {
	      if(m_logLvl <= MSG::WARNING) {
		(*m_log) << MSG::WARNING << "Problem while adding threshold: " << threshold->name << " returning prematurely" << endreq;
	      }
	      return m_jetRoIs;
	    }
	    
	  } else if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 &&
		      (threshold->type == TotalEtRoI || threshold->type == MissingEtRoI || threshold->type == METSignificanceRoI)) {
	    if(m_logLvl <= MSG::DEBUG) 
	      (*m_log) << MSG::DEBUG << threshold->name << " is of type TotalEtRoI or MissingEtRoI or METSignificanceRoI" << endreq;
	    // EnergySum (totEt, missEt) RoI
	    // -----------------------------
	    //std::cout << " in EnergySum RoI ... " << std::endl;
	    roiET.setType(MissingOrTotalEtRoI);
	    if ( ! addMetThreshold(roiET, &*threshold) ) {
	      if(m_logLvl <= MSG::WARNING) {
		(*m_log) << MSG::WARNING << "Problem while adding threshold: " << threshold->name << " returning prematurely" << endreq;
	      }
	      return m_jetRoIs;
	    }
	  } // end of Energy part (totEt, missEt)
	  
	  if (itJetEn == roIVec.end()) break;
	} // end of loop over possible thresholds
	
	if (roi.size()!=0 ) m_jetRoIs.push_back(roi);
	
	//this rois have type set to unknown in all normal jet RoIs
	// add TotalEt and MissingEt rois even if no threshold has fired
	if (roiET.type() != Unknown) {
	  if (m_logLvl <= MSG::DEBUG)
	    (*m_log) << MSG::DEBUG << "adding MissingOrTotalET RoI with: " <<  roiET.size() << " threshods" << endreq;	
	  m_jetRoIs.push_back(roiET);
	}
      } // end of loop over RoIs
    } // end of jetEnergy Result
  }// enf of m_useL1JetEnergy
  if (m_logLvl <= MSG::DEBUG)
    (*m_log) << MSG::DEBUG << "done with jet thresholds" << endreq;	
  return m_jetRoIs;
}


bool Lvl1ResultAccessTool::addJetThreshold(HLT::JetEnergyRoI & roi, const ConfigJetEThreshold* threshold ) {


  if ( !(threshold->mask & roi.thresholdMask()) ) return true;
  
  if (!roi.setType(threshold->type)) {
    (*m_log) << MSG::ERROR << "Inconsistent threshold types " << roi.type()
	     << " " << threshold->type << endreq;
    return false;
  }
  roi.addThreshold( threshold );
  if (m_logLvl <= MSG::DEBUG)
    (*m_log) << MSG::DEBUG << "creating Jet  threshold: " << threshold->name << endreq;
  return true;
  
}


// this is made separate function as it usesses different word for threhold bits (namely word1/2)
bool Lvl1ResultAccessTool::addMetThreshold( HLT::JetEnergyRoI & roi, const ConfigJetEThreshold* threshold ) {

  if ( threshold->type == METSignificanceRoI )
    if ( !(threshold->mask & roi.word0()) ) return true;

  if ( threshold->type == TotalEtRoI )
    if ( !(threshold->mask & roi.word1()) ) return true;

  if ( threshold->type == MissingEtRoI )
    if ( !(threshold->mask & roi.word2()) ) return true;

  if (!roi.setType(threshold->type)) {
    (*m_log) << MSG::ERROR << "Inconsistent threshold types " << roi.type()
             << " " << threshold->type << endreq;
    return false;
  }
  roi.addThreshold( threshold );
  if (m_logLvl <= MSG::DEBUG)
    (*m_log) << MSG::DEBUG << "creating Met threshold: " << threshold->name << endreq;
  return true;
}

StatusCode Lvl1ResultAccessTool::queryInterface( const InterfaceID& riid,
						 void** ppvIf )
{
  if ( riid == ILvl1ResultAccessTool::interfaceID() )  {
    *ppvIf = (ILvl1ResultAccessTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }

  return AlgTool::queryInterface( riid, ppvIf );
}
