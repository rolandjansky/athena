/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL
#include <sstream>
#include <fstream>
#include <vector>

// TGC
#include "TrigT1TGC/LVL1TGCTrigger.h"
#include "TrigT1TGC/TGCASDOut.h"
#include "TrigT1TGC/TGCEvent.h"
#include "TrigT1TGC/TGCReadoutIndex.h"
#include "TrigT1TGC/TGCSLSelectorOut.hh"
#include "TrigT1TGC/TGCElectronicsSystem.hh"
#include "TrigT1TGC/TGCTimingManager.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "TrigT1TGC/TGCSector.hh"
#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TrigT1TGC_ClassDEF.h"
#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCCoincidence.hh"

// Tile-Muon
#include "TileEvent/TileMuContainer.h"
#include "TileEvent/TileMuonReceiverObj.h"


// Athena/Gaudi
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

// Other stuff
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/Lvl1MuEndcapSectorLogicData.h"
#include "TrigT1Interfaces/Lvl1MuForwardSectorLogicData.h"

// MuonSpectrometer
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "PathResolver/PathResolver.h"

// Debug flag
//#define TGCDEBUG 1

using namespace TrigConf;

namespace LVL1TGCTrigger {
  bool g_STRICTWD;
  bool g_STRICTWT;
  bool g_STRICTSD;
  bool g_STRICTST;
  bool g_OUTCOINCIDENCE;
  TGCCoincidences * g_TGCCOIN;
  bool g_SINGLEBEAM;
  bool g_MUHALO;
  bool g_ISATLAS;
  bool g_DEBUGLEVEL;
  bool g_SHPT_ORED;
  bool g_USE_INNER;
  bool g_INNER_VETO;
  bool g_FULL_CW;
  bool g_TILE_MU;

///////////////////////////////////////////////////////////////////////////
  LVL1TGCTrigger::LVL1TGCTrigger::LVL1TGCTrigger(const std::string& name, ISvcLocator* pSvcLocator):
    Algorithm(name,pSvcLocator),
    m_sgSvc("StoreGateSvc", name),
    m_detectorStore(0), 
    m_cabling(0),
    db(0),
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc",name),
    TimingManager(0),
    system(0),
    nEventInSector(0),
    isAtlas(true),
    m_log( msgSvc(), name ),
    m_debuglevel(false)
{
    declareProperty("EventStore", m_sgSvc, "Event Store"); 
    declareProperty("MuCTPIInput_TGC",     m_keyMuCTPIInput_TGC="/Event/L1MuctpiStoreTGC");
    declareProperty("InputData_perEvent",  m_keyTgcDigit="TGC_DIGITS");
    declareProperty("TileMuRcv_Input",     m_keyTileMu="TileMuRcvCnt");
    declareProperty("CurrentBunchTag",     m_CurrentBunchTag=TgcDigit::BC_CURRENT);
    declareProperty("OutputTgcRDO",        m_OutputTgcRDO=true);
    declareProperty("MaskFileName",        m_MaskFileName="");
    declareProperty("MaskFileName12",      m_MaskFileName12="");
    declareProperty("LVL1ConfigSvc",       m_configSvc, "LVL1 Config Service");
    declareProperty("VersionCW",           m_VerCW="setM");
    declareProperty("STRICTWD",            m_STRICTWD            =false);
    declareProperty("STRICTWT",            m_STRICTWT            =false);
    declareProperty("STRICTSD",            m_STRICTSD            =false);
    declareProperty("STRICTST",            m_STRICTST            =false);
    declareProperty("OUTCOINCIDENCE",      m_OUTCOINCIDENCE      =false);
    declareProperty("SINGLEBEAM",          m_SINGLEBEAM          =false);
    declareProperty("MUHALO",              m_MUHALO              =false);
    declareProperty("SHPTORED",            m_SHPTORED            =true);
    declareProperty("USEINNER",            m_USEINNER            =true);
    declareProperty("INNERVETO",           m_INNERVETO           =true);
    declareProperty("FULLCW",              m_FULLCW              =false);
    declareProperty("TILEMU",              m_TILEMU              =false);
  }


////////////////////////////////////////////////////////////
  LVL1TGCTrigger::~LVL1TGCTrigger()
  {
    m_log << MSG::DEBUG << "LVL1TGCTrigger destructor called" << endreq;
    // delete TimingManager ,ElectronicsSystem and Database
    if (TimingManager){
      delete TimingManager;
      TimingManager =0;
    }
    if (system) {
      delete system;
      system = 0;
    }
    if (db) {
      delete db;
      db =0;
    }

  }


////////////////////////////////////////////////////////////
  StatusCode LVL1TGCTrigger::initialize()
  {
    // init message stram
    m_log.setLevel(outputLevel());  // inidividual outputlevel not known before initialize
    m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug

    g_DEBUGLEVEL          =  m_debuglevel;

    m_log << MSG::DEBUG << "LVL1TGCTrigger::initialize() called" << endreq;
    m_log << MSG::INFO  << "LVL1TGCTrigger initialize" << endreq;

    g_STRICTWD            = m_STRICTWD.value();
    g_STRICTWT            = m_STRICTWT.value();
    g_STRICTSD            = m_STRICTSD.value();
    g_STRICTST            = m_STRICTST.value();
    g_OUTCOINCIDENCE      = m_OUTCOINCIDENCE.value();
    g_SINGLEBEAM          = m_SINGLEBEAM.value();
    g_MUHALO              = m_MUHALO.value();
    g_SHPT_ORED           = m_SHPTORED.value();
    g_USE_INNER           = m_USEINNER.value() && isAtlas;
    g_INNER_VETO          = m_INNERVETO.value() && g_USE_INNER;
    g_FULL_CW             = m_FULLCW.value();
    g_TILE_MU             = m_TILEMU.value();
    g_ISATLAS             = isAtlas;

    if ( m_SINGLEBEAM.value() ) {
      m_log << MSG::INFO  << "CoincidenceMatrix for SINGLE BEAM will be used" << endreq;
    }
    if ( m_MUHALO.value() ) {
      m_log << MSG::INFO  << "CoincidenceMatrix for TGC_MU_HALO will be used for pt=1" << endreq;
    }
  
    if (g_USE_INNER) {
      std::string vmode="MONITOR";
      if (g_INNER_VETO) vmode="SURPRESS";
      m_log << MSG::INFO  << "CoincidenceMatrix for EI/FI will be used "
	    << "[ " << vmode << " mode ]" << endreq;
    }
    if (g_TILE_MU) {
      m_log << MSG::INFO  << "CoincidenceMatrix for TileMu will be used "
	    << endreq;
    }

    StatusCode sc = service("DetectorStore", m_detectorStore);
    if (sc.isFailure()){
      m_log << MSG::FATAL
	    << "Unable to get pointer to DetectorStore Service" << endreq;
      return StatusCode::FAILURE;
    } else {
      m_log << MSG::INFO << "Could find DetectorStore" << endreq;
    }

    // TrigConfigSvc
    sc = m_configSvc.retrieve();
    if (sc.isFailure()) {
      m_log << MSG::ERROR << "Could not connect to " << m_configSvc.typeAndName() << endreq;
    }
    else {
      m_log << MSG::DEBUG << "Connected to " << m_configSvc.typeAndName() << endreq; 
    }
    
    // clear Masked channel
    m_MaskedChannel.clear();
      
     // StoreGate
    //sc = service("StoreGateSvc", m_sgSvc);
    sc = m_sgSvc.retrieve();
    if (sc.isFailure()) {
      m_log << MSG::FATAL << "Could not find StoreGateSvc" << endreq;
      return StatusCode::FAILURE;
    } else {
      if (m_debuglevel) {
	m_log << MSG::DEBUG << "Could find StoreGateSvc" << endreq;
      }
    }

    g_TGCCOIN=0;
    if (g_OUTCOINCIDENCE) g_TGCCOIN = new TGCCoincidences();

    // For your information
    if (m_debuglevel) {
      if (m_CurrentBunchTag>0) {
	m_log << MSG::DEBUG 
	      << "---> Take hits with CURRENT banch tag = " << m_CurrentBunchTag << endreq;
      }
      m_log << MSG::DEBUG << "OutputRdo " << m_OutputTgcRDO.value() << endreq;
    }
    
    // try to initialize the TGCcabling
    sc = getCabling();
    if(sc.isFailure()) {
      m_log << MSG::INFO 
          << "TGCcablingServerSvc not yet configured; postone TGCcabling initialization at first event. "
	  << endreq;
    }

    return StatusCode::SUCCESS;
  }

////////////////////////////////////////////////
  StatusCode LVL1TGCTrigger::finalize()
  {
    if (m_debuglevel) {
      m_log << MSG::DEBUG << "LVL1TGCTrigger::finalize() called" 
	    <<  " nEventInSector = " << nEventInSector << endreq;
    }

    // clear and delete TGCCOIN
    if (g_OUTCOINCIDENCE) {
      if (g_TGCCOIN->size()) {
	for(std::vector<TGCCoincidence*>::iterator iss=g_TGCCOIN->begin(); iss!=g_TGCCOIN->end(); iss++)
	  delete (*iss);
      }
      g_TGCCOIN->clear();
    }
    delete g_TGCCOIN;
    g_TGCCOIN = 0;

    if (db) delete db;
    db = 0 ;
    if (system) delete system;
    system = 0;
    if (TimingManager) delete TimingManager;
    TimingManager=0;

    return StatusCode::SUCCESS;
  }


////////////////////////////////////////////
  StatusCode LVL1TGCTrigger::execute()
  {
    if (m_debuglevel) {
      m_log << MSG::DEBUG << "LVL1TGCTrigger::execute() called" << endreq;
    }

    if(!m_cabling) {
      // get cabling svc
      if(getCabling().isFailure()) return StatusCode::FAILURE;
    }

    // doMaskOperation is performed at the first event 
    // It is better to implement callback against 
    // MuonTGC_CablingSvc::updateCableASDToPP (Susumu Oda, 2010/10/27)
    static bool firstTime = true;
    if(firstTime) {
      // do mask operation
      if(doMaskOperation().isFailure()) return StatusCode::FAILURE;
      firstTime = false;
    }

    StatusCode sc = StatusCode::SUCCESS;
    // Tile Mu Data
    if (g_TILE_MU) {
      sc = fillTMDB();
      if (sc.isFailure()) {
	m_log << MSG::WARNING 
	      << "Cannot retrieve Tile Mu Data " << endreq;
      return StatusCode::SUCCESS;
      }
    }
 
    // TgcRdo
    m_tgcrdo.clear();
    const TgcRdoContainer * rdoCont;
    sc = m_sgSvc->retrieve( rdoCont, "TGCRDO" );
    if (sc.isFailure()) {
      m_log << MSG::WARNING << "Cannot retrieve TgcRdoContainer with key=TGCRDO" << endreq;
      return StatusCode::SUCCESS;
    }
    if (rdoCont->size()>0) {
      TgcRdoContainer::const_iterator itR = rdoCont->begin();
      for(; itR!= rdoCont->end(); itR++) {
	const TgcRdo * rdo = (*itR);
	TgcRdo * thisRdo = const_cast<TgcRdo *>(rdo);
	std::pair<int, int> subDetectorRod(thisRdo->subDetectorId(), thisRdo->rodId());
	m_tgcrdo.insert(std::map<std::pair<int, int>, TgcRdo*>::value_type(subDetectorRod, thisRdo));
      }
    }

    const DataHandle<TgcDigitContainer> tgc_container;
    sc = m_sgSvc->retrieve(tgc_container, m_keyTgcDigit);
    if (sc.isFailure()) {
      m_log << MSG::FATAL << " Cannot retrieve TGC Digit Container " << endreq;
      return sc;
    }

    // doMaskOperation (masked & fired)
    std::map<Identifier, int> TgcDigitIDs;
    std::map<Identifier, int>::iterator itCh;
    // (1) skip masked channels
    for (TgcDigitContainer::const_iterator c = tgc_container->begin(); c != tgc_container->end(); ++c) {
      for (TgcDigitCollection::const_iterator h = (*c)->begin(); h != (*c)->end(); ++h) {
	if (m_CurrentBunchTag>0 && (*h)->bcTag()!=m_CurrentBunchTag) {
	  if (m_debuglevel) {
	    m_log << MSG::VERBOSE << "bctag is not CURRENT " 
		  << " offlineID=" << (*h)->identify()
		  << " bctag=" << (*h)->bcTag() << endreq;
	  }	  
	  continue;
	}
	Identifier channelId = (*h)->identify();
	itCh=m_MaskedChannel.find(channelId);
	if (itCh!=m_MaskedChannel.end() && itCh->second==0) {
	  if (m_debuglevel) {
	    m_log << MSG::DEBUG << "This channel is masked! offlineID=" << channelId << endreq;
	  }
	  continue;
	}
	TgcDigitIDs.insert(std::map<Identifier,int>::value_type(channelId,1));
      }
    }
    // (2) add fired channels by force
    for(itCh=m_MaskedChannel.begin(); itCh!=m_MaskedChannel.end(); itCh++) {
      if (itCh->second==1) {
	if (m_debuglevel) {
	  m_log << MSG::VERBOSE << "This channel is fired by force! offlineID=" << itCh->first << endreq;
	}
	if (TgcDigitIDs.find(itCh->first)==TgcDigitIDs.end()) {
	  TgcDigitIDs.insert(std::map<Identifier,int>::value_type(itCh->first,1));
	}
      }
    }
    //
    if (m_debuglevel) {
      m_log << MSG::DEBUG << "# of total hits    " << TgcDigitIDs.size() << endreq;
    }
    //
    TGCEvent event;
    // Loop on TGC detectors (collections)
    for(itCh=TgcDigitIDs.begin(); itCh!=TgcDigitIDs.end(); itCh++) {
      Identifier channelId = itCh->first;
      int subsystemNumber;
      int octantNumber;
      int moduleNumber;
      int layerNumber;
      int rNumber;
      int wireOrStrip;
      int channelNumber;
      bool status = m_cabling->getOnlineIDfromOfflineID(channelId,
							subsystemNumber,
							octantNumber,
							moduleNumber,
							layerNumber,
							rNumber,
							wireOrStrip,
							channelNumber);

      if(!status) {
	m_log << MSG::INFO << " Fail to getOnlineIDfromOfflineID "
	      << "for  " << channelId << endreq;
      } else {
	bool fstatus; 
	int subDetectorID, rodID, sswID, sbLoc, channelID;
	int phi=0;
	int moduleType=0;
	int slbID=0;
	bool  isAside=true; 
	bool  isEndcap=true;
	
	fstatus = m_cabling->getReadoutIDfromOfflineID(channelId,
						       subDetectorID,
						       rodID,sswID,
						       sbLoc,channelID);
	if (fstatus) {
	  fstatus = m_cabling->getSLBIDfromReadoutID(phi, isAside, isEndcap, 
						     moduleType, slbID,
						     subDetectorID, 
						     rodID, sswID,sbLoc);
	}
	if (fstatus) {
	  if (m_debuglevel) {
	    m_log << MSG::DEBUG << "hit : subsys#=" << subsystemNumber 
		  << " octant#=" << octantNumber
		  << " mod#=" << moduleNumber 
		  << " layer#=" << layerNumber << " r#=" << rNumber
		  << " isStrip=" << wireOrStrip 
		  << " ch#=" << channelNumber << endreq
		  << " --> readoutID: sudetID=" << subDetectorID 
		  << " rodID=" << rodID << " sswID=" << sswID
		  << " slbID=" << slbID << " chID=" << channelID 
		  << endreq;
	  }
	  
	  TGCZDirection zdire = (subsystemNumber==1)? Z_FORWARD : Z_BACKWARD;
	  TGCReadoutIndex index(zdire,octantNumber,moduleNumber,rNumber,layerNumber);
	  TGCSignalType signal = (wireOrStrip==1)? Strip : WireGroup;
	  event.NewASDOut(index,
			  signal,
			  channelNumber,
			  0);
	} else {
	  m_log << MSG::INFO << " Fail to getSLBIDfromOfflineID "
		<< "for  " << channelId << endreq;
	}
      }
    }     // End Loop on TGC detectors (collections)


    if (m_debuglevel) {
      m_log << MSG::DEBUG
	    << "Could make TGCEvent with TgcDigitContainer."
	    << "  vector size : " << event.GetNASDOut()
	    << endreq;
    }

    for(int iout=1; iout<= event.GetNASDOut(); iout++){
      TGCASDOut* asdout = (event.GetASDOutVector()[iout-1]);
      if (m_debuglevel) {
	m_log << MSG::VERBOSE
	      << " Z:" << asdout->GetTGCReadoutIndex().GetZDirection()
	      << " O:" << asdout->GetTGCReadoutIndex().GetOctantNumber()
	      << " M:" << asdout->GetTGCReadoutIndex().GetModuleNumber()
	      << " R:" << asdout->GetTGCReadoutIndex().GetRNumber()
	      << " L:" << asdout->GetTGCReadoutIndex().GetLayerNumber()
	      << " S:" << asdout->GetSignalType()
	      << " I:" << asdout->GetHitID()
	      << " T:" << asdout->GetHitToF()
	      << endreq;
      }
    }
    

    // process trigger electronics emulation...
    TimingManager->increaseBunchCounter();
    system->distributeSignal(&event);
    int i,j,k;
    
    // clear TGCCOIN
    if (g_OUTCOINCIDENCE) {
      if (g_TGCCOIN->size()) {
	for(std::vector<TGCCoincidence*>::iterator iss=g_TGCCOIN->begin(); iss!=g_TGCCOIN->end(); iss++)
	  delete (*iss);
      }
      g_TGCCOIN->clear();
    }

    // EIFI trigger bits for SL are cleared. 
    m_innerTrackletSlotHolder.clearTriggerBits();

    // PatchPanel, SlaveBoard
    for( i=0; i<system->getNumberOfSide(); i+=1){ // i=0:Z>0(A) , i=1:Z<0(C)
      for( j=0; j<system->getNumberOfOctant(); j+=1){
	for( k=0; k<system->getNumberOfModule(); k+=1){
	  TGCSector* sector = system->getSector(i,j,k);
	  if((sector!=0)&&(sector->hasHit())){
	    nEventInSector++;
	    TimingManager->startPatchPanel(sector, db);
	    TimingManager->startSlaveBoard(sector);
	    if (m_OutputTgcRDO.value()) recordRdoSLB(sector);
	    // EIFI trigger bits for SL are filled in this method. 
	  }
	}
      }
    }

    // HighPtBoard, SectorLogic
    LVL1MUONIF::Lvl1MuCTPIInput* muctpiinput = new LVL1MUONIF::Lvl1MuCTPIInput;
    for( i=0; i<system->getNumberOfSide(); i+=1){
      int sectoraddr_endcap = 0;
      int sectoraddr_forward = 0;
      for( j=0; j<system->getNumberOfOctant(); j+=1){
	for( k=0; k<system->getNumberOfModule(); k+=1){
	  if(k>=9) continue;// skip Inner TGC
	  TGCSector* sector = system->getSector(i,j,k);
	  if(sector==0) continue;

	  if((sector->hasHit())){
	    TimingManager->startHighPtBoard(sector);
	    if (m_OutputTgcRDO.value()) recordRdoHPT(sector);

	    // EIFI trigger bits are checked if Endcap
	    if(sector->getRegionType()==Endcap && sector->getSL()) {
	      // Pointers to store EIFI trigger bits for Endcap SL
	      const TGCInnerTrackletSlot* innerTrackletSlots[TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR]
		= {0, 0, 0, 0};
	      m_innerTrackletSlotHolder.getInnerTrackletSlots(i, j, k, innerTrackletSlots);
	      sector->getSL()->setInnerTrackletSlots(innerTrackletSlots);
	    }

	    TimingManager->startSectorLogic(sector);
	    sector->clearNumberOfHit();
	  }
	  // Fill Lvl1MuCTPInput
	  size_t tgcsystem=0,subsystem=0;
	  if(i==0) subsystem = LVL1MUONIF::Lvl1MuCTPIInput::idSideA();
	  if(i==1) subsystem = LVL1MUONIF::Lvl1MuCTPIInput::idSideC();
	  if (m_OutputTgcRDO.value()) recordRdoSL(sector, subsystem);

	  TGCSLSelectorOut* selectorOut = sector->getSL()->getSelectorOutput();
	  sector->getSL()->eraseSelectorOut();
          if(sector->getRegionType()==Endcap){
	    LVL1MUONIF::Lvl1MuEndcapSectorLogicData sldata;
            tgcsystem = LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem();
	    FillSectorLogicData(&sldata,selectorOut,subsystem);
	    muctpiinput->setSectorLogicData(sldata,tgcsystem,subsystem,sectoraddr_endcap++);
	  }
          if(sector->getRegionType()==Forward){
	    LVL1MUONIF::Lvl1MuForwardSectorLogicData sldata;
            tgcsystem = LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem();
	    FillSectorLogicData(&sldata,selectorOut,subsystem);
            muctpiinput->setSectorLogicData(sldata,tgcsystem,subsystem,sectoraddr_forward++);
	  }

	  // Print
	  if(m_debuglevel && selectorOut!=0) {
	    if(selectorOut->getNCandidate()){
	      m_log << MSG::DEBUG
		    << "sector " << system->getNumberOfModule()*j+k+1
		    <<" :  number of candidates " << selectorOut->getNCandidate()
		    << endreq;
	      for(int ican=0; ican< selectorOut->getNCandidate(); ican++){
		m_log << MSG::DEBUG
		      << "candidate " << ican+1 << " : "
		      << selectorOut->getPtLevel(ican) << "   "
		      << selectorOut->getR(ican) << "   "
		      << selectorOut->getPhi(ican)
		      << endreq;
	      }
	    }
	  }
	  // delete selectorOut
          if (selectorOut != 0 ) {
	    delete selectorOut; 
	    selectorOut=0;
	  }
	} // k Module
      } // j Octant
    } // i Side

    // record   MuCTPIInput_TGC
    sc = m_sgSvc->record(muctpiinput, m_keyMuCTPIInput_TGC);
    if (sc.isFailure()) {
      m_log << MSG::FATAL
	    << "Could not record MuCTPIInput_TGC."
	    << endreq;
      return StatusCode::FAILURE;
    } else {
      if( m_debuglevel) {
	m_log << MSG::DEBUG
	      << "Could record MuCTPIInput_TGC."
	      << endreq;
	m_log << MSG::DEBUG
	      << (*muctpiinput)
	      << endreq;
      }
    }

    event.Clear();

    return StatusCode::SUCCESS;
  }



  
////////////////////////////////////////////////////////
void LVL1TGCTrigger::FillSectorLogicData(LVL1MUONIF::Lvl1MuSectorLogicData *sldata,
					   TGCSLSelectorOut* selectorOut, unsigned int subsystem)
  {
    int Zdir= (subsystem==LVL1MUONIF::Lvl1MuCTPIInput::idSideA() ? 1 : -1);
    if(selectorOut!=0){
      sldata->clear2candidatesInSector();// for temporary
      sldata->bcid(0);
      if ((selectorOut->getNCandidate()) >= 1) {
        sldata->roi(0,((selectorOut->getR(0))<<2)+(selectorOut->getPhi(0)));
//      ovl --> veto  
//      sldata->ovl(0,0);
	if (selectorOut->getInnerVeto(0)) sldata->ovl(0,1);
	else                              sldata->ovl(0,0);
        sldata->pt(0,selectorOut->getPtLevel(0));
	sldata->charge(0, getCharge(selectorOut->getDR(0),Zdir));
      } else {
        // no entry
      }
      if ((selectorOut->getNCandidate()) == 2) {
        sldata->roi(1,((selectorOut->getR(1))<<2)+(selectorOut->getPhi(1)));
//      ovl --> veto  
//      sldata->ovl(1,0);
	if (selectorOut->getInnerVeto(1)) sldata->ovl(1,1);
	else                              sldata->ovl(1,0);
        sldata->pt(1,selectorOut->getPtLevel(1));
	sldata->charge(1, getCharge(selectorOut->getDR(1),Zdir));
      }
      sldata->set2candidates(0);// not used for TGC
      sldata->clear2candidates(0);// not used for TGC
      sldata->set2candidates(1);// not used for TGC
      sldata->clear2candidates(1);// not used for TGC

      // Print
      if(m_debuglevel) {
	if ((selectorOut->getNCandidate()) >= 1) {
	  m_log << MSG::DEBUG 
		<< "SectorLogic: 1st candidate   " 
		<< " roi:" << (selectorOut->getR(0))<<2 + selectorOut->getPhi(0)
		<< " pt:" << selectorOut->getPtLevel(0)
		<< " charge:" << getCharge(selectorOut->getDR(0),Zdir)
		<< " veto:" << sldata->ovl(0)
		<<endreq;
	}
	if ((selectorOut->getNCandidate()) == 2) {
	  m_log << MSG::DEBUG 
		<< "SectorLogic: 2nd candidate   " 
		<< " roi:" << (selectorOut->getR(1))<<2 + selectorOut->getPhi(1)
		<< " pt:" << selectorOut->getPtLevel(1)
		<< " charge:" << getCharge(selectorOut->getDR(1),Zdir)
		<< " veto:" << sldata->ovl(1)
		<<endreq;
	}
      }	  

    } else {
      // no entry
    }
  }


//////////////////////////////////////////
  void LVL1TGCTrigger::recordRdoSLB(TGCSector * sector) 
  {
    uint16_t bcTag=m_CurrentBunchTag, l1Id=0, bcId=0;
    // readoutID
    int subDetectorId, rodId, sswId, sbLoc, secId, secIdEIFI;
    // SLBID
    bool isAside, isEndcap; int phi, moduleType, id, phiEIFI;
    isAside = (sector->getSideId()==0  ? 1 : 0);
    isEndcap = (sector->getRegionType()==Endcap ? 1 : 0);
    int module = sector->getModuleId();
    // OnlineID moduleNumber 
    //       <---- phi ---- 
    // EC:   7 6 4 3 1 0   11 10  9
    // FWD:   8   5   2    14 13 12 
    //       [M1, M2, M3]  [EI/FI]
    // secId=0-5(EC), 0-2(FWD) for TgcRawData
    secId = (isEndcap ? (module/3)*2+module%3 : module/3);
    // phi=1-48(EC), 1-24(FWD) in detector ID scheme
    phi = (isEndcap ? (secId+46+sector->getOctantId()*6)%48+1 : (secId+23+sector->getOctantId()*3)%24+1);
    // secIdEIFI=0-2
    secIdEIFI = module%3;
    // phiEIFI=1-24
    phiEIFI = (secIdEIFI+23+sector->getOctantId()*3)%24+1;

    // SLB
    const int NumberOfSLBType = 6;
    // 0:  WT,  1: WD,  2: ST,  3: SD,  4: WI  5:SI 
    for(int itype=0; itype<NumberOfSLBType; itype++) {
      moduleType = getLPTTypeInRawData(itype); 

      // loop over all SB of each type
      for(int index=0; index<sector->getNumberOfSB(itype); index++) {
	TGCSlaveBoard * slb = sector->getSB(itype, index);
	if (0==slb) continue;
	id = slb->getId();
	TGCSlaveBoardOut * out = slb->getOutput();
	if (0==out) continue;

	bool isEIFI = (moduleType==TgcRawData::SLB_TYPE_INNER_WIRE ||
		       moduleType==TgcRawData::SLB_TYPE_INNER_STRIP);
	
	// get ReadoutID
	bool status = 
	  m_cabling->getReadoutIDfromSLBID((isEIFI ? phiEIFI : phi), 
					   isAside, isEndcap, 
					   moduleType, id,
					   subDetectorId, rodId, 
					   sswId, sbLoc);
	if (!status) {
	  if (m_debuglevel) {
	    m_log << MSG::DEBUG 
		  << "TGCcablignSvc::getReadoutIDfromSLBID fails" << endreq
		  << MSG::DEBUG
		  << "phi=" << phi  
		  << " side=" << ((isAside) ? "A": "C") 
		  << " region=" << ((isEndcap) ? "Endcap" : "Forward")
		  << " type="   << moduleType
		  << " id="  << id
		  << " subDetectorId=" << subDetectorId
		  << " rodId=" << rodId
		  << " sswId=" << sswId
		  << " sbLoc=" << sbLoc
		  << endreq;
	  }
	  continue;
	}

	// fill TgcRawData
	for(int iData=0; iData<out->getNumberOfData(); iData++) { // max 8
	  if (!out->getHit(iData)) continue;

	  //  see TGCcabling/TGCId.h (WD=0,SD,WT,ST,SI,WI). Same as TgcRawData
	  TgcRawData::SlbType type = (TgcRawData::SlbType)moduleType;
	  int subMat = iData % 4;
	  int seg    = 0;
	  if (type==TgcRawData::SLB_TYPE_TRIPLET_STRIP ) {
	    if (iData<4) seg= 1;
            // 13.Jan.2011 reversed by Hisaya
            // because Layer swap in TGCStripTripletSB::doCoincidence()
	  } else if ( (type==TgcRawData::SLB_TYPE_INNER_WIRE )    ||
	       (type==TgcRawData::SLB_TYPE_INNER_STRIP)       ) {
	    seg= iData/4;
	  }
	  TgcRawData * rawdata =
	    new TgcRawData(bcTag,
			   static_cast<uint16_t>(subDetectorId),
			   static_cast<uint16_t>(rodId), 
			   static_cast<uint16_t>(sswId), 
			   static_cast<uint16_t>(sbLoc), 
			   l1Id, bcId,
			   type, out->getDev(iData), seg, subMat, 
			   out->getPos(iData));	    
	  addRawData(rawdata);

	  // EIFI trigger bits for SL are filled. 
	  if(isEIFI) {
	    bool setEIFITriggerBit = 
	      m_innerTrackletSlotHolder.setTriggerBit(sector->getSideId(), 
						      phiEIFI, 
						      (isEndcap ? 
						       TGCInnerTrackletSlot::EI : TGCInnerTrackletSlot::FI), 
						      (type==TgcRawData::SLB_TYPE_INNER_WIRE ? 
						       TGCInnerTrackletSlot::WIRE : TGCInnerTrackletSlot::STRIP), 
						      static_cast<unsigned int>(subMat), 
						      true);
#ifdef TGCDEBUG
	    ///////HISAYA////////////
	    m_log << MSG::INFO
	    	  << " recordRdoSLB : reg=" << (isEndcap ? "EC" : "FWD") 
	    	  << " phi=" << phiEIFI
	    	  << " type=" << type
	    	  << " subMat =" << subMat
	    	  << " pos=" << out->getPos(iData) 
		  << " dev=" << out->getDev(iData) 
	    	  <<  endreq;
	    ///////HISAYA///////////
#endif

	    if(!setEIFITriggerBit) {
	      m_log << MSG::INFO << "Fail to set Inner trigger bit of" 
				 << " sideId= " << sector->getSideId() 
				 << " slotId= " << phiEIFI
				 << " region= " << (isEndcap ? "EI" : "FI")
				 << " readout= " << (type==TgcRawData::SLB_TYPE_INNER_WIRE ? "WIRE" : "STRIP") 
				 << " subMat(iBit)= " << static_cast<unsigned int>(subMat) 
				 << endreq;
	    }
	  }

	  if (m_debuglevel) {
	    m_log << MSG::DEBUG
		  << " recordRdoSLB : reg=" << (isEndcap ? "EC" : "FWD") 
		  << " rod=" << rodId << " sswId=" << sswId 
		  << " SBLoc=" << sbLoc << " type=" << itype 
		  << " iData(subMat:seg)=" << iData << " pos=" 
		  << out->getPos(iData) << " dev=" << out->getDev(iData) 
		  <<  endreq;
	  }	    
	}
	// end of filling TgcRawData

      }
      // end of loop over SB
    }
  }

////////////////////////////////////////////////////////  
  void LVL1TGCTrigger::recordRdoHPT(TGCSector * sector) 
  {
    // readoutID
    int subDetectorId, rodId, sswId, sbLoc, secId;

    // get numbering scheme info from cabling svc 
    int startEndcapSector, coverageOfEndcapSector;
    int startForwardSector, coverageOfForwardSector;
    if (isAtlas) {
      rodId = 1;
    } else {
      rodId =0;
    }
    m_cabling->getCoveragefromRodID(rodId,
				    startEndcapSector,
				    coverageOfEndcapSector,
				    startForwardSector,
				    coverageOfForwardSector
				    ) ;

    uint16_t bcTag=m_CurrentBunchTag, l1Id=0, bcId=0;
 
   // HPTID
    bool isAside, isEndcap, isStrip; int phi;
    isAside = (sector->getSideId()==0);
    isEndcap = (sector->getRegionType()==Endcap);
    int module = sector->getModuleId();
    //  sector Id = 0..47 (Endcap) 0..23 (forward) 
    int sectorId;
    if (isEndcap){
      sectorId = ((module/3)*2+module%3) + sector->getOctantId()*6;
    } else {
      sectorId =  (module/3) +  sector->getOctantId()*3;
    }
    // secId for TgcRawData
    //  0-3(EC), 0-1(FWD) for new TGCcabling (1/12sector)
    //  0-5(EC), 0-2(FWD) for new TGCcabling (octant)
    if (isEndcap){
      secId = sectorId % coverageOfEndcapSector;
    } else {
      secId = sectorId % coverageOfForwardSector;
    }
    // phi=1-48(EC), 1-24(FWD) in detector ID scheme
    phi = (isEndcap ? (sectorId+46)%48+1 : (sectorId+23)%24+1);

    for(int itype=0; itype<2; itype++) { // loop over HPB type(wire/strip)
      isStrip = (itype==0 ? 0 : 1); // 0=wire 1=strip
      for(int ihpb=0; ihpb<sector->getNumberOfHPB(itype); ihpb++) { // loop over # of HPB per sector
	TGCHighPtBoard * hpb = sector->getHPB(itype, ihpb);
	if (0==hpb) continue;
	TGCHighPtChipOut * out = hpb->getOutput();
	if (0==out) continue;

	// get ReadoutID
	bool status = m_cabling->getReadoutIDfromHPTID(phi, isAside, isEndcap, isStrip, hpb->getId(),
						       subDetectorId, rodId, sswId, sbLoc);
	if (!status) {
	  m_log << MSG::WARNING << "TGCcablignSvc::getReadoutIDfromHPTID fails" << endreq;
	  continue;
	}

        // loop over chip and candidate	
	for(int ichip=0; ichip<NumberOfChip; ichip++) { // NumberOfChip=2
	  for(int icand=0; icand<NHitInTrackSelector; icand++) { // NHitInTrackSelector=2
	    if (!out->getSel(ichip, icand)) continue; // should be 1 or 2
	    int chip  = ichip;
	    int index = ihpb;
	    int hitId =  out->getHitID(ichip, icand);
	    m_cabling->getRDOHighPtIDfromSimHighPtID(!isEndcap, isStrip,
						     index, chip, hitId);
	    bool isHPT = out->getPt(ichip,icand)==PtHigh ? 1 : 0;
	    TgcRawData * rawdata = 
	      new TgcRawData(bcTag,
			     static_cast<uint16_t>(subDetectorId), 
			     static_cast<uint16_t>(rodId), 
			     l1Id, 
			     bcId,
			     isStrip, (!isEndcap), secId, chip, icand,
			     isHPT, hitId,
			     out->getPos(ichip, icand), 
			     out->getDev(ichip, icand));
	    addRawData(rawdata);
	    
#ifdef TGCDEBUG
	    ///////////HISAYA///////////
	    m_log << MSG::INFO
		  << "recordRdoHPT : reg=" 
		  << " side=" << ( (isAside)? "A" : "C")
		  << (isEndcap ? "EC" : "FWD") 
		  << " w/s=" << ( (isStrip)? "s" : "w")
		  << " id=" <<  hpb->getId()
		  << " ecId=" <<  secId
		  << " chip=" << ichip 
		  << " cand=" << icand
		  << " block=" << out->getHitID(ichip, icand)
		  << " subMatrix=" << out->getPos(ichip, icand)
		  << " dev=" << out->getDev(ichip, icand) 
		  << " rod=" << rodId << " sswId=" << sswId 
		  << " SBLoc=" << sbLoc
		  <<  endreq;
	    ////////////////////
#endif
	    // Print
	    if (m_debuglevel) {
	      m_log << MSG::DEBUG 
		    << "recordRdoHPT : reg=" 
		    << " side=" << ( (isAside)? "A" : "C")
		    << (isEndcap ? "EC" : "FWD") 
		    << " w/s=" << ( (isStrip)? "s" : "w")
		    << " id=" <<  hpb->getId()
		    << " ecId=" <<  secId
		    << " chip=" << ichip 
		    << " cand=" << icand
		    << " block=" << out->getHitID(ichip, icand)
		    << " subMatrix=" << out->getPos(ichip, icand)
		    << " dev=" << out->getDev(ichip, icand) 
		    << " rod=" << rodId << " sswId=" << sswId << " SBLoc=" << sbLoc
		    <<  endreq;
	    }

	    // Strip HPT hit may be duplicated
	    if (   g_SHPT_ORED &&
		   isEndcap && isStrip &&
		   (chip==1) ) {
	      int oredId = -1;
	      if (hitId == 1) oredId = 5;
	      else if (hitId == 2) oredId = 6;
	      else if (hitId == 5) oredId = 1;
	      else if (hitId == 6) oredId = 2;
	      if (oredId >=0) {
		TgcRawData * rawdata2 = 
		  new TgcRawData(bcTag,
			     static_cast<uint16_t>(subDetectorId), 
			     static_cast<uint16_t>(rodId), 
			     l1Id, 
			     bcId,
			     isStrip, (!isEndcap), secId, chip, icand,
			     isHPT, oredId,
			     out->getPos(ichip, icand), 
			     out->getDev(ichip, icand));
		addRawData(rawdata2);
	      }
	      ////////////////////

	    }

	  }
	}
	// end loop of candidate and chip 

      }
    }
    // end oop of board and type
  }
  

///////////////////////////////////////////////////////
  void LVL1TGCTrigger::recordRdoSL(TGCSector * sector, unsigned int subsystem) 
  {
    // check if whether trigger exists or not
    TGCSLSelectorOut* selectorOut = sector->getSL()->getSelectorOutput();
    if (selectorOut ==0) return;
    if (selectorOut->getNCandidate()==0) return;

    // readoutID
    int subDetectorId=0, rodId=0, sswId=0, sbLoc=0, secId=0;
    
    // trigger info
    bool cand3plus=0, isEndcap=0, isAside=0, muplus=0, overlap=0;
    int phi=0, index=0, threshold=0, roi=0;
    int Zdir= (subsystem==LVL1MUONIF::Lvl1MuCTPIInput::idSideA() ? 1 : -1);

    isAside = (sector->getSideId()==0);
    isEndcap = (sector->getRegionType()==Endcap);
    cand3plus = 0;
    //overlap = 0; 

    //  sector Id = 0..47 (Endcap) 0..23 (forward) 
    int module = sector->getModuleId();
    int sectorId;
    if (isEndcap){
      sectorId = ((module/3)*2+module%3) + sector->getOctantId()*6;
    } else {
      sectorId =  (module/3) +  sector->getOctantId()*3;
    }

    //  secID for TGCRawData
    //  0-3(EC), 0-1(FWD) for new TGCcabling (1/12sector)
    //  0-5(EC), 0-2(FWD) for new TGCcabling (octant)
    int startEndcapSector, coverageOfEndcapSector;
    int startForwardSector, coverageOfForwardSector;
    rodId = 0;
    if (isAtlas) rodId = 1;
    m_cabling->getCoveragefromRodID(rodId,
				    startEndcapSector,
				    coverageOfEndcapSector,
				    startForwardSector,
				    coverageOfForwardSector
				    ) ;
    if (isEndcap){
      secId = sectorId % coverageOfEndcapSector;
    } else {
      secId = sectorId % coverageOfForwardSector;
    }

    // phi=1-48(EC), 1-24(FWD) in detector ID scheme
    phi = (isEndcap ? (sectorId+46)%48+1 : (sectorId+23)%24+1);
    
    // get readout ID
    bool status = m_cabling->getReadoutIDfromSLID(phi, isAside, isEndcap, 
						  subDetectorId, rodId, sswId, sbLoc);	    
    if (!status) {
      m_log << MSG::WARNING << "TGCcablignSvc::ReadoutIDfromSLID fails"
	    << (isEndcap ? "  Endcap-" : "  Forward-") 
	    << (isAside  ? "A  " : "C  ")
	    << "  phi=" << phi
	    << endreq;

      return;
    }

    uint16_t bcTag=m_CurrentBunchTag, l1Id=0, bcId=0;
    for (int icand=0; icand < selectorOut->getNCandidate(); icand +=1) {
      index=icand;
      muplus = getCharge(selectorOut->getDR(icand), Zdir)==1 ? 1 : 0;
      threshold = selectorOut->getPtLevel(icand);
      roi = ((selectorOut->getR(icand))<<2)+(selectorOut->getPhi(icand));
      if (selectorOut->getInnerVeto(icand)) overlap = 1;
      else                                  overlap = 0;

      // create TgcRawData
      TgcRawData * rawdata = 
	  new TgcRawData(bcTag,
			 static_cast<uint16_t>(subDetectorId), 
			 static_cast<uint16_t>(rodId), 
			 l1Id, 
			 bcId,
			 cand3plus, (!isEndcap), secId, index,
			 muplus, threshold, overlap, roi);
      addRawData(rawdata);      
      
#ifdef TGCDEBUG
      //////////////HISAYA////////////
      m_log << MSG::INFO
	    << "recordRdoSL  : reg=" 
	    << (isEndcap ? "EC" : "FWD") 
	    << " phi=" << phi
	    << " cand=" << index 
	    << " charge=" << (muplus ? "mu+" : "mu-")
	    << " thre=" << threshold 
	    << " overlap=" << overlap 
	    << " roi=" << roi 
	    << " rod=" << rodId << " sswId=" << sswId 
	    << " SBLoc=" << sbLoc
	    << endreq;
      /////////////////////////
#endif

      if (m_debuglevel) {
	m_log << MSG::DEBUG
	      << "recordRdoSL  : reg=" 
	      << (isEndcap ? "EC" : "FWD") 
	      << " phi=" << phi
	      << " cand=" << index 
	      << " charge=" << (muplus ? "mu+" : "mu-")
	      << " thre=" << threshold 
	      << " overlap=" << overlap 
	      << " roi=" << roi 
	      << " rod=" << rodId << " sswId=" << sswId << " SBLoc=" << sbLoc
	      << endreq;
      }
    }
  }

///////////////////////////////////////////////////////////////////////////////////
  // Mask=0/Fire=1
  StatusCode LVL1TGCTrigger::doMaskOperation() 
  {
    std::string fname=m_MaskFileName.value();
    if (isAtlas) fname=m_MaskFileName12.value();
    if (fname.empty()) return StatusCode::SUCCESS;

    std::string fullName = PathResolver::find_file (fname.c_str(), "PWD");
    if( fullName.length() == 0 )
      fullName =  PathResolver::find_file (fname.c_str(), "DATAPATH");

    ifstream fin(fullName.c_str());
    if (!fin) {
      m_log << MSG::FATAL << "Cannot open file " << fullName << endreq;
      return StatusCode::FAILURE;
    } else {
      m_log << MSG::INFO << "Use mask file : " << fullName << endreq;
    }
    // read database ------------------------------------------------------------------------------
    std::vector<std::string> mask;
    std::string aLine;
    while(getline(fin,aLine)) {
      if (aLine.substr(0,3)!="///") break;
    }
    int id_type = atoi(aLine.c_str());
    while(getline(fin,aLine)) {
      if (!aLine.empty()) mask.push_back(aLine);
    }
    fin.close();

    //
    std::vector<int> ids;
    Identifier ID;
    int nmasked=0, nfired=0;
    for(int ich=0; ich<(int)mask.size(); ich++) {
      std::string ch = mask[ich];
      extractFromString(ch, ids);
      int OnOff=ids[0]; // 0=off(masked) 1=on(fired)
      //
      if (id_type==1 && ids.size()==8) { // online
	int sysno1 = (ids[1]==-99 ? -1 : ids[1]);  int sysno2=(ids[1]==-99 ? 1 : ids[1]);// -1(B) 1(F)
	int octno1 = (ids[2]==-99 ? 0  : ids[2]);  int octno2=(ids[2]==-99 ? 7 : ids[2]);
	for(int sysno=sysno1; sysno<=sysno2; sysno+=2) {
	  for(int octno=octno1; octno<=octno2; octno++) {
	    bool status = m_cabling->getOfflineIDfromOnlineID(ID,sysno,octno,
							      ids[3],ids[4],ids[5],ids[6],ids[7]);
	    if ( m_debuglevel) {
	      m_log << MSG::VERBOSE 
		    << (OnOff==0 ? "Mask" : "Fire") << " : offlineID=" << ID
		    << " sys=" << sysno << " oct=" << octno << " modno=" << ids[3] 
		    << " layerno=" << ids[4] << " rNumber=" << ids[5] 
		    << " strip=" << ids[6] << " chno=" << ids[7] << endreq;
	    }
	    if (!status) {
	      m_log << MSG::WARNING 
		    << "This onlineID is not valid and cannot be converted to offline ID." 
		    << endreq 
		    << "sys=" << sysno << " oct=" << octno << " modno=" << ids[3] 
		    << " layerno=" << ids[4] << " rNumber=" << ids[5] 
		    << " strip=" << ids[6] << " chno=" << ids[7] << endreq;
	    } else {
	      m_MaskedChannel.insert(std::map<Identifier, int>::value_type(ID, OnOff));
	      if (OnOff==0)      nmasked+=1;
	      else if (OnOff==1) nfired+=1;
	    }
	  }
	}

      } else if (id_type==2 && ids.size()==6) { // readout id
	int sysno1 = (ids[1]==-99 ? 103 : ids[1]);  int sysno2=(ids[1]==-99 ? 104 : ids[1]);// 103(F), 104(B)
	int octno1 = (ids[2]==-99 ? 0  : ids[2]);  int octno2=(ids[2]==-99 ? 7 : ids[2]);
	for(int sysno=sysno1; sysno<=sysno2; sysno+=1) {
	  for(int octno=octno1; octno<=octno2; octno++) {
	    bool status = m_cabling->getOfflineIDfromReadoutID(ID, sysno,octno,ids[3],ids[4],ids[5]);
	    if (m_debuglevel) {
	      m_log << MSG::VERBOSE 
		    << (OnOff==0 ? "Mask" : "Fire") << " : offlineID=" << ID
		    << " subdetectorID=" << sysno << " rodId=" << octno << " sswID=" << ids[3] 
		    << " SBLoc=" << ids[4] << " channelId=" << ids[5] << endreq;
	    }
	    if (!status) {
	      m_log << MSG::WARNING 
		    << "This readoutID is not valid and cannot be converted to offline ID " 
		    << endreq
		    << "subdetectorID=" << sysno << " rodId=" << octno << " sswID=" << ids[3] 
		    << " SBLoc=" << ids[4] << " channelId=" << ids[5] << endreq;
	    } else {
	      m_MaskedChannel.insert(std::map<Identifier, int>::value_type(ID, OnOff));
	      if (OnOff==0)      nmasked+=1;
	      else if (OnOff==1) nfired+=1;
	    }
	  }
	}

      } else if (id_type==3 && ids.size()==2) { // offline id
	ID = Identifier((unsigned int)ids[1]);
	if (m_debuglevel) {
	  m_log << MSG::DEBUG << (OnOff==0 ? "Mask" : "Fire") << " : offlineID=" << ID << endreq;
	}
	m_MaskedChannel.insert(std::map<Identifier, int>::value_type(ID, OnOff));
	if (OnOff==0)      nmasked+=1;
	else if (OnOff==1) nfired+=1;

      } else {
	m_log << MSG::INFO 
	      << "Invalid input. Idtype or number of parameters are invalid: idtype=" << id_type
	      << " number of elements = " << ids.size() << endreq;
	return StatusCode::FAILURE;
      }
    }
    //
    m_log << MSG::INFO << "Total number of masked channels ... " << nmasked << endreq;
    m_log << MSG::INFO << "Total number of fired  channels ... " << nfired  << endreq;
    //
    return StatusCode::SUCCESS;
  }


/////////////////////////////////////////
  void LVL1TGCTrigger::extractFromString(std::string str, std::vector<int> & v) {
    v.clear();
    if (str.empty()) return;
    std::string line=str;
    while(1) {
      if (line.empty()) break;
      int i = line.find(" ");
      if (i==(int)std::string::npos && !line.empty()) {
	v.push_back(atoi(line.c_str()));
	break;
      }
      string temp = line;
      temp.erase(i,line.size());
      v.push_back(atoi(temp.c_str()));
      line.erase(0,i+1);
    }
  }

////////////////////////////////////////////////
  int LVL1TGCTrigger::getCharge(int dR, int /*Zdir*/) {
    // old scheme
    // if (dR==0) return (Zdir>0 ? -1 : 1);
    // return (dR*Zdir>0 ? 1 : -1);
    return (dR >=0 ? 1 : -1 );
  }

////////////////////////////////////////////////
  // see TGCNumbering.hh 
  int LVL1TGCTrigger::getLPTTypeInRawData(int type) 
  {
    switch(type) {
    case WTSB :
      return TgcRawData::SLB_TYPE_TRIPLET_WIRE;
    case WDSB :
      return TgcRawData::SLB_TYPE_DOUBLET_WIRE;
    case STSB :
      return TgcRawData::SLB_TYPE_TRIPLET_STRIP;
    case SDSB :
      return TgcRawData::SLB_TYPE_DOUBLET_STRIP;
    case WISB :
      return TgcRawData::SLB_TYPE_INNER_WIRE;
    case SISB :
      return TgcRawData::SLB_TYPE_INNER_STRIP;
    default :
      return -1;
    }
  }


/////////////////////////////////////////////////
  void LVL1TGCTrigger::addRawData(TgcRawData * rawdata) 
  {
    std::pair<int, int> subDetectorRod(rawdata->subDetectorId(), rawdata->rodId());
    std::map<std::pair<int, int>, TgcRdo*>::iterator itRdo = m_tgcrdo.find(subDetectorRod);
    if (itRdo==m_tgcrdo.end()) {
      m_log << MSG::WARNING << "Inconsistent RodId with hits " 
	    << " for subDetectorId=" << rawdata->subDetectorId()
	    << " for rodId=" << rawdata->rodId()
	    << "  Type=" << static_cast<int>(rawdata->type()) 
	    << endreq;
    } else {
      TgcRdo * thisRdo = itRdo->second;
      thisRdo->push_back(rawdata);
    }
  }
  
///////////////////////////////////////////////////////////

StatusCode LVL1TGCTrigger::getCabling() 
{
  // TGCcablingSvc
  // get Cabling Server Service
  const ITGCcablingServerSvc* TgcCabGet = 0;
  StatusCode sc = service("TGCcablingServerSvc", TgcCabGet);
  if (sc.isFailure()){
    m_log << MSG::FATAL << " Can't get TGCcablingServerSvc " << endreq;
    return StatusCode::FAILURE;
  }
  
  if(!TgcCabGet->isConfigured()) {
      m_log << MSG::DEBUG << "TGCcablingServer not yet configured!" << endreq;
      return StatusCode::FAILURE;
  }
  
  // get Cabling Service
  sc = TgcCabGet->giveCabling(m_cabling);
  if (sc.isFailure()){
    m_log << MSG::FATAL << " Can't get TGCcablingSvc Server" << endreq;
    return StatusCode::FAILURE; 
  }

  int maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId;
  m_cabling->getReadoutIDRanges( maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId);
  isAtlas = (maxRodId ==12);
  g_ISATLAS = isAtlas;
  if (isAtlas) {
    m_log << MSG::INFO << m_cabling->name() << " is OK" << endreq ;
  } else {
    m_log << MSG::INFO << "TGCcablingSvc(octant segmentation) OK" << endreq ;
  }

    // determine version of CW 
    // !! NOTE : CW version is determined by jobOption of VersionCW
    // !!        independent from TrigConfigSvc
    // default set is
    //   setM : for 12-fold
    //   setD : for 8-fold
    const std::string setM="setM";
    const std::string setL="setL";
    const std::string setK="setK";
    const std::string setJ="setJ";
    const std::string setI="setI";
    const std::string setH="setH";
    const std::string setG="setG";
    const std::string setF="setF";
    const std::string setE="setE";
    const std::string setD="setD";
    const std::string setC="setC";
    std::string ver=m_VerCW.value();
    if (    (ver!=setM) &&  (ver!= setL) && (ver!= setK) && (ver!=setI) 
         && (ver!=setH) && (ver!=setF) &&  (ver!=setE) 
	 && (ver!=setD) && (ver!=setC) && (ver!=setG) &&  (ver!=setJ) ){
      // default CW is setM
      ver= setM;
    }
    if (!isAtlas) {
      // Only setC and SetD are provided for old MC cabling 
      if (    (ver==setM)  || (ver==setL) || (ver==setK) || (ver==setI ) 
           || (ver==setH ) || (ver == setF) || (ver==setG) || (ver==setJ) ) {
	// 0, 6, 10, 11, 15, 20
        ver = setD;
      } else if (ver == setE) {
	// 0, 6, 10, 15, 20, 40
        ver= setC;
      }
    }
    if ( (ver==setM) || (ver==setL)|| (ver==setK) || (ver==setI) || (ver==setG) ) {
      // setG, I, K, L and setM use 2-st . g_MUHALO flag is set to false
      //  i.e.  2-st. coincidence is described in db file
      g_MUHALO = false;
    }
    m_log << MSG::INFO 
           << " TGC CW version of " << ver << " is selected " << endreq;
    m_VerCW = ver;

    // check Inner /TileMu   
    if (g_USE_INNER) {
      g_USE_INNER = (ver==setM) || (ver==setL)|| (ver==setK) || (ver==setH) ;
    } 
    if (g_TILE_MU) {
      g_TILE_MU = (ver==setM);
    } 

    // create DataBase and TGCElectronicsSystem
    db = new TGCDatabaseManager(m_VerCW, isAtlas);
    system = new TGCElectronicsSystem(db, isAtlas);
    
    TimingManager = new TGCTimingManager;
    TimingManager->setBunchCounter(0);
    nEventInSector = 0;

  return sc;
}

StatusCode LVL1TGCTrigger::fillTMDB()
{
 StatusCode sc =  StatusCode::SUCCESS;
  TGCTMDB* tmdb = system->getTMDB(); 

  // clear TMDB
  tmdb->eraseOutput();

  // retrive TileMuonReceiverContainer
  const DataHandle<TileMuonReceiverContainer> tileMuRecCont;
  sc = m_sgSvc->retrieve(tileMuRecCont, m_keyTileMu);

  if (sc.isFailure()) {
    m_log << MSG::WARNING << " Cannot retrieve Tile Muon Receiver Container " << endreq;
    return sc;
  }
 
  // loop over all TileMuonReceiverObj in container 
  TileMuonReceiverContainer::const_iterator tmItr = tileMuRecCont->begin();
  for ( ; tmItr != tileMuRecCont->end(); ++tmItr) {

    TileMuonReceiverObj * tmObj = *tmItr;
    // Tile Module
    int moduleID = tmObj-> GetID();
    // side 0: a-side, 1: c-side,  3: NA
    // mod 0~63
    int side = 3;      
    int mod = moduleID%100;  
    if (moduleID/100 == 3) {
      side = 0;
    } else if (moduleID/100 == 4) {
      side = 1;
    }
    // TMDB decision
    bool tile2SL[4];
    //     [0]       [1]       [2]      [3]
    //     d5d6_hi   d5d6_lo   d6_hi    d6_lo
    for (size_t ip=0;ip<4;ip++){
      tile2SL[ip] = (tmObj->GetDecision()).at(ip);
    }
    
    // setOutput(side, mod, hit56, hit6) -> hit56, 6 -> 0: No Hit, 1: Low, 2: High, 3: NA
    int hit56 = 3, hit6 = 3;
    if (tile2SL[0] == true && tile2SL[1] == false) {
      hit56 = 2;
    } else if (tile2SL[0] == false && tile2SL[1] == true) {
      hit56 = 1;
    } else if (tile2SL[0] == false && tile2SL[1] == false) {
      hit56 = 0;
    }

    if (tile2SL[2] == true && tile2SL[3] == false) {
      hit6 = 2;
    } else if (tile2SL[2] == false && tile2SL[3] == true) {
      hit6 = 1;
    } else if (tile2SL[2] == false && tile2SL[3] == false) {
      hit6 = 0;
    }

    tmdb->setOutput(side, mod, hit56, hit6);
  }

  return sc;
}
} //end of namespace bracket
  
