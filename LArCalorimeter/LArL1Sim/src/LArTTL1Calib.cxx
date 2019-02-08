/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
// ........ Gaudi needed includes
//
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/INTupleSvc.h"
//#include "GaudiKernel/NTupleDirectory.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "StoreGate/StoreGateSvc.h"

//

LArTTL1Calib::LArTTL1Calib(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
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
  ATH_MSG_DEBUG ( "in initialize()" );

  NTuple::Directory* dir = 0;
  NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");

  if ( file1 != 0) {
    if ( !(dir=ntupleSvc()->createDirectory("/NTUPLES/FILE1/DIR")) ) {
      ATH_MSG_ERROR ( "Cannot create directory /NTUPLES/FILE1/DIR" );
    }
  }
  else {
    ATH_MSG_ERROR ( "Cannot access file /NTUPLES/FILE1" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "created directory /NTUPLES/FILE1/DIR" );
      
  NTuplePtr pnt(ntupleSvc(),"/NTUPLES/FILE1/DIR/10");
  if (!pnt) {
    pnt = ntupleSvc()->book(dir,10 ,CLID_ColumnWiseTuple,"LArLvl1TT");
    
    if(pnt){
      ATH_MSG_DEBUG ( "booked ntuple LArLvl1TT" );
      m_pnt = pnt;

      ATH_CHECK( pnt->addItem ("nTotEm",m_ntotem) );
      ATH_CHECK( pnt->addItem ("EtTotEm",m_etotem) );
      ATH_CHECK( pnt->addItem ("n3x3Em",m_n3x3em) );
      ATH_CHECK( pnt->addItem ("E3x3Em",m_e3x3em) );
      ATH_CHECK( pnt->addItem ("EmostEm",m_emostem) );
      ATH_CHECK( pnt->addItem ("BarrelECEm",m_bec_mostEem) );
      ATH_CHECK( pnt->addItem ("EmFcalEm",m_emfcal_mostEem) );
      ATH_CHECK( pnt->addItem ("IetaEm",m_Ieta_mostEem) );
      ATH_CHECK( pnt->addItem ("LphiEm",m_Lphi_mostEem) );
      ATH_CHECK( pnt->addItem ("nTTaboveThrEm",m_nttem,0,m_maxNtt) );
      ATH_CHECK( pnt->addItem ("EtTTEm",m_nttem,m_ettem) );
      ATH_CHECK( pnt->addItem ("BECTTEm",m_nttem,m_becttem) );
      ATH_CHECK( pnt->addItem ("EmFcalTTEm",m_nttem,m_emfcalttem) );
      ATH_CHECK( pnt->addItem ("IetaTTEm",m_nttem,m_Ietattem) );
      ATH_CHECK( pnt->addItem ("LphiTTEm",m_nttem,m_Lphittem) );

      ATH_CHECK( pnt->addItem ("nTotHad",m_ntothad) );
      ATH_CHECK( pnt->addItem ("EtTotHad",m_etothad) );
      ATH_CHECK( pnt->addItem ("n3x3Had",m_n3x3had) );
      ATH_CHECK( pnt->addItem ("E3x3Had",m_e3x3had) );
      ATH_CHECK( pnt->addItem ("EmostHad",m_emosthad) );
      ATH_CHECK( pnt->addItem ("HecFcalHad",m_hecfcal_mostEhad) );
      ATH_CHECK( pnt->addItem ("IetaHad",m_Ieta_mostEhad) );
      ATH_CHECK( pnt->addItem ("LphiHad",m_Lphi_mostEhad) );
      ATH_CHECK( pnt->addItem ("nTTaboveThrHad",m_ntthad,0,m_maxNtt) );
      ATH_CHECK( pnt->addItem ("EtTTHad",m_ntthad,m_etthad) );
      ATH_CHECK( pnt->addItem ("HecFcalTTHad",m_ntthad,m_hecfcaltthad) );
      ATH_CHECK( pnt->addItem ("IetaTTHad",m_ntthad,m_Ietatthad) );
      ATH_CHECK( pnt->addItem ("LphiTTHad",m_ntthad,m_Lphitthad) );
      
    } else {
      ATH_MSG_ERROR  ( "could not book Ntuple" );
    }
  } else {
    ATH_MSG_ERROR ( "Ntuple is already booked" );
  }
//

  ATH_CHECK( detStore()->retrieve (m_lvl1Helper) );

  ATH_MSG_DEBUG  ( "Initialization completed successfully" );
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
  ATH_MSG_DEBUG ( "Begining of execution" );

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
    ATH_MSG_DEBUG ( " asking for: " << containerVec[iTTL1Container] );
    
    const LArTTL1Container* ttl1_container ;
    ATH_CHECK( evtStore()->retrieve( ttl1_container, containerVec[iTTL1Container] ) );

    //
    ATH_MSG_DEBUG ( "number of ttl1s: " << ttl1_container->size() );

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
      float e = sampleV[3] * 1e-3 ; // go to GeV 
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
	    ATH_MSG_WARNING ( "Too many em TT. Save only  " << m_maxNtt );
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
	    ATH_MSG_WARNING ( "Too many had TT. Save only  " << m_maxNtt );
	    break;
	  }
	}
      }
      
    } // .... end of loop over ittl1s
    ATH_MSG_DEBUG ( "number of em/had ttl1s: " << indexem << " / " << indexhad);


    int etaLim=13; // barrel-ec transition
    if(hecfcal_mostE >0 ) {etaLim=14;}
    if(samp == 0) {
      // em:
      m_ntotem =  ttl1_container->size() ;
      m_nttem = indexem;
      for (int i=0; i<indexem; ++i) {
	int deltaEta = std::abs(Ieta_mostE - m_Ietattem[i]);
	int deltaPhi = std::abs(Lphi_mostE - m_Lphittem[i]);           // not quite 3x3
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
	int deltaEta = std::abs( Ieta_mostE- m_Ietatthad[i]);
	int deltaPhi = std::abs( Lphi_mostE- m_Lphitthad[i]);
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
  ATH_MSG_DEBUG ( "end of loop over containers, write record" );

  ATH_CHECK( ntupleSvc()->writeRecord(m_pnt) );
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
  ATH_MSG_DEBUG ( " LArTTL1Calib finalize completed successfully" );
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


