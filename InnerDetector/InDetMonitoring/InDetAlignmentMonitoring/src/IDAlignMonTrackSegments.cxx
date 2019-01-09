/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonTrackSegments.cxx
// AUTHORS: John Alison (john.alison@cern.ch) johnda
//          Ben Cooper
//          Tobias Golling
// **********************************************************************

#include "TMath.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "TProfile.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "IDAlignMonTrackSegments.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/TrackCollection.h"
//#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "InDetTrackSplitterTool/IInDetTrackSplitterTool.h"
#include "TrackSelectionTool.h"
#include <iostream>


struct IDAlignMonTrackSegments::UpperLowerHistograms{

  TH1F_LW* nhitstrt;
  TH1F_LW* nhitsSi;
  TH1F_LW* nhitsPix;
  TH1F_LW* nhitsSct;
  TH1F_LW* phi0;
  TH1F_LW* eta0;
  TH1F_LW* qOverPt;
  TH1F_LW* charge;
  TH1F_LW* d0;
  TH1F_LW* z0;
  TH1F_LW* pt;
  UpperLowerHistograms() : nhitstrt(0), nhitsSi(0), nhitsPix(0), nhitsSct(0), phi0(0), eta0(0), qOverPt(0), charge(0), d0(0), z0(0), pt(0) {};

};

struct IDAlignMonTrackSegments::DeltaTrackParamHistograms{

  // All Tracks, Positive Tracks, Negative Tracks
  TH1F_LW* dTp[3];
  TH1F_LW* dTpPull[3];
  TH2F_LW* VsD0[3];
  TH2F_LW* VsZ0[3];
  TH2F_LW* VsPhi0[3];
  TH2F_LW* VsPt[3];
  TH2F_LW* VsEta[3];

  DeltaTrackParamHistograms(){

    for(unsigned int charge=0; charge<3; ++charge){
      dTp[charge] = 0;
      dTpPull[charge] = 0;
      VsD0[charge] = 0;
      VsZ0[charge] = 0;
      VsPhi0[charge] = 0;
      VsPt[charge] = 0;
      VsEta[charge] = 0;
    }

  };


};


// *********************************************************************
// Public Methods
// *********************************************************************
IDAlignMonTrackSegments::IDAlignMonTrackSegments( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   //m_trackSumTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
   m_trackSplitter(""),
   m_useCTBSplitTracks(true),
   m_deltaD0Range(0.25),
   m_deltaD0Range2D(1.0),
   m_deltaPhiRange(0.005),
   m_deltaPhiRange2D(0.02),
   m_deltaQoverPtRange(0.1),
   m_deltaQoverPtRange2D(0.4),
   m_deltaPtoverPtRange(0.5),
   m_deltaPtoverPtRange2D(0.5),
   m_deltaZ0Range(0.6),
   m_d0Range(150),
   m_upperPhi(0),
   m_triggerChainName("NoTriggerSelection"),
   m_upper_hist(new UpperLowerHistograms),
   m_lower_hist(new UpperLowerHistograms),
   m_delta_d0(new DeltaTrackParamHistograms),
   m_delta_z0(new DeltaTrackParamHistograms),
   m_delta_phi0(new DeltaTrackParamHistograms),
   m_delta_eta0(new DeltaTrackParamHistograms),
   m_delta_qOverPt(new DeltaTrackParamHistograms),
   m_delta_PtqOverPt(new DeltaTrackParamHistograms),
   m_delta_nHits(new DeltaTrackParamHistograms),
   m_delta_charge(new DeltaTrackParamHistograms)
{
  m_trackSelectionUpper = ToolHandle<InDetAlignMon::TrackSelectionTool>("InDetAlignMon::TrackSelectionTool");
  m_trackSelectionLower = ToolHandle<InDetAlignMon::TrackSelectionTool>("InDetAlignMon::TrackSelectionTool");
  m_trackSumTool        = ToolHandle<Trk::ITrackSummaryTool>("Trk::TrackSummaryTool/InDetTrackSummaryTool");


  InitializeHistograms();



  declareProperty("triggerChainName"       , m_triggerChainName);
  declareProperty("MatchedRCut", m_matchedRcut = 0.2);
  declareProperty("UseCTBSplitTracks", m_useCTBSplitTracks);
  declareProperty("TrackSplitter", m_trackSplitter);
  declareProperty("InputTracksName", m_inputTracksName =  "InDetCosmic_Tracks");
  declareProperty("UpperTracksName", m_upperTracksName =  "InDetCosmic_Tracks_Upper");
  declareProperty("LowerTracksName", m_lowerTracksName =  "InDetCosmic_Tracks_Lower");
  declareProperty("trackSelectionUp", m_trackSelectionUpper);
  declareProperty("trackSelectionLow", m_trackSelectionLower);
  declareProperty("DeltaD0Range", m_deltaD0Range);
  declareProperty("DeltaD0Range2D", m_deltaD0Range2D);
  declareProperty("DeltaPhiRange", m_deltaPhiRange);
  declareProperty("DeltaPhiRange2D", m_deltaPhiRange2D);
  declareProperty("DeltaQoverPtRange", m_deltaQoverPtRange);
  declareProperty("DeltaQoverPtRange2D", m_deltaQoverPtRange2D);
  declareProperty("DeltaPtoverPtRange", m_deltaPtoverPtRange);
  declareProperty("DeltaPtoverPtRange2D", m_deltaPtoverPtRange2D);
  declareProperty("DeltaZ0Range", m_deltaZ0Range);
  declareProperty("D0Range", m_d0Range);
  declareProperty("UpperPhi", m_upperPhi);
  declareProperty("trackSumTool", m_trackSumTool);

  m_histosBooked = 0;
}

//---------------------------------------------------------------------------------------

IDAlignMonTrackSegments::~IDAlignMonTrackSegments(){
  delete m_upper_hist;
  delete m_lower_hist;
  delete m_delta_d0;
  delete m_delta_z0;
  delete m_delta_phi0;
  delete m_delta_eta0;
  delete m_delta_qOverPt;
  delete m_delta_PtqOverPt;
  delete m_delta_nHits;
  delete m_delta_charge;
}

void IDAlignMonTrackSegments::InitializeHistograms()
{

  m_debug_phi0 = 0 ;
  m_debug_eta0 = 0;

  // pt
  m_delta_pt = 0 ;
  m_reldelta_pt = 0 ;

}



//---------------------------------------------------------------------------------------

StatusCode IDAlignMonTrackSegments::bookHistograms()
{
  m_events=0;

  //initialize tools and services
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling initialize() to setup tools/services" << endmsg;
  StatusCode sc = setupTools();
  if (sc.isFailure()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Failed to initialize tools/services!" << endmsg;
    return StatusCode::SUCCESS;
  }
  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Successfully initialized tools/services" << endmsg;


  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  std::string outputDirName = "IDAlignMon/" + m_inputTracksName + "_" + m_triggerChainName + "/TrackSegments";
  if(m_useCTBSplitTracks)
    outputDirName = "IDAlignMon/" + m_upperTracksName + "_" + m_lowerTracksName + "_" + m_triggerChainName + "/TrackSegments";

  MonGroup al_mon ( this, outputDirName, run );


  if ( newLowStatFlag() || newLumiBlockFlag() ) {
  }

  if( newRunFlag() ) {

    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    //Upper Tracks
    m_upper_hist->nhitstrt = TH1F_LW::create("nhits_trtUp","Number of TRT hits for every track in Upper Track Collection",101,-0.5,100.5);
    RegisterHisto(al_mon,m_upper_hist->nhitstrt);
    m_upper_hist->nhitsSi = TH1F_LW::create("nhits_siUp","Number of SI hits for every track in Upper Track Collection",26,-0.5,25.5);
    RegisterHisto(al_mon,m_upper_hist->nhitsSi);
    m_upper_hist->nhitsPix = TH1F_LW::create("nhits_pixUp","Number of PIX hits for every track in Upper Track Collection",11,-0.5,10.5);
    RegisterHisto(al_mon,m_upper_hist->nhitsPix);
    m_upper_hist->nhitsSct = TH1F_LW::create("nhits_sctUp","Number of SCT hits for every track in Upper Track Collection",21,-0.5,20.5);
    RegisterHisto(al_mon,m_upper_hist->nhitsSct);
    m_upper_hist->phi0 = TH1F_LW::create("Upper_phi0","#phi_{0}^{Upper}", 60,-3.14,m_upperPhi);
    RegisterHisto(al_mon,m_upper_hist->phi0);
    m_upper_hist->eta0 = TH1F_LW::create("Upper_eta0","#eta_{0}^{Upper}",100,-2.1,2.1);
    RegisterHisto(al_mon,m_upper_hist->eta0);
    m_upper_hist->d0 = TH1F_LW::create("Upper_d0","d_{0}^{Upper}; d_{0} [mm]", 80,-1*m_d0Range,m_d0Range);
    RegisterHisto(al_mon,m_upper_hist->d0);
    m_upper_hist->z0 = TH1F_LW::create("Upper_z0","z_{0}^{Upper}; z_{0} [mm]", 50, -500, 500);
    RegisterHisto(al_mon,m_upper_hist->z0);
    m_upper_hist->qOverPt = TH1F_LW::create("Upper_qOverPt","q/p_{T}^{Upper}; q/p_{T} [GeV^{-1}]",100,-0.05,0.05);
    RegisterHisto(al_mon,m_upper_hist->qOverPt);
    m_upper_hist->pt = TH1F_LW::create("Upper_pt","p_{T}^{Upper}; p_{T} [GeV]", 100, 0., 100.0);
    RegisterHisto(al_mon,m_upper_hist->pt);

    //Lower Tracks
    m_lower_hist->nhitstrt = TH1F_LW::create("nhits_trtLow","Number of TRT hits for every track in Lower Track Collection",101,-0.5,100.5);
    RegisterHisto(al_mon,m_lower_hist->nhitstrt);
    m_lower_hist->nhitsSi = TH1F_LW::create("nhits_siLow","Number of SI hits for every track in Lower Track Collection",26,-0.5,25.5);
    RegisterHisto(al_mon,m_lower_hist->nhitsSi);
    m_lower_hist->nhitsPix = TH1F_LW::create("nhits_pixLow","Number of PIX hits for every track in Lower Track Collection",11,-0.5,10.5);
    RegisterHisto(al_mon,m_lower_hist->nhitsPix);
    m_lower_hist->nhitsSct = TH1F_LW::create("nhits_sctLow","Number of SCT hits for every track in Lower Track Collection",21,-0.5,20.5);
    RegisterHisto(al_mon,m_lower_hist->nhitsSct);
    m_lower_hist->phi0 = TH1F_LW::create("Lower_phi0","#phi_{0}^{Low} ", 60,-3.14,m_upperPhi);
    RegisterHisto(al_mon,m_lower_hist->phi0);
    m_lower_hist->eta0 = TH1F_LW::create("Lower_eta0","#eta_{0}^{Low} ",100,-2.1,2.1);
    RegisterHisto(al_mon,m_lower_hist->eta0);
    m_lower_hist->d0 = TH1F_LW::create("Lower_d0","d_{0}^{Low}; d_{0} [mm]", 80,-1*m_d0Range,m_d0Range);
    RegisterHisto(al_mon,m_lower_hist->d0);
    m_lower_hist->z0 = TH1F_LW::create("Lower_z0","z_{0}^{Low}; z_{0} [mm]", 50, -500, 500);
    RegisterHisto(al_mon,m_lower_hist->z0);
    m_lower_hist->qOverPt = TH1F_LW::create("Lower_qOverPt","q/p_{T}^{Low}; q/p_{T} [GeV^{-1}]",100,-0.05,0.05);
    RegisterHisto(al_mon,m_lower_hist->qOverPt);
    m_lower_hist->pt = TH1F_LW::create("Lower_pt","p_{T}^{Low}; p_{T} [GeV]", 100, 0.,100.0);
    RegisterHisto(al_mon,m_lower_hist->pt);



    //  differences in track parameters
    //==================================
    std::string chargeNames[3] = {""," (Positive Tracks)"," (Negative Tracks)"};
    std::string histNames[3] = {"","_p","_n"};

    for(unsigned int charge=0; charge<3; ++charge){
      //======== d0 =========
      m_delta_d0->dTp[charge] = MakeHist("delta_d0"+histNames[charge]
					 ,"d_{0}^{Low} - d_{0}^{Upper}"+chargeNames[charge]+"; #Delta d_{0} [mm]"
					 , 50, -m_deltaD0Range, m_deltaD0Range);
      RegisterHisto(al_mon,m_delta_d0->dTp[charge]);

      m_delta_d0->dTpPull[charge] = MakeHist("delta_d0_Pull"+histNames[charge]
					     ,"d_{0}^{Low} - d_{0}^{Upper} / #sigma_{d_{0}}"+chargeNames[charge]+"; #Delta d_{0} pull"
					     ,100,-3,3);
      RegisterHisto(al_mon,m_delta_d0->dTpPull[charge]);

      m_delta_d0->VsD0[charge] = MakeHist("delta_d0VsD0"+histNames[charge]
					  ,"d_{0}^{Low} - d_{0}^{Upper} Vs d_{0}^{Upper}"+chargeNames[charge]+"; d_{0}^{Upper} [mm]; #Delta d_{0} [mm]"
					  , 9,-m_d0Range, m_d0Range,50,-1*m_deltaD0Range2D,m_deltaD0Range2D);
      RegisterHisto(al_mon,m_delta_d0->VsD0[charge]);

      m_delta_d0->VsPhi0[charge] = MakeHist("delta_d0VsPhi0"+histNames[charge]
					    ,"d_{0}^{Low} - d_{0}^{Upper} Vs #phi_{0}^{Upper}"+chargeNames[charge]+"; #phi_{0}^{Upper} [mm]; #Delta d_{0} [mm]"
					    ,9,-3.14,m_upperPhi,50,-1*m_deltaD0Range2D,m_deltaD0Range2D);
      RegisterHisto(al_mon,m_delta_d0->VsPhi0[charge]);

      m_delta_d0->VsPt[charge] = MakeHist("delta_d0VsPt"+histNames[charge]
					  ,"d_{0}^{Low} - d_{0}^{Upper} Vs p_{T}^{Upper}"+chargeNames[charge]+"; p_{T}^{Upper} [GeV]; #Delta d_{0} [mm]"
					  ,10,0,100,50,-1*m_deltaD0Range2D,m_deltaD0Range2D);
      RegisterHisto(al_mon,m_delta_d0->VsPt[charge]);

      m_delta_d0->VsEta[charge] = MakeHist("delta_d0VsEta"+histNames[charge]
					   ,"d_{0}^{Low} - d_{0}^{Upper} Vs #eta^{Upper}"+chargeNames[charge]+"; #eta^{Upper}; #Delta d_{0} [mm]"
					   ,11,-2.1,2.1,50,-1*m_deltaD0Range2D,m_deltaD0Range2D);
      RegisterHisto(al_mon,m_delta_d0->VsEta[charge]);

      m_delta_d0->VsZ0[charge] = MakeHist("delta_d0VsZ0"+histNames[charge]
					  ,"d_{0}^{Low} - d_{0}^{Upper} Vs z_{0}^{Upper}"+chargeNames[charge]+"; #z_{0}^{Upper} [mm]; #Delta d_{0} [mm]"
					  ,11, -500., 500., 50,-1*m_deltaD0Range2D,m_deltaD0Range2D);
      RegisterHisto(al_mon,m_delta_d0->VsZ0[charge]);

      //========= z0
      m_delta_z0->dTp[charge] = MakeHist("delta_z0"+histNames[charge]
					 ,"z_{0}^{Upper} - z_{0}^{Low}"+chargeNames[charge]+"; #Delta z_{0} [mm]"
					 ,50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->dTp[charge]);

      m_delta_z0->dTpPull[charge] = MakeHist("delta_z0_Pull"+histNames[charge]
					     ,"z_{0}^{Upper} - z_{0}^{Low} / #sigma_{z_{0}}"+chargeNames[charge]+"; #Delta z_{0} pull"
					     ,50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->dTpPull[charge]);

      m_delta_z0->VsD0[charge] = MakeHist("delta_z0VsD0"+histNames[charge]
					  ,"z_{0}^{Upper} - z_{0}^{low} Vs d_{0}^{Upper}"+chargeNames[charge]+"; z_{0}^{Upper} [mm]; #Delta z_{0} [mm]"
					  ,9,-1*m_d0Range,m_d0Range,50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->VsD0[charge]);

      m_delta_z0->VsPhi0[charge] = MakeHist("delta_z0VsPhi0"+histNames[charge]
					    ,"z_{0}^{Upper} - z_{0}^{Low} Vs #phi_{0}^{Upper}"+chargeNames[charge]
					    ,9,-3.14,m_upperPhi, 50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->VsPhi0[charge]);

      m_delta_z0->VsPt[charge] = MakeHist("delta_z0VsPt"+histNames[charge]
					  ,"z_{0}^{Upper} - z_{0}^{Low} Vs p_{T}^{Upper}"+chargeNames[charge]
					  ,10,0,100,50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->VsPt[charge]);

      m_delta_z0->VsEta[charge] = MakeHist("delta_z0VsEta"+histNames[charge]
					   ,"z_{0}^{Upper} - z_{0}^{Low} Vs #eta^{Upper}"+chargeNames[charge]
					   ,11,-2.1,2.1, 50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->VsEta[charge]);

      m_delta_z0->VsZ0[charge] = MakeHist("delta_z0VsZ0"+histNames[charge]
					  ,"z_{0}^{Upper} - z_{0}^{Low} Vs z_{0}^{Upper}"+chargeNames[charge]
					  ,11,-500., 500., 50, -m_deltaZ0Range, m_deltaZ0Range);
      RegisterHisto(al_mon,m_delta_z0->VsZ0[charge]);

      //====== phi0 ========
      m_delta_phi0->dTp[charge] = MakeHist("delta_phi0"+histNames[charge]
					   ,"#phi_{0}^{Low} - #phi_{0}^{Upper}"+chargeNames[charge]
					   ,100,-1*m_deltaPhiRange,m_deltaPhiRange);
      RegisterHisto(al_mon,m_delta_phi0->dTp[charge]);

      m_delta_phi0->dTpPull[charge] = MakeHist("delta_phi0_Pull"+histNames[charge]
					     ,"phi_{0}^{Low} - phi_{0}^{Upper} / #sigma_{phi_{0}}"+chargeNames[charge]
					     ,100,-3,3);
      RegisterHisto(al_mon,m_delta_phi0->dTpPull[charge]);


      m_delta_phi0->VsD0[charge] = MakeHist("delta_phi0VsD0"+histNames[charge]
					    ,"#phi_{0}^{Low} - #phi_{0}^{Upper} Vs D0^{Upper}"+chargeNames[charge]
					    ,9,-1*m_d0Range,m_d0Range,50,-1*m_deltaPhiRange2D,m_deltaPhiRange2D);
      RegisterHisto(al_mon,m_delta_phi0->VsD0[charge]);

      m_delta_phi0->VsPhi0[charge] = MakeHist("delta_phi0VsPhi0"+histNames[charge]
					      ,"#phi_{0}^{Low} - #phi_{0}^{Upper} Vs #phi_0^{Upper}"+chargeNames[charge]
					      ,9,-3.14,m_upperPhi,50,-1*m_deltaPhiRange2D,m_deltaPhiRange2D);
      RegisterHisto(al_mon,m_delta_phi0->VsPhi0[charge]);

      m_delta_phi0->VsPt[charge] = MakeHist("delta_phi0VsPt"+histNames[charge]
					    ,"#phi_{0}^{Low} - #phi_{0}^{Upper} Vs p_{T}^{Upper}"+chargeNames[charge]
					    ,11,0,100,50,-1*m_deltaPhiRange2D,m_deltaPhiRange2D);
      RegisterHisto(al_mon,m_delta_phi0->VsPt[charge]);

      m_delta_phi0->VsEta[charge] = MakeHist("delta_phi0VsEta"+histNames[charge]
					     ,"#phi_{0}^{Low} - #phi_{0}^{Upper} Vs #eta^{Upper}"+chargeNames[charge]
					     ,11,-2.1,2.1,50,-1*m_deltaPhiRange2D,m_deltaPhiRange2D);
      RegisterHisto(al_mon,m_delta_phi0->VsEta[charge]);

      m_delta_phi0->VsZ0[charge] = MakeHist("delta_phi0VsZ0"+histNames[charge]
					    ,"#phi_{0}^{Low} - #phi_{0}^{Upper} Vs Z0^{Upper}"+chargeNames[charge]
					    ,11, -500., 500.,50,-1*m_deltaPhiRange2D,m_deltaPhiRange2D);
      RegisterHisto(al_mon,m_delta_phi0->VsZ0[charge]);

      //======= eta0 =======
      m_delta_eta0->dTp[charge] = MakeHist("delta_eta0"+histNames[charge]
					   ,"#eta_{0}^{Upper} - #eta_{0}^{Low}"+chargeNames[charge]
					   ,100, -0.005, 0.005);
      RegisterHisto(al_mon,m_delta_eta0->dTp[charge]);

      m_delta_eta0->dTpPull[charge] = MakeHist("delta_eta0_Pull"+histNames[charge]
					     ,"eta_{0}^{Upper} - eta_{0}^{Low} / #sigma_{eta_{0}}"+chargeNames[charge]
					     ,100,-4,4);
      RegisterHisto(al_mon,m_delta_eta0->dTpPull[charge]);

      m_delta_eta0->VsD0[charge] = MakeHist("delta_eta0VsD0"+histNames[charge]
					    ,"#eta_{0}^{Upper} - #eta_{0}^{Low} Vs D0^{Upper}"+chargeNames[charge]
					    ,9,-1*m_d0Range,m_d0Range,50,-0.02, 0.02);
      RegisterHisto(al_mon,m_delta_eta0->VsD0[charge]);

      m_delta_eta0->VsPhi0[charge] = MakeHist("delta_eta0VsPhi0"+histNames[charge]
					      ,"#eta_{0}^{Upper} - #eta_{0}^{low} Vs Phi0^{Upper}"+chargeNames[charge]
					      ,9,-3.14,m_upperPhi,50, -0.02, 0.02);
      RegisterHisto(al_mon,m_delta_eta0->VsPhi0[charge]);

      m_delta_eta0->VsPt[charge] = MakeHist("delta_eta0VsPt"+histNames[charge]
					    ,"#eta_{0}^{Upper} - #eta_{0}^{Low} Vs p_{T}^{Upper}"+chargeNames[charge]
					    ,11,0,100,50, -0.02, 0.02);
      RegisterHisto(al_mon,m_delta_eta0->VsPt[charge]);

      m_delta_eta0->VsEta[charge] = MakeHist("delta_eta0VsEta"+histNames[charge]
					     ,"#eta_{0}^{Upper} - #eta_{0}^{Low} Vs #eta_{0}^{Upper}"+chargeNames[charge]
					     ,51,-2.1,2.1,50, -0.02, 0.02);
      RegisterHisto(al_mon,m_delta_eta0->VsEta[charge]);

      m_delta_eta0->VsZ0[charge] = MakeHist("delta_eta0VsZ0"+histNames[charge]
					    ,"#eta_{0}^{Upper} - #eta_{0}^{Low} Vs Z0^{Upper}"+chargeNames[charge]
					    ,11,-500., 500.,50, -0.02, 0.02);
      RegisterHisto(al_mon,m_delta_eta0->VsZ0[charge]);

      //======= qOverPt
      m_delta_qOverPt->dTp[charge] = MakeHist("delta_qOverPt"+histNames[charge]
					      ,"Q/p_{T}^{Low} - Q/p_{T}^{Upper} (1/GeV)"+chargeNames[charge]
					      ,100,-1*m_deltaQoverPtRange,m_deltaQoverPtRange);
      RegisterHisto(al_mon,m_delta_qOverPt->dTp[charge]);

      m_delta_qOverPt->dTpPull[charge] = MakeHist("delta_qOverPt_Pull"+histNames[charge]
					     ,"qOverPt^{Low} - qOverPt^{Upper} / #sigma_{qOverPt}"+chargeNames[charge]
					     ,100,-3,3);
      RegisterHisto(al_mon,m_delta_qOverPt->dTpPull[charge]);

      m_delta_qOverPt->VsD0[charge] = MakeHist("delta_qOverPtVsD0"+histNames[charge]
					       ,"Q/p_{T}^{Low} - Q/p_{T}^{Upper} Vs d_{0}^{Upper}"+chargeNames[charge]
					       ,8,-1*m_d0Range,m_d0Range,50,-1*m_deltaQoverPtRange2D,m_deltaQoverPtRange2D);
      RegisterHisto(al_mon,m_delta_qOverPt->VsD0[charge]);

      m_delta_qOverPt->VsZ0[charge] = MakeHist("delta_qOverPtVsZ0"+histNames[charge]
					       ,"Q/p_{T}^{Low} - Q/p_{T}^{Upper} Vs z_{0}^{Upper}"+chargeNames[charge]
					       ,50,-1000,1000,50,-1*m_deltaQoverPtRange2D,m_deltaQoverPtRange2D);
      RegisterHisto(al_mon,m_delta_qOverPt->VsZ0[charge]);

      m_delta_qOverPt->VsPhi0[charge] = MakeHist("delta_qOverPtVsPhi0"+histNames[charge]
						 ,"Q/p_{T}^{Low} - Q/p_{T}^{Upper} (1/GeV) Vs #phi_{0}^{Upper}"+chargeNames[charge]
						 ,8,-3.14,m_upperPhi,50,-1*m_deltaQoverPtRange2D,m_deltaQoverPtRange2D);
      RegisterHisto(al_mon,m_delta_qOverPt->VsPhi0[charge]);

      m_delta_qOverPt->VsPt[charge] = MakeHist("delta_qOverPtVsPt"+histNames[charge]
					       ,"Q/p_{T}^{Low} - Q/p_{T}^{Upper} (1/GeV) Vs p_{T}^{Upper}"+chargeNames[charge]
					       ,10,0,100,50,-1*m_deltaQoverPtRange2D,m_deltaQoverPtRange2D);
      RegisterHisto(al_mon,m_delta_qOverPt->VsPt[charge]);

      m_delta_qOverPt->VsEta[charge] = MakeHist("delta_qOverPtVsEta"+histNames[charge]
						,"Q/p_{T}^{Low} - Q/p_{T}^{Upper} (1/GeV) Vs #eta^{Upper}"+chargeNames[charge]
						,50,-2.1,2.1,50,-1*m_deltaQoverPtRange2D,m_deltaQoverPtRange2D);
      RegisterHisto(al_mon,m_delta_qOverPt->VsEta[charge]);

      //======= PtqOverPt
      m_delta_PtqOverPt->dTp[charge] = MakeHist("delta_PtqOverPt"+histNames[charge]
					      ,"p_{T}^{Upper}.(Q/p_{T}^{Low} - Q/p_{T}^{Upper}) "+chargeNames[charge]
					      ,100,-1*m_deltaPtoverPtRange,m_deltaPtoverPtRange);
      RegisterHisto(al_mon,m_delta_PtqOverPt->dTp[charge]);

      m_delta_PtqOverPt->dTpPull[charge] = MakeHist("delta_PtqOverPt_Pull"+histNames[charge]
					     ,"p_{T}^{Upper}.(qOverPt^{Low} - qOverPt^{Upper} / #sigma_{qOverPt})"+chargeNames[charge]
					     ,100,-3,3);
      RegisterHisto(al_mon,m_delta_PtqOverPt->dTpPull[charge]);

      m_delta_PtqOverPt->VsD0[charge] = MakeHist("delta_PtqOverPtVsD0"+histNames[charge]
					       ,"p_{T}^{Upper}.(Q/p_{T}^{Low} - Q/p_{T}^{Upper}) Vs d_{0}^{Upper}"+chargeNames[charge]
					       ,8,-1*m_d0Range,m_d0Range,50,-1*m_deltaPtoverPtRange2D,m_deltaPtoverPtRange2D);
      RegisterHisto(al_mon,m_delta_PtqOverPt->VsD0[charge]);

      m_delta_PtqOverPt->VsZ0[charge] = MakeHist("delta_PtqOverPtVsZ0"+histNames[charge]
					       ,"p_{T}^{Upper}.(Q/p_{T}^{Low} - Q/p_{T}^{Upper}) Vs z_{0}^{Upper}"+chargeNames[charge]
					       ,50,-1000,1000,50,-1*m_deltaPtoverPtRange2D,m_deltaPtoverPtRange2D);
      RegisterHisto(al_mon,m_delta_PtqOverPt->VsZ0[charge]);

      m_delta_PtqOverPt->VsPhi0[charge] = MakeHist("delta_PtqOverPtVsPhi0"+histNames[charge]
						 ,"p_{T}^{Upper}.(Q/p_{T}^{Low} - Q/p_{T}^{Upper}) Vs #phi_{0}^{Upper}"+chargeNames[charge]
						 ,8,-3.14,m_upperPhi,50,-1*m_deltaPtoverPtRange2D,m_deltaPtoverPtRange2D);
      RegisterHisto(al_mon,m_delta_PtqOverPt->VsPhi0[charge]);

      m_delta_PtqOverPt->VsPt[charge] = MakeHist("delta_PtqOverPtVsPt"+histNames[charge]
					       ,"p_{T}^{Upper}.(Q/p_{T}^{Low} - Q/p_{T}^{Upper}) Vs p_{T}^{Upper}"+chargeNames[charge]
					       ,50,0,100,50,-1*m_deltaPtoverPtRange2D,m_deltaPtoverPtRange2D);
      RegisterHisto(al_mon,m_delta_PtqOverPt->VsPt[charge]);

      m_delta_PtqOverPt->VsEta[charge] = MakeHist("delta_PtqOverPtVsEta"+histNames[charge]
						,"p_{T}^{Upper}.(Q/p_{T}^{Low} - Q/p_{T}^{Upper}) Vs #eta^{Upper}"+chargeNames[charge]
						,50,-2.1,2.1,50,-1*m_deltaPtoverPtRange2D,m_deltaPtoverPtRange2D);
      RegisterHisto(al_mon,m_delta_PtqOverPt->VsEta[charge]);


      //========== nHits
      m_delta_nHits->dTp[charge] = MakeHist("delta_nHits"+histNames[charge]
					    ,"NHits^{upper} - NHits^{lower}"+chargeNames[charge], 61, -30.5, 30.5);
      RegisterHisto(al_mon,m_delta_nHits->dTp[charge]);

      m_delta_nHits->VsD0[charge] = MakeHist("delta_nHitsVsD0"+histNames[charge]
					     ,"NHits^{upper} - NHits^{lower} Vs d_{0}^{Upper}"+chargeNames[charge]
					     ,8,-1*m_d0Range,m_d0Range, 61, -30.5, 30.5);
      RegisterHisto(al_mon,m_delta_nHits->VsD0[charge]);

      m_delta_nHits->VsPhi0[charge] = MakeHist("delta_nHitsVsPhi0"+histNames[charge]
					       ,"NHits^{upper} - NHits^{lower} Vs #phi_{0}^{Upper}"+chargeNames[charge]
					       ,8,-3.14,m_upperPhi, 61, -30.5, 30.5);
      RegisterHisto(al_mon,m_delta_nHits->VsPhi0[charge]);

      m_delta_nHits->VsZ0[charge] = MakeHist("delta_nHitsVsZ0"+histNames[charge]
					     ,"NHits^{upper} - NHits^{lower} Vs z_{0}^{Upper}"+chargeNames[charge]
					     ,50,-1000,1000, 61, -30.5, 30.5);
      RegisterHisto(al_mon,m_delta_nHits->VsZ0[charge]);

      m_delta_nHits->VsPt[charge] = MakeHist("delta_nHitsVsPt"+histNames[charge]
					     ,"NHits^{upper} - NHits^{lower} Vs p_{T}^{Upper}"+chargeNames[charge]
					     ,10,0,100, 61, -30.5, 30.5);
      RegisterHisto(al_mon,m_delta_nHits->VsPt[charge]);

      m_delta_nHits->VsEta[charge] = MakeHist("delta_nHitsVsEta"+histNames[charge]
					      ,"NHits^{upper} - NHits^{lower} Vs #eta^{Upper}"+chargeNames[charge]
					      ,50,-2.1,2.1, 61, -30.5, 30.5);
      RegisterHisto(al_mon,m_delta_nHits->VsEta[charge]);

    }

    //========== charge
    m_delta_charge->dTp[0] = MakeHist("delta_charge"
				      ,"Charge^{upper} - Charge^{lower}; #Delta q"
				      ,5, -2.5, 2.5);
    RegisterHisto(al_mon,m_delta_charge->dTp[0]);

    m_delta_charge->VsD0[0] = MakeHist("delta_chargeVsD0"
				       ,"Charge^{upper} - Charge^{lower} Vs d_{0}^{Upper}; d_{0} [mm]; #Delta q"
				       ,8,-1*m_d0Range,m_d0Range, 5, -2.5, 2.5);
    RegisterHisto(al_mon,m_delta_charge->VsD0[0]);

    m_delta_charge->VsPhi0[0] = MakeHist("delta_chargeVsPhi0"
					 ,"Charge^{upper} - Charge^{lower} Vs #phi_{0}^{Upper}; #phi [rad]; #Delta q"
					 ,8,-3.14,m_upperPhi, 5, -2.5, 2.5);
    RegisterHisto(al_mon,m_delta_charge->VsPhi0[0]);

    m_delta_charge->VsZ0[0] = MakeHist("delta_chargeVsZ0"
				       ,"Charge^{upper} - Charge^{lower} Vs z_{0}^{Upper}; z_{0} [mm]; #Delta q"
				       ,50,-500,500, 5, -2.5, 2.5);
    RegisterHisto(al_mon,m_delta_charge->VsZ0[0]);

    m_delta_charge->VsPt[0] = MakeHist("delta_chargeVsPt"
				       ,"Charge^{upper} - Charge^{lower} Vs p_{T}^{Upper}; p_{T} [GeV]; #Delta q"
				       ,10,0,100, 5, -2.5, 2.5);
    RegisterHisto(al_mon,m_delta_charge->VsPt[0]);

    m_delta_charge->VsEta[0] = MakeHist("delta_chargeVsEta"
					,"Charge^{upper} - Charge^{lower} Vs #eta^{Upper}; #eta; #Delta q"
					,50,-2.1,2.1, 5,-2.5, 2.5);
    RegisterHisto(al_mon,m_delta_charge->VsEta[0]);

    //========== Debugging
    m_debug_phi0 = TH1F_LW::create("debug_phi0","#phi_{0} difference for the closest tracks (no match req.) ",10,-3.14,3.14);
    RegisterHisto(al_mon,m_debug_phi0);

    m_debug_eta0 = TH1F_LW::create("debug_eta0","#eta_{0} difference for the closest tracks (no match req.) ",10,-2.1,2.1);
    RegisterHisto(al_mon,m_debug_eta0);

    //========== Pt
    m_delta_pt = TH1F_LW::create("delta_pt","P_{T}^{Low} - P_{T}^{Upper}",100,-3.0,3.0);
    RegisterHisto(al_mon,m_delta_pt);
    m_reldelta_pt = TH1F_LW::create("reldelta_pt","P_{T}^{upper} - P_{T}^{lower}/P_{T}^{lower}",100,-1.0,1.0);
    RegisterHisto(al_mon,m_reldelta_pt);

    m_histosBooked++;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
// Register the histograms
//---------------------------------------------------------------------------------------
void IDAlignMonTrackSegments::RegisterHisto(MonGroup& mon, TH1F_LW* histo) {

  //histo->Sumw2();//TK: fixme
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cannot book TH1F_LW Histogram:" << endmsg;
  }
}

void IDAlignMonTrackSegments::RegisterHisto(MonGroup& mon, TH2F_LW* histo) {

  //histo->Sumw2();//TK: fixme
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cannot book TH2F_LW Histogram:" << endmsg;
  }
}


//---------------------------------------------------------------------------------------
// fill the histograms
//---------------------------------------------------------------------------------------
StatusCode IDAlignMonTrackSegments::fillHistograms()
{

  m_events++;
  //Get the track collections
  const DataVector<Trk::Track>* tracksUpper(0);
  const DataVector<Trk::Track>* tracksLower(0);


  if(m_useCTBSplitTracks){

    if(!evtStore()->contains<TrackCollection>(m_upperTracksName)){
      if(m_events == 1) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_upperTracksName << " TrackCollections" << endmsg;
      }else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_upperTracksName << " TrackCollections" << endmsg;
      return StatusCode::SUCCESS;
    }

    if (!evtStore()->contains<TrackCollection>(m_lowerTracksName)){
      if(m_events == 1) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_lowerTracksName << " TrackCollections" << endmsg;
      }else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_lowerTracksName << " TrackCollections" << endmsg;
      return StatusCode::SUCCESS;
    }

    tracksUpper = m_trackSelectionUpper->selectTracks(m_upperTracksName);
    if (!tracksUpper) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_upperTracksName<<" is NULL" << endmsg;
    }

    tracksLower = m_trackSelectionLower->selectTracks(m_lowerTracksName);
    if (!tracksLower) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_lowerTracksName<<" is NULL" << endmsg;
    }

  }else{

    //We only need the inputTracks if we're splitting them ourselves
    if (!evtStore()->contains<TrackCollection>(m_inputTracksName)){
      if(m_events == 1){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_inputTracksName << " TrackCollections" << endmsg;
      }else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_inputTracksName << " TrackCollections" << endmsg;
      return StatusCode::SUCCESS;
    }

    //Split the tracks.
    const DataVector<Trk::Track>* tracksIn;
    StatusCode sc = evtStore()->retrieve(tracksIn, m_inputTracksName);
    if (sc.isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<m_inputTracksName<<" found in StoreGate" << endmsg;

    }else{
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracksIn->size() <<" Input Tracks from StoreGate" << endmsg;

    }
    m_trackSplitter->splitTracks(tracksIn);

    //Get the Upper Tracks
    tracksUpper = m_trackSelectionUpper->selectTracks(m_upperTracksName);
    if (!tracksUpper) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_upperTracksName<<" is NULL" << endmsg;

    }

    //Get the Lower Tracks
    tracksLower = m_trackSelectionLower->selectTracks(m_lowerTracksName);
    if (!tracksLower) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_lowerTracksName<<" is NULL" << endmsg;

    }

  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracksUpper->size() <<" Upper Tracks." << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracksLower->size() <<" Lower Tracks from Track from StoreGate" << endmsg;

  //===============================================================
  // Filling the upper and lower tracks and their differences
  //===============================================================

  //looping over the upper barrel tracks
  int nTracksUpper = 0;

  DataVector<Trk::Track>::const_iterator trackItrUpper  = tracksUpper->begin();
  DataVector<Trk::Track>::const_iterator trackItrUpperE = tracksUpper->end();
  for (; trackItrUpper != trackItrUpperE; ++trackItrUpper) {
    const Trk::Track* trackUpper = *trackItrUpper;
    if(trackUpper == NULL){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No associated Trk::Track object found for track "<< nTracksUpper << endmsg;
      continue;
    }else
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Got the "<< nTracksUpper << " Upper Track" << endmsg;

    const Trk::TrackSummary* summary = NULL;
    summary = m_trackSumTool->createSummary(*trackUpper);
    unsigned int nHitsPixUp = 0;
    unsigned int nHitsSCTUp=0;
    unsigned int nHitsTRTUp=0;
    nHitsPixUp = summary->get(Trk::numberOfPixelHits);
    nHitsSCTUp = summary->get(Trk::numberOfSCTHits);
    nHitsTRTUp = summary->get(Trk::numberOfTRTHits);
    unsigned int nHitsUp = nHitsPixUp + nHitsSCTUp + nHitsTRTUp;
    delete summary;


    const Trk::Perigee* measUpperPer = trackUpper->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measUpperPer ? measUpperPer->covariance() : NULL;

    float d0Up            = -999.;
    float d0UpErr         = -999.;
    float phi0Up          = -999.;
    float phi0UpErr       = -999.;
    float thetaUp         = -999.;
    float thetaUpErr	  = -999.;
    float eta0Up	  = -999.;
    float eta0UpErr	  = -999.;
    float z0Up            = -999.;
    float z0UpErr	  = -999.;
    float qOverPtUp	  = -999.;
    float qOverPUpErr	  = -999.;
    float qOverPtUpErr2   = -999.;
    float qOverPtUpErr	  = -999.;
    float chargeUp	  = -999.;
    float ptUp 	          = -999.;

    //control on the measUpperPer!
    //Get the track parameters
    if ( covariance != NULL ){
      d0Up = measUpperPer->parameters()[Trk::d0];
      d0UpErr = Amg::error(*measUpperPer->covariance(),Trk::d0);

      phi0Up = measUpperPer->parameters()[Trk::phi0];
      phi0UpErr = Amg::error(*measUpperPer->covariance(),Trk::phi0);

      thetaUp = measUpperPer->parameters()[Trk::theta];
      thetaUpErr = Amg::error(*measUpperPer->covariance(),Trk::theta);

      eta0Up = measUpperPer->eta();
      eta0UpErr = 0.5*1.0/sin(0.5*thetaUp)*1.0/cos(0.5*thetaUp)*thetaUpErr;

      z0Up = measUpperPer->parameters()[Trk::z0];
      z0UpErr = Amg::error(*measUpperPer->covariance(),Trk::z0);

      qOverPtUp = measUpperPer->parameters()[Trk::qOverP]*1000/sin(thetaUp);
      qOverPUpErr = Amg::error(*measUpperPer->covariance(),Trk::qOverP);

      qOverPtUpErr2 = pow(1000/sin(thetaUp)*qOverPUpErr,2) + pow(qOverPtUp/tan(thetaUp)*thetaUpErr,2);
      qOverPtUpErr = pow( qOverPtUpErr2, 0.5);

      chargeUp = measUpperPer->charge();
      ptUp = measUpperPer->pT()/1000.;
    }
    else
      {
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Measured Upper Perigee not retrieved" << endmsg;
      }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Filling Upper info" << endmsg;
    m_upper_hist->nhitstrt->Fill(nHitsTRTUp);
    m_upper_hist->nhitsSi->Fill(nHitsSCTUp + nHitsPixUp);
    m_upper_hist->nhitsPix->Fill(nHitsPixUp);
    m_upper_hist->nhitsSct->Fill(nHitsSCTUp);
    m_upper_hist->phi0->Fill(phi0Up);
    m_upper_hist->z0->Fill(z0Up);
    m_upper_hist->d0->Fill(d0Up);
    m_upper_hist->eta0->Fill(eta0Up);
    m_upper_hist->qOverPt->Fill(qOverPtUp);
    m_upper_hist->pt->Fill(ptUp);

    //trackStateOnSurfaces is a vector of Trk::TrackStateOnSurface objects which contain information
    //on track at each (inner)detector surface it crosses eg hit used to fit track
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track Upper = " << nTracksUpper << endmsg;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Upper Track Eta = " << eta0Up << ", phi = " << phi0Up << endmsg;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Upper Track Pix = " << nHitsPixUp << ", SCT = " << nHitsSCTUp << ", TRT = " << nHitsTRTUp << endmsg;

    float mindR = 10000;
    float Matched_Low_d0 = -99;
    float Matched_Low_d0_Err = -99;
    float Matched_Low_phi0 = -99;
    float Matched_Low_phi0_Err = -99;
    float Matched_Low_qOverPt = -99;
    float Matched_Low_qOverPt_Err = -99;
    float Matched_Low_eta0 = -99;
    float Matched_Low_eta0_Err = -99;
    float Matched_Low_z0 = -99;
    float Matched_Low_z0_Err = -99;
    float Matched_Low_charge = -99;
    float Matched_Low_pt = -99;
    unsigned int nHitsLow = 0;
    bool matchFound = false;

    int nTracksLower = 0;
    DataVector<Trk::Track>::const_iterator trackItrLower  = tracksLower->begin();
    DataVector<Trk::Track>::const_iterator trackItrLowerE = tracksLower->end();
    for (; trackItrLower != trackItrLowerE; ++trackItrLower) { //looping over Lower tracks

      const Trk::Track* trackLower = *trackItrLower;
      if(trackLower == NULL){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No associated Trk::Track object found for track "<< nTracksLower << endmsg;
	continue;
      }

      const Trk::TrackSummary* summary = NULL;
      summary = m_trackSumTool->createSummary(*trackLower);
      unsigned int nHitsPixLow = 0;
      unsigned int nHitsSCTLow = 0;
      unsigned int nHitsTRTLow = 0;
      nHitsPixLow = summary->get(Trk::numberOfPixelHits);
      nHitsSCTLow = summary->get(Trk::numberOfSCTHits);
      nHitsTRTLow = summary->get(Trk::numberOfTRTHits);
      nHitsLow = nHitsPixLow + nHitsSCTLow + nHitsTRTLow;
      delete summary;


      const Trk::Perigee* measLowerPer = trackLower->perigeeParameters();
      const AmgSymMatrix(5)* covariance = measLowerPer ? measLowerPer->covariance() : NULL;

      float d0Low          = -999.;
      float d0LowErr       = -999.;
      float phi0Low        = -999.;
      float phi0LowErr     = -999.;
      float thetaLow       = -999.;
      float thetaLowErr    = -999.;
      float eta0Low        = -999.;
      float eta0LowErr     = -999.;
      float z0Low          = -999.;
      float z0LowErr       = -999.;
      float qOverPtLow     = -999.;
      float qOverPLowErr   = -999.;
      float qOverPtLowErr2 = -999.;
      float qOverPtLowErr  = -999.;
      float chargeLow      = -999.;
      float ptLow          = -999.;

      if ( covariance != NULL ){
	d0Low          = measLowerPer->parameters()[Trk::d0];
	d0LowErr       = Amg::error(*measLowerPer->covariance(),Trk::d0);
	phi0Low        = measLowerPer->parameters()[Trk::phi0];
	phi0LowErr     = Amg::error(*measLowerPer->covariance(),Trk::phi0);
	thetaLow       = measLowerPer->parameters()[Trk::theta];
	thetaLowErr    = Amg::error(*measLowerPer->covariance(),Trk::theta);
	eta0Low        = measLowerPer->eta();
	eta0LowErr     = 0.5*1.0/sin(0.5*thetaLow)*1.0/cos(0.5*thetaLow)*thetaLowErr;
	z0Low          = measLowerPer->parameters()[Trk::z0];
	z0LowErr       = Amg::error(*measLowerPer->covariance(),Trk::z0);
	qOverPtLow     = measLowerPer->parameters()[Trk::qOverP]*1000/sin(thetaLow);
	qOverPLowErr   = Amg::error(*measLowerPer->covariance(),Trk::qOverP);
	qOverPtLowErr2 = pow(1000/sin(thetaLow)*qOverPLowErr,2) + pow(qOverPtLow/tan(thetaLow)*thetaLowErr,2);
	qOverPtLowErr  = pow( qOverPtLowErr2, 0.5);
	chargeLow      = measLowerPer->charge();
	ptLow          = measLowerPer->pT()/1000.;
      }
      else
	{
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Measured Lower Perigee not retrieved" << endmsg;
	}


      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Lower Track = " << nTracksLower << endmsg;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Lower Track Eta = " << eta0Low << ", phi = " << phi0Low << endmsg;

      if(nTracksUpper ==0 ){//only fill the lower tracks once

	m_lower_hist->nhitstrt->Fill(nHitsTRTLow);
	m_lower_hist->nhitsSi->Fill(nHitsSCTLow + nHitsPixLow);
	m_lower_hist->nhitsPix->Fill(nHitsPixLow);
	m_lower_hist->nhitsSct->Fill(nHitsSCTLow);

	m_lower_hist->phi0->Fill(phi0Low);
	m_lower_hist->z0->Fill(z0Low);
	m_lower_hist->d0->Fill(d0Low);
	m_lower_hist->eta0->Fill(eta0Low);
	m_lower_hist->qOverPt->Fill(qOverPtLow);
	m_lower_hist->pt->Fill(ptLow);

      }//Fill lower tracks once

      //selecting Lower track that is closest to Upper in eta-phi
      float dphi2 = (phi0Up - phi0Low)*(phi0Up - phi0Low);

      //For TRT only tracks we will ignore the delta eta
      // and just require a delta phi match
      float deta2 = (eta0Up - eta0Low)*(eta0Up - eta0Low);

      float dR = sqrt(dphi2 + deta2);
      if(dR < mindR){
	mindR = dR;
	Matched_Low_d0 = d0Low;
	Matched_Low_d0_Err = d0LowErr;
	Matched_Low_phi0 = phi0Low;
	Matched_Low_phi0_Err = phi0LowErr;
	Matched_Low_eta0 = eta0Low;
	Matched_Low_eta0_Err = eta0LowErr;
	Matched_Low_z0 = z0Low;
	Matched_Low_z0_Err = z0LowErr;
	Matched_Low_qOverPt = qOverPtLow;
	Matched_Low_qOverPt_Err = qOverPtLowErr;
	Matched_Low_charge = chargeLow;
	Matched_Low_pt = ptLow;

	if(dR < m_matchedRcut)
	  matchFound = true;
      }

      nTracksLower++;

    }//looping over lower tracks

    m_debug_phi0->Fill(phi0Up - Matched_Low_phi0);
    m_debug_eta0->Fill(eta0Up - Matched_Low_eta0);

    if(matchFound){

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "there was a match found " <<endmsg;

      for(unsigned int charge=0; charge<3; ++charge){

	//Skip the postive hist for negative tracks
	if(charge == 1 && chargeUp < 0)
	  continue;

	//Skip the negative hist for postive tracks
	if(charge == 2 && chargeUp > 0)
	  continue;

	//===== D0
	m_delta_d0->dTp[charge]->Fill(d0Up - Matched_Low_d0);
	m_delta_d0->dTpPull[charge]->Fill((d0Up - Matched_Low_d0)/pow(pow(d0UpErr,2)+pow(Matched_Low_d0_Err,2),0.5));
	m_delta_d0->VsD0[charge]->Fill(d0Up, d0Up - Matched_Low_d0);
	m_delta_d0->VsPhi0[charge]->Fill(phi0Up, d0Up - Matched_Low_d0);
	m_delta_d0->VsPt[charge]->Fill(ptUp, d0Up - Matched_Low_d0);
	m_delta_d0->VsEta[charge]->Fill(eta0Up, d0Up - Matched_Low_d0);
	m_delta_d0->VsZ0[charge]->Fill(z0Up, d0Up - Matched_Low_d0);

	//===== Z0
	m_delta_z0->dTp[charge]->Fill(z0Up - Matched_Low_z0);
	m_delta_z0->dTpPull[charge]->Fill((z0Up - Matched_Low_z0)/pow(pow(z0UpErr,2)+pow(Matched_Low_z0_Err,2),0.5));
	m_delta_z0->VsD0[charge]->Fill(d0Up,z0Up - Matched_Low_z0);
	m_delta_z0->VsPhi0[charge]->Fill(phi0Up,z0Up - Matched_Low_z0);
	m_delta_z0->VsPt[charge]->Fill(ptUp,z0Up - Matched_Low_z0);
	m_delta_z0->VsEta[charge]->Fill(eta0Up,z0Up - Matched_Low_z0);
	m_delta_z0->VsZ0[charge]->Fill(z0Up,z0Up - Matched_Low_z0);

	//===== Phi0
	m_delta_phi0->dTp[charge]->Fill(phi0Up - Matched_Low_phi0);
	m_delta_phi0->dTpPull[charge]->Fill((phi0Up - Matched_Low_phi0)/pow(pow(phi0UpErr,2)+pow(Matched_Low_phi0_Err,2),0.5));
	m_delta_phi0->VsD0[charge]->Fill(d0Up, phi0Up - Matched_Low_phi0);
	m_delta_phi0->VsPhi0[charge]->Fill(phi0Up, phi0Up - Matched_Low_phi0);
	m_delta_phi0->VsPt[charge]->Fill(ptUp, phi0Up - Matched_Low_phi0);
	m_delta_phi0->VsEta[charge]->Fill(eta0Up, phi0Up - Matched_Low_phi0);
	m_delta_phi0->VsZ0[charge]->Fill(z0Up, phi0Up - Matched_Low_phi0);

	//===== eta0
	m_delta_eta0->dTp[charge]->Fill(eta0Up - Matched_Low_eta0);
	m_delta_eta0->dTpPull[charge]->Fill((eta0Up - Matched_Low_eta0)/pow(pow(eta0UpErr,2)+pow(Matched_Low_eta0_Err,2),0.5));
	m_delta_eta0->VsD0[charge]->Fill(d0Up, eta0Up - Matched_Low_eta0);
	m_delta_eta0->VsPhi0[charge]->Fill(phi0Up, eta0Up - Matched_Low_eta0);
	m_delta_eta0->VsPt[charge]->Fill(ptUp, eta0Up - Matched_Low_eta0);
	m_delta_eta0->VsEta[charge]->Fill(eta0Up, eta0Up - Matched_Low_eta0);
	m_delta_eta0->VsZ0[charge]->Fill(z0Up, eta0Up - Matched_Low_eta0);

	//====== qOverPt
	m_delta_qOverPt->dTp[charge]->Fill(qOverPtUp - Matched_Low_qOverPt);
	m_delta_qOverPt->dTpPull[charge]->Fill((qOverPtUp - Matched_Low_qOverPt)/pow(pow(qOverPtUpErr,2)+pow(Matched_Low_qOverPt_Err,2),0.5));
	m_delta_qOverPt->VsD0[charge]->Fill(d0Up,qOverPtUp - Matched_Low_qOverPt);
	m_delta_qOverPt->VsPhi0[charge]->Fill(phi0Up,qOverPtUp - Matched_Low_qOverPt);
	m_delta_qOverPt->VsPt[charge]->Fill(ptUp, qOverPtUp - Matched_Low_qOverPt);
	m_delta_qOverPt->VsEta[charge]->Fill(eta0Up, qOverPtUp - Matched_Low_qOverPt);
	m_delta_qOverPt->VsZ0[charge]->Fill(z0Up,qOverPtUp - Matched_Low_qOverPt);

	//====== PtqOverPt
	float ptUpperDeltaQOverPt = ptUp*(qOverPtUp - Matched_Low_qOverPt);
	m_delta_PtqOverPt->dTp[charge]->Fill(ptUpperDeltaQOverPt);
	m_delta_PtqOverPt->dTpPull[charge]->Fill(ptUp*((qOverPtUp - Matched_Low_qOverPt)/pow(pow(qOverPtUpErr,2)+pow(Matched_Low_qOverPt_Err,2),0.5)));
	m_delta_PtqOverPt->VsD0[charge]->Fill(d0Up,ptUpperDeltaQOverPt);
	m_delta_PtqOverPt->VsPhi0[charge]->Fill(phi0Up,ptUpperDeltaQOverPt);
	m_delta_PtqOverPt->VsPt[charge]->Fill(ptUp, ptUpperDeltaQOverPt);
	m_delta_PtqOverPt->VsEta[charge]->Fill(eta0Up, ptUpperDeltaQOverPt);
	m_delta_PtqOverPt->VsZ0[charge]->Fill(z0Up,ptUpperDeltaQOverPt);

	//===== nHits
	m_delta_nHits->dTp[charge]->Fill((int)(nHitsUp - nHitsLow));
	m_delta_nHits->VsD0[charge]->Fill(d0Up, (int)(nHitsUp - nHitsLow));
	m_delta_nHits->VsPhi0[charge]->Fill(phi0Up, (int)(nHitsUp - nHitsLow));
	m_delta_nHits->VsPt[charge]->Fill(ptUp, (int)(nHitsUp - nHitsLow));
	m_delta_nHits->VsEta[charge]->Fill(eta0Up, (int)(nHitsUp - nHitsLow));
	m_delta_nHits->VsZ0[charge]->Fill(z0Up, (int)(nHitsUp - nHitsLow));

      }

      //===== charge
      m_delta_charge->dTp[0]->Fill(chargeUp - Matched_Low_charge);
      m_delta_charge->VsD0[0]->Fill(d0Up,chargeUp - Matched_Low_charge);
      m_delta_charge->VsPhi0[0]->Fill(phi0Up,chargeUp - Matched_Low_charge);
      m_delta_charge->VsPt[0]->Fill(ptUp, chargeUp - Matched_Low_charge);
      m_delta_charge->VsEta[0]->Fill(eta0Up, chargeUp - Matched_Low_charge);
      m_delta_charge->VsZ0[0]->Fill(z0Up,chargeUp - Matched_Low_charge);

      //pT
      m_delta_pt->Fill(ptUp - Matched_Low_pt);
      m_reldelta_pt->Fill((ptUp - Matched_Low_pt)/Matched_Low_pt);

    }//match found

    nTracksUpper++;

  }//looping over upper tracks

  delete tracksLower;
  delete tracksUpper;

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------------------
// initializing tools
//---------------------------------------------------------------------------------------
StatusCode IDAlignMonTrackSegments::setupTools()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setupTools()" << endmsg;

  if (m_trackSplitter.empty()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No track splitter tool configured"<< endmsg;
  } else if (m_trackSplitter.retrieve().isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve "<< m_trackSplitter.typeAndName() << endmsg;
  } else
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Retrieved tool " << m_trackSplitter.typeAndName() << endmsg;

  // get TrackSummaryTool
  if ( m_trackSumTool.retrieve().isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSumTool << endmsg;
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSumTool << endmsg;
  }

  //Get TrackSelectionTools

  if (m_trackSelectionUpper.retrieve().isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Can not retrieve TrackSelection tool of type "
	<< m_trackSelectionUpper.typeAndName() << endmsg;
    return StatusCode::SUCCESS;
  } else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSelectionUpper.typeAndName() << endmsg;

  if (m_trackSelectionLower.retrieve().isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Can not retrieve TrackSelection tool of type "
	<< m_trackSelectionLower.typeAndName() << endmsg;
    return StatusCode::SUCCESS;
  } else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSelectionLower.typeAndName() << endmsg;


  return StatusCode::SUCCESS;
}

/** Make Histograms */
TH1F_LW* IDAlignMonTrackSegments::MakeHist(const std::string & name, const std::string & title, int nBins, float xLow, float xUp, const std::string & xAxisTitle, const std::string & yAxisTitle) {
  TH1F_LW* hist = TH1F_LW::create(name.c_str(), title.c_str(), nBins, xLow, xUp);
  hist->SetMinimum(0);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->SetTitle(xAxisTitle.c_str());
  hist->GetYaxis()->SetTitle(yAxisTitle.c_str());
  return hist;
}

/** Make Histograms */
TH2F_LW* IDAlignMonTrackSegments::MakeHist(const std::string & name, const std::string & title, int nXBins, float xLow, float xUp, int nYBins, float yLow, float yUp, const std::string & xAxisTitle, const std::string & yAxisTitle) {
  TH2F_LW* hist = TH2F_LW::create(name.c_str(), title.c_str(), nXBins, xLow, xUp, nYBins, yLow, yUp);
  hist->SetMinimum(0);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->SetTitle(xAxisTitle.c_str());
  hist->GetYaxis()->SetTitle(yAxisTitle.c_str());
  return hist;
}
