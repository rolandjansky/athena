/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionMaker/Lvl1ItemsAndRoIs.h"
#include "Lvl1ResultAccessTool.h"

#include "TrigT1Result/RoIBResult.h"
#include "CTPfragment/CTPdataformatVersion.h"

#include "TrigT1Interfaces/TriggerTypeWord.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
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
#include "TrigConfData/L1Menu.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "TrigSteeringEvent/Lvl1Result.h"

using namespace std;
using namespace TrigConf;

HLT::Lvl1ResultAccessTool::Lvl1ResultAccessTool(const std::string& name, const std::string& type,
                                                const IInterface* parent) :
   base_class(name, type, parent),
   m_jepDecoder { new LVL1::JEPRoIDecoder() },
   m_cpDecoder { new LVL1::CPRoIDecoder() }  
{
   declareProperty( "ignorePrescales", m_ignorePrescales=false, "Allows to set prescales but ignore them");
   declareProperty( "muonCommissioningStep", m_muonCommissioningStep=0, "Set thresholods creation policty. 0 means default, i.e. thresholds fully inclusive");
}

HLT::Lvl1ResultAccessTool::~Lvl1ResultAccessTool() {
   clearDecisionItems();
   delete m_jepDecoder;
   delete m_cpDecoder;
}


void
HLT::Lvl1ResultAccessTool::clearDecisionItems() {
   for (LVL1CTP::Lvl1Item* item : m_decisionItems)
      delete item;
   m_decisionItems.clear();
}

StatusCode
HLT::Lvl1ResultAccessTool::initialize() {
   // Get LVL1 Config Svc handle:
   if(m_lvl1ConfigSvc.empty()) {
      ATH_MSG_FATAL("no TrigConfigSvc set in the jobOptions-> abort");
      return StatusCode::FAILURE;
   }

   ATH_MSG_INFO("Initializing with UseNewConfig = " << m_useNewConfig);
   ATH_CHECK( m_l1PrescaleSetInputKey.initialize( m_useNewConfig ) );

   ATH_MSG_DEBUG("Retrieving TrigConfigSvc.");
   CHECK( m_lvl1ConfigSvc.retrieve() );

   if(m_ignorePrescales)
      ATH_MSG_INFO("Prescales will be ignored.");

   return StatusCode::SUCCESS;
}

// =============================================================
// Prepare LVL1 items, i.e. name -> hashID, put everything into
// a vector where the position corresponds to the LVL1 item
// position in the CTP result!
// =============================================================
std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >
HLT::Lvl1ResultAccessTool::makeLvl1ItemConfig() const
{
   // vector holding all configured LVL1 items, default initializes to empty unique_ptr
   constexpr size_t numberOfCTPItems = 512; // this is fixed
   std::vector<std::unique_ptr<LVL1CTP::Lvl1Item>> lvl1ItemConfig(numberOfCTPItems);

   if( m_useNewConfig ) {
      ATH_MSG_ERROR("Called makeLvl1ItemConfig without EventContext and UseNewConfig==True. The new configuration requires an EventContext to retrieve the prescales.");
   } else {
      const TrigConf::CTPConfig *ctpConfig = m_lvl1ConfigSvc->ctpConfig();
      const auto & prescales = ctpConfig->prescaleSet().prescales_float();
      for(const TrigConf::TriggerItem * item : ctpConfig->menu().items() ) {
         unsigned int pos = item->ctpId();
         if (lvl1ItemConfig[pos] != nullptr && item->name() != lvl1ItemConfig[pos]->name()  ) {
            ATH_MSG_ERROR( "LVL1 item: " << item->name() << " uses a CTP id: "
                           << pos << " that is already used!! --> ignore this LVL1 item! ");
         } else {
            lvl1ItemConfig[pos] =  std::make_unique<LVL1CTP::Lvl1Item>( item->name(),
                                                                        TrigConf::HLTUtils::string2hash(item->name()),
                                                                        0, 0, 0, prescales[pos]);
         }
      }
   }
   return lvl1ItemConfig;
}

std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >
HLT::Lvl1ResultAccessTool::makeLvl1ItemConfig(const EventContext& context) const
{
   // vector holding all configured LVL1 items, default initializes to empty unique_ptr
   constexpr size_t numberOfCTPItems = 512; // this is fixed
   std::vector<std::unique_ptr<LVL1CTP::Lvl1Item>> lvl1ItemConfig(numberOfCTPItems);

   if( m_useNewConfig ) {
      const TrigConf::L1Menu *l1menu = nullptr;
      if( detStore()->retrieve(l1menu).isFailure() ) {
         ATH_MSG_ERROR("No L1Menu found");
      } else {
         SG::ReadCondHandle<TrigConf::L1PrescalesSet> l1psRH(m_l1PrescaleSetInputKey, context);
         if( !l1psRH.isValid() ) {
            ATH_MSG_ERROR("No valid L1PrescalesSet handle");
         } else {
            const TrigConf::L1PrescalesSet* l1PrescaleSet{*l1psRH};
            if(l1PrescaleSet == nullptr) {
               ATH_MSG_ERROR( "No L1PrescalesSet available");
            } else {
               for(auto & item : *l1menu) {
                  double prescale = l1PrescaleSet->getPrescaleFromCut(l1PrescaleSet->prescale(item.name()).cut);
                  lvl1ItemConfig[item.ctpId()] = std::make_unique<LVL1CTP::Lvl1Item>( item.name(),
                                                                                    TrigConf::HLTUtils::string2hash(item.name()),
                                                                                    0, 0, 0, prescale);
               }
            }
         }
      }
   } else {
      const TrigConf::CTPConfig *ctpConfig = m_lvl1ConfigSvc->ctpConfig();
      const auto & prescales = ctpConfig->prescaleSet().prescales_float();
      for(const TrigConf::TriggerItem * item : ctpConfig->menu().items() ) {
         unsigned int pos = item->ctpId();
         if (lvl1ItemConfig[pos] != nullptr && item->name() != lvl1ItemConfig[pos]->name()  ) {
            ATH_MSG_ERROR( "LVL1 item: " << item->name() << " uses a CTP id: "
                           << pos << " that is already used!! --> ignore this LVL1 item! ");
         } else {
            lvl1ItemConfig[pos] =  std::make_unique<LVL1CTP::Lvl1Item>( item->name(),
                                                                        TrigConf::HLTUtils::string2hash(item->name()),
                                                                        0, 0, 0, prescales[pos]);
         }
      }
   }
   return lvl1ItemConfig;
}


StatusCode
HLT::Lvl1ResultAccessTool::updateItemsConfigOnly() {

   // =============================================================
   // Prepare LVL1 items, i.e. name -> hashID, put everything into
   // a vector where the position corresponds to the LVL1 item
   // position in the CTP result!
   // =============================================================

   m_lvl1ItemConfig = makeLvl1ItemConfig();

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

      ATH_MSG_DEBUG("name " << m_lvl1ItemConfig[i]->name() << " (and hashID " << m_lvl1ItemConfig[i]->hashId() << ") position " << i);
   }

   return StatusCode::SUCCESS;
}


StatusCode
HLT::Lvl1ResultAccessTool::updateConfig( bool useL1Muon, bool useL1Calo, bool useL1JetEnergy)
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
      ATH_MSG_DEBUG("Going through configured LVL1 MUON thresholds ... ");
      const std::vector<TrigConf::TriggerThreshold*>& muonThresholdConfig = thresholdConfig->getThresholdVector(L1DataDef::MUON);

      m_muonCfg.clear();
      m_useL1Muon = false; // care about muons in execute only if at least one muon TriggerElement is used in HLT !

      // make sure, the vector is filled up to a size of MAX MUON NUMBER (should be 6):
      while(m_muonCfg.size() <= 6) {
         m_muonCfg.push_back( ConfigThreshold() ); // default entries are not active !
      }

      for (const TrigConf::TriggerThreshold* muonTT : muonThresholdConfig) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1 MUON threshold: " << muonTT->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( muonTT->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash(muonTT->name());
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<<muonTT->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }
         int tn = muonTT->thresholdNumber()+1;
         if (tn < 0) {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!");
         } else if (tn > 6)  {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is greater 6, go on w/o this threshold!");
         } else{
            ATH_MSG_DEBUG("thresholdNumber = " << tn);
            m_muonCfg.at(tn) =  ConfigThreshold(muonTT->name(), teId, activeHLT) ;
            m_useL1Muon = true;
         }
      }
   }

   // ------------------------------
   //          EMTau RoIs
   // ------------------------------
   if (useL1Calo) {
      ATH_MSG_DEBUG("Going through configured LVL1 CALO thresholds ... ");
      m_useL1Calo = false; // care about calo RoIs in execute only if at least one calo TriggerElement is used in HLT !
      m_emCfg.clear();
      m_tauCfg.clear();
      m_emtauThresholds.clear();
      
      for( L1DataDef::TriggerType caloType : vector<L1DataDef::TriggerType>{ L1DataDef::EM, L1DataDef::TAU} ) {

         for (TriggerThreshold * clusterTT : thresholdConfig->getThresholdVector( caloType ) ) {

            if (clusterTT == nullptr) continue;
            
            // Needed to find thresholds passed by Run 2 RoI
            m_emtauThresholds.push_back(clusterTT);
            
            ATH_MSG_DEBUG("Searching TE that matches LVL1  EM/TAU threshold: " << clusterTT->name());

            unsigned int teId = 0;
            bool activeHLT = TrigConf::HLTTriggerElement::getId( clusterTT->name().c_str(), teId);
            teId = TrigConf::HLTUtils::string2hash( clusterTT->name() );
            if (activeHLT) {
               ATH_MSG_DEBUG(" => OK (found TE)");
               ATH_MSG_DEBUG(" => TE=(name="<< clusterTT->name() << ", hashID="<<teId << ")");
            } else {
               ATH_MSG_DEBUG(" => NO");
            }
            int tn = clusterTT->thresholdNumber();
            if (tn < 0) {
               ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!");
            } else {
               uint32_t mask= 0x1 << tn;
               ATH_MSG_DEBUG("thresholdNumber = " << tn << " .. bit position = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
               if (caloType == L1DataDef::EM)
                  m_emCfg.push_back( ConfigThreshold( clusterTT->name(), teId, activeHLT, mask ));
               else if (caloType == L1DataDef::TAU)
                  m_tauCfg.push_back( ConfigThreshold( clusterTT->name(), teId, activeHLT, mask ));
               m_useL1Calo = true;
            }
         }
      }
   } // if (useL1Calo)


   // ------------------------------
   //         JetEnergy RoIs
   // ------------------------------
   if (useL1JetEnergy) {

      ATH_MSG_DEBUG("Going through configured LVL1 JetEnergy thresholds ... ");

      m_useL1JetEnergy = false;// care about jetEnergy RoIs in execute only if at least one jetEnergy TriggerElement is used in HLT !
      m_jetCfg.clear();
      
      // In case we need to calculate thresholds passed: copy jet thresholds into vector RecJetRoI can use 
      m_jetThresholds = thresholdConfig->getJetThresholdVector(); 
      for (TrigConf::TriggerThreshold* fwdThr : thresholdConfig->getFJetThresholdVector() ) 
         m_jetThresholds.push_back(fwdThr); 
      // Jet threholds:
      for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getJetThresholdVector() ) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1 JET threshold: " << thr->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash( thr->name() );
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<< thr->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }

         int tn = thr->thresholdNumber();
         if (tn < 0) {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!");
         } else {
            uint32_t mask= 0x1 << tn; // 8 bits in Run 1, up to 25 in Run 2
            ATH_MSG_DEBUG("thresholdNumber = " << tn << " .. mask = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
            m_jetCfg.push_back( ConfigJetEThreshold( thr->name(), teId, activeHLT, mask, JetRoI) );
            m_useL1JetEnergy = true;
         }
      }


      // Forward Jet threholds (only Run 1):
      for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getFJetThresholdVector() ) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1  ForwardJET (FJET) threshold: " << thr->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash(thr->name());
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<<thr->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }
         int tn = thr->thresholdNumber();
         if (tn < 0) {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0" << ", go on w/o this threshold!");
         } else {
            uint32_t mask= 0x1;
            mask <<= 8; // first 8 bits are for jet thresholds (this code will never be reached in Run 2)
            mask <<= tn; // move to threshold number (4 bits)
            ATH_MSG_DEBUG("thresholdNumber = " << tn << " .. bit position = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
            m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT,  mask, ForwardJetRoI) );
            m_useL1JetEnergy = true;
         }
         
      }

      // Total Energy thresholds:
      for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getTotEtVector() ) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1  TotalEt (ET) threshold: " << thr->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash(thr->name());
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<<thr->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }
         int tn = thr->thresholdNumber();
         if (tn < 0) {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!" );
         } else {
            uint32_t mask= 0x1;
            mask <<= 16; // first 15 bits are EnergyY, 16th bit is overflow
            mask <<= tn; // 16 bits for TE (lower 8bit) and restricted range TE (upper 8bit)
            ATH_MSG_DEBUG( "thresholdNumber = " << tn << ", mask = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
            m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT, mask, TotalEtRoI) );
            m_useL1JetEnergy = true;
         }
      }


      // Jet Energy threholds:
      for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getJetEtVector() ) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1 JetEt (JE) threshold: " << thr->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash(thr->name());
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<<thr->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }
         int tn = thr->thresholdNumber();
         if (tn < 0) {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!");
         } else {
            uint32_t mask= 0x1 << tn; // 4 bits here !
            ATH_MSG_DEBUG("thresholdNumber = " << tn << ", mask = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
            m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT,mask, JetEtRoI) );
            m_useL1JetEnergy = true;
         }
      }


      // Missing Energy threholds:
      for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getMissEtVector() ) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1  MissingEt (XE) threshold: " << thr->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash(thr->name());
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<<thr->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }
         int tn = thr->thresholdNumber();
         if (tn < 0) {
            ATH_MSG_WARNING( "this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!");
         } else {
            uint32_t mask= 0x1;
            mask <<= 16; // first 15 bits are EnergyY, 16th bit is overflow
            mask <<= tn; // 16 bits for XE (lower 8bit) and restricted range XE (upper 8bit)
            ATH_MSG_DEBUG("thresholdNumber = " << tn << ", mask = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
            m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT, mask, MissingEtRoI) );
            m_useL1JetEnergy = true;
         }
      }


      for(const TrigConf::TriggerThreshold * thr : thresholdConfig->getMissEtSignVector() ) {
         ATH_MSG_DEBUG("Searching TE that matches LVL1  MissingEtSignificance (XS) threshold: " << thr->name());
         unsigned int teId = 0;
         bool activeHLT = TrigConf::HLTTriggerElement::getId( thr->name().c_str(), teId);
         teId = TrigConf::HLTUtils::string2hash(thr->name()); 
         if (activeHLT) {
            ATH_MSG_DEBUG(" => OK (found TE)");
            ATH_MSG_DEBUG(" => TE=(name="<<thr->name() << ", hashID="<<teId << ")");
         } else {
            ATH_MSG_DEBUG(" => NO");
         }
         int tn = thr->thresholdNumber();
         if (tn < 0) {
            ATH_MSG_WARNING("this LVL1 threshold bit position (" << tn << ") is smaller 0, go on w/o this threshold!");
         } else {
            uint32_t mask= 0x1;
            mask <<= 16; // first 15 bits are EnergyY, 16th bit is overflow
            mask <<= tn; // 8 bits here
            ATH_MSG_DEBUG("thresholdNumber = " << tn << ", mask = 0x" << hex << setw( 8 ) << setfill( '0' ) << mask << dec);
            m_jetCfg.push_back( ConfigJetEThreshold(thr->name(), teId, activeHLT, mask, METSignificanceRoI) );
            m_useL1JetEnergy = true;
         }
      }
   } // end of Jet/Energy part

   ATH_MSG_DEBUG(" Initialize done");
   return StatusCode::SUCCESS;
}



StatusCode
HLT::Lvl1ResultAccessTool::updateResult(const ROIB::RoIBResult& result,
                                        bool updateCaloRoIs)
{
   // set L1 items:
   createL1Items(result);
   createMuonThresholds(result);
   createEMTauThresholds(result, updateCaloRoIs);
   createJetEnergyThresholds(result,  updateCaloRoIs);
   return StatusCode::SUCCESS;
}


StatusCode
HLT::Lvl1ResultAccessTool::updateResult(const LVL1CTP::Lvl1Result& result)
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
bool
HLT::Lvl1ResultAccessTool::isCalibrationEvent(const ROIB::RoIBResult& result) const {

   ///return false; // in the RUN 2 menu there is currently no such restriction and all kinds of items are on these ctp IDs
   int ctpVersion = result.cTPResult().header().formatVersion() & 0xf ;
   CTPdataformatVersion v(ctpVersion);
   std::vector<ROIB::CTPRoI> ctpRoIVec(result.cTPResult().TAV());
   //  if ( ctpRoIVec.size() >= 8 && (ctpRoIVec[7].roIWord() &  0xE0000000) )
   if ( ctpRoIVec.size()==v.getTAVwords() && ( ctpRoIVec[ctpRoIVec.size()-1].roIWord() & 0xE0000000 ) )
      return true;
   else
      return false;
}

namespace {
   bool getBit(const std::vector<ROIB::CTPRoI>& words, unsigned position) {
      unsigned w  = position/32;  
      unsigned b  = position%32;  
      if ( words.size() > w) {
         uint32_t roIWord = words[w].roIWord();
         return (roIWord >> b) & 0x1;
      } 
      return false;
   }
}

// ==============================
//       create LVL1 items
// ==============================
std::vector< const LVL1CTP::Lvl1Item* >
HLT::Lvl1ResultAccessTool::createL1Items( const ROIB::RoIBResult& result,
                                          LVL1CTP::Lvl1Result const** lvl1ResultOut /*= nullptr*/)
{
   // Not const because it modifies the pointed-to objects in m_lvl1ItemConfig.
   return createL1Items (m_lvl1ItemConfig, result, lvl1ResultOut);
}


std::vector< const LVL1CTP::Lvl1Item* >
HLT::Lvl1ResultAccessTool::createL1Items( const std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >& lvl1ItemConfig,
                                          const ROIB::RoIBResult& result,
                                          LVL1CTP::Lvl1Result const** lvl1ResultOut /*= nullptr*/) const
{
   std::vector< const LVL1CTP::Lvl1Item* > items;
   std::vector<ROIB::CTPRoI> bitsBP = result.cTPResult().TBP();
   std::vector<ROIB::CTPRoI> bitsAP = result.cTPResult().TAP();
   std::vector<ROIB::CTPRoI> bitsAV = result.cTPResult().TAV();

   bool calib_flag = isCalibrationEvent(result);
   ATH_MSG_DEBUG("Calibration event? " << calib_flag);
   
   // loop over all configured items if no calibration, else only 3 last // Needs fixing
   int ctpVersion = result.cTPResult().header().formatVersion() & 0xf ;
   CTPdataformatVersion v(ctpVersion);
   
   unsigned first_item = calib_flag ? v.getMaxTrigItems()-3 : 0; // last three items are calib items, only those should be activated

   for ( unsigned i = first_item; i < lvl1ItemConfig.size(); i++ ) {
      if ( !lvl1ItemConfig[ i ] ) continue; // empty slot

      LVL1CTP::Lvl1Item* item =  lvl1ItemConfig[ i ].get();
      *item = LVL1CTP::Lvl1Item( item->name(), item->hashId(), 
                  getBit(bitsBP, i), 
                  getBit(bitsAP, i), 
                  getBit(bitsAV, i), 
                  item->prescaleFactor() );  
      items.push_back(item);
      ATH_MSG_DEBUG("Set bits on "<< item->name() <<" PS="<< item->prescaleFactor() <<"  BP=" <<getBit(bitsBP, i)<<" AP="<<getBit(bitsAP, i)<<" AV="<<getBit(bitsAV, i));
   }

  // Fill TBP, TAP in case we're creating the Lvl1Result
   if (lvl1ResultOut) {
      auto lvl1Result = std::make_unique<LVL1CTP::Lvl1Result>(true);

      // 1.) TAV
      const std::vector<ROIB::CTPRoI> ctpRoIVecAV = result.cTPResult().TAV();
      for (unsigned int iWord = 0; iWord < ctpRoIVecAV.size(); ++iWord) {
         uint32_t roIWord = ctpRoIVecAV[iWord].roIWord();
         lvl1Result->itemsAfterVeto().push_back(roIWord);
         ATH_MSG_DEBUG("TAV word #" << iWord << " is 0x" << hex << setw( 8 ) << setfill( '0' ) << roIWord << dec);
      }

      // 1.) TBP
      const std::vector<ROIB::CTPRoI> ctpRoIVecBP = result.cTPResult().TBP();
      for (unsigned int iWord=0; iWord < ctpRoIVecBP.size(); ++iWord) {
         uint32_t roIWord = ctpRoIVecBP[iWord].roIWord();
         lvl1Result->itemsBeforePrescale().push_back(roIWord);
         ATH_MSG_DEBUG( "TBP word #" << iWord << " is 0x" << hex
                        << setw( 8 ) << setfill( '0' ) << roIWord << dec);    
      }

      // 2.) TAP
      const std::vector<ROIB::CTPRoI> ctpRoIVecAP = result.cTPResult().TAP();
      for (unsigned int iWord=0; iWord < ctpRoIVecAP.size(); ++iWord) {
         uint32_t roIWord = ctpRoIVecAP[iWord].roIWord();
         lvl1Result->itemsAfterPrescale().push_back(roIWord);

         ATH_MSG_DEBUG("TAP word #" << iWord << " is 0x" << hex << setw( 8 ) << setfill( '0' ) << roIWord << dec);
         
      }
      // make sure TBP, TAP, TAV all have 8 entries !
      while (lvl1Result->itemsBeforePrescale().size() < 8) lvl1Result->itemsBeforePrescale().push_back(0);
      while (lvl1Result->itemsAfterPrescale().size() < 8) lvl1Result->itemsAfterPrescale().push_back(0);
      while (lvl1Result->itemsAfterVeto().size() < 8) lvl1Result->itemsAfterVeto().push_back(0);
      *lvl1ResultOut = lvl1Result.get();
      evtStore()->record(std::move(lvl1Result), "Lvl1Result").ignore();
   } // if (lvl1ResultOut)

   return items;
}

// ==============================
//        create Muon RoIs
// ==============================
const std::vector<HLT::MuonRoI> &
HLT::Lvl1ResultAccessTool::createMuonThresholds(const ROIB::RoIBResult &result)
{
   m_muonRoIs.clear();

   if (m_useL1Muon) {

      const std::vector<ROIB::MuCTPIRoI>& muonRoIVec = result.muCTPIResult().roIVec();

      // some Debug output
      ATH_MSG_DEBUG("Looking for Muon RoIs");
      ATH_MSG_DEBUG("---------------------");

      for (std::vector<ROIB::MuCTPIRoI>::const_iterator itMuon  = muonRoIVec.begin();
           itMuon != muonRoIVec.end(); ++itMuon) {

         MuonRoI roi(*itMuon);
         uint32_t roIWord = itMuon->roIWord();

         ATH_MSG_DEBUG("Muon RoI word: 0x" << hex << setw(8) << roIWord  << dec);

         // the Threshold Number (1 to 6)  associated with this Muon RoI
         unsigned int thresholdNumber = itMuon->pt();

         // this should never happen ... but who knows
         if (thresholdNumber > 6 || thresholdNumber < 1) {
            ATH_MSG_WARNING("Muon threshold number is not between 1 and 6 but ="
                            << thresholdNumber << " !!! Go on with a value of 1");
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
            }
         }

         // produce all TEs that have an equal or lower threshold number AND are needed by HLT
         for (std::vector<int>::const_iterator th = thToFire.begin(); th != thToFire.end(); ++th) {
	
            roi.addThreshold( & m_muonCfg[*th] );
	
            ATH_MSG_DEBUG("found Muon Threshold " << m_muonCfg[*th].name << " threshold number: " << *th);
         }
      
         m_muonRoIs.push_back(roi);
      }    
   }  
   return m_muonRoIs;
}


// ==============================
//          EMTau RoIs
// ==============================
const std::vector< HLT::EMTauRoI >&
HLT::Lvl1ResultAccessTool::createEMTauThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs)
{
   m_emTauRoIs.clear();

   if (m_useL1Calo) {

      // reconstruct RoIs
      ATH_MSG_DEBUG("Looking for EMTau RoIs");
      ATH_MSG_DEBUG("----------------------");

      std::map<std::pair<int,int>, int> emRoI, tau; // (crate,module) : count

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

            ATH_MSG_DEBUG("RoI word: 0x" << MSG::hex << std::setw(8) << roIWord << ", threshold pattern " << roi.thresholdMask() << MSG::dec);

            if (recRoI.roiType() == LVL1::TrigT1CaloDefs::EMRoIWordType || recRoI.roiType() == LVL1::TrigT1CaloDefs::CpRoIWordType) {
               for ( std::vector< ConfigThreshold >::const_iterator threshold = m_emCfg.begin();
                     threshold != m_emCfg.end(); ++threshold) {

                  // check whether this LVL1 threshold is used by HLT:
                  if ( !(threshold->mask & roi.thresholdMask()) ) continue;

                  roi.addThreshold( &(*threshold) );

                  ATH_MSG_DEBUG("creating EM Theshold: "  << threshold->name);
               }
            }
            if (recRoI.roiType() == LVL1::TrigT1CaloDefs::TauRoIWordType || recRoI.roiType() == LVL1::TrigT1CaloDefs::CpRoIWordType) {
               for ( std::vector< ConfigThreshold >::const_iterator threshold = m_tauCfg.begin();
                     threshold != m_tauCfg.end(); ++threshold) {

                  // check whether this LVL1 threshold is used by HLT:
                  if ( !(threshold->mask & roi.thresholdMask()) ) continue;

                  roi.addThreshold( &(*threshold) );

                  ATH_MSG_DEBUG("creating TAU Theshold: "  << threshold->name);
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
const std::vector< HLT::JetEnergyRoI >&
HLT::Lvl1ResultAccessTool::createJetEnergyThresholds(const ROIB::RoIBResult& result, bool /*updateCaloRoIs*/)
{
   m_jetRoIs.clear();

   if (m_useL1JetEnergy) {

      ATH_MSG_DEBUG("Looking for JetEnergy RoIs");
      ATH_MSG_DEBUG("--------------------------");

      const std::vector<ROIB::JetEnergyResult>& jetEnergyResult = result.jetEnergyResult();

      ATH_MSG_DEBUG("# Jet Energy results " << jetEnergyResult.size());

      for (const ROIB::JetEnergyResult & JEResult : jetEnergyResult) {
      
         const std::vector<ROIB::JetEnergyRoI>& roIVec = JEResult.roIVec();

         ATH_MSG_DEBUG("# Jet Energy RoIs " << roIVec.size());

         for ( std::vector<ROIB::JetEnergyRoI>::const_iterator itJetEn = roIVec.begin();
               itJetEn != roIVec.end(); ++itJetEn) {

            uint32_t roIWord = itJetEn->roIWord();
            // Fix for change of RoI format - long obsolete
            //if (updateCaloRoIs) roIWord = LVL1::RoIFormatUpdater::UpdateJetEnergyRoI( roIWord );

            // Create RoI
            HLT::JetEnergyRoI roi = HLT::JetEnergyRoI( ROIB::JetEnergyRoI(roIWord) );

            // create a second roi container, since the 3 roiWords contain all SumET & all MissingET thresholds!
            // use roi for SumET  and  roi2 for MissingET
            HLT::JetEnergyRoI roiET = HLT::JetEnergyRoI( ROIB::JetEnergyRoI(roIWord) );

            unsigned int etSumType = itJetEn->etSumType();

            ATH_MSG_DEBUG("RoI word: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << roIWord << std::dec);

            // RoI Type
            LVL1::TrigT1CaloDefs::RoIType roiType = m_jepDecoder->roiType(roIWord);
            
            if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0) {

               // this roIWord is of type Energy RoI (Energy RoI consists of 3 RoI words!) => get the next two words as well
               ++itJetEn;
               if (itJetEn == roIVec.end()) break; 

               uint32_t roIWord1 = itJetEn->roIWord();

               ++itJetEn;
               if (itJetEn == roIVec.end()) break; 

               uint32_t roIWord2 = itJetEn->roIWord();

               if ( !roiET.setExtraWords(roIWord, roIWord1, roIWord2)) {
                  ATH_MSG_ERROR("Error setting Total/Missing ET RoI extra words");
                  return m_jetRoIs;
               }

               ATH_MSG_DEBUG( "RoI triplet = 0x" <<  std::hex << std::setw( 8 ) << std::setfill( '0' )
                              << roIWord << " 0x" << roIWord1 << " 0x" << roIWord2 << std::dec);

            } else if ( roiType == LVL1::TrigT1CaloDefs::JetRoIWordType ) {        

               // For Jet RoIs, fill thresholdMask word, used later for setting Configured Thresholds
               // Do it here so that it only needs to be done once/RoI

               LVL1::RecJetRoI recRoI( roIWord, &m_jetThresholds );
               roi.setThresholdMask(recRoI.thresholdPattern());

               ATH_MSG_DEBUG("Jet RoI threshold pattern: 0x" << std::hex << recRoI.thresholdPattern() << std::dec);

            }


            // Loop over possible Jet thresholds
            for (std::vector< ConfigJetEThreshold >::const_iterator threshold = m_jetCfg.begin();
                 threshold != m_jetCfg.end(); ++threshold) {

               if ( roiType == LVL1::TrigT1CaloDefs::JetRoIWordType && (threshold->type == JetRoI || threshold->type == ForwardJetRoI) ) {

                  ATH_MSG_DEBUG(threshold->name << " is of type JetRoI or ForwardJetRoI");

                  // plain Jet & ForwardJet RoI
                  //---------------------
                  if ( ! addJetThreshold(roi, &*threshold) ) {
                     ATH_MSG_WARNING("Problem while adding threshold: " << threshold->name << " returning prematurely");
                     return  m_jetRoIs;
                  }

               } else if ( roiType == LVL1::TrigT1CaloDefs::JetEtRoIWordType && threshold->type == JetEtRoI ) {

                  ATH_MSG_DEBUG(threshold->name << " is of type JetEtRoI");

                  // JetEt ET RoI (total transv. energy from jets)
                  //----------------------------------------------
                  if ( ! addJetThreshold(roi, &*threshold) ) {
                     ATH_MSG_WARNING("Problem while adding threshold: " << threshold->name << " returning prematurely");
                     return m_jetRoIs;
                  }

               } else if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 && ( threshold->type == TotalEtRoI ||
                                                                                    threshold->type == MissingEtRoI ||
                                                                                    threshold->type == METSignificanceRoI)) {

                  bool ignoreThreshold = false;
                  bool thrIsRestrictedRangeThr = false;
                  // for TE and XE thresholds the etSumType distinguishes between full and restricted range thresholds
                  //    etSumType==0: full eta range
                  //    etSumType==1: restricted eta range
                  // restricted range thresholds must be ignored when the etSumType==0 and vice versa
                  // restricted range thresholds have a mask >= 0x1000000  (lower 24 bit are energy sum (15bit), overflow (1bit) and full range thresholds (8bit)
                  if( threshold->type == TotalEtRoI || threshold->type == MissingEtRoI) {

                     if(threshold->mask >= 0x1000000 )
                        thrIsRestrictedRangeThr = true;

                     if( (etSumType==0 &&  thrIsRestrictedRangeThr) ||
                         (etSumType==1 && !thrIsRestrictedRangeThr) )
                        {
                           ignoreThreshold = true;
                        }

                  }
                  
                  if( ! ignoreThreshold ) {

                     ATH_MSG_DEBUG( threshold->name << " is of type " << threshold->type << " TotalEtRoI (" << TotalEtRoI << ") or MissingEtRoI (" << MissingEtRoI << ") or METSignificanceRoI (" << METSignificanceRoI << ")"
                                    << " and etSumType " << etSumType << "(" << (etSumType==0?"full range":"restricted range") << ")");

                     roiET.setType(MissingOrTotalEtRoI);

                     if ( ! addMetThreshold(roiET, &*threshold, etSumType==1) ) {
                        ATH_MSG_WARNING("Problem while adding threshold: " << threshold->name << " returning prematurely");
                        return m_jetRoIs;
                     }

                  }

               } // end of Energy part (totEt, missEt)

               if (itJetEn == roIVec.end()) break;
            } // end of loop over possible thresholds
	
            if (roi.size()!=0 ) m_jetRoIs.push_back(roi);
	
            //this rois have type set to unknown in all normal jet RoIs
            // add TotalEt and MissingEt rois even if no threshold has fired
            if (roiET.type() != Unknown) {
               ATH_MSG_DEBUG("adding MissingOrTotalET RoI with: " <<  roiET.size() << " thresholds");
               m_jetRoIs.push_back(roiET);
            }
         } // end of loop over RoIs
      } // end of jetEnergy Result
   }// enf of m_useL1JetEnergy
   ATH_MSG_DEBUG("done with jet thresholds");
   return m_jetRoIs;
}


std::bitset<3>
HLT::Lvl1ResultAccessTool::lvl1EMTauJetOverflow(const ROIB::RoIBResult& result)
{   
   const int MAXEM = 5;         // see ATR-12285
   const int MAXTAU = 5;
   const int MAXJET = 4;
   
   std::map<std::pair<int,int>, int> em, tau, jet;  // (crate,module) : count

   // Count number of RoIs per crate/module
   for (const ROIB::EMTauResult& res : result.eMTauResult()) {
      for (const ROIB::EMTauRoI& roi : res.roIVec()) {
         int c = m_cpDecoder->crate(roi.roIWord());
         int m = m_cpDecoder->module(roi.roIWord());
         int t = m_cpDecoder->roiType(roi.roIWord());

         if (t==LVL1::TrigT1CaloDefs::EMRoIWordType) ++em[{c,m}];
         else if (t==LVL1::TrigT1CaloDefs::TauRoIWordType) ++tau[{c,m}];
      }
   }

   for (const ROIB::JetEnergyResult& res : result.jetEnergyResult()) {
      for (const ROIB::JetEnergyRoI& roi : res.roIVec()) {
         int c = m_jepDecoder->crate(roi.roIWord());
         int m = m_jepDecoder->module(roi.roIWord());
         int t = m_jepDecoder->roiType(roi.roIWord());

         if (t==LVL1::TrigT1CaloDefs::JetRoIWordType) ++jet[{c,m}];
      }
   }

   // Check if there was an overflow
   std::bitset<3> overflow;
   overflow[0] = std::count_if(em.begin(), em.end(), [](const decltype(em)::value_type& i){return i.second>MAXEM;});
   overflow[1] = std::count_if(tau.begin(), tau.end(), [](const decltype(tau)::value_type& i){return i.second>MAXTAU;});
   overflow[2] = std::count_if(jet.begin(), jet.end(), [](const decltype(jet)::value_type& i){return i.second>MAXJET;});

   if (msgLvl(MSG::DEBUG)) {
      msg() << "EM RoI multiplicities by crate,module: ";
      for (const auto& i : em) msg() << "(" << i.first.first << "," << i.first.second << "):" << i.second << " ";

      msg() << endmsg << "Tau RoI multiplicities by crate,module: ";
      for (const auto& i : tau) msg() << "(" << i.first.first << "," << i.first.second << "):" << i.second << " ";

      msg() << endmsg << "Jet RoI multiplicities by crate,module: ";
      for (const auto& i : jet) msg() << "(" << i.first.first << "," << i.first.second << "):" << i.second << " ";
      msg() << endmsg;
   }
   
   return overflow;
}

bool
HLT::Lvl1ResultAccessTool::addJetThreshold(HLT::JetEnergyRoI & roi, const ConfigJetEThreshold* threshold ) {

   if ( !(threshold->mask & roi.thresholdMask()) ) {
      return true;
   }

   if (!roi.setType(threshold->type)) {
      ATH_MSG_ERROR("Inconsistent threshold types " << roi.type() << " " << threshold->type);
      return false;
   }
   roi.addThreshold( threshold );
   ATH_MSG_DEBUG("Creating Jet  threshold: " << threshold->name);
   return true;
}


// this is made separate function as it usesses different word for threhold bits (namely word1/2)
bool
HLT::Lvl1ResultAccessTool::addMetThreshold( HLT::JetEnergyRoI & roi, const ConfigJetEThreshold* threshold, bool isRestrictedRange ) {

   uint32_t word(0);
   switch(threshold->type) {
   case METSignificanceRoI: 
      word = roi.word0();
      break;
   case TotalEtRoI: 
      word = roi.word1();
      break;
   case MissingEtRoI:
      word = roi.word2();
      break;
   default:
      word = 0;
   }

   uint32_t mask = threshold->mask;

   if (isRestrictedRange) {
      mask >>= 8;
   }

   if ( (mask & word) == 0 ) {
      return true;
   }


   if (!roi.setType(threshold->type)) {
      ATH_MSG_ERROR("Inconsistent threshold types " << roi.type() << " " << threshold->type);
      return false;
   }

   roi.addThreshold( threshold );
   ATH_MSG_DEBUG("creating Met threshold: " << threshold->name);

   return true;
}

