/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     EFTauDiKaonHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   D. Zanzi based on EFTauMVHypo
 * CREATED:  Aug 17, 2015
 *
  *********************************************************************/

#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigTauHypo/EFTauDiKaonHypo.h"
//
#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "TFile.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <PathResolver/PathResolver.h>
class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauDiKaonHypo::EFTauDiKaonHypo(const std::string& name, 
		     ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
// set unreasonable values to make sure that initialization is performed from outside
// however we assume if one parameter is set from outside then they all set ok.
  declareProperty("massTrkSysMin", m_massTrkSysMin = 0.);
  declareProperty("massTrkSysMax", m_massTrkSysMax = 1000000000.);
  declareProperty("massTrkSysKaonMin", m_massTrkSysKaonMin = 0.);
  declareProperty("massTrkSysKaonMax", m_massTrkSysKaonMax = 1000000000.);
  declareProperty("massTrkSysKaonPiMin", m_massTrkSysKaonPiMin = 0.);
  declareProperty("massTrkSysKaonPiMax", m_massTrkSysKaonPiMax = 1000000000.);
  declareProperty("targetMassTrkSysKaonPi", m_targetMassTrkSysKaonPi = 0.);
  declareProperty("leadTrkPtMin",  m_leadTrkPtMin  = 0.);
  declareProperty("EtCalibMin",    m_EtCalibMin  = 0.);
  declareProperty("nTrackMin",     m_nTrackMin  = 1);
  declareProperty("nTrackMax",     m_nTrackMax  = 2);
  declareProperty("nWideTrackMax", m_nWideTrackMax  = 1);
  declareProperty("dRmaxMax",      m_dRmaxMax  = 10.);
  declareProperty("etOverPtLeadTrkMin",    m_etOverPtLeadTrkMin  = 0.);
  declareProperty("etOverPtLeadTrkMax",    m_etOverPtLeadTrkMax  = 10.);
  declareProperty("EMPOverTrkSysPMax",     m_EMPOverTrkSysPMax  = 5.);

  declareMonitoredVariable("CutCounter",	m_cutCounter=0);
  declareMonitoredVariable("MassTrkSys",	m_massTrkSysAccepted =-10.);
  declareMonitoredVariable("MassTrkSysKaon",        m_massTrkSysKaonAccepted =-10.);
  declareMonitoredVariable("MassTrkSysKaonPi",        m_massTrkSysKaonPiAccepted =-10.);
  declareMonitoredVariable("LeadTrkPt",		m_leadTrkPtAccepted =-10.);
  declareMonitoredVariable("EtCalib",		m_ptAccepted =-10.);
  declareMonitoredVariable("NTrack",		m_nTrackAccepted =-1);
  declareMonitoredVariable("NWideTrack",	m_nWideTrackAccepted =-1);
  declareMonitoredVariable("DR",		m_dRAccepted =-1.);
  declareMonitoredVariable("EtOverPtLeadTrk",	m_etOverPtLeadTrkAccepted =-10.);
  declareMonitoredVariable("EMOverTrkSysP",	m_EMOverTrkSysPAccepted =-10.);

  m_massTrkSys = 0.;
  m_massTrkSysKaon = 0.;
  m_massTrkSysKaonPi = 0.;
  m_leadTrkPt = 0.;
  m_nTrack = 0;
  m_nWideTrack = 0;
  m_EtCalib = 0.;
  m_EMPOverTrkSysP = 0.;
  m_dRmax = 0.;
  m_etOverPtLeadTrk = 0.;

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauDiKaonHypo::~EFTauDiKaonHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauDiKaonHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: EFTauDiKaonHypo will cut on "<<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysMin " << m_massTrkSysMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysMax " << m_massTrkSysMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonMin " << m_massTrkSysKaonMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonMax " << m_massTrkSysKaonMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonPiMin " << m_massTrkSysKaonPiMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param massTrkSysKaonPiMax " << m_massTrkSysKaonPiMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param targetMassTrkSysKaonPi " << m_targetMassTrkSysKaonPi <<endmsg;
  msg() << MSG::INFO << " REGTEST: param leadTrkPtMin " << m_leadTrkPtMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param EtCalibMin " << m_EtCalibMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param nTrackMin (included) " << m_nTrackMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param nTrackMax (included) " << m_nTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param nWideTrackMax (included) " << m_nWideTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param EMPOverTrkSysPMax " << m_EMPOverTrkSysPMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param dRmaxMax " << m_dRmaxMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param etOverPtLeadTrkMin " << m_etOverPtLeadTrkMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param etOverPtLeadTrkMax " << m_etOverPtLeadTrkMax <<endmsg;

  msg() << MSG::INFO << " REGTEST: ------ "<<endmsg;
  
  if( ( m_massTrkSysKaonPiMin >  m_massTrkSysKaonPiMax ) ||  ( m_massTrkSysKaonMin >  m_massTrkSysKaonMax ) || ( m_massTrkSysMin >  m_massTrkSysMax ) || ( m_nTrackMin > m_nTrackMax )  || (m_etOverPtLeadTrkMin > m_etOverPtLeadTrkMax) )
    {
      msg() << MSG::ERROR << "EFTauDiKaonHypo is uninitialized! " << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  
  msg() << MSG::INFO
	<< "Initialization of EFTauDiKaonHypo completed successfully"
	<< endmsg;
  
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauDiKaonHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode EFTauDiKaonHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in execute()" << endmsg;
  
  // general reset
  pass=false;

  m_cutCounter = 0;
  m_massTrkSysAccepted = -10.;
  m_massTrkSysKaonAccepted = -10.;
  m_massTrkSysKaonPiAccepted = -10.;
  m_leadTrkPtAccepted = -10.;
  m_ptAccepted = -10.;
  m_nTrackAccepted = -1;
  m_nWideTrackAccepted = -1;
  m_dRAccepted = -10.;
  m_etOverPtLeadTrkAccepted = -10.;
  m_EMOverTrkSysPAccepted = -10.;

  m_massTrkSys = 0.;
  m_massTrkSysKaon = 0.;
  m_massTrkSysKaonPi = 0.;
  m_leadTrkPt = 0.;
  m_nTrack = 0;
  m_nWideTrack = 0;
  m_EtCalib = 0.;  
  m_EMPOverTrkSysP = 0.;
  m_dRmax = 0.;
  m_etOverPtLeadTrk = 0.;

  // get the trigger element and extract the RoI information
  //---------------------------------------------------------
  
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(outputTE, roiDescriptor);
  
  if ( roiDescriptor ) 
    {
      if( msgLvl() <= MSG::DEBUG ) 
	msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
	      << " located at   phi = " <<  roiDescriptor->phi()
	      << ", eta = " << roiDescriptor->eta() << endmsg;
    } 
  else 
    {
      if( msgLvl() <= MSG::DEBUG ) 
	msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }
  
  // get tau objects from the trigger element:
  //--------------------------------------------------
  
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
  
  status = getFeatures(outputTE, vectorTauContainers);
 
  if(status!=HLT::OK) 
    {
      msg() << MSG::INFO
	    << " REGTEST: Failed to get tauContainer's from the trigger element" 
	    << endmsg;
      return HLT::OK;
    } 
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got " << vectorTauContainers.size() 
	  << " tauContainers's associated to the TE " << endmsg;
  
  if(vectorTauContainers.size() == 0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }
  
  const xAOD::TauJetContainer *TauContainer = vectorTauContainers.back();
  
  msg() << MSG::DEBUG << " REGTEST: number of tau in container "<< TauContainer->size() << endmsg;
  
  for(xAOD::TauJetContainer::const_iterator tauIt = TauContainer->begin();
      tauIt != TauContainer->end(); tauIt++){ 
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " tauRec candidate "<<endmsg;
    
    m_cutCounter++;
    
    // cut on calibrated pt:
    double EFet = (*tauIt)->pt()*1e-3;
    
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: Et Calib "<< EFet <<endmsg;
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    m_cutCounter++;
    m_ptAccepted = EFet;
    
    // cut on core and wide tracks:
    m_nTrack = (*tauIt)->nTracks();
    #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    m_nWideTrack = (*tauIt)->nWideTracks();
    #else
    m_nWideTrack = (*tauIt)->nTracksIsolation();
    #endif

    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: Track size "<< m_nTrack <<endmsg;	
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: Wide Track size "<< m_nWideTrack <<endmsg;
 
    if (!( (m_nTrack >= m_nTrackMin) && (m_nTrack <= m_nTrackMax)))  continue;
    if (!( m_nWideTrack <= m_nWideTrackMax ))  continue;
    m_cutCounter++;
    m_nTrackAccepted = m_nTrack;
    m_nWideTrackAccepted = m_nWideTrack;

    // cut on leading track pt:     
    (*tauIt)->detail(xAOD::TauJetParameters::leadTrkPt, m_leadTrkPt);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: leadTrkPt "<< m_leadTrkPt <<endmsg;

    if(!( m_leadTrkPt > m_leadTrkPtMin)) continue;
    m_cutCounter++;
    m_leadTrkPtAccepted = m_leadTrkPt;

    // cut on massTrkSys:     
    (*tauIt)->detail(xAOD::TauJetParameters::massTrkSys, m_massTrkSys);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: massTrkSys "<< m_massTrkSys <<endmsg;

    // for dikaon mass hypothesis, compute invariant mass with kaon mass
    TLorentzVector my_kaons(0.,0.,0.,0.);
    std::vector<TLorentzVector> my_trks;
    // need to add checks for valid link
    for (unsigned int i=0;i<(*tauIt)->nTracks();++i) {
      const xAOD::TrackParticle* trk = 0;
      TLorentzVector tmpKaon(0.,0.,0.,0.);

      try 
      {
        #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
        trk = (*tauIt)->track(i);
	#else
	trk = (*tauIt)->track(i)->track();
	#endif
      }
      catch(std::exception e)
      {
        msg() << MSG::WARNING << " REGTEST: EFTauDiKaonHypo, failed to get tau track link! " <<endmsg;
      } 

      if(trk) {
         tmpKaon.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), 493.677);
         my_trks.push_back(trk->p4());
      }
      my_kaons = my_kaons + tmpKaon;
    }
    m_massTrkSysKaon = my_kaons.M();
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: massTrkSys with kaon mass hypo "<< m_massTrkSysKaon <<endmsg;

    // kaon+pi mass hypo
    double finalKPiMass = 0;
    if(my_trks.size()==2){

          TLorentzVector tmpKaon;
          tmpKaon.SetPtEtaPhiM(my_trks.at(0).Pt(), my_trks.at(0).Eta(), my_trks.at(0).Phi(), 493.677);
          TLorentzVector tmpPion = my_trks.at(1);

          double kPiMass1 = (tmpKaon+tmpPion).M();

          tmpKaon.SetPtEtaPhiM(my_trks.at(1).Pt(), my_trks.at(1).Eta(), my_trks.at(1).Phi(), 493.677);
          tmpPion = my_trks.at(0);

          double kPiMass2 = (tmpKaon+tmpPion).M();

          if(fabs(kPiMass1 - m_targetMassTrkSysKaonPi) < fabs(kPiMass2 - m_targetMassTrkSysKaonPi))
          {
            finalKPiMass = kPiMass1;
          }else{
            finalKPiMass = kPiMass2;
          }
    }
    m_massTrkSysKaonPi = finalKPiMass;
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: massTrkSys with kaon+pi mass hypo "<< m_massTrkSysKaonPi <<endmsg;

    if (!( (m_massTrkSys > m_massTrkSysMin) && (m_massTrkSys < m_massTrkSysMax) ) )  continue;
    m_cutCounter++;
    m_massTrkSysAccepted = m_massTrkSys;

    if (!( (m_massTrkSysKaon > m_massTrkSysKaonMin) && (m_massTrkSysKaon < m_massTrkSysKaonMax) ) )  continue;
    m_cutCounter++;   
    m_massTrkSysKaonAccepted = m_massTrkSysKaon;

    if (!( (m_massTrkSysKaonPi >= m_massTrkSysKaonPiMin) && (m_massTrkSysKaonPi < m_massTrkSysKaonPiMax) ) )  continue; //use >= otherwise singlepion chain would fail here!
    m_cutCounter++;
    m_massTrkSysKaonPiAccepted = m_massTrkSysKaonPi;

    // cut on EMPOverTrkSysP:     
    (*tauIt)->detail(xAOD::TauJetParameters::EMPOverTrkSysP, m_EMPOverTrkSysP);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: EMPOverTrkSysP "<< m_EMPOverTrkSysP <<endmsg;
               
    if ( !(m_EMPOverTrkSysP < m_EMPOverTrkSysPMax) )  continue;
    m_cutCounter++;
    m_EMOverTrkSysPAccepted = m_EMPOverTrkSysP;

    // cut on etOverPtLeadTrk:
    (*tauIt)->detail(xAOD::TauJetParameters::etOverPtLeadTrk, m_etOverPtLeadTrk);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: etOverPtLeadTrk "<< m_etOverPtLeadTrk <<endmsg;

    if( !( (m_etOverPtLeadTrk > m_etOverPtLeadTrkMin) && (m_etOverPtLeadTrk < m_etOverPtLeadTrkMax)  ) ) continue;
    m_cutCounter++;
    m_etOverPtLeadTrkAccepted = m_etOverPtLeadTrk;

    // cut on dRmax:
    (*tauIt)->detail(xAOD::TauJetParameters::dRmax, m_dRmax);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: dRmax "<< m_dRmax <<endmsg;

    if( !( m_dRmax < m_dRmaxMax ) ) continue;
    m_cutCounter++;    
    m_dRAccepted = m_dRmax; 


    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: pass taurec is "<<pass<<endmsg;
    
  } // end of loop in tau objects.
  
  if(pass)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: TE accepted !! "
	      << endmsg;
      // activate Trigger Element.
    }
  else
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: No good tau found !! TE rejected "
	      << endmsg;
    }
  
  return HLT::OK;
  
}
