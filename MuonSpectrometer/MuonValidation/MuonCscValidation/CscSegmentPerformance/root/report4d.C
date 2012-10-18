/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// report4d.C

void report4d(string dir, string rel, string dset, string alg) {
  string title = "CSC 4D segment performance";
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
  effs4(&htout);
  htout << "</pre>" << endl;
  TCanvas mycan("mycan", "My canvas", 800, 600);
  MuPublish pub(mycan, dir, true, &htout);
  report4d1(pub);
  htout << "</html>" << endl;
}

void report4d1(MuPublish& dir) {
  dir.header("Truth chi-square", 2);
  sigplot4(0, 2, 10.);
  dir.htprint("deva4d", "Evaluation distance");
  sigplot4(0, 3, 10.);
  dir.htprint("tprob4d", "Probability");
}
