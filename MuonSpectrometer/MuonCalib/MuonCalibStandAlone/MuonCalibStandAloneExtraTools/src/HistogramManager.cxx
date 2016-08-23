/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.08.2008, AUTHOR: MAURO IODICE
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
// .L HistogramManager.C
// HistogramManager * builder = new HistogramManager();
// builder->METODO1();
// builder->METODO2();;

#include "MuonCalibStandAloneExtraTools/HistogramManager.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"

//-------------------------
#include "MuonIdHelpers/MdtIdHelper.h"
#include "Identifier/IdentifierHash.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

using namespace std;
using namespace MuonCalib;

HistogramManager::HistogramManager() {
  // m_rootfile = new TFile();
  m_MdtIdHelper = NULL;
  m_rootfile = NULL;
  m_hList(0);
  m_doTracks = false;
}
HistogramManager::HistogramManager(const MdtIdHelper* mdtIdHelper) {
  // m_rootfile = new TFile();
  m_MdtIdHelper = mdtIdHelper;
  m_rootfile = NULL;
  m_hList(0);
  m_doTracks = false;
}
HistogramManager::~HistogramManager() {
}

void HistogramManager::SetDoTracks(bool the_tracks) {         
  m_doTracks=the_tracks;
}

void HistogramManager::buildGlobalHistos() {

  //cout<<"Building Global Histos "  <<endl;

  if (!m_rootfile->IsOpen()){
    //cout << "opening output file with default name"<< endl;
    string defaultOutputFile="outDQA.root";
    openOutputFile(defaultOutputFile);
  }

  m_rootfile->cd();

  string dir_name;
  // Building Base directory :
  // /GLOBAL/ 
  dir_name="/GLOBAL";
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    // A new TDirectoryFile gets owned by the current directory (side effect).
    new TDirectoryFile("GLOBAL","GLOBAL");
  }

  m_rootfile->cd("/GLOBAL");

  TH2F * globalOccupancy;

  // Inner,Middle,Outer,(extra) chambers of the region (BA/BC/EA/EC) :

  globalOccupancy = new TH2F("Global_HitsPerML_Inner","Global HitsPerML Inner",
			     45, -22.5, 22.5, 32, 0.5, 16.5 );
  globalOccupancy->GetXaxis()->SetTitle("Chamber_etaId");
  globalOccupancy->GetYaxis()->SetTitle("sectors");

  globalOccupancy = new TH2F("Global_HitsPerML_Middle","Global HitsPerML Middle",
			     45, -22.5, 22.5, 32, 0.5, 16.5 );
  globalOccupancy->GetXaxis()->SetTitle("Chamber_etaId");
  globalOccupancy->GetYaxis()->SetTitle("sectors");

  globalOccupancy = new TH2F("Global_HitsPerML_Outer","Global HitsPerML Outer",
			     45, -22.5, 22.5, 32, 0.5, 16.5 );
  globalOccupancy->GetXaxis()->SetTitle("Chamber_etaId");
  globalOccupancy->GetYaxis()->SetTitle("sectors");

  TH2F * phiVseta;
  phiVseta = new TH2F("phi_vs_eta","Global Segment_phi Vs Segment_eta",
		      40, -4., 4., 90, -180., 180. );
  phiVseta->GetYaxis()->SetTitle("Segment_phi (deg)");
  phiVseta->GetXaxis()->SetTitle("Segment_eta");

//   TH1F * segments;
//   segments = new TH1F("seg","Segments per Event",40,0.,39.);
//   segments->GetXaxis()->SetTitle("numSegments_per_event");
}
  // TRACK Histograms ....
void HistogramManager::buildTrackHistos() {

 //cout<<"Building Track Histos "  <<endl;

  if (!m_rootfile->IsOpen()){
    //cout << "opening output file with default name"<< endl;
    string defaultOutputFile="outDQA.root";
    openOutputFile(defaultOutputFile);
  }

  m_rootfile->cd();
  string dir_name;
  dir_name="TRACKS";
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    // A new TDirectoryFile gets owned by the current directory (side effect).
    new TDirectoryFile("TRACKS","TRACKS");
  }

  m_rootfile->cd("/TRACKS");

    // TH1F * nmbts;
    // nmbts=new TH1F("mbts_nb","number of mbts",12,-1.5,10.5);
    // nmbts->GetXaxis()->SetTitle("number of mbts");

    TH1F * tmbts1;
    tmbts1=new TH1F("mbts1_time","Time mbts 1 (plus)",200,-100.,100.);
    tmbts1->GetXaxis()->SetTitle("time (ns)");

    TH1F * tmbts2;
    tmbts2=new TH1F("mbts2_time","Time mbts 2 (minus)",200,-100.,100.);
    tmbts2->GetXaxis()->SetTitle("time (ns)");

    TH2F * tmbts12;
    tmbts12=new TH2F("mbts1VSmbts2","Time mbts 1 VS 2 ",200,-100.,100., 200,-100.,100.);
    tmbts12->GetXaxis()->SetTitle("time (ns)");
    tmbts12->GetYaxis()->SetTitle("time (ns)");

    TH1F * Dtmbts;
    Dtmbts=new TH1F("mbts_Dtime","Time difference mbts",200,-100.,100.);
    Dtmbts->GetXaxis()->SetTitle("#Delta t (ns)");

    TH1F * trkIDmult;
    trkIDmult=new TH1F("trk_ID_mult","ID Tracks per Event",51,-0.5,50.5);
    trkIDmult->GetXaxis()->SetTitle("num_ID_Tracks_per_event");

    TH1F * seg_nb;
    seg_nb=new TH1F("seg_nb","Segments per Track",12,-1.5,10.5);
    seg_nb->GetXaxis()->SetTitle("Associated Segments per track");

    TH1F * trk_nb;
    trk_nb=new TH1F("trk_nb","Tracks per Event",10,-0.5,9.5);
    trk_nb->GetXaxis()->SetTitle("numTracks_per_event");

    TH1F * trk_nb_sel;
    trk_nb_sel=new TH1F("trk_nb_sel","Tracks per Event",10,-0.5,9.5);
    trk_nb_sel->GetXaxis()->SetTitle("numTracks_per_event");

    TH1F * trk_author1;
    trk_author1=new TH1F("trk_authorMoore","Track Author Moore",14,-0.5,13.5);
    trk_author1->GetXaxis()->SetBinLabel(1,"ID");
    trk_author1->GetXaxis()->SetBinLabel(2,"Moore");
    trk_author1->GetXaxis()->SetBinLabel(3,"ExtrSA nocalo");
    trk_author1->GetXaxis()->SetBinLabel(4,"ExtrSA calo");
    trk_author1->GetXaxis()->SetBinLabel(5,"MuidSA");
    trk_author1->GetXaxis()->SetBinLabel(6,"MuidCB");
    trk_author1->GetXaxis()->SetBinLabel(7,"MuonCombined?");
    trk_author1->GetXaxis()->SetBinLabel(8,"MuTagIMO");
    trk_author1->GetXaxis()->SetBinLabel(9,"MuGirl");
    trk_author1->GetXaxis()->SetBinLabel(10,"AODMuidSA");
    trk_author1->GetXaxis()->SetBinLabel(11,"AODMuidCB");
    trk_author1->GetXaxis()->SetBinLabel(12,"AODMuTagIMO");
    trk_author1->GetXaxis()->SetBinLabel(13,"AODMuGirl");

    TH1F * trk_author2;
    trk_author2=new TH1F("trk_authorMuBoy","Track Author MuonBoy",14,-0.5,13.5);
    trk_author2->GetXaxis()->SetBinLabel(1,"ID");
    trk_author2->GetXaxis()->SetBinLabel(2,"MuonBoy");
    trk_author2->GetXaxis()->SetBinLabel(3,"Staco");
    trk_author2->GetXaxis()->SetBinLabel(4,"MuTagTracks");
    trk_author2->GetXaxis()->SetBinLabel(5,"StacoMuonSA");
    trk_author2->GetXaxis()->SetBinLabel(6,"StacoMuonCB");
    trk_author2->GetXaxis()->SetBinLabel(7,"StacoMuonTag");

    new TH1F("trk_pt","; pt (GeV)",1000,0.,1000.);

    new TH1F("trk_eta","; #eta",80,-4.,4.);
    new TH1F("trk_phi","; #phi",80,-4.,4.);
    new TH2F("trk_etaVSphi",";#eta; #phi",80,-4.,4.,80,-4.,4.);
    new TH2F("trk_d0VSz0",";d0(mm); z0(mm)",2000,-5000.,5000.,200,-500.,500.);
    TH2F * h2 = new TH2F("trk_yVSx","y Vs x",1000,-15000.,15000.,1000,-15000.,15000.);
    h2->GetXaxis()->SetTitle("x(mm)");
    h2->GetYaxis()->SetTitle("y(mm)");
    new TH1F("trk_qOverP","; Q over P",1000,-0.001,0.001);
    new TH1F("trk_chi2dof","; #chi^{2}/ndof",100,0.,10.);
    new TH2F("trk_chi2dof_VS_author","; #chi^{2}/ndof; Track Author",100,0.,10., 2220,-10.,1100.);
    new TH2F("trk_chi2dof_VS_eta",";#eta; #chi^{2}/ndof",80, -4.,4.,100,0.,10.);
    new TH1F("trk_d0","; d0 ",200,-500.,500.);
    new TH1F("trk_z0ip","; z0ip (mm)",2000,-5000.,5000.);
    new TH2F("trk_mdthits_VS_eta",";#eta; nr hits per track",80, -4.,4.,100,-0.5,99.5);
    new TH2F("trk_mdthits_VS_phi",";#phi; nr hits per track",80, -4.,4.,100,-0.5,99.5);
    new TH1F("hit_driftR","; drift Radius (mm)",300,-15.,15.);
    new TH1F("hit_pull","; pull (mm)",200,-10.,10.);
    new TH1F("hit_mdt","; mdt hits per track", 50, -0.5, 49.5);
    new TH2F("hit_mdtVSrpc","; rpc hits; mdt hits",  50, -0.5, 49.5,  50, -0.5, 49.5);
    new TH1F("hit_rpc","; rpc hits per track", 50, -0.5, 49.5);
    new TH1F("hit_tgc","; tgc hits per track", 50, -0.5, 49.5);
    new TH1F("hit_csc","; csc hits per track", 50, -0.5, 49.5);

    TH1F * h1 = new TH1F("trk_ID_ntrack","ID tracks",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("nb ID tracks");
    h1 = new TH1F("trk_ID_npixel","Pixel Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("Pixel Hits");
    h1 = new TH1F("trk_ID_nsct","SCT Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("SCT Hits");
    h1 = new TH1F("trk_ID_ntrt","TRT Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("TRT Hits");
    new TH1F("trk_ID_p","P in the Inner Tracker; p (GeV)",200,0.,100.);
    new TH1F("trk_Ev_trkveto","TRACK VETO in the Inner Tracker; TRACK VETO",20,-0.5,19.5);
    h1 = new TH1F("trk_Ev_pveto","P VETO in the Inner Tracker",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("PVETO");
    new TH2F("trk_ID_pixsctVStrt","pix+sct VS trt; npixel + nsct; ntrt",105,-5.5,99.5,105,-5.5,99.5);
    new TH2F("trk_ID_pixVSsct","pix VS sct; npixel; nsct",55,-5.5,49.5,55,-5.5,49.5);

    h1 = new TH1F("trk_SA_ntrack","SA tracks",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("nb SA tracks");

    h1=new TH1F("trk_SAIP_ntrack","SAIP tracks",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("nb SAIP tracks");

    h1 = new TH1F("trk_Tag_ntrack","nb Tag tracks",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("nb Tag tracks");
    h1=new TH1F("trk_Tag_npixel","Pixel Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("Pixel Hits");
    h1=new TH1F("trk_Tag_nsct","SCT Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("SCT Hits");
    h1 = new TH1F("trk_Tag_ntrt","TRT Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("TRT Hits");
    h1 = new TH1F("trk_Tag_pveto","P VETO in the Inner Tracker",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("PVETO");
    new TH2F("trk_Tag_pixsctVStrt","pix+sct VS trt; npixel + nsct; ntrt",55,-5.5,49.5,55,-5.5,49.5);
    new TH2F("trk_Tag_pixVSsct","pix VS sct; npixel; nsct",55,-5.5,49.5,55,-5.5,49.5);


    h1 = new TH1F("trk_CB_ntrack","CB tracks",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("nb CB tracks");
    h1=new TH1F("trk_CB_npixel","Pixel Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("Pixel Hits");
    h1=new TH1F("trk_CB_nsct","SCT Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("SCT Hits");
    h1=new TH1F("trk_CB_ntrt","TRT Hits in the Inner Tracker",50,-0.5,49.5);
    h1->GetXaxis()->SetTitle("TRT Hits");
    h1=new TH1F("trk_CB_pveto","P VETO in the Inner Tracker",20,-0.5,19.5);
    h1->GetXaxis()->SetTitle("PVETO");
    new TH2F("trk_CB_pixsctVStrt","pix+sct VS trt; npixel + nsct; ntrt",55,-5.5,49.5,55,-5.5,49.5);
    new TH2F("trk_CB_pixVSsct","pix VS sct; npixel; nsct",55,-5.5,49.5,55,-5.5,49.5);

    TH2F * trk_cutflow;
    trk_cutflow=new TH2F("trk_cutflow","cut flow",5,0.5,5.5,9,-0.5,8.5);
    trk_cutflow->GetXaxis()->SetBinLabel(1,"ID");
    trk_cutflow->GetXaxis()->SetBinLabel(2,"SA");
    trk_cutflow->GetXaxis()->SetBinLabel(3,"SA_IP");
    trk_cutflow->GetXaxis()->SetBinLabel(4,"Tag");
    trk_cutflow->GetXaxis()->SetBinLabel(5,"CB");
    trk_cutflow->GetYaxis()->SetBinLabel(1,"no cut");
    trk_cutflow->GetYaxis()->SetBinLabel(2,"cut 1");
    trk_cutflow->GetYaxis()->SetBinLabel(3,"cut 2");
    trk_cutflow->GetYaxis()->SetBinLabel(4,"cut 3");
    trk_cutflow->GetYaxis()->SetBinLabel(5,"cut 4");
    trk_cutflow->GetYaxis()->SetBinLabel(6,"cut 5");
    trk_cutflow->GetYaxis()->SetBinLabel(7,"cut 6");
    trk_cutflow->GetYaxis()->SetBinLabel(8,"cut 7");
    trk_cutflow->GetYaxis()->SetBinLabel(9,"cut 8");

    new TH1F("trk_selected_cut5_pt","; pt (GeV)",200,0.,100.);
    new TH1F("trk_selected_cut5_p","; p (GeV)",200,0.,100.);
    new TH2F("trk_selected_cut5_etaVSphi","#eta; #phi",90,-4.5,4.5,90,-4.5,4.5);

    new TH1F("trk_selected_pt","; pt (GeV)",200,0.,100.);
    new TH1F("trk_selected_p","; p (GeV)",200,0.,100.);
    new TH1F("trk_selected_eta","; #eta",90,-4.5,4.5);
    new TH1F("trk_selected_phi","; #phi",90,-4.5,4.5);
    new TH2F("trk_selected_etaVSphi","#eta; #phi",90,-4.5,4.5,90,-4.5,4.5);

    new TH1F("trk_associated_Deltapt","; pt (GeV)",200,-10.,10.);
    new TH1F("trk_associated_Deltap","; p (GeV)",200,-10.,10.);

    new TH1F("trk_associatedIP_Deltapt","; pt (GeV)",200,-10.,10.);
    new TH1F("trk_associatedIP_Deltap","; p (GeV)",200,-10.,10.);

    buildDebugHistos();

}

  // DEBUG AND SPECIFIC-ANALYSIS Histograms ....
  // buildDebugHistos();


void HistogramManager::buildDebugHistos() {

  //cout<<"Building DEBUG Histos "  <<endl;

  if (!m_rootfile->IsOpen()){
    //cout << "opening output file with default name"<< endl;
    string defaultOutputFile="outDQA.root";
    openOutputFile(defaultOutputFile);
  }

  m_rootfile->cd();
  string dir_name;
  // Building Base directory :
  // /DEBUG/ 
  dir_name=GetMdtDirectoryName();
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    // A new TDirectoryFile gets owned by the current directory (side effect).
    new TDirectoryFile("DEBUG","DEBUG");
  }
  m_rootfile->cd("/DEBUG");

  // new TH2F("t0FitVst0RPC","; t0RPC (ns); t0Refit (ns)",200,-50.,50.,200,-50.,50.);
  // new TH2F("At0FitVsAt0RPC","; slope t0RPC; slope t0Refit (ns)",200,-50.,50.,200,-50.,50.);
  // new TH2F("Bt0FitVsBt0RPC","; Intercept t0RPC; Intercept t0Refit",200,-50.,50.,200,-50.,50.);
  
  TH1F * h1 = new TH1F("nSegPerTrack","n Segments",21,-0.5,20.5);
  h1->GetXaxis()->SetTitle("nSegments");
  TH2F * h2 = new TH2F("nSegPerTrack_HitCut","; Sector n.; nSegments",16,0.5,16.5,21,-0.5,20.5);
  h2->SetTitle("Segment Per Track");

  new TH2F("t0_BI","; Sector n.; t0Refit_BI (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_BM","; Sector n.; t0Refit_BM (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_BO","; Sector n.; t0Refit_BO (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_BO_BM","; Sector n.; t0Refit_BO-BM (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_BO_BI","; Sector n.; t0Refit_BO-BI (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_BM_BI","; Sector n.; t0Refit_BM-BI (ns)",16,0.5,16.5,400,-200.,200.);

  new TH2F("t0_EI","; Sector n.; t0Refit_EI (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_EM","; Sector n.; t0Refit_EM (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_EO","; Sector n.; t0Refit_EO (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_EO_EM","; Sector n.; t0Refit_EO-EM (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_EO_EI","; Sector n.; t0Refit_EO-EI (ns)",16,0.5,16.5,400,-200.,200.);
  new TH2F("t0_EM_EI","; Sector n.; t0Refit_EM-EI (ns)",16,0.5,16.5,400,-200.,200.);

  new TH2F("trk_d0VSz0_GoodTime","d0(mm); z0(mm)",2000,-5000.,5000.,200,-500.,500.);
  new TH2F("trk_d0VSz0_BadTime","d0(mm); z0(mm)",2000,-5000.,5000.,200,-500.,500.);
  new TH2F("trk_d0VSz0_noSeg","d0(mm); z0(mm)",2000,-5000.,5000.,200,-500.,500.);
}


void HistogramManager::buildTopLevel(string region, string side,int sectorMin, int sectorMax) {

  //ToString _ts;
  string regionSide = region+"_"+side;
  string dir_name="none";

  //cout<<"Building top level histograms for region : " << regionSide <<endl;

  if (!m_rootfile->IsOpen()){
    //cout << "opening output file with default name"<< endl;
    string defaultOutputFile="outDQA.root";
    openOutputFile(defaultOutputFile);
  }

  m_rootfile->cd();

  // if not already created building directory "GLOBAL"
  dir_name="/GLOBAL";
  TDirectoryFile * global_dir=0;
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    global_dir = new TDirectoryFile("GLOBAL","GLOBAL");
  } else {
    global_dir = (TDirectoryFile *) m_rootfile->GetDirectory(dir_name.c_str());
  }

  global_dir->cd();

  TH2F * globalOccupancy;

 // all chambers of the region (BA/BC) :
  if (region=="Barrel") {
     globalOccupancy = (TH2F*) GetHisto("GLOBAL","Global_HitsPerML_Barrel");
     if (!globalOccupancy) { 
        globalOccupancy = new TH2F("Global_HitsPerML_Barrel","Global HitsPerML Barrel",
                                    81, -40.5, 40.5, 32, 0.5, 16.5 );
        globalOccupancy->GetXaxis()->SetTitle("Chamber_etaId (Inn,Mid,Out)");
        globalOccupancy->GetYaxis()->SetTitle("sectors");
        globalOccupancy->SetNdivisions(16,"Y");
        if (side=="A") {
           globalOccupancy->SetAxisRange(0.5,40.5,"X");
           globalOccupancy->SetTitle("Global HitsPerML BA");
        }
        if (side=="C") {
           globalOccupancy->SetAxisRange(-40.5,-0.5,"X");
           globalOccupancy->SetTitle("Global HitsPerML BC");
        }
     } else {
       globalOccupancy->SetAxisRange(-40.5,40.5,"X");
       globalOccupancy->SetTitle("Global HitsPerML Barrel");
     }
  }
  // all chambers of the region (EA/EC) :
  if (region=="Endcap") {
     globalOccupancy = (TH2F*) GetHisto("GLOBAL","Global_HitsPerML_Endcap");
     if (!globalOccupancy) { 
        globalOccupancy = new TH2F("Global_HitsPerML_Endcap","Global HitsPerML Endcap",
                                   81, -40.5, 40.5, 32, 0.5, 16.5 );
        globalOccupancy->GetXaxis()->SetTitle("Chamber_etaId (Inn,Mid,Out,Extra)");
        globalOccupancy->GetYaxis()->SetTitle("sectors");
        globalOccupancy->SetNdivisions(16,"Y");
        if (side=="A") {
           globalOccupancy->SetAxisRange(0.5,40.5,"X");
           globalOccupancy->SetTitle("Global HitsPerML EA");
        }
        if (side=="C") {
           globalOccupancy->SetAxisRange(-40.5,-0.5,"X");
           globalOccupancy->SetTitle("Global HitsPerML EC");
        }
     } else {
       globalOccupancy->SetAxisRange(-40.5,40.5,"X");
       globalOccupancy->SetTitle("Global HitsPerML Endcap");
     }
  }

  m_rootfile->cd();
  // Building Base directory :
  // /MDT/ 
  dir_name=GetMdtDirectoryName();
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    // A new TDirectoryFile gets owned by the current directory (side effect).
    new TDirectoryFile("MDT","MDT");
  }
  // Building Base directory :
  // /MDTvsRPC/ (for Barrel) -  /MDTvsTGC/ (for Endcap)
  dir_name = GetTDaqDirectoryName(region);
  string MDTvsTriggerChambers="Undefined";
  if (region == "Barrel") MDTvsTriggerChambers="MDTvsRPC";
  if (region == "Endcap") MDTvsTriggerChambers="MDTvsTGC";
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    // A new TDirectoryFile gets owned by the current directory (side effect).
    new TDirectoryFile(MDTvsTriggerChambers.c_str(),MDTvsTriggerChambers.c_str());
  }
  
  // Building <region-side> (ex. Barrel_A) directories :
  // motherdir  =  /MDT/;
  string motherdir; 
  motherdir = GetMdtDirectoryName();
  m_rootfile->cd(motherdir.c_str());
  dir_name=GetMdtDirectoryName(region,side);
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    TDirectoryFile * mdt_region = new TDirectoryFile(regionSide.c_str(),regionSide.c_str());
    mdt_region->cd();
    TH1F * h;
    TH2F * h2;
    float t0Min=200.;
    float t0Max=1200.;
    int t0Nbins = 250;
    float tdriftMin=600.;
    float tdriftMax=800.;
    int tdriftNbins = 100;

    string histoType;
    string histoTitle;
    //
    histoType="t0PerSector_Inner";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,t0Nbins,t0Min,t0Max);
    h2->GetXaxis()->SetTitle("Sector nr.");
    h2->GetYaxis()->SetTitle("t0 per ML (ns)");

    histoType="t0PerSector_Middle";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,t0Nbins,t0Min,t0Max);
    h2->GetXaxis()->SetTitle("Sector nr.");
    h2->GetYaxis()->SetTitle("t0 per ML (ns)");

    histoType="t0PerSector_Outer";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,t0Nbins,t0Min,t0Max);
    h2->GetXaxis()->SetTitle("Sector nr.");
    h2->GetYaxis()->SetTitle("t0 per ML (ns)");

    if(region=="Endcap")
    {
      histoType="t0PerSector_extra";
      histoTitle = histoType;
      h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,t0Nbins,t0Min,t0Max);
      h2->GetXaxis()->SetTitle("Sector nr.");
      h2->GetYaxis()->SetTitle("t0 per ML (ns)");
    }

    histoType="tdriftPerSector_Inner";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,tdriftNbins,tdriftMin,tdriftMax);
    h2->GetXaxis()->SetTitle("Sector nr.");
    h2->GetYaxis()->SetTitle("tdrift per ML (ns)");

    histoType="tdriftPerSector_Middle";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,tdriftNbins,tdriftMin,tdriftMax);
    h2->GetXaxis()->SetTitle("Sector nr.");
    h2->GetYaxis()->SetTitle("tdrift per ML (ns)");

    histoType="tdriftPerSector_Outer";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,tdriftNbins,tdriftMin,tdriftMax);
    h2->GetXaxis()->SetTitle("Sector nr.");
    h2->GetYaxis()->SetTitle("tdrift per ML (ns)");

    if(region=="Endcap")
    {
      histoType="tdriftPerSector_extra";
      histoTitle = histoType;
      h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5,tdriftNbins,tdriftMin,tdriftMax);
      h2->GetXaxis()->SetTitle("Sector nr.");
      h2->GetYaxis()->SetTitle("tdrift per ML (ns)");
    }

    /*
    histoType="t0PerSector_Inner";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
    h->GetXaxis()->SetTitle("Sector nr.");
    h->GetYaxis()->SetTitle("t0 (ns)");
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetAxisRange(400.,800.,"Y"); 
    //
    histoType="t0PerSector_Middle";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
    h->GetXaxis()->SetTitle("Sector nr.");
    h->GetYaxis()->SetTitle("t0 (ns)");
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetAxisRange(400.,800.,"Y"); 
    //
    histoType="t0PerSector_Outer";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
    h->GetXaxis()->SetTitle("Sector nr.");
    h->GetYaxis()->SetTitle("t0 (ns)");
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetAxisRange(400.,800.,"Y"); 
    //
    if(region=="Endcap")
    {
      histoType="t0PerSector_extra";
      histoTitle = histoType;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
      h->GetXaxis()->SetTitle("Sector nr.");
      h->GetYaxis()->SetTitle("t0 (ns)");
      h->SetMarkerStyle(20);
      h->SetMarkerSize(1.0);
      h->SetAxisRange(400.,800.,"Y"); 
    }
    //
    histoType="tdriftPerSector_Inner";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
    h->GetXaxis()->SetTitle("Sector nr.");
    h->GetYaxis()->SetTitle("tdrift (ns)");
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetAxisRange(600.,800.,"Y"); 
    //
    histoType="tdriftPerSector_Middle";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
    h->GetXaxis()->SetTitle("Sector nr.");
    h->GetYaxis()->SetTitle("tdrift (ns)");
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetAxisRange(600.,800.,"Y"); 
    //
    histoType="tdriftPerSector_Outer";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
    h->GetXaxis()->SetTitle("Sector nr.");
    h->GetYaxis()->SetTitle("tdrift (ns)");
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetAxisRange(600.,800.,"Y"); 
    //
    if(region=="Endcap")
    {
      histoType="tdriftPerSector_extra";
      histoTitle = histoType;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5);
      h->GetXaxis()->SetTitle("Sector nr.");
      h->GetYaxis()->SetTitle("tdrift (ns)");
      h->SetMarkerStyle(20);
      h->SetMarkerSize(1.0);
      h->SetAxisRange(600.,800.,"Y"); 
    }
    */

    histoType="time_Fit_t0";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),t0Nbins,t0Min,t0Max);
    h->GetXaxis()->SetTitle("t0 (ns)");    

    histoType="time_Fit_tdrift";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),tdriftNbins,tdriftMin,tdriftMax);
    h->GetXaxis()->SetTitle("tDrift (ns)");    

    histoType="time_Fit_tslope";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),40,0.,40.);
    h->GetXaxis()->SetTitle("tSlope (ns)");    

    histoType="time_Fit_chi2";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),200,0.,50.);
    h->GetXaxis()->SetTitle("tChi2perDof (ns)");    

    histoType="adc_Fit_peak";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),150,0.,300.);
    h->GetXaxis()->SetTitle("ADC counts");    

    histoType="adc_Fit_width";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),50,0.,50.);
    h->GetXaxis()->SetTitle("ADC counts");    

    histoType="hits_perML_belowADCCUT";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,0.,1.);
    h->GetXaxis()->SetTitle("Fraction of Hits");    

    histoType="TubeEfficiency_Inner";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,0.,1.1);
    h->GetXaxis()->SetTitle("TubeEfficiency");    

    histoType="TubeEfficiency_Middle";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,0.,1.1);
    h->GetXaxis()->SetTitle("TubeEfficiency");    

    histoType="TubeEfficiency_Outer";
    histoTitle = histoType;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,0.,1.1);
    h->GetXaxis()->SetTitle("TubeEfficiency");   

    if(region=="Endcap")
    {
      histoType="TubeEfficiency_extra";
      histoTitle = histoType;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,0.,1.1);
      h->GetXaxis()->SetTitle("TubeEfficiency");   
     }

    histoType="ResidualsWidth";
    histoTitle = histoType+" Vs Radius";
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),15,-15,15,100,0.,1.0);
    h2->GetXaxis()->SetTitle("Radius (mm)");    
    h2->GetYaxis()->SetTitle("Residuals width (mm)");    

    histoType="ResidualsMean";
    histoTitle = histoType+" Vs Radius";
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),15,-15,15,100,-1.0,1.0);
    h2->GetXaxis()->SetTitle("Radius (mm)");    
    h2->GetYaxis()->SetTitle("Residuals mean (mm)");    

    histoType="DeadElements";
    histoTitle = histoType;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),16,0.5,16.5, 6,0.5,6.5);
    h2->GetYaxis()->SetBinLabel(1,"Low_Stat_Ch");
    h2->GetYaxis()->SetBinLabel(2,"dead_Chamber");
    h2->GetYaxis()->SetBinLabel(3,"dead_ML");
    h2->GetYaxis()->SetBinLabel(4,"dead_Layer");
    h2->GetYaxis()->SetBinLabel(5,"dead_Mezz");
    h2->GetYaxis()->SetBinLabel(6,"dead_Tube");
    h2->GetXaxis()->SetTitle("Sector Id");    
    h2->SetMarkerSize(1.6);    

    // Here the Overview occupancy plots per chamber in Sector_Vs_Eta view for Barrel and Endcap 
    if ( region == "Barrel" ) {
      histoType="A_HitsPerML_Inner";
      histoTitle="HitsPerML_B"+side+"_Inner";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),12,-0.5,11.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");
      h2->SetAxisRange(0.5,11.5,"X"); // same range as the others but better display
     
      histoType="A_HitsPerML_Middle";
      histoTitle="HitsPerML_B"+side+"_Middle";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),12,-0.5,11.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");
      h2->SetAxisRange(0.5,6.5,"X"); // same range as the others but better display
 
      histoType="A_HitsPerML_Outer";
      histoTitle="HitsPerML_B"+side+"_Outer";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),12,-0.5,11.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");
      h2->SetAxisRange(-0.5,8.5,"X"); // same range as the others but better display
    }
    if ( region == "Endcap" ) {
      // 
      // CHECK IF THIS IS CORRECT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
      //
      histoType="A_HitsPerML_Inner";
      histoTitle="HitsPerML_E"+side+"_Inner";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),8,0.5,8.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");

      histoType="A_HitsPerML_Middle";
      histoTitle="HitsPerML_E"+side+"_Middle";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),6,0.5,6.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");

      histoType="A_HitsPerML_Outer";
      histoTitle="HitsPerML_E"+side+"_Outer";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),6,0.5,6.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");

      histoType="A_HitsPerML_extra";
      histoTitle="HitsPerML_E"+side+"_extra";
      h2  = new TH2F(histoType.c_str(),histoTitle.c_str(),4,0.5,4.5,32,0.5,16.5);
      h2->GetXaxis()->SetTitle("eta_id");
      h2->GetYaxis()->SetTitle("Sector nr.");
    }
    // End of Overview occupancy plots
  }

  
  // motherdir  =  /TrigDaq/;
  motherdir = GetTDaqDirectoryName(region);
  m_rootfile->cd(motherdir.c_str());
  dir_name = GetTDaqDirectoryName(region,side);
  if (!m_rootfile->GetDirectory(dir_name.c_str())) {
    // A new TDirectoryFile gets owned by the current directory (side effect).
    new TDirectoryFile(regionSide.c_str(),regionSide.c_str());
  }

  // Build Sectors :
  for (int sector=sectorMin; sector<=sectorMax; sector++) {   // LOOP on Sectors
    buildSector(region,side,sector);
  }
}


void HistogramManager::buildSector(string region, string side, int sector) {

  // cout << " HistogramManager::buildSector - building sector: "<<sector
  //      << " side "<<side<<" in the "<<region << endl;

  string sectordir=GetMdtDirectoryName(region, side, sector);
  if (m_rootfile->GetDirectory(sectordir.c_str())) {
    // cout << " Sector Histograms already built for : Region "<<region
    // 	 <<" side "<<side<<" Sector "<<sector<<endl;
    return;
  } 

  std::vector<MDTName> chamberList = GetChamberList(region, side, sector);

  // Sort Chambers By chamberType
  std::stable_sort(chamberList.begin(), chamberList.end(), sortMdtChambersByName() ) ;
  int numTotChambers=chamberList.size();

  string previousChamberType = "XXX";
  std::vector<MDTName> chamberListPerType[7];
  int totChamberTypes = 0;
  for (int ichamber = 0; ichamber<numTotChambers; ichamber++ ) {
    if ( chamberList[ichamber].getName() != previousChamberType) {
      previousChamberType = chamberList[ichamber].getName();
      totChamberTypes++;
    }
    chamberListPerType[totChamberTypes-1].push_back(chamberList[ichamber]);
  }


  ToString ts;
  //bool smallSector = (sector/2)*2==sector;
  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string sector_name="Sector"+sectorString;
  string sector_title="Sector "+sectorString;
   
  // string motherdir="/MDT/Barrel_"+side;
  string motherdir= GetMdtDirectoryName(region,side);
  m_rootfile->cd(motherdir.c_str());
  TH1F * h;
  TH1F * h1;
  TH2F * h2;
   
  TDirectoryFile * sector_dir = new TDirectoryFile(sector_name.c_str(),sector_title.c_str());
  TDirectoryFile * overview = new TDirectoryFile("OVERVIEW","OVERVIEW","",sector_dir);
   
  overview->cd();
  string histoTitPart2 = region+"_"+side+" Sector "+sectorString;
  string histoType;
  string histoTitle;

  int nbinMultipl=50;
  float xminMultipl=0.5; 
  float xmaxMultipl=50.5;
  ///////////////////////////////////////////////////////////////////////
    // DEFINITION OF HISTOGRAMS in directory /MDT/<region>_{A,C}/SectorXX/OVERVIEW :
    //
//     histoType="HitMultiplicity";
//     histoTitle = histoType+histoTitPart2;
//     TH1F * HitMultiplicity = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinMultipl, xminMultipl, xmaxMultipl);
//     HitMultiplicity->GetXaxis()->SetTitle("Tot nr. hits");
    // 
    //    histoType="HitMultiplicityAdcCut";
    //    histoTitle = histoType+histoTitPart2;
    //    TH1F * HitMultiplicityAdcCut = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinMultipl, xminMultipl, xmaxMultipl);
    // 
    //   histoType="HitMultiplicityVsEvent";
    //   histoTitle = histoType+histoTitPart2;
    //   TH2F * HitMultiplicityVsEvent = new TH2F(histoType.c_str(),histoTitle.c_str(),200,0.,2000000.,nbinMultipl, xminMultipl, xmaxMultipl);
    //
    // - Occupancies
    //
    histoType="z_HitsVsMezzanine";
    histoTitle = "HitsVsMezzanine "+histoTitPart2;
    TH2F * HitsVsMezzanine = new TH2F(histoType.c_str(),histoTitle.c_str(),
				      numTotChambers,0.,(float)numTotChambers,20,-0.5,19.5);
    HitsVsMezzanine->GetYaxis()->SetTitle("Mezzanine nr.");

    string chamberType;
    for (int ichamber = 0; ichamber<numTotChambers; ichamber++ ) {
      string chamberName = chamberList[ichamber].getOnlineName();
      HitsVsMezzanine->GetXaxis()->SetBinLabel(ichamber+1,chamberName.c_str());
    }

    float TDCmin=0.; 
    float TDCmax=3000.;
    int nbinTDC=300;
    //
    histoType="TDC_AllChambers_Inner";
    histoTitle = histoType+" "+histoTitPart2;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
    h->GetXaxis()->SetTitle("TDC counts");
    //   
    histoType="TDC_AllChambers_Middle";
    histoTitle = histoType+" "+histoTitPart2;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
    h->GetXaxis()->SetTitle("TDC counts");
    //   
    histoType="TDC_AllChambers_Outer";
    histoTitle = histoType+" "+histoTitPart2;
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
    h->GetXaxis()->SetTitle("TDC counts");
    //
    if(region=="Endcap") {
      histoType="TDC_AllChambers_extra";
      histoTitle = histoType+" "+histoTitPart2;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
      h->GetXaxis()->SetTitle("TDC counts"); 
    }
    //
    if(region=="Barrel") {
      histoType="z_MDT_RPC";
      histoTitle = "Distance BM Segment to RPC "+histoTitPart2;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,-50.,50.);
      h->GetXaxis()->SetTitle("Distance MDT segment to RPC hit (mm)");
    }

    if(region=="Endcap") {
	  histoType="z_MDT_TGC";
	  histoTitle = "Distance EM Segment to TGC "+histoTitPart2;
	  h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,-50.,50.);
	  h->GetXaxis()->SetTitle("Distance MDT segment to TGC hit (mm)");
    }

    //
    /*   
	 histoType="TDC_AllChambers_Inner_AdcCut";
	 histoTitle = histoType+" "+histoTitPart2;
	 h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
	 //
	 histoType="TDC_AllChambers_Middle_AdcCut";
	 histoTitle = histoType+" "+histoTitPart2;
	 h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
	 //
	 histoType="TDC_AllChambers_Outer_AdcCut";
	 histoTitle = histoType+" "+histoTitPart2;
	 h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
	 //
	 */

    int etaMinI = 99;    // Inner chambers
    int etaMaxI = -99;   
    int etaMinM = 99;    // Middle
    int etaMaxM = -99;
    int etaMinO = 99;    // Outer
    int etaMaxO = -99;
    int etaMinE = 99;    // Extra
    int etaMaxE = -99;

    int tubeMaxBM[10];
    for (int i=0;i<10;i++) tubeMaxBM[i]=1;
    int etaMinBM = 99;
    int etaMaxBM = -99;
    int tubeMaxEM[10];
    for (int i=0;i<10;i++) tubeMaxEM[i]=1;
    int etaMinEM = 99;
    int etaMaxEM = -99;
    int etaBins=98;
    float fetaMin=-24.5;
    float fetaMax=24.5;
    for (int ichamber=1; ichamber<=totChamberTypes; ichamber++){
      chamberType=chamberListPerType[ichamber-1].at(0).getName();
      //
      // check if it is a reasonable name !
      // 
      if ( !(chamberType.substr(0,1)=="B" || chamberType.substr(0,1)=="E") ) {
	//	cout << "HistogramManager::buildSector WARNING : unknown chamberType " << chamberType<<endl;
	continue;
      }
      //
      // FIND HERE etaMin and etaMax from the list chamberListPerType[ichamber-1]
      //
      int etaMin = 9;
      int etaMax = -9;

      for (unsigned int ic=0; ic<chamberListPerType[ichamber-1].size(); ic++){
	int etaic = chamberListPerType[ichamber-1].at(ic).getOnlineEta();
	if (etaic < etaMin ) etaMin = etaic; 
	if (etaic > etaMax ) etaMax = etaic; 
	if (chamberType.substr(0,2)=="BM" ) {
	  if (etaic < etaMinBM ) etaMinBM = etaic; 
	  if (etaic > etaMaxBM ) etaMaxBM = etaic;
	  tubeMaxBM[etaic]=56; // HERE SUBSTITUTE WITH tubeMax OBTAINED FROM MdtChamber
	} 
	if (chamberType.substr(0,2)=="EM" ) {
	  if (etaic < etaMinEM ) etaMinEM = etaic; 
	  if (etaic > etaMaxEM ) etaMaxEM = etaic;
	  tubeMaxEM[etaic]=64; // HERE SUBSTITUTE WITH tubeMax OBTAINED FROM MdtChamber
	} 
	if (chamberType.substr(1,1)=="I" ) {
	  if (etaic < etaMinI ) etaMinI = etaic; 
	  if (etaic > etaMaxI ) etaMaxI = etaic;
	} 
	if (chamberType.substr(1,1)=="M" ) {
	  if (etaic < etaMinM ) etaMinM = etaic; 
	  if (etaic > etaMaxM ) etaMaxM = etaic;
	} 
	if (chamberType.substr(1,1)=="O" ) {
	  if (etaic < etaMinO ) etaMinO = etaic; 
	  if (etaic > etaMaxO ) etaMaxO = etaic;
	} 
	if (chamberType.substr(1,1)=="E" ) {
	  if (etaic < etaMinE ) etaMinE = etaic; 
	  if (etaic > etaMaxE ) etaMaxE = etaic;
	} 
      }
    
      etaBins = 2*(etaMax-etaMin+1);
      fetaMin = (float)etaMin-0.5;
      fetaMax = (float)etaMax+0.5;
      histoType="HitsOnSegment"+chamberType;
      histoTitle = histoType+" "+histoTitPart2;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinMultipl, xminMultipl, xmaxMultipl);
      h->SetAxisRange(xminMultipl,20.5); 
      //
      if ( (chamberType=="BOG" || chamberType=="BOF") ) {
         histoType="t0PerMLBOGBOF";
         histoTitle = histoType+" "+histoTitPart2;
         if (side=="A") h = new TH1F(histoType.c_str(),histoTitle.c_str(),18,-0.5,8.5); // BOG0 only for side A
         if (side=="C") h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,8.5);
         h->GetXaxis()->SetTitle("eta_id");
         h->GetYaxis()->SetTitle("t0 (ns)");
         h->SetAxisRange(200.,1000.,"Y"); 
         h->SetMarkerStyle(20);
         h->SetMarkerSize(1.0);
      //
         histoType="tdriftPerMLBOGBOF";
         histoTitle = histoType+" "+histoTitPart2;
         if (side=="A") h = new TH1F(histoType.c_str(),histoTitle.c_str(),18,-0.5,8.5); // BOG0 only for side A
         if (side=="C") h = new TH1F(histoType.c_str(),histoTitle.c_str(),16,0.5,8.5);
         h->GetXaxis()->SetTitle("eta_id");
         h->GetYaxis()->SetTitle("tdrift (ns)");
         h->SetAxisRange(600.,800.,"Y"); 
         h->SetMarkerStyle(20);
         h->SetMarkerSize(1.0);

      } else {
         histoType="t0PerML"+chamberType;
         histoTitle = histoType+" "+histoTitPart2;
         h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBins,fetaMin,fetaMax);
         h->GetXaxis()->SetTitle("eta_id");
         h->GetYaxis()->SetTitle("t0 (ns)");
         h->SetAxisRange(200.,1000.,"Y"); 
         h->SetMarkerStyle(20);
         h->SetMarkerSize(1.0);
      //
         histoType="tdriftPerML"+chamberType;
         histoTitle = histoType+" "+histoTitPart2;
         h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBins,fetaMin,fetaMax);
         h->GetXaxis()->SetTitle("eta_id");
         h->GetYaxis()->SetTitle("tdrift (ns)");
         h->SetAxisRange(600.,800.,"Y"); 
         h->SetMarkerStyle(20);
         h->SetMarkerSize(1.0);
      }
    }//ichamber

    int etaBinsX;
    float fetaMinX;
    float fetaMaxX;

    etaBinsX = 2*(etaMaxI-etaMinI+1);
    fetaMinX = (float)etaMinI-0.5;
    fetaMaxX = (float)etaMaxI+0.5;
    histoType="HitsPerML_Inner";
    histoTitle = histoType+" "+histoTitPart2;
  //h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBins,fetaMin,fetaMax);
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBinsX,fetaMinX,fetaMaxX);
    h->GetXaxis()->SetTitle("eta_id");
    //
    etaBinsX = 2*(etaMaxM-etaMinM+1);
    fetaMinX = (float)etaMinM-0.5;
    fetaMaxX = (float)etaMaxM+0.5;
    histoType="HitsPerML_Middle";
    histoTitle = histoType+" "+histoTitPart2;
  //h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBins,fetaMin,fetaMax);
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBinsX,fetaMinX,fetaMaxX);
    h->GetXaxis()->SetTitle("eta_id");
    //
    etaBinsX = 2*(etaMaxO-etaMinO+1);
    fetaMinX = (float)etaMinO-0.5;
    fetaMaxX = (float)etaMaxO+0.5;
    histoType="HitsPerML_Outer";
    histoTitle = histoType+" "+histoTitPart2;
  //h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBins,fetaMin,fetaMax);
    h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBinsX,fetaMinX,fetaMaxX);
    h->GetXaxis()->SetTitle("eta_id");

   if(region=="Endcap")
    { 
      etaBinsX = 2*(etaMaxE-etaMinE+1);
      fetaMinX = (float)etaMinE-0.5;
      fetaMaxX = (float)etaMaxE+0.5;
      histoType="HitsPerML_extra";
      histoTitle = histoType+" "+histoTitPart2;
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),etaBinsX,fetaMinX,fetaMaxX);
      h->GetXaxis()->SetTitle("eta_id");
    }
    ///////////////////////////////////////////////////////////////////////
      //
      ///////////////////////////////////////////////////////////////////////
      // DEFINITION OF HISTOGRAMS in directory /MDTvsRPC/Barrel_{A,C}/SectorXX :
      //
      if (region=="Barrel") {
	motherdir= GetTDaqDirectoryName(region,side);
	m_rootfile->cd(motherdir.c_str());
   
	TDirectoryFile * sector_tdaqdir = new TDirectoryFile(sector_name.c_str(),sector_title.c_str());
	sector_tdaqdir->cd();
	histoType="MDT_RPC";
	histoTitle = "Distance Segment to RPC "+histoTitPart2;
	h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,-50.,50.);
	h->GetXaxis()->SetTitle("Distance MDT segment to RPC hit (mm)");
	// 
	//   histoType="MDT_RPC_Vs_Event";
	//   histoTitle = "Distance Segment to RPC Vs Event "+histoTitPart2;
	//   h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,0.,2000000.,100,-50.,50.);
	//
	//  GLOBAL TIME FIT HISTOGRAMS :
	//

	histoType="ResidualsVsRadius";
	histoTitle = "Residuals Vs Radius "+histoTitPart2;
	h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),30,-15,15.,200,-5.,5.);
	h2->GetXaxis()->SetTitle("Radius (mm)");
	h2->GetYaxis()->SetTitle("Residuals (mm)");

	histoType="Segment_chi2";
	histoTitle = "eta_id Vs Segment_chi2 "+histoTitPart2;
	h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),50, 0.,50.,6,0.5,6.5);
	h2->GetXaxis()->SetTitle("chi2/DoF");
	h2->GetYaxis()->SetTitle("BM chamber eta_id");

	histoType="Segment_ProbChi2";
	histoTitle = "Segment_ProbChi2 "+histoTitPart2;
	h1 = new TH1F(histoType.c_str(),histoTitle.c_str(),50,0.,1.);
	h1->GetXaxis()->SetTitle("Prob(chi2,DoF)");

	histoType="Residuals";
	histoTitle = "eta_id Vs Residuals "+histoTitPart2;
	h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,-5.,5.,6,0.5,6.5);
	h2->GetXaxis()->SetTitle("Residuals (mm)");
	h2->GetYaxis()->SetTitle("BM chamber eta_id");

	// histoType="Hit_Residuals";
	// histoTitle = "eta_id Vs Hit Residuals "+histoTitPart2;
	// h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,-5.,5.,6,0.5,6.5);
	// h2->GetXaxis()->SetTitle("Hit Residuals (mm)");
	// h2->GetYaxis()->SetTitle("BM chamber eta_id");

	histoType="HitsOnSegment";
	histoTitle = "eta_id Vs HitsOnSegment "+histoTitPart2;
	h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),10,0.5,10.5,6,0.5,6.5);
	h2->GetXaxis()->SetTitle("nr. of hits on Segment");
	h2->GetYaxis()->SetTitle("BM chamber eta_id");

	histoType="HitRadius";
	histoTitle = "Hit Radius "+histoTitPart2;
	h = new TH1F(histoType.c_str(),histoTitle.c_str(),80,-20.,20.);
	h->GetXaxis()->SetTitle("Hit Radius (mm)");

	for (int ic=etaMinBM; ic<=etaMaxBM; ic++){
	  histoType="TrigTime_BM_eta_"+ts(ic);
	  histoTitle = histoType+" "+histoTitPart2;
	  h = new TH1F(histoType.c_str(),histoTitle.c_str(),768,0.,1200.);
	  h->GetXaxis()->SetTitle("TriggerTime (ns)");

	  histoType="TrigTimeVsROdistance_BM_eta_"+ts(ic);
	  histoTitle = histoType+" "+histoTitPart2;
	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),72, 0.,3600.,192,0.,1200.);
	  h2->GetXaxis()->SetTitle("Position along the wires (mm)");
	  h2->GetYaxis()->SetTitle("TriggerTime (ns)");

	  histoType="TriggerCoverage_BM_eta_"+ts(ic);
	  histoTitle = histoType+" "+histoTitPart2;

	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),72, 0.,3600.,
			tubeMaxBM[ic],0.5,(float)tubeMaxBM[ic]+0.5);
	  h2->GetXaxis()->SetTitle("Position along the wires (mm)");
	  h2->GetYaxis()->SetTitle("Position perp to the wires (mm)");
	}
	// DEBUG HISTOGRAMS TO BE REMOVED !
	bool detailedAnalysis = false;
	if (detailedAnalysis) { 
	  histoType="a_old_Vs_a_new";
	  histoTitle = histoType;
	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,-4.,4.,200,-4.,4.);
	  h2->GetXaxis()->SetTitle("aSegment_new");
	  h2->GetYaxis()->SetTitle("aSegment_old");

	  histoType="b_old_Vs_b_new";
	  histoTitle = histoType;
	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),500,-50.,200.,500,-50.,200.);
	  h2->GetXaxis()->SetTitle("bSegment_new");
	  h2->GetYaxis()->SetTitle("bSegment_old");

	  histoType="a_diff";
	  histoTitle = histoType;
	  h1 = new TH1F(histoType.c_str(),histoTitle.c_str(),1000,-0.5,0.5);
	  h1->GetXaxis()->SetTitle("aSegment_new-aSegment_old");

	  histoType="b_diff";
	  histoTitle = histoType;
	  h1 = new TH1F(histoType.c_str(),histoTitle.c_str(),1000,-10.,10.);
	  h1->GetXaxis()->SetTitle("bSegment_new-bSegment_old");

	}
      }
  

      ///////////////////////////////////////////////////////////////////////
	//
	///////////////////////////////////////////////////////////////////////
	// DEFINITION OF HISTOGRAMS in directory /MDTvsTGC/Endcap_{A,C}/SectorXX :
	//
	if (region=="Endcap") {
	  motherdir= GetTDaqDirectoryName(region,side);
	  m_rootfile->cd(motherdir.c_str());
   
	  TDirectoryFile * sector_tdaqdir = new TDirectoryFile(sector_name.c_str(),sector_title.c_str());
	  sector_tdaqdir->cd();
	  histoType="MDT_TGC";
	  histoTitle = "Distance Segment to TGC "+histoTitPart2;
	  h = new TH1F(histoType.c_str(),histoTitle.c_str(),100,-50.,50.);
	  h->GetXaxis()->SetTitle("Distance MDT segment to TGC hit (mm)");
	  // 
	  //   histoType="MDT_TGC_Vs_Event";
	  //   histoTitle = "Distance Segment to TGC Vs Event "+histoTitPart2;
	  //   h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,0.,2000000.,100,-50.,50.);
	  //
	  //  GLOBAL TIME FIT HISTOGRAMS :
	  //

	  histoType="Track_chi2";
	  histoTitle = "eta_id Vs Track_chi2 "+histoTitPart2;
	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),50, 0.,50.,6,0.5,6.5);
	  h2->GetXaxis()->SetTitle("chi2/DoF");
	  h2->GetYaxis()->SetTitle("EM chamber eta_id");

	  histoType="Residuals";
	  histoTitle = "eta_id Vs Residuals "+histoTitPart2;
	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,-5.,5.,6,0.5,6.5);
	  h2->GetXaxis()->SetTitle("Residuals (mm)");
	  h2->GetYaxis()->SetTitle("EM chamber eta_id");

	  // histoType="Hit_Residuals";
	  // histoTitle = "eta_id Vs Hit Residuals "+histoTitPart2;
	  // h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),200,-5.,5.,6,0.5,6.5);
	  // h2->GetXaxis()->SetTitle("Hit Residuals (mm)");
	  // h2->GetYaxis()->SetTitle("EM chamber eta_id");

          histoType="Hit_Res_Chi2DoF";
          histoTitle = "eta_id Vs Hit Residuals Chi2/DoF"+histoTitPart2;
          h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),100,0.,50.,6,0.5,6.5);
          h2->GetXaxis()->SetTitle("Hit Residuals Chi2/DoF");
          h2->GetYaxis()->SetTitle("EM chamber eta_id");

	  histoType="HitsOnSegment";
	  histoTitle = "eta_id Vs HitsOnSegment "+histoTitPart2;
	  h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),10,0.5,10.5,6,0.5,6.5);
	  h2->GetXaxis()->SetTitle("nr. of hits on Segment");
	  h2->GetYaxis()->SetTitle("EM chamber eta_id");

	  histoType="HitRadius";
	  histoTitle = "Hit Radius "+histoTitPart2;
	  h = new TH1F(histoType.c_str(),histoTitle.c_str(),80,-20.,20.);
	  h->GetXaxis()->SetTitle("Hit Radius (mm)");

	  for (int ic=etaMinEM; ic<=etaMaxEM; ic++){
	    histoType="TrigTime_EM_eta_"+ts(ic);
	    histoTitle = histoType+" "+histoTitPart2;
	    h = new TH1F(histoType.c_str(),histoTitle.c_str(),768,0.,1200.);
	    h->GetXaxis()->SetTitle("TriggerTime (ns)");

	    histoType="TrigTimeVsROdistance_EM_eta_"+ts(ic);
	    histoTitle = histoType+" "+histoTitPart2;
	    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),72, 0.,3600.,192,0.,1200.);
	    h2->GetXaxis()->SetTitle("Position along the wires (mm)");
	    h2->GetYaxis()->SetTitle("TriggerTime (ns)");

	    histoType="TriggerCoverage_EM_eta_"+ts(ic);
	    histoTitle = histoType+" "+histoTitPart2;

	    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),72, 0.,3600.,
			  tubeMaxEM[ic],0.5,(float)tubeMaxEM[ic]+0.5);
	    h2->GetXaxis()->SetTitle("Position along the wires (mm)");
	    h2->GetYaxis()->SetTitle("Position perp to the wires (mm)");
	  }
	}
}
  
void HistogramManager::buildChamberHistos(MDTName chamb) {


  int eta_id = chamb.getOfflineEta();
  int phi_id = chamb.getOfflineSector();
  int sector=chamb.getOnlineSector();

  ToString ts;

  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string sector_name="Sector"+sectorString;
  string sector_title="Sector "+sectorString;

  string region=chamb.getRegion();
  string side=chamb.getSide();
  
  string chamberName=chamb.getOnlineName();
  string chamberType=chamb.getName();

  // string motherdir="/MDT/Barrel_"+side+"/"+sector_name;
  string motherdir=GetMdtDirectoryName(region, side, sector);
  m_rootfile->cd(motherdir.c_str());

  TDirectoryFile * chamber_dir = new TDirectoryFile(chamberName.c_str(),chamberName.c_str());
  TDirectoryFile * occupancy_dir = new TDirectoryFile("Occupancy","occupancy","",chamber_dir);
  TDirectoryFile * efficiency_dir = new TDirectoryFile("Efficiency","efficiency","",chamber_dir);
  TDirectoryFile * deadStatus_dir = new TDirectoryFile("DeadStatus","DeadStatus","",chamber_dir);
  TDirectoryFile * expert_dir = new TDirectoryFile("Expert","Expert","",chamber_dir);
  TDirectoryFile * noiseStatus_dir = new TDirectoryFile("NoiseStatus","NoiseStatus","",chamber_dir);
  chamber_dir->cd();
   
  //
  // The following values should be read in from a Chamber Layout File
  // 
  // int numML = 2;
  // int numLayersPerML = 4;
  // int numTubesPerLayer = 36;
  // int numMezzanines = 10;

  int numTubesPerLayer[2], numOfMezzPerML[2];
  numTubesPerLayer[0] = 0;
  numTubesPerLayer[1] = 0;
  numOfMezzPerML[0] = 0;
  numOfMezzPerML[1] = 0;
  int numLayersPerML, numML, numTubesPerMezz, numMezzanines, numMaxTubesPerLayer;
  // int nTubes, numLayers;
  int numTotTubesML[2];
  int tubeNumberOffsetML[2];
  //
  // tubeNumberOffsetML1, tubeNumberOffsetML1 MUST BE SET  to match online tube numbering
  // I think a method in HistogramManager could be used. (or in phi_eta_utils......)
  // for the moment do not add any offset:
  tubeNumberOffsetML[0] = GetTubeOffsetML1(chamberName);
  tubeNumberOffsetML[1] = 0;

  if ( m_MdtIdHelper ) {
    Identifier  station_id = m_MdtIdHelper->elementID(chamberType, eta_id, phi_id);
    numML = m_MdtIdHelper->numberOfMultilayers(station_id);
    Identifier  MdtML1_id = m_MdtIdHelper->multilayerID(station_id,1);
    Identifier  MdtML2_id;
    if ( numML>1) MdtML2_id = m_MdtIdHelper->multilayerID(station_id,2);
    numLayersPerML = m_MdtIdHelper->tubeLayerMax(MdtML1_id) - m_MdtIdHelper->tubeLayerMin(MdtML1_id) + 1; 
    if (chamberName.substr(0,4)=="BIS8") numLayersPerML=3; // PATCH TO MdtIdHelper BUG (should be fixed in next release)
    numTubesPerLayer[0] = m_MdtIdHelper->tubeMax(MdtML1_id) - m_MdtIdHelper->tubeMin(MdtML1_id) + 1;

    if ( numML>1 ) numTubesPerLayer[1] = m_MdtIdHelper->tubeMax(MdtML2_id) - m_MdtIdHelper->tubeMin(MdtML2_id) + 1;

    numMaxTubesPerLayer = numTubesPerLayer[0];
    if (numTubesPerLayer[1]>numTubesPerLayer[0]) numMaxTubesPerLayer = numTubesPerLayer[1];
    // nTubes = numLayersPerML*(numTubesPerLayer[0]+numTubesPerLayer[1]);
    // numLayers = numML*numLayersPerML;
    numTubesPerMezz = 8;
    if (numLayersPerML==4) numTubesPerMezz = 6;
    numOfMezzPerML[0] = numTubesPerLayer[0]/numTubesPerMezz;
    numOfMezzPerML[1] = numTubesPerLayer[1]/numTubesPerMezz;
    numMezzanines = numOfMezzPerML[0]+numOfMezzPerML[1];
  } else { 
    numLayersPerML=3;
    numML = GetChamberNumOfML(chamberName);
    numTubesPerMezz = GetChamberTubesPerMezz(chamberName);
    numOfMezzPerML[0] = GetChamberNumOfMezzPerML(chamberName);
    if (numML>1) numOfMezzPerML[1] = numOfMezzPerML[0];
    if (numTubesPerMezz==6) numLayersPerML=4;
    numTubesPerLayer[0]=numTubesPerMezz*numOfMezzPerML[0];
    numTubesPerLayer[1]=numTubesPerMezz*numOfMezzPerML[1];
    numMaxTubesPerLayer = numTubesPerLayer[0];
    if (numTubesPerLayer[1]>numTubesPerLayer[0]) numMaxTubesPerLayer = numTubesPerLayer[1];
    numMezzanines = numOfMezzPerML[0]+numOfMezzPerML[1];
    // nTubes = numLayersPerML*(numTubesPerLayer[0]+numTubesPerLayer[1]);
    // numLayers = numML*numLayersPerML;
  }
  numTotTubesML[0]=numLayersPerML*numTubesPerLayer[0];
  numTotTubesML[1]=numLayersPerML*numTubesPerLayer[1];

  //float tubeMin= 0.5;
  //float tubeMax= (float)nTubes + 0.5;

  float allTubesMin = 0.5;
  float allTubesMax = (float) numTotTubesML[0];
  if (numTotTubesML[0]<numTotTubesML[1]) allTubesMax = (float) numTotTubesML[1];
  if (numML>1) allTubesMax = 2.*allTubesMax;
  allTubesMax = allTubesMax + 0.5; 
  int nbinAllTubes = (int) (allTubesMax-allTubesMin);

  // //cout << "numML ,numLayersPerML, numTotTubesML[0] , numTotTubesML[1] , allTubesMin , allTubesMax"
  //      << numML <<" "<<numLayersPerML<<" " << numTotTubesML[0] <<" "<< numTotTubesML[1] <<" "<< allTubesMin
  //      <<" "<< allTubesMax << endl;

  int nbinMultipl=50;
  float xminMultipl=0.; 
  float xmaxMultipl=50.;

  float TDCmin=0.; 
  float TDCmax=3000.;
  int nbinTDC=300;
  int nbinTDCcoarse=60;

  float ADCmin=0.; 
  float ADCmax=400.;
  int nbinADC=200;
  int nbinADCcoarse=40;

  TH1F * h;
  TH1F * h1;
  TH2F * h2;

  string histoTitPart2=chamberName;
  string histoType;
  string histoTitle;
  ///////////////////////////////////////////////////////////////////////////////
    //
    // DEFINITION OF HISTOGRAMS in directory /MDT/Barrel_{A,C}/SectorXX/<chamber> :
    //
    // - Multiplicities
    //
    histoType="B_ChamberHitMultiplicity";
    histoTitle = "ChamberHitMultiplicity "+histoTitPart2;
    TH1F * HitMult = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinMultipl, xminMultipl, xmaxMultipl);
    HitMult->GetXaxis()->SetTitle("Chamber tot nr. hits");
    // 
    //    histoType="ChamberHitMultiplicityAdcCut";
    //    histoTitle = histoType+" "+histoTitPart2;
    //    TH1F * HitMultAdcCut = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinMultipl, xminMultipl, xmaxMultipl);
    // 
    histoType="D_ChamberHitsOnSegment";
    histoTitle = "ChamberHitsOnSegment "+histoTitPart2;
    TH1F * HitsOnSeg = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinMultipl, xminMultipl, xmaxMultipl);
    HitsOnSeg->GetXaxis()->SetTitle("nr. hits on segment");
    HitsOnSeg->SetAxisRange(xminMultipl,20.5);
    //
    //   histoType="ChamberHitMultiplicityVsEvent";
    //   histoTitle = histoType+" "+histoTitPart2;
    //  TH2F * HitMultVsEv = new TH2F(histoType.c_str(),histoTitle.c_str(),200,0.,2000000, nbinMultipl, xminMultipl, xmaxMultipl);
    // 
    // - occupancies
    //

    histoType="b_DeadTubeStatus";
    histoTitle = "DeadTubeStatus "+histoTitPart2;
    TH1F * DeadTubeStatus = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
    DeadTubeStatus->GetXaxis()->SetTitle("Tube_id");

    histoType="a_ChamberHitOccupancy";
    histoTitle = "ChamberHitOccupancy "+histoTitPart2;
    TH2F * ChamberHits = new TH2F(histoType.c_str(),histoTitle.c_str(),
				  numMaxTubesPerLayer,0.5,numMaxTubesPerLayer+0.5,10,0.5,10.5);
    ChamberHits->GetXaxis()->SetTitle("Tube nr.");
    ChamberHits->GetYaxis()->SetTitle("Layer");
    //

    histoType="a_HitsPerTube";
    histoTitle = "HitsPerTube "+histoTitPart2;
    TH1F * HitsPerTube = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
    HitsPerTube->GetXaxis()->SetTitle("Tube_id");
    // 
    histoType="a_HitsPerTubeAdcCut";
    histoTitle = "HitsPerTubeAdcCut "+histoTitPart2;
    TH1F * HitsPerTubeAdcCut = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
    HitsPerTubeAdcCut->GetXaxis()->SetTitle("Tube_id");
    // 
    //  - efficiencies
    // 
    /*
      histoType="EfficiencyPerLayer";
      histoTitle = histoType+histoTitPart2;
      TH1F * EfficiencyPerLayer = new TH1F(histoType.c_str(),histoTitle.c_str(),numLayers,0.5,numLayers+0.5);
      EfficiencyPerLayer->SetMarkerStyle(20);
      EfficiencyPerLayer->SetMarkerSize(1.0);
      EfficiencyPerLayer->Sumw2();
    */
    //efficiency_dir->cd();

    histoType="b_EfficiencyPerTube";
    histoTitle = "EfficiencyPerTube "+histoTitPart2;
    TH1F * EfficiencyPerTube = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
    EfficiencyPerTube->GetXaxis()->SetTitle("Tube_id");
    EfficiencyPerTube->GetYaxis()->SetTitle("efficiency");
    EfficiencyPerTube->SetMarkerStyle(20);
    EfficiencyPerTube->SetMarkerSize(1.0);
    EfficiencyPerTube->Sumw2();


    //
    // - RESIDUALS
    // 
    histoType="b_SegResidVsRadius";
    histoTitle = "Residuals Vs Radius "+histoTitPart2;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),30,-15,15.,200,-5.,5.);
    h2->GetXaxis()->SetTitle("Radius (mm)");
    h2->GetYaxis()->SetTitle("Residuals (mm)");
    // 
    histoType="b_SegmentResidual";
    histoTitle = "Segment Residuals "+histoTitPart2;
    h1 = new TH1F(histoType.c_str(),histoTitle.c_str(),200,-5.,5.);
    h1->GetXaxis()->SetTitle("Residuals (mm)");
    //
    // - Segment Prob(chi2,ndeg)
    //
    histoType="c_Segment_ProbChi2";
    histoTitle = "Segment_ProbChi2 "+histoTitPart2;
    h1 = new TH1F(histoType.c_str(),histoTitle.c_str(),50,0.,1.);
    h1->GetXaxis()->SetTitle("Prob(chi2,DoF)");

    //
    // - TDC and ADC 
    // 
    histoType="A_TDC_ML1";
    histoTitle = "TDC_ML1 "+histoTitPart2;
    TH1F * TDC_ML1 = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
    TDC_ML1->GetXaxis()->SetTitle("TDC counts");
    // 
    histoType="A_TDC_ML2";
    histoTitle = "TDC_ML2 "+histoTitPart2;
    TH1F * TDC_ML2 = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinTDC,TDCmin,TDCmax);
    TDC_ML2->GetXaxis()->SetTitle("TDC counts");
    //
    histoType="C_ADC_ML1";
    histoTitle = "ADC_ML1 "+histoTitPart2;
    TH1F * ADC_ML1 = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinADC,ADCmin,ADCmax);
    ADC_ML1->GetXaxis()->SetTitle("ADC counts");
    // 
    histoType="C_ADC_ML2";
    histoTitle = "ADC_ML2 "+histoTitPart2;
    TH1F * ADC_ML2 = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinADC,ADCmin,ADCmax);
    ADC_ML2->GetXaxis()->SetTitle("ADC counts");
    // 
    // - TDC Vs ADC spectra per Mezzanine :
    //
    for (int imezz=0; imezz<numMezzanines; imezz++) {
      histoType="tdc_Vs_adc_mezz_"+ts(imezz);
      histoTitle = histoType+" "+histoTitPart2;
      h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),nbinADCcoarse,ADCmin,ADCmax,nbinTDCcoarse,TDCmin,TDCmax);
      h2->GetXaxis()->SetTitle("ADC counts");
      h2->GetYaxis()->SetTitle("TDC counts");
    }

   
    ////////////////////////////////////////////////////////////////////////////////
    //
    // Histograms in the subdirectories: ......
    //

    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /*:::::::::::::::::::::::EXPERT:::::::::::::::::::::::::::*/
    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    expert_dir->cd();

    histoType="ChamberYZOccupancy";
    histoTitle = histoType+" "+histoTitPart2;
    TH2F * ChamberHitsYZ = new TH2F(histoType.c_str(),histoTitle.c_str(),
				    220,0.,2200.,100,0.,1000.);
    ChamberHitsYZ->GetXaxis()->SetTitle("Hit_y (mm) ");
    ChamberHitsYZ->GetYaxis()->SetTitle("Hit_z (mm) ");
    //

    histoType="DeadTubeRefMap";
    histoTitle = histoType+" "+histoTitPart2;
    TH1F * href = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
    href->GetXaxis()->SetTitle("Tube_id");
    int tubesMLextended = nbinAllTubes/2;
    if (numTubesPerLayer[1]==0) tubesMLextended = nbinAllTubes;
    int tubesLayerExtended = tubesMLextended/numLayersPerML;
    for (int ibin=1;ibin<=nbinAllTubes;ibin++) {

      // cout << " ibin, nbinAllTubes, numTotTubesML[0], numTubesPerLayer[1] " 
      //      << ibin<<" "<< nbinAllTubes<<" "<< numTotTubesML[0]<<" "<< numTubesPerLayer[1]<<endl;

      int binSign, layercode;
      if (ibin<=tubesMLextended) {
	binSign = -1;
	layercode = ((ibin-1)/tubesLayerExtended) + 1;
	layercode = layercode*10*binSign;
      } else {
	binSign = 1;
	layercode = ((ibin-tubesMLextended-1)/tubesLayerExtended) + 1;
	layercode = layercode*10*binSign;
      }

      // cout << " layercode "<< layercode << endl;

      href->SetBinContent(ibin,layercode);
    }

    setChamberCutOut(chamberName, href );
    //setChamberDisconnectedTubes(chamberName, href );

    histoType="DeadTubeMap";
    histoTitle = histoType+" "+histoTitPart2;

    h = (TH1F*) href->Clone(histoType.c_str());
    h->SetTitle(histoTitle.c_str());

    /*
      h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
      h->GetXaxis()->SetTitle("Tube_id");

      for (int ibin=1;ibin<=nbinAllTubes;ibin++) {
      int binSign, layercode;
      if (ibin<=numTotTubesML[0]) {
      binSign = -1;
      layercode = (ibin/(numTubesPerLayer[0]+1)) + 1;
      layercode = layercode*10*binSign;
      } else {
      binSign = 1;
      layercode = ((ibin-numTotTubesML[0])/(numTubesPerLayer[1])+1) +1;
      layercode = layercode*10*binSign;
      }
      h->SetBinContent(ibin,layercode);
      }
    */  



    histoType="ChamberDeadChannels";
    histoTitle = histoType+" "+histoTitPart2;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),
		  numMaxTubesPerLayer,0.5,numMaxTubesPerLayer+0.5,10,0.5,10.5);
    h2->GetXaxis()->SetTitle("Tube nr.");
    h2->GetYaxis()->SetTitle("Layer");
    //

     histoType="EffiResidVsRadius";
     histoTitle = histoType+" "+histoTitPart2;
     // h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),
     //		  30,0.,15.,128,-16.,16.);
     h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),
 		  30,0.,15.,640,-16.,16.);
     h2->GetXaxis()->SetTitle("Segment distance from wire (mm)");
     h2->GetYaxis()->SetTitle("Residual (mm)");
    //
     histoType="EfficiencyEntries";
     histoTitle = histoType+" "+histoTitPart2;
     h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
     h->GetXaxis()->SetTitle("Tube_id");
     h->GetYaxis()->SetTitle("Nr of events the tube should fire");

     histoType="EfficiencyCounts";
     histoTitle = histoType+" "+histoTitPart2;
     h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbinAllTubes,allTubesMin,allTubesMax);
     h->GetXaxis()->SetTitle("Tube_id");
     h->GetYaxis()->SetTitle("Nr. of events the tube HAS fired");


    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    /*::::::::::::::::::::DEADSTATUS::::::::::::::::::::::::::*/
    /*::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    deadStatus_dir->cd();

    histoType="ChamberDeadTubes";
    histoTitle = histoType+" "+histoTitPart2;
    h2 = new TH2F(histoType.c_str(),histoTitle.c_str(),numMaxTubesPerLayer,0.5,numMaxTubesPerLayer+0.5,10,0.5,10.5);
    h2->GetXaxis()->SetTitle("Tube nr.");
    h2->GetYaxis()->SetTitle("Layer");

    for (int ML=1;ML<=numML;ML++){
      for (int layer=1; layer<=numLayersPerML; layer++){
	int nbin=numTubesPerLayer[ML-1]; 
	float xmin =  0.5 + (float)tubeNumberOffsetML[ML-1];
	float xmax = (float)numTubesPerLayer[ML-1] + xmin;

	occupancy_dir->cd();
	histoType="HitsPerTube_ML"+ts(ML)+"_L"+ts(layer);
	histoTitle = histoType+" "+histoTitPart2;
	h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbin,xmin,xmax);
	h->GetXaxis()->SetTitle("Tube nr.");

	efficiency_dir->cd();
	histoType="EffiPerTube_ML"+ts(ML)+"_L"+ts(layer);
	histoTitle = histoType+" "+histoTitPart2;
	h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbin,xmin,xmax);
	h->GetXaxis()->SetTitle("Tube nr.");
	h->SetMarkerStyle(20);
	h->SetMarkerSize(1.0);
	deadStatus_dir->cd();
	histoType="DeadTubes_ML"+ts(ML)+"_L"+ts(layer);
	histoTitle = histoType+" "+histoTitPart2;
	h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbin,xmin,xmax);
	h->GetXaxis()->SetTitle("Tube nr.");
	noiseStatus_dir->cd();
	histoType="NoisePerTube_ML"+ts(ML)+"_L"+ts(layer);
	histoTitle = histoType+" "+histoTitPart2;
	h = new TH1F(histoType.c_str(),histoTitle.c_str(),nbin,xmin,xmax);
	h->GetXaxis()->SetTitle("Tube nr.");
      }
    }

    ///////////////////////////////////////////////////////////////////////
}


void HistogramManager::setChamberCutOut(string chamber, TH1F * href ) {

  // CHAMBERS WITH CUT-OUT 

  if (chamber=="BIR2A11" ) href->SetBinContent(28,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(29,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(30,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(58,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(59,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(60,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(88,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(89,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(90,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(118,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(119,0);
  if (chamber=="BIR2A11" ) href->SetBinContent(120,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(1,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(2,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(3,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(4,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(5,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(6,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(31,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(32,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(33,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(34,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(35,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(36,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(61,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(62,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(63,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(64,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(65,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(66,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(91,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(92,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(93,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(94,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(95,0);
  if (chamber=="BIR1A11" ) href->SetBinContent(96,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(28,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(29,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(30,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(58,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(59,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(60,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(88,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(89,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(90,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(118,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(119,0);
  if (chamber=="BIR2A15" ) href->SetBinContent(120,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(1,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(2,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(3,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(4,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(5,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(6,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(31,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(32,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(33,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(34,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(35,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(36,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(61,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(62,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(63,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(64,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(65,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(66,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(91,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(92,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(93,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(94,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(95,0);
  if (chamber=="BIR1A15" ) href->SetBinContent(96,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(1,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(2,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(3,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(31,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(32,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(33,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(61,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(62,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(63,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(91,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(92,0);
  if (chamber=="BIR4A11" ) href->SetBinContent(93,0);
/*
  if (chamber=="BIR3A11" ) href->SetBinContent(34,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(35,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(36,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(70,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(71,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(72,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(106,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(107,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(108,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(142,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(143,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(144,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(178,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(179,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(180,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(214,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(215,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(216,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(250,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(251,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(252,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(286,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(287,0);
  if (chamber=="BIR3A11" ) href->SetBinContent(288,0);
*/
  if (chamber=="BIR4A15" ) href->SetBinContent(1,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(2,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(3,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(31,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(32,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(33,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(61,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(62,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(63,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(91,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(92,0);
  if (chamber=="BIR4A15" ) href->SetBinContent(93,0);
/*
  if (chamber=="BIR3A15" ) href->SetBinContent(34,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(35,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(36,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(70,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(71,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(72,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(106,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(107,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(108,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(142,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(143,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(144,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(178,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(179,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(180,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(214,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(215,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(216,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(250,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(251,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(252,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(286,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(287,0);
  if (chamber=="BIR3A15" ) href->SetBinContent(288,0);
*/
  if (chamber=="BMS4A02" ) href->SetBinContent(41,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(42,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(43,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(44,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(45,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(46,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(47,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(48,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(89,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(90,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(91,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(92,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(93,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(94,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(95,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(96,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(137,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(138,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(139,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(140,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(141,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(142,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(143,0);
  if (chamber=="BMS4A02" ) href->SetBinContent(144,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(41,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(42,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(43,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(44,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(45,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(46,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(47,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(48,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(89,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(90,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(91,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(92,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(93,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(94,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(95,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(96,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(137,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(138,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(139,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(140,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(141,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(142,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(143,0);
  if (chamber=="BMS4A04" ) href->SetBinContent(144,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(41,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(42,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(43,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(44,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(45,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(46,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(47,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(48,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(89,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(90,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(91,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(92,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(93,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(94,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(95,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(96,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(137,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(138,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(139,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(140,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(141,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(142,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(143,0);
  if (chamber=="BMS4A06" ) href->SetBinContent(144,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(41,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(42,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(43,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(44,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(45,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(46,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(47,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(48,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(89,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(90,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(91,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(92,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(93,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(94,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(95,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(96,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(137,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(138,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(139,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(140,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(141,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(142,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(143,0);
  if (chamber=="BMS4A08" ) href->SetBinContent(144,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(41,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(42,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(43,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(44,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(45,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(46,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(47,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(48,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(89,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(90,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(91,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(92,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(93,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(94,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(95,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(96,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(137,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(138,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(139,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(140,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(141,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(142,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(143,0);
  if (chamber=="BMS4A10" ) href->SetBinContent(144,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(41,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(42,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(43,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(44,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(45,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(46,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(47,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(48,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(89,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(90,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(91,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(92,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(93,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(94,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(95,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(96,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(137,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(138,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(139,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(140,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(141,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(142,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(143,0);
  if (chamber=="BMS4A16" ) href->SetBinContent(144,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(22,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(23,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(24,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(46,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(47,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(48,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(70,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(71,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(72,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(94,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(95,0);
  if (chamber=="BIR5A11" ) href->SetBinContent(96,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(22,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(23,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(24,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(46,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(47,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(48,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(70,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(71,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(72,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(94,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(95,0);
  if (chamber=="BIR5A15" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(41,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(42,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(43,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(44,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(45,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(46,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(47,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(48,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(89,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(90,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(91,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(92,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(93,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(94,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(95,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(137,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(138,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(139,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(140,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(141,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(142,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(143,0);
  if (chamber=="BMS6A02" ) href->SetBinContent(144,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(41,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(42,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(43,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(44,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(45,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(46,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(47,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(48,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(89,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(90,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(91,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(92,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(93,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(94,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(95,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(137,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(138,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(139,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(140,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(141,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(142,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(143,0);
  if (chamber=="BMS6A04" ) href->SetBinContent(144,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(41,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(42,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(43,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(44,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(45,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(46,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(47,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(48,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(89,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(90,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(91,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(92,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(93,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(94,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(95,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(137,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(138,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(139,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(140,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(141,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(142,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(143,0);
  if (chamber=="BMS6A06" ) href->SetBinContent(144,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(41,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(42,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(43,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(44,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(45,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(46,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(47,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(48,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(89,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(90,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(91,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(92,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(93,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(94,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(95,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(137,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(138,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(139,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(140,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(141,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(142,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(143,0);
  if (chamber=="BMS6A08" ) href->SetBinContent(144,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(41,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(42,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(43,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(44,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(45,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(46,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(47,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(48,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(89,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(90,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(91,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(92,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(93,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(94,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(95,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(137,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(138,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(139,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(140,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(141,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(142,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(143,0);
  if (chamber=="BMS6A10" ) href->SetBinContent(144,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(41,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(42,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(43,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(44,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(45,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(46,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(47,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(48,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(89,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(90,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(91,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(92,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(93,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(94,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(95,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(96,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(137,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(138,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(139,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(140,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(141,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(142,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(143,0);
  if (chamber=="BMS6A16" ) href->SetBinContent(144,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(28,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(29,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(30,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(58,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(59,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(60,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(88,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(89,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(90,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(118,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(119,0);
  if (chamber=="BIR2C11" ) href->SetBinContent(120,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(1,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(2,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(3,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(4,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(5,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(6,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(31,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(32,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(33,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(34,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(35,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(36,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(61,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(62,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(63,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(64,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(65,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(66,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(91,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(92,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(93,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(94,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(95,0);
  if (chamber=="BIR1C11" ) href->SetBinContent(96,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(28,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(29,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(30,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(58,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(59,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(60,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(88,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(89,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(90,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(118,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(119,0);
  if (chamber=="BIR2C15" ) href->SetBinContent(120,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(1,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(2,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(3,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(4,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(5,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(6,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(31,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(32,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(33,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(34,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(35,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(36,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(61,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(62,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(63,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(64,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(65,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(66,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(91,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(92,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(93,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(94,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(95,0);
  if (chamber=="BIR1C15" ) href->SetBinContent(96,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(1,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(2,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(3,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(31,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(32,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(33,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(61,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(62,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(63,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(91,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(92,0);
  if (chamber=="BIR4C11" ) href->SetBinContent(93,0);
/*
  if (chamber=="BIR3C11" ) href->SetBinContent(34,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(35,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(36,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(70,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(71,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(72,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(106,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(107,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(108,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(142,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(143,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(144,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(178,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(179,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(180,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(214,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(215,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(216,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(250,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(251,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(252,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(286,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(287,0);
  if (chamber=="BIR3C11" ) href->SetBinContent(288,0);
*/
  if (chamber=="BIR4C15" ) href->SetBinContent(1,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(2,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(3,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(31,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(32,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(33,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(61,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(62,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(63,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(91,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(92,0);
  if (chamber=="BIR4C15" ) href->SetBinContent(93,0);
/*
  if (chamber=="BIR3C15" ) href->SetBinContent(34,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(35,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(36,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(70,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(71,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(72,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(106,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(107,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(108,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(142,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(143,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(144,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(178,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(179,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(180,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(214,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(215,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(216,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(250,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(251,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(252,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(286,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(287,0);
  if (chamber=="BIR3C15" ) href->SetBinContent(288,0);
*/
  if (chamber=="BMS4C02" ) href->SetBinContent(41,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(42,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(43,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(44,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(45,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(46,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(47,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(48,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(89,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(90,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(91,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(92,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(93,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(94,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(95,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(96,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(137,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(138,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(139,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(140,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(141,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(142,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(143,0);
  if (chamber=="BMS4C02" ) href->SetBinContent(144,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(41,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(42,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(43,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(44,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(45,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(46,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(47,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(48,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(89,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(90,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(91,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(92,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(93,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(94,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(95,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(96,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(137,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(138,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(139,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(140,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(141,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(142,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(143,0);
  if (chamber=="BMS4C04" ) href->SetBinContent(144,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(41,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(42,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(43,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(44,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(45,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(46,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(47,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(48,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(89,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(90,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(91,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(92,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(93,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(94,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(95,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(96,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(137,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(138,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(139,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(140,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(141,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(142,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(143,0);
  if (chamber=="BMS4C06" ) href->SetBinContent(144,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(41,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(42,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(43,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(44,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(45,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(46,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(47,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(48,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(89,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(90,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(91,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(92,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(93,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(94,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(95,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(96,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(137,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(138,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(139,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(140,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(141,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(142,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(143,0);
  if (chamber=="BMS4C08" ) href->SetBinContent(144,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(41,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(42,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(43,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(44,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(45,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(46,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(47,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(48,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(89,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(90,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(91,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(92,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(93,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(94,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(95,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(96,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(137,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(138,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(139,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(140,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(141,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(142,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(143,0);
  if (chamber=="BMS4C10" ) href->SetBinContent(144,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(41,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(42,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(43,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(44,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(45,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(46,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(47,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(48,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(89,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(90,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(91,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(92,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(93,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(94,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(95,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(96,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(137,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(138,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(139,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(140,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(141,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(142,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(143,0);
  if (chamber=="BMS4C16" ) href->SetBinContent(144,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(22,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(23,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(24,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(46,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(47,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(48,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(70,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(71,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(72,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(94,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(95,0);
  if (chamber=="BIR5C11" ) href->SetBinContent(96,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(22,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(23,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(24,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(46,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(47,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(48,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(70,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(71,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(72,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(94,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(95,0);
  if (chamber=="BIR5C15" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(41,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(42,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(43,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(44,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(45,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(46,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(47,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(48,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(89,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(90,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(91,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(92,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(93,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(94,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(95,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(137,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(138,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(139,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(140,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(141,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(142,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(143,0);
  if (chamber=="BMS6C02" ) href->SetBinContent(144,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(41,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(42,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(43,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(44,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(45,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(46,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(47,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(48,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(89,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(90,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(91,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(92,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(93,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(94,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(95,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(137,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(138,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(139,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(140,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(141,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(142,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(143,0);
  if (chamber=="BMS6C04" ) href->SetBinContent(144,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(41,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(42,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(43,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(44,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(45,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(46,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(47,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(48,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(89,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(90,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(91,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(92,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(93,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(94,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(95,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(137,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(138,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(139,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(140,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(141,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(142,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(143,0);
  if (chamber=="BMS6C06" ) href->SetBinContent(144,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(41,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(42,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(43,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(44,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(45,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(46,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(47,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(48,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(89,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(90,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(91,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(92,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(93,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(94,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(95,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(137,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(138,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(139,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(140,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(141,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(142,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(143,0);
  if (chamber=="BMS6C08" ) href->SetBinContent(144,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(41,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(42,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(43,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(44,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(45,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(46,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(47,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(48,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(89,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(90,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(91,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(92,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(93,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(94,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(95,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(137,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(138,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(139,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(140,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(141,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(142,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(143,0);
  if (chamber=="BMS6C10" ) href->SetBinContent(144,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(41,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(42,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(43,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(44,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(45,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(46,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(47,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(48,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(89,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(90,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(91,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(92,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(93,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(94,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(95,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(96,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(137,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(138,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(139,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(140,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(141,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(142,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(143,0);
  if (chamber=="BMS6C16" ) href->SetBinContent(144,0);
}

void HistogramManager::setChamberDisconnectedTubes(string chamber, TH1F * href ) {

  //  CHAMBERS WITH DISCONNECTED TUBES 

  if (chamber=="BOL2A01" ) href->SetBinContent(81,-21);
  if (chamber=="BOL1A01" ) href->SetBinContent(32,-11);
  if (chamber=="BOL1A01" ) href->SetBinContent(189,-31);
  if (chamber=="BML2A01" ) href->SetBinContent(100,-21);
  if (chamber=="BML2A01" ) href->SetBinContent(236,21);
  if (chamber=="BML2A01" ) href->SetBinContent(295,31);
  if (chamber=="BML2A01" ) href->SetBinContent(304,31);
  if (chamber=="BML1A01" ) href->SetBinContent(250,21);
  if (chamber=="BOL2A05" ) href->SetBinContent(389,31);
  if (chamber=="BIL2A05" ) href->SetBinContent(144,-41);
  if (chamber=="BOL2A07" ) href->SetBinContent(72,-11);
  if (chamber=="BML2A07" ) href->SetBinContent(237,21);
  if (chamber=="BML2A07" ) href->SetBinContent(240,21);
  if (chamber=="BML2A09" ) href->SetBinContent(57,-21);
  if (chamber=="BML1A09" ) href->SetBinContent(14,-11);
  if (chamber=="BOL1A13" ) href->SetBinContent(1,-11);
  if (chamber=="BOL1A13" ) href->SetBinContent(49,-11);
  if (chamber=="BOS1A02" ) href->SetBinContent(323,21);
  if (chamber=="BMS1A02" ) href->SetBinContent(1,-11);
  if (chamber=="BMS1A02" ) href->SetBinContent(56,-11);
  if (chamber=="BMS1A06" ) href->SetBinContent(248,21);
  if (chamber=="BOS1A08" ) href->SetBinContent(99,-31);
  if (chamber=="BMS2A08" ) href->SetBinContent(63,-21);
  if (chamber=="BMS2A10" ) href->SetBinContent(25,-11);
  if (chamber=="BOF1A12" ) href->SetBinContent(146,-31);
  if (chamber=="BOF1A12" ) href->SetBinContent(225,11);
  if (chamber=="BOF1A12" ) href->SetBinContent(249,11);
  if (chamber=="BMF1A12" ) href->SetBinContent(273,11);
  if (chamber=="BOF1A14" ) href->SetBinContent(188,-31);
  if (chamber=="BOF1A14" ) href->SetBinContent(278,11);
  if (chamber=="BMF1A14" ) href->SetBinContent(217,11);
  if (chamber=="BMF1A14" ) href->SetBinContent(218,11);
  if (chamber=="BMF1A14" ) href->SetBinContent(219,11);
  if (chamber=="BIS1A14" ) href->SetBinContent(152,11);
  if (chamber=="BMS2A16" ) href->SetBinContent(49,-21);
  if (chamber=="BMS2A16" ) href->SetBinContent(133,-31);
  if (chamber=="BML4A01" ) href->SetBinContent(5,-11);
  if (chamber=="BML4A03" ) href->SetBinContent(36,-11);
  if (chamber=="BIL4A07" ) href->SetBinContent(185,21);
  if (chamber=="BML4A11" ) href->SetBinContent(90,-31);
  if (chamber=="BIR4A11" ) href->SetBinContent(34,-21);
  if (chamber=="BOL3A13" ) href->SetBinContent(31,-11);
  if (chamber=="BOL3A13" ) href->SetBinContent(41,-11);
  if (chamber=="BML3A13" ) href->SetBinContent(302,31);
  if (chamber=="BML3A15" ) href->SetBinContent(225,21);
  if (chamber=="BMS3A02" ) href->SetBinContent(86,-21);
  if (chamber=="BIS3A02" ) href->SetBinContent(188,31);
  if (chamber=="BIS3A02" ) href->SetBinContent(195,31);
  if (chamber=="BOS4A04" ) href->SetBinContent(310,21);
  if (chamber=="BMS3A04" ) href->SetBinContent(150,11);
  if (chamber=="BMS3A04" ) href->SetBinContent(261,31);
  if (chamber=="BOS4A06" ) href->SetBinContent(204,-31);
  if (chamber=="BMS3A06" ) href->SetBinContent(117,-31);
  if (chamber=="BMS3A06" ) href->SetBinContent(175,11);
  if (chamber=="BOS4A08" ) href->SetBinContent(432,31);
  if (chamber=="BOS3A08" ) href->SetBinContent(117,-21);
  if (chamber=="BMS4A08" ) href->SetBinContent(199,21);
  if (chamber=="BOS4A10" ) href->SetBinContent(2,-11);
  if (chamber=="BOF3A12" ) href->SetBinContent(147,-31);
  if (chamber=="BOF3A12" ) href->SetBinContent(345,31);
  if (chamber=="BOF3A12" ) href->SetBinContent(370,31);
  if (chamber=="BMF2A12" ) href->SetBinContent(184,-31);
  if (chamber=="BIS3A12" ) href->SetBinContent(184,31);
  if (chamber=="BIS3A12" ) href->SetBinContent(185,31);
  if (chamber=="BIS3A12" ) href->SetBinContent(214,41);
  if (chamber=="BIS3A12" ) href->SetBinContent(215,41);
  if (chamber=="BOF5A14" ) href->SetBinContent(1,-11);
  if (chamber=="BMF2A14" ) href->SetBinContent(339,31);
  if (chamber=="BOS4A16" ) href->SetBinContent(20,-11);
  if (chamber=="BOS3A16" ) href->SetBinContent(369,31);
  if (chamber=="BMS4A16" ) href->SetBinContent(127,-31);
  if (chamber=="BMS3A16" ) href->SetBinContent(1,-11);
  if (chamber=="BOL5A01" ) href->SetBinContent(204,-31);
  if (chamber=="BIL6A01" ) href->SetBinContent(37,-21);
  if (chamber=="BIL5A01" ) href->SetBinContent(151,21);
  if (chamber=="BOL6A03" ) href->SetBinContent(19,-11);
  if (chamber=="BML6A07" ) href->SetBinContent(162,11);
  if (chamber=="BML6A09" ) href->SetBinContent(132,-31);
  if (chamber=="BOL6A11" ) href->SetBinContent(147,-31);
  if (chamber=="BOL6A11" ) href->SetBinContent(148,-31);
  if (chamber=="BOL6A11" ) href->SetBinContent(149,-31);
  if (chamber=="BOL6A11" ) href->SetBinContent(150,-31);
  if (chamber=="BML5A11" ) href->SetBinContent(207,31);
  if (chamber=="BOL6A13" ) href->SetBinContent(281,31);
  if (chamber=="BIL5A13" ) href->SetBinContent(222,41);
  if (chamber=="BOS6A02" ) href->SetBinContent(21,-11);
  if (chamber=="BOS5A04" ) href->SetBinContent(21,-11);
  if (chamber=="BOS5A04" ) href->SetBinContent(28,-11);
  if (chamber=="BMS6A06" ) href->SetBinContent(56,-21);
  if (chamber=="BOS5A08" ) href->SetBinContent(50,-11);
  if (chamber=="BOS5A08" ) href->SetBinContent(122,-21);
  if (chamber=="BOS5A08" ) href->SetBinContent(243,11);
  if (chamber=="BOS6A10" ) href->SetBinContent(337,31);
  if (chamber=="BOS6A10" ) href->SetBinContent(362,31);
  if (chamber=="BMS6A10" ) href->SetBinContent(115,-31);
  if (chamber=="BIS5A10" ) href->SetBinContent(181,31);
  if (chamber=="BIS6A14" ) href->SetBinContent(60,-21);
  if (chamber=="BIS6A14" ) href->SetBinContent(68,-31);
  if (chamber=="BIS6A14" ) href->SetBinContent(76,-31);
  if (chamber=="BOS6A16" ) href->SetBinContent(384,31);
  if (chamber=="BOS5A16" ) href->SetBinContent(147,-31);
  if (chamber=="BIL2C01" ) href->SetBinContent(207,21);
  if (chamber=="BML2C03" ) href->SetBinContent(68,-21);
  if (chamber=="BML1C03" ) href->SetBinContent(52,-21);
  if (chamber=="BML2C05" ) href->SetBinContent(42,-11);
  if (chamber=="BML2C05" ) href->SetBinContent(164,-31);
  if (chamber=="BML2C07" ) href->SetBinContent(134,-31);
  if (chamber=="BML2C07" ) href->SetBinContent(221,11);
  if (chamber=="BML2C07" ) href->SetBinContent(234,21);
  if (chamber=="BOL1C09" ) href->SetBinContent(52,-11);
  if (chamber=="BIL1C09" ) href->SetBinContent(219,41);
  if (chamber=="BML2C15" ) href->SetBinContent(138,-31);
  if (chamber=="BML2C15" ) href->SetBinContent(162,-31);
  if (chamber=="BOS2C02" ) href->SetBinContent(145,-31);
  if (chamber=="BOS2C02" ) href->SetBinContent(180,-31);
  if (chamber=="BMS2C02" ) href->SetBinContent(240,21);
  if (chamber=="BIS2C02" ) href->SetBinContent(77,-31);
  if (chamber=="BIS2C04" ) href->SetBinContent(30,-11);
  if (chamber=="BIS2C04" ) href->SetBinContent(240,41);
  if (chamber=="BOS1C06" ) href->SetBinContent(44,-11);
  if (chamber=="BOS2C08" ) href->SetBinContent(54,-11);
  if (chamber=="BOS2C08" ) href->SetBinContent(154,-31);
  if (chamber=="BOS2C08" ) href->SetBinContent(209,-31);
  if (chamber=="BIS2C08" ) href->SetBinContent(1,-11);
  if (chamber=="BIS2C08" ) href->SetBinContent(10,-11);
  if (chamber=="BIS2C08" ) href->SetBinContent(127,11);
  if (chamber=="BIS2C08" ) href->SetBinContent(202,31);
  if (chamber=="BIS1C08" ) href->SetBinContent(190,21);
  if (chamber=="BOS2C10" ) href->SetBinContent(392,31);
  if (chamber=="BMS1C10" ) href->SetBinContent(244,21);
  if (chamber=="BIS2C10" ) href->SetBinContent(94,-41);
  if (chamber=="BIS2C10" ) href->SetBinContent(96,-41);
  if (chamber=="BOG2C12" ) href->SetBinContent(148,11);
  if (chamber=="BOG2C12" ) href->SetBinContent(197,21);
  if (chamber=="BIS1C12" ) href->SetBinContent(61,-21);
  if (chamber=="BIS1C12" ) href->SetBinContent(108,-31);
  if (chamber=="BIS1C12" ) href->SetBinContent(136,-41);
  if (chamber=="BMF1C14" ) href->SetBinContent(10,-11);
  if (chamber=="BMF1C14" ) href->SetBinContent(55,-11);
  if (chamber=="BMF1C14" ) href->SetBinContent(142,-21);
  if (chamber=="BMF1C14" ) href->SetBinContent(231,11);
  if (chamber=="BOS2C16" ) href->SetBinContent(360,21);
  if (chamber=="BOS1C16" ) href->SetBinContent(72,-11);
  if (chamber=="BMS1C16" ) href->SetBinContent(264,21);
  if (chamber=="BOL4C01" ) href->SetBinContent(241,11);
  if (chamber=="BOL4C01" ) href->SetBinContent(242,11);
  if (chamber=="BOL4C01" ) href->SetBinContent(380,31);
  if (chamber=="BOL4C01" ) href->SetBinContent(381,31);
  if (chamber=="BML3C01" ) href->SetBinContent(322,31);
  if (chamber=="BML4C05" ) href->SetBinContent(107,-31);
  if (chamber=="BML4C09" ) href->SetBinContent(99,-31);
  if (chamber=="BIL3C09" ) href->SetBinContent(151,21);
  if (chamber=="BIR4C11" ) href->SetBinContent(79,-31);
  if (chamber=="BIR4C11" ) href->SetBinContent(80,-31);
  if (chamber=="BIR4C11" ) href->SetBinContent(81,-31);
  if (chamber=="BOL3C15" ) href->SetBinContent(235,21);
  if (chamber=="BML4C15" ) href->SetBinContent(107,-31);
  if (chamber=="BML3C15" ) href->SetBinContent(62,-21);
  if (chamber=="BOS3C02" ) href->SetBinContent(217,11);
  if (chamber=="BMS4C02" ) href->SetBinContent(28,-11);
  if (chamber=="BMS4C04" ) href->SetBinContent(184,11);
  if (chamber=="BOS3C06" ) href->SetBinContent(305,21);
  if (chamber=="BIS4C06" ) href->SetBinContent(3,-11);
  if (chamber=="BIS3C06" ) href->SetBinContent(140,11);
  if (chamber=="BOS3C08" ) href->SetBinContent(227,11);
  if (chamber=="BMS3C08" ) href->SetBinContent(249,31);
  if (chamber=="BIS4C08" ) href->SetBinContent(4,-11);
  if (chamber=="BIS4C08" ) href->SetBinContent(214,41);
  if (chamber=="BMS3C10" ) href->SetBinContent(3,-11);
  if (chamber=="BIS3C10" ) href->SetBinContent(90,-31);
  if (chamber=="BMF2C12" ) href->SetBinContent(208,11);
  if (chamber=="BMF2C12" ) href->SetBinContent(217,11);
  if (chamber=="BMF2C12" ) href->SetBinContent(223,11);
  if (chamber=="BMF2C12" ) href->SetBinContent(241,11);
  if (chamber=="BOF5C14" ) href->SetBinContent(163,11);
  if (chamber=="BOF5C14" ) href->SetBinContent(189,11);
  if (chamber=="BMF2C14" ) href->SetBinContent(30,-11);
  if (chamber=="BMF2C14" ) href->SetBinContent(60,-11);
  if (chamber=="BMF2C14" ) href->SetBinContent(92,-21);
  if (chamber=="BMF2C14" ) href->SetBinContent(97,-21);
  if (chamber=="BMF2C14" ) href->SetBinContent(103,-21);
  if (chamber=="BMF2C14" ) href->SetBinContent(245,11);
  if (chamber=="BOS4C16" ) href->SetBinContent(145,-31);
  if (chamber=="BOS3C16" ) href->SetBinContent(55,-11);
  if (chamber=="BIS3C16" ) href->SetBinContent(90,-31);
  if (chamber=="BIL5C03" ) href->SetBinContent(223,41);
  if (chamber=="BML6C07" ) href->SetBinContent(182,11);
  if (chamber=="BIL5C09" ) href->SetBinContent(25,-11);
  if (chamber=="BIL5C09" ) href->SetBinContent(73,-31);
  if (chamber=="BIL5C09" ) href->SetBinContent(88,-31);
  //if (chamber=="BML5C13" ) href->SetBinContent(200,21);  CONTROLLARE!!!
  if (chamber=="BML6C13" ) href->SetBinContent(200,21);
  if (chamber=="BMS6C02" ) href->SetBinContent(102,-31);
  if (chamber=="BMS5C02" ) href->SetBinContent(22,-11);
  if (chamber=="BMS5C04" ) href->SetBinContent(44,-21);
  if (chamber=="BOS5C10" ) href->SetBinContent(417,31);
  if (chamber=="BOF7C12" ) href->SetBinContent(104,-31);
  if (chamber=="BMF3C12" ) href->SetBinContent(38,-11);
  //if (chamber=="EIL4A01" ) href->SetBinContent(191,11); CONTROLLARE!!!
  if (chamber=="EIL5A01" ) href->SetBinContent(191,11);
  if (chamber=="EML4A03" ) href->SetBinContent(199,11);
  if (chamber=="EML4A03" ) href->SetBinContent(321,31);
  if (chamber=="EML4A05" ) href->SetBinContent(8,-11);
  if (chamber=="EML4A07" ) href->SetBinContent(326,31);
  if (chamber=="EML4A07" ) href->SetBinContent(327,31);
  if (chamber=="EML4A09" ) href->SetBinContent(100,-21);
  if (chamber=="EML4A09" ) href->SetBinContent(102,-21);
  if (chamber=="EML4A09" ) href->SetBinContent(103,-21);
  if (chamber=="EML4A09" ) href->SetBinContent(255,11);
  if (chamber=="EML4A09" ) href->SetBinContent(256,11);
  if (chamber=="EML4A09" ) href->SetBinContent(257,21);
  if (chamber=="EML4A09" ) href->SetBinContent(320,21);
  if (chamber=="EML4A11" ) href->SetBinContent(30,-11);
  if (chamber=="EML4A11" ) href->SetBinContent(69,-21);
  if (chamber=="EML4A11" ) href->SetBinContent(91,-21);
  if (chamber=="EML4A11" ) href->SetBinContent(140,-31);
  if (chamber=="EML4A11" ) href->SetBinContent(198,11);
  if (chamber=="EML4A11" ) href->SetBinContent(259,21);
  if (chamber=="EML4A11" ) href->SetBinContent(260,21);
  if (chamber=="EML4A11" ) href->SetBinContent(261,21);
  if (chamber=="EML4A13" ) href->SetBinContent(162,-31);
  if (chamber=="EML4A13" ) href->SetBinContent(228,11);
  if (chamber=="EML4A13" ) href->SetBinContent(271,21);
  if (chamber=="EML4A13" ) href->SetBinContent(359,31);
  if (chamber=="EIL4A13" ) href->SetBinContent(247,11);
  if (chamber=="EIL4A13" ) href->SetBinContent(393,41);
  if (chamber=="EMS5A02" ) href->SetBinContent(336,31);
  if (chamber=="EMS5A06" ) href->SetBinContent(109,-21);
  if (chamber=="EMS5A06" ) href->SetBinContent(191,-31);
  if (chamber=="EMS5A08" ) href->SetBinContent(112,-21);
  if (chamber=="BIS7A08" ) href->SetBinContent(157,21);
  if (chamber=="EMS5A12" ) href->SetBinContent(236,11);
  if (chamber=="EMS4A12" ) href->SetBinContent(119,-21);
  if (chamber=="EOL6A03" ) href->SetBinContent(4,-11);
  if (chamber=="EOL5A05" ) href->SetBinContent(42,-11);
  if (chamber=="EOL5A05" ) href->SetBinContent(67,-21);
  if (chamber=="EOL6A11" ) href->SetBinContent(38,-11);
  if (chamber=="EOS5A02" ) href->SetBinContent(11,-11);
  if (chamber=="EMS3A02" ) href->SetBinContent(187,-31);
  if (chamber=="EOS4A12" ) href->SetBinContent(191,11);
  if (chamber=="EOS4A12" ) href->SetBinContent(233,21);
  if (chamber=="EMS3A12" ) href->SetBinContent(1,-11);
  if (chamber=="EIS1A14" ) href->SetBinContent(51,-21);
  if (chamber=="EOL3A01" ) href->SetBinContent(74,-21);
  if (chamber=="EOL3A01" ) href->SetBinContent(159,11);
  if (chamber=="EOL3A01" ) href->SetBinContent(216,21);
  if (chamber=="EML2A13" ) href->SetBinContent(76,-21);
  if (chamber=="EOL3A15" ) href->SetBinContent(167,11);
  if (chamber=="EOL3A15" ) href->SetBinContent(168,11);
  if (chamber=="EML2A15" ) href->SetBinContent(330,31);
  if (chamber=="EMS2A08" ) href->SetBinContent(219,11);
  if (chamber=="EIL1A01" ) href->SetBinContent(267,41);
  //if (chamber=="EIL4C01" ) href->SetBinContent(169,11); CONTROLLARE!!!
  if (chamber=="EIL5C01" ) href->SetBinContent(169,11);
  if (chamber=="EML4C03" ) href->SetBinContent(296,21);
  if (chamber=="EML4C03" ) href->SetBinContent(335,31);
  if (chamber=="EML4C05" ) href->SetBinContent(158,-31);
  if (chamber=="EIL4C13" ) href->SetBinContent(432,41);
  if (chamber=="EOL4C01" ) href->SetBinContent(177,11);
  if (chamber=="EOL4C01" ) href->SetBinContent(178,11);
  if (chamber=="EML3C05" ) href->SetBinContent(235,11);
  if (chamber=="EOL5C07" ) href->SetBinContent(152,11);
  if (chamber=="EML3C07" ) href->SetBinContent(291,21);
  if (chamber=="EML3C09" ) href->SetBinContent(60,-11);
  if (chamber=="EOS6C04" ) href->SetBinContent(203,21);
  if (chamber=="EMS3C04" ) href->SetBinContent(252,11);
  if (chamber=="EIS1C04" ) href->SetBinContent(322,41);
  if (chamber=="EIS1C04" ) href->SetBinContent(323,41);
  if (chamber=="EMS3C12" ) href->SetBinContent(142,-31);
  if (chamber=="EOS5C14" ) href->SetBinContent(176,11);
  if (chamber=="EMS3C16" ) href->SetBinContent(54,-11);
  if (chamber=="EMS3C16" ) href->SetBinContent(357,31);
  if (chamber=="EML1C03" ) href->SetBinContent(113,-31);
  if (chamber=="EML2C07" ) href->SetBinContent(185,-31);
  if (chamber=="EML2C07" ) href->SetBinContent(190,-31);
  if (chamber=="EML2C07" ) href->SetBinContent(191,-31);
  if (chamber=="EML2C07" ) href->SetBinContent(192,-31);
  if (chamber=="EML1C09" ) href->SetBinContent(319,31);
  if (chamber=="EML1C11" ) href->SetBinContent(36,-11);
  if (chamber=="EML2C15" ) href->SetBinContent(161,-31);
  if (chamber=="EMS2C14" ) href->SetBinContent(64,-11);
  if (chamber=="EMS2C14" ) href->SetBinContent(90,-21);

}


int HistogramManager::GetTubeOffsetML1(string chamber){
  int tubeOffset=0;
  if (chamber=="BIR1A11" ) tubeOffset=6;
  if (chamber=="BIR1A15" ) tubeOffset=6;
  if (chamber=="BIR4A11" ) tubeOffset=3;
  if (chamber=="BIR4A15" ) tubeOffset=3;
  if (chamber=="BIR1C11" ) tubeOffset=6;
  if (chamber=="BIR1C15" ) tubeOffset=6;
  if (chamber=="BIR4C11" ) tubeOffset=3;
  if (chamber=="BIR4C15" ) tubeOffset=3;
  return tubeOffset;
}

int HistogramManager::GetTubeOffsetAtEndML1(string chamber){
  int tubeOffset=0;
  if (chamber.substr(0,4)=="BMS4") tubeOffset=8;
  if (chamber.substr(0,4)=="BMS6") tubeOffset=8;
  if (chamber=="BIR2A11" ) tubeOffset=3;
  if (chamber=="BIR2A15" ) tubeOffset=3;
  if (chamber=="BIR5A11" ) tubeOffset=3;
  if (chamber=="BIR5A15" ) tubeOffset=3;
  if (chamber=="BIR2C11" ) tubeOffset=3;
  if (chamber=="BIR2C15" ) tubeOffset=3;
  if (chamber=="BIR5C11" ) tubeOffset=3;
  if (chamber=="BIR5C15" ) tubeOffset=3;
  return tubeOffset;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TObject * HistogramManager::GetMdtHisto(string histo_name) {
  TObject * h = NULL;
  string dir_name=GetMdtDirectoryName();
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if(dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}
TObject * HistogramManager::GetMdtHisto(string histo_name, string region, string side) {
  TObject * h = NULL;
  string dir_name=GetMdtDirectoryName(region, side);
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if(dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}
TObject * HistogramManager::GetMdtHisto(string histo_name,string region, string side, int sector) {
  TObject * h = NULL;

  string dir_name=GetMdtDirectoryName(region,side,sector);
  dir_name+="/OVERVIEW";
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if(dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}

TObject * HistogramManager::GetMdtHisto(string histo_name, MDTName chamb) {


  TObject * h = NULL;
  string dir_name=GetMdtDirectoryName(chamb);

  if(histo_name.substr(0,14)=="HitsPerTube_ML"){
    dir_name += "/Occupancy";
  }
  if(histo_name.substr(0,18)=="ActivityPerTube_ML"){
    dir_name += "/DeadStatus";
  }
  if(histo_name.substr(0,15)=="NoisePerTube_ML"){
    dir_name += "/NoiseStatus";
  }
  if(histo_name.substr(0,14)=="EffiPerTube_ML"){
    dir_name=dir_name+"/Efficiency";
  }
  if(histo_name=="EffiResidVsRadius"){
    dir_name=dir_name+"/Expert";
    //dir_name=dir_name;
  }
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if(dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}

string HistogramManager::GetMdtDirectoryName() {
  string Mdt_dirName="/MDT";
  return Mdt_dirName;
}

string HistogramManager::GetMdtDirectoryName(string region, string side) {
  string Mdt_dirName="/MDT/"+region+"_"+side;
  return Mdt_dirName;
}

string HistogramManager::GetMdtDirectoryName(string region, string side, int sector) {
  ToString ts;
  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string Mdt_dirName="/MDT/"+region+"_"+side+"/Sector"+sectorString;
  return Mdt_dirName;
}

string HistogramManager::GetMdtDirectoryName(string region, string side, int sector, string chamberType, int eta) {
  ToString ts;
  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string chamberName = chamberType+ts(eta)+side+sectorString;
  string Mdt_dirName="/MDT/"+region+"_"+side+"/Sector"+sectorString+"/"+chamberName;
  return Mdt_dirName;
}

string HistogramManager::GetMdtDirectoryName(MDTName chamb) {

  int sector=chamb.getOnlineSector();
  ToString ts;
  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string Mdt_dirName="/MDT/"+chamb.getRegion()+"_"+chamb.getSide()+"/Sector"+sectorString+"/"+chamb.getOnlineName();
  //std::cout<<Mdt_dirName<<endl;
  return Mdt_dirName;

}

TObject * HistogramManager::GetTDaqHisto(string histo_name, string region) {
  TObject * h = NULL;
  string dir_name=GetTDaqDirectoryName(region);
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if (dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}
TObject * HistogramManager::GetTDaqHisto(string histo_name, string region, string side) {
  TObject * h = NULL;
  string dir_name=GetTDaqDirectoryName(region, side);
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if (dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}
TObject * HistogramManager::GetTDaqHisto(string histo_name, string region, string side, int sector) {
  TObject * h = NULL;
  string dir_name=GetTDaqDirectoryName(region, side,sector);
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if (dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}
TObject * HistogramManager::GetTDaqHisto(string histo_name, string region, string side, int sector, string chamberType, int eta) {
  TObject * h = NULL;
  string dir_name=GetTDaqDirectoryName(region,side,sector,chamberType,eta);
  TDirectory* dir = m_rootfile->GetDirectory(dir_name.c_str());
  if (dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}

string HistogramManager::GetTDaqDirectoryName(string region) {
  string TDaq_dirName="Undefined";
  if (region=="Barrel") TDaq_dirName="/MDTvsRPC";
  if (region=="Endcap") TDaq_dirName="/MDTvsTGC";
  return TDaq_dirName;
}

string HistogramManager::GetTDaqDirectoryName(string region, string side) {
  string TDaq_dirName="undefined";
  if (region=="Barrel") TDaq_dirName="/MDTvsRPC/"+region+"_"+side;
  if (region=="Endcap") TDaq_dirName="/MDTvsTGC/"+region+"_"+side;
  return TDaq_dirName;
}

string HistogramManager::GetTDaqDirectoryName(string region, string side, int sector) {
  ToString ts;
  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string TDaq_dirName="undefined";
  if (region=="Barrel") TDaq_dirName="/MDTvsRPC/"+region+"_"+side+"/Sector"+sectorString;
  if (region=="Endcap") TDaq_dirName="/MDTvsTGC/"+region+"_"+side+"/Sector"+sectorString;
  return TDaq_dirName;
}

string HistogramManager::GetTDaqDirectoryName(string region, string side, int sector, string chamberType, int eta) {
  ToString ts;
  string sectorString=ts(sector);
  if(sector<10) sectorString="0"+sectorString;
  string chamberName = chamberType+ts(eta)+side+sectorString;
  string TDaq_dirName="undefined";
  if (region=="Barrel") TDaq_dirName="/MDTvsRPC/"+region+"_"+side+"/Sector"+sectorString+"/"+chamberName;
  if (region=="Endcap") TDaq_dirName="/MDTvsTGC/"+region+"_"+side+"/Sector"+sectorString+"/"+chamberName;
  return TDaq_dirName;
}

TObject * HistogramManager::GetHisto(string main_dir, string histo_name) {
  TObject * h = NULL;
  TDirectory* dir = m_rootfile->GetDirectory(main_dir.c_str());
  if(dir) h = dir->FindObjectAny(histo_name.c_str());
  return h;
}
////////////////////////////////////////////////////////////////////////////////

bool HistogramManager::openOutputFile(string outFileName) {
  //cout<<"Opening Output File: "<< outFileName << endl;
  m_rootfile = new TFile(outFileName.c_str(),"recreate");
  if (!m_rootfile) {
    //cout<<"HistogramManager::openOutputFile : FAILING OPENING NEW FILE "<< outFileName << endl;
    return false;
  } else {
    return true;
  }
}

bool HistogramManager::openReadOnlyFile(string outFileName) {
  //cout<<"Opening Output File: "<< outFileName << endl;
  m_rootfile = new TFile(outFileName.c_str(),"readonly");
  if (!m_rootfile) {
    //cout<<"HistogramManager::openReadOnlyFile : FAILING OPENING FILE "<< outFileName << endl;
    return false;
  } else {
    return true;
  }
}

bool HistogramManager::openUpdateFile(string outFileName) {
  //cout<<"Opening Output File: "<< outFileName << endl;
  m_rootfile = new TFile(outFileName.c_str(),"update");
  if (!m_rootfile) {
    //cout<<"HistogramManager::openUpdateFile : FAILING OPENING FILE "<< outFileName << endl;
    return false;
  } else {
    return true;
  }
}

void HistogramManager::WriteAndCloseFile() {
  // m_rootfile->Write();
  m_rootfile->Write("",TObject::kOverwrite);
  m_rootfile->Close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int HistogramManager::GetEtaMax(string /* region*/, string /*side*/, int /*sector*/, string /*chamberType*/) {
  int etaMax = 6;
  return etaMax;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::vector<MDTName> HistogramManager::GetChamberList(string region, string side, int sector) {

  //ToString ts;
  std::vector<MDTName> chamberList;

    if ( m_MdtIdHelper ) {
      MdtIdHelper::const_id_iterator it     = m_MdtIdHelper->module_begin();
      MdtIdHelper::const_id_iterator it_end = m_MdtIdHelper->module_end();
      for(; it!=it_end;++it ) {

	if  ( !m_MdtIdHelper->is_mdt(*it) ) continue;
	int station_index = m_MdtIdHelper->stationName(*it);
	string stationName = m_MdtIdHelper->stationNameString(station_index);
	int phi_id = m_MdtIdHelper->stationPhi(*it);
	int eta_id = m_MdtIdHelper->stationEta(*it);
                
	MDTName chamber(stationName,phi_id,eta_id);

        if(chamber.getRegion()!=region) continue;
        if(chamber.getOnlineSector()!=sector) continue;
        if(chamber.getSide()!=side) continue;

        //Problem with BML in sector13, it should be automatically treated by MDTName, but it gives some problem
        //only here, I don't know why. This line should resolve it, check if it shows in other part of the code
        //It seems like the problem is in MdtIdHelper however.

        if((chamber.getName()=="BML")&&(chamber.getOnlineEta()==7)) continue;

	chamberList.push_back(chamber);

      } // loop over MdtID_iterator
    } // if m_MdtHelper

  return chamberList;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int HistogramManager::GetChamberNumOfML (string chamberName){
  int numOfML=0;
  const int numParams=10;
  int chamberGeo[numParams];
  for (int i=0;i<=9;i++) chamberGeo[i]=0;
  ReadChamberMapFile(chamberName,chamberGeo,numParams);
  numOfML=chamberGeo[0];
  return numOfML;
}
int HistogramManager::GetChamberNumOfMezzPerML (string chamberName){
  int numOfMezzPerML=0;
  const int numParams=10;
  int chamberGeo[numParams];
  for (int i=0;i<=9;i++) chamberGeo[i]=0;
  ReadChamberMapFile(chamberName,chamberGeo,numParams);
  numOfMezzPerML=chamberGeo[1];
  return numOfMezzPerML;
}
int HistogramManager::GetChamberTubesPerMezz (string chamberName){
  int numOfTubesPerMezz=0;
  const int numParams=10;
  int chamberGeo[numParams];
  for (int i=0;i<=9;i++) chamberGeo[i]=0;
  ReadChamberMapFile(chamberName,chamberGeo,numParams);
  numOfTubesPerMezz=chamberGeo[2];
  return numOfTubesPerMezz;
}

void HistogramManager::ReadChamberMapFile(string chamberName, int * chamberGeoParams, int numParams) {

  for (int i=0; i<numParams; i++) chamberGeoParams[i]=0;
  string s;
  ifstream inf("ChambersLayout.txt");
 
  if(!inf.good()){
    //cout << "Text mapfile ChambersLayout.txt is missing! Check configuration."<<endl;
    return; 
  }

  // scanning of map file
  while(getline(inf,s)){

    // bool test=false;    
    string name, tag;
    istringstream is(s);
    is>>tag;

    if(tag=="name"){ //scan chamber block
      
      if(!(is>>name).good()){//1. take the name
	//cout<< " chamber name missing on mapfile "<<s.c_str() <<endl;
	break;
      }

      /*
	Check if any chamber on the mapfile is a chamber to test;
	it's the best way to scan the mapfile just one time.
      */

      if(name!=chamberName){ 
	//if what I find in file is NOT a chamber to be tested
	continue;
      } else { //3. read full information
	// test=true;
	unsigned int ReadInfo=0;

	for(int i=0;i<20;i++){ 
	  //chamber block is supposed to be shorter than 20 lines
	    
	  if(!getline(inf,s).good()){
	    continue;
	  }
	  if(s.c_str()[0]=='#') {
	    continue;
	  }
	  if(s=="END_CHAMBER") {
	    break; //out of "for" over 20 lines
	  }

	  istringstream is(s);
	  is>>tag;

	  if (tag=="MLayers"){
	    if (!(is>>chamberGeoParams[0]).good()) { 
	      //cout << "WRONG MAP FILE FORMAT "<<endl;
	      break;
	    }
	    // cout << " Number of Multilayers = " << chamberGeoParams[0] << endl;
	    ReadInfo++;
	  }
	    
	  if(tag=="MezzPerML"){
	    if (!(is>>chamberGeoParams[1]).good()) { 
	      //std::cout << "WRONG MAP FILE FORMAT "<<std::endl;
	      break;
	    }
	    // cout << " Mezzanines Per ML = " << chamberGeoParams[1] << endl;
	    ReadInfo++;
	  }
	    
	  if(tag=="MezzTypeML1"){
	    int mezzType=0;
	    int tubesPerMezz=0;
	    is>>mezzType;
	    if (mezzType<1 || mezzType>4) { 
	      //cout << "WRONG MAP FILE FORMAT "<<endl;
	      break;
	    }
	    if (mezzType==1 || mezzType==2) tubesPerMezz=8;
	    if (mezzType==3 || mezzType==4) tubesPerMezz=6;
	    chamberGeoParams[2]=tubesPerMezz;
	    // cout << " Tubes Per Layer Per Mezzanine = " << chamberGeoParams[2] << endl;
	    ReadInfo++;
	  }
	    
	  if(tag=="MezzTypeML2"){
	    ReadInfo++;
	  }
	    
	  if(tag=="IncreaseWithZ"){
	    ReadInfo++;
	  }

	  if(tag=="ZeroMezzML"){
	    ReadInfo++;
	    break; //out of "for" over 20 lines
	  }
	    
	}// end for over 20 lines 

	if(ReadInfo!=6) {
	  // ERS_WARNING ("Incomplete map for "<<ChambList->at(it).name);
	  // ChambList->erase(ChambList->begin()+it);
	  // it--;
	}
	break; //out of "for" loop on ChambList       
      }// else if(name...) 
	
    }// end if tag=name
    
  } // end while

  return;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// TEST METHODS :
// 
/*
  void HistogramManager::test() {
  cout << " You are in the test method. Opening output file with default name"<< endl;
  string defaultOutputFile="outDQA.root";
  openOutputFile(defaultOutputFile);

  cout << " Now building All histograms buildAll()" <<endl;
  buildAll(3,5);

  TH1F * h;
  cout << " Now trying to retrieve histogram HitsPerTube from BML1A03 : " <<endl;
  h = (TH1F*) GetMdtHisto("HitsPerTube","A",3,"BML",1);
  cout << " Filling it with 1 entry " << endl;
  h->Fill(14.);
  cout <<" Check number of Entries :" << h->GetEntries() << endl;
    
  cout << " Now trying to retrieve histogram MDT_RPC for Sector 1 side C in MDTvsRPC directory : " <<endl;
  h = (TH1F*) GetTDaqHisto("MDT_RPC","C",2);
  cout << " Filling it with 1 entry " << endl;
  h->Fill(3.5);
  cout <<" Check number of Entries :" << h->GetEntries() << endl;

  cout <<" Writing and Closing File"<< endl;
  m_rootfile->Write();
  m_rootfile->Close();
  }
*/
