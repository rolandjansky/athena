/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TMath.h" // for TMath::Prob()

seg4perf_help() {
  std::cout << "  seg4perf() - returns 4D segperf tree" << std::endl;
  std::cout << "  effs4() - 4D performance summary" << std::endl;
  std::cout << std::endl;
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
