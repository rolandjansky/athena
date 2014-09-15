/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  file  : CscClusterPerformance/root/report_perf.C
//  date  : 2009-12-17
//  brief : Make web-based performance report 

#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <string>
#include <iostream> 
#include <sstream>
#include <fstream>
using namespace std;

void report_perf(){
  // set all inputs
  string rel = "15.5.1"; 
  string dstitle = "MC09_production_vlidation";
  string hitrel = "15.3.1.5";
  string hitdset = "valid3.106051.PythiaZmumu_1Lepton.simul.HITS.e490_s595";
  string esdrel = "15.3.1.8";
  string esddset = "valid3.106051.PythiaZmumu_1Lepton.recon.ESD.e490_s595_r869"; 
  string alg = "cluster";
  string perffile = "csc_perf_cluster.valid3.106051.PythiaZmumu_1Lepton.recon.ESD.e490_s595_r869.v1.root";
  string treefile = "/usatlas/groups/tracking/kyoko/csc/tuples/MC09_production_vlidation/" +  perffile; // "*.root" to read all files under input directory
  string webdir = "/afs/usatlas.bnl.gov/users/kyoko/WWW/CscExcercise/report";
  string outputdir = webdir + "/cluperf/" + esdrel + "/" + esddset + "/" + alg;
  string com = "if ! test -r " + outputdir + "; then mkdir -p " + outputdir + "; fi";
  system(com.c_str()); //gSystem->Exec(com.c_str());
  run_repot_perf(rel, dstitle, hitrel, hitdset, esdrel, esddset, alg, treefile, outputdir);
}

//void report_perf(string dir, string rel, string dset, string alg) {
void  run_repot_perf(string rel, 
		     string dstitle, 
		     string hitrel, 
		     string hitdset, 
		     string esdrel, 
		     string esddset, 
		     string alg, 
		     string treefile, 
		     string outputdir){
  // open tree file
  TFile::Open(treefile.c_str());
  //TChain * pchain = new TChain("csc_cluster");
  //pchain->Add(treefile.c_str()); 
  //pclustertree=pchain; // pclustertree is defined in cluster.C
  
  // Set color
  int color = 1;
  int fill = 0;
  MuColors cols;
  MuFills fills;
  if(alg == "cluster"){
    color = cols.green();
    fill = fills.down();
  }
  else if(alg == "qrat"){
    color = cols.blue();
    fill = fills.horizontal();
  }
  else{
    color = cols.red();
    fill = fills.up();
  }
  
  // html script
  cout << "Making report" << endl;
  cout << "  Outputdir: " << outputdir << endl;
  cout << "  Rel: " << rel << endl;
  cout << " Dset: " << esddset << endl;
  cout << "  Alg: " << alg << endl;
  string title = "CSC cluster performance";
  string htfile = "index.html";
  if ( outputdir.size() ) htfile = outputdir + "/" + htfile;
  ofstream htout(htfile.c_str());
  htout << "<html>" << endl;
  htout << "<head>" << endl;
  htout << "<title>" << title << "</title>" << endl;
  htout << "</head>" << endl;
  htout << "<body>" << endl;
  htout << "<H3>" << title << "</H3>" << endl;
  htout << "<pre>" << endl;
  htout << "Release             : " << rel << endl;
  htout << "Data Release (simul): " << hitrel << endl;
  htout << "Dataset (simul)     : " << hitdset << endl;
  htout << "Data Release (recon): " << esdrel << endl;
  htout << "Dataset (recon)     : " << esddset << endl;
  htout << "Algorithm           : " << alg << endl;
  htout << "Date                : ";
  system("date>date.tmp");
  ifstream indate("date.tmp");
  bool first = true;
  char c;
  while ( indate.get(c) && !indate.eof() ) {
    htout << c;
  }
  system("rm -f date.tmp");
  htout << "</pre>" << endl;
  htout << "<pre>" << endl;
  effs(&htout); // effs is defined in cluperf.C
  htout << "</pre>" << endl;
  htout << "<br> Residual = Cluster(reco) - Hit(truth)" << endl;
  htout << "<br> Pull = Residual/error(Gauss sigma)<br><br>" << endl;
  htout << "<hr>" << endl;
  TCanvas mycan("mycan", "My canvas", 800, 600);
  MuPublish pub(mycan, outputdir, true, &htout);
  report_perf0(pub, color, fill);
  report_perf1(pub);
  htout << "<br><br><hr>" << endl;
  report_perf2(pub, color, fill);
  htout << "</body>" << endl;
  htout << "</html>" << endl;
}

void report_perf0(MuPublish& pub, int color, int fill) {
  // Reference to evaluation (sim to reco) distance (fixed weight).
  atlas_style();
  gStyle->SetOptStat("eo");
  gStyle->SetHistLineColor(color);
  gStyle->SetHistFillColor(color);
  gStyle->SetHistFillStyle(fill);
 //  gStyle->SetOptTitle(1);
//   gStyle->SetTitleSize(0.04,"");
//   gStyle->SetTitleBorderSize(0.0);
//   gStyle->SetTitleX(0.00);
//   gStyle->SetTitleY(0.05);
 //*** spoil *** (after pt>2GeV + chamber strip limit + wzc>0)
  pub.header("Spoil flag", 2);
  spoilplot(0); // spoilplot is defined in cscperf.C
  pub.htprint("spoileta", "Eta");
  spoilplot(1);
  pub.htprint("spoilphi", "Phi");
  //*** position ***
  pub.header("Positions and position errors", 2);
  TH1* ph = 0;
  ph = new TH1F("hzc", "Eta position; #eta position[mm];", 100, -600, 600);
  //ph->SetTitle(dstitle.c_str());
  cluperf()->Draw("zc>>hzc", "wzc>0");
  pub.htprint("poseta", "Eta position");
  ph = new TH1F("hdzcusp", "Eta unspoiled error; #eta unspoiled error[mm];", 50, 0, 0.3);
  cluperf()->Draw("dzc>>hdzcusp", "wzc>0 && (szc==0 || szc==10)");
  pub.htprint("erreta_unspoil", "Eta unspoiled error");
  ph = new TH1F("hdzcsp", "Eta spoiled error; #eta spoiled error[mm];", 50, 0, 50);
  cluperf()->Draw("dzc>>hdzcsp", "wzc>0 && (szc!=0 && szc!=10)");
  pub.htprint("erreta_spoil", "Eta spoiled error");
  ph = new TH1F("hyc", "Phi position; #phi position[mm];", 100, -500, 500);
  cluperf()->Draw("yc>>hyc", "wyc>0");
  pub.htprint("posphi", "Phi position");
  ph = new TH1F("hdyc", "Phi error; #phi error[mm];", 50, 0., 50);
  cluperf()->Draw("dyc>>hdyc", "wyc>0");
  pub.htprint("errphi", "Phi error");
}

void report_perf1(MuPublish& pub) {
  // plot functions with color/fill are defined in cscperf.C
  atlas_style();
//   gStyle->SetOptTitle(1);
//   gStyle->SetTitleFillColor(0);
//   gStyle->SetTitleSize(0.04,"");
//   gStyle->SetTitleBorderSize(0.0);
//   gStyle->SetTitleX(0.00);
//   gStyle->SetTitleY(0.05);
  //*** res ***
  pub.header("Resolutions", 2); 
  resplot(0,0.5);
  pub.htprint("reseta", "Eta position");
  resplot(1,20.);
  pub.htprint("resphi", "Phi position");
  //*** pull ***
  pub.header("Pulls", 2);
  sigplot(0,5);
  pub.htprint("sigeta", "Eta position");
  sigplot(1,5.);
  pub.htprint("sigphi", "Phi position");
  //*** eff ***
  pub.header("Integrated efficiency", 2);
  reseff(0);
  pub.htprint("effreseta", "Eta residual");
  reseff(1);
  pub.htprint("effresphi", "Phi residual");
  sigeff(0);
  pub.htprint("effsigeta", "Eta pull");
  sigeff(1);
  pub.htprint("effsigphi", "Phi pull");
}

void report_perf2(MuPublish& pub, int color, int fill) {
  //For found unspoiled eta cluster
  // profile function is defined in MuDraw
  atlas_style();
  gStyle->SetHistLineColor(color);
  gStyle->SetHistFillColor(color);
  gStyle->SetHistFillStyle(fill);
//   gStyle->SetOptTitle(1);
//   gStyle->SetTitleSize(0.04,"");
//   gStyle->SetTitleBorderSize(0.0);
//   gStyle->SetTitleX(0.00);
//   gStyle->SetTitleY(0.05);
  pub.header("For found unspoiled eta cluste:", 2);
  //*** charge ***
  pub.header("Charge(left+peak+right)", 2);
  cfueta.draw("(qlz+qrz+qpz)/1000", 100, 0, 3000, "Charge", "Q_{Left}+Q_{Peak}+Q_{Right}[1000e]", "");
  pub.htprint("qsum", "Charge(left+peak+right)");

  //error
  cfueta.draw2("(qlz+qrz+qpz)/1000", "dzc", "Error vs charge", "Q_{Left}+Q_{Peak}+Q_{Right}[1000e]", "error[mm]", 0,2500, 0,0.2);
  pub.htprint("qsumerr2D", "Error vs Charge (Scatter)");
 
//  pub.set_style(10); // 10 is not defined so no lineChange
//  gStyle->SetMarkerColor(9);
//  cfueta.profile("(qlz+qrz+qpz)/1000","dzc", "gaus", "Sigma", 26,0,2500, 0,0.2, 50,0,0.2 ,"Error vs Charge","Q_{Left}+Q_{Peak}+Q_{Right}[1000e]","Sigma(Gauss) for error[mm]");
//  pub.htprint("qsumerrPfS", "(Profile: Sigma)");

//  gStyle->SetMarkerColor(49);
//  cfueta.profile("(qlz+qrz+qpz)/1000","dzc", "gaus", "Mean", 26,0,2500, 0,0.2, 50,0,0.2 ,"Error vs Charge","Q_{Left}+Q_{Peak}+Q_{Right}[1000e]","Mean(Gauss) for error[mm]");
//  pub.htprint("qsumerrPfM", "(Profile: Mean)");

  //residual
  gStyle->SetMarkerColor(color);
  cfueta.draw2("(qlz+qrz+qpz)/1000", "zc-zh", "Residual vs Charge", "Q_{Left}+Q_{Peak}+Q_{Right}[1000e]", "residul[mm]", 0,2500, -0.2, 0.2);
  pub.htprint("qsumres2D", "Residual vs Charge (Scatter)");

  pub.set_style(10); // 10 is not defined so no lineChange
  gStyle->SetMarkerColor(9);
  cfueta.profile("(qlz+qrz+qpz)/1000","zc-zh", "gaus", "Sigma", 26,0,2500, 0,0.075, 50,-0.2,0.2 ,"Residual vs Charge","Q_{Left}+Q_{Peak}+Q_{Right}[1000e]","Sigma(Gauss) for residual[mm]");
  pub.htprint("qsumresPfS", /*Residual vs Charge */"(Profile: Sigma)");
 
 gStyle->SetMarkerColor(49);
  cfueta.profile("(qlz+qrz+qpz)/1000","zc-zh", "gaus", "Mean", 26,0,2500, -0.6,0.6, 50,-0.2,0.2 ,"Residual vs Charge","Q_{Left}+Q_{Peak}+Q_{Right}[1000e]","Mean(Gauss) for residual[mm]");
  pub.htprint("qsumresPfM", /*Residual vs Charge */"(Profile: Mean)");

  //pull
  pub.set_style(2);
 gStyle->SetMarkerColor(color);
  cfueta.draw2("(qlz+qrz+qpz)/1000", "(zc-zh)/dzc", "Pull vs Charge", "Q_{Left}+Q_{Peak}+Q_{Right}[1000e]", "pull", 0,2500, -5, 5);
  pub.htprint("qsumpull2D", "Pull vs Charge (Scatter)");

  pub.set_style(10); // 10 is not defined so no lineChange
  gStyle->SetMarkerColor(9);
  cfueta.profile("(qlz+qrz+qpz)/1000","(zc-zh)/dzc", "gaus", "Sigma", 26,0,2500, 0.6,1.4, 50,-5,5 ,"Pull vs Charge","Q_{Left}+Q_{Peak}+Q_{Right}[1000e]","Sigma(Gauss) for pull");
  pub.htprint("qsumpullPfS", /*"Pull vs Charge*/ "(Profile: Sigma)");
  
  gStyle->SetMarkerColor(49);
  cfueta.profile("(qlz+qrz+qpz)/1000","(zc-zh)/dzc", "gaus", "Mean", 26,0,2500, -2,2, 50,-5,5 ,"Pull vs Charge","Q_{Left}+Q_{Peak}+Q_{Right}[1000e]","Mean(Gauss) for pull");
  pub.htprint("qsumpullPfM", /*"Pull vs Charge*/ "(Profile: Mean)");

  //*** position ***
  pub.header("Positions", 2);
  gStyle->SetMarkerColor(color);
  //  cfueta.draw("zh", 100, -600, 600, "", "hit position[mm]", "");
  //pub.htprint("pos", "Hit positon");

  //error
  gStyle->SetMarkerColor(color);
  cfueta.draw2("zh", "dzc", "Cluster error vs hit position", "hit position[mm]", "cluster error[mm]", -600,600, 0,0.12);
  pub.htprint("poserr2D", "Error vs Hit position (Scatter)");
 
  //residual
  cfueta.draw2("zh", "zc-zh", "Cluster residual vs hit position", "hit position[mm]", "residual[mm]", -600,600, -0.2,0.2);
  pub.htprint("posres2D", "Residual vs Hit position (Scatter)");

  pub.set_style(10);
  gStyle->SetMarkerColor(9);
  cfueta.profile("zh", "zc-zh", "gaus", "Sigma", 20, -600,600, 0,0.1, 50, -0.2,0.2, "Residual vs Hit position", "hit position[mm]", "Sigma(Gauss) for residual[mm]");
  pub.htprint("posresPfS", "(Profile: Sigma)");
 
  gStyle->SetMarkerColor(49);
  cfueta.profile("zh", "zc-zh", "gaus", "Mean", 20, -600,600, -0.02,0.02, 50, -0.2,0.2, "Residual vs Hit position", "hit position[mm]", "Mean(Gauss) for residual[mm]");
  pub.htprint("posresPfM", "(Profile: Mean)");

  //pull
  pub.set_style(2); 
  gStyle->SetMarkerColor(color);
  cfueta.draw2("zh", "(zc-zh)/dzc", "Cluster pull vs hit position", "hit position[mm]", "pull", -600,600, -5,5);
  pub.htprint("pospull2D", "Pull vs Hit position (Scatter)");

  pub.set_style(10);
  gStyle->SetMarkerColor(9);
  cfueta.profile("zh", "(zc-zh)/dzc", "gaus", "Sigma", 20, -600,600, 0.6,1.4, 50,-5,5, "Pull vs Position", "hit position[mm]", "Sigma(Gauss) for pull");
  pub.htprint("pospullPfS", "(Profile: Sigma)");

  gStyle->SetMarkerColor(49);
  cfueta.profile("zh", "(zc-zh)/dzc", "gaus", "Mean", 20, -600,600, -0.2,0.2, 50,-5,5, "Pull vs Position", "hit position[mm]", "Mean(Gauss) for pull");
  pub.htprint("pospullPfM", "(Profile: Mean)");
  
  //*** strip position ***
  pub.header("Strip positions", 2);
  gStyle->SetMarkerColor(color);
  //cfueta.draw("rstrippos(zh,pzc)", 100, 0, 1, "", "strip position", "");
  //pub.htprint("strippos", "Strip positon");

  //error
  gStyle->SetMarkerColor(color);
  cfueta.draw2("rstrippos(zh,pzc)", "dzc", "Cluster error vs strip position", "strip position[mm]", "cluster error[mm]", 0,1, 0,0.12);
  pub.htprint("stripposerr2D", "Error vs Strip position (Scatter)");
 
  //residual
   cfueta.draw2("rstrippos(zh,pzc)", "zc-zh", "Cluster residual vs strip position", "strip position", "residual[mm]", 0,1, -0.2,0.2);
  pub.htprint("stripposres2D", "Residual vs Strip position (Scatter)");

  pub.set_style(10);
  gStyle->SetMarkerColor(9);
  cfueta.profile("rstrippos(zh,pzc)", "zc-zh", "gaus", "Sigma", 20, 0, 1, 0, 0.1, 50, -0.2, 0.2, "Residual vs Strip position", "strip position", "Sigma(Gauss) for residual[mm]");
  pub.htprint("stripposresPfS", "(Profile: Sigma)");

  gStyle->SetMarkerColor(49);
  cfueta.profile("rstrippos(zh,pzc)", "zc-zh", "gaus", "Mean", 20, 0, 1, -0.01, 0.01, 50, -0.2, 0.2, "Residual vs Strip position", "strip position", "Mean(Gauss) for residual[mm]");
  pub.htprint("stripposresPfM", "(Profile: Mean)");

  //pull
  pub.set_style(2); 
  gStyle->SetMarkerColor(color);
  cfueta.draw2("rstrippos(zh,pzc)", "(zc-zh)/dzc", "Cluster pull vs strip position", "strip position", "pull", 0,1, -5,5);
  pub.htprint("strippospull2D", "Pull vs Strip position (Scatter)");

  pub.set_style(10);
  gStyle->SetMarkerColor(9);
  cfueta.profile("rstrippos(zh,pzc)", "(zc-zh)/dzc", "gaus", "Sigma", 20, 0, 1, 0, 2, 50, -5, 5, "Pull vs Strip position", "strip position", "Sigma(Gauss) for pull");
  pub.htprint("strippospullPfS", "(Profile: Sigma)");
  
  gStyle->SetMarkerColor(49);
  cfueta.profile("rstrippos(zh,pzc)", "(zc-zh)/dzc", "gaus", "Mean", 20, 0, 1, -0.5, 0.5, 50, -5, 5, "Pull vs Strip position", "strip position", "Mean(Gauss) for pull");
  pub.htprint("strippospullPfM", "(Profile: Mean)");
  
  //*** error ***
  pub.header("Errors", 2);
  gStyle->SetMarkerColor(color);
  // cfueta.draw("dzc", 100, 0, 0.12, "", "error[mm]", "");
  //pub.htprint("err", "Error");

  //residual 
  cfueta.draw2("dzc", "zc-zh", "Cluster residual vs error", "error[mm]", "residual[mm]", 0,0.12, -0.2,0.2);
  pub.htprint("errres2D", "Residual vs Error (Scatter)");

  pub.set_style(10);
  gStyle->SetMarkerColor(9);
  cfueta.profile("dzc", "zc-zh", "gaus", "Sigma", 20,0,0.12, 0,0.2, 50,-0.2,0.2, "Residual vs error", "error[mm]", "Sigma(Gauss) for residual[mm]");
  pub.htprint("errresPfS", "(Profile: Sigma)");

  gStyle->SetMarkerColor(49);
  cfueta.profile("dzc", "zc-zh", "gaus", "Mean", 20,0,0.12, -0.5,0.5, 50,-0.2,0.2, "Residual vs error", "error[mm]", "Mean(Gauss) for residual[mm]");
  pub.htprint("errresPfM", "(Profile: Mean)");

  //pull 
  pub.set_style(2); 
  gStyle->SetMarkerColor(color);
  cfueta.draw2("dzc", "(zc-zh)/dzc", "Cluster pull vs error", "error[mm]", "pull", 0,0.12, -5,5);
  pub.htprint("errpull2D", "Residual vs Error (Scatter)");

  pub.set_style(10);
  gStyle->SetMarkerColor(9);
  cfueta.profile("dzc", "(zc-zh)/dzc", "gaus", "Sigma", 20,0,0.12, 0.6,1.4, 50,-5,5, "Pull vs Error", "error[mm]", "Sigma(Gauss) for pull");
  pub.htprint("errpullPfS", "(Profile: Sigma)");
 
  gStyle->SetMarkerColor(49);
  cfueta.profile("dzc", "(zc-zh)/dzc", "gaus", "Mean", 20,0,0.12, -0.5,0.5, 50,-5,5, "Pull vs Error", "error[mm]", "Mean(Gauss) for pull");
  pub.htprint("errpullPfM", "(Profile: Mean)");
  
 // corrections
 // pub.header("Collections", 2);
 // cfueta.draw2("scor1", "qlz/qpz", "Strip charge ratio vs Correction","x_{Left}","Q_{Left}/Q_{Peak}", -0.5,0.5, 0.1);
 // pub.htprint("corrqrat2D", "Strip charge ratio vs Correction");
 // cfueta.draw2("scor1", "rstrippos(zh,pzc)-0.5", "Strip position ratio vs Correction","x_{Left}","strip position", -0.5,0.5, -0.5,0.5);
 // pub.htprint("corrpos2D", "Strip position vs Correction");
}

