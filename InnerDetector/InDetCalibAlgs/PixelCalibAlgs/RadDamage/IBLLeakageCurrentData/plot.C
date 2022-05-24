#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

// Jennet Dickinson
// November 28, 2018

double lumi_2015 = 4.37;
double lumi_2016 = lumi_2015 + 39.38;
double lumi_2017 = lumi_2016 + 51.64;
double lumi_2018 = lumi_2017 + 66.07;

// Function for saving canvases
void save(TCanvas* c, string str){

  string title = "plots/"+str+".";
  string title_eps = title+"eps";
  string title_png = title+"png";
  string title_pdf = title+"pdf";

  c->SaveAs(title_eps.c_str());
  c->SaveAs(title_png.c_str());
  c->SaveAs(title_pdf.c_str());

  return;
}

// Apply smoothing                                                                                                       
TGraph* smooth(TGraph* g1){

  TGraph* g1sm = new TGraph();  
  
  double* all_x = g1->GetX();
  double* all_y = g1->GetY();

  for(int i=0; i<g1->GetN()-5; i++){
    
    if(i+4 > g1->GetN() ) continue;
    
    double x[] = {all_x[i],all_x[i+1],all_x[i+2],all_x[i+3],all_x[i+4]};
    double y[] = {all_y[i],all_y[i+1],all_y[i+2],all_y[i+3],all_y[i+4]};
    
    double med_lumi = TMath::Median(5,x);
    double med_ileak = TMath::Median(5,y);
    
    g1sm->SetPoint(i,med_lumi,med_ileak);
  }

  return g1sm;
}

// Function for converting ssv to TGraph
void from_ssv(string suffix){

  TGraph* g1 = new TGraph();
  TGraph* g2 = new TGraph();
  TGraph* g3 = new TGraph();

  string indir = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/means/";
  string filename = indir + suffix + ".ssv";

  string line; int i=-1;
  // Read necessary input parameters from text file
  std::ifstream inputFile(filename.c_str());
  while(getline(inputFile, line)) {

    // skip empty lines and first line
    if (!line.length() || i == -1){
      i++;
      continue;
    }
    i++;

    string value;
    std::istringstream iss(line);
    double vals[4]={};
    int j = 0;
    while(getline(iss,value,',')){
      vals[j] = stod(value);
      j++;
    }

    double lumi = vals[0];
    double I1 = vals[1];
    double I2 = vals[2];
    double I3 = vals[3];

    g1->SetPoint(i, lumi, I1);
    g2->SetPoint(i, lumi, I2);
    g3->SetPoint(i, lumi, I3);
  }

  TGraph* g1sm = smooth(g1);
  TGraph* g2sm = smooth(g2);
  TGraph* g3sm = smooth(g3);

  string out_filename=indir+suffix+"_coarse.root";
  TFile outfile(out_filename.c_str(),"recreate");

  g1->SetTitle("I_Eg1_12");
  g2->SetTitle("I_Eg1_21");
  g3->SetTitle("I_Eg1_30");

  g1->SetName("I_Eg1_12");
  g2->SetName("I_Eg1_21");
  g3->SetName("I_Eg1_30");

  g1->Write();
  g2->Write();
  g3->Write();

  string out_filename2=indir+suffix+".root";
  TFile outfile2(out_filename2.c_str(),"recreate");

  g1sm->SetTitle("I_Eg1_12");
  g2sm->SetTitle("I_Eg1_21");
  g3sm->SetTitle("I_Eg1_30");

  g1sm->SetName("I_Eg1_12");
  g2sm->SetName("I_Eg1_21");
  g3sm->SetName("I_Eg1_30");

  g1sm->Write();
  g2sm->Write();
  g3sm->Write();

  return;
}

// Calculate the ratio hists
void get_ratios(string gname){

  vector<string> suffix = {"A_M1","A_M2","A_M3","A_M4","C_M1","C_M2","C_M3","C_M4"};
  vector<TGraph*> g;
  vector<TGraph*> g_rat;

  string indir = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/means/";

  for(int i=0; i<suffix.size(); i++){

    g.push_back(new TGraph());
    string filename = indir+suffix.at(i)+"_coarse.root";
    TFile* infile = new TFile(filename.c_str());

    TGraph* gtmp = (TGraph*)infile->Get(gname.c_str());
    g.at(i) = (TGraph*)gtmp->Clone(suffix.at(i).c_str());
  }

  string out_filename=indir+"Ratios_"+gname+".root";
  TFile outfile(out_filename.c_str(),"recreate");

  for(int i=0; i<suffix.size(); i++){
    cout << suffix.at(i) << " ratio " << endl;

    //    if( suffix.at(i) == "A_M4" || suffix.at(i) == "C_M4" ) continue;

    TGraph* gtmp = new TGraph();
    double* lumi = g.at(i)->GetX();

    if( i<=4 ){
      for(int j=0; j<g.at(i)->GetN(); j++){
        gtmp->SetPoint(j,lumi[j],g.at(i)->Eval(lumi[j])/g.at(3)->Eval(lumi[j]));
      }
    }
    else{
      for(int j=0; j<g.at(i)->GetN(); j++){
        gtmp->SetPoint(j,lumi[j],g.at(i)->Eval(lumi[j])/g.at(7)->Eval(lumi[j]));
      }
    }

    g_rat.push_back(smooth(gtmp));

    g_rat.at(i)->SetTitle(suffix.at(i).c_str());
    g_rat.at(i)->SetName(suffix.at(i).c_str());
    g_rat.at(i)->Write();
  }

  return;

}

// Draw it real pretty
void overlay(string gname){

  gStyle->SetOptTitle(0);

  vector<string> suffix = {"A_M1","C_M1","A_M2","C_M2","A_M3","C_M3","A_M4","C_M4"};
  vector<string> leg_names = {"Planar: z #epsilon [0,8] cm", "Planar: z #epsilon [-8,0] cm",
			      "Planar: z #epsilon [8,16] cm", "Planar: z #epsilon [-16,-8] cm",
			      "Planar: z #epsilon [16,24] cm", "Planar: z #epsilon [-24,-16] cm",
			      "3D: z > 24 cm", "3D: z < -24 cm"};
  vector<int> colors = {1,4,2,5,3,6,7,15};
  vector<TGraph*> g;

  string indir = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/means/";

  double x1=0.13, y1=0.78;
  TLegend* leg = new TLegend(x1,y1,x1+0.45,y1-.15);
  leg->SetBorderSize(0);
  leg->SetNColumns(2);
  leg->SetTextSize(0.025);
  
  for(int i=0; i<suffix.size(); i++){
    cout << suffix.at(i) << endl;

    g.push_back(new TGraph());
    string filename = indir+suffix.at(i)+".root";
    TFile* infile = new TFile(filename.c_str());

    TGraph* gtmp = (TGraph*)infile->Get(gname.c_str());
    g.at(i) = (TGraph*)gtmp->Clone(suffix.at(i).c_str());

    g.at(i)->GetXaxis()->SetTitle("Delivered integrated luminosity [fb^{-1}]");
    g.at(i)->GetYaxis()->SetTitle("Leakage current at 0#circC [mA/cm^{3}]");
    g.at(i)->SetMarkerColor(colors.at(i));
    g.at(i)->SetLineColor(kGray);
    g.at(i)->SetFillColor(colors.at(i));
    
    string legtext = leg_names.at(i);
    leg->AddEntry(g.at(i),legtext.c_str(),"f");
  }
  
  TCanvas* c1 =new TCanvas(gname.c_str(),gname.c_str(),800,600);  
  gPad->SetTickx();
  gPad->SetTicky();

  g.at(0)->Draw("ap");
  for(int i=1; i<suffix.size(); i++){
    g.at(i)->Draw("psame");
  }
  leg->Draw();

  // Add text
  TLatex l1;
  l1.SetNDC();
  l1.SetTextColor(1);
  l1.DrawLatex(0.15,0.83, "#it{ATLAS} Pixel Preliminary");

  string eg = gname.substr(4);
  eg.replace(1,1,".");

  TLatex l2;
  l2.SetNDC();
  l2.SetTextSize(0.03);
  l2.SetTextColor(1);
  l2.DrawLatex(0.15,0.79, ("#bf{E_{g} = " + eg + " eV}").c_str());

  double ymax = TMath::MaxElement(g.at(0)->GetN(),g.at(0)->GetY());

  TLine *l_2015 = new TLine(lumi_2015, 0., lumi_2015, 0.5*ymax);
  l_2015->SetLineWidth(1);
  l_2015->SetLineColor(15);
  l_2015->SetLineStyle(1);
  l_2015->Draw("same");

  TLine *l_2016 = new TLine(lumi_2016, 0., lumi_2016, 0.5*ymax);
  l_2016->SetLineWidth(1);
  l_2016->SetLineColor(15);
  l_2016->SetLineStyle(1);
  l_2016->Draw("same");

  TLine *l_2017 = new TLine(lumi_2017, 0., lumi_2017, 0.65*ymax);
  l_2017->SetLineWidth(1);
  l_2017->SetLineColor(15);
  l_2017->SetLineStyle(1);
  l_2017->Draw("same");

  TLatex years;
  years.SetNDC();
  years.SetTextColor(15);
  years.SetTextSize(0.03);
  years.DrawLatex(0.13,.4, "2016");
  years.DrawLatex(0.31,.4, "2017");
  years.DrawLatex(0.55,.2, "2018");

  save(c1,gname);

  return;
}

// Draw it real pretty
void overlay_ratio(string gname){

  gStyle->SetOptTitle(0);

  vector<string> suffix = {"A_M1","C_M1","A_M2","C_M2","A_M3","C_M3"};

  vector<string> leg_names = {"Planar: z #epsilon [0,8] cm", "Planar: z #epsilon [-8,0] cm",
                              "Planar: z #epsilon [8,16] cm", "Planar: z #epsilon [-16,-8] cm",
                              "Planar: z #epsilon [16,24] cm", "Planar: z #epsilon [-24,-16] cm"};
  vector<int> colors = {1,4,2,5,3,6};
  vector<TGraph*> g;

  string indir = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/means/";

  double x1=.45, y1=.85;
  TLegend* leg = new TLegend(x1,y1,x1+0.45,y1-.15);
  leg->SetBorderSize(0);
  leg->SetNColumns(2);

  string filename = indir+"Ratios_"+gname+".root";
  TFile* infile = new TFile(filename.c_str());

  for(int i=0; i<suffix.size(); i++){
    cout << suffix.at(i) << endl;

    g.push_back(new TGraph());

    TGraph* gtmp = (TGraph*)infile->Get(suffix.at(i).c_str());
    g.at(i) = smooth((TGraph*)gtmp->Clone(suffix.at(i).c_str()));

    g.at(i)->GetXaxis()->SetTitle("Delivered integrated luminosity [fb^{-1}]");
    g.at(i)->GetYaxis()->SetTitle("Ratio of leakage current to 3Ds at 0#circC [mA/cm^{3}]");
    g.at(i)->SetMarkerColor(colors.at(i));
    g.at(i)->SetLineColor(kGray);
    g.at(i)->SetFillColor(colors.at(i));
    g.at(i)->SetMaximum(2.5);
    g.at(i)->SetMinimum(0.8);

    string legtext = leg_names.at(i);
    leg->AddEntry(g.at(i),legtext.c_str(),"f");
  }
    
  TCanvas* c1 =new TCanvas(gname.c_str(),gname.c_str(),800,600);
  gPad->SetTickx();
  gPad->SetTicky();

  g.at(0)->Draw("ap");
  for(int i=1; i<suffix.size(); i++){
    g.at(i)->Draw("psame");
  }
  leg->Draw();

  // Add text    
  TLatex l1;
  l1.SetNDC();
  l1.SetTextColor(1);
  l1.DrawLatex(0.13,0.8, "#it{ATLAS} Preliminary");

  string eg = gname.substr(4);
  eg.replace(1,1,".");

  TLatex l2;
  l2.SetNDC();
  l2.SetTextSize(0.03);
  l2.SetTextColor(1);
  l2.DrawLatex(0.13,0.75, ("#bf{E_{g} = " + eg + " eV}").c_str());

  TLine *l_2015 = new TLine(lumi_2015, 0.8, lumi_2015, 2);
  l_2015->SetLineWidth(1);
  l_2015->SetLineColor(15);
  l_2015->SetLineStyle(1);
  l_2015->Draw("same");

  TLine *l_2016 = new TLine(lumi_2016, 0.8, lumi_2016, 2);
  l_2016->SetLineWidth(1);
  l_2016->SetLineColor(15);
  l_2016->SetLineStyle(1);
  l_2016->Draw("same");

  TLine *l_2017 = new TLine(lumi_2017, 0.8, lumi_2017, 2);
  l_2017->SetLineWidth(1);
  l_2017->SetLineColor(15);
  l_2017->SetLineStyle(1);
  l_2017->Draw("same");

  TLine *l_1 = new TLine(33.98, 0.8, 33.98, 2);
  l_1->SetLineWidth(1);
  l_1->SetLineColor(15);
  l_1->SetLineStyle(2);
  l_1->Draw("same");

  TLatex years;
  years.SetNDC();
  years.SetTextColor(15);
  years.SetTextSize(0.03);
  years.DrawLatex(0.13,.6, "2016 (80 V)");
  //years.DrawLatex(0.27,.55, "(150 V)");
  years.DrawLatex(0.31,.6, "2017 (350 V)");
  years.DrawLatex(0.55,.6, "2018 (400 V)");

  save(c1,"Ratio_"+gname);

  return;
}

void plot(){

  from_ssv("A_M1");
  from_ssv("C_M1");
  from_ssv("A_M2");
  from_ssv("C_M2");
  from_ssv("A_M3");
  from_ssv("C_M3");
  from_ssv("A_M4");
  from_ssv("C_M4");

  get_ratios("I_Eg1_30");
  //get_ratios("I_Eg1_21");
  get_ratios("I_Eg1_12");

  overlay("I_Eg1_30");
  //overlay("I_Eg1_21");
  overlay("I_Eg1_12");

  overlay_ratio("I_Eg1_30");   
  //overlay_ratio("I_Eg1_21");   
  overlay_ratio("I_Eg1_12");   

  return;
}
