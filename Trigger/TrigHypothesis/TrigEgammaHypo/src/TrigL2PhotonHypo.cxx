// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 ** 
 **   Author:       R.Goncalo    
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2PhotonHypo.cxx
 **                   
 **   Description: Hypothesis algorithms to be run for Photon 
 **                signatures at Level 2 after T2Calo
 ** 
 **   Authour: R.Goncalo 
 **
 **   Created:      Thu Jun 06 16:01:12 BST 2006
 **   Modified:     V. Perez-Reale, N. Berger    Nov 24 2006
 **                 Divided FEX and HYPO for attaching features
 **                 V.PEREZ REALE May 26 2007
 **                 Added Histograms for Monitoring, Timing and bug fixes
 **                 V.Dao copied the structure of TrigL2CaloHypo 13/10/2009
 **
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2PhotonHypo.h"

#include <cmath>

class ISvcLocator;

template <class SRC>
inline const DataVector<xAOD::TrigPhoton>** dvec_cast(SRC** ptr) { 
  return (const DataVector<xAOD::TrigPhoton>**)(ptr); 
} 


TrigL2PhotonHypo::TrigL2PhotonHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{


  // Read cuts - should probably get these from an xml file
  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("EtCutOnly",      m_etCutOnly=true);
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

  // declare monitoring histograms for all cut variables
  declareMonitoredCollection("PhEt",	*dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::pt);
  declareMonitoredCollection("PhEta",	*dvec_cast(&m_PhotonContainer), &PhEta);
  declareMonitoredCollection("PhPhi",	*dvec_cast(&m_PhotonContainer), &PhPhi);
  declareMonitoredCollection("dEta",	*dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::dEta);
  declareMonitoredCollection("dPhi",	*dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::dPhi);
  declareMonitoredCollection("PhRcore",	*dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::rcore);
  declareMonitoredCollection("PhEratio",*dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::eratio);
  declareMonitoredCollection("PhHadEt", *dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::etHad);
  declareMonitoredCollection("PhF1",    *dvec_cast(&m_PhotonContainer), &xAOD::TrigPhoton::f1);
  declareMonitoredVariable("CutCounter",m_PassedCuts);
  m_PhotonContainer=nullptr;
}


TrigL2PhotonHypo::~TrigL2PhotonHypo()
{ }


HLT::ErrorCode TrigL2PhotonHypo::hltInitialize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully"   << endmsg;
    msg() << MSG::DEBUG << "AcceptAll           = "
	  << (m_acceptAll==true ? "True" : "False") << endmsg; 
    // msg() << MSG::DEBUG << "SaveTrigPhotons     = "
    // 	<< (m_saveTrigPh==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "EtaBins        = " << m_etabin      << endmsg;
    msg() << MSG::DEBUG << "ETthr          = " 
	  << m_eTthr    << "(lo)/" << m_eT2thr    << "(hi)" << endmsg;  
    msg() << MSG::DEBUG << "HADETthr       = " 
	  << m_hadeTthr << "(lo)/" << m_hadeT2thr << "(hi)" << endmsg;
    msg() << MSG::DEBUG << "CARCOREthr     = " << m_carcorethr  << endmsg;
    msg() << MSG::DEBUG << "CAERATIOthr    = " << m_caeratiothr << endmsg;
    msg() << MSG::DEBUG << "dPHICLUSTERthr = " << m_dphicluster << endmsg;
    msg() << MSG::DEBUG << "dETACLUSTERthr = " << m_detacluster << endmsg;

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

  return HLT::OK;
}


HLT::ErrorCode TrigL2PhotonHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "In TrigL2PhotonHypo::finalize()" << endmsg;

  return HLT::OK;
}





HLT::ErrorCode TrigL2PhotonHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  m_PhotonContainer=0;

  float dPhi         = -99.0;
  float dEta         = -99.0;
  float EmET         = -99.0;
  float HadEmRatio   = -99.0;
  float Reta         = -99.0;
  float Eratio       = -99.0;
  float f1           = -99.0;
  float HadET        = -99.0;
  m_PassedCuts   = -1;

  // Accept-All mode
  // Allows algorithm to run
  if (m_acceptAll) {
      pass = true;
      ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
  } else {
      pass = false;
      ATH_MSG_DEBUG("AcceptAll property not set: applying selection");
  }

  // Get the TrigPhotonContainer from the Trigger Element (TE)
  // - there is a vector of TrigPhotonContainers attached to the TE
  // - but there should be only 1 TrigPhotonContainer in the vector (because of trigger persistency)

  std::vector<const xAOD::TrigPhotonContainer*> vecTrigPhotonCont;
  HLT::ErrorCode status = getFeatures(outputTE, vecTrigPhotonCont, "");

  if( status != HLT::OK  ) {
    if(msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigPhotonContainer from TE" << endmsg;
    }
    return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Retrieved vector with " << vecTrigPhotonCont.size() 
	  << " TrigPhotonContainers from TE" << endmsg;
  }

  if(vecTrigPhotonCont.size()!=1){
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Number of TrigPhotonContainers retrieved from TE is not 1, exiting" << endmsg;
    }
    return HLT::OK;
  }

  m_PhotonContainer = vecTrigPhotonCont.back(); 


  // Get the TrigPhoton from the TrigPhotonContainer
  // - there should be only 1 TrigPhoton in the TrigPhotonContainer (because the TrigL2PhotonFex attaches a TrigPhotonContainer instead of a TrigPhoton - which should be changed, but requires an EDM change)
  
  if ( m_PhotonContainer == 0 ) {
    if ( msgLvl() <= MSG::DEBUG ) { 
      msg() << MSG::DEBUG << "TrigPhotonContainer is NULL, exiting" << endmsg;
    }
    return HLT::OK;
  } 

  if(m_PhotonContainer->size()!=1){
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Number of TrigPhotons in TrigPhotonContainer is not 1, exiting" << endmsg;
    }
    return HLT::OK;
  }

  const xAOD::TrigPhoton* photon = m_PhotonContainer->back();

  if ( photon == 0 ) {
    if ( msgLvl() <= MSG::DEBUG ) { 
      msg() << MSG::DEBUG << "TrigPhoton is NULL, exiting" << endmsg;
    }
    return HLT::OK;
  } 
  
  // Increment event counter 
  m_PassedCuts++; //// the ROI at least contais the TrigPhoton

/*
  if ( !photon->isValid() ) {
    if ( msgLvl() <= MSG::DEBUG ) { 
      msg() << MSG::DEBUG << "TrigPhoton has Valid flag = FALSE" << endmsg;
    }
    return HLT::OK;
  } 
*/
  m_PassedCuts++; //// the TrigPhoton is valid

  
  // Determine which eta bin to apply the cuts 
  float absEta = fabs( photon->eta() );
  int etaBin = -1;
  for (std::size_t iBin = 0; iBin < m_etabin.size()-1; iBin++)
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin; 
  
  // getting photon variable
  dEta       = photon->dEta();
  dPhi       = photon->dPhi();
  Eratio     = photon->eratio();
  Reta       = photon->rcore(); 
  EmET       = photon->pt();
  HadET      = photon->etHad();
  f1         = photon->f1();
  
  if(m_etCutOnly) {
    // ET_em
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << "EtCutOnly property is set: TrigPhoton: ET_em=" << EmET
	    << " cut in etaBin " << etaBin << " is ET_em >= " << m_eTthr[0] << endmsg;
    }
    if ( EmET < m_eTthr[0]) return HLT::OK;
    pass = true;
    return HLT::OK;
  }

  //now use the ratio, not the absolute HadEt (this is not yet persistified in the TrigPhoton)
  HadEmRatio = (EmET!=0) ? HadET/EmET : -1.0; 

  // apply cuts: DeltaEta(clus-ROI)
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG  << "TrigPhoton dEta=" << dEta
	  << " cut is dEta <= " << m_detacluster << endmsg; 
  } 
  if ( fabs(dEta) > m_detacluster ) return HLT::OK;
  m_PassedCuts++;

  // DeltaPhi(clus-ROI)
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "TrigPhoton dPhi="  << dPhi
	<< " cut is dPhi <= "  << m_dphicluster  << endmsg;
  }
  if( fabs(dPhi) > m_dphicluster ) return HLT::OK;
  m_PassedCuts++;

  // eta range
  if ( etaBin==-1 ) {  
    msg() << MSG::DEBUG << "Photon eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] << endmsg;
    return HLT::OK;
  } else { 
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "eta bin used for cuts " << etaBin << endmsg;
  }
  m_PassedCuts++; // passed eta cut

  // Reta (was previously called Rcore)
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "TrigPhoton Reta=" << Reta 
	  << " cut in etaBin " << etaBin << " is Reta >= "  << m_carcorethr[etaBin] << endmsg;
  }
  if ( Reta < m_carcorethr[etaBin] )  return HLT::OK;
  m_PassedCuts++;

  // Eratio
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "TrigPhoton: Eratio=" << Eratio
	  << " cut in etaBin " << etaBin << " is Eratio >= " << m_caeratiothr[etaBin] << endmsg;   
  }
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );
  if ( inCrack || f1<m_F1thr[0] ) {
    msg() << MSG::DEBUG << "TrigPhoton: InCrack= " << inCrack << " F1=" << f1 
	  << " Eratio cut not being applied" << endmsg;
  } else {
    if ( Eratio < m_caeratiothr[etaBin] ) return HLT::OK;
  }
  m_PassedCuts++;
  if(inCrack)  Eratio  = -1; //Set default value in crack for monitoring.

  // ET_em
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "TrigPhoton: ET_em=" << EmET
	  << " cut in etaBin " << etaBin << " is ET_em >= " << m_eTthr[etaBin] << endmsg;
  }
  if ( EmET < m_eTthr[etaBin]) return HLT::OK;
  m_PassedCuts++;

  
  // ET_had
  // find which ET_had to apply	: this depends on the ET_em and the eta bin
  float hadET_cut=-1;
  if ( EmET >  m_eT2thr[etaBin] ) {
    hadET_cut = m_hadeT2thr[etaBin] ;

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "ET_em>"     << m_eT2thr[etaBin]
	  << ": use high ET_had cut: <" << hadET_cut << endmsg;
  } else {
    hadET_cut = m_hadeTthr[etaBin];

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "ET_em<"    << m_eT2thr[etaBin] 
	  << ": use low ET_had cut: <" << hadET_cut << endmsg;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "TrigPhoton: ET_had=" <<  HadEmRatio
	  << " cut in etaBin " << etaBin << " is ET_had <=" << hadET_cut << endmsg;
  }
  if ( HadEmRatio > hadET_cut ) return HLT::OK;
  m_PassedCuts++;

   
  // F1
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "TrigPhoton: F1=" << f1
	  << " cut: >"  << m_F1thr[0] << endmsg;
  }
  // if ( f1 < m_F1thr[0]) return HLT::OK;  //(VD) not cutting on this (see comment on TrigL2CaloHypo)
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "F1 cut is NOT being applied" << endmsg;
  }
  m_PassedCuts++;

  pass = true;

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "pass = " << pass << endmsg;
  }

  return HLT::OK;
}
















