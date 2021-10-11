/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>
#include <cmath>
#include <map>
#include <set>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TLine.h"

#include "vrtTree.C"
#include "bsTree.C"

#include "../src/BeamSpotID.cxx"
using namespace BeamSpot;

void plotBeamspots();
void standardBsOnlyPlots(TTree *);
void standardVrtOnlyPlots(TTree *);

struct VrtH { // holder for histograms for each ID 
  VrtH(){}
  TH1D *hx{},*hy{},*hz{};
  TH1D *hxbs{},*hybs{},*hzbs{};
  TProfile* vrtxvsz{}, *vrtyvsz{};
  TH1D *hpullx{}, *hpully{}, *hpullz{};
};

struct BsH { // holder for BS values
  BsH(){}
  double xc{},yc{},x0{},y0{},z{};
  double ax{},ay{},sx{},sy{},sxc{},syc{},sz{};
  double rho{},k{};
};
std::map<ID,BsH> _beamspotMap;


using namespace std;

  // global default names
  std::string beamfile("beamspot.root");
  std::string tDir("Beamspot/");
  std::string bst(tDir + "Beamspots");
  std::string vrtt(tDir + "Vertices");

  std::string outputPlotDir("plots/");

TFile *fout(0);


int main(int argc, char* argv[]) {

  if (argc >1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
    cout << "usage:\n\t" << argv[0] << " [roofile.root [beamspotTree vertexTree [treeDir [plotDir] ]]]" << endl;
    exit(1);
  }

  if (argc > 1) {
    beamfile = argv[1];
    if (argc >= 4) {
      bst = argv[2];
      vrtt= argv[3];
    }
    if (argc >=5) {
      tDir = argv[4];
      tDir += "/";
      bst = tDir + argv[2];
      vrtt= tDir + argv[3];
    }
    if (argc >=6) {
      outputPlotDir = argv[5];
    }
    
  }
  plotBeamspots();
  return 0;
}


void RootSetup(){
  cout << "Setting up myRoot" << endl;
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(256);
  
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(1112211);
  gStyle->SetTitleBorderSize(0);
}
void simpleWrite(TH1*h,const std::string & drawOpt = "") {
  if (!h) return;
  TCanvas C;
  h->Draw(drawOpt.c_str());
  h->SetLineColor(kRed-0);
  std::string nam = outputPlotDir + h->GetName() + ".eps";
  std::string nam2 = outputPlotDir + h->GetName() + ".png";
  C.SaveAs(nam.c_str());
  C.SaveAs(nam2.c_str());

}



void plotBeamspots() {
  // simple start to plot a 'default' set of beamspots
  RootSetup();

  if (!gFile) {
    // attempt to open 'standard' file
    TFile *_fii = TFile::Open(beamfile.c_str());
    if (!_fii) {
      std::cout << "Error opening file: " << beamfile << std::endl;
      return;
    }
  }

  // find the Beamspot and Vertices trees
  TTree * bs = (TTree*)gFile->Get(bst.c_str());
  
  if (!bs) {
    cout << "No Beamspot directory: " << bst << " was found, skipping beamspot plots" << endl;
  } else {
    standardBsOnlyPlots(bs);
  }

  if (gFile) gFile->Close();
  {
    // attempt to open 'standard' file
    TFile *_fii = TFile::Open(beamfile.c_str());
    if (!_fii) {
      std::cout << "Error opening file: " << beamfile << std::endl;
      return;
    }
  }
  TTree * vrt = (TTree*)gFile->Get(vrtt.c_str());
  if (!vrt) {
    cout << "No Vertices  directory: " << vrtt << " was found, skipping vertex plots" << endl;
  } else {
    gFile->cd();
    standardVrtOnlyPlots(vrt);
  }


} // plotBeamspots


void setMinMax( double x, double &xmin, double &xmax) {
  if ( x < xmin ) xmin =x;
  if ( x > xmax ) xmax =x;
}
void scaleMinMax(double k, double &xmin, double &xmax) {
  double d = xmax - xmin;
  xmin -= d*k;
  xmax += d*k;
}

void standardBsOnlyPlots(TTree * bs) {
  if (!bs) return;
  
  // start loop over trees
  bsTree t(bs);
   if (t.fChain == 0) return;
   std::set<ID> _idmap;

   Long64_t nentries = t.fChain->GetEntriesFast();
   
   double xmin(10.),xmax(-10.), ymin(10.), ymax(-10.), zmin(400.), zmax(-400.);
   double xcmin(10.),xcmax(-10.), ycmin(10.), ycmax(-10.);
   double axmin(10.),axmax(-10.), aymin(10.), aymax(-10.);
   double sxmin(10.),sxmax(-10.), symin(10.), symax(-10.), szmin(400.), szmax(-400.);
   double kmin(10.),kmax(-10.), rhomin(-1.), rhomax(1.);
   double ntrkmin(0.),ntrkmax(100);

   // first loop to get ranges
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = t.LoadTree(jentry);
      if (ientry < 0) break;
      nb = t.fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      // create ID
      //ID id(t.event_run, t.event_id);
      ID id(t.event_run, t.event_id, t.event_lumiStart, t.event_lumiRange );
      
      _idmap.insert(id);

      setMinMax(t.bs_x0,xmin,xmax);
      setMinMax(t.bs_y0,ymin,ymax);
      setMinMax(t.bsCentroid_xc,xcmin,xcmax);
      setMinMax(t.bsCentroid_yc,ycmin,ycmax);
      setMinMax(t.bs_z ,zmin,zmax);
      setMinMax(t.bs_ax,axmin,axmax);
      setMinMax(t.bs_ay,aymin,aymax);
      setMinMax(t.bs_sx,sxmin,sxmax);
      setMinMax(t.bs_sy,symin,symax);
      setMinMax(t.bs_sz,szmin,szmax);
      setMinMax(t.bs_k,kmin,kmax);
      setMinMax(t.bs_rhoxy,rhomin,rhomax);
      setMinMax(t.event_nEvents,ntrkmin,ntrkmax);


      BsH & h = _beamspotMap[id];
      h.x0 = t.bs_x0;
      h.y0 = t.bs_y0;
      h.xc = t.bsCentroid_xc;
      h.yc = t.bsCentroid_yc;      
      h.z  = t.bs_z;
      h.ax = t.bs_ax;
      h.ay = t.bs_ay;
      h.sx = t.bs_sx;
      h.sy = t.bs_sy;
      h.sz = t.bs_sz;
      h.sxc= t.bsCentroid_sxc;
      h.syc = t.bsCentroid_syc;
      h.k = t.bs_k;
      h.rho = t.bs_rhoxy;



   } // for loop
   //scale values
   double k =0.08;
   scaleMinMax(k,xmin,xmax);
   scaleMinMax(k,xcmin,xcmax);
   scaleMinMax(k,ymin,ymax);
   scaleMinMax(k,ycmin,ycmax);
   scaleMinMax(k,zmin,zmax);
   scaleMinMax(k,sxmin,sxmax);
   scaleMinMax(k,symin,symax);
   scaleMinMax(k,szmin,szmax);
   scaleMinMax(k,kmin,kmax);
   scaleMinMax(k,rhomin,rhomax);
   scaleMinMax(k,axmin,axmax);
   scaleMinMax(k,aymin,aymax);
   scaleMinMax(k,ntrkmin,ntrkmax);


   // create the histograms
   TH1D * hx0 = new TH1D("hx0","Beamspot x-position (z=0); x [mm]; events",100,xmin,xmax);
   TH1D * hy0 = new TH1D("hy0","Beamspot y-position (z=0); y [mm]; events",100,ymin,ymax);
   TH1D * hz  = new TH1D("hz" ,"Beamspot z-position; z [mm]; events",100,zmin,zmax);
   TH1D * hax = new TH1D("hax","Beamspot x-slope; x-slope; events",100,axmin,axmax);
   TH1D * hay = new TH1D("hay","Beamspot y-slope; y-slope; events",100,aymin,aymax);
   TH1D * hsx = new TH1D("hsx","Beamspot x-width (z=0); #sigma(x) [mm]; events",100,sxmin,sxmax);
   TH1D * hsy = new TH1D("hsy","Beamspot y-width (z=0); #sigma(y) [mm]; events",100,symin,symax);
   TH1D * hsz = new TH1D("hsz","Beamspot z-width; #sigma(z) [mm]; events",100,szmin,szmax);
   TH1D * hk  = new TH1D("hk" ,"Beamspot k-factor; k; events",100,kmin,kmax);
   TH1D * hrho= new TH1D("hrho","Beamspot #rho_{xy} (z=0); #rho_{xy}; events",100,rhomin,rhomax);

   TH1D * hxc = new TH1D("hxc","Beamspot x-position at centroid; x [mm]; events",100,xcmin,xcmax);
   TH1D * hyc = new TH1D("hyc","Beamspot y-position at centroid; y [mm]; events",100,ycmin,ycmax);

   TH1D * hntrk = new TH1D("hntrk","Num vrts. for Beamspot determination; Num. Vertices; events",100,ntrkmin,ntrkmax);


   TGraphErrors * grxc = new TGraphErrors(nentries);
   TGraphErrors * grsxc = new TGraphErrors(nentries);
   TGraphErrors * grsxcEh = new TGraphErrors(nentries);
   TGraphErrors * grsxcEl = new TGraphErrors(nentries);
   grxc->SetLineColor(kRed-0);
   grsxc->SetLineColor(kBlue-1);
   grsxcEh->SetLineColor(kOrange-2);
   grsxcEl->SetLineColor(kOrange-3);

   TGraphErrors * gryc = new TGraphErrors(nentries);
   TGraphErrors * grsyc = new TGraphErrors(nentries);
   TGraphErrors * grsycEh = new TGraphErrors(nentries);
   TGraphErrors * grsycEl = new TGraphErrors(nentries);
   gryc->SetLineColor(kRed-0);
   grsyc->SetLineColor(kBlue-1);
   grsycEh->SetLineColor(kOrange-2);
   grsycEl->SetLineColor(kOrange-3);

   TGraphErrors * grz = new TGraphErrors(nentries);
   TGraphErrors * grsz = new TGraphErrors(nentries);
   TGraphErrors * grszEh = new TGraphErrors(nentries);
   TGraphErrors * grszEl = new TGraphErrors(nentries);
   grz->SetLineColor(kRed-0);
   grsz->SetLineColor(kBlue-1);
   grszEh->SetLineColor(kOrange-2);
   grszEl->SetLineColor(kOrange-3);

   TGraphErrors * grax = new TGraphErrors(nentries);
   TGraphErrors * gray = new TGraphErrors(nentries);
   TGraphErrors * grk  = new TGraphErrors(nentries);
   TGraphErrors * grrho= new TGraphErrors(nentries);

   grax->SetLineColor(kRed-0);
   gray->SetLineColor(kRed-0);
   grk->SetLineColor(kOrange-3);
   grrho->SetLineColor(kCyan-3);

   
   // second loop to fill the histograms
   nbytes = 0;
   nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = t.LoadTree(jentry);
      if (ientry < 0) break;
      nb = t.fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      
      // create ID
      //      ID id(t.event_run, t.event_id);
      ID id(t.event_run, t.event_id, t.event_lumiStart, t.event_lumiRange );

      // find the id position
      /*
	std::set<ID>::iterator it = _idmap.find(id);
	int position = (int)(it - _idmap.begin());
      */
      std::set<ID>::iterator it = _idmap.begin();
      int position(0);
      while (it != _idmap.end() ) {
	if ( *it == id) break;
	++it;
	++position;
      }
      

      hx0->Fill(t.bs_x0);
      hy0->Fill(t.bs_y0);
      hxc->Fill(t.bsCentroid_xc);
      hyc->Fill(t.bsCentroid_yc);
      hz->Fill(t.bs_z);
      hax->Fill(t.bs_ax);
      hay->Fill(t.bs_ay);
      hsx->Fill(t.bs_sx);
      hsy->Fill(t.bs_sy);
      hsz->Fill(t.bs_sz);
      hk->Fill(t.bs_k);
      hrho->Fill(t.bs_rhoxy);
      hntrk->Fill(t.event_nEvents);

      double lbMid = 0.5*(id.lumiStart() + id.lumiEnd() -1 );
      double lbX   = 0.5*(id.lumiEnd() - id.lumiStart());
      cout << position << "  " << lbMid << "  " << lbX << endl;
      
      // graph loops
      grxc->SetPoint(position,lbMid, t.bsCentroid_xc);
      grxc->SetPointError(position,lbX,sqrt(t.bsCovCentroid_xcxc));
      grsxc->SetPoint(position,lbMid, t.bsCentroid_xc);
      grsxc->SetPointError(position,0,t.bsCentroid_sxc);
      grsxcEh->SetPoint(position,lbMid, t.bsCentroid_xc+t.bsCentroid_sxc);
      grsxcEh->SetPointError(position,0.,sqrt(t.bsCovCentroid_sxcsxc));
      grsxcEl->SetPoint(position,lbMid, t.bsCentroid_xc-t.bsCentroid_sxc);
      grsxcEl->SetPointError(position,0.,sqrt(t.bsCovCentroid_sxcsxc));
      gryc->SetPoint(position,lbMid, t.bsCentroid_yc);
      gryc->SetPointError(position,lbX,sqrt(t.bsCovCentroid_ycyc));
      grsyc->SetPoint(position,lbMid, t.bsCentroid_yc);
      grsyc->SetPointError(position,0,t.bsCentroid_syc);
      grsycEh->SetPoint(position,lbMid, t.bsCentroid_yc+t.bsCentroid_syc);
      grsycEh->SetPointError(position,0.,sqrt(t.bsCovCentroid_sycsyc));
      grsycEl->SetPoint(position,lbMid, t.bsCentroid_yc-t.bsCentroid_syc);
      grsycEl->SetPointError(position,0.,sqrt(t.bsCovCentroid_sycsyc));

      grz->SetPoint(position,lbMid, t.bs_z);
      grz->SetPointError(position,lbX, sqrt(t.bsCov_zz));
      grsz->SetPoint(position,lbMid, t.bs_z);
      grsz->SetPointError(position,0, t.bs_sz);
      grszEh->SetPoint(position,lbMid, t.bs_z+t.bs_sz);
      grszEh->SetPointError(position,0.,sqrt(t.bsCov_szsz));
      grszEl->SetPoint(position,lbMid, t.bs_z-t.bs_sz);
      grszEl->SetPointError(position,0.,sqrt(t.bsCov_szsz));

      grax->SetPoint(position,lbMid, t.bs_ax);
      grax->SetPointError(position,lbX, sqrt(t.bsCov_axax));
      gray->SetPoint(position,lbMid, t.bs_ay);
      gray->SetPointError(position,lbX, sqrt(t.bsCov_ayay));
      grk->SetPoint(position,lbMid, t.bs_k);
      grk->SetPointError(position,lbX, sqrt(t.bsCov_kk));
      grrho->SetPoint(position,lbMid, t.bs_rhoxy);
      grrho->SetPointError(position,lbX, sqrt(t.bsCov_rhoxyrhoxy));



   } // for loop

   // write plots
   simpleWrite(hx0);
   simpleWrite(hy0);
   simpleWrite(hz);
   simpleWrite(hxc);
   simpleWrite(hyc);
   simpleWrite(hax);
   simpleWrite(hay);
   simpleWrite(hsx);
   simpleWrite(hsy);
   simpleWrite(hsz);
   simpleWrite(hax);
   simpleWrite(hay);
   simpleWrite(hk);
   simpleWrite(hrho);
   simpleWrite(hntrk);

   TCanvas * ctest = new TCanvas("ctest");
   ctest->cd();
   ctest->Divide(1,3);
   ctest->cd(1);
   grsxc->SetTitle("Centroid Beamspot x-position and uncertainties; lb;x-pos [mm]");
   grsxc->Draw("ap");
   grsxcEh->Draw("p");
   grsxcEl->Draw("p");
   grxc->Draw("p");
   ctest->cd(2);
   grsyc->SetTitle("Centroid Beamspot y-position and uncertainties; lb;y-pos [mm]");
   grsyc->Draw("ap");
   grsycEh->Draw("p");
   grsycEl->Draw("p");
   gryc->Draw("p");
   ctest->cd(3);
   grsz->SetTitle(" Beamspot z-position and uncertainties; lb;z-pos [mm]");
   grsz->Draw("ap");
   grszEh->Draw("p");
   grszEl->Draw("p");
   grz->Draw("p");
   ctest->SaveAs((outputPlotDir+"timePos.eps").c_str());
   ctest->SaveAs((outputPlotDir+"timePos.png").c_str());

   ctest = new TCanvas("ctest2");
   ctest->Divide(1,4);
   ctest->cd(1);
   grax->SetTitle("Beamspot x-slope;lb;x-slope");
   grax->Draw("ap");
   ctest->cd(2);
   gray->SetTitle("Beamspot y-slope;lb;x-slope");
   gray->Draw("ap");
   ctest->cd(3);
   grk->Draw("ap");
   ctest->cd(4);
   grrho->Draw("Beamspot correlation coef. #rho_{xy};lb;#rho_{xy}");
   grrho->Draw("ap");
   ctest->SaveAs((outputPlotDir+"timePar.eps").c_str());
   ctest->SaveAs((outputPlotDir+"timePar.png").c_str());

} // standardBsOnlyPlots

void standardVrtOnlyPlots(TTree * vrt) {
  if (!vrt) return;
  cout << "Making standard Vertex plots" << endl;
 // start loop over trees
  vrtTree t(vrt);
  //  vrtTree t(0);
   if (t.fChain == 0) return;
   std::map<ID,VrtH> _idmap;

   Long64_t nentries = t.fChain->GetEntriesFast();
   
   double xmin(10.),xmax(-10.), ymin(10.), ymax(-10.), zmin(400.), zmax(-400.);

   // first loop to get ranges
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = t.LoadTree(jentry);
      if (ientry < 0) break;
      nb = t.fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      // currently only include 'passed' vertices, although outliers are still included
      if ( t.qual_passed == 0) continue;

      // create ID
      //      ID id(t.event_run, t.id);
      ID id(t.event_run, t.id);

      _idmap.insert(make_pair(id,VrtH()));

      setMinMax(t.vrt_x,xmin,xmax);
      setMinMax(t.vrt_y,ymin,ymax);
      setMinMax(t.vrt_z,zmin,zmax);


   } // for loop
   //scale values
   double k =0.08;
   scaleMinMax(k,xmin,xmax);
   scaleMinMax(k,ymin,ymax);
   scaleMinMax(k,zmin,zmax);
  
   // number of sets of vertices
   int nR = _idmap.size();
  
   for (std::map<ID,VrtH>::iterator it= _idmap.begin(); it != _idmap.end(); ++it) {
     // create 
     VrtH & h = it->second;
     h.hx = new TH1D(Form("hvx_%i_%i",it->first.run(),it->first.lumiStart()),
		     Form("Vertex x-position for run: %i, lb: [%i,%i]; x-pos [mm]; events",
			  it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),
		     400,xmin,xmax);
     h.hy = new TH1D(Form("hvy_%i_%i",it->first.run(),it->first.lumiStart()),
		     Form("Vertex y-position for run: %i, lb: [%i,%i]; y-pos [mm]; events",
			  it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),
		     400,ymin,ymax);
     h.hz = new TH1D(Form("hvz_%i_%i",it->first.run(),it->first.lumiStart()),
		     Form("Vertex z-position for run: %i, lb: [%i,%i]; z-pos [mm]; events",
			  it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),
		     400,zmin,zmax);


     h.vrtxvsz = new TProfile(Form("hp_vrtXvsZ_%i_%i",it->first.run(),it->first.lumiStart()),
			      Form("x-pos vs z for run: %i, lb: [%i,%i]; z [mm]; x [mm]",
				   it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1), 
			      50, zmin, zmax, xmin, xmax);
     h.vrtyvsz = new TProfile(Form("hp_vrtYvsZ_%i_%i",it->first.run(),it->first.lumiStart()),
			      Form("y-pos vs z for run: %i, lb: [%i,%i]; z [mm]; y [mm]",
				   it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1), 
			      50, zmin, zmax, ymin, ymax);
     h.vrtxvsz->SetMinimum(xmin);
     h.vrtyvsz->SetMinimum(ymin);

     h.hxbs = new TH1D(Form("hvxbs_%i_%i",it->first.run(),it->first.lumiStart()),
		       Form("Beamspot-corrected Vertex x-position for run: %i, lb: [%i,%i]; x-pos [mm]; events",
			    it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),
		       400,0.5*(xmin-xmax),0.5*(xmax-xmin));
     h.hybs = new TH1D(Form("hvybs_%i_%i",it->first.run(),it->first.lumiStart()),
		       Form("Beamspot-corrected Vertex y-position for run: %i, lb: [%i,%i]; y-pos [mm]; events",
			    it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),
		       400,0.5*(ymin-ymax),0.5*(ymax-ymin));
     h.hzbs = new TH1D(Form("hvzbs_%i_%i",it->first.run(),it->first.lumiStart()),
		       Form("Beamspot-corrected Vertex z-position for run: %i, lb: [%i,%i]; z-pos [mm]; events",
			    it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),
		       400,0.5*(zmin-zmax),0.5*(zmax-zmin));

     h.hpullx  = new TH1D(Form("hpullx_%i_%i",it->first.run(),it->first.lumiStart())
			  ,Form("Beamspot pull(x-position) for run: %i, lb: [%i,%i]; pull ; events",
				it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),100,-5,5);
     h.hpully  = new TH1D(Form("hpully_%i_%i",it->first.run(),it->first.lumiStart())
			  ,Form("Beamspot pull(y-position) for run: %i, lb: [%i,%i]; pull ; events",
				it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),100,-5,5);
     h.hpullz  = new TH1D(Form("hpullz_%i_%i",it->first.run(),it->first.lumiStart())
			  ,Form("Beamspot pull(z-position) for run: %i, lb: [%i,%i]; pull ; events",
				it->first.run(),it->first.lumiStart(),it->first.lumiEnd()-1),100,-5,5);

   }// making hists
   
   TGraphErrors * grx = new TGraphErrors(nentries);
   TGraphErrors * grsx = new TGraphErrors(nentries);
   grx->SetLineColor(kRed-0);
   grsx->SetLineColor(kBlue-1);
   grsx->SetMinimum(xmin);
   TGraphErrors * gry = new TGraphErrors(nentries);
   TGraphErrors * grsy = new TGraphErrors(nentries);
   gry->SetLineColor(kRed-0);
   grsy->SetLineColor(kBlue-1);
   grsy->SetMinimum(ymin);
   TGraphErrors * grz = new TGraphErrors(nentries);
   TGraphErrors * grsz = new TGraphErrors(nentries);
   grz->SetLineColor(kRed-0);
   grsz->SetLineColor(kBlue-1);
   grsz->SetMinimum(zmin);

   TH1D * hpullx  = new TH1D("hpullx" ,"Beamspot pull(x-position); pull ; events",100,-5,5);
   TH1D * hpully  = new TH1D("hpully" ,"Beamspot pull(y-position); pull ; events",100,-5,5);
   TH1D * hpullz  = new TH1D("hpullz" ,"Beamspot pull(z-position); pull ; events",100,-5,5);

   nbytes = 0; nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = t.LoadTree(jentry);
      if (ientry < 0) break;
      nb = t.fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      // currently on include 'passed' vertices, althogh outliers are still included
      if ( t.qual_passed == 0) continue;
      
      // create ID
      //ID id(t.event_run, t.id);
      ID id(t.event_run, t.id );

      VrtH &h  = _idmap.find(id)->second;

      h.hx->Fill(t.vrt_x);
      h.hy->Fill(t.vrt_y);
      h.hz->Fill(t.vrt_z);
      
      h.vrtxvsz->Fill(t.vrt_z,t.vrt_x);
      h.vrtyvsz->Fill(t.vrt_z,t.vrt_y);
	  
      // If beamspots have been done, run the combined 
      if ( _beamspotMap.size() ) {
	std::map<ID,BsH>::iterator bit =  _beamspotMap.find(id);
	if (bit == _beamspotMap.end()) {
	  cout << "Error finding beamspot " << id << " for combined plots" << endl;
	} else {
	  const BsH & b = bit->second;
	  double vx = t.vrt_x - ( b.x0 + b.ax*t.vrt_z);
	  double vy = t.vrt_y - ( b.y0 + b.ay*t.vrt_z);
	  double vz = t.vrt_z - b.z;

	  h.hxbs->Fill(vx);
	  h.hybs->Fill(vy);
	  h.hzbs->Fill(vz);

	  // not 100% correct, but close enough
	  hpullx->Fill(vx/sqrt( fabs(t.vrt_vxx + b.sx*b.sx) ));
	  hpully->Fill(vy/sqrt( fabs(t.vrt_vyy + b.sy*b.sy) ));
	  hpullz->Fill(vz/sqrt( fabs(t.vrt_vzz + b.sz*b.sz) ));
	  h.hpullx->Fill(vx/sqrt( fabs(t.vrt_vxx + b.sx*b.sx) ));
	  h.hpully->Fill(vy/sqrt( fabs(t.vrt_vyy + b.sy*b.sy) ));
	  h.hpullz->Fill(vz/sqrt( fabs(t.vrt_vzz + b.sz*b.sz) ));


	}// if beamspot found
      }// if available beamspots

   } // for loop

   // now loop over the ids
   int position(0);
   for (std::map<ID,VrtH>::iterator it= _idmap.begin(); it != _idmap.end(); ++it,++position) {
     // create 
     VrtH & h = it->second;
     const ID & id = it->first;
     /*
     grx->SetPoint(position,id.lumiStart(),h.hx->GetMean() );
     grx->SetPointError(position,0,h.hx->GetMeanError());
     grsx->SetPoint(position,id.lumiStart(),h.hx->GetMean() );
     grsx->SetPointError(position,0,h.hx->GetRMS());

     gry->SetPoint(position,id.lumiStart(),h.hy->GetMean() );
     gry->SetPointError(position,0,h.hy->GetMeanError());
     grsy->SetPoint(position,id.lumiStart(),h.hy->GetMean() );
     grsy->SetPointError(position,0,h.hy->GetRMS());

     grz->SetPoint(position,id.lumiStart(),h.hz->GetMean() );
     grz->SetPointError(position,0,h.hz->GetMeanError());
     grsz->SetPoint(position,id.lumiStart(),h.hz->GetMean() );
     grsz->SetPointError(position,0,h.hz->GetRMS());
     
     grx->SetPoint(position,id.lumiStart(),h.hx->GetMean() );
     grx->SetPointError(position,0,h.hx->GetMeanError());
     grsx->SetPoint(position,id.lumiStart(),h.hx->GetMean() );
     grsx->SetPointError(position,0,h.hx->GetRMS());
     */
     // double lbMid = 0.5*(id.lumiStart() + id.lumiEnd() -1 );
     //double lbX   = id.lumiEnd() -1 - id.lumiStart();
     //cout << position << "  " << lbMid << "  " << lbX << endl;
     double lbMid = id.id();
     double lbX   = 0.;

     if ( _beamspotMap.find(id) != _beamspotMap.end()) {
       const ID & id2 = _beamspotMap.find(id)->first;
       lbMid =  0.5*(id2.lumiStart() + id2.lumiEnd() -1 );
       lbX   = 0.5*(id2.lumiEnd()- id2.lumiStart());
       cout << "XX:" << position << "  " << lbMid << "  " << lbX << endl;
     }

     grx->SetPoint(position,lbMid,h.hx->GetMean() );
     grx->SetPointError(position,lbX,h.hx->GetMeanError());
     grsx->SetPoint(position,lbMid,h.hx->GetMean() );
     grsx->SetPointError(position,0,h.hx->GetRMS());
     
     gry->SetPoint(position,lbMid,h.hy->GetMean() );
     gry->SetPointError(position,lbX,h.hy->GetMeanError());
     grsy->SetPoint(position,lbMid,h.hy->GetMean() );
     grsy->SetPointError(position,0,h.hy->GetRMS());

     grz->SetPoint(position,lbMid,h.hz->GetMean() );
     grz->SetPointError(position,lbX,h.hz->GetMeanError());
     grsz->SetPoint(position,lbMid,h.hz->GetMean() );
     grsz->SetPointError(position,0,h.hz->GetRMS());
   // If beamspots have been done, run the combined 
 
   } // loop over final hists

   TCanvas * ctest = new TCanvas("cPo");
   ctest->Divide(1,3);
   ctest->cd(1);
   grsx->SetTitle("Vertex Mean x-position and RMS; lb; x-pos [mm]");
   grsx->Draw("ap");
   grx->Draw("p");
   ctest->cd(2);
   grsx->SetTitle("Vertex Mean y-position and RMS; lb; y-pos [mm]");
   grsy->Draw("ap");
   gry->Draw("p");
   ctest->cd(3);
   grsy->SetTitle("Vertex Mean z-position and RMS; lb; z-pos [mm]");
   grsz->Draw("ap");
   grz->Draw("p");
   
   ctest->SaveAs((outputPlotDir + "VrtTimePos.eps").c_str());
   ctest->SaveAs((outputPlotDir + "VrtTimePos.png").c_str());


   simpleWrite(hpullx);
   simpleWrite(hpully);
   simpleWrite(hpullz);


   //writing
   ctest = new TCanvas("cM");
   for (std::map<ID,VrtH>::iterator it= _idmap.begin(); it != _idmap.end(); ++it) {
     VrtH & h = it->second;
     simpleWrite(h.hx);
     simpleWrite(h.hy);
     simpleWrite(h.hz);
     simpleWrite(h.hxbs);
     simpleWrite(h.hybs);
     simpleWrite(h.hzbs);
     simpleWrite(h.hpullx);
     simpleWrite(h.hpully);
     simpleWrite(h.hpullz);
   //     simpleWrite(h.vrtxvsz);
     //     simpleWrite(h.vrtyvsz);
     const BsH * bs(0); 
     if ( _beamspotMap.size() ) {
       std::map<ID,BsH>::iterator bit =  _beamspotMap.find(it->first);
       if (bit == _beamspotMap.end()) {
	 cout << "Error finding beamspot for final combined plots" << endl;
       } else {
	 const BsH & b = bit->second;
	 bs = &b;
       }// if beamspot found
     }// if available beamspots
     

     ctest->cd();
     h.vrtxvsz->Draw();
     h.vrtxvsz->SetLineColor(kRed);
     if (bs) {
       TLine *lax = new TLine( -200,  bs->x0+ bs->ax*-200, 200,bs->x0+ bs->ax*200);
       lax->SetLineWidth(3); lax->SetLineColor(kBlue);  lax->Draw();
     }
     ctest->SaveAs( (outputPlotDir+ h.vrtxvsz->GetName() +".eps").c_str());
     ctest->SaveAs( (outputPlotDir+ h.vrtxvsz->GetName() +".png").c_str());
     ctest->cd();
     h.vrtyvsz->Draw();
     h.vrtyvsz->SetLineColor(kRed);
     if (bs) {
       TLine *lay = new TLine( -200,  bs->y0+ bs->ay*-200, 200,bs->y0+ bs->ay*200);
       lay->SetLineWidth(3); lay->SetLineColor(kBlue);  lay->Draw();
     }
     ctest->SaveAs( (outputPlotDir+ h.vrtyvsz->GetName() +".eps").c_str());
     ctest->SaveAs( (outputPlotDir+ h.vrtyvsz->GetName() +".png").c_str());
     
   }



} // vrts

