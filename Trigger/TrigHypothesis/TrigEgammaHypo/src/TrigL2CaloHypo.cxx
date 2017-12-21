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
  declareMonitoredVariable("dEta", m_dEta);
  declareMonitoredVariable("dPhi", m_dPhi);
  declareMonitoredVariable("Et_em", m_eT_T2Calo);
  declareMonitoredVariable("Et_had", m_hadET_T2Calo);
  declareMonitoredVariable("Rcore", m_rCore);
  declareMonitoredVariable("Eratio", m_energyRatio);
  declareMonitoredVariable("EtaBin", m_etaBin);
  declareMonitoredVariable("Eta", m_monEta);
  declareMonitoredVariable("Phi", m_monPhi);
  declareMonitoredVariable("F1", m_F1); 
  declareMonitoredVariable("Weta2", m_Weta2);
  declareMonitoredVariable("Wstot", m_Wstot);
  declareMonitoredVariable("F3", m_F3);
  declareMonitoredVariable("CutCounter", m_PassedCuts);
}


TrigL2CaloHypo::~TrigL2CaloHypo()
{ }


HLT::ErrorCode TrigL2CaloHypo::hltInitialize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully"   << endmsg;
    // default=True (for now...May06)
    msg() << MSG::DEBUG << "AcceptAll           = "
	<< (m_acceptAll==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "EtaBins        = " << m_etabin      << endmsg;
    msg() << MSG::DEBUG << "ETthr          = " 
	<< m_eTthr    << "(lo)/" << m_eT2thr    << "(hi)" << endmsg;  
    msg() << MSG::DEBUG << "HADETthr       = " 
	<< m_hadeTthr << "(lo)/" << m_hadeT2thr << "(hi)" << endmsg;
    msg() << MSG::DEBUG << "CARCOREthr     = " << m_carcorethr  << endmsg;
    msg() << MSG::DEBUG << "CAERATIOthr    = " << m_caeratiothr << endmsg;
    msg() << MSG::DEBUG << "dPHICLUSTERthr = " << m_dphicluster << endmsg;
    msg() << MSG::DEBUG << "dETACLUSTERthr = " << m_detacluster << endmsg;
    msg() << MSG::DEBUG << "WETA2thr     = "   << m_WETA2thr  << endmsg;
    msg() << MSG::DEBUG << "WSTOTthr     = "   << m_WSTOTthr  << endmsg;
    msg() << MSG::DEBUG << "F3thr     = "      << m_F3thr  << endmsg;

  }

  unsigned int nEtaBin=m_etabin.size();
  if ( m_eTthr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " etThr size is " <<  m_eTthr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  if ( m_eT2thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " et2Thr size is " <<  m_eT2thr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if ( m_hadeTthr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " hadetThr size is " <<  m_hadeTthr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if ( m_hadeT2thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " hadet2Thr size is " <<  m_hadeT2thr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  if ( m_carcorethr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " carcore size is " <<  m_carcorethr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  if ( m_caeratiothr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " caeratio size is " <<  m_caeratiothr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if ( m_WETA2thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " Weta2 size is " <<  m_WETA2thr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

   if ( m_WSTOTthr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " Wstot size is " <<  m_WSTOTthr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

   if ( m_F3thr.size() != nEtaBin-1 ) {
    msg() << MSG::ERROR << " Wstot size is " <<  m_WSTOTthr.size() << " but needs " << nEtaBin-1 << endmsg;
    return HLT::BAD_JOB_SETUP;
  }  
 
  return HLT::OK;
}

HLT::ErrorCode TrigL2CaloHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "In TrigL2CaloHypo::finalize()" << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigL2CaloHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  // initialise monitoring variables for each event
  m_dPhi         = -1.0;
  m_dEta         = -1.0;
  m_eT_T2Calo    = -1.0;
  m_hadET_T2Calo = -1.0;
  m_rCore        = -1.0;
  m_energyRatio  = -1.0;
  m_Weta2        = -1.0;
  m_Wstot        = -1.0;
  m_F3        = -1.0;
  m_PassedCuts   = -1;
  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    pass = true;
    ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
  } else {
    pass = false;
    ATH_MSG_DEBUG("AcceptAll property not set: applying selection");
  }

  ///////////// get RoI descriptor ///////////////////////////////////////////////////////
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    ATH_MSG_WARNING("No RoI for this Trigger Element! ");
    return HLT::NAV_ERROR;
  }
  
  if ( fabs(roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG("The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now");
      pass=false; // special case 
      return HLT::OK; 
  } 

  ATH_MSG_DEBUG( "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
          << "; RoI ID = "   << roiDescriptor->roiId()
          << ": Eta = "      << roiDescriptor->eta()
          << ", Phi = "      << roiDescriptor->phi());

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range (probably not needed anymore)   
  if ( fabs(phiRef) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range
  
  // retrieve TrigEMCluster from the TE: must retrieve vector first
  std::vector< const xAOD::TrigEMCluster* > vectorOfClusters;  
  HLT::ErrorCode stat = getFeatures( outputTE, vectorOfClusters );

  if ( stat != HLT::OK ) {
      ATH_MSG_WARNING("Failed to get TrigEMClusters");
      return HLT::OK;
  }

  ATH_MSG_DEBUG("Found vector with " << vectorOfClusters.size() << " TrigEMClusters");
	
  
  // should be only 1 cluster, normally!
  if (vectorOfClusters.size() != 1) {
    msg() << MSG::DEBUG << "Size of vector of TrigEMClusters is not 1!" 
	<< endmsg;
    return HLT::OK;
  }
  m_PassedCuts++; //Input
  // get cluster
  const xAOD::TrigEMCluster* pClus = vectorOfClusters.front();

  if ( !pClus ) {
    msg() << MSG::WARNING << "No EM cluster in RoI" << endmsg;
    return HLT::OK;
  }

  // increment event counter 
  m_PassedCuts++; //// the ROI at least contais the cluster

  float absEta = fabs( pClus->eta() );
  m_etaBin = -1;
  m_monEta = pClus->eta();
  m_monPhi = pClus->phi();
  for (std::size_t iBin = 0; iBin < m_etabin.size()-1; iBin++)
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) m_etaBin = iBin; 
 
  // find if electron is in calorimeter crack
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );

  
  m_dEta =  pClus->eta() - etaRef;
  // Deal with angle diferences greater than Pi
  m_dPhi =  fabs(pClus->phi() - phiRef);
  m_dPhi = (m_dPhi < M_PI ? m_dPhi : 2*M_PI - m_dPhi );


  // calculate cluster quantities // definition taken from TrigElectron constructor     
  if ( pClus->emaxs1() + pClus->e2tsts1() > 0 ) 
    m_energyRatio = ( pClus->emaxs1() - pClus->e2tsts1() ) / ( pClus->emaxs1() + pClus->e2tsts1() );

  // (VD) here the definition is a bit different to account for the cut of e277 @ EF 
  if ( pClus->e277()!= 0.) m_rCore = pClus->e237() / pClus->e277();

  //fraction of energy deposited in 1st sampling
  if ( fabs(pClus->energy()) > 0.00001) m_F1 = (pClus->energy(CaloSampling::EMB1)+pClus->energy(CaloSampling::EME1))/pClus->energy();

  m_eT_T2Calo  = pClus->et();
  
  if ( m_eT_T2Calo!=0 && pClus->eta()!=0 ) m_hadET_T2Calo = pClus->ehad1()/cosh(fabs(pClus->eta()))/m_eT_T2Calo;
 

  //extract Weta2 varable
  m_Weta2 = pClus->weta2();

  //extract Wstot varable
  m_Wstot = pClus->wstot();

  //extract F3 (backenergy i EM calorimeter
  float e0 = pClus->energy(CaloSampling::PreSamplerB) + pClus->energy(CaloSampling::PreSamplerE);
  float e1 = pClus->energy(CaloSampling::EMB1) + pClus->energy(CaloSampling::EME1);
  float e2 = pClus->energy(CaloSampling::EMB2) + pClus->energy(CaloSampling::EME2);
  float e3 = pClus->energy(CaloSampling::EMB3) + pClus->energy(CaloSampling::EME3);
  float eallsamples = e0+e1+e2+e3;
  m_F3= fabs(eallsamples)>0. ? e3/eallsamples : 0.; 

  // apply cuts: DeltaEta(clus-ROI)
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG  << "TrigEMCluster: eta="  << pClus->eta()
	  << " roi eta=" << etaRef << " DeltaEta=" << m_dEta
	  << " cut: <"   << m_detacluster          << endmsg;
  }
  if ( fabs(pClus->eta() - etaRef) > m_detacluster ) return HLT::OK;
  m_PassedCuts++; //Deta
  
  // DeltaPhi(clus-ROI)
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << ": phi="  << pClus->phi()
	  << " roi phi="<< phiRef    << " DeltaPhi="<< m_dPhi
	  << " cut: <"  << m_dphicluster            << endmsg;
  }
  if( m_dPhi > m_dphicluster ) return HLT::OK;
  m_PassedCuts++; //DPhi

  // eta range
  if ( m_etaBin==-1 ) {  // VD
    msg() << MSG::DEBUG << "Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] << endmsg;
    return HLT::OK;
  } else { 
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "eta bin used for cuts " << m_etaBin << endmsg;
  }
  m_PassedCuts++; // passed eta cut
  
  // Rcore
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Rcore=" << m_rCore 
	  << " cut: >"  << m_carcorethr[m_etaBin] << endmsg;
  if ( m_rCore < m_carcorethr[m_etaBin] )  return HLT::OK;
  m_PassedCuts++; //Rcore

  // Eratio
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Eratio=" << m_energyRatio
	  << " cut: >"  << m_caeratiothr[m_etaBin] << endmsg;   
  if ( inCrack || m_F1<m_F1thr[0] ) {
    msg() << MSG::DEBUG << "TrigEMCluster: InCrack= " << inCrack << " F1=" << m_F1 << endmsg;
  } else {
    if ( m_energyRatio < m_caeratiothr[m_etaBin] ) return HLT::OK;
  }
  m_PassedCuts++; //Eratio
  if(inCrack) m_energyRatio = -1; //Set default value in crack for monitoring.
  
  // ET_em
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: ET_em=" << m_eT_T2Calo
	<< " cut: >"  << m_eTthr[m_etaBin] << endmsg;
  if ( m_eT_T2Calo < m_eTthr[m_etaBin]) return HLT::OK;
  m_PassedCuts++; // ET_em
 
  float hadET_cut = 0.0;  
  // find which ET_had to apply	: this depends on the ET_em and the eta bin
  if ( m_eT_T2Calo >  m_eT2thr[m_etaBin] ) {
    hadET_cut = m_hadeT2thr[m_etaBin] ;

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "ET_em>"     << m_eT2thr[m_etaBin]
	  << ": use high ET_had cut: <" << hadET_cut << endmsg;
  } else {
    hadET_cut = m_hadeTthr[m_etaBin];

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "ET_em<"    << m_eT2thr[m_etaBin] 
	  << ": use low ET_had cut: <" << hadET_cut << endmsg;
  }
  
  // ET_had
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: ET_had=" << m_hadET_T2Calo
	<< " cut: <" << hadET_cut << endmsg;

  if ( m_hadET_T2Calo > hadET_cut ) return HLT::OK;
  m_PassedCuts++; //ET_had
  
  // F1
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: F1=" << m_F1
	  << " cut: >"  << m_F1thr[0] << endmsg;
  // if ( m_F1 < m_F1thr[0]) return HLT::OK;  //(VD) not cutting on this variable, only used to select whether to cut or not on eRatio
  m_PassedCuts++; //F1


  //Weta2
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Weta2=" << m_Weta2
	  << " cut: <"  << m_WETA2thr[m_etaBin] << endmsg; 
  if ( m_Weta2 > m_WETA2thr[m_etaBin]) return HLT::OK;
  m_PassedCuts++; //Weta2


  //Wstot
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: Wstot=" << m_Wstot
	  << " cut: <"  << m_WSTOTthr[m_etaBin] << endmsg; 
  if ( m_Wstot >= m_WSTOTthr[m_etaBin]) return HLT::OK;
  m_PassedCuts++; //Wstot

  //F3
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrigEMCluster: F3=" << m_F3
	  << " cut: <"  << m_F3thr[m_etaBin] << endmsg; 
  if ( m_F3 > m_F3thr[m_etaBin]) return HLT::OK;
  m_PassedCuts++; //F3


  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "pass = " << pass << endmsg;

  // (VD) this is quite obvious - it would be better to be able to arrive here eve if fails and see the line FAIL!

  return HLT::OK;
}

