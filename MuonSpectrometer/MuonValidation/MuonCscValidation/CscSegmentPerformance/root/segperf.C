/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "TMath.h" // for TMath::Prob()

segperf_help() {
  std::cout << "  effs() - performance summary" << std::endl;
  std::cout << "  segperf() - returns segperf tree" << std::endl;
  std::cout << "  resplot(measphi, angle, lim, showspoil) - resolution" << std::endl;
  std::cout << "  sigplot(measphi, angle, lim, showspoil) - pull" << std::endl;
}

//**********************************************************************

// Return the simpos tree.

TTree* segperf() {
  return dynamic_cast<TTree*>(gDirectory->Get("csc_segperf"));
}

//**********************************************************************

string sstation(int istation) {
  if ( istation == 1 ) return "CSS";
  if ( istation == 2 ) return "CSL";
  return "Unknown station";
}

string smeasphi(int measphi) {
  if ( measphi ) return "phi";
  return "eta";
}

//**********************************************************************

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
  if ( azh == 0.0 ) return false;
  double m = 1.0/tan(azh);
  double p1 = zh + m*xw4;
  double p2 = zh + m*xw1;
  return fabs(p1) < maxpos(istation, false) &&
         fabs(p2) < maxpos(istation, false);
}
bool siminy(int istation, double yh, double ayh) {
  if ( ayh == 0.0 ) return false;
  double m = 1.0/tan(ayh);
  double p1 = yh + m*xw4;
  double p2 = yh + m*xw1;
  return fabs(p1) < maxpos(istation, true) &&
         fabs(p2) < maxpos(istation, true);
}
bool simin(int istation, double yh, double zh, double ayh, double azh) {
  return siminy(istation, yh, ayh) && siminz(istation, zh, azh);
}

double floatrat(double num, double den) {
  if ( den == 0.0 ) return 0.0;
  return num/den;
}

//**********************************************************************

// Cuts.

// Cut to select truth.
TCut segperf_cut_truth_eta = "statz==0 || statz==1";
TCut segperf_cut_truth_phi = "staty==0 || staty==1";

// Aceptance cuts for truth muons.
TCut segperf_cut_acc_eta = "siminz(istation, zh, azh)";
TCut segperf_cut_acc_phi = "siminy(istation, yh, ayh)";
TCut segperf_cut_acc =     "simin(istation, yh, zh, ayh, azh)";

// Kinematic cut for truth muons.
TCut segperf_cut_kine = "pt>2000";

// Truth + Acceptance + kinematics.
TCut segperf_cut_ref_eta = segperf_cut_truth_eta +segperf_cut_kine + segperf_cut_acc_eta;
TCut segperf_cut_ref_phi = segperf_cut_truth_phi + segperf_cut_kine + segperf_cut_acc_phi;
TCut segperf_cut_ref     = segperf_cut_kine + segperf_cut_acc;

// Cut to flag found.
TCut segperf_cut_found_eta = "statz==0";
TCut segperf_cut_found_phi = "staty==0";

// Cuts to flag spoiled and unspoiled found.
TCut segperf_cut_spoiled_eta   = "statz==0 && nunspoilz<2";
TCut segperf_cut_unspoiled_eta = "statz==0 && nunspoilz>=2";
TCut segperf_cut_spoiled_phi   = "staty==0 && nunspoily<2";
TCut segperf_cut_unspoiled_phi = "staty==0 && nunspoily>=2";

// Cut to flag found good or bad.
TCut segperf_cut_good_eta = "statz==0 && drecz<=3.7";
TCut segperf_cut_bad_eta  = "statz==0 && drecz>3.7";
TCut segperf_cut_good_phi = "staty==0 && drecy<=3.7";
TCut segperf_cut_bad_phi  = "staty==0 && drecy>3.7";

// Cut to flag reco.
TCut segperf_cut_reco_eta = "statz==0 || statz==2";
TCut segperf_cut_reco_phi = "staty==0 || staty==2";

// Cuts to flag fake.
TCut segperf_cut_fake_eta   = "statz==2";
TCut segperf_cut_fake_phi   = "staty==2";

//**********************************************************************

// Display metrics.

void effs(ostream* pout =0) {
  if ( pout == 0 ) pout = &cout;
  pout->setf(ios_base::fixed);
  pout->precision(3);
  int w1 = 7;
  int w2 = 8;
  // Event count.
  segperf()->Draw("evt>>hevt", "evt", "goff");
  int nevt = hevt->GetEntries();
  *pout << "         # events: " << setw(w1)  << nevt << endl << endl;
  TTree* ptree = segperf();
  // Eta results.
  TCut simfound = segperf_cut_ref_eta + segperf_cut_found_eta;
  ptree->Draw("z-zh>>hzreco",  segperf_cut_reco_eta, "goff");
  ptree->Draw("z-zh>>hzreal",  segperf_cut_found_eta, "goff");
  ptree->Draw("z-zh>>hzfake",  segperf_cut_fake_eta, "goff");
  ptree->Draw("zh>>hznocut",   segperf_cut_truth_eta, "goff");
  ptree->Draw("zh>>hzkin",     segperf_cut_truth_eta + segperf_cut_kine, "goff");
  ptree->Draw("zh>>hzacc",     segperf_cut_truth_eta + segperf_cut_acc_eta, "goff");
  ptree->Draw("z-zh>>hzall",   segperf_cut_ref_eta, "goff");
  ptree->Draw("z-zh>>hzfound", segperf_cut_ref_eta + segperf_cut_found_eta, "goff");
  ptree->Draw("z-zh>>hzgood",  segperf_cut_ref_eta + segperf_cut_good_eta, "goff");
  ptree->Draw("z-zh>>hzbad",   segperf_cut_ref_eta + segperf_cut_bad_eta, "goff");
  ptree->Draw("nunspoilz>>hzspoil",   segperf_cut_ref_eta + segperf_cut_spoiled_eta, "goff");
  int nzreco  = hzreco->GetEntries();
  int nzreal  = hzreal->GetEntries();
  int nzfake  = hzfake->GetEntries();
  int nznocut = hznocut->GetEntries();
  int nzall   = hzall->GetEntries();
  int nzkin   = hzkin->GetEntries();
  int nzacc   = hzacc->GetEntries();
  int nzfound = hzfound->GetEntries();
  int nzgood  = hzgood->GetEntries();
  int nzbad   = hzbad->GetEntries();
  int nzspoil = hzspoil->GetEntries();
  *pout << "   eta reco/event: " << setw(w1) <<  nzreco << setw(w2) << floatrat(nzreco,nevt) << endl;
  *pout << "   eta real/event: " << setw(w1) <<  nzreal << setw(w2) << floatrat(nzreal,nevt) << endl;
  *pout << "   eta fake/event: " << setw(w1) <<  nzfake << setw(w2) << floatrat(nzfake,nevt) << endl;
  *pout << "   eta candidates: " << setw(w1) << nznocut << endl;
  *pout << "  kinematical acc: " << setw(w1) <<   nzkin << setw(w2) << floatrat(nzkin,nznocut)
        << "    (" << segperf_cut_kine.GetTitle() << ")" << endl;
  *pout << "  geometrical acc: " << setw(w1) <<   nzacc << setw(w2) << floatrat(nzacc,nznocut) << endl;
  *pout << "     combined acc: " << setw(w1) <<   nzall << setw(w2) << floatrat(nzall,nznocut) << endl;
  *pout << "       efficiency: " << setw(w1) << nzfound << setw(w2) << floatrat(nzfound,nzall) << endl;
  *pout << "  good efficiency: " << setw(w1) <<  nzgood << setw(w2) << floatrat(nzgood,nzall)
        << "    (" << segperf_cut_good_eta.GetTitle() << ")" << endl;
  *pout << "     bad fraction: " << setw(w1) <<   nzbad << setw(w2) << floatrat(nzbad,nzfound)
        << "    (" << segperf_cut_bad_eta.GetTitle() << ")" << endl;
  *pout << "          spoiled: " << setw(w1) << nzspoil << setw(w2) << floatrat(nzspoil,nzfound)
        << "    (" << segperf_cut_spoiled_eta.GetTitle() << ")" << endl;
  // Phi results.
  *pout << endl;
  simfound = segperf_cut_ref_phi + segperf_cut_found_phi;
  ptree->Draw("y-yh>>hyreco",  segperf_cut_reco_phi, "goff");
  ptree->Draw("y-yh>>hyreal",  segperf_cut_found_phi, "goff");
  ptree->Draw("y-yh>>hyfake",  segperf_cut_fake_phi, "goff");
  ptree->Draw("yh>>hynocut",   segperf_cut_truth_phi, "goff");
  ptree->Draw("yh>>hykin",     segperf_cut_truth_phi + segperf_cut_kine, "goff");
  ptree->Draw("yh>>hyacc",     segperf_cut_truth_phi + segperf_cut_acc_phi, "goff");
  ptree->Draw("y-yh>>hyall",   segperf_cut_ref_phi, "goff");
  ptree->Draw("y-yh>>hyfound", segperf_cut_ref_phi + segperf_cut_found_phi, "goff");
  ptree->Draw("y-yh>>hygood",  segperf_cut_ref_phi + segperf_cut_good_phi, "goff");
  ptree->Draw("y-yh>>hybad",   segperf_cut_ref_phi + segperf_cut_bad_phi, "goff");
  ptree->Draw("nunspoily>>hyspoil",   segperf_cut_ref_phi + segperf_cut_spoiled_phi, "goff");
  int nyreco  = hyreco->GetEntries();
  int nyfake  = hyfake->GetEntries();
  int nyreal  = hyreal->GetEntries();
  int nynocut = hynocut->GetEntries();
  int nyall   = hyall->GetEntries();
  int nykin   = hykin->GetEntries();
  int nyacc   = hyacc->GetEntries();
  int nyfound = hyfound->GetEntries();
  int nygood  = hygood->GetEntries();
  int nybad   = hybad->GetEntries();
  int nyspoil = hyspoil->GetEntries();
  *pout << "   phi reco/event: " << setw(w1) <<  nyreco << setw(w2) << floatrat(nyreco,nevt) << endl;
  *pout << "   phi real/event: " << setw(w1) <<  nyreal << setw(w2) << floatrat(nyreal,nevt) << endl;
  *pout << "   phi fake/event: " << setw(w1) <<  nyfake << setw(w2) << floatrat(nyfake,nevt) << endl;
  *pout << "   phi candidates: " << setw(w1) << nynocut << endl;
  *pout << "  kinematical acc: " << setw(w1) <<   nykin << setw(w2) << floatrat(nykin,nynocut)
        << "    (" << segperf_cut_kine.GetTitle() << ")" << endl;
  *pout << "  geometrical acc: " << setw(w1) <<   nyacc << setw(w2) << floatrat(nyacc,nynocut) << endl;
  *pout << "     combined acc: " << setw(w1) <<   nyall << setw(w2) << floatrat(nyall,nynocut) << endl;
  *pout << "       efficiency: " << setw(w1) << nyfound << setw(w2) << floatrat(nyfound,nyall) << endl;
  *pout << "  good efficiency: " << setw(w1) <<  nygood << setw(w2) << floatrat(nygood,nyall)
        << "    (" << segperf_cut_good_phi.GetTitle() << ")" << endl;
  *pout << "     bad fraction: " << setw(w1) <<   nybad << setw(w2) << floatrat(nybad,nyfound)
        << "    (" << segperf_cut_bad_phi.GetTitle() << ")" << endl;
  *pout << "          spoiled: " << setw(w1) << nyspoil << setw(w2) << floatrat(nyspoil,nyfound)
        << "    (" << segperf_cut_spoiled_phi.GetTitle() << ")" << endl;
}

//**********************************************************************

// Efficiencies.

// Display efficiencies for
//   finding
//   reconstructing (drec<5).
//   spoiled (nspoil>1)

void oldeffs(ostream* pout =0) {
  if ( pout == 0 ) pout = &cout;
  string sgood = "3.7";
  int rptot[2] = {0,0};
  int rpfnd[2] = {0,0};
  int rprec[2] = {0,0};
  int rpspo[2] = {0,0};
  int can[2][3];
  double efind[2][3];
  double egood[2][3];
  double spoil[2][3];
  int iplane;
  for ( int measphi=0; measphi<2; ++measphi ) {
    for ( int istation=1; istation<3; ++istation ) {
      ostringstream sscut;
      string c = measphi ? "y" : "z";
      string c2 = measphi ? "phi" : "eta";
      sscut << "istation==" << istation << " && simin" << c
            << "(istation, " << c << "h, a" << c << "h)";
      string scut = sscut.str();
      string scuttot = scut + " && stat" + c + "<2";
      string scutfnd = scut + " && stat" + c + "==0";
      string scutrec = scutfnd + " && drec" + c + "<" + sgood;
      string scutspo = scuttot + " && nspoil" + c + ">1";
      string sdraw = c + "h";
      int tot = segperf()->Draw(sdraw.c_str(), scuttot.c_str(), "goff");
      int fnd = segperf()->Draw(sdraw.c_str(), scutfnd.c_str(), "goff");
      int rec = segperf()->Draw(sdraw.c_str(), scutrec.c_str(), "goff");
      int spo = segperf()->Draw(sdraw.c_str(), scutspo.c_str(), "goff");
      rptot[measphi] += tot;
      rpfnd[measphi] += fnd;
      rprec[measphi] += rec;
      rpspo[measphi] += spo;
      can[measphi][istation] = tot;
      double xefind = 0.0;
      double xegood = 0.0;
      double xspoil = 0.0;
      if ( tot ) {
        xefind = double(fnd)/tot;
        xegood = double(rec)/tot;
      }
      if ( fnd ) {
        xspoil = double(spo)/fnd;
      }
      efind[measphi][istation] = xefind;
      egood[measphi][istation] = xegood;
      spoil[measphi][istation] = xspoil;
    }
  }
  for ( int measphi=0; measphi<2; ++measphi ) {
    int tot = rptot[measphi];
    int fnd = rpfnd[measphi];
    int rec = rprec[measphi];
    int spo = rpspo[measphi];
    double xefind = 0.0;
    double xegood = 0.0;
    double xspoil = 0.0;
    can[measphi][0] = tot;
    if ( tot ) {
      xefind = double(fnd)/tot;
      xegood = double(rec)/tot;
    }
    if ( fnd ) {
      xspoil = double(spo)/fnd;
    }
    efind[measphi][0] = xefind;
    egood[measphi][0] = xegood;
    spoil[measphi][0] = xspoil;
  }
  pout->setf(ios_base::fixed);
  pout->precision(3);
  // Get the event count and fake rate.
  // ...truth
  TH1F* phntruth = new TH1F("ntruth", "ntruth", 100, 0, 100);
  segperf()->Draw("nentry>>ntruth", "", "goff");
  int nevt = phntruth->GetEntries();
  double mean_ntruth = phntruth->GetMean();
  double mean_ntruth_err = phntruth->GetMeanError();
  // ...fake
  TH1F* phfakey = new TH1F("fakesy", "fakesy", 100, 0, 100);
  segperf()->Draw("nfakey>>fakesy", "", "goff");
  double fakey = phfakey->GetMean();
  double fakeyerr = phfakey->GetMeanError();
  TH1F* phfakez = new TH1F("fakesz", "fakesz", 100, 0, 100);
  segperf()->Draw("nfakez>>fakesz", "", "goff");
  double fakez = phfakez->GetMean();
  double fakezerr = phfakez->GetMeanError();
  // ...found
  TH1F* phfoundy = new TH1F("foundy", "foundy", 100, 0, 100);
  segperf()->Draw("staty>>foundy", "staty!=1", "goff");
  int nfoundy = phfoundy->GetEntries();
  double mean_foundy = double(nfoundy)/nevt;
  double foundyerr = sqrt(double(nfoundy))/nevt;
  TH1F* phfoundz = new TH1F("foundz", "foundz", 100, 0, 100);
  segperf()->Draw("statz>>foundz", "statz!=1", "goff");
  int nfoundz = phfoundz->GetEntries();
  double mean_foundz = double(nfoundz)/nevt;
  double foundzerr = sqrt(double(nfoundz))/nevt;
  // ...show
  int w1 = 6;
  *pout << endl;
  *pout << "                 # events: " << setw(w1) << nevt << endl;
  *pout << "         # truth segments: " << setw(w1) << int(nevt*mean_ntruth) << endl;
  *pout << "     # found eta segments: " << setw(w1) << nfoundz << endl;
  *pout << "     # found phi segments: " << setw(w1) << nfoundy << endl;
  *pout << "      # fake eta segments: " << setw(w1) << int(nevt*fakez) << endl;
  *pout << "      # fake phi segments: " << setw(w1) << int(nevt*fakey) << endl;
  *pout << "     truth segments/event: " << mean_ntruth << " +/- " << mean_ntruth_err <<  endl;
  *pout << " found eta segments/event: " << mean_foundz << " +/- " << foundzerr <<  endl;
  *pout << " found phi segments/event: " << mean_foundy << " +/- " << foundyerr <<  endl;
  *pout << "  fake eta segments/event: " << fakez << " +/- " << fakezerr <<  endl;
  *pout << "  fake phi segments/event: " << fakey << " +/- " << fakeyerr <<  endl;
  // Show acceptance efficiencies.
  string sp1 = "   ";
  string sp2 = "  ";
  int w1 = 8;
  int w2 = 7;
  *pout << "                                   CSS            CSL          CSS+CSL" << endl;
  *pout << "                                eta    phi     eta    phi     eta    phi" << endl;
  *pout << "With acceptance cuts:        ------ ------   -----  -----   -----  -----" << endl;
  *pout << "            # true segments";
  *pout << setw(w1) << can[0][1] << setw(w2) << can[1][1];
  *pout << setw(w1) << can[0][2] << setw(w2) << can[1][2];
  *pout << setw(w1) << can[0][0] << setw(w2) << can[1][0];
  *pout << endl;
  *pout << "                 efficiency";
  *pout << sp1 << efind[0][1] << sp2 << efind[1][1];
  *pout << sp1 << efind[0][2] << sp2 << efind[1][2];
  *pout << sp1 << efind[0][0] << sp2 << efind[1][0];
  *pout << endl;
  *pout << "           egood (deva<"+ sgood + ")";
  *pout << sp1 << egood[0][1] << sp2 << egood[1][1];
  *pout << sp1 << egood[0][2] << sp2 << egood[1][2];
  *pout << sp1 << egood[0][0] << sp2 << egood[1][0];
  *pout << endl;
  *pout << " spoil (2 or more clusters)";
  *pout << sp1 << spoil[0][1] << sp2 << spoil[1][1];
  *pout << sp1 << spoil[0][2] << sp2 << spoil[1][2];
  *pout << sp1 << spoil[0][0] << sp2 << spoil[1][0];
  *pout << endl;
  // Reset output stream state.
  pout->setf(0);
  pout->precision(6);
}

//**********************************************************************

// Resolution plot.

void resplot(bool measphi, bool angle, double lim, bool fit =true, bool showspoil =true) {
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(1);
  int nbin = 100;
  string hname = "hres_";
  if ( angle ) hname += "a";
  string c = measphi ? "y" : "z";
  string a = angle ? "a" : "";
  string splim = measphi ? "2" : "2";
  hname += measphi ? "phi" : "eta";
  hname_spoil = hname + "_spoiled";
  hname_unspoil = hname + "_unspoiled";
  string var = a + c + "-" + a + c  + "h";
  string draw = var + ">>" + hname;
  string draw_spoil = var + ">>" + hname_spoil;
  string draw_unspoil = var + ">>" + hname_unspoil;
  string title = "CSC ";
  title += measphi ? "phi" : "eta";
  title += " segment ";
  if ( angle == 0 ) title + = " position";
  else if ( angle ) title + = " angle";
  else {
    cerr << "Invalid angle: " << angle << endl;
    return;
  }
  title += " resolution";
  title_spoil = title + " spoiled";
  title_unspoil = title + " unspoiled";
  string c = measphi ? "y" : "z";
  ostringstream sslim;
  sslim << lim;
  string scutlim = "abs(" + var + ")<" + sslim.str();
  TCut cutlim = scutlim.c_str();
  TCut cut_found = segperf_cut_ref_eta + segperf_cut_found_eta + cutlim;
  TCut cut_spoil = segperf_cut_ref_eta + segperf_cut_spoiled_eta + cutlim;
  TCut cut_unspoil = segperf_cut_ref_eta + segperf_cut_unspoiled_eta + cutlim;
  if ( measphi ) {
    cut_found = segperf_cut_ref_phi + segperf_cut_found_phi + cutlim;
    cut_spoil = segperf_cut_ref_phi + segperf_cut_spoiled_eta + cutlim;
    cut_unspoil = segperf_cut_ref_phi + segperf_cut_unspoiled_eta + cutlim;
  }
  cout << "Var: " << var << endl;
  cout << "Cut: " << cut_found.GetTitle() << endl;
  cout << "Spoil limit: " << splim << endl;
  cout << "Histo names: " << hname << " " << hname_spoil
       << " " << hname_unspoil << endl;
  TH1F* ph = new TH1F(hname.c_str(), title.c_str(), nbin, -lim, lim);
  segperf()->Draw(draw.c_str(), cut_found, "goff");
  TH1F* phspoil = new
    TH1F(hname_spoil.c_str(), title_spoil.c_str(), nbin, -lim, lim);
  segperf()->Draw(draw_spoil.c_str(), cut_spoil, "goff");
  TH1F* phunspoil = new
    TH1F(hname_unspoil.c_str(), title_unspoil.c_str(), nbin, -lim, lim);
  segperf()->Draw(draw_unspoil.c_str(), cut_unspoil, "goff");
  ph->SetFillColor(33);
  ph->SetFillStyle(3001);
  phspoil->SetMinimum(0.0);
  phspoil->SetMaximum(1.0);
  if ( fit ) ph->Fit("gaus");
  ph->Draw("");
  if ( angle ) {
    ph->SetXTitle("residual (rad)");
  } else {
    ph->SetXTitle("residual (mm)");
  }
  if ( showspoil ) {
    phspoil->SetFillColor(46);
    phspoil->SetFillStyle(3001);
    phspoil->Draw("SAME");
  }
}

//**********************************************************************

// Pull plot.
// angle = 0 for coordinate
//         1 for angle
//         2 for evaluation distance
//         3 for truth chi-square probaility

void sigplot(bool measphi, int angle, double lim, int isplim =2,
             bool fit =true, bool showspoil =true) {
  gStyle->SetOptStat(1111);
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
  if ( angle == 2 ) var = "drec" + c;
  if ( angle == 3 ) var = "TMath::Prob(drec" + c + "*drec" + c + ",2)";
  string draw = var + ">>" + hname;
  string draw_spoil = var + ">>" + hname_spoil;
  string draw_unspoil = var + ">>" + hname_unspoil;
  string title = "CSC ";
  title += measphi ? "phi" : "eta";
  title += " segment ";
  if ( angle == 0 ) title + = "position";
  if ( angle == 1 ) title + = "angle";
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
  string scutlim = "abs(" + var + ")<" + sslim.str();
  TCut cutlim = scutlim.c_str();
  TCut cut_found = segperf_cut_ref_eta + segperf_cut_found_eta + cutlim;
  TCut cut_spoil = segperf_cut_ref_eta + segperf_cut_spoiled_eta + cutlim;
  TCut cut_unspoil = segperf_cut_ref_eta + segperf_cut_unspoiled_eta + cutlim;
  if ( measphi ) {
    cut_found = segperf_cut_ref_phi + segperf_cut_found_phi + cutlim;
    cut_spoil = segperf_cut_ref_phi + segperf_cut_spoiled_phi + cutlim;
    cut_unspoil = segperf_cut_ref_phi + segperf_cut_unspoiled_phi + cutlim;
  }
  cout << "Var: " << var << endl;
  cout << "Cut: " << cut_found.GetTitle() << endl;
  cout << "Cut spoil: " << cut_spoil.GetTitle() << endl;
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
  segperf()->Draw(draw.c_str(), cut_found, "goff");
  TH1F* phspoil = new
    TH1F(hname_spoil.c_str(), title_spoil.c_str(), nbin, lmin, lmax);
  segperf()->Draw(draw_spoil.c_str(), cut_spoil, "goff");
  TH1F* phunspoil = new
    TH1F(hname_unspoil.c_str(), title_unspoil.c_str(), nbin, lmin, lmax);
  segperf()->Draw(draw_unspoil.c_str(), cut_unspoil, "goff");
  ph->SetFillColor(33);
  ph->SetFillStyle(3001);
  ph->SetMinimum(0);
  if ( angle >= 2 ) fit = false;
  if ( fit ) ph->Fit("gaus");
  ph->Draw("");
  if ( angle < 2 ) {
    ph->SetXTitle("residual/error");
  } else if ( angle == 2 ) {
    ph->SetXTitle("sqrt(truth chi-square)");
  } else if ( angle == 3 ) {
    ph->SetXTitle("chi-square probability");
  }
  if ( showspoil ) {
    phspoil->SetFillColor(46);
    phspoil->SetFillStyle(3001);
    phspoil->Draw("SAME");
  }
}

//**********************************************************************

// Resolution efficiency plot.

void reseff(bool measphi, bool angle, bool truthnorm =true) {
  double lmax = 3;
  if ( angle ) lmax = 1;
  double lmin = lmax - 6;
  TH1F* phall = 0;
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  int nbin = 60;
  string sangle = "position";
  if ( angle ) sangle = "angle";
  string hname = "heff_" + smeasphi(measphi) + "_" + sangle;
  string title = smeasphi(measphi) + " " + sangle;
  string title_spoil = title + " spoiled efficiency";
  title += " efficiency";
  string hname_spoil = hname + "_spoil";
  string hname_unspoil = hname + "_unspoil";
  string pvar = "z";
  if ( measphi ) pvar = "y";
  string var = "";
  if ( angle ) var += "a";
  if ( measphi ) var += "y";
  else var += pvar;
  string truth_var = pvar + "h";
  string sarg = "log10(abs(" + var + "-" + var + "h))>>";
  string sarg_spoil = sarg + hname_spoil;
  sarg += hname;
  TCut cut_all = segperf_cut_ref_eta;
  TCut cut_found = segperf_cut_ref_eta + segperf_cut_found_eta;
  TCut cut_spoil = segperf_cut_ref_eta + segperf_cut_spoiled_eta;
  TCut cut_unspoil = segperf_cut_ref_eta + segperf_cut_unspoiled_eta;
  if ( measphi ) {
    cut_all = segperf_cut_ref_phi;
    cut_found = segperf_cut_ref_phi + segperf_cut_found_phi;
    cut_spoil = segperf_cut_ref_phi + segperf_cut_spoiled_phi;
    cut_unspoil = segperf_cut_ref_phi + segperf_cut_unspoiled_phi;
  }
  gDirectory->Delete(hname.c_str());
  gDirectory->Delete(hname_spoil.c_str());
  ph = new TH1F(hname.c_str(), title.c_str(), nbin, lmin, lmax);
  segperf()->Draw(sarg.c_str(), cut_found, "goff");
  phspoil = new TH1F(hname_spoil.c_str(), title_spoil.c_str(), nbin, lmin, lmax);
  segperf()->Draw(sarg_spoil.c_str(), cut_spoil, "goff");
  if ( truthnorm ) {
    phall = new TH1F("truth_eta", "Truth eta", nbin, lmin, lmax);
    segperf()->Draw("eta>>truth_eta", cut_all, "goff");
    ph->SetBinContent(nbin+1, phall->GetEntries() - ph->GetEntries());
    phspoil->SetBinContent(nbin+1, phall->GetEntries() - phspoil->GetEntries());
    string app = " (truth normalization)";
    string title = ph->GetTitle() + app;
    ph->SetTitle(title.c_str());
    title = phspoil->GetTitle() + app;
    phspoil->SetTitle(title.c_str());
  } else {
    phspoil->SetBinContent(nbin+1, ph->GetEntries() - phspoil->GetEntries());
  }
  if ( angle ) {
    ph->SetXTitle("log10(abs(residual) in rad)");
  } else {
    ph->SetXTitle("log10(abs(residual) in mm)");
  }
  gStyle->SetOptStat(false);
  TH1F* phint = integrate(ph);
  TH1F* phint_spoiled = integrate(phspoil);
  //phint->SetFillColor(31);
  phint->SetFillColor(33);
  phint->SetFillStyle(3001);
  phint_spoiled->SetFillColor(46);
  phint_spoiled->SetFillStyle(3001);
  phint->SetMinimum(0.0);
  phint->SetMaximum(1.0);
  phint->Draw("");
  gPad->SetGridx();
  gPad->SetGridy();
  phint_spoiled->Draw("SAME");
}

//**********************************************************************

// Pull efficiency plot.

void sigeff(bool measphi, bool angle, bool truthnorm =false) {
  double lmax = 3;
  double lmin = -3;
  TH1F* phall = 0;
  TH1F* ph = 0;
  TH1F* phspoil = 0;
  int nbin = 60;
  string sangle = "position";
  if ( angle ) sangle = "angle";
  string hname = "hsig_" + smeasphi(measphi) + "_" + sangle;
  string title = smeasphi(measphi) + " " + sangle + " pull";
  string title_spoil = title + " spoiled efficiency";
  title += " efficiency";
  string hname_spoil = hname + "_spoil";
  string hname_unspoil = hname + "_unspoil";
  string pvar = "z";
  if ( measphi ) pvar = "y";
  string var = "";
  if ( angle ) var += "a";
  if ( measphi ) var += "y";
  else var += pvar;
  string truth_var = pvar + "h";
  string sarg = "log10(abs(" + var + "-" + var + "h)/d" + var + ")>>";
  string sarg_spoil = sarg + hname_spoil;
  sarg += hname;
  TCut cut_all = segperf_cut_ref_eta;
  TCut cut_found = segperf_cut_ref_eta + segperf_cut_found_eta;
  TCut cut_spoil = segperf_cut_ref_eta + segperf_cut_spoiled_eta;
  TCut cut_unspoil = segperf_cut_ref_eta + segperf_cut_unspoiled_eta;
  if ( measphi ) {
    cut_all = segperf_cut_ref_phi;
    cut_found = segperf_cut_ref_phi + segperf_cut_found_phi;
    cut_spoil = segperf_cut_ref_phi + segperf_cut_spoiled_phi;
    cut_unspoil = segperf_cut_ref_phi + segperf_cut_unspoiled_phi;
  }
  gDirectory->Delete(hname.c_str());
  gDirectory->Delete(hname_spoil.c_str());
  ph = new TH1F(hname.c_str(), title.c_str(), nbin, lmin, lmax);
  segperf()->Draw(sarg.c_str(), cut_found, "goff");
  phspoil = new TH1F(hname_spoil.c_str(), title_spoil.c_str(), nbin, lmin, lmax);
  segperf()->Draw(sarg_spoil.c_str(), cut_spoil, "goff");
  if ( truthnorm ) {
    phall = new TH1F("truth_eta", "Truth eta", nbin, lmin, lmax);
    segperf()->Draw("eta>>truth_eta", cut_all, "goff");
    ph->SetBinContent(nbin+1, phall->GetEntries() - ph->GetEntries());
    phspoil->SetBinContent(nbin+1, phall->GetEntries() - phspoil->GetEntries());
    string app = " (truth normalization)";
    string title = ph->GetTitle() + app;
    ph->SetTitle(title.c_str());
    title = phspoil->GetTitle() + app;
    phspoil->SetTitle(title.c_str());
  } else {
    phspoil->SetBinContent(nbin+1, ph->GetEntries() - phspoil->GetEntries());
  }
  if ( angle ) {
    ph->SetXTitle("log10(abs(residual/error))");
  } else {
    ph->SetXTitle("log10(abs(residual/error))");
  }
  gStyle->SetOptStat(false);
  TH1F* phint = integrate(ph);
  TH1F* phint_spoiled = integrate(phspoil);
  //phint->SetFillColor(31);
  phint->SetFillColor(33);
  phint->SetFillStyle(3001);
  phint_spoiled->SetFillColor(46);
  phint_spoiled->SetFillStyle(3001);
  phint->SetMinimum(0.0);
  phint->SetMaximum(1.0);
  phint->Draw("");
  gPad->SetGridx();
  gPad->SetGridy();
  phint_spoiled->Draw("SAME");
  gaussintlog(nbin, lmin, lmax)->Draw("same");
  flatintlog(nbin, lmin, lmax)->Draw("same");
}

//**********************************************************************
