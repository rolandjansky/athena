/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonSivsTRT.cxx
// AUTHORS: Ben Cooper
// **********************************************************************

#include "IDAlignMonSivsTRT.h"

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"

#include "GaudiKernel/IJobOptionsSvc.h"



#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrackSelectionTool.h"



// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonSivsTRT::IDAlignMonSivsTRT( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{
  m_trackSelection = ToolHandle<InDetAlignMon::TrackSelectionTool>("InDetAlignMon::TrackSelectionTool");
  m_trackSumTool = ToolHandle<Trk::ITrackSummaryTool>("Trk::TrackSummaryTool/InDetTrackSummaryTool");
  m_triggerChainName = "NoTriggerSelection";

  InitializeHistograms();


  declareProperty("CheckRate"              , m_checkrate = 1000);
  declareProperty("triggerChainName"       , m_triggerChainName);
  declareProperty("trackSelection"         , m_trackSelection);
  declareProperty("trackSumTool"           , m_trackSumTool);
  declareProperty("MatchdRCut"             , m_matchdRcut = 0.01);

}

//---------------------------------------------------------------------------------------

void IDAlignMonSivsTRT::InitializeHistograms(){


  m_delta_phi0 = 0 ;
  m_delta_eta0 = 0 ;
  m_delta_R = 0 ;
  m_delta_qoverp = 0 ;
  m_delta_charge = 0 ;
  m_delta_d0 = 0 ;
  m_delta_z0 = 0 ;
  m_delta_pt = 0 ;
  m_reldelta_pt = 0 ;
  m_nhitstrt = 0 ;

  m_delta_phi0_b = 0 ;
  m_delta_eta0_b = 0 ;
  m_delta_R_b = 0 ;
  m_delta_qoverp_b = 0 ;
  m_delta_charge_b = 0 ;
  m_delta_d0_b = 0 ;
  m_delta_z0_b = 0 ;
  m_delta_pt_b = 0 ;
  m_reldelta_pt_b = 0 ;

  m_alltrks_phi0 = 0 ;
  m_alltrks_phi0_b = 0 ;
  m_alltrks_phi0_eca = 0 ;
  m_alltrks_phi0_ecc = 0 ;
  m_alltrks_eta0 = 0 ;
  m_si_phi0 = 0 ;
  m_si_eta0 = 0 ;
  m_trt_phi0 = 0 ;
  m_trt_phi0_b = 0 ;
  m_trt_phi0_eca = 0 ;
  m_trt_phi0_ecc = 0 ;
  m_trt_eta0 = 0 ;
  m_matched_phi0 = 0 ;
  m_matched_eta0 = 0;

  m_sieff_phi0 = 0 ;
  m_sieff_eta0 = 0 ;
  m_trteff_phi0 = 0 ;
  m_trteff_phi0_b = 0 ;
  m_trteff_phi0_eca = 0 ;
  m_trteff_phi0_ecc = 0 ;
  m_trteff_eta0 = 0 ;


}






IDAlignMonSivsTRT::~IDAlignMonSivsTRT() { }


StatusCode IDAlignMonSivsTRT::initialize()
{

  m_events = 0;
  m_histosBooked = 0;

  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  //initialize tools and services
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling initialize() to setup tools/services" << endmsg;
  sc = setupTools();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to initialize tools/services!" << endmsg;
    return StatusCode::FAILURE;
  }
  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Successfully initialized tools/services" << endmsg;
  return sc;
}


StatusCode IDAlignMonSivsTRT::bookHistograms()
{
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  MonGroup al_mon ( this, "IDAlignMon/SivsTRT/" + m_triggerChainName, run );

  if ( newLowStatFlag() || newLumiBlockFlag() ) {
  }

  if( newRunFlag() ) {

    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    m_nhitstrt = new TH1F("nhits_trt","Number of TRT hits for every track in ExtendedTracks",100,0,100);
    RegisterHisto(al_mon,m_nhitstrt);

    m_delta_phi0 = new TH1F("delta_phi0","#phi_{0}^{TRT} - #phi_{0}^{Si-only}",200,-0.1,0.1);
    RegisterHisto(al_mon,m_delta_phi0);
    m_delta_eta0 = new TH1F("delta_eta0","#eta_{0}^{TRT} - #eta_{0}^{Si-only}",200,-0.1,0.1);
    RegisterHisto(al_mon,m_delta_eta0);
    m_delta_R = new TH1F("delta_R","deltaR(TRT-SiOnly)",200,-0.1,0.1);
    RegisterHisto(al_mon,m_delta_R);
    m_delta_qoverp = new TH1F("delta_qoverp","Q/P^{TRT} - Q/P^{Si-only} (1/GeV)",200,-0.01,0.01);
    RegisterHisto(al_mon,m_delta_qoverp);
    m_delta_d0 = new TH1F("delta_d0","d_{0}^{TRT} - d_{0}^{Si-only}",500,-1.0,1.0);
    RegisterHisto(al_mon,m_delta_d0);
    m_delta_z0 = new TH1F("delta_z0","z_{0}^{TRT} - z_{0}^{Si-only}",500,-1.0,1.0);
    RegisterHisto(al_mon,m_delta_z0);
    m_delta_pt = new TH1F("delta_pt","P_{T}^{TRT} - P_{T}^{Si-only}",300,-30.0,30.0);
    RegisterHisto(al_mon,m_delta_pt);
    m_reldelta_pt = new TH1F("reldelta_pt","P_{T}^{TRT} - P_{T}^{Si-only}/P_{T}^{Si-only}",500,-1.0,1.0);
    RegisterHisto(al_mon,m_reldelta_pt);
    m_delta_charge = new TH1F("delta_charge","Charge^{TRT} - Charge^{Si-only}",5,-2.5,2.5);
    RegisterHisto(al_mon,m_delta_charge);

    m_delta_phi0_b = new TH1F("delta_phi0_b","#phi_{0}^{TRT} - #phi_{0}^{Si-only} |#eta|<0.8",500,-0.1,0.1);
    RegisterHisto(al_mon,m_delta_phi0_b);
    m_delta_eta0_b = new TH1F("delta_eta0_b","#eta_{0}^{TRT} - #eta_{0}^{Si-only} |#eta|<0.8",500,-0.1,0.1);
    RegisterHisto(al_mon,m_delta_eta0_b);
    m_delta_R_b = new TH1F("delta_R_b","deltaR(TRT-SiOnly) |#eta|<0.8",200,-0.1,0.1);
    RegisterHisto(al_mon,m_delta_R_b);
    m_delta_qoverp_b = new TH1F("delta_qoverp_b","Q/P^{TRT} - Q/P^{Si-only} |#eta|<0.8 (1/GeV)",500,-0.01,0.01);
    RegisterHisto(al_mon,m_delta_qoverp_b);
    m_delta_d0_b = new TH1F("delta_d0_b","d_{0}^{TRT} - d_{0}^{Si-only} |#eta|<0.8",500,-1.0,1.0);
    RegisterHisto(al_mon,m_delta_d0_b);
    m_delta_z0_b = new TH1F("delta_z0_b","z_{0}^{TRT} - z_{0}^{Si-only} |#eta|<0.8",500,-1.0,1.0);
    RegisterHisto(al_mon,m_delta_z0_b);
    m_delta_pt_b = new TH1F("delta_pt_b","P_{T}^{TRT} - P_{T}^{Si-only} |#eta|<0.8",300,-30.0,30.0);
    RegisterHisto(al_mon,m_delta_pt_b);
    m_reldelta_pt_b = new TH1F("reldelta_pt_b","P_{T}^{TRT} - P_{T}^{Si-only}/P_{T}^{Si-only} |#eta|<0.8",500,-1.0,1.0);
    RegisterHisto(al_mon,m_reldelta_pt_b);
    m_delta_charge_b = new TH1F("delta_charge_b","Charge^{TRT} - Charge^{Si-only} |#eta|<0.8",5,-2.5,2.5);
    RegisterHisto(al_mon,m_delta_charge_b);

    //for investigation of efficiency for TRT hits and TRT-Si track matching as function of phi,eta

    m_alltrks_phi0 = new TH1F("alltrks_phi0","#phi_{0} all tracks",10,-3.14,3.14);
    RegisterHisto(al_mon,m_alltrks_phi0);
    m_alltrks_phi0_b = new TH1F("alltrks_phi0_b","#phi_{0} all tracks |#eta| < 1.0",10,-3.14,3.14);
    RegisterHisto(al_mon,m_alltrks_phi0_b);
    m_alltrks_phi0_eca = new TH1F("alltrks_phi0_eca","#phi_{0} all tracks 1.0 < #eta < 2.1",10,-3.14,3.14);
    RegisterHisto(al_mon,m_alltrks_phi0_eca);
    m_alltrks_phi0_ecc = new TH1F("alltrks_phi0_ecc","#phi_{0} all tracks -2.1 < #eta < -1.0",10,-3.14,3.14);
    RegisterHisto(al_mon,m_alltrks_phi0_ecc);
    m_alltrks_eta0 = new TH1F("alltrks_eta0","#eta_{0} all tracks",10,-2.1,2.1);
    RegisterHisto(al_mon,m_alltrks_eta0);
    m_si_phi0 = new TH1F("si_phi0","#phi_{0} Si-only tracks with no TRT hits",10,-3.14,3.14);
    RegisterHisto(al_mon,m_si_phi0);
    m_si_eta0 = new TH1F("si_eta0","#eta_{0} Si-only tracks with no TRT hits",10,-2.1,2.1);
    RegisterHisto(al_mon,m_si_eta0);
    m_trt_phi0 = new TH1F("trt_phi0","#phi_{0} tracks with TRT hits #eta < 2.1",10,-3.14,3.14);
    RegisterHisto(al_mon,m_trt_phi0);
    m_trt_phi0_b = new TH1F("trt_phi0_b","#phi_{0} tracks with TRT hits |#eta| < 1.0",10,-3.14,3.14);
    RegisterHisto(al_mon,m_trt_phi0_b);
    m_trt_phi0_eca = new TH1F("trt_phi0_eca","#phi_{0} tracks with TRT hits 1.0 < #eta < 2.1",10,-3.14,3.14);
    RegisterHisto(al_mon,m_trt_phi0_eca);
    m_trt_phi0_ecc = new TH1F("trt_phi0_ecc","#phi_{0} tracks with TRT hits -2.1 < #eta < -1.0",10,-3.14,3.14);
    RegisterHisto(al_mon,m_trt_phi0_ecc);
    m_trt_eta0 = new TH1F("trt_eta0","#eta_{0} tracks with TRT hits",10,-2.1,2.1);
    RegisterHisto(al_mon,m_trt_eta0);
    m_matched_phi0 = new TH1F("matched_phi0","#phi_{0} tracks with TRT hits that match Sionly",10,-3.14,3.14);
    RegisterHisto(al_mon,m_matched_phi0);
    m_matched_eta0 = new TH1F("matched_eta0","#eta_{0} tracks with TRT hits that match Sionly",10,-2.1,2.1);
    RegisterHisto(al_mon,m_matched_eta0);

    //actual Efficiency histograms are now TProfile to enable merger of histos
    m_sieff_eta0 = new TProfile("eff_simatch_eta0","eff #DeltaR matching Sionly to TRT extended track vs #eta_{0}",10,-2.1,2.1,0.0,1.0);
    RegisterHisto(al_mon,m_sieff_eta0);
    m_sieff_phi0 = new TProfile("eff_simatch_phi0","eff #DeltaR matching Sionly to TRT extended track vs #phi_{0}",10,-3.14,3.14,0.0,1.0);
    RegisterHisto(al_mon,m_sieff_phi0);
    m_trteff_phi0 = new TProfile("eff_trthits_phi0","eff TRT hits association vs #phi_{0}",10,-3.14,3.14,0.0,1.0);
    RegisterHisto(al_mon,m_trteff_phi0);
    m_trteff_phi0_b = new TProfile("eff_trthits_phi0_b","eff TRT hits association vs #phi_{0}, |#eta| < 1.0",10,-3.14,3.14,0.0,1.0);
    RegisterHisto(al_mon,m_trteff_phi0_b);
    m_trteff_phi0_eca = new TProfile("eff_trthits_phi0_eca","eff TRT hits association vs #phi_{0}, 1.0 < #eta < 2.1",10,-3.14,3.14,0.0,1.0);
    RegisterHisto(al_mon,m_trteff_phi0_eca);
    m_trteff_phi0_ecc = new TProfile("eff_trthits_phi0_ecc","eff TRT hits association vs #phi_{0}, -2.1 < #eta < 1.0",10,-3.14,3.14,0.0,1.0);
    RegisterHisto(al_mon,m_trteff_phi0_ecc);
    m_trteff_eta0 = new TProfile("eff_trthits_eta0","eff TRT hits association vs #eta_{0}",10,-2.1,2.1,0.0,1.0);
    RegisterHisto(al_mon,m_trteff_eta0);

    m_histosBooked++;
  }



  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

void IDAlignMonSivsTRT::RegisterHisto(MonGroup& mon, TH1* histo) {

  //histo->Sumw2();
  histo->SetOption("e");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram:" << endmsg;
  }
}

void IDAlignMonSivsTRT::RegisterHisto(MonGroup& mon, TProfile* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TProfile Histogram:" << endmsg;
  }
}

void IDAlignMonSivsTRT::RegisterHisto(MonGroup& mon, TH2* histo) {

  //histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH2 Histogram:" << endmsg;
  }
}


//---------------------------------------------------------------------------------------

StatusCode IDAlignMonSivsTRT::fillHistograms()
{

  m_events++;

  //checking that we can retrieve the required track collections from SG
  //if not print out warning but only for first event
  if (!evtStore()->contains<TrackCollection>("ResolvedTracks")) {
    if(m_events == 1) {if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to get ResolvedTracks TrackCollection" << endmsg;}
    else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get ResolvedTracks TrackCollection - histograms will not be filled" << endmsg;
    return StatusCode::SUCCESS;
  }
  if (!evtStore()->contains<TrackCollection>("ExtendedTracks")) {
    if(m_events == 1) {if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to get ExtendedTracks TrackCollection" << endmsg;}
    else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get ExtendedTracks TrackCollection - histograms will not be filled" << endmsg;
    return StatusCode::SUCCESS;
  }


  //tracks that are fitted to Si hits only, before extension to TRT
  DataVector<Trk::Track>* tracksSi = m_trackSelection->selectTracks("ResolvedTracks");
  if(!tracksSi) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Histograms not filled because TrackSelectionTool returned NULL track collection"<< endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Retrieved "<< tracksSi->size() <<" ResolvedTracks tracks from StoreGate" << endmsg;
    if(tracksSi->size()==0) msg(MSG::DEBUG) << "Histograms will not be filled because 0 tracks in ResolvedTracks (track collection probably doesn't exist)" << endmsg;
  }

  //tracks that are fitted with Si and TRT hits
  DataVector<Trk::Track>* tracksTRT = m_trackSelection->selectTracks("ExtendedTracks");
  if(!tracksTRT) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Histograms not filled because TrackSelectionTool returned NULL track collection"<< endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Retrieved "<< tracksTRT->size() <<" ExtendedTracks tracks from StoreGate" << endmsg;
    if(tracksTRT->size()==0) msg(MSG::DEBUG) << "Histograms will not be filled because 0 tracks in ExtendedTracks (track collection probably doesn't exist)" << endmsg;
  }

  int nTracksTRT = 0;

  DataVector<Trk::Track>::const_iterator trackItr  = tracksTRT->begin();
  DataVector<Trk::Track>::const_iterator trackItrE = tracksTRT->end();
  for (; trackItr != trackItrE; ++trackItr) { //looping over extended tracks


    const Trk::Track* trackTRT = *trackItr;
    if(trackTRT == NULL){
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No associated Trk::Track object found for track "<< nTracksTRT << endmsg;
      continue;
    }

    const Trk::TrackSummary* summary = NULL;
    summary = m_trackSumTool->createSummary(*trackTRT);
    int nhpix=0, nhsct=0, nhtrt=0;

    if ( summary != NULL ){
      nhpix = summary->get(Trk::numberOfPixelHits);
      nhsct = summary->get(Trk::numberOfSCTHits);
      nhtrt = summary->get(Trk::numberOfTRTHits);
      delete summary;
    }


    const Trk::Perigee* TRTPerigee =  trackTRT->perigeeParameters();

    if (!TRTPerigee)
      msg(MSG::WARNING) << "TRTPerigee is NULL. Track Information may be missing"<<endmsg;


    const AmgSymMatrix(5)* TRTPerCovariance = TRTPerigee ? TRTPerigee->covariance() : NULL;

    if ( TRTPerCovariance == 0 )
      msg(MSG::WARNING) << " failed dynamic_cast TRT track perigee to measured perigee, some parameters may be missing" << endmsg;

    double d0 = -999;
    double phi0 = -999;
    double qoverp = -999;
    double eta0 = -999;
    double z0 = -999;
    double charge = -999.;
    double pt = -999;

    if (TRTPerigee)
      {

	d0 = TRTPerigee->parameters()[Trk::d0];
	phi0 = TRTPerigee->parameters()[Trk::phi0];
	qoverp = TRTPerigee->parameters()[Trk::qOverP]*1000;
	eta0 = TRTPerigee->eta();
	z0 = TRTPerigee->parameters()[Trk::z0];
	charge = TRTPerigee->charge();
	if ( TRTPerCovariance != 0 )  pt = TRTPerigee->pT()/1000.;
      }


    //trackStateOnSurfaces is a vector of Trk::TrackStateOnSurface objects which contain information
    //on track at each (inner)detector surface it crosses eg hit used to fit track
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT Track = " << nTracksTRT << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT Track Eta = " << eta0 << ", phi = " << phi0 << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRT Track nhitspix = " << nhpix << ", sct = " << nhsct << ", trt = " << nhtrt << endmsg;


    m_nhitstrt->Fill(nhtrt);

    //tracks with eta > 2.1 do not intersect TRT and
    //thus aren't considered for TRT-Si matching efficiency plots
    if(fabs(eta0) > 2.1) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ExtendedTracks track is outside eta < 2.1 - skipped" << endmsg;
      continue;
    }

    //filling histos for tracks which may or may not have TRT hits
    m_alltrks_phi0->Fill(phi0);
    m_alltrks_eta0->Fill(eta0);
    if(fabs(eta0) < 1.0) m_alltrks_phi0_b->Fill(phi0);
    if(eta0 > 1.0) m_alltrks_phi0_eca->Fill(phi0);
    if(eta0 < -1.0) m_alltrks_phi0_ecc->Fill(phi0);

    // we only examine the delta Sionly-TRTextended track quantities for
    // tracks in ExtendedTracks which actually have some TRT hits associated
    // i.e. all tracks that are in ResolvedTracks are in ExtendedTracks too, most
    // without TRT hits
    if(nhtrt==0) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ExtendedTracks track has zero TRT hits" << endmsg;
      //filling histos for tracks which do not have TRT hits (fitted with Si only)
      m_si_phi0->Fill(phi0);
      m_si_eta0->Fill(eta0);
      continue;
    }

    //filling histos for tracks which have TRT hits
    m_trt_phi0->Fill(phi0);
    m_trt_eta0->Fill(eta0);
    if(fabs(eta0) < 1.0) m_trt_phi0_b->Fill(phi0);
    if(eta0 > 1.0) m_trt_phi0_eca->Fill(phi0);
    if(eta0 < -1.0) m_trt_phi0_ecc->Fill(phi0);

    double mindR = 10000;
    //double Xtheta = -999999;
    double Xd0 = -999999;
    double Xphi0 = -999999;
    double Xqoverp = -999999;
    double Xeta0 = -999999;
    double Xz0 = -999999;
    double Xcharge = -999999;
    double Xpt = -999999;
    bool matchFound = false;

    int nTracksSi = 0;
    DataVector<Trk::Track>::const_iterator trackItr2  = tracksSi->begin();
    DataVector<Trk::Track>::const_iterator trackItrE2 = tracksSi->end();
    for (; trackItr2 != trackItrE2; ++trackItr2) { //looping over Sionly tracks

      const Trk::Track* trackSi = *trackItr2;
      if(trackSi == NULL){
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No associated Trk::Track object found for track "<< nTracksSi << endmsg;
      continue;
    }


      const Trk::Perigee* SiPerigee = trackSi->perigeeParameters();

      if (!SiPerigee)
	msg(MSG::WARNING) << " SiPerigee is NULL. Track information may be missing"<<endmsg;

      const AmgSymMatrix(5)* SiPerCovariance = SiPerigee ? SiPerigee->covariance() : NULL;

      if ( SiPerCovariance == 0 )
	msg(MSG::WARNING) << " failed dynamic_cast Si track perigee to measured perigee, some parameters may be missing" << endmsg;

      double Siphi0 = -9999;
      double Sieta0 = -9999;
      double dphi2  = -9999;
      double deta2  = -9999;
      double dR     = -9999;


      if (SiPerigee)
	{
	  Siphi0 = SiPerigee->parameters()[Trk::phi0];
	  Sieta0 = SiPerigee->eta();
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Si Track = " << nTracksSi << endmsg;
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Si Track Eta = " << Sieta0 << ", phi = " << Siphi0 << endmsg;
	  //selecting Sionly track that is closest to TRT in eta-phi
	  dphi2 = (phi0 - Siphi0)*(phi0 - Siphi0);
	  deta2 = (eta0 - Sieta0)*(eta0 - Sieta0);
	  dR = sqrt(dphi2 + deta2);
	  if(dR < mindR){
	    mindR = dR;
	    //Xtheta = SiPerigee->parameters()[Trk::theta];
	    Xd0 = SiPerigee->parameters()[Trk::d0];
	    Xphi0 = SiPerigee->parameters()[Trk::phi0];
	    Xqoverp = SiPerigee->parameters()[Trk::qOverP]*1000;
	    Xeta0 = SiPerigee->eta();
	    Xz0 = SiPerigee->parameters()[Trk::z0];
	    Xcharge = SiPerigee->charge();
	    //Xpt = (1/Xqoverp)*Xcharge*sin(Xtheta);
	    if ( SiPerCovariance != 0 ) Xpt = SiPerigee->pT()/1000.;
	    if(dR < m_matchdRcut) matchFound = true;
	  }
	}
      nTracksSi++;
    }

    //filling for closest Si-only match in dR (before dR cut)
    m_delta_phi0->Fill(phi0 - Xphi0);
    m_delta_eta0->Fill(eta0 - Xeta0);
    m_delta_R->Fill(mindR);
    if(fabs(eta0) < 0.8) {
      m_delta_phi0_b->Fill(phi0 - Xphi0);
      m_delta_eta0_b->Fill(eta0 - Xeta0);
      m_delta_R_b->Fill(mindR);
    }

    //filling histos for tracks with TRT hits which also match
    //with a silicon track in dR
    if(matchFound){

      m_delta_qoverp->Fill(qoverp - Xqoverp);
      m_delta_d0->Fill(d0 - Xd0);
      m_delta_z0->Fill(z0 - Xz0);
      m_delta_charge->Fill(charge - Xcharge);
      m_delta_pt->Fill(pt - Xpt);
      m_reldelta_pt->Fill((pt - Xpt)/Xpt);

      m_matched_phi0->Fill(phi0);
      m_matched_eta0->Fill(eta0);

      if(fabs(eta0) < 0.8) {

	//filling for tracks whose TRT-defined eta is within TRT barrel
	//helps us to understand if it is a endcap/barrel only relative misalignment
	m_delta_qoverp_b->Fill(qoverp - Xqoverp);
	m_delta_d0_b->Fill(d0 - Xd0);
	m_delta_z0_b->Fill(z0 - Xz0);
	m_delta_charge_b->Fill(charge - Xcharge);
	m_delta_pt_b->Fill(pt - Xpt);
	m_reldelta_pt_b->Fill((pt - Xpt)/Xpt);
      }
    }

    nTracksTRT++;

  } // end of loop on tracks

  delete tracksSi;
  delete tracksTRT;


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonSivsTRT::procHistograms()
{

  if( endOfLowStatFlag() || endOfLumiBlockFlag() ) {

    //StatusCode sc = fillEfficiencyHisto(m_matched_eta0,m_trt_eta0,m_sieff_eta0);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
    //sc = fillEfficiencyHisto(m_matched_phi0,m_trt_phi0,m_sieff_phi0);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
    //sc = fillEfficiencyHisto(m_trt_eta0,m_alltrks_eta0,m_trteff_eta0);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
    //sc = fillEfficiencyHisto(m_trt_phi0,m_alltrks_phi0,m_trteff_phi0);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
    //sc = fillEfficiencyHisto(m_trt_phi0_b,m_alltrks_phi0_b,m_trteff_phi0_b);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
    //sc = fillEfficiencyHisto(m_trt_phi0_eca,m_alltrks_phi0_eca,m_trteff_phi0_eca);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
    //sc = fillEfficiencyHisto(m_trt_phi0_ecc,m_alltrks_phi0_ecc,m_trteff_phi0_ecc);
    //if (sc.isFailure()) return StatusCode::SUCCESS;
  }

  if( endOfRunFlag() ) {

    StatusCode sc = fillEfficiencyProfileHisto(m_matched_eta0,m_trt_eta0,m_sieff_eta0);
    if (sc.isFailure()) return StatusCode::SUCCESS;
    sc = fillEfficiencyProfileHisto(m_matched_phi0,m_trt_phi0,m_sieff_phi0);
    if (sc.isFailure()) return StatusCode::SUCCESS;
    sc = fillEfficiencyProfileHisto(m_trt_eta0,m_alltrks_eta0,m_trteff_eta0);
    if (sc.isFailure()) return StatusCode::SUCCESS;
    sc = fillEfficiencyProfileHisto(m_trt_phi0,m_alltrks_phi0,m_trteff_phi0);
    if (sc.isFailure()) return StatusCode::SUCCESS;
    sc = fillEfficiencyProfileHisto(m_trt_phi0_b,m_alltrks_phi0_b,m_trteff_phi0_b);
    if (sc.isFailure()) return StatusCode::SUCCESS;
    sc = fillEfficiencyProfileHisto(m_trt_phi0_eca,m_alltrks_phi0_eca,m_trteff_phi0_eca);
    if (sc.isFailure()) return StatusCode::SUCCESS;
    sc = fillEfficiencyProfileHisto(m_trt_phi0_ecc,m_alltrks_phi0_ecc,m_trteff_phi0_ecc);
    if (sc.isFailure()) return StatusCode::SUCCESS;

  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonSivsTRT::fillEfficiencyProfileHisto(TH1* h_num, TH1* h_denom, TProfile* h_eff) {

  //copied straight from IDAlignMonElectrons::makeEffHisto
  //means that efficiency histograms are properly combined when merge histos from multiple job segments

  int Nbins;
  Nbins = h_num->GetNbinsX();
  for (int bin=0; bin!=Nbins; ++bin) {
    int Npass = int(h_num->GetBinContent(bin+1));
    int Nfail = int(h_denom->GetBinContent(bin+1)) - Npass;//because denominator is total Nevents
    double x = h_denom->GetBinCenter(bin+1);
    for (int pass=0; pass!=Npass; ++pass) {
      h_eff->Fill(x,1.);
    }
    for (int fail=0; fail!=Nfail; ++fail) {
      h_eff->Fill(x,0.);
    }
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonSivsTRT::fillEfficiencyHisto(TH1* num, TH1* den, TH1* eff){

  if(num->GetNbinsX()!=den->GetNbinsX() || num->GetNbinsX()!=eff->GetNbinsX()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of bins mismatch" << endmsg;
    return StatusCode::SUCCESS;
  }

  for (int i=0;i<=den->GetNbinsX()+1;i++){
    float e = 0.0;
    if(den->GetBinContent(i)>0){
      e = num->GetBinContent(i)/den->GetBinContent(i);
    }
    eff->SetBinContent(i,e);
  }

  return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonSivsTRT::setupTools()
{
  //initializing tools

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setupTools()" << endmsg;

  if ( evtStore().retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve service " << evtStore() << endmsg;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved service " << evtStore() << endmsg;

  if (m_trackSelection.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Can not retrieve TrackSelection tool of type "
	<< m_trackSelection.typeAndName() << endmsg;
    return StatusCode::FAILURE;
  } else msg(MSG::INFO) << "Retrieved tool " << m_trackSelection.typeAndName() << endmsg;

  // get TrackSummaryTool
  if ( m_trackSumTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSumTool << endmsg;
    return StatusCode::FAILURE;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSumTool << endmsg;
  }


  return StatusCode::SUCCESS;
}
