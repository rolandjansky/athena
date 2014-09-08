// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEammaHypo/TrigL2CaloHypo.cxx
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after T2Calo 
 **
 **   Authour: R.Goncalo <r.goncalo@rhul.ac.uk> Thu Jan 20 22:02:43 BST 2005
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified: Eduardo J. Ortega <ortegaue@uregina.ca> Mon Mar  3 14:27:57 CST 2008
 **   Modified: Valerio Dao <valerio.dao@cern.ch> 12-10-2009 (added eta range cut+fixes)
 **
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2CaloHypo.h"

#include<cmath>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"

class ISvcLocator;


TrigL2CaloHypo::TrigL2CaloHypo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator)
{
  // Read cuts - should probably get these from an xml file
  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("EtaBins",        m_etabin);
  declareProperty("ETthr",          m_eTthr);
  declareProperty("ET2thr",         m_eT2thr);
  declareProperty("HADETthr",       m_hadeTthr);
  declareProperty("HADET2thr",      m_hadeT2thr);
  declareProperty("CARCOREthr",     m_carcorethr);
  declareProperty("CAERATIOthr",    m_caeratiothr);
  declareProperty("dETACLUSTERthr", m_detacluster=0.2);
  declareProperty("dPHICLUSTERthr", m_dphicluster=0.2);
  declareProperty("F1thr",          m_F1thr);  
  declareProperty("WETA2thr",       m_WETA2thr);
  declareProperty("WSTOTthr",       m_WSTOTthr);
  declareProperty("F3thr",          m_F3thr);

  // declare monitoring histograms for all cut variables
  declareMonitoredVariable("dEta", dEta);
  declareMonitoredVariable("dPhi", dPhi);
  declareMonitoredVariable("Et_em", eT_T2Calo);
  declareMonitoredVariable("Et_had", hadET_T2Calo);
  declareMonitoredVariable("Rcore", rCore);
  declareMonitoredVariable("Eratio", energyRatio);
  declareMonitoredVariable("EtaBin", etaBin);
  declareMonitoredVariable("Eta", monEta);
  declareMonitoredVariable("Phi", monPhi);
  declareMonitoredVariable("F1", F1); 
  declareMonitoredVariable("Weta2", Weta2);
  declareMonitoredVariable("Wstot", Wstot);
  declareMonitoredVariable("F3", Weta2);
  declareMonitoredVariable("CutCounter", PassedCuts);
}


TrigL2CaloHypo::~TrigL2CaloHypo()
{ }


HLT::ErrorCode TrigL2CaloHypo::hltInitialize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully"   << endreq;
    // default=True (for now...May06)
    msg() << MSG::DEBUG << "AcceptAll           = "
	<< (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "EtaBins        = " << m_etabin      << endreq;
    msg() << MSG::DEBUG << "ETthr          = " 
	<< m_eTthr    << "(lo)/" << m_eT2thr    << "(hi)" << endreq;  
    msg() << MSG::DEBUG << "HADETthr       = " 
	<< m_hadeTthr << "(lo)/" << m_hadeT2thr << "(hi)" << endreq;
    msg() << MSG::DEBUG << "CARCOREthr     = " << m_carcorethr  << endreq;
    msg() << MSG::DEBUG << "CAERATIOthr    = " << m_caeratiothr << endreq;
    msg() << MSG::DEBUG << "dPHICLUSTERthr = " << m_dphicluster << endreq;
    msg() << MSG::DEBUG << "dETACLUSTERthr = " << m_detacluster << endreq;
    msg() << MSG::DEBUG << "WETA2thr     = "   << m_WETA2thr  << endreq;
    msg() << MSG::DEBUG << "WSTOTthr     = "   << m_WSTOTthr  << endreq;
    msg() << MSG::DEBUG << "F3thr     = "      << m_F3thr  << endreq;

  }

  unsigned int nEtaBin=m_etabin.size();
  if ( m_eTthr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " etThr size is " <<  m_eTthr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( m_eT2thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " et2Thr size is " <<  m_eT2thr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_hadeTthr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " hadetThr size is " <<  m_hadeTthr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_hadeT2thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " hadet2Thr size is " <<  m_hadeT2thr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( m_carcorethr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " carcore size is " <<  m_carcorethr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( m_caeratiothr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " caeratio size is " <<  m_caeratiothr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_WETA2thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " Weta2 size is " <<  m_WETA2thr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }

   if ( m_WSTOTthr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " Wstot size is " <<  m_WSTOTthr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }

   if ( m_F3thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " Wstot size is " <<  m_WSTOTthr.size() << " but needs " << nEtaBin-1 << endreq;
    return StatusCode::FAILURE;
  }  
 
  return HLT::OK;
}

HLT::ErrorCode TrigL2CaloHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "In TrigL2CaloHypo::finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigL2CaloHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  // initialise monitoring variables for each event
  dPhi         = -1.0;
  dEta         = -1.0;
  eT_T2Calo    = -1.0;
  hadET_T2Calo = -1.0;
  rCore        = -1.0;
  energyRatio  = -1.0;
  Weta2        = -1.0;
  Wstot        = -1.0;
  F3        = -1.0;
  PassedCuts   = -1;
  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    pass = true;
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endreq;
    }
    return HLT::OK;
  } else {
    pass = false;
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	    << endreq;
    }
  }

  ///////////// get RoI descriptor ///////////////////////////////////////////////////////
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor) ;

  if (stat != HLT::OK) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    return stat;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  }

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range (probably not needed anymore)   
  if ( fabs(phiRef) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range
  
  // retrieve TrigEMCluster from the TE: must retrieve vector first
  std::vector< const xAOD::TrigEMCluster* > vectorOfClusters;  
  stat = getFeatures( outputTE, vectorOfClusters );

  if ( stat != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get TrigEMClusters" << endreq;
 
    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Found vector with " << vectorOfClusters.size() 
	<< " TrigEMClusters" << endreq;
  }
  
  // should be only 1 cluster, normally!
  if (vectorOfClusters.size() != 1) {
    msg() << MSG::DEBUG << "Size of vector of TrigEMClusters is not 1!" 
	<< endreq;
    return HLT::OK;
  }

  // get cluster
  const xAOD::TrigEMCluster* pClus = vectorOfClusters.front();

  if ( !pClus ) {
    msg() << MSG::WARNING << "No EM cluster in RoI" << endreq;
    return HLT::OK;
  }

  // increment event counter 
  PassedCuts++; //// the ROI at least contais the cluster

  float absEta = fabs( pClus->eta() );
  etaBin = -1;
  monEta = pClus->eta();
  monPhi = pClus->phi();
  for (std::size_t iBin = 0; iBin < m_etabin.size()-1; iBin++)
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin; 
 
  // find if electron is in calorimeter crack
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );

  
  dEta =  pClus->eta() - etaRef;
  // Deal with angle diferences greater than Pi
  dPhi =  fabs(pClus->phi() - phiRef);
  dPhi = (dPhi < M_PI ? dPhi : 2*M_PI - dPhi );


  // calculate cluster quantities // definition taken from TrigElectron constructor     
  if ( pClus->emaxs1() + pClus->e2tsts1() > 0 ) 
    energyRatio = ( pClus->emaxs1() - pClus->e2tsts1() ) / ( pClus->emaxs1() + pClus->e2tsts1() );

  // (VD) here the definition is a bit different to account for the cut of e277 @ EF 
  if ( pClus->e277()!= 0.) rCore = pClus->e237() / pClus->e277();

  //fraction of energy deposited in 1st sampling
  if ( fabs(pClus->energy()) > 0.00001) F1 = (pClus->energy(CaloSampling::EMB1)+pClus->energy(CaloSampling::EME1))/pClus->energy();

  eT_T2Calo  = pClus->et();
  
  if ( eT_T2Calo!=0 && pClus->eta()!=0 ) hadET_T2Calo = pClus->ehad1()/cosh(fabs(pClus->eta()))/eT_T2Calo;
 

  //extract Weta2 varable
  Weta2 = pClus->weta2();

  //extract Wstot varable
  Wstot = pClus->wstot();

  //extract F3 (backenergy i EM calorimeter
  float e0 = pClus->energy(CaloSampling::PreSamplerB) + pClus->energy(CaloSampling::PreSamplerE);
  float e1 = pClus->energy(CaloSampling::EMB1) + pClus->energy(CaloSampling::EME1);
  float e2 = pClus->energy(CaloSampling::EMB2) + pClus->energy(CaloSampling::EME2);
  float e3 = pClus->energy(CaloSampling::EMB3) + pClus->energy(CaloSampling::EME3);
  float eallsamples = e0+e1+e2+e3;
  F3= fabs(eallsamples)>0. ? e3/eallsamples : 0.; 

  // apply cuts: DeltaEta(clus-ROI)
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG  << "TrigEMCluster: eta="  << pClus->eta()
	  << " roi eta=" << etaRef << " DeltaEta=" << dEta
	  << " cut: <"   << m_detacluster          << endreq;
  }
  if ( fabs(pClus->eta() - etaRef) > m_detacluster ) return HLT::OK;
  PassedCuts++;
  
  // DeltaPhi(clus-ROI)
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << ": phi="  << pClus->phi()
	  << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
	  << " cut: <"  << m_dphicluster            << endreq;
  }
  if( dPhi > m_dphicluster ) return HLT::OK;
  PassedCuts++;

  // eta range
  if ( etaBin==-1 ) {  // VD
    msg() << MSG::DEBUG << "Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] << endreq;
    return HLT::OK;
  } else { 
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "eta bin used for cuts " << etaBin << endreq;
  }
  PassedCuts++; // passed eta cut
  
  // Rcore
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Rcore=" << rCore 
	  << " cut: >"  << m_carcorethr[etaBin] << endreq;
  if ( rCore < m_carcorethr[etaBin] )  return HLT::OK;
  PassedCuts++;

  // Eratio
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Eratio=" << energyRatio
	  << " cut: >"  << m_caeratiothr[etaBin] << endreq;   
  if ( inCrack || F1<m_F1thr[0] ) {
    msg() << MSG::DEBUG << "TrigEMCluster: InCrack= " << inCrack << " F1=" << F1 << endreq;
  } else {
    if ( energyRatio < m_caeratiothr[etaBin] ) return HLT::OK;
  }
  PassedCuts++;
  if(inCrack) energyRatio = -1; //Set default value in crack for monitoring.
  
  // ET_em
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: ET_em=" << eT_T2Calo
	<< " cut: >"  << m_eTthr[etaBin] << endreq;
  if ( eT_T2Calo < m_eTthr[etaBin]) return HLT::OK;
  PassedCuts++;
 
  float hadET_cut = 0.0;  
  // find which ET_had to apply	: this depends on the ET_em and the eta bin
  if ( eT_T2Calo >  m_eT2thr[etaBin] ) {
    hadET_cut = m_hadeT2thr[etaBin] ;

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "ET_em>"     << m_eT2thr[etaBin]
	  << ": use high ET_had cut: <" << hadET_cut << endreq;
  } else {
    hadET_cut = m_hadeTthr[etaBin];

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "ET_em<"    << m_eT2thr[etaBin] 
	  << ": use low ET_had cut: <" << hadET_cut << endreq;
  }
  
  // ET_had
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: ET_had=" << hadET_T2Calo
	<< " cut: <" << hadET_cut << endreq;

  if ( hadET_T2Calo > hadET_cut ) return HLT::OK;
  PassedCuts++;
  
  // F1
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: F1=" << F1
	  << " cut: >"  << m_F1thr[0] << endreq;
  // if ( F1 < m_F1thr[0]) return HLT::OK;  //(VD) not cutting on this variable, only used to select whether to cut or not on eRatio
  PassedCuts++;


  //Weta2
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Weta2=" << Weta2
	  << " cut: <"  << m_WETA2thr[etaBin] << endreq; 
  if ( Weta2 > m_WETA2thr[etaBin]) return HLT::OK;
  PassedCuts++;


  //Wstot
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Wstot=" << Wstot
	  << " cut: <"  << m_WSTOTthr[etaBin] << endreq; 
  if ( Wstot >= m_WSTOTthr[etaBin]) return HLT::OK;
  PassedCuts++;

  //F3
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: F3=" << F3
	  << " cut: <"  << m_F3thr[etaBin] << endreq; 
  if ( F3 > m_F3thr[etaBin]) return HLT::OK;
  PassedCuts++;


  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "pass = " << pass << endreq;

  // (VD) this is quite obvious - it would be better to be able to arrive here eve if fails and see the line FAIL!

  return HLT::OK;
}

