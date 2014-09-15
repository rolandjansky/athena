/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// cscperf.C

#include <iomanip>

cscperf_help() {
  cout << endl;
  cout << "CSC performance macros:" << endl;
  cout << endl;
  cout << "  cluperf() - returns cscperf tree" << endl;
  cout << endl;
  cout << "  effs() - performance summary" << endl;
  cout << "  reseff(measphi) - integrated residual efficiency" << endl;
  cout << "  sigeff(measphi, draw=\"spoiled\") - integrated pull efficiency" << endl;
  cout << "  rmssig(measphi) - charge RMS vs pull" << endl;
  cout << "  resplot(measphi, lim, fit) - resolution" << endl;
  cout << "  sigplot(measphi, lim, fit) - pull" << endl;
  cout << endl;
  cout << "  rstrippos(Double_t pos, Double_t pitch, Double_t qrat=0.0)" << endl;
  cout << endl;
  cout << "  resfit(istation, measphi, strip, fit=true, xmin=0, nterm=4) - scor for r-plane" << endl;
  cout << "    - obselete fit of position correction vs. strip position" << endl;
  cout << "  qratfit(istation, measphi, wc, fit =true, nterm =4, xmin=0.0, xmax=1.0)" << endl;
  cout << "    - obselete polynomial fit of position correction vs charge ratio" << endl;
  cout << "  qrat_table_fit(Int_t istation, Bool_t measphi =false, Int_t nbin=100, Double_t nrms)" << endl;
  cout << "    - fit giving charge ratio lookup table for position correction" << endl;
  cout << endl;
  cout << "  cscperf_help() - show this message" << endl;
  cout << endl;
}

//**********************************************************************

// Return the CSC cluster performance tree.

TTree* cluperf(Int_t verbose =0) {
  return find_tree_openfile("csc_perf", verbose);
}

//**********************************************************************

// Return the cluster performance tree reader.

class CluperfReader;
CluperfReader* cluperf_reader() {
  static Bool_t first = true;
  if ( cluperf() == 0 ) return 0;
  if ( first ) {
    cluperf()->MakeClass("CluperfReader");
    gROOT->ProcessLine(".L CluperfReader.C");
    first = false;
  }
  pout = new CluperfReader(cluperf());
  return pout;
}


//**********************************************************************

// Chamber strip limits: 0.5*pitch*nstrip
Double_t maxpos(Int_t istation, Bool_t measphi) {
  if ( istation == 1 ) {
    if ( measphi ) return 310.1;
    return 533.8;
  } else if ( istation == 2 ) {
    if ( measphi ) return 504.0;
    return 509.8;
  }
  return 0.0;
}

// Return if a sim hit is within the above boundaries.
Bool_t siminz(Int_t istation, Double_t zh) {
  return fabs(zh) < maxpos(istation, false)-0.1;
}
Bool_t siminy(Int_t istation, Double_t yh) {
  return fabs(yh) < maxpos(istation, true)-0.1;
}
Bool_t simin(Int_t istation, Double_t yh, Double_t zh) {
  return siminy(istation, yh) && siminz(istation, zh);
}

// Aceptance cuts.
TCut cluperf_cut_acc_eta = "siminz(istation, zh)";
TCut cluperf_cut_acc_phi = "siminy(istation, yh)";
TCut cluperf_cut_acc =     "simin(istation, yh, zh)";
  
// Kinematic cut for simulated muon.
TCut cluperf_cut_kine = "pt>2000";

// Acceptance + kinematics.
TCut cluperf_cut_sim_eta = cluperf_cut_kine + cluperf_cut_acc_eta;
TCut cluperf_cut_sim_phi = cluperf_cut_kine + cluperf_cut_acc_phi;
TCut cluperf_cut_sim     = cluperf_cut_kine + cluperf_cut_acc;

// Cut to flag found.
TCut cluperf_cut_found_eta = "wzc>0";
TCut cluperf_cut_found_phi = "wyc>0";

// Cut to flag lost.
TCut cluperf_cut_lost_eta = "wzc==0";
TCut cluperf_cut_lost_phi = "wyc==0";

// Cuts to flag spoiled and unspoiled found clusters.
TCut cluperf_cut_spoiled_eta   = "wzc>0 && (szc!=0 && szc!=10)";
TCut cluperf_cut_unspoiled_eta = "wzc>0 && (szc==0 || szc==10)";
TCut cluperf_cut_spoiled_phi   = "wyc>0 && (syc!=0 && syc!=10)";
TCut cluperf_cut_unspoiled_phi = "wyc>0 && (syc==0 || syc==10)";

// Cut to flag found good or bad (5 sigma).
TCut cluperf_cut_good_eta = "wzc>0 && abs(zc-zh)/dzc<=5.0";
TCut cluperf_cut_good_phi = "wyc>0 && abs(yc-yh)/dyc<=5.0";
TCut cluperf_cut_bad_eta = "wzc>0 && abs(zc-zh)/dzc>5.0";
TCut cluperf_cut_bad_phi = "wyc>0 && abs(yc-yh)/dyc>5.0";

//**********************************************************************

// Scans.

void scan_lost() {
  cluperf()->Scan("run:evt:istation:zsec:phisec:wlay:dzc:wzc",
                  cluperf_cut_acc + cluperf_cut_lost_eta);
}

void scan_spoiled() {
  cluperf()->Scan("run:evt:istation:zsec:phisec:wlay:dzc:wzc",
                  cluperf_cut_acc + cluperf_cut_spoiled_eta);
}

void scan_misreco() {
  cluperf()->Scan("run:evt:istation:zsec:phisec:wlay:dzc:wzc:zh-zc:(zc-zh)/dzc",
                  cluperf_cut_acc + cluperf_cut_bad_eta);
}

//**********************************************************************

// Show or fit residual for istation-measphi clusters with width wc as
// a function of strip coordinate over the range (xmin, 1-xmin).
// Fit is an odd polynomial with nterm terms.

void resfit(Int_t istation, Int_t measphi, Int_t wc, Bool_t fit =true, Double_t xmin =0, Int_t nterm =4) {
  string var = "z";
  if ( measphi ) var = "y";
  ostringstream ssxvar;
  // X-variable
  ssxvar << "fmod(100.";
  if ( 2*(wc/2) == wc ) ssxvar << "5";
  else ssxvar << "0";
  ssxvar << "+" << var << "c/p" << var << "c,1)";
  string sxvar = ssxvar.str();
  // Y-variable.
  ostringstream ssyvar;
  ssyvar << "(" << var << "h-" << var << "c)/p" << var << "c";
  string syvar = ssyvar.str();
  // Plot variables.
  ostringstream ssyxvar;
  ssyxvar << syvar << ":" << sxvar;
  string syxvar = ssyxvar.str();
  // Cut.
  ostringstream sscut;
  sscut << "simin(istation,yh,zh) && d" << var << "c>0 && d" << var << "c<1.0";
  string sstation = "ALL";
  if ( istation ) {
    sscut << " && istation==" << istation;
    if ( istation == 1 ) {
      sstation = "CSS";
    } else if ( istation == 2 ) {
      sstation = "CSL";
    } else {
      cout << "Invalid station index: " << istation << endl;
      return;
    }
  }
  sscut << " && w" << var << "c==" << wc;
  string scut = sscut.str();
  // Display evaluated input.
  cout << "Station: " << sstation << endl;
  cout << "Cluster width: " << wc << endl;
  cout << "Plot: " << syxvar << endl;
  cout << "Cut: " << scut << endl;
  // Extract graph.
  cout << "*** Extract graph." << endl;
  cluperf()->Draw(syxvar.c_str(), scut.c_str());
  gDirectory->Delete("gres");
  TGraph* pgraw = new TGraph(*(TGraph*)gPad->GetPrimitive("Graph"));
  pgraw->SetName("gfitres");
  ostringstream sstitle;
  sstitle << "Strip correction for " << sstation;
  if ( measphi ) sstitle << " phi";
  else sstitle << " r";
  sstitle << ", cluster width " << wc;
  pgraw->SetTitle(sstitle.str().c_str());
  gPad->Clear();
  pgraw->Draw("AP");
  gPad->Flush();
  if ( ! fit ) {
    return;
  }
  // Evaluate the fit range.
  Double_t xmax = 1.0 - xmin;
  // Create the fit function.
  cout << "*** Create fit function." << endl;
  ostringstream ssfun;
  for ( Int_t iterm=0; iterm<nterm; ++iterm ) {
    if ( iterm != 0 ) ssfun << " + ";
    ssfun << "[" << iterm << "]";
    for ( Int_t ipow=0; ipow<2*iterm+1; ++ipow ) {
      ssfun << "*(x-0.5)";
    }
  }
  string sfun = ssfun.str();
  gDirectory->Delete("fit_resfit");
  TF1* pfun = new TF1("fit_resfit", sfun.c_str(), xmin, xmax);
  for ( Int_t iterm=0; iterm<nterm; ++iterm ) {
    pfun->SetParameter(iterm, 0.0);
    pfun->SetParError(iterm, 0.1);
  }
  cout << "Fun: " << sfun << endl;
  Int_t oldcount = 0;
  // Loop over fits.
  Int_t maxfit = 20;
  TGraph* pgfit = 0;
  sstitle << " (selected points)";
  for ( Int_t ifit=0; ifit<maxfit; ++ifit ) {
    Double_t dymax = 0.02;
    if ( ifit == 0 ) dymax = 1.0;
    if ( ifit == 1 ) dymax = 0.2;
    // Select points for fitting: those within 0.10 + 5*sigma.
    gDirectory->Delete("fit_resfit_sel");
    vector<Double_t> newx;
    vector<Double_t> newy;
    cout << "*** Selecting." << endl;
    for ( Int_t i=0; i<pgraw->GetN(); ++i ) {
      Double_t xg = pgraw->GetX()[i];
      Double_t yg = pgraw->GetY()[i];
      Double_t yf = pfun->Eval(xg);
      if ( xg>xmin && xg<xmax && fabs(yg - yf) < dymax ) {
        newx.push_back(xg);
        newy.push_back(yg);
      }
    }
    pgfit = new TGraph(newx.size(), &newx.front();, &newy.front());
    pgfit->SetTitle(sstitle.str().c_str());
    Int_t newcount = pgfit->GetN();
    // Fit.
    cout << "*** Fitting " << pgfit->GetN() << " of " << pgraw->GetN() << " points" << endl;
    pgfit->Fit("fit_resfit", "R M");
    // If select count has not changed exit.
    if ( ifit > 1 && newcount == oldcount ) break;
    oldcount = newcount;
  }
  gStyle->SetOptFit(1);
  pgfit->Draw("P");
  new TCanvas();
  pgfit->Draw("AP");
  for ( Int_t iterm=0; iterm<nterm; ++iterm ) {
    cout << iterm << " " << pfun->GetParameter(iterm) << endl;
  }
}

//**********************************************************************

void qratfit(Int_t istation, Int_t measphi, Int_t wc, Bool_t fit =true, Int_t nterm =4,
             Double_t xmin=0.0, Double_t xmax=1.0) {
  string var = "z";
  if ( measphi ) var = "y";
  ostringstream ssxvar;
  // X-variable
  //ssxvar << "fmod(100.0+" << var << "c/p" << var << "c,1)";
  ssxvar << "qr" << var << "/qp" << var;
  string sxvar = ssxvar.str();
  // Y-variable.
  ostringstream ssyvar;
  ssyvar << var << "h/p" << var << "c-(o" << var << "c+m" << var << "c-n" << var << "c/2-0.5)";
  //ssyvar << var << "h-" << var << "c";
  string syvar = ssyvar.str();
  // Plot variables.
  ostringstream ssyxvar;
  ssyxvar << syvar << ":" << sxvar;
  string syxvar = ssyxvar.str();
  // Cut.
  ostringstream sscut;
  sscut << "simin(istation,yh,zh) && d" << var << "c>0 && d" << var << "c<1.0";
  sscut << " && m" << var << "c<w" << var << "c-1";
  string sstation = "ALL";
  if ( istation ) {
    sscut << " && istation==" << istation;
    if ( istation == 1 ) {
      sstation = "CSS";
    } else if ( istation == 2 ) {
      sstation = "CSL";
    } else {
      cout << "Invalid station index: " << istation << endl;
      return;
    }
  }
  if ( wc ) {
    sscut << " && w" << var << "c==" << wc;
  } else {
    sscut << " && w" << var << "c>2";
  }
  string scut = sscut.str();
  // Display evaluated input.
  cout << "Station: " << sstation << endl;
  ostringstream sswc;
  if ( wc ) sswc << wc;
  else sswc << ">2";
  string swc = sswc.str();
  cout << "Cluster width: " << swc << endl;
  cout << "Plot: " << syxvar << endl;
  cout << "Cut: " << scut << endl;
  // Extract graph.
  cout << "*** Extract graph." << endl;
  cluperf()->Draw(syxvar.c_str(), scut.c_str());
  gDirectory->Delete("gres");
  TGraph* pgraw = new TGraph(*(TGraph*)gPad->GetPrimitive("Graph"));
  pgraw->SetName("gfitres");
  ostringstream sstitle;
  sstitle << "Qrat correction for " << sstation;
  if ( measphi ) sstitle << " phi";
  else sstitle << " r";
  sstitle << ", cluster width " << swc;
  pgraw->SetTitle(sstitle.str().c_str());
  gPad->Clear();
  pgraw->Draw("AP");
  gPad->Flush();
  if ( ! fit ) {
    return;
  }
  // Create the fit function.
  cout << "*** Create fit function." << endl;
  ostringstream ssfun;
  for ( Int_t iterm=0; iterm<nterm; ++iterm ) {
    if ( iterm != 0 ) ssfun << " + ";
    ssfun << "[" << iterm << "]";
    for ( Int_t ipow=0; ipow<iterm; ++ipow ) {
      ssfun << "*x";
    }
  }
  string sfun = ssfun.str();
  gDirectory->Delete("fit_resfit");
  TF1* pfun = new TF1("fit_resfit", sfun.c_str(), xmin, xmax);
  for ( Int_t iterm=0; iterm<nterm; ++iterm ) {
    pfun->SetParameter(iterm, 0.0);
    pfun->SetParError(iterm, 0.1);
  }
  cout << "Fun: " << sfun << endl;
  Int_t oldcount = 0;
  // Loop over fits.
  Int_t maxfit = 5;
  TGraph* pgfit = 0;
  sstitle << " (selected points)";
  for ( Int_t ifit=0; ifit<maxfit; ++ifit ) {
    Double_t dymax = 0.2;
    if ( ifit == 0 ) dymax = 5.0;
    if ( ifit == 1 ) dymax = 1.0;
    gDirectory->Delete("fit_resfit_sel");
    vector<Double_t> newx;
    vector<Double_t> newy;
    cout << "*** Selecting." << endl;
    for ( Int_t i=0; i<pgraw->GetN(); ++i ) {
      Double_t xg = pgraw->GetX()[i];
      Double_t yg = pgraw->GetY()[i];
      Double_t yf = pfun->Eval(xg);
      if ( xg>xmin && xg<xmax && fabs(yg - yf) < dymax ) {
        newx.push_back(xg);
        newy.push_back(yg);
      }
    }
    if ( newx.size() < nterm ) {
      cout << "Only " << newx.size() << " points remin--Giving up" << endl;
    }
    pgfit = new TGraph(newx.size(), &newx.front(), &newy.front());
    pgfit->SetTitle(sstitle.str().c_str());
    Int_t newcount = pgfit->GetN();
    // Fit.
    cout << "*** Fitting " << pgfit->GetN() << " of " << pgraw->GetN() << " points" << endl;
    pgfit->Fit("fit_resfit", "R M");
    // If select count has not changed exit.
    if ( ifit > 1 && newcount == oldcount ) break;
    oldcount = newcount;
  }
  gStyle->SetOptFit(1);
  pgfit->Draw("P");
  new TCanvas();
  pgfit->Draw("AP");
  for ( Int_t iterm=0; iterm<nterm; ++iterm ) {
    cout << iterm << " " << pfun->GetParameter(iterm) << endl;
  }
}

//**********************************************************************

// Draw qrat left vs qrat right.

void qrat2(Int_t istation, Int_t measphi=0, Int_t wc=0) {
  string var = "z";
  if ( measphi ) var = "y";
  // X-variable
  ostringstream ssxvar;
  ssxvar << "qr" << var << "/qp" << var;
  string sxvar = ssxvar.str();
  // Y-variable.
  ostringstream ssyvar;
  ssyvar << "ql" << var << "/qp" << var;
  string syvar = ssyvar.str();
  // Plot variables.
  ostringstream ssyxvar;
  ssyxvar << syvar << ":" << sxvar;
  string syxvar = ssyxvar.str();
  // Cut.
  ostringstream sscut;
  sscut << "simin(istation,yh,zh) && d" << var << "c>0 && d" << var << "c<1.0";
  string sstation = "ALL";
  if ( istation ) {
    sscut << " && istation==" << istation;
    if ( istation == 1 ) {
      sstation = "CSS";
    } else if ( istation == 2 ) {
      sstation = "CSL";
    } else {
      cout << "Invalid station index: " << istation << endl;
      return;
    }
  }
  if ( wc ) {
    sscut << " && w" << var << "c==" << wc;
  } else {
    sscut << " && w" << var << "c>2";
  }
  string scut = sscut.str();
  // Display evaluated input.
  cout << "Station: " << sstation << endl;
  cout << "Cluster width: " << wc << endl;
  cout << "Plot: " << syxvar << endl;
  cout << "Cut: " << scut << endl;
  // Extract graph.
  cout << "*** Extract graph." << endl;
  cluperf()->Draw(syxvar.c_str(), scut.c_str());
  gDirectory->Delete("gres");
  TGraph* pgraw = new TGraph(*(TGraph*)gPad->GetPrimitive("Graph"));
}

//**********************************************************************

// Calculate efficiencies.

void effs(ostream* pout =0) {
  if ( pout == 0 ) pout = &cout;
  pout->setf(ios_base::fixed);
  pout->precision(3);
  Int_t w1 = 7;
  Int_t w2 = 8;
  // Event count.
  cluperf()->Draw("evt>>hevt", "evt", "goff");
  Int_t nevt = hevt->GetEntries();
  *pout << "         # events: " << setw(w1)  << nevt << endl << endl;
  TTree* ptree = cluperf();
  // Eta results.
  TCut simfound = cluperf_cut_sim_eta + cluperf_cut_found_eta;
  ptree->Draw("zc>>hznocut",  "", "goff");
  ptree->Draw("zc>>hzkin",      cluperf_cut_kine, "goff");
  ptree->Draw("zc>>hzacc",      cluperf_cut_acc_eta, "goff");
  ptree->Draw("zc-zh>>hzall",   cluperf_cut_sim_eta, "goff");
  ptree->Draw("zc-zh>>hzfound", cluperf_cut_sim_eta + cluperf_cut_found_eta, "goff");
  ptree->Draw("zc-zh>>hzgood",  cluperf_cut_sim_eta + cluperf_cut_good_eta, "goff");
  ptree->Draw("zc-zh>>hzbad",   cluperf_cut_sim_eta + cluperf_cut_bad_eta, "goff");
  ptree->Draw("dzc>>hzspoil",   cluperf_cut_sim_eta + cluperf_cut_spoiled_eta, "goff");
  Int_t nznocut = hznocut->GetEntries();
  Int_t nzall   = hzall->GetEntries();
  Int_t nzkin   = hzkin->GetEntries();
  Int_t nzacc   = hzacc->GetEntries();
  Int_t nzfound = hzfound->GetEntries();
  Int_t nzgood  = hzgood->GetEntries();
  Int_t nzbad   = hzbad->GetEntries();
  Int_t nzspoil = hzspoil->GetEntries();
  *pout << "   eta candidates: " << setw(w1) << nznocut << endl;
  *pout << "  kinematical acc: " << setw(w1) <<   nzkin << setw(w2) << floatrat(nzkin,nznocut)
        << "    (" << cluperf_cut_kine.GetTitle() << "MeV)" << endl;
  *pout << "  geometrical acc: " << setw(w1) <<   nzacc << setw(w2) << floatrat(nzacc,nznocut) << endl;
  *pout << "     combined acc: " << setw(w1) <<   nzall << setw(w2) << floatrat(nzall,nznocut) << endl;
  *pout << "       efficiency: " << setw(w1) << nzfound << setw(w2) << floatrat(nzfound,nzall) << endl;
  *pout << "  good efficiency: " << setw(w1) <<  nzgood << setw(w2) << floatrat(nzgood,nzall)
        << "    (" << cluperf_cut_good_eta.GetTitle() << ")" << endl;
  *pout << "     bad fraction: " << setw(w1) <<   nzbad << setw(w2) << floatrat(nzbad,nzfound)
        << "    (" << cluperf_cut_bad_eta.GetTitle() << ")" << endl;
  *pout << "          spoiled: " << setw(w1) << nzspoil << setw(w2) << floatrat(nzspoil,nzfound) << endl;
  // Phi results.
  simfound = cluperf_cut_sim_phi + cluperf_cut_found_phi;
  ptree->Draw("yc>>hynocut",  "", "goff");
  ptree->Draw("yc>>hykin",      cluperf_cut_kine, "goff");
  ptree->Draw("yc>>hyacc",      cluperf_cut_acc_phi, "goff");
  ptree->Draw("yc-yh>>hyall",   cluperf_cut_sim_phi, "goff");
  ptree->Draw("yc-yh>>hyfound", cluperf_cut_sim_phi + cluperf_cut_found_phi, "goff");
  ptree->Draw("yc-yh>>hygood",  cluperf_cut_sim_phi + cluperf_cut_good_phi, "goff");
  ptree->Draw("yc-yh>>hybad",   cluperf_cut_sim_phi + cluperf_cut_bad_phi, "goff");
  ptree->Draw("dyc>>hyspoil",   cluperf_cut_sim_phi + cluperf_cut_spoiled_phi, "goff");
  Int_t nynocut = hynocut->GetEntries();
  Int_t nyall = hyall->GetEntries();
  Int_t nykin    = hykin->GetEntries();
  Int_t nyacc   = hyacc->GetEntries();
  Int_t nyfound = hyfound->GetEntries();
  Int_t nygood  = hygood->GetEntries();
  Int_t nybad   = hybad->GetEntries();
  Int_t nyspoil = hyspoil->GetEntries();
  *pout << "\n";
  *pout << "   phi candidates: " << setw(w1) << nynocut << endl;
  *pout << "  kinematical acc: " << setw(w1) <<   nykin << setw(w2) << floatrat(nykin,nynocut)
        << "    (" << cluperf_cut_kine.GetTitle() << "MeV)" << endl;
  *pout << "  geometrical acc: " << setw(w1) <<   nyacc << setw(w2) << floatrat(nyacc,nynocut) << endl;
  *pout << "     combined acc: " << setw(w1) <<   nyall << setw(w2) << floatrat(nyall,nynocut) << endl;
  *pout << "       efficiency: " << setw(w1) << nyfound << setw(w2) << floatrat(nyfound,nyall) << endl;
  *pout << "  good efficiency: " << setw(w1) <<  nygood << setw(w2) << floatrat(nygood,nyall)
        << "    (" << cluperf_cut_good_phi.GetTitle() << ")" << endl;
  *pout << "     bad fraction: " << setw(w1) <<   nybad << setw(w2) << floatrat(nybad,nyfound)
        << "    (" << cluperf_cut_bad_phi.GetTitle() << ")" << endl;
  *pout << "          spoiled: " << setw(w1) << nyspoil << setw(w2) << floatrat(nyspoil,nyfound) << endl;
  pout->setf(0);
  pout->precision(6);
}

//**********************************************************************

// Resolution efficiency plot.

void reseff(Bool_t measphi, Bool_t truthnorm =true) {
  Double_t lmax = 3;
  Double_t lmin = -lmax;
  TH1F* phall = 0;
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  TH1F* phunspoil = 0;
  Int_t nbin = 60;
  TTree* ptree = cluperf();
  if ( measphi ) {
    gDirectory->Delete("heff_allphi");
    gDirectory->Delete("heff_phi");
    gDirectory->Delete("heff_phispoil");
    gDirectory->Delete("heff_phiunspoil");
    phall = new TH1F("heff_allphi", "true phi position", nbin, lmin, lmax);
    ptree->Draw("yh>>heff_allphi", cluperf_cut_sim_phi, "goff");
    ph = new TH1F("heff_phi", "phi residual", nbin, lmin, lmax);
    ptree->Draw("log10(abs(yc-yh))>>heff_phi",
                cluperf_cut_sim_phi + cluperf_cut_found_phi, "goff");
    phspoil = new TH1F("heff_phispoil", "phi residual spoiled efficiency", nbin, lmin, lmax);
    ptree->Draw("log10(abs(yc-yh))>>heff_phispoil",
                cluperf_cut_sim_phi + cluperf_cut_spoiled_phi, "goff");
    phunspoil = new TH1F("heff_phiunspoil", "phi residual unspoiled efficiency", nbin, lmin, lmax);
    ptree->Draw("log10(abs(yc-yh))>>heff_phiunspoil",
                cluperf_cut_sim_eta + cluperf_cut_unspoiled_eta, "goff");
  } else {
    gDirectory->Delete("heff_alleta");
    gDirectory->Delete("heff_eta");
    gDirectory->Delete("heff_etaspoil");
    gDirectory->Delete("heff_etaunspoil");
    phall = new TH1F("heff_alleta", "true eta position", nbin, lmin, lmax);
    ptree->Draw("zh>>heff_alleta", cluperf_cut_sim_eta, "goff");
    ph = new TH1F("heff_eta", "eta residual", nbin, lmin, lmax);
    ptree->Draw("log10(abs(zc-zh))>>heff_eta",
                cluperf_cut_sim_eta + cluperf_cut_found_eta, "goff");
    phspoil = new TH1F("heff_etaspoil", "eta residual spoiled efficiency", nbin, lmin, lmax);
    ptree->Draw("log10(abs(zc-zh))>>heff_etaspoil",
                cluperf_cut_sim_eta + cluperf_cut_spoiled_eta, "goff");
    phunspoil = new TH1F("heff_etaunspoil", "eta residual unspoiled efficiency", nbin, lmin, lmax);
    ptree->Draw("log10(abs(zc-zh))>>heff_etaunspoil",
                cluperf_cut_sim_eta + cluperf_cut_unspoiled_eta, "goff");
  }
  // Normalize.
  if ( truthnorm ) {
    ph->SetBinContent(nbin+1, phall->GetEntries() - ph->GetEntries());
    phspoil->SetBinContent(nbin+1, phall->GetEntries() - phspoil->GetEntries());
    phunspoil->SetBinContent(nbin+1, phall->GetEntries() - phunspoil->GetEntries());
    string app = " (truth normalization)";
    string title = ph->GetTitle();
    title += app;
    ph->SetTitle(title.c_str());
    title = phspoil->GetTitle();
    title += app;
    phspoil->SetTitle(title.c_str());
    title = phunspoil->GetTitle();
    title += app;
    phunspoil->SetTitle(title.c_str());
  } else {
    phspoil->SetBinContent(nbin+1, ph->GetEntries() - phspoil->GetEntries());
    phunspoil->SetBinContent(nbin+1, ph->GetEntries() - phunspoil->GetEntries());
  }
  // RSet histogram properties.
  ph->SetXTitle("log10(abs(residual) in mm)");
  TH1F* phint = integrate(ph);
  TH1F* phint_spoiled = integrate(phspoil);
  //phint->SetFillColor(31);
  phint->SetFillColor(33);
  phint->SetFillStyle(3001);
  phint_spoiled->SetFillColor(46);
  phint_spoiled->SetFillStyle(3010);
  phint->SetStats(false);
  phint->SetMinimum(0.0);
  phint->SetMaximum(1.00);
  phint->Draw("");
  gPad->SetGridx();
  gPad->SetGridy();
  // Draw.
  phint_spoiled->Draw("SAME");
}

//**********************************************************************

// Pull plot.

void sigeff(Bool_t measphi, string draw ="both", Int_t istation =0, Int_t width =0) {
  Double_t lmax = 2;
  Double_t lmin = -lmax;
  Int_t nbin = 60;
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  TH1F* phunspoil = 0;
  TTree* ptree = cluperf();
  if ( measphi ) {
    gDirectory->Delete("hsig_allphi");
    gDirectory->Delete("hsig_phi");
    gDirectory->Delete("hsig_phispoil");
    gDirectory->Delete("hsig_phiunspoil");
    phall = new TH1F("hsig_allphi", "true phi position", nbin, lmin, lmax);
    ptree->Draw("yh>>hsig_allphi", cluperf_cut_sim_phi, "goff");
    ph = new TH1F("hsig_phi", "phi residual pull", nbin, lmin, lmax);
    ptree->Draw("log10(abs(yc-yh)/dyc)>>hsig_phi",
                cluperf_cut_sim_phi + cluperf_cut_found_phi, "goff");
    string test_spoiled = (cluperf_cut_sim_phi + cluperf_cut_spoiled_phi).GetTitle();
    string test_unspoiled = (cluperf_cut_sim_phi + cluperf_cut_unspoiled_phi).GetTitle();
    string title_spoiled = "phi residual spoiled pull";
    string title_unspoiled = "phi residual unspoiled pull";
    if ( draw != "both" ) {
      if ( istation == 1 ) {
        title_spoiled += " CSS";
        title_unspoiled += " CSS";
        test_spoiled += " && istation==1";
        test_unspoiled += " && istation==1";
      } else if ( istation == 2 ) {
        title_spoiled += " CSL";
        title_unspoiled += " CSL";
        test_spoiled += " && istation==2";
        test_unspoiled += " && istation==2";
      }
    }
    if ( width && draw != "both" ) {
      ostringstream ssw;
      ssw << width;
      string sw = ssw.str();
      title_spoiled += " w=" + sw;
      title_unspoiled += " w=" + sw;
      test_spoiled += " && wyc==" + sw;
      test_unspoiled += " && wyc==" + sw;
    }
    phspoil = new TH1F("hsig_phispoil", title_spoiled.c_str(), nbin, lmin, lmax);
    phunspoil = new TH1F("hsig_phiunspoil", title_unspoiled.c_str(), nbin, lmin, lmax);
    ptree->Draw("log10(abs(yc-yh)/dyc)>>hsig_phispoil", test_spoiled.c_str(), "goff");
    ptree->Draw("log10(abs(yc-yh)/dyc)>>hsig_phiunspoil", test_unspoiled.c_str(), "goff");
  } else {
    gDirectory->Delete("hsig_eta");
    gDirectory->Delete("hsig_alleta");
    gDirectory->Delete("hsig_etaspoil");
    gDirectory->Delete("hsig_etaunspoil");
    phall = new TH1F("hsig_alleta", "true eta position", nbin, lmin, lmax);
    ptree->Draw("zh>>hsig_alleta", cluperf_cut_sim_eta, "goff");
    ph = new TH1F("hsig_eta", "eta residual pull", nbin, lmin, lmax);
    ptree->Draw("log10(abs(zc-zh)/dzc)>>hsig_eta",
                cluperf_cut_sim_eta + cluperf_cut_found_eta, "goff");
    string test_spoiled = (cluperf_cut_sim_eta + cluperf_cut_spoiled_eta).GetTitle();
    string test_unspoiled = (cluperf_cut_sim_eta + cluperf_cut_unspoiled_eta).GetTitle();
    string title_spoiled = "eta residual spoiled pull";
    string title_unspoiled = "eta residual unspoiled pull";
    if ( draw != "both" ) {
      if ( istation == 1 ) {
        title_spoiled += " CSS";
        title_unspoiled += " CSS";
        test_spoiled += " && istation==1";
        test_unspoiled += " && istation==1";
      } else if ( istation == 2 ) {
        title_spoiled += " CSL";
        title_unspoiled += " CSL";
        test_spoiled += " && istation==2";
        test_unspoiled += " && istation==2";
      }
    }
    if ( width && draw != "both" ) {
      ostringstream ssw;
      ssw << width;
      string sw = ssw.str();
      title_spoiled += " w=" + sw;
      title_unspoiled += " w=" + sw;
      test_spoiled += " && wzc==" + sw;
      test_unspoiled += " && wzc==" + sw;
    }
    phspoil = new TH1F("hsig_etaspoil", title_spoiled.c_str(), nbin, lmin, lmax);
    phunspoil = new TH1F("hsig_etaunspoil", title_unspoiled.c_str(), nbin, lmin, lmax);
    ptree->Draw("log10(abs(zc-zh)/dzc)>>hsig_etaspoil", test_spoiled.c_str(), "goff");
    ptree->Draw("log10(abs(zc-zh)/dzc)>>hsig_etaunspoil", test_unspoiled.c_str(), "goff");
  }
  // Normalize.
  if ( draw = = "both" ) {
    //ph->SetBinContent(nbin+1, phall->GetEntries() - ph->GetEntries());
    phspoil->SetBinContent(nbin+1, ph->GetEntries() - phspoil->GetEntries());
    phunspoil->SetBinContent(nbin+1, ph->GetEntries() - phunspoil->GetEntries());
  }
  // Set histogram properies.
  ph->SetXTitle("log10(abs(residual)/error)");
  gStyle->SetOptStat(false);
  TH1F* phint = integrate(ph);
  TH1F* phint_spoiled = integrate(phspoil);
  TH1F* phint_unspoiled = integrate(phunspoil);
  //phint->SetFillColor(31);
  phint->SetFillColor(33);
  phint->SetFillStyle(3001);
  phint->SetStats(false);
  phint_spoiled->SetFillColor(46);
  phint_spoiled->SetFillStyle(3010);
  phint_spoiled->SetStats(false);
  phint_unspoiled->SetFillColor(30);
  phint_unspoiled->SetFillStyle(3001);
  phint_unspoiled->SetStats(false);
  phint->GetYaxis()->SetRangeUser(0.0, 1.00);
  phint_spoiled->GetYaxis()->SetRangeUser(0.0, 1.00);
  phint_unspoiled->GetYaxis()->SetRangeUser(0.0, 1.00);
  // Draw.
  if ( draw = = "both" ) {
    phint->Draw("");
    phint_spoiled->Draw("SAME");
  } else if ( draw == "spoiled" ) {
    phint_spoiled->Draw("");
  } else if ( draw == "unspoiled" ) {
    phint_unspoiled->Draw("");
  }
  gPad->SetGridx();
  gPad->SetGridy();
  gaussintlog(nbin, lmin, lmax)->Draw("same");
  flatintlog(nbin, lmin, lmax)->Draw("same");
}

//**********************************************************************

// RMS vs pull.

void rmssig(Int_t measphi) {
  if ( measphi ) {
    cluperf()->Draw("ryc:abs(yc-yh)/dyc", "siminy(istation,yh) && wyc>0 && dyc<1 && abs(yc-yh)/dyc>1");
  } else {
    cluperf()->Draw("rzc:abs(zc-zh)/dzc", "siminz(istation,zh) && wzc>0 && dzc<1 && abs(zc-zh)/dzc>1");
  }
}

//**********************************************************************

// Resolution plot.

void resplot(Bool_t measphi, Double_t lim, Bool_t fit =true, Bool_t showspoil =true) {
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(1);
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  Int_t nbin = 100;
  TTree* ptree = cluperf();
  if ( measphi ) {
    gDirectory->Delete("hres_phi");
    gDirectory->Delete("hres_phispoil");
    gDirectory->Delete("hres_phiunspoil");
    ph = new TH1F("hres_phi", "phi resolution", nbin, -lim, lim);
    ostringstream sscut;
    sscut << "abs(yc-yh)<" << lim;
    TCut cut_lim(sscut.str().c_str());
    ptree->Draw("yc-yh>>hres_phi",
                cluperf_cut_sim_phi + cluperf_cut_found_phi + cut_lim, "goff");
    phspoil = new TH1F("hres_phi_spoiled", "spoiled phi resolution", nbin, -lim, lim);
    ptree->Draw("yc-yh>>hres_phi_spoiled",
                cluperf_cut_sim_phi + cluperf_cut_spoiled_phi + cut_lim, "goff");
  } else {
    gDirectory->Delete("hres_eta");
    gDirectory->Delete("hres_etaspoil");
    gDirectory->Delete("hres_etaunspoil");
    ph = new TH1F("hres_eta", "eta resolution", nbin, -lim, lim);
    ostringstream sscut;
    sscut << "abs(zc-zh)<" << lim;
    TCut cut_lim(sscut.str().c_str());
    ptree->Draw("zc-zh>>hres_eta",
                cluperf_cut_sim_eta + cluperf_cut_found_eta + cut_lim, "goff");
    phspoil = new TH1F("hres_eta_spoiled", "spoiled r resolution", nbin, -lim, lim);
    ptree->Draw("zc-zh>>hres_eta_spoiled",
                cluperf_cut_sim_eta + cluperf_cut_spoiled_eta + cut_lim, "goff");
  }
  ph->SetFillColor(33);    //gray
  ph->SetFillStyle(3001);
  if ( fit ) ph->Fit("gaus");
  ph->Draw("");
  ph->SetXTitle("residual (mm)");
  if ( showspoil ) {
    phspoil->SetFillColor(46);   //red
    phspoil->SetFillStyle(3010); //brick
    phspoil->Draw("SAME");
  }
  gPad->SetGridx(false);
  gPad->SetGridy(false);
}

//**********************************************************************

// Spoil plot.

void spoilplot(Bool_t measphi) {
  TH1* ph = 0;
  Int_t nbin = 19;
  TTree* ptree = cluperf();
  if ( measphi ) {
    gDirectory->Delete("hspoil_phi");
    ph = new TH1I("hspoil_phi", "phi spoil flag; #phi spoil flag;", nbin, 0, nbin);
    ostringstream sscut;
    ptree->Draw("syc>>hspoil_phi", cluperf_cut_sim_phi + cluperf_cut_found_phi, "goff");
  } else {
    gDirectory->Delete("hspoil_eta");
    ph = new TH1I("hspoil_eta", "eta spoil flag; #eta spoil flag", nbin, 0, nbin);
    ptree->Draw("szc>>hspoil_eta", cluperf_cut_sim_eta + cluperf_cut_found_eta, "goff");
  }
  //ph->SetXTitle("spoil flag");
  ph->Draw();
}

//**********************************************************************

// Pull plot.

void sigplot(Bool_t measphi, Double_t lim, Bool_t fit =true, Bool_t showspoil =true) {
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(1);
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  TH1F* phunspoil = 0;
  Int_t nbin = 100;
  TTree* ptree = cluperf();
  if ( measphi ) {
    gDirectory->Delete("hsig_phi");
    gDirectory->Delete("hsig_phispoil");
    gDirectory->Delete("hsig_phiunspoil");
    ph = new TH1F("hsig_phi", "phi pull", nbin, -lim, lim);
    ostringstream sscut;
    sscut << "abs(yc-yh)/dyc<" << lim;
    TCut cut_lim(sscut.str().c_str());
    ptree->Draw("(yc-yh)/dyc>>hsig_phi", 
                cluperf_cut_sim_phi + cluperf_cut_found_phi + cut_lim, "goff");
    phspoil = new TH1F("hsig_phi_spoiled", "spoiled phi pull", nbin, -lim, lim);
    phunspoil = new TH1F("hsig_phi_unspoiled", "unspoiled phi pull", nbin, -lim, lim);
    ptree->Draw("(yc-yh)/dyc>>hsig_phi_spoiled",
                cluperf_cut_sim_phi + cluperf_cut_spoiled_phi + cut_lim, "goff");
    ptree->Draw("(yc-yh)/dyc>>hsig_phi_unspoiled",
                cluperf_cut_sim_phi + cluperf_cut_unspoiled_phi + cut_lim, "goff");
  } else {
    gDirectory->Delete("hsig_eta");
    gDirectory->Delete("hsig_etaspoil");
    gDirectory->Delete("hsig_etaunspoil");
    ph = new TH1F("hsig_eta", "eta pull", nbin, -lim, lim);
    ostringstream sscut;
    sscut << "abs(zc-zh)/dzc<" << lim;
    TCut cut_lim(sscut.str().c_str());
    ptree->Draw("(zc-zh)/dzc>>hsig_eta",
                cluperf_cut_sim_eta + cluperf_cut_found_eta + cut_lim, "goff");
    phspoil = new TH1F("hsig_eta_spoiled", "spoiled r pull", nbin, -lim, lim);
    phunspoil = new TH1F("hsig_eta_unspoiled", "unspoiled r pull", nbin, -lim, lim);
    ptree->Draw("(zc-zh)/dzc>>hsig_eta_spoiled",
                cluperf_cut_sim_eta + cluperf_cut_spoiled_eta + cut_lim, "goff");
    ptree->Draw("(zc-zh)/dzc>>hsig_eta_unspoiled",
                cluperf_cut_sim_eta + cluperf_cut_unspoiled_eta + cut_lim, "goff");
  }
  ph->SetFillColor(33);
  ph->SetFillStyle(3001);
  phspoil->SetFillColor(46);
  phspoil->SetFillStyle(3010);
  phunspoil->SetFillColor(30);
  phunspoil->SetFillStyle(3001);
  if ( fit ) ph->Fit("gaus");
  ph->Draw("");
  ph->SetXTitle("pull (residual/error)");
  if ( showspoil ) {
    phspoil->Draw("SAME");
  }
  gPad->SetGridx(false);
  gPad->SetGridy(false);
}

//**********************************************************************

// Draw clusters and truth hits for an event.

void draw_event_clusters(Int_t run, Int_t evt) {
  CscDetectorPlot* ppdet = CscDetectorPlot::new_global(run, evt);
  draw_clusters(run, evt, ppdet, false);
  draw_simpos(run, evt, ppdet, false);
  ppdet->draw();
}

//**********************************************************************
// Charge ratio table.
//**********************************************************************

// Calculate strip coordinate in range (0,1) going from left to right.
// If a charge ratio is provided, then values in the corners are put
// slightly outside the range to make them more consistent with that
// ratio.
Double_t rstrippos(Double_t pos, Double_t pitch, Double_t qrat =0.0) {
  Double_t spos = fmod(100+pos/pitch,1);
  if ( qrat > 0.0 ) {
    if ( spos-qrat > 0.7 ) spos -= 1.0;
    if ( spos-qrat < -0.8 ) spos += 1.0;
  }
  return spos;
}

// Calculate strip coordinate in range (0,1) going from right to left.
// If a charge ratio is provided, then values in the corners are put
// slightly outside the range to make them more consistent with that
// ratio.
Double_t lstrippos(Double_t pos, Double_t pitch, Double_t qrat =0.0) {
  Double_t spos = 1.0 - fmod(100+pos/pitch,1);
  if ( qrat > 0.0 ) {
    if ( spos-qrat > 0.7 ) spos -= 1.0;
    if ( spos-qrat < -0.8 ) spos += 1.0;
  }
  return spos;
}

// Return the name for a bin histogram.
string binhistname(Int_t ibin) {
  ostringstream sout;
  sout << "hqrbin" << ibin;
  return sout.str();
}

void qrat_table_fit(Int_t istation, Bool_t measphi =false, Int_t nbin=100, Double_t nrms) {
  TTree* ptree = cluperf();
  new TCanvas("qrat");
  Int_t verbose = 1;
  if ( istation < 1 || istation > 2 ) {
    cout << "Invalid station: " << istation << endl;
    return;
  }
  if ( nbin < 20 || nbin > 1000 ) {
    cout << "Invalid number of bins: " << nbin << endl;
    return;
  }
  if ( verbose > 0 ) {
    cout << "Generating MC QRAT table for station ";
    if ( istation ==1 ) cout << "CSS";
    else cout << "CSL";
    cout << " ";
    if ( measphi ) cout << "phi";
    else cout << "r";
    cout << "\n  # bins is " << nbin << endl;
    cout << "  Points discarded if deviation/RMS > " << nrms << endl;
  }
  TObjArray gcol;
  string var = measphi ? "y" : "z";
  ostringstream sscut;
  sscut << "ql" << var << ">0 && qr" << var << ">0"
        << " && istation==" << istation;
  string scut = sscut.str();
  if ( verbose > 1 ) cout << " Base cut: " << scut << endl;
  string rmain = "rstrippos(zh,pzc,qrz/qpz)-qrz/qpz";
  string scorner = "0.7";
  string rcorner = rmain + ">" + scorner;
  rmain += "<=" + scorner;
  string lmain = "lstrippos(zh,pzc,qlz/qpz)-qlz/qpz";
  string lcorner = lmain + "<" + scorner;
  lmain += "<=" + scorner;
  // Fetch the data for qleft/qpeak.
  if ( verbose > 0 ) cout << "Fetching left data" << endl;
  string sldraw = "lstrippos(zh,pzc,qlz/qpz):qlz/qpz";
  string slcut = scut + " && " + lmain;
  if ( verbose > 1 ) cout << "  Left draw: " << sldraw << endl;
  if ( verbose > 1 ) cout << "  Left cut: " << slcut << endl;
  ptree->Draw(sldraw.c_str(), slcut.c_str());
  TGraph* pgql = new TGraph(*(TGraph*)gPad->GetPrimitive("Graph"));
  pgql->SetName("gql");
  pgql->SetMarkerColor(38);
  if ( verbose > 1 )  cout << "  # left entries: " << pgql->GetN() << endl;
  // Fetch the data for qright/qpeak.
  if ( verbose > 0 ) cout << "Fetching right data" << endl;
  string srdraw = "rstrippos(zh,pzc,qrz/qpz):qrz/qpz";
  string srcut = scut + " && " + rmain;
  if ( verbose > 1 ) cout << "  Right cut: " << srcut << endl;
  ptree->Draw(srdraw.c_str(), srcut.c_str());
  TGraph* pgqr = new TGraph(*(TGraph*)gPad->GetPrimitive("Graph"));
  pgqr->SetName("hqr");
  pgqr->SetMarkerColor(46);
  gPad->Clear();
  pgqr->SetTitle("MC QRAT calibration");
  pgqr->GetXaxis()->SetTitle("QRAT (Qadjacent/Qpeak)");
  pgqr->GetYaxis()->SetTitle("Strip position");
  pgqr->Draw("ap");
  pgql->Draw("p");
  // Combine the above.
  gcol.Clear();
  gcol.Add(pgql);
  gcol.Add(pgqr);
  TGraph* pgq = new TGraph();
  TGraph& gq = *pgq;
  gq.Merge(&gcol);
  // Fill profile.
  if ( verbose > 0 ) cout << "Selecting and fitting..." << endl;
  TProfile* phq = 0;
  Int_t newcount = gq.GetN();
  Int_t oldcount = 0;
  if ( verbose > 0 ) cout << "Starting count is " << newcount << endl;
  Int_t nprint = 10;
  Int_t maxloop = 10;
  for ( Int_t iloop=0; iloop<maxloop; ++iloop ) {
    if ( iloop > 1 ) {
      if ( newcount <= 0 ) break;
      if ( newcount==oldcount ) break;
    }
    oldcount = newcount;
    ostringstream sshname;
    sshname << "hq" << iloop;
     string shname = sshname.str();
    if ( verbose > 0 ) cout << "Creating profile " << shname << endl;
    gDirectory->Delete(shname.c_str());
    TProfile* phqold = phq;
    Double_t qrmin = 0.5/nbin;
    Double_t qrmax = 1.0 + qrmin;
    TProfile* phq = new TProfile(shname.c_str(), "Strip position vs. QRAT",
                                 nbin, qrmin, qrmax);
    for ( Int_t ipt=0; ipt<gq.GetN(); ++ipt ) {
      Double_t qr, pos;
      gq.GetPoint(ipt, qr, pos);
      // Use existing profile to make nrms cut.
      Bool_t keep = true;
      if ( phqold != 0 ) {
        keep = false;
        Int_t bin = phqold->FindBin(qr);
        if ( bin>0 && bin<nbin+1 ) {
          Double_t pred = phqold->GetBinContent(bin);
          Double_t err = phqold->GetBinError(bin);
          Double_t count = phqold->GetBinEntries(bin);
          Double_t rms = sqrt(count)*err;
          Double_t posmin = pred - nrms*rms;
          Double_t posmax = pred + nrms*rms;
          keep = pos>posmin && pos<posmax;
          if ( ipt < nprint ) {
            if ( verbose > 2 ) cout << "  " << pos << " " << pred << " " << rms << " ";
            string msg = keep ? "KEEP" : "SKIP";
            if ( verbose > 2 ) cout << keep << endl;
          }
        }
      }
      if ( keep ) phq->Fill(qr, pos);
    }
    newcount = phq->GetEntries();
    if ( verbose > 0 ) cout << "New count is " << newcount << endl;
    phq->Print();
  }
  if ( newcount == 0 ) return;
  phq->Draw("same");
  // Look for zero crossing.
  new TCanvas("xing");
  if ( verbose > 0 ) cout << "Find zero crossing." << endl;
  Double_t spmin = -0.01;
  Double_t spmax = 0.06;
  Int_t nsp = (spmax-spmin+0.001)/0.01;
  gDirectory->Delete("qxing");
  TProfile* phx = new TProfile("qxing", "QRAT vs strip position", nsp, spmin, spmax);
  for ( Int_t ipt=0; ipt<gq.GetN(); ++ipt ) {
    Double_t qr, pos;
    gq.GetPoint(ipt, qr, pos);
    if ( pos>spmin && pos<spmax ) {
      Int_t bin = phqold->FindBin(qr);
      Double_t pred = phq->GetBinContent(bin);
      Double_t err = phq->GetBinError(bin);
      Double_t count = phq->GetBinEntries(bin);
      Double_t rms = sqrt(count)*err;
      Double_t posmin = pred - nrms*rms;
      Double_t posmax = pred + nrms*rms;
      Bool_t keep = pos>posmin && pos<posmax;
      if ( verbose > 3 ) {
        cout << "  " << pos << " " << pred << " " << rms << " ";
        if ( keep ) cout << "KEEP";
        else cout << "SKIP";
      }
      if ( keep ) phx->Fill(pos, qr);
    }
  }
  string sfitfun = "pol1";
  phx->Fit(sfitfun.c_str());
  Double_t qrzero = phx->GetFunction(sfitfun.c_str())->GetParameters()[0];
  if ( verbose > 0 ) cout << "  Zero crossing at QRAT = " << qrzero << endl;
  phx->Draw();
  // Write the result.
  // Include a -0.5 offset so correction is relative to bin center rather than edge.
  cout << "# V02 MC QRAT calibration with nbin=" << nbin << " and nrms= "
       << nrms << endl;
  string suffix = "_";
  suffix += istation==1 ? "css" : "csl";
  suffix += "_";
  suffix += measphi ? "phi" : "eta";
  string algname = "theQratCscClusterFitter";
  string corname = algname + ".qratcor" + suffix;
  string minname = algname + ".qratmin" + suffix;
  string blank = "  ";
  cout << minname << " = " << qrzero << endl;
  cout << corname << " = [";
  Int_t addeol = 999;
  // Bin where the zero crossing occurs.
  // Numbering strazts from 1 (0 is for underflows).
  Int_t binzero = (qrzero-qrmin)*nbin + 1;
  for ( Int_t ibin=1; ibin<=nbin; ++ibin ) {
    if ( ibin > 1 ) cout << ", ";
    if ( ++addeol > 8 ) {
      addeol = 1;
      cout << endl << blank;
    }
    if ( ibin < binzero ) {
      cout << "0.000000";
    } else if ( ibin == nbin ) {
      cout << "1.000000";
    } else {
      cout << phq->GetBinContent(ibin);
    }
  }
  cout << "\n]" << endl;
  if ( verbose > 0 ) cout << "Goodbye" << endl;
}

//**********************************************************************
