/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAMEABILITY:     DQTMuonIDTrackTool.cxx
// PACKAGE:  DataQualityTools  
//
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            and Max Baak (mbaakcern.ch)
//            Updated by: 
//              Simon Viel (svielcern.ch)
//		Suvayu Ali (sali@cern.ch)
//
// ********************************************************************

#include "DataQualityTools/DQTMuonIDTrackTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 

#include "TrkParameters/TrackParameters.h"

#include "TProfile.h"
#include "TMath.h"
#include <cmath>
//#include "TLorentzVector.h"
//#include "CommissionEvent/ComTime.h"


//----------------------------------------------------------------------------------
DQTMuonIDTrackTool::DQTMuonIDTrackTool(const std::string & type, 
                                       const std::string & name,
                                       const IInterface* parent)
   : DataQualityFatherMonTool(type, name, parent)
   , m_trkEtasGM(0)
   , m_trkThetasGM(0)
   , m_trkPhi0sGM(0)
   , m_trkd0sGM(0)
   , m_trkz0sGM(0)
   , m_trkpOverQsGM(0)
   , m_trkEtaDiff(0)
   , m_trkThetaDiff(0)
   , m_trkPhi0Diff(0)
   , m_trkd0Diff(0)
   , m_trkz0Diff(0)
   , m_trkpOverQDiff(0)
   , m_trkPxDiff(0)
   , m_trkPyDiff(0)
   , m_trkPzDiff(0)
   , m_trkPDiff(0)
   , m_trkPtDiff(0)
   , m_trkChargeDiff(0)
   , m_trkXDiff(0)
   , m_trkYDiff(0)
   , m_trkZDiff(0)
   , m_ntrkMS(0)       
   , m_ntrkID(0)       
   , m_ntrkComb(0)
   , m_trkEtaIDTag(0)	
   , m_trkPhi0IDTag(0)  
   , m_trkEtaMSProbe(0) 
   , m_trkPhi0MSProbe(0)  
   , m_trkEtaID(0)
   , m_trkThetaID(0)
   , m_trkPhi0ID(0)
   , m_trkd0ID(0)
   , m_trkd0IDZOOM(0)
   , m_trkz0ID(0)
   , m_trkpOverQID(0)
   , m_trkPxID(0)
   , m_trkPyID(0)
   , m_trkPzID(0)
   , m_trkPID(0)
   , m_trkPtID(0)
   , m_trkChargeID(0)
   , m_trkXID(0)
   , m_trkYID(0)
   , m_trkZID(0)
   , m_trkd0vsPhi0ID(0)
   , m_trkeffvsphi(0)
   , m_trkeffvseta(0)
   , m_trkeffvsLumi(0)
   , m_fwdetatrkeffvsLumi(0)
   , m_bkwdetatrkeffvsLumi(0)
   , m_MS2ID_MuontrkeffvsPhi(0)
   , m_ID2MS_MuontrkeffvsEta(0)
   , m_trkEtaMuon(0)
   , m_trkThetaMuon(0)
   , m_trkPhi0Muon(0)
   , m_trkd0Muon(0)
   , m_trkz0Muon(0)
   , m_trkpOverQMuon(0)
   , m_trkPxMuon(0)
   , m_trkPyMuon(0)
   , m_trkPzMuon(0)
   , m_trkPMuon(0)
   , m_trkPtMuon(0)
   , m_trkChargeMuon(0)
   , m_trkXMuon(0)
   , m_trkYMuon(0)
   , m_trkZMuon(0)
   , m_JPsiMassID(0)
   , m_JPsiMassMuon(0)
   , m_JPsiMassDiff(0)
   , m_JPsiMass2D(0)
   , m_UpsilonMassID(0)
   , m_UpsilonMassMuon(0)
   , m_UpsilonMassDiff(0)
   , m_UpsilonMass2D(0)
   , m_ZMassID(0)
   , m_ZMassMuon(0)
   , m_ZMassDiff(0)
   , m_ZMass2D(0)
   , m_JPsiMassID_broad(0)
   , m_UpsilonMassID_broad(0)
   , m_CombinedInDetTracksName("CombinedInDetTracks")
   , m_MooreTracksName("MooreTracks")
   , m_CombinedTracksName("CombinedTracks")
   , m_extrapolator("Trk::Extrapolator/InDetExtrapolator")
   , m_z0DiffCutID(200)
   , m_d0DiffCutID(500)
   , m_z0DiffCutMuon(200)
   , m_d0DiffCutMuon(500)
   , m_nMinSCTHits(5)
   , m_minPtCut(4000)
   , m_muonPtCut(6000)
   , m_JPsiCounter(0)
   , m_printedErrorID(false)
   , m_printedErrorMuon(false)
   , m_printedErrorCombined(false)
   , m_printedErrorMuonColl(false)
//----------------------------------------------------------------------------------

{
   declareProperty("InDetTracks",    m_CombinedInDetTracksName);
   declareProperty("MuonTracks",     m_MooreTracksName);
   declareProperty("CombinedTracks", m_CombinedTracksName);
   declareProperty("ExtrapolationTool",  m_extrapolator);
   declareProperty("z0DiffCutID",  m_z0DiffCutID);
   declareProperty("d0DiffCutID",  m_d0DiffCutID);
   declareProperty("z0DiffCutMuon",  m_z0DiffCutMuon);
   declareProperty("d0DiffCutMuon",  m_d0DiffCutMuon);
   declareProperty("MinSCTHits", m_nMinSCTHits);
   declareProperty("MinPtCut", m_minPtCut);
   declareProperty("muonPtCut", m_muonPtCut);
   //declareProperty("doRunCosmics", m_doRunCosmics = 1);
   //declareProperty("doRunBeam", m_doRunBeam = 1);  
   //declareProperty("doOfflineHists", m_doOfflineHists = 1);
   //declareProperty("doOnlineHists", m_doOnlineHists = 1);
   declareInterface<IMonitorToolBase>(this);
   m_path = "GLOBAL/DQTMuonIDTrack";

}


//----------------------------------------------------------------------------------
DQTMuonIDTrackTool::~DQTMuonIDTrackTool()
//----------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------
StatusCode DQTMuonIDTrackTool::initialize()
//----------------------------------------------------------------------------------
{
  ATH_CHECK( m_EventInfoKey.initialize() );
  ATH_CHECK( m_MuonContainerKey.initialize() );
  return DataQualityFatherMonTool::initialize();
}

//----------------------------------------------------------------------------------
StatusCode DQTMuonIDTrackTool::bookHistograms( )
//StatusCode DQTMuonIDTrackTool::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
//----------------------------------------------------------------------------------
{
   bool failure(false);
   m_printedErrorID = false;
   m_printedErrorMuon = false;
   m_printedErrorCombined = false;
   m_printedErrorMuonColl = false;

   //if (newRun) {
      ATH_MSG_DEBUG( "in bookHistograms() and m_doRunCosmics = " << m_doRunCosmics << " and m_doRunBeam = " << m_doRunBeam );
      ATH_MSG_DEBUG( "Using base path " << m_path );
      
      failure = bookMuons();
   //}
   //else if (newEventsBlock || newLumiBlock) {
   //   return StatusCode::SUCCESS;
   //}
   if (failure) {return  StatusCode::FAILURE;}
   else {return StatusCode::SUCCESS;}
}	

		

//----------------------------------------------------------------------------------
bool DQTMuonIDTrackTool::bookMuons()
//----------------------------------------------------------------------------------
{
   bool failure(false);
   //  if (isNewEventsBlock || isNewLumiBlock || isNewRun) {
    

   if(m_extrapolator.retrieve().isFailure()) {
     ATH_MSG_FATAL( "Could not get " << m_extrapolator.type() );
      return false;
   }



   std::string  fullPathMuons=m_path+"/Trk";

   // Booking!
   failure = failure | registerHist(fullPathMuons, m_trkEtasGM    = TH2F_LW::create("m_trk_EtasGM", "Trk Etas", 100, -3.5, 3.5, 100, -3.5, 3.5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkThetasGM  = TH2F_LW::create("m_trk_ThetasGM", "Trk Thetas", 100, 0, TMath::Pi(), 100, 0, TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPhi0sGM   = TH2F_LW::create("m_trk_Phi0sGM", "Trk Phi0s", 200, -TMath::Pi(), TMath::Pi(), 200, -TMath::Pi(), TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkd0sGM     = TH2F_LW::create("m_trk_d0sGM", "Trk d0s", 100, -2000, 2000, 100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkz0sGM     = TH2F_LW::create("m_trk_z0sGM", "Trk z0s", 100, -2000, 2000, 100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkpOverQsGM = TH2F_LW::create("m_trk_pOverQsGM", "Trk pOverQs", 100, -1E5, 1E5, 100, -1E5, 1E5)).isFailure();  


   failure = failure | registerHist(fullPathMuons, m_trkEtaDiff    = TH1F_LW::create("m_trk_EtaDiff", "Diff Trk Etas", 100, -0.5, 0.5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkThetaDiff  = TH1F_LW::create("m_trk_ThetaDiff", "Diff Trk Thetas", 100, -TMath::Pi(), TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPhi0Diff   = TH1F_LW::create("m_trk_Phi0Diff", "Diff Trk Phi0s", 200, -1., 1.)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkd0Diff     = TH1F_LW::create("m_trk_d0Diff", "Diff Trk d0s", 100, -500, 500)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkz0Diff     = TH1F_LW::create("m_trk_z0Diff", "Diff Trk z0s", 100, -500, 500)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkpOverQDiff = TH1F_LW::create("m_trk_pOverQDiff", "Diff Trk pOverQs", 100, -5E3, 5E3)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPxDiff     = TH1F_LW::create("m_trk_PxDiff", "Diff Trk Px", 100, -5E3, 5E3)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPyDiff     = TH1F_LW::create("m_trk_PyDiff", "Diff Trk Py", 100, -5E3, 5E3)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPzDiff     = TH1F_LW::create("m_trk_PzDiff", "Diff Trk Pz", 100, -5E3, 5E3)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPDiff      = TH1F_LW::create("m_trk_PDiff",  "Diff Trk P",  100, -25E3, 25E3)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPtDiff     = TH1F_LW::create("m_trk_PtDiff", "Diff Trk Pt", 100, -25E3, 25E3)).isFailure();  
   failure = failure | registerHist(fullPathMuons, m_trkChargeDiff = TH1F_LW::create("m_trk_ChargeDiff", "Diff Trk Charge", 100, -3, 3)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkXDiff      = TH1F_LW::create("m_trk_XDiff",  "Diff Trk X perigee",  100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkYDiff      = TH1F_LW::create("m_trk_YDiff",  "Diff Trk Y perigee",  100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkZDiff      = TH1F_LW::create("m_trk_ZDiff",  "Diff Trk Z perigee",  100, -2000, 2000)).isFailure();

   failure = failure | registerHist(fullPathMuons, m_ntrkMS      = TH1F_LW::create("m_trk_ntrkMS",  "Number of Muon tracks",  10, 0, 10)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_ntrkID      = TH1F_LW::create("m_trk_ntrkID",  "Number of ID tracks",  10, 0, 10)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_ntrkComb      = TH1F_LW::create("m_trk_ntrkComb",  "Number of Combined tracks",  10, 0, 10)).isFailure();
   
   failure = failure | registerHist(fullPathMuons, m_trkEtaIDTag     = TH1F_LW::create("m_trk_EtaIDTag", "ID Tag Trk Etas", 100, -3.0, 3.0)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPhi0IDTag   = TH1F_LW::create("m_trk_Phi0IDTag", "ID Tag Trk Phi0s", 200, -TMath::Pi(), TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkEtaMSProbe  = TH1F_LW::create("m_trk_EtaMSProbe", "MS Probe Trk Etas", 100, -3.0, 3.0)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPhi0MSProbe = TH1F_LW::create("m_trk_Phi0MSProbe", "MS Probe Trk Phi0s", 200, -TMath::Pi(), TMath::Pi())).isFailure();
   

   failure = failure | registerHist(fullPathMuons, m_trkEtaID    = TH1F_LW::create("m_trk_EtaID", "ID Trk Etas", 100, -3.0, 3.0)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkThetaID  = TH1F_LW::create("m_trk_ThetaID", "ID Trk Thetas", 100, 0, TMath::Pi() )).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPhi0ID   = TH1F_LW::create("m_trk_Phi0ID", "ID Trk Phi0s", 200, -TMath::Pi(), TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkd0ID     = TH1F_LW::create("m_trk_d0ID", "ID Trk d0s", 100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkd0IDZOOM = TH1F_LW::create("m_trk_d0IDZOOM", "ID Trk d0s zoom", 400, -20, 20)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkz0ID     = TH1F_LW::create("m_trk_z0ID", "ID Trk z0s", 100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkpOverQID = TH1F_LW::create("m_trk_pOverQID", "ID Trk pOverQs", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPxID     = TH1F_LW::create("m_trk_PxID", "ID Trk Px", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPyID     = TH1F_LW::create("m_trk_PyID", "ID Trk Py", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPzID     = TH1F_LW::create("m_trk_PzID", "ID Trk Pz", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPID      = TH1F_LW::create("m_trk_PID",  "ID Trk P",  100, 0, 5E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPtID     = TH1F_LW::create("m_trk_PtID", "ID Trk Pt", 100, 0, 5E5)).isFailure();  
   failure = failure | registerHist(fullPathMuons, m_trkChargeID = TH1F_LW::create("m_trk_ChargeID", "ID Trk Charge", 100, -2, 2)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkXID      = TH1F_LW::create("m_trk_XID",  "ID Trk X perigee",  100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkYID      = TH1F_LW::create("m_trk_YID",  "ID Trk Y perigee",  100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkZID      = TH1F_LW::create("m_trk_ZID",  "ID Trk Z perigee",  100, -2000, 2000)).isFailure();

   //TProfile
   failure = failure | registerHist(fullPathMuons, m_trkd0vsPhi0ID     = new TProfile("m_trk_d0vsPhi0ID", "ID Trk d0 vs Phi0", 20, -TMath::Pi(), TMath::Pi(), -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkeffvsphi       = new TProfile("m_trk_trkeffvsphi", "Trk eff vs Phi0", 20, -TMath::Pi(), TMath::Pi(), 0, 1)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkeffvseta       = new TProfile("m_trk_trkeffvseta", "Trk eff vs Eta", 20, -2.5, 2.5, 0, 1)).isFailure();

   failure = failure | registerHist(fullPathMuons, m_trkeffvsLumi   = new TProfile("m_trk_trkeffvsLumi", "Trk eff per Lumi block", 1000, 0, 1000, 0, 1)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_fwdetatrkeffvsLumi    = new TProfile("m_trk_fwdetatrkeffvsLumi", "Forward Eta eff per Lumi block", 1000, 0, 1000, 0, 1)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_bkwdetatrkeffvsLumi   = new TProfile("m_trk_bkwdetatrkeffvsLumi", "Backward Eta eff per Lumi block", 1000, 0, 1000, 0, 1)).isFailure();

   failure = failure | registerHist(fullPathMuons, m_MS2ID_MuontrkeffvsPhi   = new TProfile("m_trk_MS2ID_MuontrkeffvsPhi",  "Muon Trk #phi matching eff (MS to ID) vs #phi",  100, -TMath::Pi(), TMath::Pi(), 0, 1)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_ID2MS_MuontrkeffvsEta   = new TProfile("m_trk_ID2MS_MuontrkeffvsEta",  "Muon Trk #eta eff (ID to MS) vs #eta",  100, -4, 4, 0, 1)).isFailure();

   failure = failure | registerHist(fullPathMuons, m_trkEtaMuon    = TH1F_LW::create("m_trk_EtaMuon", "Muon Trk Etas", 100, -3.0, 3.0)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkThetaMuon  = TH1F_LW::create("m_trk_ThetaMuon", "Muon Trk Thetas", 100, 0, TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPhi0Muon   = TH1F_LW::create("m_trk_Phi0Muon", "Muon Trk Phi0s", 200, -TMath::Pi(), TMath::Pi())).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkd0Muon     = TH1F_LW::create("m_trk_d0Muon", "Muon Trk d0s", 100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkz0Muon     = TH1F_LW::create("m_trk_z0Muon", "Muon Trk z0s", 100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkpOverQMuon = TH1F_LW::create("m_trk_pOverQMuon", "Muon Trk pOverQs", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPxMuon     = TH1F_LW::create("m_trk_PxMuon", "Muon Trk Px", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPyMuon     = TH1F_LW::create("m_trk_PyMuon", "Muon Trk Py", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPzMuon     = TH1F_LW::create("m_trk_PzMuon", "Muon Trk Pz", 100, -1E5, 1E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPMuon      = TH1F_LW::create("m_trk_PMuon",  "Muon Trk P",  100, 0, 5E5)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkPtMuon     = TH1F_LW::create("m_trk_PtMuon", "Muon Trk Pt", 100, 0, 5E5)).isFailure();  
   failure = failure | registerHist(fullPathMuons, m_trkChargeMuon = TH1F_LW::create("m_trk_ChargeMuon", "Muon Trk Charge", 100, -2, 2)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkXMuon      = TH1F_LW::create("m_trk_XMuon",  "Muon Trk X perigee",  100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkYMuon      = TH1F_LW::create("m_trk_YMuon",  "Muon Trk Y perigee",  100, -2000, 2000)).isFailure();
   failure = failure | registerHist(fullPathMuons, m_trkZMuon      = TH1F_LW::create("m_trk_ZMuon",  "Muon Trk Z perigee",  100, -2000, 2000)).isFailure();


   if (m_doRunBeam) {

      failure = failure | registerHist(fullPathMuons, m_JPsiMassID    = TH1F_LW::create("m_JPsi_MassID", "JPsi Mass ID", 50, 2600, 3600)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_JPsiMassMuon  = TH1F_LW::create("m_JPsi_MassMuon", "JPsi Mass Muon", 50, 0, 6300)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_JPsiMassDiff  = TH1F_LW::create("m_JPsi_MassDiff", "JPsi Mass Diff", 50, -3000, 3000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_JPsiMass2D    = TH2F_LW::create("m_JPsi_Mass2D", "JPsi Mass 2D", 100, 2600, 3600, 100, 0, 6300)).isFailure();
        
      failure = failure | registerHist(fullPathMuons, m_UpsilonMassID    = TH1F_LW::create("m_Upsilon_MassID", "Upsilon Mass ID", 50, 8000, 11000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_UpsilonMassMuon  = TH1F_LW::create("m_Upsilon_MassMuon", "Upsilon Mass Muon", 50, 0, 19000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_UpsilonMassDiff  = TH1F_LW::create("m_Upsilon_MassDiff", "Upsilon Mass Diff", 50, -10000, 10000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_UpsilonMass2D    = TH2F_LW::create("m_Upsilon_Mass2D", "Upsilon Mass 2D", 100, 8000, 11000, 100, 0, 19000)).isFailure();
        
      failure = failure | registerHist(fullPathMuons, m_ZMassID    = TH1F_LW::create("m_Z_MassID", "Z Mass ID", 50, 50000, 130000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_ZMassMuon  = TH1F_LW::create("m_Z_MassMuon", "Z Mass Muon", 50, 50000, 130000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_ZMassDiff  = TH1F_LW::create("m_Z_MassDiff", "Z Mass Diff", 50, -10000, 10000)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_ZMass2D    = TH2F_LW::create("m_Z_Mass2D", "Z Mass 2D", 100, 50000, 130000, 100, 50000, 130000)).isFailure();
        
    
      failure = failure | registerHist(fullPathMuons, m_JPsiMassID_broad     = TH1F_LW::create("m_JPsi_MassID_broad", "JPsi Mass ID Broad", 300, 0, 6300)).isFailure();
      failure = failure | registerHist(fullPathMuons, m_UpsilonMassID_broad  = TH1F_LW::create("m_Upsilon_MassID_broad", "Upsilon Mass ID Broad", 150, 5000, 14000)).isFailure();
  
    
      failure = failure | registerHist(fullPathMuons, m_JPsiCounter    = TH1F_LW::create("m_JPsi_Counter", "JPsi Count per Lumi Block", 1000, 0, 1000)).isFailure();
   }
      
   if (failure) {
     ATH_MSG_ERROR( "Error Booking histograms " );
   }
   return failure;

}


//----------------------------------------------------------------------------------
StatusCode DQTMuonIDTrackTool::fillHistograms()
//----------------------------------------------------------------------------------
{
  ATH_MSG_DEBUG("in fillHists()");
   StatusCode sc;
   sc.setChecked();

   //////////////////////////////////////////////////////

   SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey);
   if(! muons.isValid()) {
     ATH_MSG_WARNING( "No Container with name " << "Muons" << " found in evtStore" );
     return StatusCode::SUCCESS;
   } 

   ATH_MSG_DEBUG( "Muons" << " Collection size : " << muons->size() );
  

   // iterators over the muon container and retrieve ID tracks only
   xAOD::MuonContainer::const_iterator muonItr;
   std::vector<const xAOD::TrackParticle* > idtracks;
   std::vector<const xAOD::TrackParticle* > muontracks;
   std::vector<const xAOD::TrackParticle*>::const_iterator idtracksItr;
   std::vector<const xAOD::TrackParticle*>::const_iterator muontracksItr;

   std::vector<const xAOD::TrackParticle*>::const_iterator idtracksItr2;
   std::vector<const xAOD::TrackParticle*>::const_iterator muontracksItr2;

   // Muon trk eff
   std::vector<const xAOD::Muon* > MSMuons;
   std::vector<const xAOD::Muon* >::const_iterator MSMuonsItr;
   std::vector<const xAOD::Muon* > IDMuons;
   std::vector<const xAOD::Muon* >::const_iterator IDMuonsItr;

   int nms = 0;
   int nid = 0;
   int ncomb = 0;  

   SG::ReadHandle<xAOD::EventInfo> thisEventInfo(m_EventInfoKey);
   uint32_t LumiBlockNumber = thisEventInfo->lumiBlock();

   for (muonItr=muons->begin(); muonItr != muons->end(); ++muonItr) {

     ATH_MSG_DEBUG( "Muons Container track pointers (primary, inDet, muonSpect, comb, calo) " 
          << (*muonItr)->primaryTrackParticleLink().isValid()  << " " 
          << (*muonItr)->inDetTrackParticleLink().isValid() << " " 
          << (*muonItr)->muonSpectrometerTrackParticleLink().isValid()  << " " 
          << (*muonItr)->combinedTrackParticleLink().isValid()  << " " 
          << (*muonItr)->clusterLink().isValid() << " " 
		    );

      uint8_t NumOfPixHits = 0;
      uint8_t NumOfSCTHits = 0;
      if ((*muonItr)->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) ) {
	ATH_MSG_DEBUG( "Could not retrieve number of SCT hits" );
      }
      if ((*muonItr)->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) ) {
	ATH_MSG_DEBUG( "Could not retrieve number of Pixel hits" );
      }

      if ((*muonItr)->combinedTrackParticleLink().isValid() &&
          (*muonItr)->inDetTrackParticleLink().isValid()    &&
          (*muonItr)->muonSpectrometerTrackParticleLink().isValid() &&
          (NumOfSCTHits>=m_nMinSCTHits) &&
          ((*muonItr)->pt()>m_minPtCut) ) {
         idtracks.push_back( *((*muonItr)->inDetTrackParticleLink()) );
         muontracks.push_back( *((*muonItr)->muonSpectrometerTrackParticleLink()) );
         ncomb++;
      }

      if (((*muonItr)->inDetTrackParticleLink().isValid()) &&
          (NumOfSCTHits>=m_nMinSCTHits) &&
          ((*muonItr)->pt()>m_minPtCut) ) {
         m_trkPhi0IDTag->Fill((*muonItr)->phi());
         m_trkEtaIDTag->Fill((*muonItr)->eta());
         nid++;
	 
         int eveff = 0;
         if ((*muonItr)->muonSpectrometerTrackParticleLink().isValid()) {
            m_trkPhi0MSProbe->Fill((*muonItr)->phi());
            m_trkEtaMSProbe->Fill((*muonItr)->eta());
            eveff = 1;	   
         }
         m_trkeffvsphi->Fill((*muonItr)->phi(),eveff);
         m_trkeffvseta->Fill((*muonItr)->eta(),eveff);

         //Eff vs LumiBlocks
         m_trkeffvsLumi->Fill( LumiBlockNumber, eveff);
  
         if ( (*muonItr)->eta() >= 0 )
            m_fwdetatrkeffvsLumi->Fill(LumiBlockNumber, eveff);
         else
            m_bkwdetatrkeffvsLumi->Fill(LumiBlockNumber, eveff);
      }
      
      if ((*muonItr)->muonSpectrometerTrackParticleLink().isValid() && (*muonItr)->pt()>m_minPtCut) nms++;           
      
      // Muon trk eff
      //if ( ((*muonItr)->isStandAloneMuon()) &&  //EP-->StandAlone is not a link!  TODO
      if ( ((*muonItr)->muonSpectrometerTrackParticleLink().isValid()) &&
      	   ((*muonItr)->pt() > m_muonPtCut) &&
//     	   ((*muonItr)->allAuthors() == 8 ))  // for MuonParameters::MuidSA --> NOT COMPATIBLE WITH REL18
      	   ((*muonItr)->allAuthors() == 5 ))  // for MuonParameters::Muonboy
         MSMuons.push_back( *muonItr);

      if ( (*muonItr)->inDetTrackParticleLink().isValid() )
         IDMuons.push_back( *muonItr);
   }
   
   m_ntrkMS->Fill(nms);
   m_ntrkID->Fill(nid);
   m_ntrkComb->Fill(ncomb);
   
   for (idtracksItr=idtracks.begin(), muontracksItr = muontracks.begin(); idtracksItr!=idtracks.end();
        ++idtracksItr, ++muontracksItr) {

      const Trk::Perigee *idPerigee   = &((*idtracksItr)->perigeeParameters());
      const Trk::Perigee *premeasPerigee = &((*muontracksItr)->perigeeParameters());
      const Trk::TrackParameters *measPerigee(0);
     
      if (premeasPerigee) {
	ATH_MSG_WARNING("idPerigee? " << idPerigee << " " << idPerigee->associatedSurface());
	ATH_MSG_WARNING("premeasPerigee? " << premeasPerigee->associatedSurface());
	measPerigee = m_extrapolator->extrapolate(**muontracksItr,(idPerigee->associatedSurface()),Trk::anyDirection,true,Trk::pion);
	   ATH_MSG_WARNING("measPerigee? " << measPerigee);
         if (!measPerigee) {            
	   ATH_MSG_WARNING( "Extrapolation failed 1!!" );
         } 
      }
     
      if (idPerigee!=0 && measPerigee!=0) {
        
         float measEta = measPerigee->eta();
         float measPhi = measPerigee->parameters()[Trk::phi0];
         float measTheta = measPerigee->parameters()[Trk::theta]; 
/*        
         // id track and ms track point in opposite directions
         if (idPerigee->parameters()[Trk::phi0] * measPerigee->parameters()[Trk::phi0] < 0) {
            measPhi += TMath::Pi();
            if (measPhi < -TMath::Pi()) measPhi+=2.*TMath::Pi();
            if (measPhi > +TMath::Pi()) measPhi-=2.*TMath::Pi();
            measTheta = TMath::Pi() - measTheta ;
            measEta = -measEta;
         }
*/
         // Now loop over second set of tracks and muons to form inv. masses
         // but do this only if beam!
         if (m_doRunBeam) {
            for (idtracksItr2=idtracksItr+1, muontracksItr2=muontracksItr+1; idtracksItr2!=idtracks.end(); 
                 ++idtracksItr2, ++muontracksItr2) {
              
            
               const Trk::Perigee *idPerigee2   = &((*idtracksItr2)->perigeeParameters());
               const Trk::Perigee *measPerigee2 = &((*muontracksItr2)->perigeeParameters());
               
               if (measPerigee2) {
                  measPerigee2 = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*measPerigee2,(idPerigee2->associatedSurface()),Trk::anyDirection,false,Trk::muon));
                  if (!measPerigee2) {
		    ATH_MSG_WARNING( "Extrapolation failed 2!!" );
                  } 
               }
               
               if (idPerigee2!=0 && measPerigee2!=0) {
                  
                  // We should pay attention only to opposite sign tracks for forming masses
                  if (measPerigee->parameters()[Trk::qOverP]*measPerigee2->parameters()[Trk::qOverP] < 0) {
                     
/*                     float measEta2 = measPerigee2->eta();
                     float measPhi2 = measPerigee2->parameters()[Trk::phi0];
                     float measTheta2 = measPerigee2->parameters()[Trk::theta]; 
                     
                     // id track and ms track point in opposite directions
                     if (idPerigee2->parameters()[Trk::phi0] * measPerigee2->parameters()[Trk::phi0] < 0) {
                        measPhi2 += TMath::Pi();
                        if (measPhi2 < -TMath::Pi()) measPhi2+=2.*TMath::Pi();
                        if (measPhi2 > +TMath::Pi()) measPhi2-=2.*TMath::Pi();
                        measTheta2 = TMath::Pi() - measTheta2 ;
                        measEta2 = -measEta2;
                     }
*/                     
                     Float_t diffz0ID = TMath::Abs(idPerigee->parameters()[Trk::z0]-idPerigee2->parameters()[Trk::z0]);
                     Float_t diffz0Muon = TMath::Abs(measPerigee->parameters()[Trk::z0]-measPerigee2->parameters()[Trk::z0]);
                     
                     Float_t diffd0ID = TMath::Abs(idPerigee->parameters()[Trk::d0]-idPerigee2->parameters()[Trk::d0]);
                     Float_t diffd0Muon = TMath::Abs(measPerigee->parameters()[Trk::d0]-measPerigee2->parameters()[Trk::d0]);
                     
                     ATH_MSG_DEBUG( " diffz0id = " << diffz0ID << " diffz0Muon = " << diffz0Muon );
                     ATH_MSG_DEBUG( " diffd0id = " << diffz0ID << " diffd0Muon = " << diffz0Muon );
                     
                     CLHEP::HepLorentzVector xvec;
                     
                     if (diffz0ID < m_z0DiffCutID && diffz0Muon < m_z0DiffCutMuon  && diffd0ID < m_d0DiffCutID && diffd0Muon < m_d0DiffCutMuon) {
                        CLHEP::HepLorentzVector muonid1(idPerigee->momentum().x(),idPerigee->momentum().y(),idPerigee->momentum().z(),
                                                 TMath::Sqrt(idPerigee->momentum().x()*idPerigee->momentum().x()+
                                                             idPerigee->momentum().y()*idPerigee->momentum().y()+
                                                             idPerigee->momentum().z()*idPerigee->momentum().z()+105.7*105.7));
                        
                        CLHEP::HepLorentzVector muonid2(idPerigee2->momentum().x(),idPerigee2->momentum().y(),idPerigee2->momentum().z(),
                                                 TMath::Sqrt(idPerigee2->momentum().x()*idPerigee2->momentum().x()+
                                                             idPerigee2->momentum().y()*idPerigee2->momentum().y()+
                                                             idPerigee2->momentum().z()*idPerigee2->momentum().z()+105.7*105.7));
                        
                        CLHEP::HepLorentzVector muonmuon1(measPerigee->momentum().x(),measPerigee->momentum().y(),measPerigee->momentum().z(),
                                                   TMath::Sqrt(measPerigee->momentum().x()*measPerigee->momentum().x()+
                                                               measPerigee->momentum().y()*measPerigee->momentum().y()+
                                                               measPerigee->momentum().z()*measPerigee->momentum().z()+105.7*105.7));
                        
                        CLHEP::HepLorentzVector muonmuon2(measPerigee2->momentum().x(),measPerigee2->momentum().y(),measPerigee2->momentum().z(),
                                                   TMath::Sqrt(measPerigee2->momentum().x()*measPerigee2->momentum().x()+
                                                               measPerigee2->momentum().y()*measPerigee2->momentum().y()+
                                                               measPerigee2->momentum().z()*measPerigee2->momentum().z()+105.7*105.7));
                        
                        
                        if (muonid1.perp() > 1000 && muonid2.perp() > 1000 && muonmuon1.perp() > 1000 && muonmuon2.perp() > 1000) {
                           CLHEP::HepLorentzVector idvec = muonid1+muonid2;
                           CLHEP::HepLorentzVector muonvec = muonmuon1+muonmuon2;
                          
			   			   
                           m_JPsiMassID->Fill(idvec.m());
                           m_JPsiMassMuon->Fill(muonvec.m());
                           m_JPsiMass2D->Fill(idvec.m(), muonvec.m());
                           if (idvec.m() > 2600 && idvec.m() < 3600 && muonvec.m() > 0 && muonvec.m() < 6300)
                              m_JPsiMassDiff->Fill(idvec.m()-muonvec.m());
                           
                           m_UpsilonMassID->Fill(idvec.m());
                           m_UpsilonMassMuon->Fill(muonvec.m());
                           m_UpsilonMass2D->Fill(idvec.m(), muonvec.m());
                           if (idvec.m() > 8000 && idvec.m() < 11000 && muonvec.m() > 0 && muonvec.m() < 19000)
                              m_UpsilonMassDiff->Fill(idvec.m()-muonvec.m());
                             
                           m_JPsiMassID_broad->Fill(idvec.m());
                           m_UpsilonMassID_broad->Fill(idvec.m());
                             
	
                           if (muonid1.perp() > 20000 && muonid2.perp() > 20000 && muonmuon1.perp() > 20000 && muonmuon2.perp() > 20000) {
  
                              m_ZMassID->Fill(idvec.m());
                              m_ZMassMuon->Fill(muonvec.m());
                              m_ZMass2D->Fill(idvec.m(), muonvec.m());
                              if (idvec.m() > 50000 && idvec.m() < 130000 && muonvec.m() > 50000 && muonvec.m() < 130000)
                                 m_ZMassDiff->Fill(idvec.m()-muonvec.m());
                           }
                             
                           //---JPsiCounter ---//
                           if ((idvec.m() > 2400 && idvec.m() < 2800) || (idvec.m() > 3400 && idvec.m() < 3800))
                              m_JPsiCounter->Fill(LumiBlockNumber, -0.5);
                           if (idvec.m() > 2900 && idvec.m() < 3300)
                              m_JPsiCounter->Fill(LumiBlockNumber, 1.0);
                                                        
                        }
                     }
                  }              
               }
  
               if (idPerigee2)  idPerigee2 = NULL;
               if (measPerigee2) measPerigee2 = NULL;
            }
         }

         ATH_MSG_DEBUG( idPerigee->parameters()[Trk::theta]  << " " 
			<< measTheta
			<< "\n"
			<< idPerigee->parameters()[Trk::phi0]   << " " 
			<< measPhi 
			<< "\n"
			<< idPerigee->parameters()[Trk::d0]     << " " 
			<< measPerigee->parameters()[Trk::d0] 
			<< "\n"
			<< idPerigee->parameters()[Trk::z0]     << " " 
			<< measPerigee->parameters()[Trk::z0] 
			<< "\n"
			<< idPerigee->parameters()[Trk::qOverP] << " " 
			<< measPerigee->parameters()[Trk::qOverP] 
			<< "\n"
			<< idPerigee->momentum().perp() << " "
			<< measPerigee->momentum().perp() 
			);

         m_trkEtasGM->Fill(idPerigee->eta(),measEta);
         m_trkThetasGM->Fill(idPerigee->parameters()[Trk::theta],measTheta);
         m_trkPhi0sGM->Fill(idPerigee->parameters()[Trk::phi0],measPhi);
         m_trkd0sGM->Fill(idPerigee->parameters()[Trk::d0],measPerigee->parameters()[Trk::d0]);
         m_trkz0sGM->Fill(idPerigee->parameters()[Trk::z0],measPerigee->parameters()[Trk::z0]);
         m_trkpOverQsGM->Fill(1./idPerigee->parameters()[Trk::qOverP],1./measPerigee->parameters()[Trk::qOverP]);
         m_trkEtaDiff->Fill(idPerigee->eta() - measEta);
         m_trkThetaDiff->Fill(idPerigee->parameters()[Trk::theta] - measTheta);
         m_trkPhi0Diff->Fill(idPerigee->parameters()[Trk::phi0] - measPhi);
         m_trkd0Diff->Fill(idPerigee->parameters()[Trk::d0] - measPerigee->parameters()[Trk::d0]);
         m_trkz0Diff->Fill(idPerigee->parameters()[Trk::z0] - measPerigee->parameters()[Trk::z0]);
         m_trkpOverQDiff->Fill(1./idPerigee->parameters()[Trk::qOverP] - 1./measPerigee->parameters()[Trk::qOverP]);
         m_trkPxDiff->Fill(idPerigee->momentum().x()-measPerigee->momentum().x());
         m_trkPyDiff->Fill(idPerigee->momentum().y()-measPerigee->momentum().y());
         m_trkPzDiff->Fill(idPerigee->momentum().z()-measPerigee->momentum().z());
         m_trkPDiff->Fill(idPerigee->momentum().mag()-measPerigee->momentum().mag());
         m_trkPtDiff->Fill(idPerigee->momentum().perp()-measPerigee->momentum().perp());
         m_trkChargeDiff->Fill(idPerigee->charge()-measPerigee->charge());
         m_trkXDiff->Fill(idPerigee->position().x()-measPerigee->position().x());
         m_trkYDiff->Fill(idPerigee->position().y()-measPerigee->position().y());
         m_trkZDiff->Fill(idPerigee->position().z()-measPerigee->position().z());

         m_trkd0vsPhi0ID->Fill(idPerigee->parameters()[Trk::phi0],idPerigee->parameters()[Trk::d0]);

         m_trkEtaID->Fill(idPerigee->eta());
         m_trkThetaID->Fill(idPerigee->parameters()[Trk::theta]);
         m_trkPhi0ID->Fill(idPerigee->parameters()[Trk::phi0]);
         m_trkd0ID->Fill(idPerigee->parameters()[Trk::d0]);
         m_trkd0IDZOOM->Fill(idPerigee->parameters()[Trk::d0]);
         m_trkz0ID->Fill(idPerigee->parameters()[Trk::z0]);
         m_trkpOverQID->Fill(1./idPerigee->parameters()[Trk::qOverP]);
         m_trkPxID->Fill(idPerigee->momentum().x());
         m_trkPyID->Fill(idPerigee->momentum().y());
         m_trkPzID->Fill(idPerigee->momentum().z());
         m_trkPID->Fill(idPerigee->momentum().mag());
         m_trkPtID->Fill(idPerigee->momentum().perp());
         m_trkChargeID->Fill(idPerigee->charge());
         m_trkXID->Fill(idPerigee->position().x());
         m_trkYID->Fill(idPerigee->position().y());
         m_trkZID->Fill(idPerigee->position().z());

         m_trkEtaMuon->Fill(measEta);
         m_trkThetaMuon->Fill(measTheta);
         m_trkPhi0Muon->Fill(measPhi);
         m_trkd0Muon->Fill(measPerigee->parameters()[Trk::d0]);
         m_trkz0Muon->Fill(measPerigee->parameters()[Trk::z0]);
         m_trkpOverQMuon->Fill(1./measPerigee->parameters()[Trk::qOverP]);
         m_trkPxMuon->Fill(measPerigee->momentum().x());
         m_trkPyMuon->Fill(measPerigee->momentum().y());
         m_trkPzMuon->Fill(measPerigee->momentum().z());
         //m_trkEMuon->Fill(idPerigee->momentum().t()-measPerigee->momentum().t());
         m_trkPMuon->Fill(measPerigee->momentum().mag());
         m_trkPtMuon->Fill(measPerigee->momentum().perp());
         m_trkChargeMuon->Fill(measPerigee->charge());
         m_trkXMuon->Fill(measPerigee->position().x());
         m_trkYMuon->Fill(measPerigee->position().y());
         m_trkZMuon->Fill(measPerigee->position().z());
      }
      if (measPerigee)  measPerigee = NULL;
      if (idPerigee)  idPerigee = NULL;
   } 

   // Muon trk eff
   std::vector<const xAOD::Muon* >::const_iterator MSMuonsItrE = MSMuons.end();
   std::vector<const xAOD::Muon* >::const_iterator IDMuonsItrE = IDMuons.end();

   // MS --> ID phi matching
   for ( MSMuonsItr = MSMuons.begin(); MSMuonsItr != MSMuonsItrE; ++MSMuonsItr)
   {
      float delta = 1.0;
      int eff = 0;

      // accessing track collections
      float muon_phi = (*MSMuonsItr)->phi();
      uint8_t RPCphiLayers = 0;
      uint8_t MDTLayers = 0;
      if ((*MSMuonsItr)->summaryValue(RPCphiLayers, xAOD::numberOfPhiLayers) )
	ATH_MSG_DEBUG( "Could not retrieve number of (RPC) Phi layers with a trigger phi hit" );
      if ((*MSMuonsItr)->summaryValue(MDTLayers, xAOD::numberOfPrecisionLayers) )
	ATH_MSG_DEBUG( "Could not retrieve number of MDT layers with at least 3 hits" );

      for (IDMuonsItr  = IDMuons.begin(); IDMuonsItr  != IDMuonsItrE; ++IDMuonsItr)
      {
         // calculating efficiency
         float muon_ID_phi = (*IDMuonsItr)->phi();
         float delta_phi = fabs(muon_phi - muon_ID_phi);

         float delta_phi2 = fabs(2*TMath::Pi() - delta_phi);
         delta_phi = delta_phi > delta_phi2 ? delta_phi2 : delta_phi;

         if (( delta_phi < delta ) && (delta_phi <= 0.2))
   	     {
            delta = delta_phi;
            eff = 1;
   	     }
         else
            eff = 0;
      }

      // cutting on MS
      if (( RPCphiLayers > 2 ) && ( MDTLayers >= 3 )) // was 10 MDT hits -> 3 MDT layers with at least 3 hits each?
         m_MS2ID_MuontrkeffvsPhi->Fill( muon_phi, eff);	   
   }

   // ID --> MS eta matching
   for (IDMuonsItr = IDMuons.begin(); IDMuonsItr != IDMuonsItrE; ++IDMuonsItr)
   {       
      float delta = 1.0;
      int eff = 0;

      // accessing track containers
      float muon_ID_eta = (*IDMuonsItr)->eta();
      uint8_t TRThits = 0;
      uint8_t SCThits = 0;
      if ((*IDMuonsItr)->summaryValue(SCThits, xAOD::numberOfSCTHits) )
	ATH_MSG_DEBUG( "Could not retrieve number of SCT hits" );
      if ((*IDMuonsItr)->summaryValue(TRThits, xAOD::numberOfTRTHits) )
	ATH_MSG_DEBUG( "Could not retrieve number of TRT hits" );

      for ( MSMuonsItr = MSMuons.begin(); MSMuonsItr != MSMuonsItrE; ++MSMuonsItr)
      {
         // calculating efficiency
         float muon_eta = (*MSMuonsItr)->eta();
         float delta_eta = fabs(muon_eta - muon_ID_eta);

         if (( delta_eta < delta ) && (delta_eta <= 0.2))
   	     {
            delta = delta_eta;
            eff = 1;
   	     }
         else
            eff = 0;
      }

      // cutting on ID
      if ((SCThits > 7) && (TRThits >= 10))
         m_ID2MS_MuontrkeffvsEta->Fill( muon_ID_eta, eff);
   }

   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTMuonIDTrackTool::procHistograms( )
//StatusCode DQTMuonIDTrackTool::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//----------------------------------------------------------------------------------
{
   //if ( endOfEventsBlock || endOfLumiBlock || endOfRun ) {
  if ( endOfLumiBlockFlag() || endOfRunFlag() ) {

    ATH_MSG_DEBUG( "in finalHists()" );
   }

   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTMuonIDTrackTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  ATH_MSG_DEBUG( "in checkHists()" );

  return StatusCode::SUCCESS;
}

