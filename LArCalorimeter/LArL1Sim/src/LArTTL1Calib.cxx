/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Institut ......: ISN Grenoble                                        +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArL1Sim/LArTTL1Calib.h"
// .......... utilities
//

#include "LArRawEvent/LArTTL1.h"
#include "LArRawEvent/LArTTL1Container.h"

#include "CaloIdentifier/CaloLVL1_ID.h"
//
// ........ Event Header Files:
//
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
//
// ........ Gaudi needed includes
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/INTupleSvc.h"
//#include "GaudiKernel/NTupleDirectory.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "StoreGate/StoreGateSvc.h"

//

LArTTL1Calib::LArTTL1Calib(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
// + -------------------------------------------------------------------- +
// + Author ........: F. Ledroit                                          +
// + Creation date .: 20/04/2004                                          +
// + Subject: TTL1 Calib constructor                                      +
// + -------------------------------------------------------------------- +
{
//
// ........ default values of private data
//
//  m_thresholdGeVTTs       = 0.1;    // pb here: threshold depends on whether calib already applied or not...
//  m_thresholdGeVTTs       = 0.001;  // and whether want to study calib or noise
  m_thresholdGeVTTs       = 0.; 
  m_maxNtt                = 8000; 
  //  m_maxNtt                = 500; 

  m_pnt                   = 0;

  m_storeGateSvc          = 0;
  m_lvl1Helper            = 0;

//
  return;
}


LArTTL1Calib::~LArTTL1Calib()
{  
  /**  TTL1 Calib destructor          */

return;
}


StatusCode LArTTL1Calib::initialize()
{
// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  MsgStream  msglog(messageService(),name());
  msglog << MSG::DEBUG << "in initialize()" << endreq;

  NTuple::Directory* dir = 0;
  NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");

  if ( file1 != 0) {
    if ( !(dir=ntupleSvc()->createDirectory("/NTUPLES/FILE1/DIR")) ) {
      msglog << MSG::ERROR << "Cannot create directory /NTUPLES/FILE1/DIR" << endreq;
    }
  }
  else {
    msglog << MSG::ERROR << "Cannot access file /NTUPLES/FILE1" << endreq;
    return StatusCode::FAILURE;
  }

  msglog << MSG::DEBUG << "created directory /NTUPLES/FILE1/DIR" << endreq;
      
  NTuplePtr pnt(ntupleSvc(),"/NTUPLES/FILE1/DIR/10");
  if (!pnt) {
    pnt = ntupleSvc()->book(dir,10 ,CLID_ColumnWiseTuple,"LArLvl1TT");
    
    if(pnt){
      msglog << MSG::DEBUG << "booked ntuple LArLvl1TT" << endreq;
      m_pnt = pnt;
      StatusCode sc = pnt->addItem ("nTotEm",m_ntotem);
      if ( sc == StatusCode::FAILURE ) {
	msglog<<MSG::ERROR << "   could not add item to col wise ntuple" << endreq;
	return StatusCode::FAILURE;
      }
    
      sc = pnt->addItem ("EtTotEm",m_etotem);
      sc = pnt->addItem ("n3x3Em",m_n3x3em);
      sc = pnt->addItem ("E3x3Em",m_e3x3em);
      sc = pnt->addItem ("EmostEm",m_emostem);
      sc = pnt->addItem ("BarrelECEm",m_bec_mostEem);
      sc = pnt->addItem ("EmFcalEm",m_emfcal_mostEem);
      sc = pnt->addItem ("IetaEm",m_Ieta_mostEem);
      sc = pnt->addItem ("LphiEm",m_Lphi_mostEem);
      sc = pnt->addItem ("nTTaboveThrEm",m_nttem,0,m_maxNtt);
      sc = pnt->addItem ("EtTTEm",m_nttem,m_ettem);
      sc = pnt->addItem ("BECTTEm",m_nttem,m_becttem);
      sc = pnt->addItem ("EmFcalTTEm",m_nttem,m_emfcalttem);
      sc = pnt->addItem ("IetaTTEm",m_nttem,m_Ietattem);
      sc = pnt->addItem ("LphiTTEm",m_nttem,m_Lphittem);

      sc = pnt->addItem ("nTotHad",m_ntothad);
      sc = pnt->addItem ("EtTotHad",m_etothad);
      sc = pnt->addItem ("n3x3Had",m_n3x3had);
      sc = pnt->addItem ("E3x3Had",m_e3x3had);
      sc = pnt->addItem ("EmostHad",m_emosthad);
      sc = pnt->addItem ("HecFcalHad",m_hecfcal_mostEhad);
      sc = pnt->addItem ("IetaHad",m_Ieta_mostEhad);
      sc = pnt->addItem ("LphiHad",m_Lphi_mostEhad);
      sc = pnt->addItem ("nTTaboveThrHad",m_ntthad,0,m_maxNtt);
      sc = pnt->addItem ("EtTTHad",m_ntthad,m_etthad);
      sc = pnt->addItem ("HecFcalTTHad",m_ntthad,m_hecfcaltthad);
      sc = pnt->addItem ("IetaTTHad",m_ntthad,m_Ietatthad);
      sc = pnt->addItem ("LphiTTHad",m_ntthad,m_Lphitthad);
      
    } else {
      msglog << MSG::ERROR  << "could not book Ntuple" << endreq;
    }
  } else {
    msglog << MSG::ERROR << "Ntuple is already booked" << endreq;
  }
//
// ....... Access Event StoreGate
//
  StatusCode sc = service ( "StoreGateSvc" , m_storeGateSvc ) ;
  if (sc.isFailure()) 
  {
    msglog << MSG::ERROR
	   << "Unable to access pointer to StoreGate"
	   << endreq;
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore = 0;
  const CaloLVL1_ID*	lvl1Helper;
  sc = service( "DetectorStore", detStore );
    if ( sc.isSuccess( ) ) {
	sc = detStore->retrieve(lvl1Helper);
	if (sc.isFailure()) {
	    msglog << MSG::ERROR << "Unable to retrieve CaloLVL1_ID from DetectorStore" << endreq;
	    return StatusCode::FAILURE;
	} else {
	    msglog << MSG::DEBUG << "Successfully retrieved CaloLVL1_ID from DetectorStore" << endreq;
	}	
    } 
    else {
	msglog << MSG::ERROR << "Could not locate DetectorStore" << endreq;
	return StatusCode::FAILURE;
    }
    m_lvl1Helper = lvl1Helper;

  msglog << MSG::DEBUG 
	 << "Initialization completed successfully" 
	 << endreq;


  return StatusCode::SUCCESS;

}





StatusCode LArTTL1Calib::execute()
{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+
  //
  // ......... declarations
  //
  MsgStream  msglog(messageService(),name());

  msglog << MSG::DEBUG 
	 << "Begining of execution" 
	 << endreq;

  LArTTL1Container::const_iterator ttl1iter;
  Identifier ttOffId;

  std::vector<std::string> containerVec;    // FIX ME USING PROPERTIES !!!
  containerVec.push_back("LArTTL1EM");
  containerVec.push_back("LArTTL1HAD");


//
// ............ loop over the wanted ttl1 containers (1 em, 1 had)
//

  for (unsigned int iTTL1Container=0;iTTL1Container<containerVec.size();iTTL1Container++)
  {
  //
  // ..... Get the pointer to the TTL1 Container from StoreGate
  //
    msglog << MSG::DEBUG << " asking for: " 
	   << containerVec[iTTL1Container] 
	   << endreq;
    
    const LArTTL1Container* ttl1_container ;
    StatusCode sc = m_storeGateSvc->retrieve( ttl1_container, containerVec[iTTL1Container] ) ;
    
    if ( sc.isFailure() || !ttl1_container) 
      {
      msglog << MSG::ERROR << "Could not retrieve a LArTTL1Container " 
	     << containerVec[iTTL1Container] 
	     << endreq;
      return StatusCode::FAILURE;
    }

    //
    msglog << MSG::DEBUG 
	   << "number of ttl1s: " << ttl1_container->size() 
	   << endreq;

    int samp = iTTL1Container;
    int indexem=0;
    int indexhad=0;

    float e_mostE=0.;
    int bec_mostE=99;
    int hecfcal_mostE=99;
    int Ieta_mostE=99;
    int Lphi_mostE=99;
    int phiMax_mostE = -99;

    //
    // ....... loop over ttl1s and get informations
    //

    for(ttl1iter=ttl1_container->begin();
	ttl1iter != ttl1_container->end();ttl1iter++)     {
      //      ttOnlId = (*ttl1iter)->ttOnlineID();
      ttOffId = (*ttl1iter)->ttOfflineID();
      std::vector<float> sampleV = (*ttl1iter)->samples();
      float e = sampleV[3] / 1000. ; // go to GeV 
      int reg = m_lvl1Helper->region(ttOffId);
      int eta = m_lvl1Helper->eta(ttOffId);
      int bec=99;
      int hecfcal=99;
      int Ieta=99;
      decodeInverseTTChannel(reg, samp, eta, bec, hecfcal, Ieta);
      int Lphi= m_lvl1Helper->phi(ttOffId) + 1;
      int phiMax = m_lvl1Helper->phi_max(ttOffId);
      if(e>e_mostE) {
	e_mostE=e;
	bec_mostE = bec;
	hecfcal_mostE = hecfcal;
	Ieta_mostE = Ieta;
	Lphi_mostE = Lphi;
	phiMax_mostE = phiMax;
      }
      
      if(samp == 0) {
	// EM towers
	m_etotem += e;
	if(indexem < m_maxNtt) {
	  if(e > m_thresholdGeVTTs) {
	    m_ettem[indexem] = e;
	    m_becttem[indexem] = bec;
	    m_emfcalttem[indexem] = hecfcal;
	    m_Ietattem[indexem] = Ieta;
	    m_Lphittem[indexem] = Lphi;
	    indexem++;
	  }
	}
	else {
	  if (m_maxNtt == indexem) {
	    msglog << MSG::WARNING
		   << "Too many em TT. Save only  " << m_maxNtt
		   << endreq;
	    break;
	  }
	}
      }
      else {
	// HAD towers
	m_etothad += e;
	if(indexhad < m_maxNtt) {
	  if(e > m_thresholdGeVTTs) {
	    m_etthad[indexhad] = e;
	    m_hecfcaltthad[indexhad] = hecfcal;
	    m_Ietatthad[indexhad] = Ieta;
	    m_Lphitthad[indexhad] = Lphi;
	    indexhad++;
	  }
	}
	else {
	  if (m_maxNtt == indexhad) {
	    msglog << MSG::WARNING
		   << "Too many had TT. Save only  " << m_maxNtt
		   << endreq;
	    break;
	  }
	}
      }
      
    } // .... end of loop over ittl1s
    msglog << MSG::DEBUG 
	   << "number of em/had ttl1s: " << indexem << " / " << indexhad
	   << endreq;


    int etaLim=13; // barrel-ec transition
    if(hecfcal_mostE >0 ) {etaLim=14;}
    if(samp == 0) {
      // em:
      m_ntotem =  ttl1_container->size() ;
      m_nttem = indexem;
      for (int i=0; i<indexem; ++i) {
	int deltaEta = abs(Ieta_mostE - m_Ietattem[i]);
	int deltaPhi = abs(Lphi_mostE - m_Lphittem[i]);           // not quite 3x3
	if(    (deltaEta <= 1 || deltaEta >= etaLim)                  // because phi granularity changes
	       && (deltaPhi <= 1 || deltaPhi == phiMax_mostE) ) { // at eta=2.5 and 3.2
	  m_n3x3em +=1;
	  m_e3x3em += m_ettem[i];
	}
      }
      m_emostem = e_mostE;
      m_bec_mostEem = bec_mostE;
      m_emfcal_mostEem = hecfcal_mostE;
      m_Ieta_mostEem = Ieta_mostE;
      m_Lphi_mostEem = Lphi_mostE;
    }
    else {
      // had:
      m_ntothad =  ttl1_container->size() ;
      m_ntthad = indexhad;
      for (int i=0; i<indexhad; ++i) {
	int deltaEta = abs( Ieta_mostE- m_Ietatthad[i]);
	int deltaPhi = abs( Lphi_mostE- m_Lphitthad[i]);
	if(  (deltaEta <= 1 || deltaEta >=etaLim)
	    && (deltaPhi <= 1 || deltaPhi == phiMax_mostE) ) {
	  m_n3x3had +=1;
	  m_e3x3had += m_etthad[i];
	}
      }
      m_emosthad = e_mostE;
      m_hecfcal_mostEhad = hecfcal_mostE;
      m_Ieta_mostEhad = Ieta_mostE;
      m_Lphi_mostEhad = Lphi_mostE;
    }
      
  } // .... end of loop over containers
  msglog << MSG::DEBUG 
	 << "end of loop over containers, write record"
	 << endreq;

  StatusCode sc = ntupleSvc()->writeRecord(m_pnt);
  if (sc!=StatusCode::SUCCESS) {
    msglog << MSG::ERROR << "writeRecord failed" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
    
}


StatusCode LArTTL1Calib::finalize()
{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  MsgStream  msglog(messageService(),name());

//
  msglog << MSG::DEBUG << " LArTTL1Calib finalize completed successfully" 
	 << endreq;

//
  return StatusCode::SUCCESS;

}

void 
LArTTL1Calib::decodeInverseTTChannel(int region, int layer, int eta, int & bec, int & emhf, int & Ieta)  const
{
  // fix me !!! this method is duplicated from LArCablingService
  if(region == 0){
    if(eta <= 14) {
      bec=0;         // EM barrel
      emhf=0;
      Ieta=eta+1;         
    }
    else {
      bec=1;                // EMEC or HEC
      emhf=layer;                
      Ieta=eta-13;
    }
  }
  else if( region == 1 || region == 2) {
    bec=1;
    emhf = layer ;
    if(region == 1) {   
      Ieta=eta+12;
    }
    else {   
      Ieta=15;
    }
  }
  else if(region == 3) {  // FCAL only 
    bec=1;
    emhf=2+layer;
    Ieta=eta+1;
  }
}


