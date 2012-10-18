/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// report2d.C

void report2d(string dir, string rel, string dset, string alg) {
  string title = "CSC 2D segment performance";
  string htfile = "index.html";
  if ( dir.size() ) htfile = dir + "/" + htfile;
  ofstream htout(htfile.c_str());
  htout << "<html>" << endl;
  htout << "<head>" << endl;
  htout << "<title>" << title << "</title>" << endl;
  htout << "</head>" << endl;
  htout << "<body>" << endl;
  htout << "<H3>" << title << "</H3>" << endl;
  htout << "Release: " << rel << endl;
  htout << "<br>Dataset: " << dset << endl;
  htout << "<br>Algorithm: " << alg << endl;
  htout << "<br>Date: ";
  system("date>date.tmp");
  ifstream indate("date.tmp");
  bool first = true;
  string s;
  while ( indate>>s && s.size()!=0 && s!= '\n' ) {
    if ( ! first ) htout << " ";
    first = false;
    htout << s;
  }
  system("rm -f date.tmp");
  htout << "<pre>" << endl;
  effs(&htout);
  htout << "</pre>" << endl;
  TCanvas mycan("mycan", "My canvas", 800, 600);
  MuPublish pub(mycan, dir, true, &htout);
  report2d1(pub);
  htout << "</html>" << endl;
}

void report2d1(MuPublish& dir) {
  // Reference to evaluation (sim to reco) distance (fixed weight).
  // res
  dir.header("Resolutions", 2);
  resplot(0, 0, 0.2);
  dir.htprint("resposeta", "Eta position");
  resplot(0, 1, 0.01);
  dir.htprint("resangeta", "Eta angle");
  resplot(1, 0, 20.);
  dir.htprint("resphipos", "Phi position");
  resplot(1, 1, 0.5);
  dir.htprint("resangphi", "Phi angle");
  // pull
  dir.header("Pulls", 2);
  sigplot(0, 0, 5.);
  dir.htprint("sigposeta", "Eta position");
  sigplot(0, 1, 5.);
  dir.htprint("sigangeta", "Eta angle");
  sigplot(1, 0, 5.);
  dir.htprint("sigposphi", "Phi position");
  sigplot(1, 1, 5.);
  dir.htprint("sigangphi", "Phi angle");
  // chi-squares
  dir.header("Truth chi-square", 2);
  sigplot(0, 2, 10.);
  dir.htprint("devaeta", "Eta evaluation distance");
  sigplot(1, 2, 10.);
  dir.htprint("devaphi", "Phi evaluation distance");
  sigplot(0, 3, 1.);
  dir.htprint("tprobeta", "Eta probability");
  sigplot(1, 3, 1.);
  dir.htprint("tprobphi", "Phi probability");
  // eff
  dir.header("Integrated residual efficiency", 2);
  reseff(0,0);
  dir.htprint("effresetapos", "Eta position");
  reseff(1,0);
  dir.htprint("effresphipos", "Phi position");
  reseff(0,1);
  dir.htprint("effresetaang", "Eta angle");
  reseff(1,1);
  dir.htprint("effresphiang", "Phi angle");
  // eff
  dir.header("Integrated pull efficiency", 2);
  sigeff(0,0);
  dir.htprint("effsigetapos", "Eta position");
  sigeff(1,0);
  dir.htprint("effsigphipos", "Phi position");
  sigeff(0,1);
  dir.htprint("effsigetaang", "Eta angle");
  sigeff(1,1);
  dir.htprint("effsigphiang", "Phi angle");
  //sigeff(0);
  //dir.htprint("effsigeta", "Eta pull");
  //sigeff(1);
  //dir.htprint("effsigphi", "Phi pull");
}

