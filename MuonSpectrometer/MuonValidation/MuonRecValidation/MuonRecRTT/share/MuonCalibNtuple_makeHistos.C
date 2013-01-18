// Macro for to print plots from the Calibration ntuple
//
// Useage: to run in batch mode, type
//         root -q -b 'MuonCalibNtuple_makeHistos.C("CalibrationNtuple.root")'
//
// Sep 2010     S. Willocq
//////////////////////////////////////////////////////////////////////////////

void MuonCalibNtuple_makeHistos(char* inputName)
{
//Set Some default styles for the histogram plots
  gStyle->SetMarkerStyle(8);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.2);
  gStyle->SetStatFontSize(0.07);
  gStyle->SetLabelSize(0.06);
  gStyle->SetOptFit(1111);  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetMarkerSize(0.3);  

  printf("Input name = %s\n", inputName);
  char inputNtupleFile[256];
  char outputHistosFile[256];
  sprintf(inputNtupleFile,"%s.root",inputName);
  sprintf(outputHistosFile,"%s_histos.root",inputName);
  printf("Input ntuple file = %s\n", inputNtupleFile);
  printf("output histos file = %s\n", outputHistosFile);

// open ntuple file
  //TFile *fin = new TFile("CalibrationNtuple.root","READ");
  TFile *fin = new TFile(inputNtupleFile,"READ");
  TTree *tree = (TTree*)fin->Get("PatternNtupleMaker/Segments");
  //TFile *fout = new TFile("MuonCalibNtuple_fromESD_histos.root","RECREATE");
  TFile *fout = new TFile(outputHistosFile,"RECREATE");

// create histograms
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(111111);

  // Csc block
  TH1F *csc_num = new TH1F("csc_num","No. CSC clusters / evt",40,0,40);
  TH1F *csc_strips = new TH1F("csc_strips","No. CSC strips/cluster",40,0,40);
  TH1F *csc_charge = new TH1F("csc_charge","CSC cluster charge",80,0.,4000000.);
  // ctp block
  TH1F *ctp_ntriginfo = new TH1F("ctp_ntriginfo","No. trigger info items",100,0,200);
  TH1F *ctp_bcid = new TH1F("ctp_bcid","CTP BCID",100,0,4000);
  TH1F *ctp_type = new TH1F("ctp_type","CTP type",12,0,12);
  // event block
  TH1F *evt_bcid = new TH1F("evt_bcid","event BCID",100,0,4000);
  TH1F *evt_lb = new TH1F("evt_lb","event lumi block",100,0,1000);
  // mdt block
  TH1F *mdt_num = new TH1F("mdt_num","No. of MDT hits",100,0,500);
  TH1F *mdt_adc = new TH1F("mdt_adc","MDT hit ADC",100,0,400);
  TH1F *mdt_radius = new TH1F("mdt_radius","MDT hit radius",60,-15,15);
  TH1F *mdt_time = new TH1F("mdt_time","MDT drift time",100,-50,950);
  // muctpi block
  TH1F *muctpi_sectorid = new TH1F("muctpi_sectorid","MUCTPI sector ID",50,0,50);
  TH1F *muctpi_eta = new TH1F("muctpi_eta","MUCTPI eta",60,-3.,3.);
  TH1F *muctpi_phi = new TH1F("muctpi_phi","MUCTPI phi",60,-3.1416,3.1416);
  // pat block
  TH1F *pat_num = new TH1F("pat_num","No. of patterns",100,0,100);
  //phi block
  TH1F *phi_num = new TH1F("phi_num","No. of phi hits",100,0,1000);
  TH1F *phi_posZ = new TH1F("phi_posZ","phi hit Z coordinate",92,-23000,23000);
  // raw csc block
  TH1F *rawCsc_num = new TH1F("rawCsc_num","No. raw CSC clusters / evt",80,0,800);
  TH1F *rawCsc_time = new TH1F("rawCsc_time","Raw CSC time",100,-200,300);
  TH1F *rawCsc_charge = new TH1F("rawCsc_charge","Raw CSC charge",55,-100000,1000000);
  // raw mdt block
  TH1F *rawMdt_num = new TH1F("rawMdt_num","No. of raw MDT hits",100,0,1000);
  TH1F *rawMdt_adc = new TH1F("rawMdt_adc","Raw MDT hit ADC",100,0,400);
  TH1F *rawMdt_radius = new TH1F("rawMdt_radius","MDT hit radius",50,0,25);
  TH1F *rawMdt_time = new TH1F("rawMdt_tdc","Raw MDT TDC",70,0,3500);
  // raw RPC coincidences block
  TH1F *rawRpcCoin_num = new TH1F("rawRpcCoin_num","No. of raw RPC coincidences",60,0,60);
  TH1F *rawRpcCoin_parsecid = new TH1F("rawRpcCoin_parsecid","Raw RPC coincidence parent sector ID",70,0,70);
  TH1F *rawRpcCoin_time = new TH1F("rawRpcCoin_time","Raw RPC CM time",50,0,200);
  // raw rpc block
  TH1F *rawRpc_num = new TH1F("rawRpc_num","No. raw RPC hits / evt",80,0,400);
  TH1F *rawRpc_time = new TH1F("rawRpc_time","Raw RPC time",50,0,200);
  // raw TGC coincidences block
  TH1F *rawTgcCoin_num = new TH1F("rawTgcCoin_num","No. of raw TGC coincidences",50,0,50);
  TH1F *rawTgcCoin_eta_sl = new TH1F("rawTgcCoin_eta_sl","Raw TGC coincidence eta sector logic",12,-6,6);
  TH1F *rawTgcCoin_eta_sl_Next = new TH1F("rawTgcCoin_eta_sl_Next","Raw TGC coincidence eta sector logic Next BC",12,-6,6);
  TH1F *rawTgcCoin_eta_sl_Prev = new TH1F("rawTgcCoin_eta_sl_Prev","Raw TGC coincidence eta sector logic Prev BC",12,-6,6);
  TH1F *rawTgcCoin_phi_sl = new TH1F("rawTgcCoin_phi_sl","Raw TGC coincidence phi sector logic",50,0,50);
  TH1F *rawTgcCoin_phi_sl_Next = new TH1F("rawTgcCoin_phi_sl_Next","Raw TGC coincidence phi sector logic Next BC",50,0,50);
  TH1F *rawTgcCoin_phi_sl_Prev = new TH1F("rawTgcCoin_phi_sl_Prev","Raw TGC coincidence phi sector logic Prev BC",50,0,50);
  // raw TGC block
  TH1F *rawTgc_num = new TH1F("rawTgc_num","No. of raw TGC hits",60,0,300);
  TH1F *rawTgc_bctag = new TH1F("rawTgc_bctag","Raw TGC bunch crossing tag",5,0,5);
  TH1F *rawTgc_station = new TH1F("rawTgc_station","Raw TGC hit station",10,40,50);
  TH1F *rawTgc_station_Next = new TH1F("rawTgc_station_Next","Raw TGC hit station Next BC",10,40,50);
  TH1F *rawTgc_station_Prev = new TH1F("rawTgc_station_Prev","Raw TGC hit station Prev BC",10,40,50);
  // rpcOs block
  TH1F *rpcOs_num = new TH1F("rpcOs_num","No. RPC Os hits / evt",80,0,400);
  TH1F *rpcOs_time = new TH1F("rpcOs_time","RPC Os time",50,0,200);
  TH1F *rpcOs_distro = new TH1F("rpcOs_distro","RPC Os dist to readout",50,-100,1400);
  // rpc SL block
  TH1F *rpc_SLnum = new TH1F("rpc_SLnum","No. RPC Os hits / evt",20,0,20);
  TH1F *rpc_SLhit_sector = new TH1F("rpc_SLhit_sector","RPC SL hit sector",70,0,70);
  TH1F *rpc_SLhit_roi = new TH1F("rpc_SLhit_roi","RPC SL hit ROI",30,0,30);
  // seg block
  TH1F *seg_num = new TH1F("seg_num","No. Segments / evt", 50, 0, 100);
  TH1F *seg_hits = new TH1F("seg_hits","Hits per Segment", 20,0,20);
  TH1F *seg_theta = new TH1F("seg_theta","Segments theta", 70,0,3.5);
  TH1F *seg_theta_mboy = new TH1F("seg_theta_mboy","Muonboy segments theta", 70,0,3.5);
  TH1F *seg_theta_moore = new TH1F("seg_theta_moore","Moore segments theta", 70,0,3.5);
  TH1F *seg_phi = new TH1F("seg_phi","Segments phi", 70,0,3.5);
  // tgc block
  TH1F *tpc_num = new TH1F("tgc_num","No. TGC hits / evt",80,0,400);
  TH1F *tgc_error = new TH1F("tgc_error","TGC hit error",50,0,100);
  // trkHit block
  TH1F *trkHit_num = new TH1F("trkHit_num","No. track hits / trk",50,0,500);
  TH1F *trkHit_resi = new TH1F("trkHit_resi","Track hit residual",100,-0.5,0.5);
  // trk block
  TH1F *trk_num = new TH1F("trk_num","Tracks / evt", 10, 0, 10);
  TH1F *trk_qoverp = new TH1F("trk_qoverp","Track q/p (1/MeV) ", 60, -150000., 150000.);
  TH1F *trk_pt = new TH1F("trk_pt","Track pt (MeV)", 50, 0., 100000.);
  TH1F *trk_pt_mboy = new TH1F("trk_pt_mboy","Muonboy track pt (MeV)", 50, 0., 100000.);
  TH1F *trk_pt_moore = new TH1F("trk_pt_moore","Moore track pt (MeV)", 50, 0., 100000.);
  TH1F *trk_eta = new TH1F("trk_eta","Track eta", 60, -3., 3.);
  TH1F *trk_phi = new TH1F("trk_phi","Track phi", 60, -3.1416, 3.1416);




// fill histograms

  //tree.Draw(">>");

  tree->Draw("csc_nCsc>>csc_num");
  tree->Draw("csc_nStrips>>csc_strips");
  tree->Draw("csc_charge>>csc_charge");
  tree->Draw("ctp_nTriggerInfo>>ctp_ntriginfo");
  tree->Draw("ctp_bcid>>ctp_bcid");
  tree->Draw("ctp_type>>ctp_type");
  tree->Draw("event_bcId>>evt_bcid");
  tree->Draw("event_lumiBlock>>evt_lb");
  tree->Draw("mdt_nMdt>>mdt_num");
  tree->Draw("mdt_adc>>mdt_adc");
  tree->Draw("mdt_r>>mdt_radius");
  tree->Draw("mdt_t>>mdt_time");
  tree->Draw("muctpi_sectorID>>muctpi_sectorid");
  tree->Draw("muctpi_eta>>muctpi_eta");
  tree->Draw("muctpi_phi>>muctpi_phi");
  tree->Draw("pat_nPatterns>>pat_num");
  tree->Draw("phi_nPhiHits>>phi_num");
  tree->Draw("phi_posZ>>phi_posZ");
  tree->Draw("rawCsc_nRCsc>>rawCsc_num");
  tree->Draw("rawCsc_t>>rawCsc_time");
  tree->Draw("rawCsc_charge>>rawCsc_charge");
  tree->Draw("rawMdt_nRMdt>>rawMdt_num");
  tree->Draw("rawMdt_adc>>rawMdt_adc");
  tree->Draw("rawMdt_r>>rawMdt_radius");
  tree->Draw("rawMdt_tdc>>rawMdt_tdc");
  tree->Draw("rawRpcCoin_nRpcCoin>>rawRpcCoin_num");
  tree->Draw("rawRpcCoin_parentSectorId>>rawRpcCoin_parsecid");
  tree->Draw("rawRpcCoin_t>>rawRpcCoin_time");
  tree->Draw("rawRpc_nRRpc>>rawRpc_num");
  tree->Draw("rawRpc_t>>rawRpc_time");
  tree->Draw("rawTgcCoin_nRTgc_All>>rawTgcCoin_num");
  tree->Draw("rawTgcCoin_eta_sl>>rawTgcCoin_eta_sl");
  tree->Draw("rawTgcCoin_eta_sl_Next>>rawTgcCoin_eta_sl_Next");
  tree->Draw("rawTgcCoin_eta_sl_Prev>>rawTgcCoin_eta_sl_Prev");
  tree->Draw("rawTgcCoin_phi_sl>>rawTgcCoin_phi_sl");
  tree->Draw("rawTgcCoin_phi_sl_Next>>rawTgcCoin_phi_sl_Next");
  tree->Draw("rawTgcCoin_phi_sl_Prev>>rawTgcCoin_phi_sl_Prev");
  tree->Draw("rawTgc_nRTgc_All_Bunch>>rawTgc_num");
  tree->Draw("rawTgc_bcTag>>rawTgc_bctag");
  tree->Draw("rawTgc_station>>rawTgc_station");
  tree->Draw("rawTgc_station_Next>>rawTgc_station_Next");
  tree->Draw("rawTgc_station_Prev>>rawTgc_station_Prev");
  tree->Draw("rpcOs_nRpcHits>>rpcOs_num");
  tree->Draw("rpcOs_time>>rpcOs_time");
  tree->Draw("rpcOs_distanceToRO>>rpcOs_distro");
  tree->Draw("rpc_SLnTriggerHits>>rpc_SLnum");
  tree->Draw("rpc_SLhit_sector>>rpc_SLhit_sector");
  tree->Draw("rpc_SLhit_roi>>rpc_SLhit_roi");
  tree->Draw("seg_nSegments>>seg_num");
  tree->Draw("seg_nHits>>seg_hits");
  tree->Draw("acos(seg_gDirZ)>>seg_theta");
  tree->Draw("acos(seg_gDirZ)>>seg_theta_mboy","seg_author==3");
  tree->Draw("acos(seg_gDirZ)>>seg_theta_moore","seg_author==4");
  tree->Draw("atan2(seg_gDirY,seg_gDirX)>>seg_phi");
  tree->Draw("tgc_nTgc>>tgc_num");
  tree->Draw("tgc_error>>tgc_error");
  tree->Draw("trkHit_nHits>>trkHit_num");
  tree->Draw("trkHit_resi>>trkHit_resi");
  tree->Draw("trk_nTracks>>trk_num");
  tree->Draw("1./trk_qOverP>>trk_qoverp");
  tree->Draw("sin(trk_theta)/abs(trk_qOverP)>>trk_pt");
  tree->Draw("sin(trk_theta)/abs(trk_qOverP)>>trk_pt_moore","trk_author==0");
  tree->Draw("sin(trk_theta)/abs(trk_qOverP)>>trk_pt_mboy","trk_author==100");
  tree->Draw("-log(tan(trk_theta/2.))>>trk_eta");
  tree->Draw("trk_phi>>trk_phi");

  fout->Write();
  fout->Close();
}

