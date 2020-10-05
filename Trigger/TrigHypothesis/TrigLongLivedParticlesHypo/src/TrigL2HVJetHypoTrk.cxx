/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2HVJetHypoTrk.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// AUTHORS:  Antonio.Policicchio@cern.ch
//
// ********************************************************************

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigLongLivedParticlesHypo/TrigL2HVJetHypoTrk.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CxxUtils/phihelper.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2HVJetHypoTrk::TrigL2HVJetHypoTrk(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("doMonitoring_L2",     m_doMonitoring = true, "switch on/off monitoring" );
  declareProperty("AcceptAll",           m_acceptAll=false);
  //ID
  declareProperty("UseID",               m_UseID = true, "to use or not the ID informations");
  declareProperty("IDalgo",              m_ID_algo_to_use = "STRATEGY_B", "");
  declareProperty("Deta",                m_Deta = 0.2, "cut value for EtaJet-EtaTrack");
  declareProperty("Dphy",                m_Dphi = 0.2, "cut value for PhiJet-PhiTrack");
  declareProperty("PtMinID",             m_PtMinID = 1000.0, "minimum Pt (MeV) of the ID tracks");
  declareProperty("nTracks",             m_nTracks = 0, "number of ID tracks in the Dphi*Deta");

  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("NumberOfTracks",    m_NumberOfTracks);

}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2HVJetHypoTrk::~TrigL2HVJetHypoTrk()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HVJetHypoTrk::hltInitialize()
// ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endmsg;


  m_accepted_L2=0;
  m_rejected_L2=0;
  m_errors_L2=0;


  //Set ID Algo to use
  std::string algoId = m_ID_algo_to_use;
  
  if      (algoId=="IDSCAN")     m_algoId = TrigInDetTrack::IDSCANID;
  else if (algoId=="SITRACK")    m_algoId = TrigInDetTrack::SITRACKID;
  else if (algoId=="STRATEGY_A") m_algoId = TrigInDetTrack::STRATEGY_A_ID;
  else if (algoId=="STRATEGY_B") m_algoId = TrigInDetTrack::STRATEGY_B_ID;
  else if (algoId=="STRATEGY_C") m_algoId = TrigInDetTrack::STRATEGY_C_ID;
  else if (algoId=="TRTXK")      m_algoId = TrigInDetTrack::TRTXKID;
  else if (algoId=="TRTSEG")     m_algoId = TrigInDetTrack::TRTLUTID;
  else                           m_algoId = TrigInDetTrack::IDSCANID;


  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HVJetHypoTrk::hltFinalize(){
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HVJetHypoTrk::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  m_cutCounter = -1;


  pass=false;


  HLT::ErrorCode status;

  // get the trigger element and extract the RoI information
  if(msgLvl() <= MSG::DEBUG) { // pure debug purposes...
    const TrigRoiDescriptor* roiDescriptor = 0;
    if ( getFeature(outputTE, roiDescriptor, "a_label")==HLT::OK ) {
      if ( roiDescriptor ) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
            << " located at   phi = " <<  roiDescriptor->phi() << ", eta = " << roiDescriptor->eta() << endmsg;
      } else {
        if(msgLvl() <= MSG::DEBUG) msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
      }
    } else {
      if(msgLvl() <= MSG::DEBUG) msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }
  }


  std::vector<const TrigT2Jet*> vectorOfJets;

  HLT::ErrorCode ec = getFeatures(outputTE, vectorOfJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the L2 Jets " << endmsg;
    m_errors_L2++; 
    return ec;
  }

  m_cutCounter = 0;

  // Check that there is only one JETROI.
  // We only expect ONE input RoI.
  if (vectorOfJets.size() != 1){
    msg() << MSG::ERROR << "The number of Jets attached to the TE is not 1" << endmsg;
    return HLT::OK;
  }

  // Get first (and only) RoI:
  const TrigT2Jet* pJet = vectorOfJets.back();
  if(!pJet){
    msg() << MSG::ERROR << "Retrieval of RoI from vector failed"  << endmsg;
    return HLT::OK;
  }





  //double etjet = pJet->e();
  double etjet  = pJet->et();
  double ehjet  = pJet->ehad0();
  double emjet  = pJet->eem0();
  double jetEta = pJet->eta();
  double jetPhi = pJet->phi();
  double erat = -99999999.;

  if (emjet <= 0. && ehjet > 0.) erat = 999999;
  if (emjet > 0. && ehjet >= 0.) erat = log10(ehjet/emjet);

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet energy (L2): " << etjet << endmsg;
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet log10 of had / em energy ratio (L2): " << erat << endmsg;





  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Accessing the ID track collection " << endmsg;
  //ACCESSING THE ID TRACK COLLECTION FOR THE TRACKLESS JET REQUEST
  int ntrk = 0;
  if(m_UseID) {
 
    std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
    status = getFeatures(outputTE,vectorOfTrackCollections, "");

    if( status!=HLT::OK ) {
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Failed to get InDetTrackCollections " << endmsg;
    }else{ 
    
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() 
            << " InDetTrackCollections " << endmsg;  

      std::vector<const TrigInDetTrackCollection*>::iterator 
      theTrackColl = vectorOfTrackCollections.begin(),
      endTrackColl = vectorOfTrackCollections.end();
             
      for( ; theTrackColl != endTrackColl;  theTrackColl++){
           
        TrigInDetTrackCollection::const_iterator 
        track     = (*theTrackColl)->begin(),
        lasttrack = (*theTrackColl)->end();
       
        for(; track !=lasttrack; track++ ){

          if( (*track)->algorithmId() != m_algoId ) continue;
          if (!((*track)->param())) continue;

          double pt_idtr = (*track)->param()->pT();
          if (fabs(pt_idtr) <= m_PtMinID) continue;

          double phi_id  = (*track)->param()->phi0();
          double eta_id  = (*track)->param()->eta();

          if(msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "trying " << m_ID_algo_to_use << " track with " << "pt=" << pt_idtr << ", eta=" << eta_id << ", phi=" << phi_id  << endmsg;

          double deta = fabs( eta_id - jetEta );
          double dphi = fabs( CxxUtils::wrapToPi(phi_id - jetPhi) );

          if(deta <= m_Deta && dphi <= m_Dphi){ 
            ntrk++;
          }
        }
      }
    }
  }

  //monitored variable
  m_NumberOfTracks = ntrk;

  if(ntrk <= m_nTracks){
    pass = true;
    m_accepted_L2++;
    m_cutCounter=1;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Number of tracks in ID is " << ntrk << ".... Event Accepted!" << endmsg;
  }else{
    m_rejected_L2++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Number of tracks in ID is " << ntrk << ".... Event rejected !" << endmsg;
  }


  return HLT::OK;

}
