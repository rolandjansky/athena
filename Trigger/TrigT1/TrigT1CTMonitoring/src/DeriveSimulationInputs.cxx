/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes:
#include <vector>

// Athena/Gaudi includes:
#include "StoreGate/StoreGateSvc.h"
#include "AthenaMonitoring/IMonitorToolBase.h"

// Trigger configuration interface includes:
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Package includes
#include "TrigT1CTMonitoring/DeriveSimulationInputs.h"
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
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfStorage/TrigConfCoolFolderSpec.h"
#include "TrigConfStorage/DBLoader.h"

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

// COOL includes
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoralBase/Blob.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"


namespace TrigT1CTMonitoring {

  DeriveSimulationInputs::DeriveSimulationInputs( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), 
      m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name)
  {
    declareProperty("TrigConfigSvc", m_configSvc, "Trigger Config Service");
    declareProperty("do_MuCTPI_input", do_MuCTPI_input=0,
		    "Flag to rederive MuCTPI input from raw data fragment");
    declareProperty("do_L1Calo_input", do_L1Calo_input=0,
		    "Flag to rederive L1Calo input from raw data fragment");
    declareProperty("do_L1Calo_sim", do_L1Calo_sim=0,
		    "Flag to resimulate L1Calo input");
    declareProperty("AttrListFolders",par_atrlist);
    declareProperty("AttrListCollFolders",par_atrcollist);
    declareProperty("N_cablebits", N_cablebits=31);
    declareProperty("N_slots", N_slots=3);
    declareProperty("N_connectors", N_connectors=4);
  }
  
  StatusCode DeriveSimulationInputs::initialize() {
    ATH_MSG_DEBUG("begin initialize()");
    
    if (StatusCode::SUCCESS!=m_configSvc.retrieve()){
      ATH_MSG_FATAL("Unable to get pointer to TrigConfigSvc");
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode DeriveSimulationInputs::beginRun(){
    StatusCode sc=StatusCode::SUCCESS;
    ATH_MSG_DEBUG("beginRun()");
    sc = ReadPitConfigFromCool();
    ATH_MSG_DEBUG("Read switch matrix of size " << m_pitVector.size() << " from cool ");
    return sc;
  }

  
  StatusCode DeriveSimulationInputs::execute() {
    
    StatusCode sc = StatusCode::SUCCESS;
    ATH_MSG_DEBUG("Executing DeriveSimulationInputs algorithm");

    const DataHandle < CTP_RDO > theCTP_RDO = 0;
    const DataHandle < CTP_RIO > theCTP_RIO = 0;
    const DataHandle < MuCTPI_RDO > theMuCTPI_RDO = 0;

    for (int i = 0; i < N_cablebits; i++)
      for (int j = 0; j < N_slots; j++)
	for (int k = 0; k < N_connectors; k++)
	  Cables[i][j][k]=0;

    sc = evtStore()->retrieve(theCTP_RDO, "CTP_RDO");
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not find \"CTP_RDO\" in StoreGate");
      return sc;
    }

    sc = evtStore()->retrieve(theCTP_RIO, "CTP_RIO");
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not find \"CTP_RIO\" in StoreGate");
      return sc;
    }
 
    sc = evtStore()->retrieve(theMuCTPI_RDO, "MUCTPI_RDO");
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not find \"MUCTPI_RDO\" in StoreGate");
      return sc;
    }

    const std::vector < uint32_t > &cDataWords = theCTP_RDO->getDataWords();
    if (cDataWords.size() == 0) {
      ATH_MSG_INFO("CTP_RDO is empty, ignoring CTP");
      return sc;
    }

    CTP_Decoder ctp;
    ctp.setRDO(theCTP_RDO);

    uint32_t numberBC = theCTP_RDO->getNumberOfBunches();
    if (numberBC <= 0) return sc;

    const std::vector < CTP_BC >& BCs = ctp.getBunchCrossings();
    std::bitset < 160 > currentPIT(BCs[theCTP_RDO->getL1AcceptBunchPosition()].getPIT());
    
    for (size_t pitNum = 0; pitNum < currentPIT.size(); ++pitNum) {
      if (currentPIT.test(pitNum))
	sc = fillPitMap(pitNum, m_pitVector);
    }

    ATH_MSG_DEBUG("now go to fill store gate");
    sc = fillStoreGate();

    //example shown in L1CaloCTPMon where they make this comparison 
    //information stored in L1Calo raw data fragment

    ATH_MSG_DEBUG("end execute()");
    return sc;
  }
  
  
  StatusCode DeriveSimulationInputs::fillPitMap(int pitNum, std::vector<TrigConf::PIT*> m_pitVector) {
    
    StatusCode sc = StatusCode::SUCCESS;
 
    ATH_MSG_DEBUG("Filling pit map");
    if ( m_configSvc->ctpConfig() ){
      //for ( std::vector< TrigConf::PIT* >::const_iterator pit = m_configSvc->ctpConfig()->menu()->pitVector().begin(); pit != m_configSvc->ctpConfig()->menu()->pitVector().end(); ++pit ) { //doesn't exist from XML
      for ( std::vector< TrigConf::PIT* >::const_iterator pit = m_pitVector.begin(); pit != m_pitVector.end(); ++pit ) { 
	const int pitNum_config = (*pit)->pitNumber();
	if (pitNum_config == pitNum){
	  ATH_MSG_DEBUG("PIT fired: " << (*pit)->pitNumber() << " " << (*pit)->thresholdName());
	  ATH_MSG_DEBUG("found cable bit");
				      ATH_MSG_DEBUG("slot = " << (*pit)->ctpinSlot() << " connector = " << (*pit)->ctpinConnector() << " cable = " << (*pit)->cableBit());
	  
	  if (((*pit)->cableBit()+1 > N_cablebits) ||
	      ((*pit)->ctpinSlot()-6 > N_slots) ||
	      ((*pit)->ctpinConnector()+1 > N_connectors)) {
	    ATH_MSG_INFO("null pit connection");
	    sc = StatusCode::FAILURE;
	    return sc;
	  }

	  Cables[(*pit)->cableBit()][(*pit)->ctpinSlot()-7][(*pit)->ctpinConnector()] = 1;
	  ATH_MSG_DEBUG("filled cable bit");
	  return sc;
	}
      }
    }
    else {
      ATH_MSG_WARNING("Unable to retrive CTP configuration");
      sc = StatusCode::FAILURE;
      return sc;
    }
    sc = StatusCode::FAILURE;
    return sc;
  }
  
  StatusCode DeriveSimulationInputs::fillStoreGate() {
    StatusCode sc = StatusCode::SUCCESS;
    
    ATH_MSG_DEBUG("Dumping filled cable array");

    for (int i = 0; i < N_cablebits; i++) //want to store these numbers somewhere
      for (int j = 0; j < N_slots; j++) //want to store these numbers somewhere
	for (int k = 0; k < N_connectors; k++)//want to store these numbers somewhere
	ATH_MSG_DEBUG(" Cable = " << i << 
		      " Slot = " << j+7 << 
		      " Connector = " << k << 
		      " Cable bit = " << Cables[i][j][k]);
    
    unsigned int muonCables = 0;
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
    
    for ( std::vector< TrigConf::TriggerThreshold* >::const_iterator it = m_configSvc->ctpConfig()->menu().thresholdVector().begin(); 
         it != m_configSvc->ctpConfig()->menu().thresholdVector().end(); ++it){

      std::string s((*it)->cableCtpin());
      std::string::size_type pos = s.find_last_of("T");
      char* dummy(&s[pos+1]);
      int SlotNumber = std::atoi(dummy)-7;
      s = (*it)->cableConnector();
      pos = s.find_last_of("N");
      dummy = &s[pos+1];
      int Connector = std::atoi(dummy);
      
      if ((SlotNumber < 0) || (Connector < 0) || (SlotNumber > N_slots) || (Connector > N_connectors)) {
	ATH_MSG_DEBUG("Null connection " << SlotNumber << " " << Connector);
	continue;
      }

      int cable = 0;
      for (int cablebit = (*it)->cableStart(); cablebit <= (*it)->cableEnd(); cablebit++) {
	if ((cablebit > N_cablebits) || (cablebit < 0)) {
	  ATH_MSG_DEBUG("cablebit out of range " << cablebit);
	  continue;
	}
	cable |= Cables[cablebit][SlotNumber][Connector] ? (1 << cablebit) : 0;
      }
      
      ATH_MSG_DEBUG("Dumping TriggerThreshold: " << (*it)->cableName() << " " << (*it)->type() << " " << (*it)->cableCtpin() << " " << (*it)->cableConnector() << " " << (*it)->cableStart() << " " << (*it)->cableEnd());
          
      if ((*it)->cableName() == "MU")
	muonCables |= cable;
      else if ((*it)->cableName() == "CP1")
      	emtauCables0 |= cable;
      else if ((*it)->cableName() == "CP2")
	emtauCables1 |= cable;
      else if ((*it)->cableName() == "JEP1")
	jetCables0 |= cable;
      else if ((*it)->cableName() == "JEP2")
	jetCables1 |= cable;
      else if ((*it)->cableName() == "JEP3")
	energyCables |= cable;
      else if (!(*it)->type().find("BCM"))
	bcmCables |= cable;
      else if ((*it)->type() == "LUCID")
	lucidCables |= cable;
      else if ((*it)->type() == "ZDC")
	zdcCables |= cable;

      //belof 4 are a hack but I don't think there is another way other than connecting threshold->pit
      else if (((*it)->type().find("MBTS") != std::string::npos)&&((*it)->cableConnector() == "CON0"))
	mbtsCablesA |= cable;
      else if (((*it)->type().find("MBTS") != std::string::npos)&&((*it)->cableConnector() == "CON1"))
	mbtsCablesC |= cable;

      else if (((*it)->cableCtpin() == "SLOT8") && ((*it)->cableConnector() == "CON2") && ((*it)->cableStart() == 15))
	bptxCables |= cable;
      else if (((*it)->cableCtpin() == "SLOT8") && ((*it)->cableConnector() == "CON2") && ((*it)->cableStart() == 16))
	bptxCables |= cable;

      else if ((((*it)->cableName() == "NIM") && ((*it)->type() == "NIM"))||
	       (((*it)->cableName() == "NIM") && ((*it)->type() == "CALREQ"))){
	if (((*it)->cableCtpin() == "SLOT8") && ((*it)->cableConnector() == "CON2"))
	  nimCables0 |= cable;
	if (((*it)->cableCtpin() == "SLOT9") && ((*it)->cableConnector() == "CON0"))
	  nimCables1 |= cable;
	if (((*it)->cableCtpin() == "SLOT9")&&((*it)->cableConnector() == "CON1"))
	  nimCables2 |= cable;
      }

      else
	ATH_MSG_WARNING("Unrecognised cable type: " << (*it)->type());
    }
    
    ATH_MSG_DEBUG("Recording CTP simulation items into storeGate");
    
    //only do this if we didn't do already directly from the RDO
    if (!do_MuCTPI_input) {
      LVL1::MuCTPICTP* newMuCTPI = new LVL1::MuCTPICTP(muonCables);
      sc = evtStore()->record(newMuCTPI, LVL1MUCTPI::DEFAULT_MuonCTPLocation);
      
      if ( sc.isFailure() ) {
	ATH_MSG_ERROR(" could not register object " << LVL1MUCTPI::DEFAULT_MuonCTPLocation);
	return sc;
      }
    }

    if ((!do_L1Calo_input) && (!do_L1Calo_sim)) {
      LVL1::EmTauCTP* newEMTau = new LVL1::EmTauCTP(emtauCables0, emtauCables1);
      ATH_MSG_DEBUG(" Recording EMTau word into storegate " << LVL1::TrigT1CaloDefs::EmTauCTPLocation);
      sc = evtStore()->record(newEMTau, LVL1::TrigT1CaloDefs::EmTauCTPLocation);
      if ( sc.isFailure() ) {
	ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::EmTauCTPLocation);
	return sc;
      }
      
      LVL1::JetCTP* newJet = new LVL1::JetCTP(jetCables0, jetCables1);
      sc = evtStore()->record(newJet, LVL1::TrigT1CaloDefs::JetCTPLocation);
      if ( sc.isFailure() ) {
	ATH_MSG_ERROR(" could not register object " << LVL1::TrigT1CaloDefs::JetCTPLocation);
	return sc;
      }
      
      LVL1::EnergyCTP* newEnergy = new LVL1::EnergyCTP(energyCables);
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

    
    LVL1::MbtsCTP* newMBTSA = new LVL1::MbtsCTP(mbtsCablesA);
    ATH_MSG_DEBUG(" Recording MBTS_A to storegate " << LVL1::DEFAULT_MbtsACTPLocation);
    sc = evtStore()->record(newMBTSA, LVL1::DEFAULT_MbtsACTPLocation); 
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_MbtsACTPLocation);
      return sc;
    }

    LVL1::MbtsCTP* newMBTSC = new LVL1::MbtsCTP(mbtsCablesC);
    ATH_MSG_DEBUG(" Recording MBTS_C to storegate " << LVL1::DEFAULT_MbtsCCTPLocation);
    sc = evtStore()->record(newMBTSC, LVL1::DEFAULT_MbtsCCTPLocation);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_MbtsCCTPLocation);
      return sc;
    }
    
    LVL1::BcmCTP* newBCM = new LVL1::BcmCTP(bcmCables);
    sc = evtStore()->record(newBCM, LVL1::DEFAULT_BcmCTPLocation);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_BcmCTPLocation);
      return sc;
    }
    
    LVL1::LucidCTP* newLUCID = new LVL1::LucidCTP(lucidCables);
    sc = evtStore()->record(newLUCID, LVL1::DEFAULT_LucidCTPLocation);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_LucidCTPLocation);
      return sc;
    }
    
    LVL1::ZdcCTP* newZDC = new LVL1::ZdcCTP(zdcCables);
    sc = evtStore()->record(newZDC, LVL1::DEFAULT_ZdcCTPLocation); 
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_ZdcCTPLocation);
      return sc;
    }
        
    LVL1::BptxCTP* newBPTX = new LVL1::BptxCTP(bptxCables);
    sc = evtStore()->record(newBPTX, LVL1::DEFAULT_BptxCTPLocation); 
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR(" could not register object " << LVL1::DEFAULT_BptxCTPLocation);
      return sc;
    }
    
    //retrieve muctpi nim object if it is there
    LVL1::NimCTP* MuctpiNim;
    if ( evtStore()->contains< LVL1::NimCTP >( LVL1::DEFAULT_NimCTPLocation ) ) {
      evtStore()->retrieve( MuctpiNim, LVL1::DEFAULT_NimCTPLocation );
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

      //updating SG NIM information
      //WARNING!!! this object will be modified in SG
      LVL1::NimCTP* updateMuctpiNim = const_cast< LVL1::NimCTP* >(MuctpiNim);
      updateMuctpiNim->SetCableWord0(nimCables0);
      updateMuctpiNim->SetCableWord1(nimCables1);
      updateMuctpiNim->SetCableWord2(nimCables2);
    }
    
    //if muctpi sim has not already put something into SG
    //construct the NIM object from scratch and write to SG
    else {
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
  
  StatusCode DeriveSimulationInputs::DeriveL1CaloInput(){
    StatusCode sc = StatusCode::SUCCESS;
    
    ATH_MSG_DEBUG("Rederiving L1 Calo input to CTP from hits info" );

    const CMMCPHitsCollection* CMMCPHits;
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
    
    const CMMJetHitsCollection* CMMJetHits;
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
        
    const CMMEtSumsCollection* CMMEtSums;
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

  StatusCode DeriveSimulationInputs::DeriveL1CaloInput_fromSim(){
    StatusCode sc = StatusCode::SUCCESS;
    
    ATH_MSG_DEBUG("Rederiving L1 Calo input to CTP from L1Calo simulation output" );
    return sc;
  }
  
  StatusCode DeriveSimulationInputs::ReadPitConfigFromCool(){
    StatusCode sc = StatusCode::SUCCESS;
    ATH_MSG_DEBUG("Trying to access CTP configuration from COOL");  

    const CondAttrListCollection* atrlistcol;
    sc = detStore()->retrieve(atrlistcol,"/TRIGGER/LVL1/CTPCoreInputMapping");

    if (sc.isFailure()){
      ATH_MSG_INFO("Could not retrieve CTP configuration from COOL, no PIT lines will be simulated ");
      return sc;
    }

    for (CondAttrListCollection::const_iterator citr = atrlistcol->begin();citr != atrlistcol->end(); ++citr) {
      ATH_MSG_DEBUG("Dumping CTP configuration from COOL");  

      TrigConf::PIT* pit = new TrigConf::PIT();
      
      int PitBit = (int)(*citr).first;
      std::string ThresholdName((((*citr).second)["ThresholdName"]).data<std::string>());
      int CtpinSlot = (int)(((*citr).second)["CtpinSlot"]).data<unsigned char>();  
      int CtpinConnector = (int)(((*citr).second)["CtpinConnector"]).data<unsigned char>();  
      int CableBit = (int)(((*citr).second)["CableBit"]).data<unsigned char>();  

      pit->setPitNumber(PitBit);
      pit->setThresholdName(ThresholdName);
      pit->setCtpinSlot(CtpinSlot);
      pit->setCtpinConnector(CtpinConnector);
      pit->setCableBit(CableBit);
      
      m_pitVector.push_back((TrigConf::PIT*) pit);
    }
    return sc;
  }


  StatusCode DeriveSimulationInputs::finalize() {
    StatusCode sc = StatusCode::SUCCESS;
    ATH_MSG_INFO("Finalizing " << name() << " - package version " << PACKAGE_VERSION);
    return sc;
  }
  
  DeriveSimulationInputs::~DeriveSimulationInputs() {  
  }

} //namespace TrigT1CTMonitoring


