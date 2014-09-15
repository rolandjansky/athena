/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// report.C

void report(string dir, string rel, string dset, string alg) {
  cout << "Making report" << endl;
  cout << "  Dir: " << dir << endl;
  cout << "  Rel: " << rel << endl;
  cout << " Dset: " << dset << endl;
  cout << "  Alg: " << alg << endl;
  string title = "CSC cluster performance";
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
  char c;
  while ( indate.get(c) && !indate.eof() ) {
    htout << c;
  }
  system("rm -f date.tmp");
  htout << "<pre>" << endl;
  effs(&htout);
  htout << "</pre>" << endl;
  TCanvas mycan("mycan", "My canvas", 800, 600);
  MuPublish pub(mycan, dir, true, &htout);
  report1(pub);
  htout << "</html>" << endl;
}

void report1(MuPublish& dir) {
  // Reference to evaluation (sim to reco) distance (fixed weight).
  // spoil
  dir.header("Spoil flag", 2);
  spoilplot(0,0.5);
  dir.htprint("spoileta", "Eta");
  spoilplot(1,20.);
  dir.htprint("spoilphi", "Phi");
  // position
  dir.header("Positions and position errors", 2);
  cluperf()->Draw("zc", "wzc>0");
  dir.htprint("poseta", "Eta position");
  cluperf()->Draw("dzc", "wzc>0 && szc==0");
  dir.htprint("erreta_unspoil", "Eta unspoiled error");
  cluperf()->Draw("dzc", "wzc>0 && szc!=0");
  dir.htprint("erreta_spoil", "Eta spoiled error");
  cluperf()->Draw("yc", "wyc>0");
  dir.htprint("posphi", "Phi position");
  cluperf()->Draw("dyc", "wyc>0");
  dir.htprint("errphi", "Phi error");
  // res
  dir.header("Resolutions", 2);
  resplot(0,0.5);
  dir.htprint("reseta", "Eta position");
  resplot(1,20.);
  dir.htprint("resphi", "Phi position");
  // pull
  dir.header("Pulls", 2);
  sigplot(0,5);
  dir.htprint("sigeta", "Eta position");
  sigplot(1,5.);
  dir.htprint("sigphi", "Phi position");
  // eff
  dir.header("Integrated efficiency", 2);
  reseff(0);
  dir.htprint("effreseta", "Eta residual");
  reseff(1);
  dir.htprint("effresphi", "Phi residual");
  sigeff(0);
  dir.htprint("effsigeta", "Eta pull");
  sigeff(1);
  dir.htprint("effsigphi", "Phi pull");
}

/* To be added
//charge
cfueta.draw("qlz+qrz+qpz", 100,0,4000000)
cfueta.profile("qlz+qrz+qpz","zc-zh", "gaus", "Sigma", 21,300000,2400000, 0,0.075, 50,-0.2,0.2 ,"","","")
cfueta.profile("qlz+qrz+qpz","(zc-zh)/dzc", "gaus", "Sigma", 21,300000,2400000, 0.6,1.4, 50,-5,5 ,"","","")

//position
cfueta.draw2("zh", "dzc", "Eta cluster error vs position", "", "", -600,600, 0,0.12)
cfueta.draw2("zh", "zh-zc", "Eta cluster residual vs position", "", "", -600,600, -0.2,0.2)
cfueta.profile("zh", "zh-zc", "gaus", "Sigma", 20, -600,600, 0,0.1, 50, -0.3,0.3, "", "", "")
cfueta.profile("zh", "(zh-zc)/dzc", "gaus", "Sigma", 20, -600,600, 0, 2, 50, -0.3,0.3, "", "", "")
cfueta.profile("zh", "(zh-zc)/dzc", "gaus", "Sigma", 20, -600,600, 0.6,1.4, 50,-5,5, "", "", "")

// strip position
cfueta.profile("rstrippos(zh,pzc)", "zh-zc", "gaus", "Mean", 20, 0, 1, -0.02, 0.02, 50, -0.2, 0.2, "", "", "")
cfueta.profile("rstrippos(zh,pzc)", "(zh-zc)/dzc", "gaus", "Sigma", 20, 0, 1, 0, 0.1, 50, -1, 1, "", "", "")

//error
cfueta.draw("dzc",100,0,.12)
cfueta.draw2("qlz+qrz+qpz", "dzc", "eta charge vs. error", "eta charge (left+peak+right e)", "eta error (mm)", 0,2500000, 0,0.12)
cfueta.profile("dzc", "zc-zh", "gaus", "Sigma", 20,0,0.12, 0,0.2, 50,-0.2,0.2, "eta resolution vs error", "eta error (mm)", "")
cfueta.profile("dzc", "(zc-zh)/dzc", "gaus", "Sigma", 20,0,0.12, 0.6,1.4, 50,-5,5, "eta pull vs error", "eta error (mm)", "")

//residual
cfueta.draw2("qlz+qrz+qpz", "zc-zh", "eta residual vs. error", "charge (left+peak+right e)", "eta residual (reco-truth mm)", 0,2500000, -0.2, 0.2)

// pull
cfueta.draw("(zc-zh)/dzc", 100, -5, 5, "Unspoiled eta cluster pull", "", "gaus")
cfueta.draw2("qlz+qrz+qpz", "(zc-zh)/dzc", "eta pull vs. error", "charge (left+peak+right e)", "eta pull", 0,2500000, -5, 5)


// corrections
cfueta.draw2("scor1", "qlz/qpz", "","","", -0.5,0.5, 0.1)
cfueta.draw2("rstrippos(zh,pzc)-0.5", "scor1", "","","", -0.5,0.5, -0.5,0.5)
*/
