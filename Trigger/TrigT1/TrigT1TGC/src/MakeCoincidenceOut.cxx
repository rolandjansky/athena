/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  MakeCoincidenceOut.cxx

// STL
#include <string>
#include <iostream>

// Athena/Gaudi
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

// MuonSpectrometer
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"

#include "MuonIdHelpers/TgcIdHelper.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h" 
#include "GaudiKernel/SmartDataPtr.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h" 
#include "TrigT1TGC/MakeCoincidenceOut.h"
#include "TrigT1TGC/TGCCoincidence.hh"

#define MaxNhpt 20
#define MaxNmu  20
#define MaxNcol 1600
#define MaxNdig 4096

namespace LVL1TGCTrigger {
  extern bool g_OUTCOINCIDENCE;
  extern TGCCoincidences * g_TGCCOIN;
  
  MakeCoincidenceOut::MakeCoincidenceOut(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_sgSvc("StoreGateSvc", name),
    m_tgcIdHelper(0)
    //m_ntuplePtr(0)
  {
    declareProperty("EventStore", m_sgSvc, "Event Store");
    declareProperty("InputData_perEvent",  m_key);
    declareProperty("WriteMCtruth",  m_WriteMCtruth=true);
  }
  
  MakeCoincidenceOut::~MakeCoincidenceOut()
  {
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "MakeCoincidenceOut destructor called" << endmsg;
    }
  }
  
  
  StatusCode MakeCoincidenceOut::initialize()
  {
    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "MakeCoincidenceOut::initialize() called" << endmsg;
    }
    msg(MSG::INFO) << "MakeCoincidenceOut initialize" << endmsg;

    // StoreGateSvc
    //StatusCode sc = service("StoreGateSvc", m_sgSvc);
    StatusCode sc = m_sgSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not find StoreGateSvc" << endmsg;
      return StatusCode::FAILURE;
    } else {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could find StoreGateSvc" << endmsg;
    }
    
    // Initialize the IdHelper
    StoreGateSvc* detStore = 0;
    sc = service("DetectorStore", detStore);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Can't locate the DetectorStore" << endmsg;
      return sc;
    }
    
    // get TGC ID helper
    sc = detStore->retrieve( m_tgcIdHelper, "TGCIDHELPER");
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not get TgcIdHelper !" << endmsg;
      return sc;
    }
    

    if (0==g_OUTCOINCIDENCE) {
      msg(MSG::INFO) << "You should make LVL1TGCTrigger::OUTCOINCIDENCE=1 in your jobOptions file" << endmsg;
      return StatusCode::FAILURE;
    }

    sc = bookHistos();
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "Cannot book histograms" << endmsg;
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS; 
  }
  
  StatusCode MakeCoincidenceOut::finalize()
  {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "MakeCoincidenceOut::finalize() called" << endmsg;
    return StatusCode::SUCCESS; 
  }
  
  
  StatusCode MakeCoincidenceOut::execute()
  {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "MakeCoincidenceOut::execute() called" << endmsg;

    const EventInfo * evtInfo=0;
    StatusCode sc = m_sgSvc->retrieve(evtInfo, "McEventInfo");
    if (sc.isFailure()) {
      msg(MSG::WARNING) << "Cannot retrieve EventInfo" << endmsg;
      m_runNumber=-1;
      m_eventNumber=-1;
    } else {
      m_runNumber   = (long int)(evtInfo->event_ID())->run_number();
      m_eventNumber = (long int)(evtInfo->event_ID())->event_number();
    }

    TGCCoincidences::iterator iss=g_TGCCOIN->begin(); 
    m_nhpt=0;
    if (g_TGCCOIN->size()>MaxNhpt) {
      msg(MSG::WARNING) << "Number of Hpt triggers is " << g_TGCCOIN->size() << ", more than " << MaxNhpt << endmsg;
    }
    while( iss!=g_TGCCOIN->end() && m_nhpt<MaxNhpt) {
      m_hbid  [m_nhpt]=(*iss)->getBid();
      m_hsec  [m_nhpt]=(*iss)->getSLid();
      m_hmod  [m_nhpt]=(*iss)->getModule();
      m_hreg  [m_nhpt]=(*iss)->getRegion();
      m_hssc  [m_nhpt]=(*iss)->getSSCid();
      m_hssr  [m_nhpt]=(*iss)->getR();
      m_hssphi[m_nhpt]=(*iss)->getPhi();
      m_hssid [m_nhpt]=(*iss)->getSSid();
      m_hptr  [m_nhpt]=(*iss)->getPtR();
      m_hdr   [m_nhpt]=(*iss)->getDR();
      m_hptphi[m_nhpt]=(*iss)->getPtPhi();
      m_hdphi [m_nhpt]=(*iss)->getDPhi();
      m_hptmax[m_nhpt]=(*iss)->getPtMax();
      m_nhpt++; iss++;
    }  

    const DataHandle<TgcDigitContainer> tgc_container;
    sc = m_sgSvc->retrieve(tgc_container, m_key);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << " Cannot retrieve TGC Digit Container " << endmsg;
      return sc;
    }

    // Loop on TGC detectors (collections)
    //MuonSpectrometer/MuonCnv/MuonByteStreamCnvTest
    m_ncol=(int)tgc_container->size();
    m_ndig=0;
    for (TgcDigitContainer::const_iterator c = tgc_container->begin(); c != tgc_container->end(); ++c) {
      TgcDigitCollection::const_iterator h = (*c)->begin(); 
      while ( h!=(*c)->end() && m_ndig<MaxNdig) {
	Identifier id = (*h)->identify();
	// ID information
	m_stationName[m_ndig] = m_tgcIdHelper->stationName(id);
	m_stationEta [m_ndig] = m_tgcIdHelper->stationEta(id);
	m_stationPhi [m_ndig] = m_tgcIdHelper->stationPhi(id);
	m_gasGap     [m_ndig] = m_tgcIdHelper->gasGap(id);
	m_isStrip    [m_ndig] = m_tgcIdHelper->isStrip(id);
	m_channel    [m_ndig] = m_tgcIdHelper->channel(id); 	
	m_ndig++;  h++;
      }
    }
    
    if (m_WriteMCtruth) {
      const DataHandle<McEventCollection> mcColl(0);
      sc = m_sgSvc->retrieve(mcColl,"TruthEvent");
      if (sc.isFailure() && !mcColl) {
        msg(MSG::WARNING) << "Cannot retrieve McEventCollection. McEventCollection is recorded in simulation file?" << endmsg;
      } else {
	McEventCollection::const_iterator itr;
	m_nmuMC=0;
	for(itr = mcColl->begin(); itr!=mcColl->end(); ++itr) {
          HepMC::GenEvent::particle_const_iterator Part = (*itr)->particles_begin(); 
	  while (Part !=(*itr)->particles_end() && m_nmuMC<MaxNmu) {
	    int status = (*Part)->status();
	    int barcode= (*Part)->barcode();
	    if ((status%1000==1 || (status%1000==2 && status>1000)) && barcode<100000) { // <-correct way
	      m_idMC[m_nmuMC]  = (int) (*Part)->pdg_id();
	      m_ptMC[m_nmuMC]  = (int) (*Part)->momentum().perp()/1000.; // in GeV
	      m_etaMC[m_nmuMC] = (*Part)->momentum().pseudoRapidity();
	      m_phiMC[m_nmuMC] = (*Part)->momentum().phi();
	      m_nmuMC++; 
	    }
	    Part++;
	  }
	}
      }
    }
    
    // write 
    sc = ntupleSvc()->writeRecord("/NTUPLES/FILE1/merge");  
    if (!sc.isSuccess()) { 
      msg(MSG::ERROR) << "Cannot fill ntuple" << endmsg;
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }  
  
  
  StatusCode MakeCoincidenceOut::bookHistos() {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "bookHistos is Called" << endmsg;

    StatusCode sc;
    
    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/merge");
    if ( !nt )    {    // Check if already booked //
      nt = ntupleSvc()->book ("/NTUPLES/FILE1/merge",
			      CLID_ColumnWiseTuple, "Hpt Dig");
      if ( nt )    {
	sc = nt->addItem ("runNumber",m_runNumber);
	sc = nt->addItem ("eventNumber",m_eventNumber);
	sc = nt->addItem ("nhpt",m_nhpt,0,MaxNhpt);
	sc = nt->addItem ("hsec",  m_nhpt,  m_hsec);
	sc = nt->addItem ("hbid",  m_nhpt,  m_hbid);
	sc = nt->addItem ("hmod",  m_nhpt,  m_hmod);
	sc = nt->addItem ("hreg",  m_nhpt,  m_hreg);
	sc = nt->addItem ("hssc",  m_nhpt,  m_hssc);
	sc = nt->addItem ("hssr",  m_nhpt,  m_hssr);
	sc = nt->addItem ("hssphi",m_nhpt,  m_hssphi);
	sc = nt->addItem ("hssid", m_nhpt,  m_hssid);
	sc = nt->addItem ("hptr",  m_nhpt,  m_hptr);
	sc = nt->addItem ("hdr",   m_nhpt,  m_hdr);
	sc = nt->addItem ("hptphi",m_nhpt,  m_hptphi);
	sc = nt->addItem ("hdphi", m_nhpt,  m_hdphi);
	sc = nt->addItem ("hptmax",m_nhpt,  m_hptmax);
	//
	sc = nt->addItem ("ncol"   ,m_ncol,0,MaxNcol);
	sc = nt->addItem ("ndig"   ,m_ndig,0,MaxNdig);
	sc = nt->addItem ("dstation",m_ndig,m_stationName);
	sc = nt->addItem ("deta",    m_ndig,m_stationEta);
	sc = nt->addItem ("dphi",    m_ndig,m_stationPhi);
	sc = nt->addItem ("dgasgap", m_ndig,m_gasGap);
	sc = nt->addItem ("disstrip",m_ndig,m_isStrip);
	sc = nt->addItem ("dchannel",m_ndig,m_channel);
	if (m_WriteMCtruth) {
	  if (sc.isSuccess()) sc = nt->addItem ("nmuMC",m_nmuMC,0,MaxNmu);
	  if (sc.isSuccess()) sc = nt->addItem ("idMC", m_nmuMC,m_idMC);
	  if (sc.isSuccess()) sc = nt->addItem ("ptMC", m_nmuMC,m_ptMC);
	  if (sc.isSuccess()) sc = nt->addItem ("etaMC", m_nmuMC,m_etaMC);
	  if (sc.isSuccess()) sc = nt->addItem ("phiMC", m_nmuMC,m_phiMC);
	}
      } else {
        msg(MSG::ERROR) << "Cannot book this histo" << endmsg;
	return StatusCode::FAILURE;
      }
      
      if (sc.isFailure()) {
        msg(MSG::ERROR) << "Error happens during add an item..." << endmsg;
	return sc;
      }
    }
    return StatusCode::SUCCESS;
  }

}

