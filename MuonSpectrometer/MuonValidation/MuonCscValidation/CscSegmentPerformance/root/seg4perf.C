/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// seg4perf.C

seg4perf_help() {
  cout << "  seg4perf() - returns 4D segperf tree" << endl;
  cout << "  effs4() - 4D performance summary" << endl;
  cout << endl;
/*
  cout << "  reseff(measphi) - integrated residual efficiency" << endl;
  cout << "  sigeff(measphi) - integrated pull efficiency" << endl;
  cout << "  rmssig(measphi) - RMS vs pull" << endl;
  cout << "  resplot(measphi, lim) - resolution" << endl;
  cout << "  sigplot(measphi, lim) - pull" << endl;
  cout << endl;
  cout << "  resfit(istation, measphi, strip, fit=true, xmin=0, nterm=4) - scor for r-plane" << endl;
  cout << "  qratfit(istation, measphi, wc, fit =true, nterm =4, xmin=0.0, xmax=1.0)" << endl;
*/
}

//**********************************************************************

// Return the simpos tree.

TTree* seg4perf() {
  return dynamic_cast<TTree*>(gDirectory->Get("csc_4d_segperf"));
}

//**********************************************************************

string sstation(int istation) {
  if ( istation == 1 ) return "CSS";
  if ( istation == 2 ) return "CSL";
  return "Unknown station";
}

//**********************************************************************
/*

// Chamber strip limits: 0.5*pitch*nstrip
double maxpos(int istation, bool measphi) {
  if ( istation == 1 ) {
    if ( measphi ) return 310.1;
    return 533.8;
  } else if ( istation == 2 ) {
    if ( measphi ) return 504.0;
    return 509.8;
  }
  return 0.0;
}

// Layer positions.
const double xw1 = -38.5;
const double xw2 = -12.8;
const double xw3 =  12.9;
const double xw4 =  38.6;

// Return if a seg is within the above boundaries.
//   measphi = 0 for r
//             1 for phi
//             2 for both
bool siminz(int istation, double zh, double azh) {
  double m = 1.0/tan(azh);
  double p1 = zh + m*xw4;
  double p2 = zh + m*xw1;
  return fabs(p1) < maxpos(istation, false) &&
         fabs(p2) < maxpos(istation, false);
}
bool siminy(int istation, double yh, double ayh) {
  double m = 1.0/tan(ayh);
  double p1 = yh + m*xw4;
  double p2 = yh + m*xw1;
  return fabs(p1) < maxpos(istation, true) &&
         fabs(p2) < maxpos(istation, true);
}
bool simin(int istation, double yh, double zh, double ayh, double azh) {
  return siminy(istation, yh, ayh) && siminz(istation, zh, azh);
}
*/
//**********************************************************************

// Cuts.

// Aceptance cuts for truth muons.
TCut seg4perf_cut_acc = "simin(istation, yh, zh, ayh, azh)";

// Kinematic cut for truth muons.
TCut seg4perf_cut_kine = "pt>2000";

// Acceptance + kinematics.
TCut seg4perf_cut_ref = seg4perf_cut_kine + seg4perf_cut_acc;

// Cut to flag found.
TCut seg4perf_cut_found = "found";

// Cut to flag found good or bad.
TCut seg4perf_cut_good = "found && drec<=4.3";
TCut seg4perf_cut_bad = "found && drec>4.3";

//**********************************************************************

// Display metrics.

void effs4(ostream* pout =0) {
  if ( pout == 0 ) pout = &cout;
  pout->setf(ios_base::fixed);
  pout->precision(3);
  int w1 = 7;
  int w2 = 8;
  TTree* ptree = seg4perf();
  ptree->Draw("evt>>hevt", "evt", "goff");
  ptree->Draw("nreco>>hnreco",  "", "goff");
  ptree->Draw("ntruth>>hntruth",  "", "goff");
  ptree->Draw("nfound>>hnfound",  "", "goff");
  ptree->Draw("nlost>>hnlost",  "", "goff");
  ptree->Draw("nfake>>hnfake",  "", "goff");
  int nevt = hevt->GetEntries();
  double mtruth = hntruth->GetMean();
  double mreco = hnreco->GetMean();
  double mfound = hnfound->GetMean();
  double mlost = hnlost->GetMean();
  double mfake = hnfake->GetMean();
  *pout << "           events: " << setw(w1)  << nevt << endl;
  *pout << "      truth/event: " << setw(w1)  << mtruth << endl;
  *pout << "       reco/event: " << setw(w1)  << mreco << endl;
  *pout << "      found/event: " << setw(w1)  << mfound << endl;
  *pout << "       lost/event: " << setw(w1)  << mlost << endl;
  *pout << "       fake/event: " << setw(w1)  << mfake << endl;
  *pout << "      found/truth: " << setw(w1) << floatrat(mfound,mtruth) << endl;
  // Calculate effficiency with acceptance and kinematic cuts.
  ptree->Draw("zh>>h4truth",  "", "goff");
  ptree->Draw("zh>>h4kin",   seg4perf_cut_kine, "goff");
  ptree->Draw("zh>>h4acc",   seg4perf_cut_acc, "goff");
  ptree->Draw("zh>>h4ref",   seg4perf_cut_ref, "goff");
  ptree->Draw("zh>>h4found", seg4perf_cut_ref + seg4perf_cut_found, "goff");
  ptree->Draw("zh>>h4good",  seg4perf_cut_ref + seg4perf_cut_good, "goff");
  ptree->Draw("zh>>h4bad",   seg4perf_cut_ref + seg4perf_cut_bad, "goff");
  int ntruth = h4truth->GetEntries();
  int nkin   = h4kin->GetEntries();
  int nacc   = h4acc->GetEntries();
  int nref   = h4ref->GetEntries();
  int nfound = h4found->GetEntries();
  int ngood  = h4good->GetEntries();
  int nbad   = h4bad->GetEntries();
  *pout << endl;
  *pout << "            Truth: " << setw(w1) << ntruth << endl;
  *pout << "  kinematical acc: " << setw(w1) <<   nkin << setw(w2) << floatrat(nkin,ntruth) << endl;
  *pout << "  geometrical acc: " << setw(w1) <<   nacc << setw(w2) << floatrat(nacc,ntruth) << endl;
  *pout << "     combined acc: " << setw(w1) <<   nref << setw(w2) << floatrat(nref,ntruth) << endl;
  *pout << "     found in acc: " << setw(w1) <<   nref << setw(w2) << floatrat(nfound,ntruth) << endl;
  *pout << "       efficiency: " << setw(w1) << floatrat(nfound,nref) 
        << "              (in acceptance)" << endl;
  *pout << "  good efficiency: " << setw(w1) << floatrat(ngood,nref)
        << "              (in acceptance; deva<4.3)" << endl;
  *pout << "     bad fraction: " << setw(w1) << floatrat(nbad,nfound)
        << "              (in acceptance; deva>4.3)" << endl;

  // Reset output stream state.
  pout->setf(0);
  pout->precision(6);
}

//**********************************************************************

// Old Efficiencies.

void effs4old(ostream* pout =0) {
  if ( ! pout ) pout = &cout;
  string sgood = "4.3";
  int gtot = 0;
  int gfnd = 0;
  int grec = 0;
  int can[3];
  double efind[3];
  double egood[3];
  for ( int istation=1; istation<3; ++istation ) {
    ostringstream sscut;
    sscut << "istation==" << istation << " && simin(istation, yh, zh, ayh, azh)";
    string scuttot = sscut.str();
    string scutfnd = sscut.str() + " && found";
    string scutrec = scutfnd + " && drec<" + sgood;
    string sdraw = "z";
    int tot = seg4perf()->Draw(sdraw.c_str(), scuttot.c_str(), "goff");
    int fnd = seg4perf()->Draw(sdraw.c_str(), scutfnd.c_str(), "goff");
    int rec = seg4perf()->Draw(sdraw.c_str(), scutrec.c_str(), "goff");
    can[istation] = tot;
    efind[istation] = double(fnd)/tot;
    egood[istation] = double(rec)/tot;
    gtot += tot;
    gfnd += fnd;
    grec += rec;
  }
  can[0] = gtot;
  efind[0] = double(gfnd)/gtot;
  egood[0] = double(grec)/gtot;
  efind[0] = double(gfnd)/gtot;
  // Get the event count and fake rate.
  // ...truth
  TH1F* phntruth = new TH1F("ntruth", "ntruth", 100, 0, 100);
  seg4perf()->Draw("nentry>>ntruth", "", "goff");
  int nevt = phntruth->GetEntries();
  double mean_ntruth = phntruth->GetMean();
  double mean_ntruth_err = phntruth->GetMeanError();
  // ...fake
  TH1F* phfake = new TH1F("fakes", "fakes", 100, 0, 100);
  seg4perf()->Draw("nfake>>fakes", "", "goff");
  double fake = phfake->GetMean();
  double fakeerr = phfake->GetMeanError();
  // ...found
  TH1F* phfound = new TH1F("found", "found", 100, 0, 100);
  seg4perf()->Draw("found>>found", "found", "goff");
  int nfound = phfound->GetEntries();
  double mean_found = double(nfound)/nevt;
  double founderr = sqrt(double(nfound))/nevt;
  // ...show
  pout->setf(ios_base::fixed);
  pout->precision(3);
  int w1 = 6;
  *pout << "                # events: " << setw(w1) << nevt << endl;
  *pout << "        # truth segments: " << setw(w1) << int(nevt*mean_ntruth) << endl;
  *pout << "     # found 4D segments: " << setw(w1) << nfound << endl;
  *pout << "      # fake 4D segments: " << setw(w1) << int(nevt*fake) << endl;
  *pout << "    truth segments/event: " << mean_ntruth << " +/- " << mean_ntruth_err <<  endl;
  *pout << " found 4D segments/event: " << mean_found << " +/- " << founderr <<  endl;
  *pout << "  fake 4D segments/event: " << fake << " +/- " << fakeerr <<  endl;
  // Show efficiencies.
  string sp1 = "   ";
  string sp2 = "  ";
  int w1 = 8;
  int w2 = 7;
  *pout << endl;
  *pout << "                                CSS     CSL CSS+CSL" << endl;
  *pout << " With acceptance cuts:        -----   -----   -----" << endl;
  *pout << "           # truth segments";
  *pout << setw(w1) << can[1];
  *pout << setw(w1) << can[2];
  *pout << setw(w1) << can[0];
  *pout << endl;
  *pout << "                 efficiency";
  *pout << setw(w1) << efind[1];
  *pout << setw(w1) << efind[2];
  *pout << setw(w1) << efind[0];
  *pout << endl;
  *pout << " good efficiency (deva<"+ sgood + ")";
  *pout << setw(w1) << egood[1];
  *pout << setw(w1) << egood[2];
  *pout << setw(w1) << egood[0];
  *pout << endl;
  // Reset ostream state.
  pout->setf(0);
  pout->precision(6);
}

//**********************************************************************

// Pull plot.
// angle = 0 for coordinate
//         1 for angle
//         2 for evaluation distance
//         3 for truth chi-square probaility

void sigplot4(bool measphi, int angle, double lim, int isplim =2,
              bool fit =true, bool showspoil =true) {
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(1);
  int nbin = 100;
  string hname = "hsig_";
  if ( angle ) hname += "a";
  if ( angle == 2 ) hname += "deva";
  string c = measphi ? "y" : "z";
  string a = angle ? "a" : "";
  ostringstream sssplim;
  sssplim << isplim;
  string ssplim = sssplim.str();
  hname += measphi ? "phi" : "eta";
  hname_spoil = hname + "_spoiled";
  hname_unspoil = hname + "_unspoiled";
  string var = "(" + a + c + "-" + a + c  + "h)/d" + a + c;
  if ( angle == 2 ) var = "drec";
  if ( angle == 3 ) var = "TMath::Prob(drec*drec,4)";
  string draw = var + ">>" + hname;
  string draw_spoil = var + ">>" + hname_spoil;
  string draw_unspoil = var + ">>" + hname_unspoil;
  string title = "CSC 4D segment ";
  if ( angle < 2 ) title += measphi ? "phi" : "eta";
  if ( angle == 0 ) title + = " position";
  if ( angle == 1 ) title + = " angle";
  if ( angle < 2 ) {
    title += " pull";
  } else if ( angle == 2 ) {
    title += " evaluation distance";
  } else if ( angle == 3 ) {
    title += " truth chi-square probability";
  } else {
    cerr << "Unknown angle option: " << angle << endl;
    return 0;
  }
  title_spoil = title + " spoiled";
  title_unspoil = title + " unspoiled";
  string c = measphi ? "y" : "z";
  ostringstream sslim;
  sslim << lim;
  cut = "simin" + c + "(istation," + c + "h, a" + c + "h) && found" +
        " && abs(" + var + ")<" + sslim.str();
  bool have_spoil = false;  // Should we add spoil to the tuple?
  string cut_unspoil = "1";
  string cut_spoil = "0";
  if ( have_spoil ) {
    cut_spoil = cut + " && nspoil" + c + ">=" + ssplim;
    cut_unspoil = cut + " && nspoil" + c + "<" + ssplim;
  }
  cout << "Var: " << var << endl;
  cout << "Cut: " << cut << endl;
  cout << "Cut spoil: " << cut_spoil << endl;
  cout << "Spoil limit: " << isplim << endl;
  cout << "Histo names: " << hname << " " << hname_spoil
       << " " << hname_unspoil << endl;
  double lmax = lim;
  double lmin = -lim;
  if ( angle == 2 ) lmin = 0.0;
  if ( angle == 3 ) {
    lmin = 0.0;
    lmax = 1.000001;
  }
  TH1F* ph = new TH1F(hname.c_str(), title.c_str(), nbin, lmin, lmax);
  seg4perf()->Draw(draw.c_str(), cut.c_str(), "goff");
  TH1F* phspoil = new
    TH1F(hname_spoil.c_str(), title_spoil.c_str(), nbin, lmin, lmax);
  seg4perf()->Draw(draw_spoil.c_str(), cut_spoil.c_str(), "goff");
  TH1F* phunspoil = new
    TH1F(hname_unspoil.c_str(), title_unspoil.c_str(), nbin, lmin, lmax);
  seg4perf()->Draw(draw_unspoil.c_str(), cut_unspoil.c_str(), "goff");
  ph->SetFillColor(33);
  ph->SetFillStyle(3001);
  ph->SetMinimum(0);
  if ( angle >= 2 ) fit = false;
  if ( fit ) ph->Fit("gaus");
  ph->Draw("B");
  if ( angle < 2 ) {
    ph->SetXTitle("residual/error");
  } else if ( angle == 2 ) {
    ph->SetXTitle("sqrt(truth chi-square)");
  } else if ( angle == 3 ) {
    ph->SetXTitle("chi-square probability");
  }
  if ( showspoil && have_spoil ) {
    phspoil->SetFillColor(46);
    phspoil->SetFillStyle(3001);
    phspoil->Draw("B SAME");
  }
}

//**********************************************************************



/*
void resfit(int istation, int measphi, int wc, bool fit =true, double xmin =0, int nterm =4) {
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
  sscut << "abs(zh)<500 && d" << var << "c>0 && d" << var << "c<1.0";
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
  cscperf()->Draw(syxvar.c_str(), scut.c_str());
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
  double xmax = 1.0 - xmin;
  // Create the fit function.
  cout << "*** Create fit function." << endl;
  ostringstream ssfun;
  for ( int iterm=0; iterm<nterm; ++iterm ) {
    if ( iterm != 0 ) ssfun << " + ";
    ssfun << "[" << iterm << "]";
    for ( int ipow=0; ipow<2*iterm+1; ++ipow ) {
      ssfun << "*(x-0.5)";
    }
  }
  string sfun = ssfun.str();
  gDirectory->Delete("fit_resfit");
  TF1* pfun = new TF1("fit_resfit", sfun.c_str(), xmin, xmax);
  for ( int iterm=0; iterm<nterm; ++iterm ) {
    pfun->SetParameter(iterm, 0.0);
    pfun->SetParError(iterm, 0.1);
  }
  cout << "Fun: " << sfun << endl;
  int oldcount = 0;
  // Loop over fits.
  int maxfit = 20;
  TGraph* pgfit = 0;
  sstitle << " (selected points)";
  for ( int ifit=0; ifit<maxfit; ++ifit ) {
    double dymax = 0.02;
    if ( ifit == 0 ) dymax = 1.0;
    if ( ifit == 1 ) dymax = 0.2;
    // Select points for fitting: those within 0.10 + 5*sigma.
    gDirectory->Delete("fit_resfit_sel");
    vector<double> newx;
    vector<double> newy;
    cout << "*** Selecting." << endl;
    for ( int i=0; i<pgraw->GetN(); ++i ) {
      double xg = pgraw->GetX()[i];
      double yg = pgraw->GetY()[i];
      double yf = pfun->Eval(xg);
      if ( xg>xmin && xg<xmax && fabs(yg - yf) < dymax ) {
        newx.push_back(xg);
        newy.push_back(yg);
      }
    }
    pgfit = new TGraph(newx.size(), &newx.front();, &newy.front());
    pgfit->SetTitle(sstitle.str().c_str());
    int newcount = pgfit->GetN();
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
  for ( int iterm=0; iterm<nterm; ++iterm ) {
    cout << iterm << " " << pfun->GetParameter(iterm) << endl;
  }
}

//**********************************************************************

void qratfit(int istation, int measphi, int wc, bool fit =true, int nterm =4,
             double xmin=0.0, double xmax=1.0) {
  string var = "z";
  if ( measphi ) var = "y";
  ostringstream ssxvar;
  // X-variable
  //ssxvar << "fmod(100.0+" << var << "c/p" << var << "c,1)";
  ssxvar << "q" << var << "c[m" << var << "c+1]/q" << var << "c[m" << var << "c]";
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
  sscut << "abs(zh)<500 && d" << var << "c>0 && d" << var << "c<1.0";
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
  cscperf()->Draw(syxvar.c_str(), scut.c_str());
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
  for ( int iterm=0; iterm<nterm; ++iterm ) {
    if ( iterm != 0 ) ssfun << " + ";
    ssfun << "[" << iterm << "]";
    for ( int ipow=0; ipow<iterm; ++ipow ) {
      ssfun << "*x";
    }
  }
  string sfun = ssfun.str();
  gDirectory->Delete("fit_resfit");
  TF1* pfun = new TF1("fit_resfit", sfun.c_str(), xmin, xmax);
  for ( int iterm=0; iterm<nterm; ++iterm ) {
    pfun->SetParameter(iterm, 0.0);
    pfun->SetParError(iterm, 0.1);
  }
  cout << "Fun: " << sfun << endl;
  int oldcount = 0;
  // Loop over fits.
  int maxfit = 5;
  TGraph* pgfit = 0;
  sstitle << " (selected points)";
  for ( int ifit=0; ifit<maxfit; ++ifit ) {
    double dymax = 0.2;
    if ( ifit == 0 ) dymax = 5.0;
    if ( ifit == 1 ) dymax = 1.0;
    gDirectory->Delete("fit_resfit_sel");
    vector<double> newx;
    vector<double> newy;
    cout << "*** Selecting." << endl;
    for ( int i=0; i<pgraw->GetN(); ++i ) {
      double xg = pgraw->GetX()[i];
      double yg = pgraw->GetY()[i];
      double yf = pfun->Eval(xg);
      if ( xg>xmin && xg<xmax && fabs(yg - yf) < dymax ) {
        newx.push_back(xg);
        newy.push_back(yg);
      }
    }
    if ( newx.size() < nterm ) {
      cout << "Only " << newx.size() << " points remin--Giving up" << endl;
    }
    pgfit = new TGraph(newx.size(), &newx.front();, &newy.front());
    pgfit->SetTitle(sstitle.str().c_str());
    int newcount = pgfit->GetN();
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
  for ( int iterm=0; iterm<nterm; ++iterm ) {
    cout << iterm << " " << pfun->GetParameter(iterm) << endl;
  }
}

//**********************************************************************

// Draw qrat left vs qrat right.

void qrat2(int istation, int measphi=0, int wc=0) {
  string var = "z";
  if ( measphi ) var = "y";
  // X-variable
  ostringstream ssxvar;
  ssxvar << "q" << var << "c[m" << var << "c+1]/q" << var << "c[m" << var << "c]";
  string sxvar = ssxvar.str();
  // Y-variable.
  ostringstream ssyvar;
  ssyvar << "q" << var << "c[m" << var << "c-1]/q" << var << "c[m" << var << "c]";
  string syvar = ssyvar.str();
  // Plot variables.
  ostringstream ssyxvar;
  ssyxvar << syvar << ":" << sxvar;
  string syxvar = ssyxvar.str();
  // Cut.
  ostringstream sscut;
  sscut << "abs(zh)<500 && d" << var << "c>0 && d" << var << "c<1.0";
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
  cscperf()->Draw(syxvar.c_str(), scut.c_str());
  gDirectory->Delete("gres");
  TGraph* pgraw = new TGraph(*(TGraph*)gPad->GetPrimitive("Graph"));
}

//**********************************************************************

// Calculate efficiencies.

void effs() {
  cscperf()->Draw("dzc>>hzspoil", "abs(zh)<500 && wzc>0 && dzc>1.0");
  cscperf()->Draw("zc-zh>>hzall", "abs(zh)<500");
  cscperf()->Draw("zc-zh>>hzres100", "abs(zh)<500 && wzc>0 && abs(zc-zh)<100");
  cscperf()->Draw("zc-zh>>hzres10", "abs(zh)<500 && wzc>0 && abs(zc-zh)<10");
  cscperf()->Draw("zc-zh>>hzres1", "abs(zh)<500 && wzc>0 && abs(zc-zh)<1");
  cscperf()->Draw("(zc-zh)/dzc>>hzsig10", "abs(zh)<500 && wzc>0 && abs(zc-zh)/dzc<10");
  cscperf()->Draw("(zc-zh)/dzc>>hzsig5", "abs(zh)<500 && wzc>0 && abs(zc-zh)/dzc<5");
  cscperf()->Draw("(zc-zh)/dzc>>hzsig2", "abs(zh)<500 && wzc>0 && abs(zc-zh)/dzc<2");
  cscperf()->Draw("(zc-zh)/dzc>>hzsig5g", "abs(zh)<500 && wzc>0 && abs(zc-zh)/dzc>5");
  cscperf()->Draw("(zc-zh)/dzc>>hzsig10g", "abs(zh)<500 && wzc>0 && abs(zc-zh)/dzc>10");
  double nzall = hzall->GetEntries();
  double nzres100 = hzres100->GetEntries();
  double nzres10 = hzres10->GetEntries();
  double nzres1 = hzres1->GetEntries();
  double nzsig10 = hzsig10->GetEntries();
  double nzsig5 = hzsig5->GetEntries();
  double nzsig2 = hzsig2->GetEntries();
  double nzsig5g = hzsig5g->GetEntries();
  double nzsig10g = hzsig10g->GetEntries();
  double nzspoil = hzspoil->GetEntries();
  cout << "\n   z candidates: " << nzall << endl;
  cout << "z residual <100: " << nzres100 << " " << nzres100/nzall << endl;
  cout << " z residual <10: " << nzres10 << " " << nzres10/nzall << endl;
  cout << "  z residual <1: " << nzres1 << " " << nzres1/nzall << endl;
  cout << "     z pull <10: " << nzsig10 << " " << nzsig10/nzall << endl;
  cout << "      z pull <5: " << nzsig5 << " " << nzsig5/nzall << endl;
  cout << "      z pull <2: " << nzsig2 << " " << nzsig2/nzall << endl;
  cout << "      z pull >5: " << nzsig5g << " " << nzsig5g/nzall << endl;
  cout << "      z pull >10: " << nzsig10g << " " << nzsig10g/nzall << endl;
  cout << "      z spoiled: " << nzspoil << " " << nzspoil/nzall << endl;
  cscperf()->Draw("dyc>>hyspoil", "abs(zh)<500 && wyc>0 && dyc>1.0");
  cscperf()->Draw("yc-yh>>hyall", "abs(zh)<500");
  cscperf()->Draw("yc-yh>>hyres100", "abs(zh)<500 && wyc>0 && abs(yc-yh)<100");
  cscperf()->Draw("yc-yh>>hyres10", "abs(zh)<500 && wyc>0 && abs(yc-yh)<10");
  cscperf()->Draw("yc-yh>>hyres1", "abs(zh)<500 && wyc>0 && abs(yc-yh)<1");
  cscperf()->Draw("(yc-yh)/dyc>>hysig10", "abs(zh)<500 && wyc>0 && abs(yc-yh)/dyc<10");
  cscperf()->Draw("(yc-yh)/dyc>>hysig5", "abs(zh)<500 && wyc>0 && abs(yc-yh)/dyc<5");
  cscperf()->Draw("(yc-yh)/dyc>>hysig2", "abs(zh)<500 && wyc>0 && abs(yc-yh)/dyc<2");
  cscperf()->Draw("(yc-yh)/dyc>>hysig5g", "abs(zh)<500 && wyc>0 && abs(yc-yh)/dyc>5");
  cscperf()->Draw("(yc-yh)/dyc>>hysig10g", "abs(zh)<500 && wyc>0 && abs(yc-yh)/dyc>10");
  double nyall = hyall->GetEntries();
  double nyres100 = hyres100->GetEntries();
  double nyres10 = hyres10->GetEntries();
  double nyres1 = hyres1->GetEntries();
  double nysig10 = hysig10->GetEntries();
  double nysig5 = hysig5->GetEntries();
  double nysig2 = hysig2->GetEntries();
  double nysig5g = hysig5g->GetEntries();
  double nysig10g = hysig10g->GetEntries();
  double nyspoil = hyspoil->GetEntries();
  cout << "\n   y candidates: " << nyall << endl;
  cout << "y residual <100: " << nyres100 << " " << nyres100/nyall << endl;
  cout << " y residual <10: " << nyres10 << " " << nyres10/nyall << endl;
  cout << "  y residual <1: " << nyres1 << " " << nyres1/nyall << endl;
  cout << "     y pull <10: " << nysig10 << " " << nysig10/nyall << endl;
  cout << "      y pull <5: " << nysig5 << " " << nysig5/nyall << endl;
  cout << "      y pull <2: " << nysig2 << " " << nysig2/nyall << endl;
  cout << "      y pull >5: " << nysig5g << " " << nysig5g/nyall << endl;
  cout << "      y pull >10: " << nysig10g << " " << nysig10g/nyall << endl;
  cout << "      y spoiled: " << nyspoil << " " << nyspoil/nyall << endl;
}

//**********************************************************************

// Integrate a histogram.

TH1F* integrate(TH1F* phin0 =0) {
  if ( phin0 == 0 ) phin0 = htemp;
  if ( phin0 == 0 ) return 0;
  TH1F* phin = new TH1F(*phin0);
  string inname = phin->GetName();
  inname += "_input";
  gDirectory->Delete(inname.c_str());
  string outname = phin->GetName();
  outname += "_integrated";
  gDirectory->Delete(outname.c_str());
  phin->SetName(inname.c_str());
  string title = "Integrated ";
  title += phin->GetTitle();
  TH1F* phout = new TH1F(outname.c_str(), title.c_str(), phin->GetNbinsX(),
                        phin->GetXaxis()->GetXmin(), phin->GetXaxis()->GetXmax());
  phout->SetXTitle(phin->GetXaxis()->GetTitle());
  phout->SetYTitle("Integrated efficiency");
  double sum = 0;
  int nbin = phin->GetNbinsX()+2;
  double fullsum = phin->Integral(0, nbin);
  if ( fullsum > 0 ) {
    for ( int i=0; i<nbin; ++i ) {
      sum += phin->GetBinContent(i)/fullsum;
      phout->SetBinContent(i, sum);
    }
  }
  return phout;
}

//**********************************************************************

// Create histogram with integrated Gaussian.

TH1F* gaussintlog(int nbin, double xmin, double xmax) {
  double eps = .001;
  gDirectory->Delete("hgaussintlog");
  TH1F* ph = new TH1F("hgaussintlog", "Integrated Gaussian", nbin, xmin, xmax);
  TF1* pfun = new TF1("fgauss", "exp(-0.5*x*x)");
  double* params = 0;
  double norm = pfun->Integral(0.0, 100, params, eps);
  double underflow = pfun->Integral(0.0, pow(10.0, xmin), params, eps )/norm;
  ph->SetBinContent(0, underflow);
  ph->SetBinContent(nbin+1, 1.0);
  for ( int i=0; i<nbin; ++i ) {
    int ibin = i + 1;
    double logval = ph->GetBinLowEdge(ibin+1);
    double arg = pow(10.0, logval);
    double val = 1.0;
    if ( arg < 10.0 ) {
      val = pfun->Integral(0.0, arg, params, eps)/norm;
    }
    ph->SetBinContent(ibin, val);
  }
  return ph;
}

//**********************************************************************

// Create histogram with integrated flat.

TH1F* flatintlog(int nbin, double xmin, double xmax) {
  double eps = .001;
  gDirectory->Delete("hflatlog");
  TH1F* ph = new TH1F("hflatlog", "Integrated Gaussian", nbin, xmin, xmax);
  double norm = sqrt(3.0);
  ph->SetBinContent(0, 0.0);
  ph->SetBinContent(nbin+1, 1.0);
  for ( int i=0; i<nbin; ++i ) {
    int ibin = i + 1;
    double logval = ph->GetBinLowEdge(ibin+1);
    double arg = pow(10.0, logval);
    double x2 = sqrt(3.0);
    double val = 0.0;
    if ( x2 > arg ) x2 = arg;
    val = x2/norm;
    ph->SetBinContent(ibin, val);
  }
  ph->SetLineStyle(3);
  return ph;
}

//**********************************************************************

// Resolution efficiency plot.

void reseff(bool measphi) {
  double lmax = 3;
  double lmin = -lmax;
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  int nbin = 60;
  if ( measphi ) {
    gDirectory->Delete("heff_phi");
    gDirectory->Delete("heff_phispoil");
    gDirectory->Delete("heff_phiunspoil");
    ph = new TH1F("heff_phi", "phi residual efficiency", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(yc-yh))>>heff_phi","abs(zh)<500 && wyc>0", "goff");
    phspoil = new TH1F("heff_phispoil", "phi residual spoiled efficiency", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(yc-yh))>>heff_phispoil","abs(zh)<500 && dyc>1", "goff");
    int n_unspoiled = heff_phi->GetEntries() - heff_phispoil->GetEntries();
    heff_phispoil->SetBinContent(nbin+1, n_unspoiled);
  } else {
    gDirectory->Delete("heff_r");
    gDirectory->Delete("heff_rspoil");
    gDirectory->Delete("heff_runspoil");
    ph = new TH1F("heff_r", "r residual efficiency", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(zc-zh))>>heff_r","abs(zh)<500", "goff");
    phspoil = new TH1F("heff_rspoil", "r residual spoiled efficiency", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(zc-zh))>>heff_rspoil","abs(zh)<500 && dzc>1", "goff");
    phunspoil = new TH1F("heff_runspoil", "r residual unspoiled efficiency", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(zc-zh))>>heff_runspoil","abs(zh)<500 && dzc<1", "goff");
    // Add spoiled to unspoled overflows and vv.
    int n_spoiled = heff_rspoil->GetEntries();
    int n_unspoiled = heff_runspoil->GetEntries();
    heff_rspoil->SetBinContent(nbin+1, n_unspoiled);
    heff_runspoil->SetBinContent(nbin+1, n_spoiled);
  }
  ph->SetXTitle("log10(abs(residual) in mm)");
  gStyle->SetOptStat(false);
  TH1F* phint = integrate(ph);
  TH1F* phint_spoiled = integrate(phspoil);
  //phint->SetFillColor(31);
  phint->SetFillColor(33);
  phint->SetFillStyle(3001);
  phint_spoiled->SetFillColor(46);
  phint_spoiled->SetFillStyle(3001);
  phint->Draw("B");
  gPad->SetGridx();
  gPad->SetGridy();
  phint_spoiled->Draw("B SAME");
}

//**********************************************************************

// Pull plot.

void sigeff(bool measphi, string draw ="both", int istation =0, int width =0) {
  double lmax = 2;
  double lmin = -lmax;
  int nbin = 60;
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  TH1F* phunspoil = 0;
  if ( measphi ) {
    gDirectory->Delete("hsig_phi");
    gDirectory->Delete("hsig_phispoil");
    gDirectory->Delete("hsig_phiunspoil");
    ph = new TH1F("hsig_phi", "phi residual pull", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(yc-yh)/dyc)>>hsig_phi","abs(zh)<500 && wyc>0", "goff");
    string test_spoiled = "abs(zh)<500 && dyc>1";
    string test_unspoiled = "abs(zh)<500 && dyc<1";
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
    cscperf()->Draw("log10(abs(yc-yh)/dyc)>>hsig_phispoil", test_spoiled.c_str(), "goff");
    cscperf()->Draw("log10(abs(yc-yh)/dyc)>>hsig_phiunspoil", test_unspoiled.c_str(), "goff");
    int n_spoiled = hsig_phispoil->GetEntries();
    int n_unspoiled = hsig_phispoil->GetEntries();
    if ( draw == "both" ) hsig_phispoil->SetBinContent(nbin+1, n_unspoiled);
    if ( draw == "both" ) hsig_phiunspoil->SetBinContent(nbin+1, n_spoiled);
  } else {
    gDirectory->Delete("hsig_r");
    gDirectory->Delete("hsig_rspoil");
    gDirectory->Delete("hsig_runspoil");
    ph = new TH1F("hsig_r", "r residual pull", nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(zc-zh)/dzc)>>hsig_r","abs(zh)<500", "goff");
    string test_spoiled = "abs(zh)<500 && dzc>1";
    string test_unspoiled = "abs(zh)<500 && dzc<1";
    string title_spoiled = "r residual spoiled pull";
    string title_unspoiled = "r residual unspoiled pull";
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
    phspoil = new TH1F("hsig_rspoil", title_spoiled.c_str(), nbin, lmin, lmax);
    phunspoil = new TH1F("hsig_runspoil", title_unspoiled.c_str(), nbin, lmin, lmax);
    cscperf()->Draw("log10(abs(zc-zh)/dzc)>>hsig_rspoil", test_spoiled.c_str(), "goff");
    cscperf()->Draw("log10(abs(zc-zh)/dzc)>>hsig_runspoil", test_unspoiled.c_str(), "goff");
    // Add spoiled to unspoled overflows and vv.
    int n_spoiled = hsig_rspoil->GetEntries();
    int n_unspoiled = hsig_runspoil->GetEntries();
    if ( draw == "both" ) hsig_rspoil->SetBinContent(nbin+1, n_unspoiled);
    if ( draw == "both" ) hsig_runspoil->SetBinContent(nbin+1, n_spoiled);
  }
  ph->SetXTitle("log10(abs(residual)/error)");
  gStyle->SetOptStat(false);
  TH1F* phint = integrate(ph);
  TH1F* phint_spoiled = integrate(phspoil);
  TH1F* phint_unspoiled = integrate(phunspoil);
  //phint->SetFillColor(31);
  phint->SetFillColor(33);
  phint->SetFillStyle(3001);
  phint_spoiled->SetFillColor(46);
  phint_spoiled->SetFillStyle(3001);
  phint_unspoiled->SetFillColor(30);
  phint_unspoiled->SetFillStyle(3001);
  phint->GetYaxis()->SetRangeUser(0.0, 1.02);
  phint_spoiled->GetYaxis()->SetRangeUser(0.0, 1.02);
  phint_unspoiled->GetYaxis()->SetRangeUser(0.0, 1.02);
  if ( draw = = "both" ) {
    phint->Draw("B");
    phint_spoiled->Draw("B SAME");
  } else if ( draw == "spoiled" ) {
    phint_spoiled->Draw("B");
  } else if ( draw == "unspoiled" ) {
    phint_unspoiled->Draw("B");
  }
  gPad->SetGridx();
  gPad->SetGridy();
  gaussintlog(nbin, lmin, lmax)->Draw("same");
  flatintlog(nbin, lmin, lmax)->Draw("same");
}

//**********************************************************************

// RMS vs pull.

void rmssig(int measphi) {
  if ( measphi ) {
    cscperf()->Draw("ryc:abs(yc-yh)/dyc", "abs(zh)<500 && wyc>0 && dyc<1 && abs(yc-yh)/dyc>1");
  } else {
    cscperf()->Draw("rzc:abs(zc-zh)/dzc", "abs(zh)<500 && wzc>0 && dzc<1 && abs(zc-zh)/dzc>1");
  }
}

//**********************************************************************

// Resolution plot.

void resplot(bool measphi, double lim, bool showspoil =true) {
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  int nbin = 100;
  if ( measphi ) {
    gDirectory->Delete("hres_phi");
    gDirectory->Delete("hres_phispoil");
    gDirectory->Delete("hres_phiunspoil");
    ph = new TH1F("hres_phi", "phi resolution", nbin, -lim, lim);
    ostringstream sscut;
    sscut << "abs(zh)<500 && wyc>0 && abs(yc-yh)<" << lim;
    cscperf()->Draw("yc-yh>>hres_phi", sscut.str().c_str(), "goff");
    sscut << " && dyc>1";
    phspoil = new TH1F("hres_phi_spoiled", "spoiled phi resolution", nbin, -lim, lim);
    cscperf()->Draw("yc-yh>>hres_phi_spoiled", sscut.str().c_str(), "goff");
  } else {
    gDirectory->Delete("hres_r");
    gDirectory->Delete("hres_rspoil");
    gDirectory->Delete("hres_runspoil");
    ph = new TH1F("hres_r", "r resolution", nbin, -lim, lim);
    ostringstream sscut;
    sscut << "abs(zh)<500 && wzc>0 && abs(zc-zh)<" << lim;
    cscperf()->Draw("zc-zh>>hres_r", sscut.str().c_str(), "goff");
    sscut << " && dzc>1";
    phspoil = new TH1F("hres_r_spoiled", "spoiled r resolution", nbin, -lim, lim);
    cscperf()->Draw("zc-zh>>hres_r_spoiled", sscut.str().c_str(), "goff");
  }
  ph->SetFillColor(33);
  ph->SetFillStyle(3001);
  ph->Draw("B");
  ph->SetXTitle("residual (mm)");
  if ( showspoil ) {
    phspoil->SetFillColor(46);
    phspoil->SetFillStyle(3001);
    phspoil->Draw("B SAME");
  }
}

//**********************************************************************

// Pull plot.

void sigplot(bool measphi, double lim, bool showspoil =true) {
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  TH1F* phunspoil = 0;
  int nbin = 100;
  if ( measphi ) {
    gDirectory->Delete("hsig_phi");
    gDirectory->Delete("hsig_phispoil");
    gDirectory->Delete("hsig_phiunspoil");
    ph = new TH1F("hsig_phi", "phi pull", nbin, -lim, lim);
    ostringstream sscut;
    ostringstream ssuncut;
    sscut << "abs(zh)<500 && wyc>0 && abs(yc-yh)/dyc<" << lim;
    cscperf()->Draw("(yc-yh)/dyc>>hsig_phi", sscut.str().c_str(), "goff");
    ssuncut << sscut.str();
    sscut << " && dyc>1";
    ssuncut << " && dyc<1";
    phspoil = new TH1F("hsig_phi_spoiled", "spoiled phi pull", nbin, -lim, lim);
    phunspoil = new TH1F("hsig_phi_unspoiled", "unspoiled phi pull", nbin, -lim, lim);
    cscperf()->Draw("(yc-yh)/dyc>>hsig_phi_spoiled", sscut.str().c_str(), "goff");
    cscperf()->Draw("(yc-yh)/dyc>>hsig_phi_unspoiled", ssuncut.str().c_str(), "goff");
  } else {
    gDirectory->Delete("hsig_r");
    gDirectory->Delete("hsig_rspoil");
    gDirectory->Delete("hsig_runspoil");
    ph = new TH1F("hsig_r", "r pull", nbin, -lim, lim);
    ostringstream sscut;
    ostringstream ssuncut;
    sscut << "abs(zh)<500 && wzc>0 && abs(zc-zh)/dzc<" << lim;
    cscperf()->Draw("(zc-zh)/dzc>>hsig_r", sscut.str().c_str(), "goff");
    ssuncut << sscut.str();
    sscut << " && dzc>1";
    ssuncut << " && dzc<1";
    phspoil = new TH1F("hsig_r_spoiled", "spoiled r pull", nbin, -lim, lim);
    phunspoil = new TH1F("hsig_r_unspoiled", "unspoiled r pull", nbin, -lim, lim);
    cscperf()->Draw("(zc-zh)/dzc>>hsig_r_spoiled", sscut.str().c_str(), "goff");
    cscperf()->Draw("(zc-zh)/dzc>>hsig_r_unspoiled", ssuncut.str().c_str(), "goff");
  }
  ph->SetFillColor(33);
  ph->SetFillStyle(3001);
  phspoil->SetFillColor(46);
  phspoil->SetFillStyle(3001);
  phunspoil->SetFillColor(30);
  phunspoil->SetFillStyle(3001);
  ph->Draw("B");
  ph->SetXTitle("pull (residual/error)");
  if ( showspoil ) {
    phspoil->Draw("B SAME");
  }
}

//**********************************************************************
*/
