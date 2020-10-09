// Macro for RTT commissioning jobs (R. Nikolaidou. B. Resende)
// plots from both the AANtuple and Calibration ntuples
// Adapted to collisions data (S.Willocq)
{
//Set Some default styles for the histogram plots
  gStyle->SetMarkerStyle(8);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.1);
  gStyle->SetStatFontSize(0.05);
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


// open ntuple files (Calibration Nutple and AANtuple)
  TFile *f1 = new TFile("CalibrationNtuple.root","READ");
  TTree *tree1 = (TTree*)f1->Get("PatternNtupleMaker/Segments");
  TFile *f2 = new TFile("ntuple.root","READ");
  TTree *tree2 = (TTree*)f2->Get("CollectionTree");
  TFile *fout = new TFile("MuonRec_histos.root","UPDATE");

// create histograms
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(111111);


  //-->AANtuple (Raw data information from MDT, RPC, TGC)
  //c1 Canvas 
  int mdtmaxprd = tree2->GetMaximum("mdt_nprd");
  int mdtbinprd = mdtmaxprd+1;  
  TH1F *mdtprd = new TH1F("mdtprd","MDT PrepRawData per Event",500,0,1000);
  TH1F *mdtstation = new TH1F("mdtstation", "MDT Station Number per Event",60,0,60);
  TH1F *mdteta = new TH1F("mdteta", "MDT Eta  Sector of the Chamber",16,-8,8);
  TH1F *mdtphi = new TH1F("mdtphi", "MDT Eta  Sector of the Chamber",9,0,9);
  TH2F *mdtposXposYBarrel =  new TH2F("mdtposXposYBarrel","XY view of MDT chambers for barrel", 220,-11000,11000,220,-11000,11000);
  TH2F *mdtposXposYEndcap =  new TH2F("mdtposXposYEndcap","XY view for MDT chambers for endcap ", 220,-11000,11000,220,-11000,11000);

  //c11 Canvas
  TH1F *mdttdc_barrel = new TH1F("mdttdc_barrel","MDT TDC counts in barrel",300,0,3000);
  TH1F *mdttdc_endcap = new TH1F("mdttdc_endcap","MDT TDC counts in endcap",300,0,3000);
  TH1F *mdttdc_barrel_ADCcut50 = new TH1F("mdttdc_barrel_ADCcut50","MDT TDC counts in barrel with ADC cut at 50",300,0,3000);
  TH1F *mdttdc_endcap_ADCcut50 = new TH1F("mdttdc_endcap_ADCcut50","MDT TDC counts in endcap with ADC cut at 50",300,0,3000);

  //c2 Canvas
  int rpcmaxprd = tree2->GetMaximum("rpc_nprd");
  int rpcbinprd = rpcmaxprd+1;  
  TH1F *rpcprd = new TH1F("rpcprd", "RPC PrepRawData Hits per Event",100,0,100);
  TH1F *rpcstation = new TH1F("rpcstation", "RPC Station Number per Event",60,0,60);
  TH1F *rpceta = new TH1F("rpceta", "RPC Eta Sector of the Chamber",16,-8,8);
  TH1F *rpcphi = new TH1F("rpcphi", "RPC Phi Sector of the Chamber",9,0,9);

  //c3 Canvas
  int tgcmaxprd = tree2->GetMaximum("tgc_nprd");
  int tgcbinprd = tgcmaxprd+1;  
  TH1F *tgcprd = new TH1F("tgcprd", "TGC PrepRawData Hits per Event",100,0,100);
  TH1F *tgcstation = new TH1F("tgcstation", "TGC Station Number per Event",60,0,60);
  TH1F *tgceta = new TH1F("tgceta", "TGC Eta Sector of the Chamber",16,-8,8);
  TH1F *tgcphi = new TH1F("tgcphi", "TGC Phi Sector of the Chamber",50,0,100);

  //c4 Canvas (for CSC chambers when on)

  //-->Calibration Ntuple (MDT information)
  //c5 Canvas
  TH1F *t = new TH1F("t","MDT : Drift Time",105,-100,2000);
  TH1F *r = new TH1F("r","MDT : Drift Radius",52,-26,26);
  TH1F *resi = new TH1F("resi","MDT : Residual",40,-10,10);
  TH1F *t0= new TH1F("t0","MDT : tube t0",50,0,1000);



  //c6 Canvas 
  TH1F *mboytracks = new TH1F("mboytracks"," Muonboy tracks per event", 10, 0, 10);
  TH1F *mboytracksqoverp = new TH1F("mboytracksqoverp"," Muonboy track q/p (1/MeV) ", 60, -150000., 150000.);
  TH1F *mboytrackspt = new TH1F("mboytrackspt"," Muonboy track pt (MeV)", 50, 0., 100000.);
  TH1F *mboytrackseta = new TH1F("mboytrackseta"," Muonboy track eta", 60, -3., 3.);
  TH1F *mboytracksphi = new TH1F("mboytracksphi"," Muonboy track phi", 60, -3.1416, 3.1416);
  //c61 Canvas
  TH1F *mboyhitsontracks = new TH1F("mboyhitsontracks"," Muonboy Hits on Track per Event", 50, 0,100);
  TH1F *mboyhitstech = new TH1F("mboyhitstech","Muonboy hits belong to MDT/RPC/TGC/CSC=1/2/3/4 ",8,0.,8);
  TH1F *mboytrackresiduals = new TH1F("mboytrackresiduals", " Muonboy MDT residuals on Track per Event", 50, -5, 5);
  TH1F *mboyhitsrpctime = new TH1F("mboyhitsrpctime", "Muonboy hits on Track time measured from RC",50,0,200);
  TH1F *mboyhitstgcchannel = new TH1F("mboyhitstgcchannel","Muonboy hits on Track TGC number of strip",500,0,500);
  TH1F *mboyhitscscwirelayer = new TH1F("mboyhitscscwirelayer","Muonboy hits on Track CSC wire layer ",4,0,4);

  //c62 Canvas 
  TH1F *mboyseg = new TH1F("mboyseg"," Muonboy Segments per Event", 100, 0, 100);
  TH1F *mboyhitsonseg = new TH1F("mboyhitsonseg"," Muonboy Total number of hits per Segment", 20, 0,20);
  TH1F *mboysegtheta = new TH1F("mboysegtheta"," Muonboy Segments Theta distribution ", 60,0,3.1416);
  TH1F *mboysegphi = new TH1F("mboysegphi"," Muonboy Segments Phi distribution ", 60, -3.1416, 3.1416);


 //c8 Canvas
  TH1F *mooretracks = new TH1F("mooretracks"," Moore Tracks per Event", 10, 0, 10);
  TH1F *mooretracksqoverp = new TH1F("mooretracksqoverp"," Moore track q/p (1/MeV) ", 60, -150000., 150000.);
  TH1F *mooretrackspt = new TH1F("mooretrackspt"," Moore track pt (MeV)", 50, 0., 100000.);
  TH1F *mooretrackseta = new TH1F("mooretrackseta"," Moore track eta", 60, -3., 3.);
  TH1F *mooretracksphi = new TH1F("mooretracksphi"," Moore track phi", 60, -3.1416, 3.1416);

  //c81 Canvas
  TH1F *mooremdthitsontrack = new TH1F("mooremdthitsontrack"," Moore MDT hits on Track ", 50,0,50);
  TH1F *moorecscetahitsontrack = new TH1F("moorecscetahitsontrack"," Moore CSC eta hits on Track ", 10,0,10);
  TH1F *moorecscphihitsontrack = new TH1F("moorecscphihitsontrack"," Moore CSC phi hits on Track ", 10,0,10);

  //c82 Canvas
  TH1F *moorerpcetahitsontrack = new TH1F("moorerpcetahitsontrack"," Moore RPC eta hits on Track ", 10,0,10);
  TH1F *moorerpcphihitsontrack = new TH1F("moorerpcphihitsontrack"," Moore RPC phi hits on Track ", 10,0,10);
  TH1F *mooretgcetahitsontrack = new TH1F("mooretgcetahitsontrack"," Moore TGC eta hits on Track ", 10,0,10);
  TH1F *mooretgcphihitsontrack = new TH1F("mooretgcphihitsontrack"," Moore TGC phi hits on Track ", 10,0,10);
 
  //c85 Canvas
  TH1F *mooreseg = new TH1F("mooreseg"," Moore Segments per Event", 100, 0, 100);
  TH1F *moorehitsonseg = new TH1F("moorehitsonseg"," Moore Hits per Segment", 20,0,20);
  TH1F *mooresegtheta = new TH1F("mooresegtheta"," Moore Segments Theta distribution ", 60,0,3.1416);
  TH1F *mooresegphi = new TH1F("mooresegphi"," Moore Segments Phi distribution ", 60, -3.1416, 3.1416);
			       


// fill histograms

  //c1 canvas
  tree2.Draw("mdt_nprd>>mdtprd");
  tree2.Draw("mdt_station>>mdtstation");
  tree2.Draw("mdt_eta>>mdteta");
  tree2.Draw("mdt_phi>>mdtphi");
  tree2.Draw("mdt_globPosX:mdt_globPosY>>mdtposXposYBarrel","mdt_station<11 ||mdt_station>49");
  tree2.Draw("mdt_globPosX:mdt_globPosY>>mdtposXposYEndcap","mdt_station>10 || mdt_station<50");
  //c11 Canvas
  tree2.Draw("mdt_tdc>>mdttdc_barrel","mdt_station<11 ||mdt_station>49");
  tree2.Draw("mdt_tdc>>mdttdc_endcap","mdt_station>10 || mdt_station<50");
  tree2.Draw("mdt_tdc>>mdttdc_barrel_ADCcut50","(mdt_station<11 ||mdt_station>49) && mdt_adc>50");
  tree2.Draw("mdt_tdc>>mdttdc_endcap_ADCcut50","(mdt_station>10 || mdt_station<50)&& mdt_adc>50");

  //c2 Canvas
  tree2.Draw("rpc_nprd>>rpcprd");
  tree2.Draw("rpc_prd_station>>rpcstation");
  tree2.Draw("rpc_prd_eta>>rpceta");
  tree2.Draw("rpc_prd_phi>>rpcphi");

  //c3 Canvas
  tree2.Draw("tgc_nprd>>tgcprd");
  tree2.Draw("tgc_station>>tgcstation");
  tree2.Draw("tgc_eta>>tgceta");
  tree2.Draw("tgc_phi>>tgcphi");

  //c5 Canvas
  tree1.Draw("mdt_t>>t");
  tree1.Draw("mdt_r>>r");
  tree1.Draw("mdt_resi>>resi");
  tree1.Draw("mdt_tube_t0>>t0");

  //c6 Canvas
  tree2.Draw("MboyTrkVT_NTrk>>mboytracks");

  tree1.Draw("1./trk_qOverP>>mboytracksqoverp","trk_author==100");
  tree1.Draw("sin(trk_theta)/abs(trk_qOverP)>>mboytrackspt","trk_author==100");
  tree1.Draw("-log(tan(trk_theta/2.))>>mboytrackseta","trk_author==100");
  tree1.Draw("trk_phi>>mboytracksphi","trk_author==100");

  //c71 Canvas
  tree2.Draw("MboyTrkVT_digits>>mboyhitsontracks");
  tree2.Draw("MboyTrkVT_TrHi_ChamberTech>>mboyhitstech");
  tree2.Draw("MboyTrkVT_TrHi_Rdigi-MboyTrkVT_TrHi_Rtrack>>mboytrackresiduals","MboyTrkVT_TrHi_ChamberTech==1");
  tree2.Draw("MboyTrkVT_TrHi_RpcTime>>mboyhitsrpctime");
  tree2.Draw("MboyTrkVT_TrHi_Channel>>mboyhitstgcchannel");
  tree2.Draw("MboyTrkVT_TrHi_ChamberLayer>>mboyhitscscwirelayer");

  //c72 Canvas
  tree2.Draw("MboySeg_nseg>>mboyseg"); 
  tree1.Draw("seg_nHits>>mboyhitsonseg","seg_author==3");
  tree1.Draw("acos(seg_gDirZ)>>mboysegtheta","seg_author==3");
  tree1.Draw("atan2(seg_gDirY,seg_gDirX)>>mboysegphi","seg_author==3");

  //c8 Canvas
  tree2.Draw("Moore_nmuon>>mooretracks");
  tree2.Draw("Moore_a0r>>mooretracksimpa0");
  tree2.Draw("Moore_z0r>>mooretracksimpZ");
  tree2.Draw("Moore_xvtxr>>mooretracksX");
  tree2.Draw("Moore_yvtxr>>mooretracksY");
  tree2.Draw("Moore_zvtxr>>mooretracksZ");

  tree1.Draw("1./trk_qOverP>>mooretracksqoverp","trk_author==0");
  tree1.Draw("sin(trk_theta)/abs(trk_qOverP)>>mooretrackspt","trk_author==0");
  tree1.Draw("-log(tan(trk_theta/2.))>>mooretrackseta","trk_author==0");
  tree1.Draw("trk_phi>>mooretracksphi","trk_author==0");

  //c81 Canvas
  tree2.Draw("Moore_mdth>>mooremdthitsontrack");
  tree2.Draw("Moore_cscetah>>moorecscetahitsontrack");
  tree2.Draw("Moore_cscphih>>moorecscphihitsontrack");

  //c82 Canvas
  tree2.Draw("Moore_rpcetah>>moorerpcetahitsontrack");
  tree2.Draw("Moore_rpcphih>>moorerpcphihitsontrack");
  tree2.Draw("Moore_tgcetah>>mooretgcetahitsontrack");
  tree2.Draw("Moore_tgcphih>>mooretgcphihitsontrack");

  //c85 Canvas
  tree2.Draw("MooreSegments_nseg>>mooreseg");
  tree1.Draw("seg_nHits>>moorehitsonseg","seg_author==4");
  tree1.Draw("acos(seg_gDirZ)>>mooresegtheta","seg_author==4");
  tree1.Draw("atan2(seg_gDirY,seg_gDirX)>>mooresegphi","seg_author==4");

  //MboyTrkVT_CotTh,MboyTrkVT_Phi,MboyTrkVT_PTInv
  tree2.Draw("MooreSegments_nseg>>seg_nsegMoo");
  tree2.Draw("MboySeg_nseg>>seg_nsegMB");

//Prepare canvas with plots 

  //c1:MDT part I raw info 
  TCanvas *c1 = new TCanvas("c1","MDT  Information",1200,1200);
  c1->Divide(2,3);
  c1.cd(1);
  mdtprd->SetFillColor(2);
  mdtprd.Draw();
  c1.cd(2);
  mdtstation->SetFillColor(2);
  mdtstation.Draw();
  c1.cd(3);
  mdteta->SetFillColor(2);
  mdteta.Draw();
  c1.cd(4);
  mdtphi->SetFillColor(2);
  mdtphi.Draw();
  c1.cd(5);
  mdtposXposYBarrel->SetFillColor(2);
  mdtposXposYBarrel.Draw("box");
  c1.cd(6);
  mdtposXposYEndcap->SetFillColor(2);
  mdtposXposYEndcap.Draw("box");
  c1.Update();
  c1.SaveAs("AAN_mdt_raw.gif");

  //c11:MDT part II raw info 
  TCanvas *c11 = new TCanvas("c11","MDT TDC/ADC Information",1200,800);
  c11->Divide(2,2);
  c11.cd(1);
  mdttdc_barrel->SetFillColor(2);
  mdttdc_barrel.Draw();
  c11.cd(2);
  mdttdc_endcap->SetFillColor(2);
  mdttdc_endcap.Draw(); 
  c11.cd(3);
  mdttdc_barrel_ADCcut50->SetFillColor(2);
  mdttdc_barrel_ADCcut50.Draw();
  c11.cd(4);
  mdttdc_endcap_ADCcut50->SetFillColor(2);
  mdttdc_endcap_ADCcut50.Draw(); 
  c11.Update();
  c11.SaveAs("AAN_mdt_raw_tdc.gif");

  //c2:RPC part I raw info 
  TCanvas *c2 = new TCanvas("c2","RPC  Information",1200,800);
  c2->Divide(2,2);
  c2.cd(1);
  rpcprd->SetFillColor(2);
  rpcprd.Draw();
  c2.cd(2);
  rpcstation->SetFillColor(2);
  rpcstation.Draw();
  c2.cd(3);
  rpceta->SetFillColor(2);
  rpceta.Draw();
  c2.cd(4);
  rpcphi->SetFillColor(2);
  rpcphi.Draw();
  c2.Update();
  c2.SaveAs("AAN_rpc_raw.gif");

  //c3:TGC part I raw info 
  TCanvas *c3 = new TCanvas("c3","TGC  Information",1200,800);
  c3->Divide(2,2);
  c3.cd(1);
  tgcprd->SetFillColor(2);
  tgcprd.Draw();
  c3.cd(2);
  tgcstation->SetFillColor(2);
  tgcstation.Draw();
  c3.cd(3);
  tgceta->SetFillColor(2);
  tgceta.Draw();
  c3.cd(4);
  tgcphi->SetFillColor(2);
  tgcphi.Draw();
  c3.Update();
  c3.SaveAs("AAN_tgc_raw.gif");

  //c5:MDT part I reco info from Calibration ntuple 
  TCanvas *c5 = new TCanvas("c5","MDT R-T Information",1200,800);
  c5->Divide(2,2);
  c5.cd(1);
  t.SetFillColor(2);
  t.Draw();
  c5.cd(2);
  r.SetFillColor(2);
  r.Draw();
  c5.cd(3);
  resi.SetFillColor(2);
  resi.Draw();
  c5.cd(4);
  t0.SetFillColor(2);
  t0.Draw();
  c5.Update();
  c5.SaveAs("CalNt_mdt_rt_info.gif");


  //c6:Mboy Tracks
  TCanvas *c6 = new TCanvas("c6","Muonboy Track Information",1800,800);
  c6->Divide(3,2);
  c6.cd(1);
  mboytracks.SetFillColor(2);
  mboytracks.Draw();
  c6.cd(2);
  mboytracksqoverp.SetFillColor(2);
  mboytracksqoverp.Draw();
  c6.cd(3);
  mboytrackspt.SetFillColor(2);
  mboytrackspt.Draw();
  c6.cd(4);
  mboytrackseta.SetFillColor(2);
  mboytrackseta.Draw();
  c6.cd(5);
  mboytracksphi.SetMinimum(0);
  mboytracksphi.SetFillColor(2);
  mboytracksphi.Draw();
  c6.Update();
  c6.SaveAs("AAN_mboytracks.gif");

  //c61:Mboy Hits on Track 
  TCanvas *c61 = new TCanvas("c61","Muonboy Hits on Track Information",1200,1200);
  c61->Divide(2,3);
  c61.cd(1);
  mboyhitsontracks.SetFillColor(2);
  mboyhitsontracks.Draw();
  c61.cd(2);
  mboyhitstech.SetFillColor(2);
  mboyhitstech.Draw();
  c61.cd(3);
  mboytrackresiduals.SetFillColor(2);
  mboytrackresiduals.Draw();
  c61.cd(4);
  mboyhitsrpctime.SetFillColor(2);
  mboyhitsrpctime.Draw();
  c61.cd(5);
  mboyhitstgcchannel.SetFillColor(2);
  mboyhitstgcchannel.Draw();
  c61.cd(6);
  //  mboytgchitscscwirelayer.SetFillColor(2);
  //  mboytgchitscscwirelayer.Draw();
  c61.Update();
  c61.SaveAs("AAN_mboyhitsontrack.gif");


  //c62 Canvas
  TCanvas *c62 = new TCanvas("c62","Muonboy Segment Information",1200,800);
  c62->Divide(2,2);
  c62.cd(1);
  mboyseg.SetFillColor(2);
  mboyseg.Draw();
  c62.cd(2);
  mboyhitsonseg.SetFillColor(2);
  mboyhitsonseg.Draw();
  c62.cd(3);
  mboysegtheta.SetFillColor(2);
  mboysegtheta.Draw();
  c62.cd(4);
  mboysegphi.SetMinimum(0);
  mboysegphi.SetFillColor(2);
  mboysegphi.Draw();
  c62.Update();
  c62.SaveAs("AAN_mboysegments.gif");


  //c8:Moore Tracks
  TCanvas *c8 = new TCanvas("c8","Moore Track Information",1800,800);
  c8->Divide(3,2);
  c8.cd(1);
  mooretracks.SetFillColor(2);
  mooretracks.Draw();
  c8.cd(2);
  mooretracksqoverp.SetFillColor(2);
  mooretracksqoverp.Draw();
  c8.cd(3);
  mooretrackspt.SetFillColor(2);
  mooretrackspt.Draw();
  c8.cd(4);
  mooretrackseta.SetFillColor(2);
  mooretrackseta.Draw();
  c8.cd(5);
  mooretracksphi.SetMinimum(0);
  mooretracksphi.SetFillColor(2);
  mooretracksphi.Draw();
  c8.Update();
  c8.SaveAs("AAN_mooretracks.gif");

  //c81:Moore Precision Hits on Track
  TCanvas *c81 = new TCanvas("c81","Moore Precision Hits on Track Information",1200,1200);
  c81->Divide(3,3);
  c81.cd(3);
  mooremdthitsontrack.SetFillColor(2);
  mooremdthitsontrack.Draw();
  c81.cd(1);
  moorecscetahitsontrack.SetFillColor(2);
  moorecscetahitsontrack.Draw();
  c81.cd(2);
  moorecscphihitsontrack.SetFillColor(2);
  moorecscphihitsontrack.Draw();
  c81.cd(4);
  moorerpcetahitsontrack.SetFillColor(2); 
  moorerpcetahitsontrack.Draw(); 
  c81.cd(5);
  moorerpcphihitsontrack.SetFillColor(2); 
  moorerpcphihitsontrack.Draw(); 
  c81.cd(7);
  mooretgcetahitsontrack.SetFillColor(2); 
  mooretgcetahitsontrack.Draw(); 
  c81.cd(8);
  mooretgcphihitsontrack.SetFillColor(2); 
  mooretgcphihitsontrack.Draw(); 
  c81.Update();
  c81.SaveAs("AAN_moorehitsontrack.gif");

  //c85:Moore Segments
  TCanvas *c85 = new TCanvas("c85","Moore Segments Information",1200,800);
  c85->Divide(2,2);
  c85.cd(1);
  mooreseg.SetFillColor(2);
  mooreseg.Draw();
  c85.cd(2);
  moorehitsonseg.SetFillColor(2);
  moorehitsonseg.Draw();
  c85.cd(3);
  mooresegtheta.SetFillColor(2);
  mooresegtheta.Draw();
  c85.cd(4);
  mooresegphi.SetMinimum(0);
  mooresegphi.SetFillColor(2);
  mooresegphi.Draw();
  c85.Update();
  c85.SaveAs("AAN_mooresegments.gif");

  fout->Write();
  fout->Close();

}

