/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/VrtSecInclusive.h"
//#include "GaudiKernel/MsgStream.h"
//-------------------------------------------------
//
#include <string>
//----
#include  "TrkTrack/Track.h"
#include  "TrkTrack/TrackCollection.h"
#include  "Particle/TrackParticleContainer.h"
#include  "TrkParticleBase/LinkToTrackParticleBase.h"
#include  "VxVertex/VxTrackAtVertex.h"
//-----
#include "McParticleEvent/TruthParticleContainer.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
//#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/SimpleVector.h"
#include "TMath.h"

// Event Info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

//----
#include <new>
#include <iostream>


namespace Trk {
extern 
  int  pgraphm_(
       long int *weit, long int *edges, long int *nodes,
       long int *set, long int *nptr,  long int *nth);
}

namespace VKalVrtAthena{


//Constructor and destructor
  VrtSecInclusive::VrtSecInclusive(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_a0TrkPVDstMinCut(0.),
    m_a0TrkPVDstMaxCut(1000.),
    m_zTrkPVDstMinCut(0.),
    m_zTrkPVDstMaxCut(1000.),
    m_a0TrkPVSignifCut(0.),
    m_zTrkPVSignifCut(0.),
    m_TrkChi2Cut(3.),
    m_SelVrtChi2Cut(4.5),
    m_TrkPtCut(500.),
    m_CutSctHits(0),
    m_CutPixelHits(0),
    m_CutSiHits(0),
    m_CutBLayHits(0),
    m_CutSharedHits(0),
    m_A0TrkErrorCut(10.),
    m_ZTrkErrorCut(20.),
    m_VertexMergeCut(3.),
    m_TrackDetachCut(6.),
    m_FillHist(0),
    m_LoPtCut(500.),
    
//    m_sumSvc("Trk::TrackSummaryTool/AtlasTrackSummaryTool"),
    m_fitSvc("Trk::TrkVKalVrtFitter",this),
    m_truthToTrack("Trk::TruthToTrack/InDetTruthToTrack"),
    m_trackToVertexTool("Reco::TrackToVertex"),
    m_trackToVertexIPEstimatorTool("Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator")
  {

    m_TrackLocation     = "TrackParticleCandidate";
    m_PrimVrtLocation   = "VKalPrimVrtCollection";
    m_SecVrtLocation    = "VKalSecVrtCollection";
    m_BeamPosition.clear();
    m_BeamPosition.push_back(0.);   
    m_BeamPosition.push_back(0.);
    

    declareProperty("TrackLocation",      m_TrackLocation);
    declareProperty("PrimVrtLocation",    m_PrimVrtLocation);
    declareProperty("SecVrtLocation",     m_SecVrtLocation);
    declareProperty("BeamPosition",       m_BeamPosition);

    // default values are set upstream - check top of file

    declareProperty("a0TrkPVDstMinCut",   m_a0TrkPVDstMinCut);
    declareProperty("a0TrkPVDstMaxCut",   m_a0TrkPVDstMaxCut);
    declareProperty("zTrkPVDstMinCut",    m_zTrkPVDstMinCut);
    declareProperty("zTrkPVDstMaxCut",    m_zTrkPVDstMaxCut);
    declareProperty("a0TrkPVSignifCut",   m_a0TrkPVSignifCut);
    declareProperty("zTrkPVSignifCut",    m_zTrkPVSignifCut);

    declareProperty("TrkChi2Cut",         m_TrkChi2Cut);
    declareProperty("SelVrtChi2Cut",      m_SelVrtChi2Cut);
    declareProperty("TrkPtCut",           m_TrkPtCut=1000.);   
    declareProperty("TrkLoPtCut",         m_LoPtCut=250.);   
    declareProperty("TrkA0ErrCut",        m_A0TrkErrorCut);
    declareProperty("TrkZErrCut",         m_ZTrkErrorCut);
    declareProperty("CutSctHits",    	  m_CutSctHits  );
    declareProperty("CutPixelHits",	  m_CutPixelHits  );
    declareProperty("CutSiHits",  	  m_CutSiHits  );
    declareProperty("DoSAloneTRT",        m_SAloneTRT=false);
    declareProperty("CutBLayHits",	  m_CutBLayHits  );
    declareProperty("CutSharedHits",	  m_CutSharedHits );
    declareProperty("VertexMergeCut",	  m_VertexMergeCut );
    declareProperty("TrackDetachCut",	  m_TrackDetachCut );
    declareProperty("FillHist",   	  m_FillHist=true  );
    declareProperty("DoTruth",            m_doTruth=false);
    declareProperty("RemoveFake2TrkVrt",  m_removeFakeVrt=true);
    declareProperty("McParticleContainer",m_truthParticleContainerName = "SpclMC");
    declareProperty("MCEventContainer",   m_mcEventContainerName = "GEN_AOD");
    declareProperty("MCTrackResolution",  m_mcTrkResolution=0.06); // see getTruth for explanation
    declareProperty("TruthTrkLen",        m_TruthTrkLen=1000);
    declareProperty("ExtrapPV",           m_extrapPV=false); // Leave false. only for testing

//    declareProperty("TrackSummaryTool", m_sumSvc, " Public InDet TrackSummaryTool" );
    declareProperty("VertexFitterTool", m_fitSvc, " Private TrkVKalVrtFitter" );
    declareProperty("Extrapolator", m_extrapolator);
//
//-- Conversion units  
    m_cnv_xyz=0.1;     /* conversion units for distance mm*/ 
    m_cnv_mom=1000.;   /* conversion units for energy MeV*/   
    //
    m_SelTrk_barcode = new std::vector<int>;
  }

  VrtSecInclusive::~VrtSecInclusive()
  {    
    ATH_MSG_DEBUG("VrtSecInclusive destructor called");
  }




//__________________________________________________________________________
  StatusCode VrtSecInclusive::beginRun()  { 

    ATH_MSG_INFO("VrtSecInclusive beginRun()");
    //
    m_pi = 139.57018; // mass of charged pion in MeV
    m_e  = 0.511;     // electron mass in MeV
    m_PI = 4.*atan(1.);  // value of PI
    ATH_MSG_VERBOSE("Charged electron/pion mass " << m_e<<","<<m_pi);
    //
    // average radii of various material layers - use both MC and data
    //
    m_avRad_bp = 32;
    m_avRad_pix1 = 52.5;
    m_avRad_pix2 = 90.5;
    m_avRad_pix3 = 125; // use just MC, since I don't have enough in data
    m_avRad_sct1 = 302;
    m_avRad_sct2 = 373;

    return StatusCode::SUCCESS;
  }

//---------------- HBOOK
  StatusCode VrtSecInclusive::initialize()
  {
    ITHistSvc*     hist_root=0;
    StatusCode sc;
    ATH_MSG_INFO("VrtSecInclusive initialize()");
    //
    // first instantiate tools

    //  VKalVrt vertex fitter
    if (m_fitSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("Can't find Trk::TrkVKalVrtFitter");
       return StatusCode::SUCCESS; 
    } else {
      ATH_MSG_INFO("Trk::TrkVKalVrtFitter found");
    }

    //----------------------------
    // this block is obsolete
    //    m_SummaryToolExist = 0;
    //    if (m_sumSvc.retrieve().isFailure()) {
    //       ATH_MSG_INFO("Could not find TrackSummaryTool");
    //       ATH_MSG_INFO("Default values for Trk::Track will be used");
    //    } else {
    //        m_SummaryToolExist = 1;
    //        ATH_MSG_INFO("TrkSummaryTool found");
    //    }
    //----------------------------
    //
    // retreive tool to get trackParameters of generated Particles
    if(m_truthToTrack.retrieve().isFailure()) {
      ATH_MSG_INFO("Cannot retrieve Trk::TruthToTrack Tool!");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO("Retrieved Trk::TruthToTrack Tool" << m_truthToTrack);

    }
    // extract TrackToVertex extrapolator tool
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("failed to retrieve trackToVertex tool ");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_INFO("Retrieved Reco::TrackToVertex Tool" << m_trackToVertexTool);
    }
    // extract TrackToVertexIPEstimator extrapolator tool
    if ( m_trackToVertexIPEstimatorTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("failed to retrieve trackToVertexIPEstimator tool ");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_INFO("Retrieved Trk::TrackToVertexIPEstimator Tool" << m_trackToVertexIPEstimatorTool);
    }
    //

    // now make histograms/ntuples

    if(m_FillHist){
       sc = service( "THistSvc", hist_root); 
       if( sc.isFailure() ) {
         ATH_MSG_ERROR("Could not find THistSvc service");
       }
       ATH_MSG_INFO("VrtSecInclusiv Histogram found");
       //
       m_hb_massPiPi   = new TH1D("91"," massPiPi",200,200., 800.);
       m_hb_2Ddist    = new TH1D("92"," 2Ddist", 200,0., 500.);
       m_hb_massEE     = new TH1D("93"," massEE", 100,0., 200.);
       m_hb_nvrt2      = new TH1D("94"," vertices2", 50,0., 50.);
       m_hb_ratio      = new TH1D("95"," ratio", 50,0., 1.);
       m_trkSelCuts    = new TH1D("TrkSelCuts"," TrkSelCuts ",30,0.,30.);
       std::string histDir("/AANT/stat/SecVrtInclusive/");
       sc = hist_root->regHist(histDir+"91", m_hb_massPiPi);
       sc = hist_root->regHist(histDir+"92", m_hb_2Ddist);
       sc = hist_root->regHist(histDir+"93", m_hb_massEE );
       sc = hist_root->regHist(histDir+"94", m_hb_nvrt2);
       sc = hist_root->regHist(histDir+"95", m_hb_ratio);
       sc = hist_root->regHist(histDir+"TrkSelCuts", m_trkSelCuts);
       //
       if( sc.isFailure() ) {     // Check of StatusCode
         ATH_MSG_INFO("VrtSecInclusive Histogram registration failure!!!");
       }
       w_1 = 1.;

       // set up ntuple here. first make new vectors
       m_tree_Vert = new TTree("tree_VrtSec","TTree of VrtSecInclusive"); 
       sc = hist_root->regTree("/AANT/tree_VrtSec", m_tree_Vert);
       if(sc.isFailure()) {
	 ATH_MSG_WARNING("problem registering ntuple");
         return StatusCode::SUCCESS;
       }
       sc = setupNtuple();
       if(sc.isFailure()) {
	 ATH_MSG_WARNING("problem initializing ntuple");
         return StatusCode::SUCCESS;
       }
    }
    // 
    ATH_MSG_INFO("Exit VrtSecInclusive::initialize()");
    return StatusCode::SUCCESS; 
  }



//__________________________________________________________________________


  StatusCode VrtSecInclusive::finalize()
  {
    //
    ATH_MSG_INFO("VrtSecInclusive finalize()");
    return StatusCode::SUCCESS; 
  }

// 
StatusCode VrtSecInclusive::initEvent()
{

  ATH_MSG_DEBUG("in initEvent ");
  m_allTrks=0;
  m_selTrks=0;
  m_sizeIncomp=0;
  //
  m_numPV=0;
  m_numDOFPV->clear();
  m_numTrksPV =0; 
  m_PVX=0;
  m_PVY=0;
  m_PVZ=0;
  m_PVZpile->clear();
  //
  m_numAll2trkVert=0;
  m_numAfFak2trkVert=0;
  m_All2trkMass->clear();
  m_All2trkPt->clear();
  m_All2trkChg->clear();
  m_All2trkVertX->clear();
  m_All2trkVertY->clear();
  m_All2trkVertZ->clear();
  m_All2trkVertChi2->clear();
  m_All2trkBLHSum->clear();
  ATH_MSG_DEBUG("in initEvent:1 ");
  //
  m_AfFak2trkMass->clear();
  m_AfFak2trkPt->clear();
  m_AfFak2trkChg->clear();
  m_AfFak2trkVertX->clear();
  m_AfFak2trkVertY->clear();
  m_AfFak2trkVertZ->clear();
  m_AfFak2trkVpos->clear();
  m_AfFak2trkVertChi2->clear();
  ATH_MSG_DEBUG("in initEvent:1a ");
  //
  m_RecoTrk_pT->clear(); 
  m_RecoTrk_eta->clear(); 
  m_RecoTrk_phi->clear(); 
  m_RecoTrk_chi2->clear(); 
  m_RecoTrk_2dIPPV->clear(); 
  m_RecoTrk_zIPPV->clear(); 
  m_RecoTrk_2dIP->clear(); 
  m_RecoTrk_zIP->clear(); 
  m_RecoTrk_numBLay->clear(); 
  m_RecoTrk_PixBar1->clear();
  m_RecoTrk_PixBar2->clear();
  m_RecoTrk_numPix->clear(); 
  m_RecoTrk_numSCT->clear(); 
  m_RecoTrk_numTRT->clear(); 
  ATH_MSG_DEBUG("in initEvent:2 ");
  /*
  m_RecoTrk_SctBar0->clear();
  m_RecoTrk_SctBar1->clear();
  m_RecoTrk_SctBar2->clear();
  m_RecoTrk_SctBar3->clear();
  */
  m_RecoTrk_barcode->clear();
  m_SelTrk_barcode->clear();
  m_RecoTrk_matchPr->clear();
  m_RecoTrk_A0Error->clear();
  m_RecoTrk_ZError->clear();
  m_RecoTrk_PixShare->clear(); 
  m_RecoTrk_SctShare->clear(); 
  m_RecoTrk_TrkAuth->clear();
  m_RecoTrk_TrkLowPt->clear();

  //  m_RecoTrk_numTRTholes->clear();
  //m_RecoTrk_numTRTout->clear();

  m_RecoTrk_Int_bpZ->clear();
  m_RecoTrk_Int_pix1Z->clear();
  m_RecoTrk_Int_pix2Z->clear();
  m_RecoTrk_Int_pix3Z->clear();
  m_RecoTrk_Int_sct1Z->clear();
  m_RecoTrk_Int_sct2Z->clear();
  /*
  m_RecoTrk_BLayOut->clear();
  m_RecoTrk_BLayX->clear();
  m_RecoTrk_BLayY->clear();
  m_RecoTrk_BLayZ->clear();
  */
  //
  ATH_MSG_DEBUG("in initEvent:3 ");
  m_SVTrk_pT->clear(); 
  m_SVTrk_p->clear(); 
  m_SVTrk_eta->clear(); 
  m_SVTrk_phi->clear(); 
  m_SVTrk_2dIP->clear(); 
  m_SVTrk_zIP->clear(); 
  m_SVTrk_delp->clear(); 
  m_SVTrk_del2dIP->clear(); 
  m_SVTrk_delzIP->clear(); 
  m_SVTrk_numBLay->clear(); 
  m_SVTrk_numPix->clear(); 
  m_SVTrk_numSCT->clear(); 
  m_SVTrk_numTRT->clear(); 
  m_SVTrk_barcode->clear();
  m_SVTrk_matchPr->clear();
  m_SVTrk_TrkAuth->clear();
  m_SVTrk_TrkLowPt->clear();
  /*
  */
  //
  m_InitialSVsize=0;
  m_RearrangedSVsize=0;
  m_numSecVrt=0;
  m_SecVtxX->clear();
  m_SecVtxY->clear();
  m_SecVtxZ->clear();
  m_numTrksSecVtx->clear();
  m_massSecVtx->clear();
  m_masseSecVtx->clear();
  m_chi2SecVtx->clear();
  m_pTSecVtx->clear();
  m_pZSecVtx->clear();
  m_chgSecVtx->clear();
  m_SumBLayHits->clear();
  m_AllTrksBLayHits->clear();
  m_SecVtxMinOpAng->clear();
  m_SecVtxTrkPt->clear();
  m_SecVtxTrkPhi->clear();
  m_SecVtxTrkEta->clear();
  m_SecVtxTrkBLay->clear();
  m_SecVtxTrkPixExclBLay->clear();
  m_SecVtxTrkSCT->clear();
  //
  ATH_MSG_DEBUG("in initEvent:4 ");

  m_numpT= 0;
  m_numChg= 0; // includes incident pion
  m_numEta= 0;
  m_numPix= 0;
  m_numpt200=0;
  //
  m_truthSV1X=0;
  m_truthSV1Y=0;
  m_truthSV1Z=0;
  m_numTrksSV1a=0;
  m_numTrksSV1b=0;
  //
  m_truthEta= -10;
  m_truthPhi= -10;
  m_truthPt=  0;
  m_truthInt= -1;
  //
  m_nSVLoPt=0;
  m_nSVHiPt=0;
  m_nSVGe1HiPt=0;
  m_truthAllVtxType->clear();
  m_truthAllSVX->clear();
  m_truthAllSVY->clear();
  m_truthAllSVZ->clear();
  //
  ATH_MSG_DEBUG("in initEvent:5 ");
  m_truthAllNumTrks->clear();
  m_truthAllVtxTrk1Pt->clear();
  m_truthAllVtxTrk2Pt->clear();
  m_truthAllVtxTrk3Pt->clear();
  m_truthAllVtxTrk4Pt->clear();
  m_truthAllVtxTrk5Pt->clear();
  m_truthAllNumRecoTrks->clear();
  m_truthAllCharge->clear();
  m_truthAllSumTrksPt->clear();
  m_truthAllSumTrksPz->clear();
  m_truthAllVtxHasLifetime->clear();
  m_truthAllVtxStrangeBaryon->clear();
  m_truthAllVtxIncomingPID->clear();
  m_truthAllVtxNumIncident->clear();
  m_truthAllVtxPtIncident->clear();
  m_truthAllVtxPzIncident->clear();
  m_truthAllMinOpAng->clear();
  m_truthAllTrk2dIP->clear();
  m_truthAllTrkzIP->clear();
  m_truthAllTrkPt->clear();
  m_truthAllTrkEta->clear();
  m_truthAllTrkPhi->clear();
  m_truthAllTrkR->clear();
  m_truthAllTrkZ->clear();
  m_truthAllTrkbc->clear();
  m_truthAllRefitStatus->clear();
  m_truthAllNumReTrk->clear();
  m_truthAllRefitChi2->clear();
  m_truthAllRefitMass->clear();
  m_truthAllRefitSVX->clear();
  m_truthAllRefitSVY->clear();
  m_truthAllRefitSVZ->clear();
  //
  /*
  m_TruthTrk_pt->clear();
  m_TruthTrk_eta->clear();
  m_TruthTrk_phi->clear();
  m_TruthTrk_R->clear();
  m_TruthTrk_Z->clear();
  m_TruthTrk_EndR->clear();
  m_TruthTrk_EndZ->clear();
  m_TruthTrk_L->clear();
  m_TruthTrk_barcode->clear();
  m_TruthTrk_pid->clear();
  m_TruthTrk_matchPr->clear();
  m_TruthTrk_2dIP->clear();
  m_TruthTrk_ZIP->clear();
  */
  //
  ATH_MSG_DEBUG("returning from initEvent ");
  return StatusCode::SUCCESS;
   
}
//__________________________________________________________________________
 StatusCode VrtSecInclusive::execute()
 {
   //
   ATH_MSG_DEBUG("VrtSecInclusive execute()");

    // clear ntuple variables
    StatusCode sc = initEvent();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Problem in initEvent ");
       return StatusCode::SUCCESS;
    }
    // add event level info to ntuple
    sc = addEventInfo();
    if (sc.isFailure() ) {
      ATH_MSG_WARNING("Failure in getEventInfo() ");
      return StatusCode::SUCCESS;
    }
    //
    // now start algorithm
    //
    vector<Trk::TrackParticleBase*>  SelectedTrackBase; 

//--------------------------------------------------------
//  Extract tracks from Trk::Track container
//
    vector<const Trk::TrackParticleBase*>  InitialBaseTracks,SelectedBaseTracks;
    const Trk::TrackParticleBaseCollection* newPrtCol = 0;
    //    StatusCode sc = m_sgSvc->retrieve( newPrtCol, m_TrackLocation);
    sc = evtStore()->retrieve( newPrtCol, m_TrackLocation);
    //
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG("Requested TrackParticleBaseCollection doesn't exist");
      ATH_MSG_DEBUG("Try TrackParticleContainer then");
      const Rec::TrackParticleContainer* oldPrtCol = 0;
      //      sc = m_sgSvc->retrieve( oldPrtCol, m_TrackLocation);
      sc = evtStore()->retrieve( oldPrtCol, m_TrackLocation);
      if ( sc.isFailure() ) {
	ATH_MSG_DEBUG("TrackParticleContainer also doesn't exist. Exit!");
        return StatusCode::SUCCESS;
      }
      // need this later for getting TrackParticle Truth
      if(m_doTruth) setTrackParticleContainer(oldPrtCol);
      // 
      ATH_MSG_DEBUG("Extracted Rec::TrackParticle number=" <<oldPrtCol->size());
      Rec::TrackParticleContainer::const_iterator i_ntrk  = oldPrtCol->begin();
      for (i_ntrk = oldPrtCol->begin(); i_ntrk != oldPrtCol->end(); ++i_ntrk) {
       InitialBaseTracks.push_back((*i_ntrk));
      }
    } else {

      // need this later for getting TrackParticle Truth
      if(m_doTruth) setTrackParticleContainer(newPrtCol);
      //
      ATH_MSG_DEBUG("Extracted Trk::TrackParticleBase number=" <<newPrtCol->size());
      Trk::TrackParticleBaseCollection::const_iterator i_ntrk = newPrtCol->begin();
      for (i_ntrk = newPrtCol->begin(); i_ntrk != newPrtCol->end(); ++i_ntrk) {
       InitialBaseTracks.push_back((*i_ntrk));
      }
    }
//--------------------------------------------------------
//  Primary vertex extraction from TES
//
  sc=evtStore()->retrieve( m_vertexTES, "VxPrimaryCandidate");
  if( sc.isFailure()  ||  !m_vertexTES ) {
    ATH_MSG_WARNING("No AOD vertex container found in TDS"); 
    return StatusCode::SUCCESS;
  }  
  else {
    ATH_MSG_DEBUG(" Primary vertex successful ");
  }
  m_numPV = m_vertexTES->size();
  VxContainer::const_iterator   n_vrtV = m_vertexTES->begin();
  VxContainer::const_iterator   n_vrtE = m_vertexTES->end();
  
  ATH_MSG_DEBUG("Primary Vertex Retreived. #PV ="<<m_numPV);
  Amg::Vector3D  PrmVrt( (*n_vrtV)->recVertex().position().x(),
                         (*n_vrtV)->recVertex().position().y(),
                         (*n_vrtV)->recVertex().position().z());
  m_PVX = PrmVrt.x();
  m_PVY = PrmVrt.y();
  m_PVZ = PrmVrt.z();

  // loop over PV container and get number of tracks of each PV
  for(; n_vrtV!=n_vrtE; ++n_vrtV) {
      m_numDOFPV->push_back((*n_vrtV)->recVertex().fitQuality().numberDoF());
      Amg::Vector3D PrmVrt1( (*n_vrtV)->recVertex().position().x(),
                             (*n_vrtV)->recVertex().position().y(),
                             (*n_vrtV)->recVertex().position().z());
      m_PVZpile->push_back((*n_vrtV)->recVertex().position().z());

      ATH_MSG_DEBUG("PrimVertex x/y/z/nDOF "<<PrmVrt1.x()<<","<<PrmVrt1.y()<<","<<PrmVrt1.z()<<","<<(*n_vrtV)->recVertex().fitQuality().numberDoF()); 
  }

  // loop over InitialBaseTracks, find first one associated with PV,
  // get number of tracks at PV
  std::vector<const Trk::TrackParticleBase*>::const_iterator   i_temp;
  int numTrksPV=0;
  for (i_temp = InitialBaseTracks.begin(); i_temp < InitialBaseTracks.end(); ++i_temp) {

    const Trk::TrackParticleOrigin tpo = (*i_temp)->particleOriginType();
    if(tpo== Trk::PriVtx) numTrksPV++;

  }
  m_numTrksPV = numTrksPV;
  //
  // set up truth information
  //
  if(m_doTruth) {

     // we need the following to look for Truth tracks matching RecoParticle
     //
     const McEventCollection* gen(0);
     sc = evtStore()->retrieve(gen, m_mcEventContainerName);
     if( sc.isFailure() ) {
       ATH_MSG_WARNING("MC Event " << m_mcEventContainerName << " not found."); 
       return StatusCode::SUCCESS;
    } 
    else {
      ATH_MSG_VERBOSE("MC Event " << m_mcEventContainerName << " found with " << gen->size() << " entries."); 
      setMcEventCollection(gen); // for later use
    }
    //
    // TrackTruthParticle Container

    const TrackParticleTruthCollection* trackTruthColl(0);
    //    sc = m_sgSvc->retrieve(trackTruthColl,"TrackParticleTruthCollection");
    sc = evtStore()->retrieve(trackTruthColl,"TrackParticleTruthCollection");
    if (sc.isFailure() || 0 == trackTruthColl) {
      ATH_MSG_ERROR("No TrackParticleTruthCollection found in StoreGate");
      trackTruthColl=0; 
    }
    setTrackParticleTruthCollection(trackTruthColl);
    //
    
  } // do the previous things if m_doTruth is TRUE

//-----------------------------------------------------------
//  Track selection
//

   ATH_MSG_DEBUG("Reco. Tracks in event = "<<(int) InitialBaseTracks.size());

   n_vrtV = m_vertexTES->begin();
   SelGoodTrkParticle( InitialBaseTracks , (*n_vrtV)->recVertex(), SelectedBaseTracks);

   m_allTrks=InitialBaseTracks.size();
   m_selTrks=SelectedBaseTracks.size();

   // look at truth vertices
   // do this after I have filled m_RecoTrk_barcode in Utilities.cxx
   //
   if(m_doTruth) {

     // this method runs on MCEventCollection, i.e., GEN_AOD or TruthEvent in ESD file
     sc = getNewTruthInfo(SelectedBaseTracks);
     if(sc.isFailure()) ATH_MSG_INFO("Couldn't get truth info");
   }

//-------------------------------------------------------
//  Now we are ready. Start search.

    
    int i,j,ii;
    vector<const Trk::TrackParticleBase*>  ListBaseTracks;
    vector<double> Results;
    vector<int> Incomp; Incomp.clear();
    Amg::Vector3D               FitVertex;
    TLorentzVector              Momentum;
    vector< vector<double> > TrkAtVrt; 
    vector<double> Chi2PerTrk,ErrorMatrix;
    long int           Charge;
    double             Chi2 = 0.;
;
    long int NTracks = (int) (SelectedBaseTracks.size());
    ATH_MSG_DEBUG("Selected Tracks = "<<NTracks);

    // fill information about selected tracks in AANT
    //
    std::vector<const Trk::TrackParticleBase*>::const_iterator   i_ntrk;

    const Trk::Perigee* m_mPer;
    for (i_ntrk=SelectedBaseTracks.begin(); i_ntrk<SelectedBaseTracks.end(); ++i_ntrk) {
      //
      const Trk::TrackSummary* testSum = (*i_ntrk)->trackSummary();
      m_SVTrk_numPix->push_back( (int) testSum->get(Trk::numberOfPixelHits));
      m_SVTrk_numSCT->push_back((int) testSum->get(Trk::numberOfSCTHits));
      m_SVTrk_numTRT->push_back((int) testSum->get(Trk::numberOfTRTHits));
      m_SVTrk_numBLay->push_back((int) testSum->get(Trk::numberOfBLayerHits));
      //
      // now get track author
      //
      const Trk::TrackInfo& trackInfo = (*i_ntrk)->info();
      int sitrk = trackInfo.patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder);
      int sitrkWTRT = trackInfo.patternRecoInfo(Trk::TrackInfo::InDetExtensionProcessor);
      int BackTrk = trackInfo.patternRecoInfo(Trk::TrackInfo::TRTSeededTrackFinder);
      int SAlone = trackInfo.patternRecoInfo(Trk::TrackInfo::TRTStandalone);
      //
      int trkauth=0;
      if(sitrk==1 && sitrkWTRT==1) trkauth=1;
      if(sitrk==1 && sitrkWTRT==0) trkauth=2;
      if(BackTrk==1) trkauth=3;
      if(SAlone==1) trkauth=4;
      if((sitrk==1 || sitrkWTRT==1) && (BackTrk==1 || SAlone==1)) trkauth=-1;
      //
      m_SVTrk_TrkAuth->push_back(trkauth);
      //      int lowpttrk = trackInfo.trackProperties(Trk::TrackInfo::LowPtTrack); 
      int lowpttrk =  trackInfo.patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum);
      m_SVTrk_TrkLowPt->push_back(lowpttrk);
      //

      //
      // get perigee params
      //
      m_mPer=GetPerigee( (*i_ntrk) ) ;
      if( m_mPer == NULL ){ continue; } 

      double phi = m_mPer->parameters()[Trk::phi];
      double theta = m_mPer->parameters()[Trk::theta];
      double d0 = m_mPer->parameters()[Trk::d0];
      double qOverP = m_mPer->parameters()[Trk::qOverP];
      double errqOverP = (*(m_mPer->covariance()))(Trk::qOverP,Trk::qOverP);
      double errd0 = (*(m_mPer->covariance()))(Trk::d0,Trk::d0);
      //
      double ptrk = (1./qOverP);
      double pT = ptrk*sin(theta);
      double trketa = -1.*log( tan(theta/2));

      m_SVTrk_pT->push_back(pT);
      m_SVTrk_p->push_back(ptrk);
      m_SVTrk_phi->push_back(phi);
      m_SVTrk_eta->push_back(trketa);
      m_SVTrk_2dIP->push_back(d0);
      m_SVTrk_zIP->push_back(m_mPer->parameters()[Trk::z0]);

      double matchProb=-1;
      int barcode = 0;
      if(m_doTruth) {
	HepMC::GenParticle* aTemp_truth = getTrkGenParticle((*i_ntrk), matchProb);
	if(aTemp_truth) {// found it
          barcode = aTemp_truth->barcode();
	}
      }
      m_SVTrk_barcode->push_back(barcode);
      m_SVTrk_matchPr->push_back(matchProb);

      ATH_MSG_DEBUG(" Sel Trk d0/pT/eta/match bc/pr "<<d0<<","<<pT<<","<<trketa<<","<<barcode<<","<<matchProb);

      double errp = ptrk*ptrk*errqOverP;
      m_SVTrk_delp->push_back(errp);
      m_SVTrk_del2dIP->push_back(errd0);
      m_SVTrk_delzIP->push_back( (*(m_mPer->covariance()))(Trk::z0,Trk::z0) );

    } // end of selected tracks
    /*
    */
    //
    // first make all 2-track vertices
    for (i=0; i<NTracks-1; i++) {
        for (j=i+1; j<NTracks; j++) {
	     ListBaseTracks.clear();
	     Incomp.push_back(i);Incomp.push_back(j);
	     
	     ListBaseTracks.push_back( SelectedBaseTracks[i] );
	     ListBaseTracks.push_back( SelectedBaseTracks[j] );
	     // new code to find initial approximate vertex
             Amg::Vector3D IniVertex;
             sc= m_fitSvc->VKalVrtFitFast(ListBaseTracks,IniVertex);/* Fast crude estimation */
             if(sc.isFailure()) ATH_MSG_DEBUG("fast crude estimation fails ");
             m_fitSvc->setApproximateVertex(IniVertex.x(), IniVertex.y(),IniVertex.z());
	     //
             sc=m_fitSvc->VKalVrtFit(ListBaseTracks,FitVertex, Momentum,Charge,
                                         ErrorMatrix,Chi2PerTrk,TrkAtVrt,Chi2);
 
	     if(sc.isFailure())            continue;          /* No fit */ 
             m_numAll2trkVert++;
             m_All2trkVertChi2->push_back(Chi2);
             m_All2trkVertX->push_back(FitVertex.x());
             m_All2trkVertY->push_back(FitVertex.y());
             m_All2trkVertZ->push_back(FitVertex.z());
             m_All2trkMass->push_back(Momentum.M());
             m_All2trkPt->push_back(Momentum.Perp());
             m_All2trkChg->push_back(Charge);
	     //
             const Trk::TrackSummary* trkiSum = SelectedBaseTracks[i]->trackSummary();
             const Trk::TrackSummary* trkjSum = SelectedBaseTracks[j]->trackSummary();
             int trkiBLHit = trkiSum->get(Trk::numberOfBLayerHits);
             int trkjBLHit = trkjSum->get(Trk::numberOfBLayerHits);
             m_All2trkBLHSum->push_back(trkiBLHit+trkjBLHit);

             double rad = sqrt(FitVertex.x()*FitVertex.x() + FitVertex.y()*FitVertex.y()); 
	     // remove if the vertex is inside the beampipe
	     // if(rad < 25)continue;
	     //
	     if(Chi2>m_SelVrtChi2Cut)           continue;          /* Bad Chi2 */
	     //
             ATH_MSG_VERBOSE("2-track vrt mass/pt "<<Momentum.M()<<","<<Momentum.Perp());
	     //
	     // now check hits on track, i.e., 
	     //
             if(m_removeFakeVrt) {
	       //
               bool Pix1iHit = trkiSum->isHit(Trk::pixelBarrel1); 
               bool Pix1jHit = trkjSum->isHit(Trk::pixelBarrel1); 
	       //bool Pix1Hit = Pix1iHit && Pix1jHit; // both tracks have hits in Pixel1
	       //
               bool Pix2iHit = trkiSum->isHit(Trk::pixelBarrel2); 
               bool Pix2jHit = trkjSum->isHit(Trk::pixelBarrel2); 
               bool Pix2Hit = Pix2iHit && Pix2jHit; // both tracks have hits in Pixel2
	       //
	       // look at all SCT hits and not just barrel SCT hits
	       //
               bool SctiHit = trkiSum->get(Trk::numberOfSCTHits) >0;
               bool SctjHit = trkjSum->get(Trk::numberOfSCTHits) >0;
               bool SctHit = SctiHit && SctjHit; // both tracks have some SCT
	       //
	       // rough guesses for active layers:
	       // Pix0 (BLayer): 47.7-54.4, Pix1: 85.5-92.2, Pix2: 119.3-126.1:
	       // Sct0: 290-315, Sct1: 360-390, Sct2: 430-460, Sct3:500-530
	       //
 	       // if vertex has R< BLayer radius, tracks in it need to have hits in BLayer
	       // if vertex is outside BLayer, tracks can't have hits in BLayer
               // if vertex is within BLayer, require some Pixel hits on both trks
               if(rad<=47.7 && (trkiBLHit<1 || trkjBLHit<1)) continue;
               if(rad>54.4 && (trkiBLHit>0 || trkjBLHit>0)) continue;
               bool somePixi = trkiBLHit>0 || Pix1iHit || Pix2iHit ; //trki has some Pixel Hit
               bool somePixj = trkjBLHit>0 || Pix1jHit || Pix2jHit; // trkj has some Pixel Hit
               bool somePix = somePixi && somePixj; // both trks have some Pixel hits
               if((rad>47.7 && rad <=54.4) && !somePix) continue;
               //
	       // if rad is between Pix0 and Pix1, 
	       // both tracks should have hits in Pix1 or Pix2 but none in Pix0
               somePixi = Pix1iHit || Pix2iHit ; // trki has hit in Pix1 or Pix2
               somePixj = Pix1jHit || Pix2jHit ;// trkj
               somePix = somePixi && somePixj;  // both trks have hits in either Pix1 and/or Pix2

               if((rad > 54.4 && rad <=92.2) && !somePix) continue; 
	       //
	       // if rad is greater Pix1, neither track should have any hit in Blayer or Pix1
	       // check on Blayer is redundant at this point
               somePixi = trkiBLHit>0 || Pix1iHit;
               somePixj = trkjBLHit>0 || Pix1jHit;
               if(rad > 92.2 && (somePixi || somePixj)) continue;
	       // if rad is between Pix1 and Pix2, both tracks should have hits in Pix2 & SCT
	       // if rad in Pix2 (119.3-126.1) just require SCT hit - don't check on Pix2

               if((rad> 92.2 && rad <=119.3) && !Pix2Hit) continue;
               if((rad> 92.2 && rad <=126.1) && !SctHit) continue;

	       // if rad > Pix2, neither track can have any Pix hits,should have some SCT hits
	       // check on Blayer is redundant
               somePixi= (trkiBLHit>0 || Pix1iHit || Pix2iHit);
               somePixj = (trkjBLHit>0 || Pix1jHit || Pix2jHit);
               if(rad > 126.1 && (somePixi || somePixj) ) continue;
               if(rad > 126.1 && !SctHit) continue;
	       //
	       //
	     }
	     //
	     Amg::Vector3D vDist=FitVertex-PrmVrt;
	     double vPos=(vDist.x()*Momentum.Px()+vDist.y()*Momentum.Py()+vDist.z()*Momentum.Pz())/Momentum.Rho();
	     //
             m_numAfFak2trkVert++;
             m_AfFak2trkVertChi2->push_back(Chi2);
             m_AfFak2trkVertX->push_back(FitVertex.x());
             m_AfFak2trkVertY->push_back(FitVertex.y());
             m_AfFak2trkVertZ->push_back(FitVertex.z());
             m_AfFak2trkMass->push_back(Momentum.M());
             m_AfFak2trkPt->push_back(Momentum.Perp());
             m_AfFak2trkChg->push_back(Charge);
             m_AfFak2trkVpos->push_back(vPos);

	     if(vPos<-20.) continue;
	     //
	     Incomp.pop_back();Incomp.pop_back();
        }
    } 
  
    ATH_MSG_VERBOSE("Found= "<<Incomp.size()<<", "<<NTracks);
    m_sizeIncomp=Incomp.size();
//
//    Return if no good tracks for search
//    if(Incomp.size() < 2 ) return StatusCode::SUCCESS; //VK Means ALL TRACKS ARE COMPATIBLE!!!

//
//  Creating of initial solution set
//	
    vector<WrkVrt> *WrkVrtSet= new vector<WrkVrt>;
    WrkVrt newvrt;
    long int* weit     = new long int[Incomp.size()];
    long int* Solution = new long int[NTracks];
    for(i=0; i<(int)Incomp.size(); i++) *(weit+i)=(long int) (Incomp[i]+1); /* +1 is needed for PGRAPH*/
    long int edges = Incomp.size()/2; 
    long int NPTR=0;
    long int nth=2;    //VK some speed up
    m_fitSvc->setDefault();

    while(true) {
       Trk::pgraphm_( weit, &edges, &NTracks, Solution, &NPTR, &nth);
//  std::cout << "PGRAPHM="<<NPTR<<'\n';
                                     if(NPTR <= 0)  break;      /* No more solutions   */
                                     if(NPTR == 1)  continue;   /* Not a good solution */
       ListBaseTracks.clear();
       newvrt.Good = true;
       newvrt.SelTrk.clear();
       for(i=0;i<NPTR;i++) {
//  std::cout<<"Solution="<<Solution[i]<<'\n';
           newvrt.SelTrk.push_back(Solution[i]-1);
           ListBaseTracks.push_back( SelectedBaseTracks.at(Solution[i]-1) );
       }
       
       sc=m_fitSvc->VKalVrtFit(ListBaseTracks,
                                    newvrt.vertex,
                                    newvrt.vertexMom,
				    newvrt.Charge,
		                    newvrt.vertexCov,
			            newvrt.Chi2PerTrk, 
				    newvrt.TrkAtVrt,
				    newvrt.Chi2);   
//  std::cout << "Res="<<newvrt.Chi2<<'\n';
       ATH_MSG_DEBUG("FoundAppVrt="<<NPTR<<", "<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<
		     ", "<<newvrt.vertex[2]<<", "<<newvrt.Chi2);
       if( sc.isFailure() )  continue;   /* Bad fit - goto next solution */
       newvrt.nCloseVrt    = 0;
       newvrt.dCloseVrt    = 1000000.;
       WrkVrtSet->push_back(newvrt);
    } 

    int NSoluI=(*WrkVrtSet).size();
    for(int iv=0; iv<NSoluI; iv++){
       if( (*WrkVrtSet)[iv].Chi2 > (4.*(*WrkVrtSet)[iv].SelTrk.size()) )
          DisassembleVertex(WrkVrtSet,iv,SelectedBaseTracks);
    }

    ATH_MSG_DEBUG("Found Initial Solution Set");
    m_InitialSVsize=WrkVrtSet->size();


//
//  Rearrangement of solutions
//
    vector< deque<long int> > *TrkInVrt  =new vector< deque<long int> >(NTracks);  
    double FoundMax; long int SelectedTrack, SelectedVertex;
    double foundMinVrtDst; int foundV1, foundV2;
    
    TrackClassification( WrkVrtSet, TrkInVrt);

    m_fitSvc->setDefault();
    while((FoundMax=MaxOfShared( WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex))>0) {
// std::cout << "MAX="<<FoundMax<<", "<<SelectedTrack<<", "<<SelectedVertex<<'\n';
// std::cout << "VRT="<<minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<", "<<foundV1<<", "<<foundV2<<'\n';
// printWrkSet(WrkVrtSet,"Interm");

          foundMinVrtDst = 10000000.;
          if(FoundMax<m_TrackDetachCut) foundMinVrtDst = minVrtVrtDist( WrkVrtSet, foundV1, foundV2);

//
//Choice of action
          if( FoundMax<m_TrackDetachCut && foundMinVrtDst<m_VertexMergeCut){
             MergeVertices( WrkVrtSet, foundV1, foundV2);
             sc = RefitVertex( WrkVrtSet, foundV1, SelectedBaseTracks);
             if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */
             while ( minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2) < m_VertexMergeCut) {
	        MergeVertices( WrkVrtSet, foundV1, foundV2);
                sc = RefitVertex( WrkVrtSet, foundV1, SelectedBaseTracks);
                if( sc.isFailure() )  break;                            /* Bad fit - goto next solution */
             }
             TrkInVrt->~vector();
             TrkInVrt  =new vector< deque<long int> >(NTracks);  
             TrackClassification( WrkVrtSet, TrkInVrt);
          }else{
             RemoveTrackFromVertex(WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex);
             sc = RefitVertex( WrkVrtSet, SelectedVertex, SelectedBaseTracks);
             if( sc.isFailure() )  continue;                            /* Bad fit - goto next solution */
          }
  
    }
//
//-----------------------------------------------------------------------------------------------
//
    m_RearrangedSVsize=WrkVrtSet->size();
    ATH_MSG_DEBUG("Size of Solution Set"<<m_RearrangedSVsize);

//
//  Select good vertices with refit
//
     Trk::Perigee * tmpMeasPer=0; 
     Amg::MatrixX CovMtx(3,3);
     m_fitSvc->setDefault();
     m_fitSvc->setMomCovCalc(1);
     VxContainer*             nVrtVx =new VxContainer;

     for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          (*WrkVrtSet).at(iv).Good = false;
          ListBaseTracks.clear();
          nth=(*WrkVrtSet).at(iv).SelTrk.size();
	  if(nth <= 1) continue;               /* Bad vertices */
          sc = RefitVertex( WrkVrtSet, iv, SelectedBaseTracks);
          if( sc.isFailure() )  continue;   /* Bad fit - goto next solution */
	  //if( TMath::Prob( (*WrkVrtSet)[iv].Chi2,2*nth-3) < 0.001) continue;

          if(m_FillHist){
	      m_hb_2Ddist->Fill( sqrt( (*WrkVrtSet)[iv].vertex[0]*(*WrkVrtSet)[iv].vertex[0]
                                      +(*WrkVrtSet)[iv].vertex[1]*(*WrkVrtSet)[iv].vertex[1]),w_1);
              if( nth==2 && (*WrkVrtSet)[iv].Charge==0) m_hb_massPiPi->Fill((*WrkVrtSet)[iv].vertexMom.M(),w_1);
          }
          (*WrkVrtSet)[iv].Good = true;          /* Good vertex */
//
//  Store good vertices into StoreGate 
//
          m_numSecVrt++;

          CovMtx(0,0)            =(*WrkVrtSet)[iv].vertexCov[0];
          CovMtx(0,1)=CovMtx(1,0)=(*WrkVrtSet)[iv].vertexCov[1];
          CovMtx(1,1)            =(*WrkVrtSet)[iv].vertexCov[2];
          CovMtx(0,2)=CovMtx(2,0)=(*WrkVrtSet)[iv].vertexCov[3];
          CovMtx(1,2)=CovMtx(2,1)=(*WrkVrtSet)[iv].vertexCov[4];
          CovMtx(2,2)            =(*WrkVrtSet)[iv].vertexCov[5];

          std::vector <double> CovFull;
          sc = m_fitSvc->VKalGetFullCov( (long int)nth, CovFull); 
          if( sc.isFailure() )  ATH_MSG_VERBOSE(" here 6 ");
          int covarExist=0; if( sc.isSuccess() ) covarExist=1;

	  Trk::RecVertex * tmpRecV = new  Trk::RecVertex( 
			    (*WrkVrtSet)[iv].vertex,	
                            CovMtx ,
			   (int) (nth*2-1),
			   (*WrkVrtSet)[iv].Chi2);

	  vector<Trk::VxTrackAtVertex*> * tmpVTAV = new std::vector<Trk::VxTrackAtVertex*>();
          double vert_mass=0; double vert_pt=0; double vert_pz=0;
          double vert_masse=0; //assuming tracks are electrons
          double vx=0; double vy=0; double vz=0; double ve=0; double vee=0;
          int AllBLay = 1;
          int SumBLay=0;
	  // 
          double trkpx[10], trkpy[10], trkpz[10];
          for (int i=0; i<10; ++i) {
            trkpx[i]=0;trkpy[i]=0;trkpz[i]=0;
	  }
	  //
          for(ii=0; ii<nth; ii++) {
               AmgSymMatrix(5) * tmpCovMatr=new AmgSymMatrix(5); (*tmpCovMatr).setIdentity();
               if(covarExist){  FillCovMatrix(ii, CovFull, (*tmpCovMatr) ); }
               tmpMeasPer  =  new Trk::Perigee( 0.,0.,
	       (*WrkVrtSet)[iv].TrkAtVrt[ii][0] ,                                    /* Phi   */
	       (*WrkVrtSet)[iv].TrkAtVrt[ii][1] ,                                    /* Theta */
	       (*WrkVrtSet)[iv].TrkAtVrt[ii][2] ,                                    /* 1/p   */
                    Trk::PerigeeSurface((*WrkVrtSet)[iv].vertex),
                    tmpCovMatr);
  
                Trk::VxTrackAtVertex * tmpPointer = new Trk::VxTrackAtVertex( 1., tmpMeasPer ) ;
 	        int jtrk=(*WrkVrtSet)[iv].SelTrk[ii];                           /*Track number*/
		ElementLink<Trk::TrackParticleBaseCollection> TEL; TEL.setElement(SelectedBaseTracks[jtrk]);
                Trk::LinkToTrackParticleBase *ITL = new Trk::LinkToTrackParticleBase(TEL);    //pointer to initial Prt
                ITL->setStorableObject( *newPrtCol );
                tmpPointer->setOrigTrack(ITL);                //pointer to initial Prt
		// access the original track
                const Trk::TrackParticleBase* origTrk = ITL->cachedElement();
		const Trk::TrackSummary* Sum = origTrk->trackSummary();
                SumBLay += Sum->get(Trk::numberOfBLayerHits);

                if(Sum->get(Trk::numberOfBLayerHits)==0) AllBLay =0;
                tmpVTAV->push_back( tmpPointer );
		//
		// calculate mass/pT of tracks at this vertex
		//
                double theta = (*WrkVrtSet)[iv].TrkAtVrt[ii][1];
                double trk_p = fabs(1./ (*WrkVrtSet)[iv].TrkAtVrt[ii][2]);
                double trk_pz = trk_p*cos(theta);  
                double trk_pt = trk_p*sin(theta);  
                double trk_px = trk_pt*cos((*WrkVrtSet)[iv].TrkAtVrt[ii][0]);  
                double trk_py = trk_pt*sin((*WrkVrtSet)[iv].TrkAtVrt[ii][0]);  
                double trkp2 = trk_p*trk_p;
                double trk_e = sqrt(trkp2 + m_pi*m_pi); 
                double trk_ee = sqrt(trkp2 + m_e*m_e); 
		//
                if(ii<10) { // store info about tracks (upto 10)
		  trkpx[ii]=trk_px;trkpy[ii]=trk_py;trkpz[ii]=trk_pz;
		}

                vert_pt += trk_pt; 
                vert_pz += trk_pz;
                vx += trk_px;
                vy += trk_py;
                vz += trk_pz;
                ve += trk_e; 
                vee += trk_ee;
		//
		// add variables for each track into ntuple
		// Add all vertices to same ntuple, and extract later in root macro 
		//
                m_SecVtxTrkPt->push_back(trk_pt);
                m_SecVtxTrkPhi->push_back((*WrkVrtSet)[iv].TrkAtVrt[ii][0]);
                m_SecVtxTrkEta->push_back(-log(tan(theta/2)));
		m_SecVtxTrkBLay->push_back(Sum->get(Trk::numberOfBLayerHits));
                int diff = Sum->get(Trk::numberOfPixelHits) -Sum->get(Trk::numberOfBLayerHits);
		m_SecVtxTrkPixExclBLay->push_back(diff);
		m_SecVtxTrkSCT->push_back(Sum->get(Trk::numberOfSCTHits));

                ATH_MSG_DEBUG("Tracks in vrt chg/px/py/pz/pt/e/phi/eta = "<<(1./ (*WrkVrtSet)[iv].TrkAtVrt[ii][2])/trk_p<<","<<trk_px<<","<<trk_py<<","<<trk_pz<<","<<trk_pt<<","<<trk_e<<","<<(*WrkVrtSet)[iv].TrkAtVrt[ii][0]<<","<<-log(tan(theta/2)));

    
          } // loop over tracks in vertex
	  //
          Trk::VxCandidate* tmpVx =new Trk::VxCandidate(*tmpRecV,*tmpVTAV);
          delete tmpVTAV;
          delete tmpRecV;

          nVrtVx->push_back(tmpVx);
       
	  // Make vertex mass
          vert_mass = sqrt(ve*ve - vx*vx -vy*vy -vz*vz);
          vert_masse = sqrt(vee*vee - vx*vx -vy*vy -vz*vz);
	  //
          ATH_MSG_DEBUG("Final Sec.Vertex="<<nth<<", "
	     <<(*WrkVrtSet)[iv].vertex[0]<<", "<<(*WrkVrtSet)[iv].vertex[1]<<", "
			<<(*WrkVrtSet)[iv].vertex[2]<<","<<vert_mass<<","<<vert_pt<<","<<vert_masse);

          m_numTrksSecVtx->push_back(nth);
	  m_massSecVtx->push_back(vert_mass);
	  m_masseSecVtx->push_back(vert_masse);
	  m_pTSecVtx->push_back(vert_pt);
	  m_pZSecVtx->push_back(vert_pz);
          m_chi2SecVtx->push_back((*WrkVrtSet)[iv].Chi2);
          m_SecVtxX->push_back((*WrkVrtSet)[iv].vertex[0]);
          m_SecVtxY->push_back((*WrkVrtSet)[iv].vertex[1]);
          m_SecVtxZ->push_back((*WrkVrtSet)[iv].vertex[2]);
          m_chgSecVtx->push_back((*WrkVrtSet)[iv].Charge);
          m_SumBLayHits->push_back(SumBLay);
          m_AllTrksBLayHits->push_back(AllBLay);
	  //
	  // calculate opening angle between all 2-track pairs, and store the minimum
	  //
	  double minOpAng = 1000;
	  double opAng=0;
	  double p1=0;
	  double p2=0;
	  for (int it1=0; it1<ii; ++it1) {
	    if(it1>10) continue;
	    p1 = sqrt(trkpx[it1]*trkpx[it1] + trkpy[it1]*trkpy[it1] + trkpz[it1]*trkpz[it1]);
	    for (int it2=0; it2<ii; ++it2) {
	      if(it2>10) continue;
	      if(it1==it2) continue;
	      //
	      p2 = sqrt(trkpx[it2]*trkpx[it2] + trkpy[it2]*trkpy[it2] + trkpz[it2]*trkpz[it2]);
	      opAng = trkpx[it1]*trkpx[it2]+trkpy[it1]*trkpy[it2]+trkpz[it1]*trkpz[it2];
	      opAng = (opAng/p1)/p2;
	      ATH_MSG_DEBUG("Final Sec Vtx Opening Ang/ minOpAng "<<opAng<<","<<minOpAng);
	      if(opAng < minOpAng) minOpAng = opAng;
	    }
	  }
	  m_SecVtxMinOpAng->push_back(minOpAng);

     } // loop over vertices



    if(nVrtVx->size() > 0) {
      //      sc = m_sgSvc->record(nVrtVx,m_SecVrtLocation,false);
      sc = evtStore()->record(nVrtVx,m_SecVrtLocation,false);
      if ( sc.isFailure() ) {
	ATH_MSG_ERROR("Could not store VKalSecVrtCollection");
      } else {
        ATH_MSG_DEBUG("Secondary vertices saved");
      }
    }
//
//  Memory cleaning
//
    Incomp.clear();
    delete[] weit; delete[] Solution; TrkInVrt->~vector(); WrkVrtSet->~vector();

    
    if(sc.isFailure()) ATH_MSG_INFO("Why is sc still failure here");
    m_tree_Vert->Fill(); // fill ntuple tree
    return StatusCode::SUCCESS;   
 }
  //
  //////////////////////////////////////////////////////
                      /* Service routines*/

   void VrtSecInclusive::DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int iv, 
                         vector<const Trk::TrackParticleBase*>  AllTracks)
   {
      StatusCode sc;    

      WrkVrt newvrt;
      vector<const Trk::TrackParticleBase*>  ListBaseTracks;
      int NTrk=(*WrkVrtSet)[iv].SelTrk.size(), SelT=-1;
      double Chi2Max=0.;
      for(int i=0; i<NTrk; i++){
         if( (*WrkVrtSet)[iv].Chi2PerTrk[i]>Chi2Max) { Chi2Max=(*WrkVrtSet)[iv].Chi2PerTrk[i];  SelT=i;}
      }	    
      if(SelT==-1)return;
      for(int i=0; i<NTrk; i++){
	   if(i==SelT)continue;
           ListBaseTracks.clear();
	   ListBaseTracks.push_back( AllTracks[(*WrkVrtSet)[iv].SelTrk[i]] );
	   ListBaseTracks.push_back( AllTracks[(*WrkVrtSet)[iv].SelTrk[SelT]] );
           newvrt.SelTrk.resize(2); 
	   newvrt.SelTrk[0]=(*WrkVrtSet)[iv].SelTrk[i]; 
	   newvrt.SelTrk[1]=(*WrkVrtSet)[iv].SelTrk[SelT];
           sc = m_fitSvc->VKalVrtFitFast(ListBaseTracks,newvrt.vertex);
           if( sc.isFailure() )  continue;
           m_fitSvc->setApproximateVertex(newvrt.vertex[0],newvrt.vertex[1],newvrt.vertex[2]);
           sc=m_fitSvc->VKalVrtFit(ListBaseTracks,
                                        newvrt.vertex,
                                        newvrt.vertexMom,
				        newvrt.Charge,
		                        newvrt.vertexCov,
			                newvrt.Chi2PerTrk, 
				        newvrt.TrkAtVrt,
				        newvrt.Chi2);   
           if( sc.isFailure() )  continue;  
           newvrt.nCloseVrt    = 0;
           newvrt.dCloseVrt    = 1000000.;
           WrkVrtSet->push_back(newvrt);
      }
      (*WrkVrtSet)[iv].SelTrk.erase( (*WrkVrtSet)[iv].SelTrk.begin() + SelT ); //remove track
      sc = RefitVertex( WrkVrtSet, iv, AllTracks);
      if( sc.isFailure() )  ATH_MSG_INFO(" Wrong vertex");
   }


   void VrtSecInclusive::TrackClassification(std::vector<WrkVrt> *WrkVrtSet, 
                                             std::vector< std::deque<long int> > *TrkInVrt)
   { 
      long int NSet=WrkVrtSet->size(); long int iv, jt, NTrkAtVrt, tracknum;
      for( iv=0; iv<NSet; iv++) {
         NTrkAtVrt=(*WrkVrtSet)[iv].SelTrk.size();
         if(NTrkAtVrt<2) continue;
         for( jt=0; jt<NTrkAtVrt; jt++){
           tracknum=(*WrkVrtSet)[iv].SelTrk[jt];
	   (*TrkInVrt).at(tracknum).push_back(iv);
         }
      }
   }


   double VrtSecInclusive::MaxOfShared(std::vector<WrkVrt> *WrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int & SelectedTrack,
				       long int & SelectedVertex)
   {

      long int NTrack=TrkInVrt->size(); long int it, jv, itmp, NVrt, VertexNumber;
 
      double MaxOf=-999999999;
//
      long int NShMax=0;
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt > NShMax ) NShMax=NVrt;
      }
      if(NShMax<=1)return MaxOf;              /* No shared tracks */
//      
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt < NShMax )    continue;     /*Not a shared track with maximal sharing*/
         for( jv=0; jv<NVrt; jv++ ){
	    VertexNumber=(*TrkInVrt)[it][jv];
	    if( (*WrkVrtSet).at(VertexNumber).SelTrk.size() <= 1) continue; // one track vertex - nothing to do
	    for( itmp=0; itmp<(int)(*WrkVrtSet)[VertexNumber].SelTrk.size(); itmp++){
	       if( (*WrkVrtSet)[VertexNumber].SelTrk[itmp] == it ) {         /* Track found*/  
                  if( MaxOf < (*WrkVrtSet)[VertexNumber].Chi2PerTrk.at(itmp) ){
		      MaxOf = (*WrkVrtSet)[VertexNumber].Chi2PerTrk[itmp];
		      SelectedTrack=it;
		      SelectedVertex=VertexNumber;
		  }
               }
            }
	 }
      }
      return MaxOf;
   }


    void VrtSecInclusive::RemoveTrackFromVertex(std::vector<WrkVrt> *WrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int & SelectedTrack,
				       long int & SelectedVertex)
    {   
	std::deque<long int>::iterator it;
//std::cout<<" In Found ="<<SelectedTrack<<", "<<SelectedVertex<<'\n';
	for(it=(*WrkVrtSet).at(SelectedVertex).SelTrk.begin(); 
	    it!=(*WrkVrtSet)[SelectedVertex].SelTrk.end();     it++) {
	    if( (*it) == SelectedTrack ) { 
	       (*WrkVrtSet)[SelectedVertex].SelTrk.erase(it);
	       break;
	    }     
	}   


	for(it=(*TrkInVrt).at(SelectedTrack).begin(); 
	    it!=(*TrkInVrt)[SelectedTrack].end();     it++) {
	    if( (*it) == SelectedVertex ) { 
	       (*TrkInVrt)[SelectedTrack].erase(it);
	       break;
	    }     
	}   
//Check if track is removed from 2tr vertex => then sharing of track left should also be decreased
        if( (*WrkVrtSet)[SelectedVertex].SelTrk.size() == 1){
	   long int LeftTrack=(*WrkVrtSet)[SelectedVertex].SelTrk[0];  // track left in 1tr vertex
	   for(it=(*TrkInVrt).at(LeftTrack).begin();  it!=(*TrkInVrt)[LeftTrack].end();  it++) {
	      if( (*it) == SelectedVertex ) { 
	       (*TrkInVrt)[LeftTrack].erase(it); break;
	      }     
	   }   
        }

   }

//
//  Minimal normalized vertex-vertex distance
//

   double VrtSecInclusive::minVrtVrtDist( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   {  
     for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) { (*WrkVrtSet)[iv].dCloseVrt=1000000.; (*WrkVrtSet)[iv].nCloseVrt=0;}
     
     double foundMinVrtDst=1000000.;
     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet).at(iv).SelTrk.size()< 2) continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)WrkVrtSet->size(); jv++) {
           if( (*WrkVrtSet).at(jv).SelTrk.size()< 2) continue;   /* Bad vertices */
           double tmp= fabs((*WrkVrtSet)[iv].vertex.x()-(*WrkVrtSet)[jv].vertex.x())
                      +fabs((*WrkVrtSet)[iv].vertex.y()-(*WrkVrtSet)[jv].vertex.y())
                      +fabs((*WrkVrtSet)[iv].vertex.z()-(*WrkVrtSet)[jv].vertex.z());
           if(tmp>15.) continue;
           double tmpDst = VrtVrtDist((*WrkVrtSet)[iv].vertex,(*WrkVrtSet)[iv].vertexCov,
                                      (*WrkVrtSet)[jv].vertex,(*WrkVrtSet)[jv].vertexCov);
           if( tmpDst < foundMinVrtDst){foundMinVrtDst = tmpDst; V1=iv; V2=jv;} 
           if( tmpDst < (*WrkVrtSet)[iv].dCloseVrt ) {(*WrkVrtSet)[iv].dCloseVrt=tmpDst; (*WrkVrtSet)[iv].nCloseVrt=jv;}
           if( tmpDst < (*WrkVrtSet)[jv].dCloseVrt ) {(*WrkVrtSet)[jv].dCloseVrt=tmpDst; (*WrkVrtSet)[jv].nCloseVrt=iv;}
         }
      }
      return foundMinVrtDst;
   }
//   
// Give minimal distance between nonmodifed yet vertices
//   
   double VrtSecInclusive::minVrtVrtDistNext( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   {  
     V1=0; V2=0;
     double foundMinVrtDst=1000000.;
     for(int iv=0; iv<(int)WrkVrtSet->size()-1; iv++) {
        if( (*WrkVrtSet)[iv].SelTrk.size()< 2) continue;   /* Bad vertex */
        if( (*WrkVrtSet)[iv].nCloseVrt==0)     continue;   /* Used vertex */
        if( (*WrkVrtSet)[iv].dCloseVrt <foundMinVrtDst ) {
	   int vtmp=(*WrkVrtSet)[iv].nCloseVrt;
           if((*WrkVrtSet)[vtmp].nCloseVrt==0) { continue;}  // Close vertex to given [iv] one is modified already 
	   foundMinVrtDst=(*WrkVrtSet)[iv].dCloseVrt;
	   V1=iv;
	   V2=vtmp;
	}
      }
      return foundMinVrtDst;
   }
//
//  Merge two close vertices into one (first) and set NTr=0 for second vertex
//
   void VrtSecInclusive::MergeVertices( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2)
   {

      int i;
      int nth=(*WrkVrtSet).at(V2).SelTrk.size();   //number of tracks in second vertex

      for(i=0;i<nth;i++) (*WrkVrtSet)[V1].SelTrk.push_back(   (*WrkVrtSet)[V2].SelTrk[i]   );
//
// Cleaning
      deque<long int>::iterator   TransfEnd ;
      sort( (*WrkVrtSet)[V1].SelTrk.begin(), (*WrkVrtSet)[V1].SelTrk.end() );
      TransfEnd =  unique((*WrkVrtSet)[V1].SelTrk.begin(), (*WrkVrtSet)[V1].SelTrk.end() );
      (*WrkVrtSet)[V1].SelTrk.erase( TransfEnd, (*WrkVrtSet)[V1].SelTrk.end());
//
//----------------------------------------------------------
      (*WrkVrtSet)[V2].SelTrk.clear();     //Clean dropped vertex
      (*WrkVrtSet)[V2].dCloseVrt=1000000.; //Clean dropped vertex
      (*WrkVrtSet)[V2].nCloseVrt=0;        //Clean dropped vertex
      (*WrkVrtSet)[V1].dCloseVrt=1000000.; //Clean new vertex
      (*WrkVrtSet)[V1].nCloseVrt=0;        //Clean new vertex

   }
 
   StatusCode VrtSecInclusive::RefitVertex( vector<WrkVrt> *WrkVrtSet, int SelectedVertex,
                                            vector<const Trk::TrackParticleBase*> & SelectedBaseTracks)
   {
     //
      int i,j;

      int nth = (*WrkVrtSet)[SelectedVertex].SelTrk.size();
 
      if(nth<2) return StatusCode::SUCCESS;

      i = (*WrkVrtSet)[SelectedVertex].Chi2PerTrk.size();
      if( nth > i){
         for( ; i<nth; i++) (*WrkVrtSet)[SelectedVertex].Chi2PerTrk.push_back(100.);
      }
 
      vector<const Trk::TrackParticleBase*>  ListBaseTracks;
      ListBaseTracks.clear();
      for(i=0;i<nth;i++) {
	  j=(*WrkVrtSet)[SelectedVertex].SelTrk[i];                           /*Track number*/
          ListBaseTracks.push_back( SelectedBaseTracks[j] );
      }
      m_fitSvc->setApproximateVertex((*WrkVrtSet)[SelectedVertex].vertex[0],
	                             (*WrkVrtSet)[SelectedVertex].vertex[1],
	                             (*WrkVrtSet)[SelectedVertex].vertex[2]);
      StatusCode SC=m_fitSvc->VKalVrtFit(ListBaseTracks,
                                (*WrkVrtSet)[SelectedVertex].vertex,
                                (*WrkVrtSet)[SelectedVertex].vertexMom,
				(*WrkVrtSet)[SelectedVertex].Charge,
		                (*WrkVrtSet)[SelectedVertex].vertexCov,
				(*WrkVrtSet)[SelectedVertex].Chi2PerTrk, 
				(*WrkVrtSet)[SelectedVertex].TrkAtVrt,
				(*WrkVrtSet)[SelectedVertex].Chi2); 

      if(SC.isFailure()) ATH_MSG_DEBUG("SC in RefitVertex returned failure "); 
      ATH_MSG_VERBOSE("RefitVertex "<<SC<<", "<<ListBaseTracks.size()<<","<<(*WrkVrtSet)[SelectedVertex].Chi2PerTrk.size());

      return SC;
   }

// Fills 5x5 matrix.  Input Matrix is a full covariance
   void  VrtSecInclusive::FillCovMatrix(int iTrk, std::vector<double> & Matrix, AmgSymMatrix(5)& CovMtx )
   {
     int iTmp=(iTrk+1)*3;
     int NContent = Matrix.size();
     CovMtx(0,0)=0.; CovMtx(1,1)=0.;
     int pnt = (iTmp+1)*iTmp/2 + iTmp;   if( pnt   > NContent ) return;
     CovMtx(2,2) =  Matrix[pnt];
     pnt = (iTmp+1+1)*(iTmp+1)/2 + iTmp; if( pnt+1 > NContent ) return;
     CovMtx(2,3) =  CovMtx(3,2) =Matrix[pnt];
     CovMtx(3,3) =  Matrix[pnt+1];
     pnt = (iTmp+2+1)*(iTmp+2)/2 + iTmp; if( pnt+2 > NContent ) return; 
     CovMtx(2,4) = CovMtx(4,2) =Matrix[pnt];
     CovMtx(3,4) = CovMtx(4,3) =Matrix[pnt+1];
     CovMtx(4,4)               =Matrix[pnt+2];
     return;
   }

  ///////////////////////////////
  // get truth info
  ///////////////////////////////
  StatusCode VrtSecInclusive::getNewTruthInfo(vector<const Trk::TrackParticleBase*> & SelectedBaseTracks){


    ATH_MSG_DEBUG("VrtSecInclusive getNewTruthInfo()");

    /** Use McEventCollection container - already retrieved earlier */

    if(!m_importedFullTruthColl) {
      ATH_MSG_WARNING("No McEventCollection container in TDS");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("MCEventCollection Container size = "<<m_importedFullTruthColl->size());
    //
    // event primary vertex
    const Trk::VxCandidate* evPV=0;  
    if(m_vertexTES) {
      evPV = *(m_vertexTES->begin());
      ATH_MSG_DEBUG("In truth routine: PV x/y/z "<<evPV->recVertex().position().x()<<","<<evPV->recVertex().position().y()<<","<<evPV->recVertex().position().z());
    }
    else{
      ATH_MSG_INFO("what happened to m_vertexTES? "<<m_vertexTES);
    }
    //
    /** loop over particles produced at the hard scatter */

    const HepMC::GenEvent* genEvt1 = *(m_importedFullTruthColl->begin());

    HepMC::GenEvent::particle_const_iterator partPtr = genEvt1->particles_begin();
    HepMC::GenEvent::particle_const_iterator partEnd = genEvt1->particles_end();

    // get some info about incident pion and where it interacts
    //int incPi_endVtx_bc=1;
    HepMC::FourVector IncPiPos;
    HepMC::FourVector PartProdVtx;
    HepMC::FourVector PartEndVtx;
    //double trklen;
    //double trkrad;
    //double trkz;
    //double trkendR, trkendZ;
    //
    const Trk::TrackParameters* generatedTrackParam=0;

    // this loop is over all particles. turn it off as it is not needed for most studies
    /*
    for (; partPtr != partEnd; ++partPtr) {
    //    for (; mcpItr != mcpItrE; ++mcpItr) {

      // const HepMC::GenParticle* part =(*mcpItr)->genParticle();
      //
      const HepMC::GenParticle* part =(*partPtr);
      int pdgid = part->pdg_id(); int absid = abs(pdgid);

      HepMC::GenVertex* prod_vtx = part->production_vertex();
      int prod_vtx_barcode = 1; if(prod_vtx) prod_vtx_barcode = prod_vtx->barcode();
      //
      HepMC::GenVertex* end_vtx = part->end_vertex();
      int end_vtx_barcode = 1; if(end_vtx) end_vtx_barcode = end_vtx->barcode();

      // make trk pt/eta/phi from px/py/pz
      HepMC::FourVector pVec = part->momentum();

      double tanth = pVec.perp()/pVec.pz();
      double theta = atan(tanth);
      theta += m_PI; // atan returns between -PI/2 and PI/2, whereas we want it between 0 and PI
      if(theta<0) theta += m_PI;
      if(theta>m_PI) theta -= m_PI;
      double peta = -1*log(tan(theta/2.));
      double pphi = atan2(pVec.y(),pVec.x());

      // keep track of whether this track can be reconstructed, and if so, has it been
      // 
      //
      //
      bool isTrack = (absid==211 || absid==321 || absid==2212 || absid==11 || absid==13); 
      isTrack = isTrack || absid==2112 || absid == 130; // keep track of neutrons/KL
      isTrack = isTrack || absid==1000010020; //a deuteron is 1000010020
      isTrack = isTrack || (absid==1000022 && part->status()==2); // neutralino for SUSY
      if(absid==1000022) ATH_MSG_DEBUG(" neutralino status/pt "<<part->status() <<","<<pVec.perp());


      if(isTrack && pVec.perp()>200 && fabs(peta)<2.5){
      //      if(isTrack && pVec.perp()>200 && fabs(peta)<5.0){ // for a test loosen eta cut
	trklen = -10;
        trkrad = -10;
        trkz = 10000;
        trkendR=-1;
        trkendZ=10000;
        if(prod_vtx) {
          PartProdVtx = prod_vtx->position();
          trkrad = PartProdVtx.perp();
	  trkz = PartProdVtx.z();
           if(end_vtx) {
             PartEndVtx = end_vtx->position();
	     HepMC::ThreeVector len(PartProdVtx.x()-PartEndVtx.x(),PartProdVtx.y()-PartEndVtx.y(),PartProdVtx.z()-PartEndVtx.z());
             trklen = len.r();
             trkendR = PartEndVtx.perp();
             trkendZ = PartEndVtx.z();
	   }
	}
	//
	
        m_TruthTrk_R->push_back(trkrad);
        m_TruthTrk_Z->push_back(trkz);
        m_TruthTrk_L->push_back(trklen);
        m_TruthTrk_barcode->push_back(part->barcode());
        m_TruthTrk_pid->push_back(pdgid);
	// primary particle did interact
	m_TruthTrk_EndR->push_back(trkendR);
	m_TruthTrk_EndZ->push_back(trkendZ);
	//
        // generate parameters of genParticle relative to (0,0,0)
	//
        if(m_extrapPV) generatedTrackParam = m_truthToTrack->makeProdVertexParameters(part);
        else generatedTrackParam = m_truthToTrack->makePerigeeParameters(part);
	//
        double tempd0=-10000.;
        double tempz0=-10000.;
        double tempphi=-10000;
        double tempp=-10000.;
        double temptheta=-10000;
        double temppt=-10000;
        double tempeta=-10000;
        if(generatedTrackParam) {
          tempd0 = generatedTrackParam->parameters()[Trk::d0];
          tempz0 = generatedTrackParam->parameters()[Trk::z0];
          tempphi  = generatedTrackParam->parameters()[Trk::phi];
          tempp   = generatedTrackParam->parameters()[Trk::qOverP];
          temptheta  = generatedTrackParam->parameters()[Trk::theta];
	  //
          temppt = (1./tempp)*sin(temptheta);
          tempeta = -1.*log( tan(temptheta/2));
        }
        m_TruthTrk_2dIP->push_back(tempd0);
        m_TruthTrk_ZIP->push_back(tempz0);
        m_TruthTrk_pt->push_back(temppt);
        m_TruthTrk_eta->push_back(tempeta);
        m_TruthTrk_phi->push_back(tempphi);
        ATH_MSG_DEBUG("Truth Perigee: pt/d0/z0/R/Z "<<temppt<<","<<tempd0<<","<<tempz0<<","<<trkrad<<","<<trkz);
	//
	//
        double matchPr=0;
        for (unsigned int ilist = 0; ilist<(m_RecoTrk_barcode->size()); ++ilist) {
	  if( m_RecoTrk_barcode->at(ilist) == (part->barcode())) {
	    matchPr = m_RecoTrk_matchPr->at(ilist);
	  }
        }
        m_TruthTrk_matchPr->push_back(matchPr);  
        ATH_MSG_DEBUG("Truth barcode/pid/pt/eta/phi/rad/Z/len/match "<<part->barcode()<<","<<pdgid<<","<<pVec.perp()<<","<<peta<<","<<pphi<<","<<trkrad<<","<<trkz<<","<<trklen<<","<<matchPr);
      } // if track is a stable kind

      // this check is for particles created at the primary vertex

      // I don't think the next code snippet is used for anything. 
      // It was relevant for when I was looking at single pion MC.
      // Leave in for now - VJ, Feb 11, 2010
      if(prod_vtx_barcode == -1){

        m_truthEta = peta; m_truthPhi = pphi; m_truthPt = pVec.perp();

        if(end_vtx) { // when a primary particle interacts

          m_truthInt=1;
          IncPiPos= end_vtx->position();
	  m_truthSV1X = IncPiPos.x(); m_truthSV1Y = IncPiPos.y(); m_truthSV1Z = IncPiPos.z(); 
          incPi_endVtx_bc = end_vtx->barcode();

	}
        else {m_truthInt=0;}
      } // if it came from the prod_vertex
    }
    */
    //// end of obsolete block of code
    //
    // for the rest try looping over vertices rather than tracks.
    
    StatusCode sc;    
    HepMC::GenEvent::vertex_const_iterator vtxPtr = genEvt1->vertices_begin();
    HepMC::GenEvent::vertex_const_iterator vtxEnd = genEvt1->vertices_end();

    bool FidCut=false; // vertex fiducial cut
    int vtxHasLifetime=0;
    int strangeBaryon=0;
    int IncomingPID=0;
    double pTincoming=0; // flags for the incident particle that makes the vertex
    double pZincoming=0;
    HepMC::FourVector PrimVrt;
    // since tracks have pT resoltion, allow for tracks to have somewhat lower momentum
    // set by m_mcTrkResolution
    double momLowerCut = m_TrkPtCut*(1-m_mcTrkResolution); // the 500 needs to be tied to reconstruction

    for (; vtxPtr != vtxEnd; ++vtxPtr) {


      if((*vtxPtr)->barcode() == -1) {
	PrimVrt = (*vtxPtr)->position();
        continue; // skip if this was the primary vertex
      }

      HepMC::FourVector Vpos = (*vtxPtr)->position();
      HepMC::ThreeVector Dist(PrimVrt.x()-Vpos.x(),PrimVrt.y()-Vpos.y(),PrimVrt.z()-Vpos.z());
      if(Dist.r()<0.01) continue; // skip if this vertex is at the PV

      // if in the future, I require a cut on the SiHits or BLayerHits
      // I will have to change the logic
      //
      bool pixFid = (Vpos.perp()<110. && fabs(Vpos.z())<620.); // vtx within pixels
      bool sctFid = (Vpos.perp()<500. && fabs(Vpos.z())<2500.); // vtx within SCT
      if(m_CutPixelHits>0) FidCut = pixFid; 
      if(m_CutSctHits>0) FidCut = sctFid;

      // now get barcodes of incident particle and outgoing particles
      //

      if(!FidCut) continue; //skip if vertex is out of the pixel/sct fiducial region

      ATH_MSG_DEBUG(" barcode of vert/#partIn/Out "<<(*vtxPtr)->barcode()<<","<<(*vtxPtr)->particles_in_size()<<","<<(*vtxPtr)->particles_out_size());
      ATH_MSG_DEBUG(" XYZ of this vert/FidCut/Dist "<<Vpos.x()<<","<<Vpos.y()<<","<<Vpos.z()<<","<<FidCut<<","<<Dist.r());

      // tag this SV if the incoming particle has lifetime
      // check for Ks, KL, strange baryons, charm meson/baryon, B meson/baryon

      HepMC::GenVertex::particles_in_const_iterator inP = (*vtxPtr)->particles_in_const_begin();
      HepMC::GenVertex::particles_in_const_iterator inPend = (*vtxPtr)->particles_in_const_end();
      vtxHasLifetime=0;
      strangeBaryon=0;
      for(; inP!=inPend; ++inP) {        
	int pid = (*inP)->pdg_id();
        int abspid = abs(pid);
        IncomingPID = pid;
        if(abspid==22) vtxHasLifetime=1; // photon
        if(abspid==211) vtxHasLifetime=2; // pi+-
        if(abspid==2212) vtxHasLifetime=12; // proton
        if(abspid==15) vtxHasLifetime=16; // tau
        if(abspid==2112) vtxHasLifetime=17; // (anti-)neutron
        if(abspid==310) vtxHasLifetime=3; //Ks
        if(abspid==130) vtxHasLifetime=-3; // K-long
        if(abspid/100 ==3 && (abspid!=310 && abspid!= 130)){
	   if(abspid==321) vtxHasLifetime=-13; // K+-
           else vtxHasLifetime=-16; // other strange mesons
	}
        if(abspid/1000==3) vtxHasLifetime=13; //Strange baryons
        if(abspid/100==4) vtxHasLifetime=4; // charm mesons & baryons
        if(abspid/1000==4) vtxHasLifetime=14;
        if(abspid/100==5) vtxHasLifetime=5; // b mesons & baryons
        if(abspid/1000==5) vtxHasLifetime=15;
        if(vtxHasLifetime==13) strangeBaryon=pid;
        if(abspid==1000022) vtxHasLifetime=19; // neutralino

        HepMC::FourVector iVec = (*inP)->momentum();

        pZincoming = iVec.pz();
        pTincoming = iVec.perp();
	//
        ATH_MSG_VERBOSE(" barcode/pid/pt/pz of incoming part/lifetime flag "<<(*inP)->barcode()<<","<<pid<<" ---- "<<vtxHasLifetime<<","<<pTincoming<<","<<pZincoming);
      }

      // now get particles emerging from this vertex
      HepMC::GenVertex::particles_out_const_iterator outP = (*vtxPtr)->particles_out_const_begin();
      HepMC::GenVertex::particles_out_const_iterator outPend = (*vtxPtr)->particles_out_const_end();

      int numOutP=0;
      int numOutPLoPt=0; // useless variable. will comment out later.
      double sumTrkpt=0; double sumTrkpz=0;
      int sumChg=0;
      m_maxTrks=10;
      double Trkpt[10]; // array size is same as m_maxTrks
      double Trkpx[10], Trkpy[10], Trkpz[10];
      int numReco=0;
      int bcodeOfRecoTrks[10];
      for (int i=0; i<m_maxTrks; ++i) {
       Trkpt[i]=0; Trkpx[i]=0; Trkpy[i]=0; Trkpz[i]=0;
       bcodeOfRecoTrks[i]=-1;
      }
      //
      // loop over outgoing particles
      //
      for(; outP!=outPend; ++outP) {

        // look for pions/kaons/protons/eletrons/muons with a long track length

        int id = (*outP)->pdg_id();
	int absid = abs(id);

	// only continue for charged stable particles
        bool isTrack = (absid==211 || absid==321 || absid==2212 || absid==11 || absid==13);
        isTrack = isTrack || absid == 1000010020; // add deuteron
        if(!isTrack) continue;
	//
        ATH_MSG_VERBOSE("out track pid "<<absid);

        HepMC::GenVertex* endv = (*outP)->end_vertex();
        //int endv_barcode = 1;
        double length=m_TruthTrkLen;
        if(endv) {
          //endv_barcode = endv->barcode();

	  HepMC::FourVector end = endv->position();
	  HepMC::ThreeVector len(Vpos.x()-end.x(),Vpos.y()-end.y(),Vpos.z()-end.z());

	  length = len.r();

        } //end vtx

        HepMC::FourVector pVec = (*outP)->momentum();


        double tanth = pVec.perp()/pVec.pz();
        double theta = atan(tanth);
        theta += m_PI; // atan returns between -PI/2 and PI/2, whereas we want it between 0 and PI
        if(theta<0) theta += m_PI;
        if(theta>m_PI) theta -= m_PI;
        double peta = -1*log(tan(theta/2.));
	//
	// look for matching reco trk
	//
        
        for (unsigned int ilist = 0; ilist<(m_RecoTrk_barcode->size()); ++ilist) {

	  ATH_MSG_VERBOSE("ilist/m_RecoTrk/Truth_bcode "<<ilist<<","<<m_RecoTrk_barcode->at(ilist)<<","<<(*outP)->barcode());

	  if( m_RecoTrk_barcode->at(ilist) == ((*outP)->barcode()) ) {
            numReco++;
            if(numReco<10) bcodeOfRecoTrks[numReco-1]=m_RecoTrk_barcode->at(ilist);
            ATH_MSG_VERBOSE("---- numReco/bcodeOf/ilist/m_RecoTrk "<<numReco<<","<<bcodeOfRecoTrks[numReco-1]<<","<<ilist<<","<<m_RecoTrk_barcode->at(ilist));
            break;
	  }

	}
	//
	// generate parameters of genParticle relative to (0,0,0)
        bool genPerigee=false;
        if(m_extrapPV) generatedTrackParam = m_truthToTrack->makeProdVertexParameters(*outP);
        else generatedTrackParam = m_truthToTrack->makePerigeeParameters(*outP);
	//
	double tempd0 = -9999.;	double tempz0 = -9999.;
	double tempp=-10000.;  double tempphi=-10000;
	double temptheta=-10000; double temppt=-10000;
	double tempeta=-10000;
        double temppz=-10000;
	//
	if(generatedTrackParam) {
          genPerigee=true;
	  tempd0 = generatedTrackParam->parameters()[Trk::d0];
	  tempz0 = generatedTrackParam->parameters()[Trk::z0];
	  tempphi  = generatedTrackParam->parameters()[Trk::phi];
	  tempp   = generatedTrackParam->parameters()[Trk::qOverP];
	  temptheta  = generatedTrackParam->parameters()[Trk::theta];
	  //
          double inv=(1./tempp);
	  temppt = inv*sin(temptheta);
          temppz = sqrt(inv*inv - temppt*temppt);
	  tempeta = -1.*log( tan(temptheta/2));
	}
        double abstemppt=fabs(temppt);
        if(abstemppt/1000.>200.) ATH_MSG_INFO("pt@0 is huge, barcode/pdgid "<<abstemppt<<","<<(*outP)->barcode()<<","<<absid);
	// make cuts on tracks
	//
        bool etaFid = fabs(tempeta)<2.5;
        bool momCut = abstemppt > momLowerCut &&( abstemppt/1000<10000.); //pathalogical
	bool longTrk = length>= m_TruthTrkLen; // track must have length >= m_TruthTrkLen
	// choose good tracks
	ATH_MSG_DEBUG("rad/ZSV, pid/bc/genPerigee,pt@Vrt/pt@0/pz@0/eta@vrt/eta@0/2dIP@0/zIP@0 "<<Vpos.perp()<<","<<Vpos.z()<<","<<absid<<","<<(*outP)->barcode()<<","<<genPerigee<<","<<pVec.perp()<<","<<temppt<<","<<temppz<<","<<peta<<","<<tempeta<<","<< tempd0<<","<<tempz0);
        if(longTrk && isTrack && etaFid && momCut ) {
	  //
          if(!genPerigee) ATH_MSG_INFO("no Perigee "<<(*outP)->barcode());
	  //
  	  // add variables for each track into ntuple
	  // Add all vertices to same ntuple, and extract later in root macro 
          m_truthAllTrk2dIP->push_back(tempd0); // these five variables 
          m_truthAllTrkzIP->push_back(tempz0); // are measured 
          m_truthAllTrkPt->push_back(temppt);  // relative to 
          m_truthAllTrkEta->push_back(tempeta); 
          m_truthAllTrkPhi->push_back(tempphi); // (0,0,0)

          m_truthAllTrkR->push_back(Vpos.perp());
          m_truthAllTrkZ->push_back(Vpos.z());
          m_truthAllTrkbc->push_back( ((*outP)->barcode()) );
	  //
          numOutP++;
          sumTrkpt += abstemppt; // these two are at (0,0,0)
          sumTrkpz += temppz;
          if(numOutP<=m_maxTrks) {
	    Trkpt[numOutP-1]=abstemppt; // @(0,0,0)
            Trkpx[numOutP-1]=abstemppt*cos(tempphi); 
            Trkpy[numOutP-1]=abstemppt*sin(tempphi);
            Trkpz[numOutP-1]=temppz;
	  }
	  ATH_MSG_DEBUG("After Cuts: Out part bc/id/length/numOutP/sumTrkpz/sumTrkpt "<<(*outP)->barcode()<<","<<(*outP)->pdg_id()<<","<<length<<","<<numOutP<<","<<sumTrkpt<<","<<sumTrkpz);
	  //
          int trkchg=1;
          if(id<0) trkchg=-1; // pi-/K-/anti-p have id<0
          if(id == -11 || id == -13) trkchg=1; // however, e+ and mu+ have id<0
          sumChg += trkchg;
	}
        if(longTrk && isTrack && etaFid && abstemppt> m_LoPtCut ) numOutPLoPt++;
      } // loop over particles in vertex

      ATH_MSG_DEBUG("# outParticles passing cuts (High Pt) "<<numOutP);
      ATH_MSG_DEBUG("# outParticles passing cuts (Lo Pt) "<<numOutPLoPt);
      //
      if(numOutP>=1) m_nSVGe1HiPt++;
      int vtxType=0;
      if(numOutPLoPt == 1) vtxType=11;
      if(numOutPLoPt > 1) vtxType=12;
      if(numOutP == 1) vtxType=1;
      if(numOutP > 1) vtxType=2;
      //
      if(numOutP>= 1) { // only store vertex type 1 or 2, i.e., # HiPt tracks >=1
      // if(numOutP>= 1 || numOutPLoPt>=1) { // temporary test. by default this is >=2
       m_nSVHiPt++;
       //
       m_truthAllVtxType->push_back(vtxType);
       m_truthAllSVX->push_back(Vpos.x());
       m_truthAllSVY->push_back(Vpos.y());
       m_truthAllSVZ->push_back(Vpos.z());
       m_truthAllVtxHasLifetime->push_back(vtxHasLifetime);
       m_truthAllVtxStrangeBaryon->push_back(strangeBaryon);
       m_truthAllVtxIncomingPID->push_back(IncomingPID);
       if((*vtxPtr)->particles_in_size()==1) ATH_MSG_DEBUG("Incoming PID/Lifetime flag "<<IncomingPID<<","<<vtxHasLifetime);

       m_truthAllVtxNumIncident->push_back((*vtxPtr)->particles_in_size());
       m_truthAllVtxPtIncident->push_back(pTincoming);
       m_truthAllVtxPzIncident->push_back(pZincoming);
       // first sort the track pt		
       //sort(Trkpt,Trkpt+m_maxTrks);					  
       sort(Trkpt,Trkpt+10);					  
       ATH_MSG_DEBUG("numTrksHi/Lo, re-orderered "<<numOutP<<","<<numOutPLoPt<<","<<Trkpt[m_maxTrks-1]<<","<<Trkpt[m_maxTrks-2]<<","<<Trkpt[m_maxTrks-3]<<","<<Trkpt[m_maxTrks-4]<<","<<Trkpt[m_maxTrks-5]);
       //
       // TrkPt has been initialized to zero, 
       // so if there are only 2 tracks all other vectors will get filled with zero
       m_truthAllVtxTrk1Pt->push_back(Trkpt[m_maxTrks-1]);		  
       m_truthAllVtxTrk2Pt->push_back(Trkpt[m_maxTrks-2]);		  
       m_truthAllVtxTrk3Pt->push_back(Trkpt[m_maxTrks-3]);
       m_truthAllVtxTrk4Pt->push_back(Trkpt[m_maxTrks-4]);
       m_truthAllVtxTrk5Pt->push_back(Trkpt[m_maxTrks-5]);
       //
       m_truthAllNumTrks->push_back(numOutP);
       m_truthAllNumRecoTrks->push_back(numReco);
       m_truthAllCharge->push_back(sumChg);
       m_truthAllSumTrksPt->push_back(sumTrkpt);
       m_truthAllSumTrksPz->push_back(sumTrkpz);
       //
       // calculate opening angle between all 2-track pairs, and store the minimum
       //
       double minOpAng = 1000;
       double opAng=0;
       double p1=0;
       double p2=0;
       for (int it1=0; it1<numOutP; ++it1) {
         if(it1>m_maxTrks) continue;
         p1 = sqrt(Trkpx[it1]*Trkpx[it1] + Trkpy[it1]*Trkpy[it1] + Trkpz[it1]*Trkpz[it1]);
         for (int it2=0; it2<numOutP; ++it2) {
           if(it2>m_maxTrks) continue;
           if(it1==it2) continue;
	   //
           p2 = sqrt(Trkpx[it2]*Trkpx[it2] + Trkpy[it2]*Trkpy[it2] + Trkpz[it2]*Trkpz[it2]);
           opAng = Trkpx[it1]*Trkpx[it2]+Trkpy[it1]*Trkpy[it2]+Trkpz[it1]*Trkpz[it2];
           opAng = (opAng/p1)/p2;
	   ATH_MSG_DEBUG("Opening Ang/ minOpAng "<<opAng<<","<<minOpAng);
           if(opAng < minOpAng) minOpAng = opAng;
	 }
       }
       m_truthAllMinOpAng->push_back(minOpAng);
       //
       // if there are >=2 matched reco trks at vertex, do a quick fit
       // 
       ATH_MSG_DEBUG("numReco "<<numReco);
       ATH_MSG_DEBUG("Selected trk size "<<SelectedBaseTracks.size());
       ATH_MSG_DEBUG("size of SelTrk_barcode "<<m_SelTrk_barcode->size());
       //
       int refitS =0;
       int numretr=0;
       double rex=0;
       double rey=0;
       double rez=0;
       double rec=0;
       double remass=0;
       //
       if(numReco>=2 && numReco<10 && SelectedBaseTracks.size()>=2) {

        vector<const Trk::TrackParticleBase*>  ListMatBaseTracks;
        ListMatBaseTracks.clear();
        WrkVrt newvrt;
	// 
	// first fill vector with selected tracks.
        for (int itmp=0; itmp<numReco; ++itmp) {

	  for(unsigned int isel=0; isel<SelectedBaseTracks.size(); ++isel) {

            ATH_MSG_DEBUG("itmp/bcode of RecoTrks/SelTrk "<<itmp<<","<<bcodeOfRecoTrks[itmp]<<","<<isel<<","<<m_SelTrk_barcode->at(isel));

            // check if Selected reco trk passes all Si cuts
	    //

	    if(bcodeOfRecoTrks[itmp]==m_SelTrk_barcode->at(isel)) {

	      ListMatBaseTracks.push_back(SelectedBaseTracks[isel]);
	      //
	    }

	  } // loop over selected tracks

	} // numReco loop
	//
	// check size of selected reco tracks
        numretr = ListMatBaseTracks.size();
        if((int) ListMatBaseTracks.size() <2 ) {
          ATH_MSG_DEBUG("mismatch numreco & re-fit size "<<numReco<<","<<ListMatBaseTracks.size());}
        else{
          ATH_MSG_DEBUG("numreco & re-fit size "<<numReco<<","<<ListMatBaseTracks.size());

	  m_fitSvc->setApproximateVertex(Vpos.x(),Vpos.y(),Vpos.z());
	  sc=m_fitSvc->VKalVrtFit(ListMatBaseTracks,newvrt.vertex,newvrt.vertexMom,
				       newvrt.Charge,newvrt.vertexCov,newvrt.Chi2PerTrk, 
				       newvrt.TrkAtVrt,newvrt.Chi2);
	  //
	  // check if refit is success
	  //
          if(!sc) {refitS=-1; ATH_MSG_INFO("Refit of match Selected tracks failed ");}
	  //
          else {
            refitS=1;
            ATH_MSG_INFO("Refit of match Selected tracks successful ");
            ATH_MSG_INFO(" MC vertex "<<Vpos.x()<<","<<Vpos.y()<<","<<Vpos.z());
	    //
            rex=newvrt.vertex[0]; rey=newvrt.vertex[1]; rez=newvrt.vertex[2]; rec=newvrt.Chi2;
	    // 
	    // now loop over tracks at vertex and get mass
            double vx=0; double vy=0; double vz=0; double ve=0;
            for(unsigned int itatv=0; itatv<ListMatBaseTracks.size(); ++itatv) {
	      double theta = newvrt.TrkAtVrt[itatv][1];
	      double trk_p = fabs(1./ newvrt.TrkAtVrt[itatv][2]);
	      double trk_pt = trk_p*sin(theta);  
	      double trk_px = trk_pt*cos(newvrt.TrkAtVrt[itatv][0]);  
	      double trk_py = trk_pt*sin(newvrt.TrkAtVrt[itatv][0]);  
	      double trk_pz = trk_p*cos(theta);  
	      double trkp2 = trk_p*trk_p;
	      double trk_e = sqrt(trkp2 + m_pi*m_pi); 
	      //
	      vx += trk_px;
	      vy += trk_py;
	      vz += trk_pz;
	      ve += trk_e; 
	    }
	    //
            remass = sqrt(ve*ve - vx*vx -vy*vy -vz*vz);

            ATH_MSG_INFO(" Refit vertex "<<newvrt.vertex[0]<<","<<newvrt.vertex[1]<<","<<newvrt.vertex[2]<<","<<rec<<","<<remass);
            
	  }
	} // successful re-fit

       } // if > 2 tracks with tigher cuts
       m_truthAllRefitStatus->push_back(refitS);
       m_truthAllNumReTrk->push_back(numretr);
       m_truthAllRefitSVX->push_back(rex);
       m_truthAllRefitSVY->push_back(rey);
       m_truthAllRefitSVZ->push_back(rez);
       m_truthAllRefitChi2->push_back(rec);
       m_truthAllRefitMass->push_back(remass);

      if(numOutPLoPt>= 2) m_nSVLoPt++;

      } // loop over vertices
    }
    ATH_MSG_DEBUG("# SV with >= 1 Hi Pt outTrks passing cuts "<<m_nSVHiPt);
    ATH_MSG_DEBUG("# SV with >= 1 Lo Pt outTrks passing cuts "<<m_nSVLoPt);

    // delete object
    if(generatedTrackParam!=0) delete generatedTrackParam;
    
    if(sc.isFailure()) ATH_MSG_INFO("some status code failed. check. probably not serious ");
    return StatusCode::SUCCESS;
  }
  ////////////////////////////////////////////////////////////////////
  HepMC::GenParticle* VrtSecInclusive::getTrkGenParticle(const Trk::TrackParticleBase *trk, double& matchProb){

    const HepMC::GenEvent* theGenEvent = *(m_importedFullTruthColl->begin());
    long theBarcode;
    HepMC::GenParticle* theGenParticle=NULL;

    ElementLink< Rec::TrackParticleContainer > myLink;
    TrackParticleTruthCollection::const_iterator TPTItr = m_importedTrkTruthColl->end();
    const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(trk);
    if (tp) {
        const Rec::TrackParticleContainer* tpCont = dynamic_cast<const Rec::TrackParticleContainer*>(m_importedTrkColl);
        if (tpCont) {
            myLink.setElement(tp);
            myLink.setStorableObject(*tpCont);
            TPTItr = m_importedTrkTruthColl->find(Rec::TrackParticleTruthKey(myLink));
        }
    }

    if (TPTItr != m_importedTrkTruthColl->end()) {
      const HepMcParticleLink& tempHepMCLink = (*TPTItr).second.particleLink();
      if(!tempHepMCLink) {
          //std::cout << '\t' << "Track with no truth association found" << std::endl;
          return(theGenParticle);
      }
      TrackParticleTruth trtruth= (*TPTItr).second;
      matchProb = trtruth.probability();
      theBarcode = tempHepMCLink->barcode();
      theGenParticle = theGenEvent->barcode_to_particle(theBarcode);
    }

    return(theGenParticle);
    }
  //////////////////////////////

  void VrtSecInclusive::setMcEventCollection(const McEventCollection* mcEventCollectionIn) {        
	m_importedFullTruthColl = mcEventCollectionIn;
  }

  ///////////////////////////////////

  void VrtSecInclusive::setTrackParticleTruthCollection(const TrackParticleTruthCollection* trackParticleTruthCollectionIn) {
	m_importedTrkTruthColl = trackParticleTruthCollectionIn;
  }
  /////////////////////////
  void VrtSecInclusive::setTrackParticleContainer(const Trk::TrackParticleBaseCollection* importedTrackCollectionIn){ 
	m_importedTrkColl = importedTrackCollectionIn;
  }
  ////////////////////////////////
  ///
  // VJ Mar 4, 2010
  // the next method is obsolete. 
  // It uses SpclMC, whereas getNewTruth uses McEventCollection
  // I am keeping it for posterity
StatusCode VrtSecInclusive::getTruthInfo(vector<const Trk::TrackParticleBase*> & SelectedBaseTracks){

    MsgStream mlog( messageService(), name() );

    mlog << MSG::DEBUG << "VrtSecInclusive getTruthInfo()" << endreq;

    /** get the MC truth particle AOD container from StoreGate */

    const TruthParticleContainer*  mcpartTES = 0;
    //    StatusCode sc=m_sgSvc->retrieve( mcpartTES, m_truthParticleContainerName);
    StatusCode sc=evtStore()->retrieve( mcpartTES, m_truthParticleContainerName);
    if( sc.isFailure()  ||  !mcpartTES ) {
      mlog << MSG::WARNING
	  << "No AOD MC truth particle container found in TDS"
	  << endreq; 
      return StatusCode::SUCCESS;
    }
    mlog <<MSG::DEBUG << "MC Truth Container Successfully Retrieved" << endreq;
    //
    /** loop over particles and get the top quarks produced at the hard scatter */

    mlog<<MSG::DEBUG<<"truth particle Container size = "<<mcpartTES->size()<<endreq;

    const HepMC::GenEvent* genEvt = mcpartTES->genEvent();

    // get some info about incident pion and where it interacts
    //int incPi_endVtx_bc=1;
    HepMC::FourVector IncPiPos;
    HepMC::FourVector PartProdVtx;
    HepMC::FourVector PartEndVtx;
    //double trklen;
    //double trkrad;
    //double trkz;
    //double trkendR, trkendZ;
    //
    //    StatusCode sc;    
    /*
    TruthParticleContainer::const_iterator mcpItr  = mcpartTES->begin();
    TruthParticleContainer::const_iterator mcpItrE = mcpartTES->end();
    for (; mcpItr != mcpItrE; ++mcpItr) {

      const HepMC::GenParticle* part =(*mcpItr)->genParticle();
      int pdgid = part->pdg_id(); int absid = abs(pdgid);

      HepMC::GenVertex* prod_vtx = part->production_vertex();
      int prod_vtx_barcode = 1; if(prod_vtx) prod_vtx_barcode = prod_vtx->barcode();
      //
      HepMC::GenVertex* end_vtx = part->end_vertex();
      int end_vtx_barcode = 1; if(end_vtx) end_vtx_barcode = end_vtx->barcode();

      // keep track of whether this track can be reconstructed, and if so, has it been
      // 
      //
      bool isTrack = (absid==211 || absid==321 || absid==2212 || absid==11 || absid==13 || absid==130 || absid==2112); 
      isTrack = isTrack || absid==1000010020; //a deuteron is 1000010020
      if(isTrack && (*mcpItr)->pt()>200 && fabs((*mcpItr)->eta())<2.5){
	trklen = -10;
        trkrad = -10;
        trkz = 10000;
        trkendR=-1;
        trkendZ=10000;
        if(prod_vtx) {
          PartProdVtx = prod_vtx->position();
          trkrad = PartProdVtx.perp();
	  trkz = PartProdVtx.z();
           if(end_vtx) {
             PartEndVtx = end_vtx->position();
	     HepMC::ThreeVector len(PartProdVtx.x()-PartEndVtx.x(),PartProdVtx.y()-PartEndVtx.y(),PartProdVtx.z()-PartEndVtx.z());
             trklen = len.r();
             trkendR = PartEndVtx.perp();
             trkendZ = PartEndVtx.z();
	   }
	}
	//
        m_TruthTrk_R->push_back(trkrad);
        m_TruthTrk_Z->push_back(trkz);
        m_TruthTrk_L->push_back(trklen);
        m_TruthTrk_barcode->push_back(part->barcode());
        m_TruthTrk_pid->push_back(pdgid);
	// primary particle did interact
	m_TruthTrk_EndR->push_back(trkendR);
	m_TruthTrk_EndZ->push_back(trkendZ);
	//
        // generate parameters of genParticle relative to (0,0,0)
	//
        const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(part);
        double tempd0=-10000.;
        double tempz0=-10000.;
        double tempphi=-10000;
        double tempp=-10000.;
        double temptheta=-10000;
        double temppt=-10000;
        double tempeta=-10000;
        if(generatedTrackPerigee) {
          tempd0 = generatedTrackPerigee->parameters()[Trk::d0];
          tempz0 = generatedTrackPerigee->parameters()[Trk::z0];
          tempphi  = generatedTrackPerigee->parameters()[Trk::phi];
          tempp   = generatedTrackPerigee->parameters()[Trk::qOverP];
          temptheta  = generatedTrackPerigee->parameters()[Trk::theta];
	  //
          temppt = (1./tempp)*sin(temptheta);
          tempeta = -1.*log( tan(temptheta/2));
        }
        m_TruthTrk_2dIP->push_back(tempd0);
        m_TruthTrk_ZIP->push_back(tempz0);
        m_TruthTrk_pt->push_back(temppt);
        m_TruthTrk_eta->push_back(tempeta);
        m_TruthTrk_phi->push_back(tempphi);
        
	//
        double matchPr=0;
        for (unsigned int ilist = 0; ilist<(m_RecoTrk_barcode->size()); ++ilist) {
	  if( m_RecoTrk_barcode->at(ilist) == (part->barcode())) {
	    matchPr = m_RecoTrk_matchPr->at(ilist);
	  }
        }
        m_TruthTrk_matchPr->push_back(matchPr);  
        // mlog<<MSG::INFO<<"barcode/pid/pt/eta/phi/rad/Z/len/match "<<part->barcode()<<","<<pdgid<<","<<(*mcpItr)->pt()<<","<<(*mcpItr)->eta()<<","<<(*mcpItr)->phi()<<","<<trkrad<<","<<trkz<<","<<trklen<<","<<match<<endreq;
      } // if track is a stable kind

      // this check is for particles created at the primary vertex

      // I don't think the next code snippet is used for anything. 
      // It was relevant for when I was looking at single pion MC.
      // Leave in for now - VJ, Feb 11, 2010
      if(prod_vtx_barcode == -1){

        m_truthEta = (*mcpItr)->eta(); m_truthPhi = (*mcpItr)->phi(); m_truthPt = (*mcpItr)->pt();

        if(end_vtx) { // when a primary particle interacts

          m_truthInt=1;
          IncPiPos= end_vtx->position();
	  m_truthSV1X = IncPiPos.x(); m_truthSV1Y = IncPiPos.y(); m_truthSV1Z = IncPiPos.z(); 
          incPi_endVtx_bc = end_vtx->barcode();

	}
        else {m_truthInt=0;}
      } // if it came from the prod_vertex
    }
    */
    // for the rest try looping over vertices rather than tracks.
    

    HepMC::GenEvent::vertex_const_iterator vtxPtr = genEvt->vertices_begin();
    HepMC::GenEvent::vertex_const_iterator vtxEnd = genEvt->vertices_end();

    bool FidCut=false; // vertex fiducial cut
    int vtxHasLifetime=0;
    int strangeBaryon=0;
    int IncomingPID=0;
    double pTincoming=0; // flags for the incident particle that makes the vertex
    double pZincoming=0;
    HepMC::FourVector PrimVrt;
    // since tracks have pT resoltion, allow for tracks to have somewhat lower momentum
    // set by m_mcTrkResolution
    double momLowerCut = m_TrkPtCut*(1-m_mcTrkResolution); // the 500 needs to be tied to reconstruction

    for (; vtxPtr != vtxEnd; ++vtxPtr) {


      if((*vtxPtr)->barcode() == -1) {
	PrimVrt = (*vtxPtr)->position();
        continue; // skip if this was the primary vertex
      }

      HepMC::FourVector Vpos = (*vtxPtr)->position();
      HepMC::ThreeVector Dist(PrimVrt.x()-Vpos.x(),PrimVrt.y()-Vpos.y(),PrimVrt.z()-Vpos.z());
      if(Dist.r()<0.01) continue; // skip if this vertex is at the PV

      // if in the future, I require a cut on the SiHits or BLayerHits
      // I will have to change the logic
      //
      bool pixFid = (Vpos.perp()<110. && fabs(Vpos.z())<620.); // vtx within pixels
      bool sctFid = (Vpos.perp()<500. && fabs(Vpos.z())<2500.); // vtx within SCT
      if(m_CutPixelHits>0) FidCut = pixFid; 
      if(m_CutSctHits>0) FidCut = sctFid;

      // now get barcodes of incident particle and outgoing particles
      //

      if(!FidCut) continue; //skip if vertex is out of the pixel/sct fiducial region

      mlog<<MSG::DEBUG<<" "<<endreq;
      mlog<<MSG::DEBUG<<" barcode of vert/#partIn/Out "<<(*vtxPtr)->barcode()<<","<<(*vtxPtr)->particles_in_size()<<","<<(*vtxPtr)->particles_out_size()<<endreq;
      mlog<<MSG::DEBUG<<" XYZ of this vert/FidCut/Dist "<<Vpos.x()<<","<<Vpos.y()<<","<<Vpos.z()<<","<<FidCut<<","<<Dist.r()<<endreq;

      // tag this SV if the incoming particle has lifetime
      // check for Ks, KL, strange baryons, charm meson/baryon, B meson/baryon

      HepMC::GenVertex::particles_in_const_iterator inP = (*vtxPtr)->particles_in_const_begin();
      HepMC::GenVertex::particles_in_const_iterator inPend = (*vtxPtr)->particles_in_const_end();
      vtxHasLifetime=0;
      strangeBaryon=0;
      for(; inP!=inPend; ++inP) {        
	int pid = (*inP)->pdg_id();
        int abspid = abs(pid);
        IncomingPID = pid;
        if(abspid==22) vtxHasLifetime=1; // photon
        if(abspid==211) vtxHasLifetime=2; // pi+-
        if(abspid==2212) vtxHasLifetime=12; // proton
        if(abspid==310) vtxHasLifetime=3; //Ks
        if(abspid==130) vtxHasLifetime=-3; // K-long
        if((abspid!=310 && abspid!= 130) && abspid/100==3) vtxHasLifetime=-13; // non-K0 strange meson
        if(abspid/1000==3) vtxHasLifetime=13; //Strange baryons
        if(abspid/100==4) vtxHasLifetime=4; // charm mesons & baryons
        if(abspid/1000==4) vtxHasLifetime=14;
        if(abspid/100==5) vtxHasLifetime=5; // b mesons & baryons
        if(abspid/1000==5) vtxHasLifetime=15;
        if(vtxHasLifetime==13) strangeBaryon=pid;

        HepMC::FourVector iVec = (*inP)->momentum();

        pZincoming = iVec.pz();
        pTincoming = iVec.perp();
	//
        mlog<<MSG::VERBOSE<<" barcode/pid/pt/pz of incoming part/lifetime flag "<<(*inP)->barcode()<<","<<pid<<" ---- "<<vtxHasLifetime<<","<<pTincoming<<","<<pZincoming<<endreq;
      }

      // now get particles emerging from this vertex
      HepMC::GenVertex::particles_out_const_iterator outP = (*vtxPtr)->particles_out_const_begin();
      HepMC::GenVertex::particles_out_const_iterator outPend = (*vtxPtr)->particles_out_const_end();

      int numOutP=0;
      int numOutPLoPt=0;
      double sumTrkpt=0; double sumTrkpz=0;
      int sumChg=0;
      m_maxTrks=10;
      double Trkpt[10]; // array size is same as m_maxTrks
      double Trkpx[10], Trkpy[10], Trkpz[10];
      int numReco=0;
      int bcodeOfRecoTrks[10];
      for (int i=0; i<m_maxTrks; ++i) {
       Trkpt[i]=0; Trkpx[i]=0; Trkpy[i]=0; Trkpz[i]=0;
       bcodeOfRecoTrks[i]=-1;
      }
      //
      // loop over outgoing particles
      //
      for(; outP!=outPend; ++outP) {

        // look for pions/kaons/protons/eletrons/muons with a long track length

        int id = (*outP)->pdg_id();
	int absid = abs(id);

	// only continue for charged stable particles
        bool isTrack = (absid==211 || absid==321 || absid==2212 || absid==11 || absid==13);
        isTrack = isTrack || absid == 1000010020; // add deuteron
        if(!isTrack) continue;
	//
        mlog<<MSG::VERBOSE<<"out track pid "<<absid<<endreq;

        HepMC::GenVertex* endv = (*outP)->end_vertex();
        //int endv_barcode = 1;
        double length=m_TruthTrkLen;
        if(endv) {
          //endv_barcode = endv->barcode();

	  HepMC::FourVector end = endv->position();
	  HepMC::ThreeVector len(Vpos.x()-end.x(),Vpos.y()-end.y(),Vpos.z()-end.z());

	  length = len.r();

        } //end vtx

        HepMC::FourVector pVec = (*outP)->momentum();


        double tanth = pVec.perp()/pVec.pz();
        double theta = atan(tanth);
        theta += m_PI; // atan returns between -PI/2 and PI/2, whereas we want it between 0 and PI
        if(theta<0) theta += m_PI;
        if(theta>m_PI) theta -= m_PI;
        double peta = -1*log(tan(theta/2.));
	//
	// look for matching reco trk
	//
        
        for (unsigned int ilist = 0; ilist<(m_RecoTrk_barcode->size()); ++ilist) {

	  mlog<<MSG::VERBOSE<<"ilist/m_RecoTrk/Truth_bcode "<<ilist<<","<<m_RecoTrk_barcode->at(ilist)<<","<<(*outP)->barcode()<<endreq;

	  if( m_RecoTrk_barcode->at(ilist) == ((*outP)->barcode()) ) {
            numReco++;
            if(numReco<10) bcodeOfRecoTrks[numReco-1]=m_RecoTrk_barcode->at(ilist);
            mlog<<MSG::VERBOSE<<"---- numReco/bcodeOf/ilist/m_RecoTrk "<<numReco<<","<<bcodeOfRecoTrks[numReco-1]<<","<<ilist<<","<<m_RecoTrk_barcode->at(ilist)<<endreq;
            break;
	  }

	}
	//
	// generate parameters of genParticle relative to (0,0,0)
        bool genPerigee=false;
	const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(*outP);
	double tempd0 = -9999.;	double tempz0 = -9999.;
	double tempp=-10000.;  double tempphi=-10000;
	double temptheta=-10000; double temppt=-10000;
	double tempeta=-10000;
        double temppz=-10000;
	//
	if(generatedTrackPerigee) {
          genPerigee=true;
	  tempd0 = generatedTrackPerigee->parameters()[Trk::d0];
	  tempz0 = generatedTrackPerigee->parameters()[Trk::z0];
	  tempphi  = generatedTrackPerigee->parameters()[Trk::phi];
	  tempp   = generatedTrackPerigee->parameters()[Trk::qOverP];
	  temptheta  = generatedTrackPerigee->parameters()[Trk::theta];
	  //
          double inv=(1./tempp);
	  temppt = inv*sin(temptheta);
          temppz = sqrt(inv*inv - temppt*temppt);
          mlog<<MSG::VERBOSE<<"p/pt/pz "<<inv<<","<<temppt<<","<<temppz<<endreq;
	  tempeta = -1.*log( tan(temptheta/2));
	}
        double abstemppt=fabs(temppt);
        if(abstemppt/1000.>200.) mlog<<MSG::INFO<<"pt@0 is huge, barcode/pdgid "<<abstemppt<<","<<(*outP)->barcode()<<","<<absid<<endreq;
	// make cuts on tracks
	//
        bool etaFid = fabs(tempeta)<2.5;
        bool momCut = abstemppt > momLowerCut &&( abstemppt/1000<10000.); //pathalogical
	bool longTrk = length>= m_TruthTrkLen; // track must have length >= m_TruthTrkLen
	// choose good tracks
	mlog<<MSG::DEBUG<<"rad/ZSV, pid/bc/genPerigee,pt@Vrt/pt@0/pz@0/eta@vrt/eta@0/2dIP@0/zIP@0 "<<Vpos.perp()<<","<<Vpos.z()<<","<<absid<<","<<(*outP)->barcode()<<","<<genPerigee<<","<<pVec.perp()<<","<<temppt<<","<<temppz<<","<<peta<<","<<tempeta<<","<< tempd0<<","<<tempz0<<endreq;
        if(longTrk && isTrack && etaFid && momCut ) {
	  //
          if(!genPerigee) mlog<<MSG::INFO<<"no Perigee "<<(*outP)->barcode()<<endreq;
	  //
  	  // add variables for each track into ntuple
	  // Add all vertices to same ntuple, and extract later in root macro 
          m_truthAllTrk2dIP->push_back(tempd0); // these five variables 
          m_truthAllTrkzIP->push_back(tempz0); // are measured 
          m_truthAllTrkPt->push_back(temppt);  // relative to 
          m_truthAllTrkEta->push_back(tempeta); 
          m_truthAllTrkPhi->push_back(tempphi); // (0,0,0)

          m_truthAllTrkR->push_back(Vpos.perp());
          m_truthAllTrkZ->push_back(Vpos.z());
          m_truthAllTrkbc->push_back( ((*outP)->barcode()) );
	  //
          numOutP++;
          sumTrkpt += abstemppt; // these two are at (0,0,0)
          sumTrkpz += temppz;
          if(numOutP<=m_maxTrks) {
	    Trkpt[numOutP-1]=abstemppt; // @(0,0,0)
            Trkpx[numOutP-1]=abstemppt*cos(tempphi); 
            Trkpy[numOutP-1]=abstemppt*sin(tempphi);
            Trkpz[numOutP-1]=temppz;
	  }
	  mlog<<MSG::DEBUG<<"After Cuts: Out part bc/id/length/numOutP/sumTrkpz/sumTrkpt "<<(*outP)->barcode()<<","<<(*outP)->pdg_id()<<","<<length<<","<<numOutP<<","<<sumTrkpt<<","<<sumTrkpz<<endreq;
	  //
          int trkchg=1;
          if(id<0) trkchg=-1; // pi-/K-/anti-p have id<0
          if(id == -11 || id == -13) trkchg=1; // however, e+ and mu+ have id<0
          sumChg += trkchg;
	}
        if(longTrk && isTrack && etaFid && abstemppt> m_LoPtCut ) numOutPLoPt++;
      } // loop over particles in vertex

      mlog<<MSG::DEBUG<<"# outParticles passing cuts (High Pt) "<<numOutP<<endreq;
      mlog<<MSG::DEBUG<<"# outParticles passing cuts (Lo Pt) "<<numOutPLoPt<<endreq;
      //
      if(numOutP>=1) m_nSVGe1HiPt++;
      int vtxType=0;
      if(numOutPLoPt == 1) vtxType=11;
      if(numOutPLoPt > 1) vtxType=12;
      if(numOutP == 1) vtxType=1;
      if(numOutP > 1) vtxType=2;
      //
      if(numOutP>= 1) { // only store vertex type 1 or 2, i.e., # HiPt tracks >=1
      // if(numOutP>= 1 || numOutPLoPt>=1) { // temporary test. by default this is >=2
       m_nSVHiPt++;
       //
       m_truthAllVtxType->push_back(vtxType);
       m_truthAllSVX->push_back(Vpos.x());
       m_truthAllSVY->push_back(Vpos.y());
       m_truthAllSVZ->push_back(Vpos.z());
       m_truthAllVtxHasLifetime->push_back(vtxHasLifetime);
       m_truthAllVtxStrangeBaryon->push_back(strangeBaryon);
       m_truthAllVtxIncomingPID->push_back(IncomingPID);
       if((*vtxPtr)->particles_in_size()==1) mlog<<MSG::DEBUG<<"Incoming PID/Lifetime flag "<<IncomingPID<<","<<vtxHasLifetime<<endreq;

       m_truthAllVtxNumIncident->push_back((*vtxPtr)->particles_in_size());
       m_truthAllVtxPtIncident->push_back(pTincoming);
       m_truthAllVtxPzIncident->push_back(pZincoming);
       // first sort the track pt		
       //sort(Trkpt,Trkpt+m_maxTrks);					  
       sort(Trkpt,Trkpt+10);					  
       mlog<<MSG::DEBUG<<"numTrksHi/Lo, re-orderered "<<numOutP<<","<<numOutPLoPt<<","<<Trkpt[m_maxTrks-1]<<","<<Trkpt[m_maxTrks-2]<<","<<Trkpt[m_maxTrks-3]<<","<<Trkpt[m_maxTrks-4]<<","<<Trkpt[m_maxTrks-5]<<endreq;
       //
       // TrkPt has been initialized to zero, 
       // so if there are only 2 tracks all other vectors will get filled with zero
       m_truthAllVtxTrk1Pt->push_back(Trkpt[m_maxTrks-1]);		  
       m_truthAllVtxTrk2Pt->push_back(Trkpt[m_maxTrks-2]);		  
       m_truthAllVtxTrk3Pt->push_back(Trkpt[m_maxTrks-3]);
       m_truthAllVtxTrk4Pt->push_back(Trkpt[m_maxTrks-4]);
       m_truthAllVtxTrk5Pt->push_back(Trkpt[m_maxTrks-5]);
       //
       m_truthAllNumTrks->push_back(numOutP);
       m_truthAllNumRecoTrks->push_back(numReco);
       m_truthAllCharge->push_back(sumChg);
       m_truthAllSumTrksPt->push_back(sumTrkpt);
       m_truthAllSumTrksPz->push_back(sumTrkpz);
       //
       // calculate opening angle between all 2-track pairs, and store the minimum
       //
       double minOpAng = 1000;
       double opAng=0;
       double p1=0;
       double p2=0;
       for (int it1=0; it1<numOutP; ++it1) {
         if(it1>m_maxTrks) continue;
         p1 = sqrt(Trkpx[it1]*Trkpx[it1] + Trkpy[it1]*Trkpy[it1] + Trkpz[it1]*Trkpz[it1]);
         for (int it2=0; it2<numOutP; ++it2) {
           if(it2>m_maxTrks) continue;
           if(it1==it2) continue;
	   //
           p2 = sqrt(Trkpx[it2]*Trkpx[it2] + Trkpy[it2]*Trkpy[it2] + Trkpz[it2]*Trkpz[it2]);
           opAng = Trkpx[it1]*Trkpx[it2]+Trkpy[it1]*Trkpy[it2]+Trkpz[it1]*Trkpz[it2];
           opAng = (opAng/p1)/p2;
	   mlog<<MSG::DEBUG<<"Opening Ang/ minOpAng "<<opAng<<","<<minOpAng<<endreq;
           if(opAng < minOpAng) minOpAng = opAng;
	 }
       }
       m_truthAllMinOpAng->push_back(minOpAng);
       //
       // if there are >=2 matched reco trks at vertex, do a quick fit
       // 
       mlog<<MSG::DEBUG<<"numReco "<<numReco<<endreq;
       mlog<<MSG::DEBUG<<"Selected trk size "<<SelectedBaseTracks.size()<<endreq;
       mlog<<MSG::DEBUG<<"size of SelTrk_barcode "<<m_SelTrk_barcode->size()<<endreq;
       //
       int refitS =0;
       int numretr=0;
       double rex=0;
       double rey=0;
       double rez=0;
       double rec=0;
       double remass=0;
       //
       if(numReco>=2 && numReco<10 && SelectedBaseTracks.size()>=2) {

        vector<const Trk::TrackParticleBase*>  ListMatBaseTracks;
        ListMatBaseTracks.clear();
        WrkVrt newvrt;
	// 
	// first fill vector with selected tracks.
        for (int itmp=0; itmp<numReco; ++itmp) {

	  for(unsigned int isel=0; isel<SelectedBaseTracks.size(); ++isel) {

            mlog<<MSG::DEBUG<<"itmp/bcode of RecoTrks/SelTrk "<<itmp<<","<<bcodeOfRecoTrks[itmp]<<","<<isel<<","<<m_SelTrk_barcode->at(isel)<<endreq;

            // check if Selected reco trk passes all Si cuts
	    //

	    if(bcodeOfRecoTrks[itmp]==m_SelTrk_barcode->at(isel)) {

	      ListMatBaseTracks.push_back(SelectedBaseTracks[isel]);
	      //
	    }

	  } // loop over selected tracks

	} // numReco loop
	//
	// check size of selected reco tracks
        numretr = ListMatBaseTracks.size();
        if((int) ListMatBaseTracks.size() <2 ) {
          mlog<<MSG::DEBUG<<"mismatch numreco & re-fit size "<<numReco<<","<<ListMatBaseTracks.size()<<endreq;}
        else{
          mlog<<MSG::DEBUG<<"numreco & re-fit size "<<numReco<<","<<ListMatBaseTracks.size()<<endreq;

	  m_fitSvc->setApproximateVertex(Vpos.x(),Vpos.y(),Vpos.z());
	  sc=m_fitSvc->VKalVrtFit(ListMatBaseTracks,newvrt.vertex,newvrt.vertexMom,
				       newvrt.Charge,newvrt.vertexCov,newvrt.Chi2PerTrk, 
				       newvrt.TrkAtVrt,newvrt.Chi2);
	  //
	  // check if refit is success
	  //
          if(!sc) {refitS=-1; mlog<<MSG::INFO<<"Refit of match Selected tracks failed "<<endreq;}
	  //
          else {
            refitS=1;
            mlog<<MSG::INFO<<"Refit of match Selected tracks successful "<<endreq;
            mlog<<MSG::INFO<<" MC vertex "<<Vpos.x()<<","<<Vpos.y()<<","<<Vpos.z()<<endreq;
	    //
            rex=newvrt.vertex[0]; rey=newvrt.vertex[1]; rez=newvrt.vertex[2]; rec=newvrt.Chi2;
	    // 
	    // now loop over tracks at vertex and get mass
            double vx=0; double vy=0; double vz=0; double ve=0;
            for(unsigned int itatv=0; itatv<ListMatBaseTracks.size(); ++itatv) {
	      double theta = newvrt.TrkAtVrt[itatv][1];
	      double trk_p = fabs(1./ newvrt.TrkAtVrt[itatv][2]);
	      double trk_pt = trk_p*sin(theta);  
	      double trk_px = trk_pt*cos(newvrt.TrkAtVrt[itatv][0]);  
	      double trk_py = trk_pt*sin(newvrt.TrkAtVrt[itatv][0]);  
	      double trk_pz = trk_p*cos(theta);  
	      double trkp2 = trk_p*trk_p;
	      double trk_e = sqrt(trkp2 + m_pi*m_pi); 
	      //
	      vx += trk_px;
	      vy += trk_py;
	      vz += trk_pz;
	      ve += trk_e; 
	    }
	    //
            remass = sqrt(ve*ve - vx*vx -vy*vy -vz*vz);

            mlog<<MSG::INFO<<" Refit vertex "<<newvrt.vertex[0]<<","<<newvrt.vertex[1]<<","<<newvrt.vertex[2]<<","<<rec<<","<<remass<<endreq;
            
	  }
	} // successful re-fit

       } // if > 2 tracks with tigher cuts
       m_truthAllRefitStatus->push_back(refitS);
       m_truthAllNumReTrk->push_back(numretr);
       m_truthAllRefitSVX->push_back(rex);
       m_truthAllRefitSVY->push_back(rey);
       m_truthAllRefitSVZ->push_back(rez);
       m_truthAllRefitChi2->push_back(rec);
       m_truthAllRefitMass->push_back(remass);

      if(numOutPLoPt>= 2) m_nSVLoPt++;

      } // loop over vertices
    }
    mlog<<MSG::DEBUG<<"# SV with >= 1 Hi Pt outTrks passing cuts "<<m_nSVHiPt<<endreq;
    mlog<<MSG::DEBUG<<"# SV with >= 1 Lo Pt outTrks passing cuts "<<m_nSVLoPt<<endreq;
    mlog<<MSG::DEBUG<<" "<<endreq;
    
    return StatusCode::SUCCESS;
  }
  //
  ///////////////////////////////
  StatusCode VrtSecInclusive::setupNtuple() {

    // initialize the vectors, and add them to ntuple branches
       //
       m_numDOFPV = new std::vector<int>;
       m_PVZpile = new std::vector<double>;
       m_RecoTrk_pT  = new std::vector<double>;
       m_RecoTrk_2dIPPV  = new std::vector<double>;
       m_RecoTrk_zIPPV  = new std::vector<double>;
       m_RecoTrk_2dIP  = new std::vector<double>;
       m_RecoTrk_zIP  = new std::vector<double>;
       m_RecoTrk_phi  = new std::vector<double>;
       m_RecoTrk_eta  = new std::vector<double>;
       m_RecoTrk_chi2  = new std::vector<double>;
       m_RecoTrk_numBLay = new std::vector<int>;
       m_RecoTrk_numPix = new std::vector<int>;
       m_RecoTrk_numSCT = new std::vector<int>;
       m_RecoTrk_numTRT = new std::vector<int>;
       m_RecoTrk_PixBar1 = new std::vector<int>;
       m_RecoTrk_PixBar2 = new std::vector<int>;
       /*
       m_RecoTrk_SctBar0 = new std::vector<int>;
       m_RecoTrk_SctBar1 = new std::vector<int>;
       m_RecoTrk_SctBar2 = new std::vector<int>;
       m_RecoTrk_SctBar3 = new std::vector<int>;
	*/
       m_RecoTrk_barcode = new std::vector<int>;
       m_RecoTrk_matchPr = new std::vector<double>;
       m_RecoTrk_A0Error = new std::vector<double>;
       m_RecoTrk_ZError = new std::vector<double>;
       m_RecoTrk_PixShare = new std::vector<int>;
       m_RecoTrk_SctShare = new std::vector<int>;
       m_RecoTrk_TrkAuth = new std::vector<int>;
       m_RecoTrk_TrkLowPt = new std::vector<int>;
       //
       m_RecoTrk_Int_bpZ  = new std::vector<double>;
       m_RecoTrk_Int_pix1Z = new std::vector<double>;
       m_RecoTrk_Int_pix2Z = new std::vector<double>;
       m_RecoTrk_Int_pix3Z = new std::vector<double>;
       m_RecoTrk_Int_sct1Z = new std::vector<double>;
       m_RecoTrk_Int_sct2Z = new std::vector<double>;
       /*
       m_RecoTrk_numTRTholes = new std::vector<int>;
       m_RecoTrk_numTRTout = new std::vector<int>;
       m_RecoTrk_BLayOut = new std::vector<int>;
       m_RecoTrk_BLayX = new std::vector<double>;
       m_RecoTrk_BLayY = new std::vector<double>;
       m_RecoTrk_BLayZ = new std::vector<double>;
	*/
       m_SVTrk_pT = new std::vector<double>;
       m_SVTrk_p = new std::vector<double>;	 
       m_SVTrk_phi = new std::vector<double>;	 
       m_SVTrk_eta = new std::vector<double>;	 
       m_SVTrk_2dIP = new std::vector<double>; 
       m_SVTrk_zIP = new std::vector<double>;	 
       m_SVTrk_delp = new std::vector<double>; 
       m_SVTrk_del2dIP= new std::vector<double>;
       m_SVTrk_delzIP = new std::vector<double>;
       m_SVTrk_numBLay = new std::vector<int>;
       m_SVTrk_numPix = new std::vector<int>;
       m_SVTrk_numSCT = new std::vector<int>;
       m_SVTrk_numTRT = new std::vector<int>;
       m_SVTrk_barcode = new std::vector<int>;
       m_SVTrk_matchPr = new std::vector<double>;
       m_SVTrk_TrkAuth = new std::vector<int>;
       m_SVTrk_TrkLowPt = new std::vector<int>;
       //
       m_All2trkMass = new std::vector<double>;
       m_All2trkPt = new std::vector<double>;
       m_All2trkChg = new std::vector<int>;
       m_All2trkBLHSum = new std::vector<int>;
       m_All2trkVertX = new std::vector<double>;
       m_All2trkVertY = new std::vector<double>;
       m_All2trkVertZ = new std::vector<double>;
       m_All2trkVertChi2 = new std::vector<double>;
       //
       m_AfFak2trkMass = new std::vector<double>;
       m_AfFak2trkPt = new std::vector<double>;
       m_AfFak2trkChg = new std::vector<int>;
       m_AfFak2trkVertX = new std::vector<double>;
       m_AfFak2trkVertY = new std::vector<double>;
       m_AfFak2trkVertZ = new std::vector<double>;
       m_AfFak2trkVpos = new std::vector<double>;
       m_AfFak2trkVertChi2 = new std::vector<double>;
       //
       m_SecVtxX = new std::vector<double>;
       m_SecVtxY = new std::vector<double>;
       m_SecVtxZ = new std::vector<double>;
       m_numTrksSecVtx = new std::vector<int>;
       m_massSecVtx = new std::vector<double>;
       m_masseSecVtx = new std::vector<double>;
       m_chi2SecVtx = new std::vector<double>;
       m_pTSecVtx = new std::vector<double>;
       m_pZSecVtx = new std::vector<double>;
       m_chgSecVtx = new std::vector<int>;
       m_SumBLayHits = new std::vector<int>;
       m_AllTrksBLayHits = new std::vector<int>;
       m_SecVtxMinOpAng = new std::vector<double>;
       m_SecVtxTrkPt = new std::vector<double>;
       m_SecVtxTrkPhi = new std::vector<double>;
       m_SecVtxTrkEta = new std::vector<double>;
       m_SecVtxTrkBLay = new std::vector<int>;
       m_SecVtxTrkPixExclBLay = new std::vector<int>;
       m_SecVtxTrkSCT = new std::vector<int>;
       //
       m_truthAllVtxType = new std::vector<int>;
       m_truthAllSVX = new std::vector<double>;
       m_truthAllSVY = new std::vector<double>;
       m_truthAllSVZ = new std::vector<double>;
       m_truthAllVtxTrk1Pt = new std::vector<double>;
       m_truthAllVtxTrk2Pt = new std::vector<double>;
       m_truthAllVtxTrk3Pt = new std::vector<double>;
       m_truthAllVtxTrk4Pt = new std::vector<double>;
       m_truthAllVtxTrk5Pt = new std::vector<double>;
       m_truthAllNumTrks = new std::vector<int>;
       m_truthAllNumRecoTrks = new std::vector<int>;
       m_truthAllCharge = new std::vector<int>;
       m_truthAllSumTrksPt = new std::vector<double>;
       m_truthAllSumTrksPz = new std::vector<double>;
       m_truthAllVtxHasLifetime = new std::vector<int>;
       m_truthAllVtxStrangeBaryon = new std::vector<int>;
       m_truthAllVtxIncomingPID = new std::vector<int>;
       m_truthAllVtxNumIncident = new std::vector<int>;
       m_truthAllVtxPtIncident = new std::vector<double>;
       m_truthAllVtxPzIncident = new std::vector<double>;
       m_truthAllMinOpAng = new std::vector<double>;
       m_truthAllTrk2dIP = new std::vector<double>;
       m_truthAllTrkzIP = new std::vector<double>;
       m_truthAllTrkPt = new std::vector<double>;
       m_truthAllTrkEta = new std::vector<double>;
       m_truthAllTrkPhi = new std::vector<double>;
       m_truthAllTrkR = new std::vector<double>;
       m_truthAllTrkZ = new std::vector<double>;
       m_truthAllTrkbc = new std::vector<int>;
       //
       m_truthAllRefitStatus = new std::vector<int>;
       m_truthAllNumReTrk = new std::vector<int>;
       m_truthAllRefitChi2 = new std::vector<double>;
       m_truthAllRefitSVX = new std::vector<double>;
       m_truthAllRefitSVY = new std::vector<double>;
       m_truthAllRefitSVZ = new std::vector<double>;
       m_truthAllRefitMass = new std::vector<double>;

    
       // now make branches

       m_tree_Vert->Branch("Run",  &m_runNumber,   "Run/I");    // run number
       m_tree_Vert->Branch("Event",&m_eventNumber, "Event/I");  // event number
       m_tree_Vert->Branch("Time", &m_eventTime,   "Time/I");   // time stamp
       m_tree_Vert->Branch("LumiBlock", &m_lumiBlock,"LumiBlock/I"); // lum block num 
       m_tree_Vert->Branch("BCID", &m_bCID,"BCID/I"); // bunch crossing ID
       //
       m_tree_Vert->Branch("NumAllTrks", &m_allTrks, "NumAllTrks/i");
       m_tree_Vert->Branch("NumSelTrks", &m_selTrks, "NumSelTrks/i");
       m_tree_Vert->Branch("SizeIncomp", &m_sizeIncomp, "SizeIncomp/i");
       //
       m_tree_Vert->Branch("NumPV",&m_numPV,"NumPV/i");
       m_tree_Vert->Branch("NdofTrksPV",&m_numDOFPV);
       m_tree_Vert->Branch("PVX",&m_PVX,"PVX/d");
       m_tree_Vert->Branch("PVY",&m_PVY,"PVY/d");
       m_tree_Vert->Branch("PVZ",&m_PVZ,"PVZ/d");
       m_tree_Vert->Branch("PVZpile",&m_PVZpile);
       m_tree_Vert->Branch("NTrksPV",&m_numTrksPV,"NTrksPV/i");
       //
       // parameters of all tracks in event
       m_tree_Vert->Branch("RecoTrk_pT",&m_RecoTrk_pT);
       m_tree_Vert->Branch("RecoTrk_2dIPPV",&m_RecoTrk_2dIPPV);
       m_tree_Vert->Branch("RecoTrk_ZIPPV",&m_RecoTrk_zIPPV);
       m_tree_Vert->Branch("RecoTrk_2dIP",&m_RecoTrk_2dIP);
       m_tree_Vert->Branch("RecoTrk_ZIP",&m_RecoTrk_zIP);
       m_tree_Vert->Branch("RecoTrk_phi",&m_RecoTrk_phi);
       m_tree_Vert->Branch("RecoTrk_chi2",&m_RecoTrk_chi2);
       m_tree_Vert->Branch("RecoTrk_eta",&m_RecoTrk_eta);
       m_tree_Vert->Branch("RecoTrk_BLayHits",&m_RecoTrk_numBLay);
       m_tree_Vert->Branch("RecoTrk_PixHits",&m_RecoTrk_numPix);
       m_tree_Vert->Branch("RecoTrk_SCTHits",&m_RecoTrk_numSCT);
       m_tree_Vert->Branch("RecoTrk_TRTHits",&m_RecoTrk_numTRT);
       m_tree_Vert->Branch("RecoTrk_PixBar1",&m_RecoTrk_PixBar1);
       m_tree_Vert->Branch("RecoTrk_PixBar2",&m_RecoTrk_PixBar2);
       /*
       m_tree_Vert->Branch("RecoTrk_SctBar0",&m_RecoTrk_SctBar0);
       m_tree_Vert->Branch("RecoTrk_SctBar1",&m_RecoTrk_SctBar1);
       m_tree_Vert->Branch("RecoTrk_SctBar2",&m_RecoTrk_SctBar2);
       m_tree_Vert->Branch("RecoTrk_SctBar3",&m_RecoTrk_SctBar3);
       */
       m_tree_Vert->Branch("RecoTrk_barcode",&m_RecoTrk_barcode);
       m_tree_Vert->Branch("RecoTrk_matchPr",&m_RecoTrk_matchPr);
       m_tree_Vert->Branch("RecoTrk_2dIPErr",&m_RecoTrk_A0Error);
       m_tree_Vert->Branch("RecoTrk_ZIPErr",&m_RecoTrk_ZError);
       m_tree_Vert->Branch("RecoTrk_PixShare",&m_RecoTrk_PixShare);
       m_tree_Vert->Branch("RecoTrk_SCTShare",&m_RecoTrk_SctShare);
       m_tree_Vert->Branch("RecoTrk_TrkAuth",&m_RecoTrk_TrkAuth);
       m_tree_Vert->Branch("RecoTrk_TrkLowPt",&m_RecoTrk_TrkLowPt);

       m_tree_Vert->Branch("RecoTrk_Int_bpZ",   &m_RecoTrk_Int_bpZ);
       m_tree_Vert->Branch("RecoTrk_Int_pix1Z", &m_RecoTrk_Int_pix1Z);
       m_tree_Vert->Branch("RecoTrk_Int_pix2Z", &m_RecoTrk_Int_pix2Z);
       m_tree_Vert->Branch("RecoTrk_Int_pix3Z", &m_RecoTrk_Int_pix3Z);
       m_tree_Vert->Branch("RecoTrk_Int_sct1Z", &m_RecoTrk_Int_sct1Z);
       m_tree_Vert->Branch("RecoTrk_Int_sct2Z", &m_RecoTrk_Int_sct2Z);
       /*
       m_tree_Vert->Branch("RecoTrk_TRTHoles",&m_RecoTrk_numTRTholes);       
       m_tree_Vert->Branch("RecoTrk_TRTOut",  &m_RecoTrk_numTRTout);
       m_tree_Vert->Branch("RecoTrk_BLayOut", &m_RecoTrk_BLayOut);
       m_tree_Vert->Branch("RecoTrk_BLayX",   &m_RecoTrk_BLayX);
       m_tree_Vert->Branch("RecoTrk_BLayY",   &m_RecoTrk_BLayY);
       m_tree_Vert->Branch("RecoTrk_BLayZ",   &m_RecoTrk_BLayZ);
       */
       // parameters of tracks selected for SVT
       m_tree_Vert->Branch("SVTrk_pT",&m_SVTrk_pT);
       m_tree_Vert->Branch("SVTrk_p",&m_SVTrk_p);
       m_tree_Vert->Branch("SVTrk_phi",&m_SVTrk_phi);
       m_tree_Vert->Branch("SVTrk_eta",&m_SVTrk_eta);
       m_tree_Vert->Branch("SVTrk_2dIP",&m_SVTrk_2dIP);
       m_tree_Vert->Branch("SVTrk_ZIP",&m_SVTrk_zIP);
       m_tree_Vert->Branch("SVTrk_delp",&m_SVTrk_delp);
       m_tree_Vert->Branch("SVTrk_del2dIP",&m_SVTrk_del2dIP);
       m_tree_Vert->Branch("SVTrk_delZIP",&m_SVTrk_delzIP);
       m_tree_Vert->Branch("SVTrk_BLayHits",&m_SVTrk_numBLay);
       m_tree_Vert->Branch("SVTrk_PixHits", &m_SVTrk_numPix);
       m_tree_Vert->Branch("SVTrk_SCTHits", &m_SVTrk_numSCT);
       m_tree_Vert->Branch("SVTrk_TRTHits", &m_SVTrk_numTRT);
       m_tree_Vert->Branch("SVTrk_barcode", &m_SVTrk_barcode);
       m_tree_Vert->Branch("SVTrk_matchPr", &m_SVTrk_matchPr);
       m_tree_Vert->Branch("SVTrk_TrkAuth", &m_SVTrk_TrkAuth);
       m_tree_Vert->Branch("SVTrk_TrkLowPt",&m_SVTrk_TrkLowPt);
       // 2trk vertices 
       m_tree_Vert->Branch("All2TrkVrtNum",&m_numAll2trkVert,"NumAll2TrkVrt/i");
       m_tree_Vert->Branch("All2TrkVrtMass",&m_All2trkMass);
       m_tree_Vert->Branch("All2TrkVrtPt",&m_All2trkPt);
       m_tree_Vert->Branch("All2TrkVrtCharge",&m_All2trkChg);
       m_tree_Vert->Branch("All2TrkVrtSumBLHits",&m_All2trkBLHSum);
       m_tree_Vert->Branch("All2TrkVrtX",&m_All2trkVertX);
       m_tree_Vert->Branch("All2TrkVrtY",&m_All2trkVertY);
       m_tree_Vert->Branch("All2TrkVrtZ",&m_All2trkVertZ);
       m_tree_Vert->Branch("All2TrkVrtChiSq",&m_All2trkVertChi2);
       //
       m_tree_Vert->Branch("AfFak2TrkVrtNum",&m_numAfFak2trkVert,"NumAfFak2TrkVrt/i");
       m_tree_Vert->Branch("AfFak2TrkVrtMass",&m_AfFak2trkMass);
       m_tree_Vert->Branch("AfFak2TrkVrtPt",&m_AfFak2trkPt);
       m_tree_Vert->Branch("AfFak2TrkVrtCharge",&m_AfFak2trkChg);
       m_tree_Vert->Branch("AfFak2TrkVrtX",&m_AfFak2trkVertX);
       m_tree_Vert->Branch("AfFak2TrkVrtY",&m_AfFak2trkVertY);
       m_tree_Vert->Branch("AfFak2TrkVrtZ",&m_AfFak2trkVertZ);
       m_tree_Vert->Branch("AfFak2TrkVpos",&m_AfFak2trkVpos);
       m_tree_Vert->Branch("AfFak2TrkVrtChiSq",&m_AfFak2trkVertChi2);

       //
       m_tree_Vert->Branch("NumInitSecVrt",&m_InitialSVsize,"NumInitSecVrt/i");
       m_tree_Vert->Branch("NumRearrSecVrt",&m_RearrangedSVsize,"NumRearrSecVrt/i");
       m_tree_Vert->Branch("NumSecVrt",&m_numSecVrt,"NumSecVrt/i");
       m_tree_Vert->Branch("SecVtxX",&m_SecVtxX);
       m_tree_Vert->Branch("SecVtxY",&m_SecVtxY);
       m_tree_Vert->Branch("SecVtxZ",&m_SecVtxZ);
       m_tree_Vert->Branch("SecVtx_NumTrks",&m_numTrksSecVtx);
       m_tree_Vert->Branch("SecVtx_Mass",&m_massSecVtx);
       m_tree_Vert->Branch("SecVtx_Mass_electron",&m_masseSecVtx);
       m_tree_Vert->Branch("SecVtx_Chi2",&m_chi2SecVtx);
       m_tree_Vert->Branch("SecVtx_pT",&m_pTSecVtx);
       m_tree_Vert->Branch("SecVtx_pZ",&m_pZSecVtx);
       m_tree_Vert->Branch("SecVtx_Charge",&m_chgSecVtx);
       m_tree_Vert->Branch("SecVtx_SumBLayHits",&m_SumBLayHits);
       m_tree_Vert->Branch("SecVtx_AllTrksBLayHits",&m_AllTrksBLayHits);
       m_tree_Vert->Branch("SecVtx_MinOpAng",&m_SecVtxMinOpAng);
       m_tree_Vert->Branch("SecVtx_TrkPt",&m_SecVtxTrkPt);
       m_tree_Vert->Branch("SecVtx_TrkPhi",&m_SecVtxTrkPhi);
       m_tree_Vert->Branch("SecVtx_TrkEta",&m_SecVtxTrkEta);
       m_tree_Vert->Branch("SecVtx_TrkBLay",&m_SecVtxTrkBLay);
       m_tree_Vert->Branch("SecVtx_TrkPixExclBLay",&m_SecVtxTrkPixExclBLay);
       m_tree_Vert->Branch("SecVtx_TrkSCT",&m_SecVtxTrkSCT);

       //
       // truth information (will make sense for single pi MC)
       //
       m_tree_Vert->Branch("Truth_SV1X",&m_truthSV1X,"Truth_SV1X/d");
       m_tree_Vert->Branch("Truth_SV1Y",&m_truthSV1Y,"Truth_SV1Y/d");
       m_tree_Vert->Branch("Truth_SV1Z",&m_truthSV1Z,"Truth_SV1Z/d");
       //
       /*
       // these vectors have not been initialized. do so before uncommenting
       m_tree_Vert->Branch("TruthTrk_pt",&m_TruthTrk_pt);       
       m_tree_Vert->Branch("TruthTrk_eta",&m_TruthTrk_eta);       
       m_tree_Vert->Branch("TruthTrk_phi",&m_TruthTrk_phi);       
       m_tree_Vert->Branch("TruthTrk_R",&m_TruthTrk_R);       
       m_tree_Vert->Branch("TruthTrk_Z",&m_TruthTrk_Z);       
       m_tree_Vert->Branch("TruthTrk_EndR",&m_TruthTrk_EndR);       
       m_tree_Vert->Branch("TruthTrk_EndZ",&m_TruthTrk_EndZ);       
       m_tree_Vert->Branch("TruthTrk_L",&m_TruthTrk_L);       
       m_tree_Vert->Branch("TruthTrk_barcode",&m_TruthTrk_barcode);       
       m_tree_Vert->Branch("TruthTrk_pid",&m_TruthTrk_pid);       
       m_tree_Vert->Branch("TruthTrk_matchPr",&m_TruthTrk_matchPr);       
       m_tree_Vert->Branch("TruthTrk_2dIP",&m_TruthTrk_2dIP);       
       m_tree_Vert->Branch("TruthTrk_ZIP",&m_TruthTrk_ZIP);       
       */
       //
       m_tree_Vert->Branch("Truth_PiEta",&m_truthEta,"Truth_PiEta/d");
       m_tree_Vert->Branch("Truth_PiPhi",&m_truthPhi,"Truth_PiPhi/d");
       m_tree_Vert->Branch("Truth_PiPt",&m_truthPt,"Truth_PiPt/d");
       m_tree_Vert->Branch("Truth_PiInt",&m_truthInt,"Truth_PiInt/i");
       // num. of SV with >=2 outgoing tracks passing cuts
       m_tree_Vert->Branch("Truth_nSVHiPt",&m_nSVHiPt,"Truth_nSVHiPt/i");
       m_tree_Vert->Branch("Truth_nSVLoPt",&m_nSVLoPt,"Truth_nSVLoPt/i");
       m_tree_Vert->Branch("Truth_nSVGe1HiPt",&m_nSVGe1HiPt,"Truth_nSVGe1HiPt/i");
       m_tree_Vert->Branch("Truth_AllVtxType",&m_truthAllVtxType);
       m_tree_Vert->Branch("Truth_AllSVX",&m_truthAllSVX);
       m_tree_Vert->Branch("Truth_AllSVY",&m_truthAllSVY);
       m_tree_Vert->Branch("Truth_AllSVZ",&m_truthAllSVZ);
       m_tree_Vert->Branch("Truth_AllSVTrk1Pt",&m_truthAllVtxTrk1Pt);
       m_tree_Vert->Branch("Truth_AllSVTrk2Pt",&m_truthAllVtxTrk2Pt);
       m_tree_Vert->Branch("Truth_AllSVTrk3Pt",&m_truthAllVtxTrk3Pt);
       m_tree_Vert->Branch("Truth_AllSVTrk4Pt",&m_truthAllVtxTrk4Pt);
       m_tree_Vert->Branch("Truth_AllSVTrk5Pt",&m_truthAllVtxTrk5Pt);
       m_tree_Vert->Branch("Truth_AllSVNumTrks",&m_truthAllNumTrks);
       m_tree_Vert->Branch("Truth_AllSVNumRecoTrks",&m_truthAllNumRecoTrks);
       m_tree_Vert->Branch("Truth_AllSVCharge",&m_truthAllCharge);
       m_tree_Vert->Branch("Truth_AllSVSumTrksPt",&m_truthAllSumTrksPt);
       m_tree_Vert->Branch("Truth_AllSVSumTrksPz",&m_truthAllSumTrksPz);
       m_tree_Vert->Branch("Truth_AllSVHasLifetime",&m_truthAllVtxHasLifetime);
       m_tree_Vert->Branch("Truth_AllSVStrangeBaryon",&m_truthAllVtxStrangeBaryon);
       m_tree_Vert->Branch("Truth_AllSVIncomingPid",&m_truthAllVtxIncomingPID);
       m_tree_Vert->Branch("Truth_AllSVNumIncident",&m_truthAllVtxNumIncident);
       m_tree_Vert->Branch("Truth_AllSVPtIncident",&m_truthAllVtxPtIncident);
       m_tree_Vert->Branch("Truth_AllSVPzIncident",&m_truthAllVtxPzIncident);
       m_tree_Vert->Branch("Truth_AllSVMinOpAng",&m_truthAllMinOpAng);
       m_tree_Vert->Branch("Truth_AllTrk2dIP",&m_truthAllTrk2dIP);
       m_tree_Vert->Branch("Truth_AllTrkZIP",&m_truthAllTrkzIP);
       m_tree_Vert->Branch("Truth_AllTrkPt",&m_truthAllTrkPt);
       m_tree_Vert->Branch("Truth_AllTrkEta",&m_truthAllTrkEta);
       m_tree_Vert->Branch("Truth_AllTrkPhi",&m_truthAllTrkPhi);
       m_tree_Vert->Branch("Truth_AllTrkR",&m_truthAllTrkR);
       m_tree_Vert->Branch("Truth_AllTrkZ",&m_truthAllTrkZ);
       m_tree_Vert->Branch("Truth_AllTrkBC",&m_truthAllTrkbc);
       //
       m_tree_Vert->Branch("Truth_AllRefitSt"  ,&m_truthAllRefitStatus);
       m_tree_Vert->Branch("Truth_AllRefitNTrk",&m_truthAllNumReTrk);
       m_tree_Vert->Branch("Truth_AllRefitChi2",&m_truthAllRefitChi2);
       m_tree_Vert->Branch("Truth_AllRefitSVX", &m_truthAllRefitSVX);
       m_tree_Vert->Branch("Truth_AllRefitSVY", &m_truthAllRefitSVY);
       m_tree_Vert->Branch("Truth_AllRefitSVZ", &m_truthAllRefitSVZ);
       m_tree_Vert->Branch("Truth_AllRefitMass",&m_truthAllRefitMass);

       return StatusCode::SUCCESS;
  }
  // add event level variables
  StatusCode VrtSecInclusive::addEventInfo() {
    //
    ATH_MSG_DEBUG( "in addEventInfo");

    // this code has been taken from AnalysisExamples/VFitZmmOnAOD
    // 
    //get EventInfo for run and event number

    const EventInfo* eventInfo;
    StatusCode sc = evtStore()->retrieve(eventInfo);
  
    if (sc.isFailure())
      {
	ATH_MSG_WARNING("Could not retrieve event info");
	return sc;
      }
  
    const EventID* myEventID=eventInfo->event_ID();
    //
    m_runNumber=myEventID->run_number();
    m_eventNumber=myEventID->event_number();
    ATH_MSG_DEBUG( "event "<<m_eventNumber);

    m_eventTime= myEventID->time_stamp() ; 
    m_lumiBlock=myEventID->lumi_block() ;
    m_bCID=myEventID->bunch_crossing_id() ;

    return StatusCode::SUCCESS;
  }
} // end of namespace bracket
