/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes:
#include <vector>

// Athena/Gaudi includes:
#include "AthenaMonitoring/IMonitorToolBase.h"


// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Package includes
#include "DeriveSimulationInputs.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_RIO.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigT1Result/RoIBResult.h"

// TrigConf includes
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TIP.h"
#include "TrigConfL1Data/Muctpi.h"

//Additional needs for CTP simulation
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/TrigT1CTPDefs.h"
#include "TrigT1CTP/CTPSimulation.h"

//Additional needs for reading muon input
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/MuCTPICTP.h"

//Additional needs for reading L1Calo input
#include "TrigT1CaloEvent/CMMCPHitsCollection.h"
#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"
#include "TrigT1CaloEvent/CMMEtSumsCollection.h"

// Inputs to the CTP:
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1Interfaces/EnergyCTP.h"
#include "TrigT1Interfaces/MbtsCTP.h"
#include "TrigT1Interfaces/BcmCTP.h"
#include "TrigT1Interfaces/TrtCTP.h"
#include "TrigT1Interfaces/LucidCTP.h"
#include "TrigT1Interfaces/ZdcCTP.h"
#include "TrigT1Interfaces/NimCTP.h"
#include "TrigT1Interfaces/BptxCTP.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"


#include "AthenaPoolUtilities/CondAttrListCollection.h"

using namespace std;


TrigT1CTMonitoring::DeriveSimulationInputs::DeriveSimulationInputs( const std::string& name, ISvcLocator* pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ), 
     m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name)
{
   declareProperty("TrigConfigSvc",       m_configSvc,     "Trigger Config Service");
   declareProperty("do_MuCTPI_input",     do_MuCTPI_input, "Flag to rederive MuCTPI input from raw data fragment");
   declareProperty("do_L1Calo_input",     do_L1Calo_input, "Flag to rederive L1Calo input from raw data fragment");
   declareProperty("do_L1Calo_sim",       do_L1Calo_sim,   "Flag to resimulate L1Calo input");
   declareProperty("AttrListFolders",     m_par_atrlist);
   declareProperty("AttrListCollFolders", m_par_atrcollist);
   declareProperty("N_cablebits",         N_cablebits);
   declareProperty("N_slots_ctpin",       N_slots_ctpin);
   declareProperty("N_connectors_ctpin",  N_connectors_ctpin);
   declareProperty("N_connectors_fp",     N_connectors_fp);
}


TrigT1CTMonitoring::DeriveSimulationInputs::~DeriveSimulationInputs()
{}
  
	
//======================================================================================================
	
	
StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::initialize() {
   ATH_MSG_DEBUG("initialize()");
   CHECK(m_configSvc.retrieve());
   if ( ! m_configSvc->ctpConfig() ) {
      ATH_MSG_WARNING("Unable to retrieve CTP configuration");
      return StatusCode::RECOVERABLE;
   }
   return StatusCode::SUCCESS;
}
	
	
	
//======================================================================================================
	

StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::beginRun(){
   ATH_MSG_DEBUG("beginRun()");
   StatusCode sc = ReadInputMappingFromCool();
   ATH_MSG_DEBUG("Read switch matrix of size " << m_pitVector.size() << " from cool ");
   return sc;
}



//======================================================================================================



StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::execute() {
    
   ATH_MSG_DEBUG("Executing DeriveSimulationInputs: CTP_RDO -> TIP pattern -> threshold multiplicity -> cable words (EMTAU, JET, MUON, etc)");
   ATH_MSG_DEBUG("requires correct TIP mapping from COOL (present at data taking)");
		
   if ( ! m_configSvc->ctpConfig() ) {
      ATH_MSG_WARNING("Unable to retrieve CTP configuration");
      return StatusCode::RECOVERABLE;
   }

   /**
    * reset CTPIN and FP cable arrays 
    **/

   // ctpin
   for (int bit = 0; bit < 31; bit++)
      for (int slot = 0; slot < 3; slot++)
         for (int conn = 0; conn < 4; conn++)
            CTPINCables[bit][slot][conn] = 0;
   
   // ctp core front panel reset
   for (int bit = 0; bit < 32; bit++)
      for (int conn = 0; conn < 3; conn++)
         for(int clock = 0; clock < 2; clock++)
            FPCables[bit][conn][clock] = 0;



   // get CTP RDO from storegate
   const CTP_RDO* theCTP_RDO = nullptr;
   CHECK( evtStore()->retrieve(theCTP_RDO, "CTP_RDO") );

   
   // we should not use the ctpVersion from the menu to guaranty consistency
   //unsigned int ctpVersionNumber = theCTP_RDO->getCTPVersionNumber(); 

   unsigned int ctpVersionNumber = m_configSvc->ctpConfig()->ctpVersion();
   ATH_MSG_DEBUG("Menu configuration with ctp version " << ctpVersionNumber);
		
   const std::vector < uint32_t > & cDataWords = theCTP_RDO->getDataWords();
   if (cDataWords.size() == 0) {
      ATH_MSG_INFO("CTP_RDO is empty, ignoring CTP");
      return StatusCode::SUCCESS;
   }
		
   uint32_t numberBC = theCTP_RDO->getNumberOfBunches();
   if (numberBC == 0) {
      ATH_MSG_INFO("CTP_RDO with 0 bunch crossings, ignoring CTP");
      return StatusCode::SUCCESS;
   }


   CTP_Decoder ctp;
   ctp.setRDO(theCTP_RDO);
   
   const std::vector<CTP_BC> & BCs = ctp.getBunchCrossings();

   std::bitset < 512 > currentTIP( BCs[ theCTP_RDO->getL1AcceptBunchPosition() ].getTIP() );
   

   if (ctpVersionNumber<=3) { // Run 1 CTP

      for (size_t tipNum = 0; tipNum < currentTIP.size(); ++tipNum) {

         if ( ! currentTIP.test(tipNum) ) continue;

         fillPitMap(tipNum).ignore();

      }

   } else { // Run 2 CTP

//       unsigned int LUTinputs = 16;
//       unsigned int nLut = currentTIP.size()/LUTinputs;

      for (size_t tipNum = 0; tipNum < currentTIP.size(); ++tipNum) {

         if ( ! currentTIP.test(tipNum) ) continue;

         //assume that LUT input 0..11 are PIT, 12..15 are FPI

         bool isPIT = (tipNum<320); // (false), isFPI(false);

         ATH_MSG_VERBOSE("TIP " << tipNum << " fired, type is " << (isPIT?"PIT":"FPI"));
					
         if (isPIT) {
            fillPitMap(tipNum).ignore();
         } else {
            fillFPIMap(tipNum).ignore();
         }
      }
   }

		
   ATH_MSG_DEBUG("filling store gate");
   StatusCode sc = fillStoreGate(ctpVersionNumber);
		
   ATH_MSG_DEBUG("end execute()");
   return sc;
}
  
	
//======================================================================================================

	
  
StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::fillPitMap(unsigned int pitNum) {
    
   ATH_MSG_DEBUG("Filling pit map with pitNum " << pitNum );
   if ( ! m_configSvc->ctpConfig() ) {
      ATH_MSG_WARNING("Unable to retrieve CTP configuration");
      return StatusCode::FAILURE;
   }


   for ( TrigConf::PIT * pit : m_pitVector ) {

      if (pit->pitNumber() != pitNum) continue;

      ATH_MSG_DEBUG("fillPitMap: PIT fired " << pit->pitNumber() << " " << pit->thresholdName());
      ATH_MSG_DEBUG("found cable slot = " << pit->ctpinSlot() << ", connector = " << pit->ctpinConnector() << ", bit = " << pit->cableBit());
         
      if (( pit->cableBit()+1 > (int)N_cablebits) ||
          ( pit->ctpinSlot()-6 > (int)N_slots_ctpin) ||  
          ( pit->ctpinConnector()+1 > (int)N_connectors_ctpin)) {
         ATH_MSG_WARNING("Cable outside allowed range");
         cout << pit->cableBit()+1 << "   " << N_cablebits << endl;
         cout << pit->ctpinSlot()-6 << "  " << N_slots_ctpin << endl;
         cout << pit->ctpinConnector()+1 << "  " << N_connectors_ctpin << endl;
         return StatusCode::RECOVERABLE;
      }
         
      CTPINCables[ pit->cableBit()][ pit->ctpinSlot()-7][ pit->ctpinConnector()] = 1;
      return StatusCode::SUCCESS;
   }

   return StatusCode::FAILURE;
}


//======================================================================================================
	
	
StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::fillFPIMap(unsigned int tipNum) {
   
   unsigned int cableBit = ((tipNum-320)%64)/2;
   unsigned int con = (tipNum-320)/64;
   unsigned int clock = (tipNum-320)%2;

   ATH_MSG_DEBUG("Filling FPI map with tipNum " << tipNum << " cable bit " << cableBit << ", conn " << con << " clock " << clock);
		
   if ( cableBit >=32 || con > 2 || clock>1 ) {
      ATH_MSG_WARNING("Cable outside allowed range");
      return StatusCode::RECOVERABLE;
   }
		
   FPCables[cableBit][con][clock] = 1;
		
   return StatusCode::SUCCESS;
}


//======================================================================================================

namespace {

   // return slot-7 for CTPIN
   unsigned int getSlot(const string & slot) { // slot : SLOT7 .. SLOT9 or CTPCORE
      if(slot=="CTPCORE") {
         return 10;
      }
      std::string::size_type pos = slot.find_last_of("T");
      const char* tmp(&slot[pos+1]);
      return std::atoi(tmp)-7;
   }

   // return connector for CTPIN and CTPCORE
   unsigned int getConnector(const string & connector) { // slot : CON0 .. CON3
      std::string::size_type pos = connector.find_last_of("N");
      const char* tmp = &connector[pos+1];
      return std::atoi(tmp);
   }



}

StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::fillStoreGate(unsigned int ctpVersionNumber) {
   StatusCode sc = StatusCode::SUCCESS;

   //cout << "JOERG in DeriveSimulationInputs::fillStoreGate ctp version " << ctpVersionNumber << endl;
    
   // dump the TIP info (which PIT and DIRECT inputs have fired)
   if(msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("Dumping filled cable array for CTPIN");
      for (unsigned int slot = 0; slot < N_slots_ctpin; slot++)  //want to store these numbers somewhere
         for (unsigned int conn = 0; conn < N_connectors_ctpin; conn++) //want to store these numbers somewhere
            for (unsigned int bit = 0; bit < N_cablebits; ++bit)   //want to store these numbers somewhere
               ATH_MSG_DEBUG(" Cable slot = " << slot+7 << ", connector = " << conn << ", cable bit = " << bit << "  -> " << CTPINCables[bit][slot][conn]);
      
      if (ctpVersionNumber>3) {
         ATH_MSG_DEBUG("Dumping filled cable array for CTPCORE front panel");
         for (unsigned int conn = 0; conn < 3; conn++)
            for (unsigned int bit = 0; bit < 32; bit++)
               for (unsigned int clock= 0; clock < 2; clock++)
                  ATH_MSG_DEBUG(" CTPCORE connector = " << conn << ", clock " << clock << ", cable bit = " << bit << "  -> " << FPCables[bit][conn][clock]);
      }
   }
		
   unsigned int muonCable = 0;
   unsigned int emtauCables0 = 0;
   unsigned int emtauCables1 = 0;
   unsigned int jetCables0 = 0;
   unsigned int jetCables1 = 0;
   unsigned int energyCables = 0;
   unsigned int bcmCables = 0;
   unsigned int bptxCables = 0;
   unsigned int lucidCables = 0;
   unsigned int zdcCables = 0;
   unsigned int mbtsCablesA = 0;
   unsigned int mbtsCablesC = 0;
   unsigned int nimCables0 = 0;
   unsigned int nimCables1 = 0;   
   unsigned int nimCables2 = 0;

   uint32_t calCable = 0;
   uint32_t emCables[2]  = {0,0};
   uint32_t tauCables[2] = {0,0};
   uint32_t jetCables[2] = {0,0};
   uint32_t enCables[2]  = {0,0};
   uint32_t nimCables[2] = {0,0};
   uint64_t fpCables[3]  = {0,0,0};
		




   for ( TrigConf::TriggerThreshold* thr : m_configSvc->ctpConfig()->menu().thresholdVector() ) {

      if(thr->ttype() == TrigConf::L1DataDef::BGRP || thr->ttype() == TrigConf::L1DataDef::RNDM) {
         continue;
      }

      uint64_t cable = 0;

      // finding slot and connector number and clock phase in case of direct inputs

      if ( ctpVersionNumber>3 && (thr->input()!="ctpin" || thr->cableCtpin()=="CTPCORE") ) { // front panel inputs (L1TOPO or ALFA)
         /*
           Direct inputs ALFA and TOPO
         */
         unsigned int connector = getConnector(thr->cableConnector()); // 0..2
         unsigned int clock = thr->clock(); // 0..1

         
         if ( connector > 2 || clock > 1 ) {
            ATH_MSG_WARNING("Invalid front panel connector " << connector);
            continue;
         }
				
         for (int cablebit = thr->cableStart(); cablebit <= thr->cableEnd(); cablebit++) {
            if ((cablebit > 31) || (cablebit < 0)) {
               ATH_MSG_WARNING("Cable bit outside range " << cablebit);
               continue;
            }
            //cable |= FPCables[cablebit][connector][clock] ? (1UL << (32 * clock + cablebit) ) : 0;
            cable |= FPCables[cablebit][connector][clock] ? (1UL << (2 * cablebit + clock) ) : 0; // if we want to go to "alternating pattern"
         }
         
         ATH_MSG_DEBUG( "Dumping TriggerThreshold " << thr->name() << " details: " << thr->cableName() 
                        << " " << thr->type() << " front panel " << thr->cableConnector() << " bit range " << thr->cableStart()
                        << " - " << thr->cableEnd() << ", clock " << thr->clock() << ", first pos " << (2 * thr->cableStart() + clock) );
         
      } else {
         /*
           CTPIN
         */
         unsigned int slot = getSlot(thr->cableCtpin()); // 0..2
         unsigned int connector = getConnector(thr->cableConnector()); // 0..3

         if ( slot > 2 || connector > 3 ) {
            ATH_MSG_WARNING("Invalid CTPIN connection with slot " << slot << " and connector " << connector << " for threshold " << thr->name() << "  and " << thr->cableCtpin());
            continue;
         }

         for (int cablebit = thr->cableStart(); cablebit <= thr->cableEnd(); cablebit++) {
            if ((cablebit > (int)N_cablebits) || (cablebit < 0)) {
               ATH_MSG_WARNING("cablebit out of range " << cablebit);
               continue;
            }
            if ( cablebit > 31 ) {
               ATH_MSG_WARNING("Invalid cablebit " << cablebit << " for threshold " << thr->name() << " and " << thr->cableCtpin());
               continue;
            }
            cable |= CTPINCables[cablebit][slot][connector] ? (1U << cablebit) : 0;
         }
         ATH_MSG_DEBUG("Dumping TriggerThreshold " << thr->name() << " details: " << thr->cableName() << " " << thr->type() << " " << thr->cableCtpin() << " " << thr->cableConnector() << " " << thr->cableStart() << " " << thr->cableEnd());
      }
      

      ATH_MSG_DEBUG("Cable " << cable );
			
      if(ctpVersionNumber<=3) {
         if ( thr->cableName() == "MU" )
            muonCable |= cable;
         else if (thr->cableName() == "CP1")
            emtauCables0 |= cable;
         else if (thr->cableName() == "CP2")
            emtauCables1 |= cable;
         else if (thr->cableName() == "JEP1")
            jetCables0 |= cable;
         else if (thr->cableName() == "JEP2")
            jetCables1 |= cable;
         else if (thr->cableName() == "JEP3")
            energyCables |= cable;
         else if (!thr->type().find("BCM"))
            bcmCables |= cable;
         else if (thr->type() == "LUCID")
            lucidCables |= cable;
         else if (thr->type() == "ZDC")
            zdcCables |= cable;
			
         //belof 4 are a hack but I don't think there is another way other than connecting threshold->pit
         else if ((thr->type().find("MBTS") != std::string::npos)&&(thr->cableConnector() == "CON0"))
            mbtsCablesA |= cable;
         else if ((thr->type().find("MBTS") != std::string::npos)&&(thr->cableConnector() == "CON1"))
            mbtsCablesC |= cable;
			
         else if ((thr->cableCtpin() == "SLOT8") && (thr->cableConnector() == "CON2") && (thr->cableStart() == 15))
            bptxCables |= cable;
         else if ((thr->cableCtpin() == "SLOT8") && (thr->cableConnector() == "CON2") && (thr->cableStart() == 16))
            bptxCables |= cable;
			
         else if (((thr->cableName() == "NIM") && (thr->type() == "NIM"))||
                  ((thr->cableName() == "NIM") && (thr->type() == "CALREQ"))){

            if ((thr->cableCtpin() == "SLOT8") && (thr->cableConnector() == "CON2"))
               nimCables0 |= cable;

            if ((thr->cableCtpin() == "SLOT9") && (thr->cableConnector() == "CON0"))
               nimCables1 |= cable;

            if ((thr->cableCtpin() == "SLOT9")&&(thr->cableConnector() == "CON1"))
               nimCables2 |= cable;

         }		
         else
            ATH_MSG_WARNING("Unrecognised cable type: " << thr->type());

      } else {

         // run 2
         if      (thr->cableName() == "EM1")      emCables[0]  |= cable;  // SLOT  7, CONN 0
         else if (thr->cableName() == "EM2")      emCables[1]  |= cable;  // SLOT  7, CONN 1
         else if (thr->cableName() == "TAU1")     tauCables[0] |= cable;  // SLOT  7, CONN 2
         else if (thr->cableName() == "TAU2")     tauCables[1] |= cable;  // SLOT  7, CONN 3
         else if (thr->cableName() == "JET1")     jetCables[0] |= cable;  // SLOT  8, CONN 0
         else if (thr->cableName() == "JET2")     jetCables[1] |= cable;  // SLOT  8, CONN 1
         else if (thr->cableName() == "EN1")      enCables[0]  |= cable;  // SLOT  8, CONN 2
         else if (thr->cableName() == "EN2")      enCables[1]  |= cable;  // SLOT  8, CONN 3
         else if (thr->cableName() == "MUCTPI")   muonCable    |= cable;  // SLOT  9, CONN 0
         else if (thr->cableName() == "CTPCAL")   calCable     |= cable;  // SLOT  9, CONN 1
         else if (thr->cableName() == "NIM1")     nimCables[0] |= cable;  // SLOT  9, CONN 2
         else if (thr->cableName() == "NIM2")     nimCables[1] |= cable;  // SLOT  9, CONN 3
         else if (thr->cableName() == "ALFA")     fpCables[0]  |= cable;  // SLOT 10, CONN 0
         else if (thr->cableName() == "TOPO1")    fpCables[1]  |= cable;  // SLOT 10, CONN 1
         else if (thr->cableName() == "TOPO2")    fpCables[2]  |= cable;  // SLOT 10, CONN 2
         else
            ATH_MSG_WARNING("Cable " << thr->cableName() << " with unrecognised cable type for run 2: " << thr->type());
      }

   }
    
   ATH_MSG_DEBUG("Recording CTP simulation items into storeGate");
    


   //only do this if we didn't do already directly from the RDO
   if (!do_MuCTPI_input) {
      LVL1::MuCTPICTP* newMuCTPI = new LVL1::MuCTPICTP(muonCable);
      sc = evtStore()->record(newMuCTPI, LVL1MUCTPI::DEFAULT_MuonCTPLocation);
      
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1MUCTPI::DEFAULT_MuonCTPLocation);
         return sc;
      }
   }
		
   if ((!do_L1Calo_input) && (!do_L1Calo_sim)) {

      LVL1::EmTauCTP* newEMTau(nullptr);
      ATH_MSG_DEBUG("Recording EmTauCTP to storegate " << LVL1::TrigT1CaloDefs::EmTauCTPLocation);
      if (ctpVersionNumber<=3) {
         newEMTau = new LVL1::EmTauCTP(emtauCables0, emtauCables1);
         ATH_MSG_DEBUG("    " << hex << emtauCables0 << dec);
         ATH_MSG_DEBUG("    " << hex << emtauCables1 << dec);
      } else {
         newEMTau = new LVL1::EmTauCTP( emCables[0], emCables[1], tauCables[0], tauCables[1] );
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << emCables[0]  << dec << " SLOT 7 / CONN 0 => EM1");
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << emCables[1]  << dec << " SLOT 7 / CONN 1 => EM2");
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << tauCables[0] << dec << " SLOT 7 / CONN 2 => TAU1");
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << tauCables[1] << dec << " SLOT 7 / CONN 3 => TAU2");
      }
      sc = evtStore()->record(newEMTau, LVL1::TrigT1CaloDefs::EmTauCTPLocation);
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::EmTauCTPLocation);
         return sc;
      }



      LVL1::JetCTP* newJet(nullptr);
      ATH_MSG_DEBUG("Recording JetCTP to storegate " << LVL1::TrigT1CaloDefs::JetCTPLocation);
      if (ctpVersionNumber<=3) {
         newJet = new LVL1::JetCTP(jetCables0, jetCables1);
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << jetCables0  << dec);
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << jetCables1  << dec);
      } else {
         newJet = new LVL1::JetCTP(jetCables[0], jetCables[1]);
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << jetCables[0]  << dec << " SLOT 8 / CONN 0 => JET1");
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << jetCables[1]  << dec << " SLOT 8 / CONN 1 => JET2");
      }
      sc = evtStore()->record(newJet, LVL1::TrigT1CaloDefs::JetCTPLocation);
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::JetCTPLocation);
         return sc;
      }
      
      LVL1::EnergyCTP* newEnergy(nullptr);
      ATH_MSG_DEBUG("Recording EnergyCTP to storegate " << LVL1::TrigT1CaloDefs::EnergyCTPLocation);
      if (ctpVersionNumber<=3) {
         newEnergy = new LVL1::EnergyCTP(energyCables);
      } else {
         newEnergy = new LVL1::EnergyCTP(enCables[0], enCables[1]);
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << enCables[0]  << dec << " SLOT 8 / CONN 2 => EN1");
         ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << enCables[1]  << dec << " SLOT 8 / CONN 3 => EN2");
      }
      sc = evtStore()->record(newEnergy, LVL1::TrigT1CaloDefs::EnergyCTPLocation);
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::EnergyCTPLocation);
         return sc;
      }
   }
   if (do_L1Calo_input)
      sc = DeriveL1CaloInput();
   if (do_L1Calo_sim)
      sc = DeriveL1CaloInput_fromSim();
		
   if (ctpVersionNumber>3) {
      uint32_t cableword0clock0( fpCables[0] & 0xffffffff );  // lower 32 bit
      uint32_t cableword0clock1( (fpCables[0] & 0xffffffff00000000) >> 32 );  // upper 32 bit
      uint32_t cableword1clock0( fpCables[1] & 0xffffffff );
      uint32_t cableword1clock1( (fpCables[1] & 0xffffffff00000000) >> 32 );
      uint32_t cableword2clock0( fpCables[2] & 0xffffffff );
      uint32_t cableword2clock1( (fpCables[2] & 0xffffffff00000000) >> 32 );
      LVL1::FrontPanelCTP* newFP = new LVL1::FrontPanelCTP( cableword0clock0, cableword0clock1,
                                                            cableword1clock0, cableword1clock1,
                                                            cableword2clock0, cableword2clock1);
      ATH_MSG_DEBUG(" Recording FrontPanel inputs to storegate " << LVL1::DEFAULT_L1TopoCTPLocation);
      ATH_MSG_DEBUG("    clock 0 0x" << hex << setfill('0') << setw(8) << cableword0clock0 << 
                    "    clock 1 0x" << setfill('0') << setw(8) << cableword0clock1 << dec << " SLOT 10 / CONN 0 => ALFA");
      ATH_MSG_DEBUG("    clock 0 0x" << hex << setfill('0') << setw(8) << cableword1clock0 <<
                    "    clock 1 0x" << setfill('0') << setw(8) << cableword1clock1 << dec << " SLOT 10 / CONN 1 => TOPO1");
      ATH_MSG_DEBUG("    clock 0 0x" << hex << setfill('0') << setw(8) << cableword2clock0 <<
                    "    clock 1 0x" << setfill('0') << setw(8) << cableword2clock1 << dec << " SLOT 10 / CONN 2 => TOPO2");
      sc = evtStore()->record(newFP, LVL1::DEFAULT_L1TopoCTPLocation); 
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_L1TopoCTPLocation);
         return sc;
      }
   }
		
   LVL1::MbtsCTP* newMBTSA{nullptr};
   ATH_MSG_DEBUG(" Recording MBTS_A to storegate " << LVL1::DEFAULT_MbtsACTPLocation);
   if (ctpVersionNumber<=3) {
      newMBTSA = new LVL1::MbtsCTP(mbtsCablesA);
   } else {
      newMBTSA = new LVL1::MbtsCTP(nimCables[0]);
      ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << nimCables[0]  << dec << " SLOT 9 / CONN 2 => NIM1");
   }
   sc = evtStore()->record(newMBTSA, LVL1::DEFAULT_MbtsACTPLocation); 
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_MbtsACTPLocation);
      return sc;
   }


		
   LVL1::MbtsCTP* newMBTSC{nullptr};
   ATH_MSG_DEBUG(" Recording MBTS_C to storegate " << LVL1::DEFAULT_MbtsCCTPLocation);
   if (ctpVersionNumber<=3) {
      newMBTSC = new LVL1::MbtsCTP(mbtsCablesC);
   } else {
      newMBTSC = new LVL1::MbtsCTP(nimCables[1]);
      ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << nimCables[1]  << dec << " SLOT 9 / CONN 2 => NIM2");
   }
   sc = evtStore()->record(newMBTSC, LVL1::DEFAULT_MbtsCCTPLocation);
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_MbtsCCTPLocation);
      return sc;
   }


   
   LVL1::BcmCTP* newBCM{nullptr};
   if (ctpVersionNumber<=3) {
      newBCM = new LVL1::BcmCTP(bcmCables);
   } else {
      newBCM = new LVL1::BcmCTP(calCable);
      ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << calCable  << dec << " SLOT 8 / CONN 3 => CTPCAL");
   }
   sc = evtStore()->record(newBCM, LVL1::DEFAULT_BcmCTPLocation);
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_BcmCTPLocation);
      return sc;
   }
    
   LVL1::LucidCTP* newLUCID{nullptr};
   if (ctpVersionNumber<=3) {
      newLUCID = new LVL1::LucidCTP(lucidCables);
   } else {
      newLUCID = new LVL1::LucidCTP(calCable);
      ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << calCable  << dec << " SLOT 8 / CONN 3 => CTPCAL");
   }
   sc = evtStore()->record(newLUCID, LVL1::DEFAULT_LucidCTPLocation);
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_LucidCTPLocation);
      return sc;
   }
    
   LVL1::ZdcCTP* newZDC{nullptr};
   if (ctpVersionNumber<=3) {
      newZDC = new LVL1::ZdcCTP(zdcCables);
   } else {
      newZDC = new LVL1::ZdcCTP(calCable);
      ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << calCable  << dec << " SLOT 8 / CONN 3 => CTPCAL");
   }
   sc = evtStore()->record(newZDC, LVL1::DEFAULT_ZdcCTPLocation); 
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_ZdcCTPLocation);
      return sc;
   }
		
   LVL1::BptxCTP* newBPTX = new LVL1::BptxCTP(bptxCables);
   if (ctpVersionNumber<=3) {
      newBPTX = new LVL1::BptxCTP(bptxCables);
   } else {
      newBPTX = new LVL1::BptxCTP(calCable);
      ATH_MSG_DEBUG("    0x" << hex << setfill('0') << setw(8) << calCable  << dec << " SLOT 8 / CONN 3 => CTPCAL");
   }
   sc = evtStore()->record(newBPTX, LVL1::DEFAULT_BptxCTPLocation); 
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_BptxCTPLocation);
      return sc;
   }
    


   //retrieve muctpi nim object if it is there
   LVL1::NimCTP* MuctpiNim(nullptr);
   if ( evtStore()->contains< LVL1::NimCTP >( LVL1::DEFAULT_NimCTPLocation ) ) {

      evtStore()->retrieve( MuctpiNim, LVL1::DEFAULT_NimCTPLocation );

      
      if(ctpVersionNumber <= 3) {
         // MUB and MUE only existed in Run1
         unsigned int muctpiNimCables1 = MuctpiNim->cableWord1();
         
         unsigned int nimBarrelBitMask = 1 << 29;//hard coded MUB position      
         //overwrite MUB bits with muctpi simulation value
         if ((muctpiNimCables1&nimBarrelBitMask) != (nimCables1&nimBarrelBitMask)) {
            ATH_MSG_INFO(" Data-simulation discrepancy in NIM MUB item. Overwriting bit mask. ");
            nimCables1 ^= nimBarrelBitMask;
         }
         
         unsigned int nimEndcapBitMask = 1<<30;//hard coded MUE position
         //overwite MUE bits with muctpi simulation value
         if ((muctpiNimCables1&nimEndcapBitMask) != (nimCables1&nimEndcapBitMask)){
            ATH_MSG_INFO(" Data-simulation discrepancy in NIM MUE item. Overwriting bit mask. ");
            nimCables1 ^= nimEndcapBitMask;
         }
      }
      //updating SG NIM information
      //WARNING!!! this object will be modified in SG
      LVL1::NimCTP* updateMuctpiNim = const_cast< LVL1::NimCTP* >(MuctpiNim);
      updateMuctpiNim->SetCableWord0(nimCables0);
      updateMuctpiNim->SetCableWord1(nimCables1);
      updateMuctpiNim->SetCableWord2(nimCables2);

   } else {

      // if muctpi sim has not already put something into SG
      // construct the NIM object from scratch and write to SG

      MuctpiNim = new LVL1::NimCTP();
      MuctpiNim->SetCableWord0(nimCables0);
      MuctpiNim->SetCableWord1(nimCables1);
      MuctpiNim->SetCableWord2(nimCables2);
			
      //writing SG NIM information 
      sc = evtStore()->record(MuctpiNim, LVL1::DEFAULT_NimCTPLocation); 
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_NimCTPLocation);
         return sc;
      }
   }
   return sc;
}


//======================================================================================================


StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::DeriveL1CaloInput() {
   StatusCode sc = StatusCode::SUCCESS;
    
   ATH_MSG_DEBUG("Rederiving L1 Calo input to CTP from hits info" );
		
   const CMMCPHitsCollection* CMMCPHits = nullptr;
   ATH_MSG_DEBUG("Retrieving CMMCP hits from storegate location "  << LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
   sc = evtStore()->retrieve(CMMCPHits, LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
    
   if (sc.isFailure()) {
      ATH_MSG_WARNING("No CMM CPHits found in TES at "  << LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
      return sc;
   }
		
   for ( CMMCPHitsCollection::const_iterator it_CMMCPHits=CMMCPHits->begin(); it_CMMCPHits!=CMMCPHits->end(); ++it_CMMCPHits ) {
      if (((*it_CMMCPHits)->dataID() == 19) && ((*it_CMMCPHits)->crate() == 3)){
         LVL1::EmTauCTP *newEMTau = new LVL1::EmTauCTP((*it_CMMCPHits)->HitWord0(), (*it_CMMCPHits)->HitWord1());
         ATH_MSG_DEBUG(" Recording EMTau word into storegate " << LVL1::TrigT1CaloDefs::EmTauCTPLocation);
         sc = evtStore()->record(newEMTau, LVL1::TrigT1CaloDefs::EmTauCTPLocation);
         if ( sc.isFailure() ) {
            ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::EmTauCTPLocation);
            return sc;
         }
      }
			
      const CMMJetHitsCollection* CMMJetHits = nullptr;
      sc = evtStore()->retrieve(CMMJetHits, LVL1::TrigT1CaloDefs::CMMJetHitsLocation);
      if (sc.isFailure()) {
         ATH_MSG_WARNING("No CMM JetHits found in TES at "  << LVL1::TrigT1CaloDefs::CMMJetHitsLocation);
         return sc;
      }
			
      unsigned int MainHits = 0;
      unsigned int FBHits = 0;
			
      for (CMMJetHitsCollection::const_iterator it_CMMJetHits = CMMJetHits->begin(); it_CMMJetHits != CMMJetHits->end(); ++it_CMMJetHits) {      
         if (((*it_CMMJetHits)->dataID() == 18) && ((*it_CMMJetHits)->crate() == 1)) 
            MainHits = (*it_CMMJetHits)->Hits();
				
         if (((*it_CMMJetHits)->dataID() == 21) && ((*it_CMMJetHits)->crate() == 1)){
            FBHits = (*it_CMMJetHits)->Hits();
         }
      }
			
      LVL1::JetCTP* newJet = new LVL1::JetCTP(MainHits, FBHits);
      sc = evtStore()->record(newJet, LVL1::TrigT1CaloDefs::JetCTPLocation);
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::JetCTPLocation);
         return sc;
      }
			
      const CMMEtSumsCollection* CMMEtSums = nullptr;
      sc = evtStore()->retrieve(CMMEtSums, LVL1::TrigT1CaloDefs::CMMEtSumsLocation);
      if (sc.isFailure()) {
         ATH_MSG_WARNING("No CMMEtSums found in TES at " << LVL1::TrigT1CaloDefs::CMMEtSumsLocation);
         return sc;
      }
			
      unsigned int MissingEtHits=0;
      unsigned int SumEtHits=0; 
			
      for (CMMEtSumsCollection::const_iterator it_CMMEtSums=CMMEtSums->begin(); it_CMMEtSums!=CMMEtSums->end(); ++it_CMMEtSums) {
         //either these are the wrong way round
         //or the order in which they are pushed back should be reversed and swap 4->8
         //either way gives correct bit mask
         if (((*it_CMMEtSums)->dataID() == 19) && ((*it_CMMEtSums)->crate() == 1)) 
            MissingEtHits = (*it_CMMEtSums)->Et();  
         if (((*it_CMMEtSums)->dataID() == 20) && ((*it_CMMEtSums)->crate() == 1)) 
            SumEtHits = (*it_CMMEtSums)->Et();
      }
			
      unsigned int EnergyHits=0;
      EnergyHits |= (MissingEtHits << 4);
      EnergyHits |= SumEtHits;
			
      LVL1::EnergyCTP* newEnergy = new LVL1::EnergyCTP(EnergyHits);
      sc = evtStore()->record(newEnergy, LVL1::TrigT1CaloDefs::EnergyCTPLocation);       
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::EnergyCTPLocation);
         return sc;
      }
   }
   return sc;
}    


//======================================================================================================



StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::DeriveL1CaloInput_fromSim() {
   StatusCode sc = StatusCode::SUCCESS;
    
   ATH_MSG_DEBUG("Rederiving L1 Calo input to CTP from L1Calo simulation output" );
   return sc;
}


//======================================================================================================



StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::ReadInputMappingFromCool() {
   StatusCode sc = StatusCode::SUCCESS;
   ATH_MSG_DEBUG("Reading CTP input mapping from COOL");  
		
   const CondAttrListCollection* atrlistcol;
   sc = detStore()->retrieve(atrlistcol,"/TRIGGER/LVL1/CTPCoreInputMapping");
		
   if (sc.isFailure()){
      ATH_MSG_INFO("Could not retrieve CTP input mapping from COOL, no PIT lines will be simulated ");
      return sc;
   }
		
   ATH_MSG_DEBUG("Reading CTP PIT mapping from COOL /TRIGGER/LVL1/CTPCoreInputMapping");

   for (CondAttrListCollection::const_iterator citr = atrlistcol->begin();citr != atrlistcol->end(); ++citr) {
      
      TrigConf::PIT* pit = new TrigConf::PIT();
      
      int TIPnumber = (int)(*citr).first;
      std::string ThresholdName((((*citr).second)["ThresholdName"]).data<std::string>());
      int CtpinSlot = (int)(((*citr).second)["CtpinSlot"]).data<unsigned char>();  
      int CtpinConnector = (int)(((*citr).second)["CtpinConnector"]).data<unsigned char>();  
      int CableBit = (int)(((*citr).second)["CableBit"]).data<unsigned char>();  

      // test fix
      //       if(true) { // TODO
      //          int newTIPnumber=0;
      //          if(TIPnumber<160) {
      //             newTIPnumber = 2 * TIPnumber;
      //          } else if(TIPnumber<320) {
      //             newTIPnumber = 2 * TIPnumber - 319;
      //          } else {
      //             newTIPnumber = TIPnumber;
      //          }
      //          TIPnumber = newTIPnumber;
      //       }
      
      pit->setPitNumber(TIPnumber);
      pit->setThresholdName(ThresholdName);
      pit->setCtpinSlot(CtpinSlot);
      pit->setCtpinConnector(CtpinConnector);
      pit->setCableBit(CableBit);

      ATH_MSG_DEBUG("TIP number " << TIPnumber << ", thr " << ThresholdName << " slot " << CtpinSlot << " conn " << CtpinConnector << " cable bit " << CableBit);
      
      m_pitVector.push_back(pit);
   }
   return sc;
}


//======================================================================================================

StatusCode
TrigT1CTMonitoring::DeriveSimulationInputs::finalize() {
   StatusCode sc = StatusCode::SUCCESS;
   ATH_MSG_INFO("Finalizing " << name() << " - package version " << PACKAGE_VERSION);
   return sc;
}


//======================================================================================================



