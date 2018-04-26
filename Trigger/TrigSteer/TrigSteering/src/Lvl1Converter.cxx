/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl1Converter
 *
 * @brief see header file!
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>         - UCI - AGH
 *
 * File and Version Information:
 * $Id: Lvl1Converter.cxx,v 1.49 2009-03-24 20:41:45 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Lvl1Converter.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Lvl1Result.h"

#include "TrigT1Result/RoIBResult.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecJetEtRoI.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

// Include for the Level-1 configuration
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"
#include "TrigSteering/Lvl1ConsistencyChecker.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ThreadGaudi.h"

using namespace HLT;

Lvl1Converter::Lvl1Converter(const std::string& name, const std::string& type,
			   const IInterface* parent)
  : LvlConverter(name, type, parent),
    m_gotL1Config(false),
    m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool",this),
    m_lvl1ConsistencyTool("Lvl1ConsistencyChecker/Lvl1ConsistencyChecker", this),
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_recRPCRoiSvc(0), m_recTGCRoiSvc(0), m_timerSvc(0),    
    m_totalTime(0), m_sgTime(0), m_lvl1ItemsTime(0), m_caloTime(0),
    m_muonTime(0), m_jetTime(0)
{
   declareProperty("useL1Calo", m_useL1Calo = true);
   declareProperty("useL1Muon", m_useL1Muon = true);
   declareProperty("useL1JetEnergy", m_useL1JetEnergy = true);
   declareProperty("useOldRoIWordFormat",m_updateCaloRoIs=false);
   declareProperty("doTiming",  m_doTiming = false, "Enables timing measurement for this algorithm");
   declareProperty( "Lvl1ResultAccessTool", m_lvl1Tool, "tool to retrieve lvl 1 trigger information");
   declareProperty( "Lvl1ConsistencyChecker", m_lvl1ConsistencyTool, "tool to cehck items & thresholds consistency");
   declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
   
   declareProperty( "overallRoIsLimit", m_overallRoIsLimit = 0, "Return failure (specific one) if number of RoIs > that. 0 means no limit." );
   declareProperty( "jetRoIsLimit", m_jetRoIsLimit = 0,         "Return failure (specific one) if number of JET RoIs > that. 0 means no limit." );
   declareProperty( "emtauRoIsLimit", m_emtauRoIsLimit = 0,     "Return failure (specific one) if number of EMTAU RoIs > that. 0 means no limit." );
   declareProperty( "muonRoIsLimit", m_muonRoIsLimit = 0,           "Return failure (specific one) if number of MU RoIs > that. 0 means no limit." );
   declareProperty( "ignoreL1Prescales", m_ignoreL1Prescales=false, "If true  L1 prescales are ignored (takes But Before Prescale)");

}

ErrorCode Lvl1Converter::hltInitialize()
{
  
  // retrieve Lvl1ResultAccessTool :
  //  StatusCode sc = toolSvc()->retrieveTool("HLT::Lvl1ResultAccessTool", m_lvl1Tool);

  if ( m_lvl1Tool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve lvl1 result access tool: " << m_lvl1Tool);
    return HLT::FATAL;
  }

  
  if ( m_lvl1ConsistencyTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Unable to retrieve lvl1 consistency tool: " << m_lvl1ConsistencyTool);
    return HLT::FATAL;
  }

  StatusCode sc;
  if (m_doTiming) {
    sc = service("TrigTimerSvc", m_timerSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("unable to locate timing service TrigTimerSvc");
      m_timerSvc = 0;
      return HLT::ERROR;
    }
    ATH_MSG_ERROR("Got timing service TrigTimerSvc");
    m_totalTime = m_timerSvc->addItem(getGaudiThreadGenericName(name())+":TotalTime");
    m_sgTime = m_timerSvc->addItem(getGaudiThreadGenericName(name())+":RetrievalTime");
    m_lvl1ItemsTime = m_timerSvc->addItem(getGaudiThreadGenericName(name())+":ItemsTime");
    m_caloTime = m_timerSvc->addItem(getGaudiThreadGenericName(name())+":EMTauTime");
    m_muonTime = m_timerSvc->addItem(getGaudiThreadGenericName(name())+":MuonTime");
    m_jetTime = m_timerSvc->addItem(getGaudiThreadGenericName(name())+":JetEnergyTime");
  }

  sc = m_lvl1Tool->updateConfig(m_useL1Muon, m_useL1Calo, m_useL1JetEnergy);

  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to configure tool!");
    return HLT::FATAL;
  }

  // --------------------------------------------------------------
  //   Connect to the LVL1ConfigSvc for the trigger configuration:
  // --------------------------------------------------------------
  sc = m_configSvc.retrieve();
  m_gotL1Config = true;
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Couldn't connect to " << m_configSvc.typeAndName());
    m_gotL1Config = false;
  } 
    

  // ------------------------------
  //            Muon RoIs
  // ------------------------------
  if (m_useL1Muon) {
    // Get the RPC RecRoI service
    sc = service(LVL1::ID_RecRpcRoiSvc, m_recRPCRoiSvc);
    if(sc.isFailure()) ATH_MSG_WARNING("Unable to get pointer to RecRPCRoiSvc!");

    // Get the TGC RecRoI service
    sc = service(LVL1::ID_RecTgcRoiSvc, m_recTGCRoiSvc);
    if(sc.isFailure()) ATH_MSG_WARNING("Unable to get pointer to RecTGCRoiSvc!");

    // +++++++++++++++++++++++++++++++++++
  }

  if ( m_lvl1ConsistencyTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Unable to get Lvl1ConsistencyChecker pointer!");
    return HLT::ERROR;
  }

  if ( m_lvl1ConsistencyTool->updateConfig(&*m_configSvc).isFailure() ) {
    ATH_MSG_ERROR("Lvl1ConsistencyChecker unable to digest config");
    return HLT::ERROR;
  }

  ATH_MSG_DEBUG("Initialize done");
  return HLT::OK;
}

ErrorCode Lvl1Converter::hltFinalize()
{
  return HLT::OK;
}

ErrorCode Lvl1Converter::hltExecute(std::vector<HLT::SteeringChain*>& chainsToRun)
{
   m_RoI2TE.clear();
   std::vector<HLT::SteeringChain*> chains;
   if(m_doTiming) m_totalTime->start();
   // =============================================================
   // Step 1: Retrieve the LVL1 results, containing all RoIs and
   //         the CTP result
   // =============================================================
   if(m_doTiming) m_sgTime->start();

   const ROIB::RoIBResult* result;
   StatusCode sc = evtStore()->retrieve(result);

   if(sc.isFailure()){
      ATH_MSG_WARNING("Unable to retrieve RoIBResult from storeGate!");
      return HLT::NO_LVL1_RESULT;
   }

   if ( !result->cTPResult().isComplete() ) {
      ATH_MSG_WARNING("Incomplete CTP result");       
      return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::USERDEF_1, HLT::SteeringInternalReason::NO_LVL1_RESULT);
   }

   //  m_lvl1Tool->updateResult(*result, m_updateCaloRoIs);// is done in each step ..
   unsigned int l1Id = m_config->getLvl1Id();

   if(m_doTiming) m_sgTime->stop();

   // =============================================================
   // Step 2: Activate HLT chains that have a matching LVL1 item
   //         - First, get and decode the CTP result for this event
   //         - Then, compare with HLT chains
   // =============================================================

   if(m_doTiming) m_lvl1ItemsTime->start();
   const std::vector<const LVL1CTP::Lvl1Item*>& items = m_lvl1Tool->createL1Items(*result, true);

   ATH_MSG_DEBUG("Lvl1 provides: " <<  items.size()  << " items");
   if  ( items.size() == 0 ) {
      ATH_MSG_WARNING("Lvl1 delivered 0 items, HLT cannot continue");
      return HLT::NO_LVL1_ITEMS;
   }

   for (const LVL1CTP::Lvl1Item* item : items) {

      bool l1BeforePrescale = item->isPassedBeforePrescale();
      bool l1Passed         = item->isPassedAfterVeto(); // that looks like bug but it isn't 

      if ( m_ignoreL1Prescales ) // undo L1 prescaling (only when JO says so)
         l1Passed = l1BeforePrescale;

      // search for corresponding configured HLT chain(s):
      for(HLT::SteeringChain* chain : m_chainIdMap[ item->hashId() ]) {
         // activate and push into vector of active chains
         if (chain) {
            addChain(chains, chain, l1Passed, l1BeforePrescale);
         } else {
            ATH_MSG_WARNING("No configured chain found,  that would match LVL1 item from previous level, this should not happen at thsi stage " << item->name());
         }
      }
      // some debug output
      ATH_MSG_DEBUG( "Used LVL1 item " << item->name() << " (" << item->hashId() << ") "
                     << "TBP: "  <<  item->isPassedBeforePrescale() << " TAP: " << item->isPassedAfterPrescale()  << " TAV: " << item->isPassedAfterVeto());
   }


   // =============================================================
   // Step 3: Activate unseeded chains
   //         This which have no specified lvl1 seeding item
   // =============================================================

   // loop over chains and activate (in addition) all those that have no lower_chain specified !
   if( ! m_lvl1Tool->isCalibrationEvent(*result) ) {
      for ( std::vector<HLT::SteeringChain*>::const_iterator chain = m_chainsAlwaysActive.begin();
            chain != m_chainsAlwaysActive.end(); ++chain) {
         addChain(chains, *chain, true, true);
      }
   }

   // make chains list unique
   std::set<HLT::SteeringChain*> temp;
   for ( std::vector<HLT::SteeringChain*>::const_iterator chain = chains.begin();
         chain != chains.end(); ++chain) {    
      unsigned int ss = temp.size(); 
      temp.insert(*chain);
      if ( ss != temp.size() ) 
         chainsToRun.push_back(*chain);
   }

   // -----------------------------------------------------------------

   // do we need to go on, if there is no active LVL1 item ? => stop here
   if ( chains.size() == 0 ) {
      ATH_MSG_DEBUG("None of configured HLT chains matched Lvl1 items");
      return HLT::OK;
   }

   if(m_doTiming) m_lvl1ItemsTime->stop();

   // =============================================================
   // Step 4: Create TriggerElements from level 1 RoIs, but only
   //         those which are needed in at least one HLT sequence
   // =============================================================

   // create initial leaf in Navigation:
   HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();
   ATH_MSG_DEBUG("initial Navigation node created");

   unsigned int roiId = 0;
   unsigned muonRoIsCount=0;
   unsigned emtauRoIsCount=0;
   unsigned jetRoIsCount=0;
   // ------------------------------
   //            Muon RoIs
   // ------------------------------
   if(m_doTiming) m_muonTime->start();
   if (m_useL1Muon) {
    
      // Get configuration
      std::vector<TrigConf::TriggerThreshold*> muonConfig;
      if (m_gotL1Config) {
         const std::vector<TrigConf::TriggerThreshold*>& thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
         for (TrigConf::TriggerThreshold * thr : thresholds) {
            if ( thr->type() == TrigConf::L1DataDef::muonType() ) muonConfig.push_back(thr);
         }
      }
  
      // reconstruct RoIs
      ATH_MSG_DEBUG("Muon RoIs");

      const std::vector< HLT::MuonRoI >& muonRoIs = m_lvl1Tool->createMuonThresholds(*result);

      for (const HLT::MuonRoI & muonRoI : muonRoIs) {

         // First create the RoI TE
         TriggerElement* roiTE = 0;
         TrigRoiDescriptor* roID = 0;

         for (const ConfigThreshold * threshold : muonRoI) {

            if (!m_recRPCRoiSvc || !m_recTGCRoiSvc) {
               ATH_MSG_WARNING("Unable to create muon TriggerElements, because RPC RoI Scv or TGC RoI Svc were not found!");
               continue;
            }

            // check whether this LVL1 threshold is used by HLT:
            //        if ( ! threshold->activeHLT ) continue;

            if (!roID) {
               roiTE = m_config->getNavigation()->addRoINode( initialTE );
               LVL1::RecMuonRoI* recRoI = new LVL1::RecMuonRoI(muonRoI.lvl1RoI().roIWord(), m_recRPCRoiSvc, m_recTGCRoiSvc, &muonConfig);
               float fixedphi = fixphi(recRoI->phi());
               roID = new TrigRoiDescriptor(recRoI->roiWord(),l1Id, roiId++,
                                            recRoI->eta(), recRoI->eta()-0.1, recRoI->eta()+0.1,
                                            fixedphi, fixedphi-0.1, fixedphi+0.1);

               std::string key;
               m_config->getNavigation()->attachFeature( roiTE, roID,   HLT::Navigation::ObjectCreatedByNew, key,  "initialRoI" );
               m_config->getNavigation()->attachFeature( roiTE, recRoI, HLT::Navigation::ObjectCreatedByNew, key, "RecMuonRoI" );

               ATH_MSG_DEBUG("creating RoINode with attached RoIDescriptor with id: " << (roiId - 1));
            }

            ATH_MSG_DEBUG("creating TriggerElement for Muon ROI with name: " << threshold->name << " and hashID: " <<  threshold->hashId);
    
            m_config->getNavigation()->addNode( roiTE, threshold->hashId );
         }

         if(roiTE) {
            insertRoITEinMap(muonRoI.lvl1RoI().roIWord(), roiTE);
         }


      }
      // put the limit on the number of muon RoIs
      muonRoIsCount = muonRoIs.size();
   }
   if(m_doTiming) m_muonTime->stop();

   // ------------------------------
   //          EMTau RoIs
   // ------------------------------
   if(m_doTiming) m_caloTime->start();
   if (m_useL1Calo) {

      // Get configuration
      std::vector<TrigConf::TriggerThreshold*> caloConfig;
      if (m_gotL1Config) {
         const std::vector<TrigConf::TriggerThreshold*>& thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
         for (TrigConf::TriggerThreshold * thr : thresholds) {
            if ( thr->type() == TrigConf::L1DataDef::emType() ||
                 thr->type() == TrigConf::L1DataDef::tauType() ) caloConfig.push_back(thr);
         }
      }

      // reconstruct RoIs
      ATH_MSG_DEBUG("EmTau RoIs");

      const std::vector< HLT::EMTauRoI >& emTauRoIs = m_lvl1Tool->createEMTauThresholds(*result, m_updateCaloRoIs);

      for (std::vector<  HLT::EMTauRoI >::const_iterator emTauRoI = emTauRoIs.begin();
           emTauRoI != emTauRoIs.end(); ++emTauRoI) {

         ATH_MSG_DEBUG( "RoI word: 0x" << std::hex << std::setw(8) << emTauRoI->lvl1RoI().roIWord() << std::dec);

         // First create the RoI TE
         TriggerElement* roiTE = 0;
         TrigRoiDescriptor* roID = 0;

         for (std::vector<const ConfigThreshold*>::const_iterator threshold = emTauRoI->begin();
              threshold != emTauRoI->end(); ++threshold) {

            // check whether this LVL1 threshold is used by HLT:
            //if ( !(*threshold)->activeHLT ) continue;

            if (!roID) {

               roiTE = m_config->getNavigation()->addRoINode( initialTE );
               LVL1::RecEmTauRoI* recRoI = new LVL1::RecEmTauRoI(emTauRoI->lvl1RoI().roIWord(), &caloConfig);
               float fixedphi = fixphi(recRoI->phi());
               roID = new TrigRoiDescriptor(recRoI->roiWord(), l1Id, roiId++,
                                            recRoI->eta(),recRoI->eta()-0.1,recRoI->eta()+0.1,
                                            fixedphi, fixedphi-0.1, fixedphi+0.1
                                            );

               std::string key;
               m_config->getNavigation()->attachFeature( roiTE, roID,   HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );
               m_config->getNavigation()->attachFeature( roiTE, recRoI, HLT::Navigation::ObjectCreatedByNew, key, "RecEmTauRoI" );

               ATH_MSG_DEBUG("creating RoINode with attached RoIDescriptor with id: "
                             << (roiId-1) << " roiWord: " << roID->roiWord());
            }

            ATH_MSG_DEBUG("creating TriggerElement for Calo ROI with name: "
                          << (*threshold)->name << " and hashID: " <<  (*threshold)->hashId);

            m_config->getNavigation()->addNode( roiTE, (*threshold)->hashId );

         }

         if(roiTE) {
            insertRoITEinMap(emTauRoI->lvl1RoI().roIWord(), roiTE);
         }

      }

      // put the limit on the number of emtau RoIs
      emtauRoIsCount = emTauRoIs.size();
   }
   if(m_doTiming) m_caloTime->stop();


   // ------------------------------
   //         JetEnergy RoIs
   // ------------------------------
   if(m_doTiming) m_jetTime->start();
   if (m_useL1JetEnergy) {
      // Get configuration
      std::vector<TrigConf::TriggerThreshold*> jetConfig;
      std::vector<TrigConf::TriggerThreshold*> energyConfig;
      if (m_gotL1Config) {
         for (TrigConf::TriggerThreshold * thr : m_configSvc->ctpConfig()->menu().thresholdVector() ) {
            if ( thr->type() == TrigConf::L1DataDef::jetType() ||
                 thr->type() == TrigConf::L1DataDef::jfType()  ||
                 thr->type() == TrigConf::L1DataDef::jbType()
                 )
               {
                  jetConfig.push_back(thr);
               } else if ( thr->type() == TrigConf::L1DataDef::xeType() ||
                           thr->type() == TrigConf::L1DataDef::teType() )
               {
                  energyConfig.push_back(thr);
               }
         }
      }

      // reconstruct RoIs
      ATH_MSG_DEBUG("JetEnergy RoIs");

      const std::vector< HLT::JetEnergyRoI >& jetERoIs = m_lvl1Tool->createJetEnergyThresholds(*result, m_updateCaloRoIs);
      unsigned jetRoIsCount = 0;

      for ( std::vector<  HLT::JetEnergyRoI >::const_iterator jetERoI = jetERoIs.begin();
            jetERoI != jetERoIs.end(); ++jetERoI) {
      
         ATH_MSG_DEBUG("RoI word: 0x" << std::hex
                       << std::setw(8) << jetERoI->lvl1RoI().roIWord() << std::dec);

         // First create the RoI TE
         TriggerElement* roiTE = 0;
         TriggerElement* metRoITE = 0;
         TriggerElement* jetEtRoITE     = 0;
         std::string key;


         // always create jetEtRoI + objects attached to it 
         if (jetERoI->type() == JetEtRoI ) {
            jetEtRoITE = m_config->getNavigation()->addRoINode( initialTE );
            LVL1::RecJetEtRoI* recRoI = new LVL1::RecJetEtRoI(jetERoI->lvl1RoI().roIWord());
            m_config->getNavigation()->attachFeature( jetEtRoITE, recRoI, HLT::Navigation::ObjectCreatedByNew, key, "RecJetRoI" );
            ATH_MSG_DEBUG("creating RoINode with attached LVL1::RecJetEtRoI ");
         }

         // always create MetEt&TotEt RoI  + objects attached to it 
         if ( jetERoI->type() == MissingOrTotalEtRoI ) {
            metRoITE = m_config->getNavigation()->addRoINode( initialTE );
            LVL1::RecEnergyRoI* recRoI = new LVL1::RecEnergyRoI(jetERoI->lvl1RoI().roIWord(),
                                                                jetERoI->word1(), jetERoI->word2(), &energyConfig); 
            m_config->getNavigation()->attachFeature( metRoITE, recRoI, HLT::Navigation::ObjectCreatedByNew, key, "RecJetRoI" ); 
            ATH_MSG_DEBUG("creating RoINode with attached LVL1::RecEnergyRoI");
         }
         roiTE=0;	

         for (std::vector<const ConfigThreshold*>::const_iterator threshold = jetERoI->begin();
              threshold != jetERoI->end(); ++threshold) {

            TrigRoiDescriptor* roID = 0;

            // check whether this LVL1 threshold is used by HLT:
            //	if ( !(*threshold)->activeHLT ) continue;

            if (!roiTE) {

               if (jetERoI->type() == JetRoI || jetERoI->type() == ForwardJetRoI) {
                  roiTE = m_config->getNavigation()->addRoINode( initialTE );

                  if(roiTE) {
                     insertRoITEinMap( jetERoI->lvl1RoI().roIWord(), roiTE );                  

                     LVL1::RecJetRoI* recRoI = new LVL1::RecJetRoI(jetERoI->lvl1RoI().roIWord(), &jetConfig);
                     float fixedphi = fixphi(recRoI->phi());
                     roID = new TrigRoiDescriptor( recRoI->roiWord(), l1Id, roiId++,
                                                   recRoI->eta(), recRoI->eta()-0.4, recRoI->eta()+0.4,
                                                   fixedphi, fixedphi-0.4, fixedphi+0.4
                                                   );

                     m_config->getNavigation()->attachFeature( roiTE, recRoI, HLT::Navigation::ObjectCreatedByNew, key, "RecJetRoI" );
                     m_config->getNavigation()->attachFeature( roiTE, roID, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );

                     ATH_MSG_DEBUG("creating RoINode with attached RoIDescriptor with id: " << (roiId-1));

                     jetRoIsCount++;
                  }
               }
               else if (jetERoI->type() == JetEtRoI) {
                  roiTE = jetEtRoITE;
               }
               else if (jetERoI->type() == MissingOrTotalEtRoI) {
                  roiTE = metRoITE;
               }

            }
            if ( !roiTE ) {
               ATH_MSG_ERROR( "creating threshold Node with id: " << (*threshold)->hashId 
                              << " name: " << (*threshold)->name); 
            }
            m_config->getNavigation()->addNode( roiTE, (*threshold)->hashId );
            ATH_MSG_DEBUG("created threshold node: " 
                          <<  (*threshold)->name << " and id: " <<  (*threshold)->hashId);
	
         }
      }
   } // end of m_useL1JetEnergy
   if(m_doTiming) m_jetTime->stop();    

   if(m_doTiming) m_totalTime->stop();

   // if this is calibration event we do not apply RoI count cuts (meansy any busy event is just OK)
   if ( ! m_lvl1Tool->isCalibrationEvent(*result) ) {

      // check the limits    
      bool isAboveLimit = false;
      if ( m_muonRoIsLimit && muonRoIsCount        > m_muonRoIsLimit) {
         isAboveLimit = true;
         ATH_MSG_DEBUG("Busy event, passes limits of the MUON rois counts per event: "
                       << muonRoIsCount << " > " << m_muonRoIsLimit);
      }
    
      if ( m_emtauRoIsLimit && emtauRoIsCount      > m_emtauRoIsLimit) {
         isAboveLimit = true;
         ATH_MSG_DEBUG("Busy event, passes limits of the EMTAU rois counts per event: "
                       << emtauRoIsCount << " > " << m_emtauRoIsLimit);
      }
      if (m_jetRoIsLimit && jetRoIsCount          > m_jetRoIsLimit) {
         isAboveLimit = true;
         ATH_MSG_DEBUG("Busy event, passes limits of the JET rois counts per event: "
                       << jetRoIsCount << " > " << m_jetRoIsLimit);
      }

      unsigned overallRoIsCount= muonRoIsCount + emtauRoIsCount + jetRoIsCount;
      if ( m_overallRoIsLimit && overallRoIsCount > m_overallRoIsLimit ) {
         isAboveLimit = true;
         ATH_MSG_DEBUG("Busy event, passes limits of the all types rois counts per event: "
                       << overallRoIsCount << " > " << m_overallRoIsLimit);
      }
    
      if ( isAboveLimit ) {
         chains.clear(); // we do not want chains returned because PT chains will bias streaming
         return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::UNKNOWN, HLT::SteeringInternalReason::BUSY );
      }
   }

   // Check for L1Calo overflows
   std::bitset<3> overflow = m_lvl1Tool->lvl1EMTauJetOverflow(*result);
   std::vector<std::string> ignore;
   const char* thr[] = {"EM","TAU","JET"};
   for (size_t i=0; i<overflow.size(); ++i) {
     if (overflow[i]) {
       ATH_MSG_WARNING("All " << thr[i] << " L1 thresholds were forced on due to overflow in TOB transmission to CMX");     
       ignore.push_back(thr[i]);
     }
   }

   // Consistency check
   HLT::ErrorCode ecl1 = m_lvl1ConsistencyTool->check(items, m_config->getNavigation(), ignore);
   if ( ecl1  != HLT::OK ) {
      ATH_MSG_WARNING("Lvl1 decision inconsistent");
      return ecl1;
   }

   return HLT::OK;
}

void
Lvl1Converter::insertRoITEinMap(uint32_t roiWord, TriggerElement* te) {
   m_RoI2TE[roiWord] = te;
}


/*
 * Temporary
 */
double Lvl1Converter::fixphi(double phi) {
  if ( phi < -M_PI  || phi > M_PI ) {
    ATH_MSG_WARNING("Phi value from Lvl1 must be fixed (not in -pi +pi): " << phi);
    if ( phi < -M_PI )
      return phi + 2*M_PI;
    else
      return phi - 2*M_PI;
  }
  return phi;
}
