{
// RelocatedMuonHits_Plots.C
// 
// Make simple hit map plots

// open ntuple file
TChain tc("COL/1");
tc.Add("RelocatedMuonHits.root");

// create histograms
gROOT->SetStyle("Plain");
gStyle->SetOptStat(111111);
TH1F *phires = new TH1F("phires","phi residual",100,-0.010,0.010);
TH1F *phiresmdt = new TH1F("phiresmdt","MDT phi residual",100,-0.010,0.010);
TH1F *phirescsc = new TH1F("phirescsc","CSC phi residual",100,-0.010,0.010);
TH1F *phiresrpc = new TH1F("phiresrpc","RPC phi residual",100,-0.010,0.010);
TH1F *phirestgc = new TH1F("phirestgc","TGC phi residual",100,-0.010,0.010);
TH1F *zres = new TH1F("zres","z residual (mm)",100,-0.01,0.01);
TH1F *zresmdt = new TH1F("zresmdt","MDT z residual (mm)",100,-0.01,0.01);
TH1F *zrescsc = new TH1F("zrescsc","CSC z residual (mm)",100,-0.01,0.01);
TH1F *zresrpc = new TH1F("zresrpc","RPC z residual (mm)",100,-0.01,0.01);
TH1F *zrestgc = new TH1F("zrestgc","TGC z residual (mm)",100,-0.01,0.01);
TH1F *dist1 = new TH1F("dist1","dist (mm)",100,0.0,0.1);
TH1F *dist1mdt = new TH1F("dist1mdt","MDT dist (mm)",100,0.0,0.1);
TH1F *dist1csc = new TH1F("dist1csc","CSC dist (mm)",100,0.0,0.1);
TH1F *dist1rpc = new TH1F("dist1rpc","RPC dist (mm)",100,0.0,0.1);
TH1F *dist1tgc = new TH1F("dist1tgc","TGC dist (mm)",100,0.0,0.1);
TH1F *dist2 = new TH1F("dist2","dist (mm)",100,0.0,0.001);
TH1F *dist2mdt = new TH1F("dist2mdt","MDT dist (mm)",100,0.0,0.001);
TH1F *dist2csc = new TH1F("dist2csc","CSC dist (mm)",100,0.0,0.001);
TH1F *dist2rpc = new TH1F("dist2rpc","RPC dist (mm)",100,0.0,0.001);
TH1F *dist2tgc = new TH1F("dist2tgc","TGC dist (mm)",100,0.0,0.001);
TH1F *tech = new TH1F("tech","tech",5,0.0,5);
TH1F *statname = new TH1F("statname","station name",60,0.0,60.0);
TH1F *statnamemdt = new TH1F("statnamemdt","MDT station name",60,0.0,60.0);
TH1F *statnamecsc = new TH1F("statnamecsc","CSC station name",60,0.0,60.0);
TH1F *statnamerpc = new TH1F("statnamerpc","RPC station name",60,0.0,60.0);
TH1F *statnametgc = new TH1F("statnametgc","TGC station name",60,0.0,60.0);
TH1F *stateta = new TH1F("stateta","station eta index",16,-10,10);
TH1F *statetamdt = new TH1F("statetamdt","MDT station eta index",16,-10,10);
TH1F *statetacsc = new TH1F("statetacsc","CSC station eta index",16,-10,10);
TH1F *statetarpc = new TH1F("statetarpc","RPC station eta index",16,-10,10);
TH1F *statetatgc = new TH1F("statetatgc","TGC station eta index",16,-10,10);
TH1F *statphi = new TH1F("statphi","station phi index",60,-10,50);
TH1F *statphimdt = new TH1F("statphimdt","MDT station phi index",18,0,18);
TH1F *statphicsc = new TH1F("statphicsc","CSC station phi index",18,0,18);
TH1F *statphirpc = new TH1F("statphirpc","RPC station phi index",18,0,18);
TH1F *statphitgc = new TH1F("statphitgc","TGC station phi index",50,0,50);

TH2F *etaphi = new TH2F("etaphi","hit phi vs. eta;hit eta;hit phi",600,-3.0,3.0,720,-3.1416,3.1416);
gStyle->SetMarkerColor(kBlack); 
TH2F *etaphimdt = new TH2F("etaphimdt","MDT hit phi vs. eta;hit eta;hit phi",600,-3.0,3.0,720,-3.1416,3.1416);
gStyle->SetMarkerColor(kBlue); 
TH2F *etaphicsc = new TH2F("etaphicsc","CSC hit phi vs. eta;hit eta;hit phi",600,-3.0,3.0,720,-3.1416,3.1416);
gStyle->SetMarkerColor(kRed); 
TH2F *etaphirpc = new TH2F("etaphirpc","RPC hit phi vs. eta;hit eta;hit phi",600,-3.0,3.0,720,-3.1416,3.1416);
gStyle->SetMarkerColor(kGreen); 
TH2F *etaphitgc = new TH2F("etaphitgc","TGC hit phi vs. eta;hit eta;hit phi",600,-3.0,3.0,720,-3.1416,3.1416);
gStyle->SetMarkerColor(kBlack); 

gStyle->SetMarkerColor(kBlack); 
TH2F *rzmdt = new TH2F("rzmdt",";z (mm);R (mm)",1000,-24000,24000,1000,0,13000);
TH2F *srzmdt = new TH2F("srzmdt",";z (mm);R (mm)",1000,-24000,24000,1000,-13000,13000);
gStyle->SetMarkerColor(kBlue); 
TH2F *rzcsc = new TH2F("rzcsc",";z (mm);R (mm)",1000,-24000,24000,1000,0,13000);
TH2F *srzcsc = new TH2F("srzcsc",";z (mm);R (mm)",1000,-24000,24000,1000,-13000,13000);
gStyle->SetMarkerColor(kRed); 
TH2F *rzrpc = new TH2F("rzrpc",";z (mm);R (mm)",1000,-24000,24000,1000,0,13000);
TH2F *srzrpc = new TH2F("srzrpc",";z (mm);R (mm)",1000,-24000,24000,1000,-13000,13000);
gStyle->SetMarkerColor(kGreen); 
TH2F *rztgc = new TH2F("rztgc",";z (mm);R (mm)",1000,-24000,24000,1000,0,13000);
TH2F *srztgc = new TH2F("srztgc",";z (mm);R (mm)",1000,-24000,24000,1000,-13000,13000);
gStyle->SetMarkerColor(kBlack); 

// fill histograms
gStyle->SetOptStat(111111);
tc.Draw("ZRESID>>zres");
tc.Draw("ZRESID>>zresmdt","TECH==0");
tc.Draw("ZRESID>>zrescsc","TECH==1");
tc.Draw("ZRESID>>zresrpc","TECH==2");
tc.Draw("ZRESID>>zrestgc","TECH==3");
tc.Draw("PHIRESID>>phires");
tc.Draw("PHIRESID>>phiresmdt","TECH==0");
tc.Draw("PHIRESID>>phirescsc","TECH==1");
tc.Draw("PHIRESID>>phiresrpc","TECH==2");
tc.Draw("PHIRESID>>phirestgc","TECH==3");
tc.Draw("DIST>>dist1");
tc.Draw("DIST>>dist1mdt","TECH==0");
tc.Draw("DIST>>dist1csc","TECH==1");
tc.Draw("DIST>>dist1rpc","TECH==2");
tc.Draw("DIST>>dist1tgc","TECH==3");
tc.Draw("DIST>>dist2");
tc.Draw("DIST>>dist2mdt","TECH==0");
tc.Draw("DIST>>dist2csc","TECH==1");
tc.Draw("DIST>>dist2rpc","TECH==2");
tc.Draw("DIST>>dist2tgc","TECH==3");
tc.Draw("TECH>>tech");
tc.Draw("STNAME>>statname");
tc.Draw("STNAME>>statnamemdt","TECH==0");
tc.Draw("STNAME>>statnamecsc","TECH==1");
tc.Draw("STNAME>>statnamerpc","TECH==2");
tc.Draw("STNAME>>statnametgc","TECH==3");
tc.Draw("STETA>>stateta");
tc.Draw("STETA>>statetamdt","TECH==0");
tc.Draw("STETA>>statetacsc","TECH==1");
tc.Draw("STETA>>statetarpc","TECH==2");
tc.Draw("STETA>>statetatgc","TECH==3");
tc.Draw("STPHI>>statphi");
tc.Draw("STPHI>>statphimdt","TECH==0");
tc.Draw("STPHI>>statphicsc","TECH==1");
tc.Draw("STPHI>>statphirpc","TECH==2");
tc.Draw("STPHI>>statphitgc","TECH==3");

tc.Draw("sqrt(X*X+Y*Y):Z>>rzmdt","TECH==0");
tc.Draw("sqrt(X*X+Y*Y):Z>>rzcsc","TECH==1");
tc.Draw("sqrt(X*X+Y*Y):Z>>rzrpc","TECH==2");
tc.Draw("sqrt(X*X+Y*Y):Z>>rztgc","TECH==3");
tc.Draw("sqrt(X*X+Y*Y)*abs(Y)/Y:Z>>srzmdt","TECH==0");
tc.Draw("sqrt(X*X+Y*Y)*abs(Y)/Y:Z>>srzcsc","TECH==1");
tc.Draw("sqrt(X*X+Y*Y)*abs(Y)/Y:Z>>srzrpc","TECH==2");
tc.Draw("sqrt(X*X+Y*Y)*abs(Y)/Y:Z>>srztgc","TECH==3");

tc.Draw("Phi:-log(tan(Theta/2.))>>etaphi");
tc.Draw("Phi:-log(tan(Theta/2.))>>etaphimdt","TECH==0");
tc.Draw("Phi:-log(tan(Theta/2.))>>etaphicsc","TECH==1");
tc.Draw("Phi:-log(tan(Theta/2.))>>etaphirpc","TECH==2");
tc.Draw("Phi:-log(tan(Theta/2.))>>etaphitgc","TECH==3");

// Prepare output plots
TCanvas *c1 = new TCanvas("c1","Residuals.C");
c1->Divide(2,2);
c1.cd(1);
phires.Draw();
c1.cd(2);
zres.Draw();
c1.cd(3);
dist1.Draw();
c1.cd(4);
dist2.Draw();
c1.Update();
c1.SaveAs("Residuals.gif");

TCanvas *c1 = new TCanvas("c1","ResidualsMDT.C");
c1->Divide(2,2);
c1.cd(1);
phiresmdt.Draw();
c1.cd(2);
zresmdt.Draw();
c1.cd(3);
dist1mdt.Draw();
c1.cd(4);
dist2mdt.Draw();
c1.Update();
c1.SaveAs("ResidualsMDT.gif");

TCanvas *c1 = new TCanvas("c1","ResidualsCSC.C");
c1->Divide(2,2);
c1.cd(1);
phirescsc.Draw();
c1.cd(2);
zrescsc.Draw();
c1.cd(3);
dist1csc.Draw();
c1.cd(4);
dist2csc.Draw();
c1.Update();
c1.SaveAs("ResidualsCSC.gif");

TCanvas *c1 = new TCanvas("c1","ResidualsRPC.C");
c1->Divide(2,2);
c1.cd(1);
phiresrpc.Draw();
c1.cd(2);
zresrpc.Draw();
c1.cd(3);
dist1rpc.Draw();
c1.cd(4);
dist2rpc.Draw();
c1.Update();
c1.SaveAs("ResidualsRPC.gif");

TCanvas *c1 = new TCanvas("c1","ResidualsTGC.C");
c1->Divide(2,2);
c1.cd(1);
phirestgc.Draw();
c1.cd(2);
zrestgc.Draw();
c1.cd(3);
dist1tgc.Draw();
c1.cd(4);
dist2tgc.Draw();
c1.Update();
c1.SaveAs("ResidualsTGC.gif");

TCanvas *c1 = new TCanvas("c1","LargeResiduals.C");
c1->Divide(2,3);
c1->cd(1);
tc->Draw("ZRESID","abs(ZRESID)<1.e-3","", 138062, 0);
c1->cd(2);
tc->Draw("ZRESID","abs(ZRESID)>=1.e-3","", 138062, 0);
c1->cd(3);
tc->Draw("TECH","abs(ZRESID)>=1.e-3","", 138062, 0);
c1->cd(4);
tc->Draw("TECH:STNAME","abs(ZRESID)>=1.e-3","BOX", 138062, 0);
c1->cd(5);
tc->Draw("TECH","abs(ZRESID)<1.e-3","", 138062, 0);
c1->cd(6);
tc->Draw("STPHI:STETA","abs(ZRESID)>=1.e-3","BOX", 138062, 0);
c1->Update();
c1.SaveAs("LargeResiduals.gif");

TCanvas *c1 = new TCanvas("c1","Stations.C");
c1->Divide(2,2);
c1.cd(1);
tech.Draw();
c1.cd(2);
statname.Draw();
c1.cd(3);
stateta.Draw();
c1.cd(4);
statphi.Draw();
c1.Update();
c1.SaveAs("Stations.gif");

TCanvas *c1 = new TCanvas("c1","StationsMDT.C");
c1->Divide(2,2);
c1.cd(1);
statnamemdt.Draw();
c1.cd(2);
statetamdt.Draw();
c1.cd(3);
statphimdt.Draw();
c1.cd(4);
c1.Update();
c1.SaveAs("StationsMDT.gif");

TCanvas *c1 = new TCanvas("c1","StationsCSC.C");
c1->Divide(2,2);
c1.cd(1);
statnamecsc.Draw();
c1.cd(2);
statetacsc.Draw();
c1.cd(3);
statphicsc.Draw();
c1.cd(4);
c1.Update();
c1.SaveAs("StationsCSC.gif");

TCanvas *c1 = new TCanvas("c1","StationsRPC.C");
c1->Divide(2,2);
c1.cd(1);
statnamerpc.Draw();
c1.cd(2);
statetarpc.Draw();
c1.cd(3);
statphirpc.Draw();
c1.cd(4);
c1.Update();
c1.SaveAs("StationsRPC.gif");

TCanvas *c1 = new TCanvas("c1","StationsTGC.C");
c1->Divide(2,2);
c1.cd(1);
statnametgc.Draw();
c1.cd(2);
statetatgc.Draw();
c1.cd(3);
statphitgc.Draw();
c1.cd(4);
c1.Update();
c1.SaveAs("StationsTGC.gif");

// TCanvas *c1 = new TCanvas("c1","Hits_EtaPhi.C");
// c1->Divide(1,1);
// c1.cd(1);
// etaphi.Draw();
// c1.Update();
// c1.SaveAs("Hits_EtaPhi.gif");

gStyle->SetOptStat(0);

TCanvas *c1 = new TCanvas("c1","HitsMDT_EtaPhi.C");
c1->Divide(1,1);
c1.cd(1);
etaphimdt.Draw();
c1.Update();
c1.SaveAs("HitsMDT_EtaPhi.gif");

TCanvas *c1 = new TCanvas("c1","HitsCSC_EtaPhi.C");
c1->Divide(1,1);
c1.cd(1);
etaphicsc.Draw();
c1.Update();
c1.SaveAs("HitsCSC_EtaPhi.gif");

TCanvas *c1 = new TCanvas("c1","HitsRPC_EtaPhi.C");
c1->Divide(1,1);
c1.cd(1);
etaphirpc.Draw();
c1.Update();
c1.SaveAs("HitsRPC_EtaPhi.gif");

TCanvas *c1 = new TCanvas("c1","HitsTGC_EtaPhi.C");
c1->Divide(1,1);
c1.cd(1);
etaphitgc.Draw();
c1.Update();
c1.SaveAs("HitsTGC_EtaPhi.gif");

TCanvas *c1 = new TCanvas("c1","HitsBarrelMDT_XY.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("Y:X","TECH==0&&(STNAME<13||STNAME>51)");
c1.Update();
c1.SaveAs("HitsBarrelMDT_XY.gif");

TCanvas *c1 = new TCanvas("c1","HitsEndcapMDT_XY.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("Y:X","TECH==0&&STNAME>12&&STNAME<40");
c1.Update();
c1.SaveAs("HitsEndcapMDT_XY.gif");

TCanvas *c1 = new TCanvas("c1","HitsMDT_RZ.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("sqrt(X*X+Y*Y):Z","TECH==0");
c1.Update();
c1.SaveAs("HitsMDT_RZ.gif");

TCanvas *c1 = new TCanvas("c1","HitsCSC_XY.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("Y:X","TECH==1");
c1.Update();
c1.SaveAs("HitsCSC_XY.gif");

TCanvas *c1 = new TCanvas("c1","HitsRPC_XY.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("Y:X","TECH==2");
c1.Update();
c1.SaveAs("HitsRPC_XY.gif");

TCanvas *c1 = new TCanvas("c1","HitsTGC_XY.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("Y:X","TECH==3");
c1.Update();
c1.SaveAs("HitsTGC_XY.gif");

gStyle->SetOptStat(0);
TCanvas *c1 = new TCanvas("c1","HitsAll_EtaPhi.C");
c1->Divide(1,1);
c1.cd(1);
etaphimdt.Draw();
etaphicsc.Draw("same");
etaphirpc.Draw("same");
etaphitgc.Draw("same");
c1.Update();
c1.SaveAs("HitsAll_EtaPhi.gif");

TCanvas *c1 = new TCanvas("c1","HitsAll_RZ.C");
c1->Divide(1,1);
c1.cd(1);
rzmdt.Draw();
rzcsc.Draw("same");
rzrpc.Draw("same");
rztgc.Draw("same");
c1.Update();
c1.SaveAs("HitsAll_RZ.gif");

TCanvas *c1 = new TCanvas("c1","HitsAll_signedRZ.C");
c1->Divide(1,1);
c1.cd(1);
srzmdt.Draw();
srzcsc.Draw("same");
srzrpc.Draw("same");
srztgc.Draw("same");
c1.Update();
c1.SaveAs("HitsAll_signedRZ.gif");

cout<< "All done - bye!"<<endl;
 

}
