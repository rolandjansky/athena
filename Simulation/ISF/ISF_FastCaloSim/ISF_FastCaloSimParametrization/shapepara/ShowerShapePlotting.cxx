/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************
* Macro to make pretty plots
* <a.hasib@cern.ch>
****************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>



#include "TLine.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TLatex.h"



#include "ShowerShapePlotting.h"
#include "AtlasStyle.h"

using namespace std;


ShowerShapePlotting::ShowerShapePlotting()
{
   gROOT->SetBatch(kTRUE);
   gStyle->SetOptStat(0);
   //gStyle->SetPalette(1);

   m_calolayer      = 0;
   m_PCAbin         = 0;
   m_particle       = "";
   m_particleEnergy = 0.;
   m_etamin         = 0.;
   m_etamax         = 0.;

   m_topDir        = "";
   m_fileName      = "";
   m_outputDirName = "";
   m_NNinputName   = "";
   m_NNoutputName  = "";
}


ShowerShapePlotting::~ShowerShapePlotting()
{
}


void ShowerShapePlotting::PlotEnergyDensityGradient()
{
   cout << " * Plotting energy density gradient..." << endl;

   TFile *NNinputFile = TFile::Open(m_NNinputName.c_str());

   TH1F *hist = (TH1F *)NNinputFile->Get("hGradient");

   hist->SetTitle("");

   for (int i = 0; i < hist->GetNbinsX(); i++)
   {
      hist->SetBinError(i, 0);
   }



   hist->SetMarkerStyle(20);
   hist->SetMarkerSize(1);
   hist->GetYaxis()->SetLabelSize(.02);
   hist->GetXaxis()->SetLabelSize(.02);
   //hist->GetYaxis()->SetRangeUser(-1,0);
   hist->SetMaximum(0);
   hist->GetYaxis()->SetTitle("#it{log}(#rho(E)) variation");
   hist->GetXaxis()->SetTitle("r [mm]");



   TCanvas *c0 = new TCanvas("c0", "Gradient", 800, 800);
   c0->SetLogx();
   c0->SetRightMargin(0.13);

   hist->Draw("P");


   int nzoom1 = int((hist->GetNbinsX()) / 3. * 2.) - 1;
   int nzoom2 = int((hist->GetNbinsX()) / 3.) - 1;

   std::cout << "ZOOM VALUES : " << nzoom1 << " " << nzoom2 << std::endl;

   float Rzoom2 = hist->GetXaxis()->GetBinLowEdge(nzoom2);
   float Rzoom1 = hist->GetXaxis()->GetBinLowEdge(nzoom1);
   float Rzoom0 = hist->GetXaxis()->GetBinLowEdge(hist->GetNbinsX()) + hist->GetXaxis()->GetBinWidth(hist->GetNbinsX());

   TLine *lg1p = new TLine(0, -0.1, Rzoom2, -0.1);
   lg1p->SetLineWidth(2);
   lg1p->SetLineColor(kBlue);
   lg1p->Draw();

   TLine *lg2 = new TLine(Rzoom2, -0.1, Rzoom2, -0.3);
   lg2->SetLineWidth(2);
   lg2->SetLineStyle(7);
   lg2->SetLineColor(kBlue);
   lg2->Draw();

   TLine *lg2p = new TLine(Rzoom2, -0.3, Rzoom1, -0.3);
   lg2p->SetLineWidth(2);
   lg2p->SetLineColor(kBlue);
   lg2p->Draw();

   TLine *lg3 = new TLine(Rzoom1, -0.3, Rzoom1, -0.8);
   lg3->SetLineWidth(2);
   lg3->SetLineStyle(7);
   lg3->SetLineColor(kBlue);
   lg3->Draw();

   TLine *lg3p = new TLine(Rzoom1, -0.8, Rzoom0, -0.8);
   lg3p->SetLineWidth(2);
   lg3p->SetLineColor(kBlue);
   lg3p->Draw();


   TLatex *l = new TLatex(60, -0.1, "ATLAS");
   l->SetTextSize(.03);
   l->SetTextFont(72);

   TLatex *l2 = new TLatex(300, -0.1, "Simulation Preliminary");
   l2->SetTextSize(.03);
   l2->SetTextFont(42);

   std::string labeltitle = CreateLabels();
   TLatex      *l3        = new TLatex(-1, 0.01, labeltitle.c_str());
   l3->SetTextSize(.02);
   l3->SetTextFont(42);

   l->Draw();
   l2->Draw();
   l3->Draw();

   c0->Update();
   c0->SaveAs((m_outputDirName + "/EnergyDensityGradient.png").c_str());
}


void ShowerShapePlotting::PlotPolar(std::vector<string> histNameVec, bool isOutput)
{
   cout << " * Plotting NNinput polar plots ..." << endl;

   gStyle->SetPalette(1);
   SetCustomPalette();
   std::string labeltitle = CreateLabels();


   TFile *NNinputFile  = TFile::Open(m_NNinputName.c_str());
   cout << " input file = " << m_NNinputName.c_str() << endl;
   TFile *NNoutputFile = TFile::Open(m_NNoutputName.c_str());


   for (unsigned int ihist = 0; ihist < histNameVec.size(); ihist++)
   {
      TH2F *href = (TH2F *)NNinputFile->Get("hEnergyNorm");
      //href->Scale(1 / href->Integral());
      TH2F *hout;



      std::string label;

      if ((histNameVec.at(ihist)).compare("hHits") == 0)
      {
         label = "Number of hits";
      }
      else if ((histNameVec.at(ihist)).compare("hEnergy") == 0)
      {
         label = "E [MeV] ";
      }
      else if ((histNameVec.at(ihist)).compare("hEnergyDensity") == 0)
      {
         label = "#rho(E) [MeV/mm^{2}]";
      }
      else if ((histNameVec.at(ihist)).compare("hLnEnergy") == 0)
      {
         label = "#it{log}(E) [MeV]";
      }
      else if ((histNameVec.at(ihist)).compare("hLnEnergyDensity") == 0)
      {
         label = "#it{log}(#rho(E)) [MeV/mm^{2}]";
      }


      // * loop over different zoom

      for (int izoom = 0; izoom < 3; izoom++)
      {
         float  zoom      = -1;
         string labelzoom = "";

         if (izoom == 0)
         {
            zoom      = href->GetYaxis()->GetBinLowEdge(href->GetNbinsY()) + href->GetYaxis()->GetBinWidth(href->GetNbinsY());
            labelzoom = "zoom0";
         }
         else if (izoom == 1)
         {
            int nzoom = int(2. * (href->GetNbinsY()) / 3.) - 1;
            zoom      = href->GetYaxis()->GetBinLowEdge(nzoom);
            labelzoom = "zoom1";
         }
         else if (izoom == 2)
         {
            int nzoom = int((href->GetNbinsY()) / 3.) - 1;
            zoom      = href->GetYaxis()->GetBinLowEdge(nzoom);
            labelzoom = "zoom2";
         }



         float myr[3];
         myr[0] = -zoom;
         myr[1] = 0;
         myr[2] = zoom;

         TLatex *l = new TLatex(-1 * zoom, -1.20 * zoom, "ATLAS");
         l->SetTextSize(.035);
         l->SetTextFont(72);

         TLatex *l2 = new TLatex(-0.6 * zoom, -1.20 * zoom, "Simulation Preliminary");
         l2->SetTextSize(.035);
         l2->SetTextFont(42);

         TLatex *lInputTitle = new TLatex(-zoom, 1.02 * zoom, (labeltitle + /*", " + labelzoom +*/ ", Geant4").c_str());
         lInputTitle->SetTextSize(.03);
         lInputTitle->SetTextFont(42);


         TLatex *lOutputTitle = new TLatex(-zoom, 1.02 * zoom, (labeltitle + /*", " + labelzoom + */", NN output" /*, neurons =" + std::to_string(m_neurons)*/).c_str());
         lOutputTitle->SetTextSize(.03);
         lOutputTitle->SetTextFont(42);


         TLatex *lRatioTitle = new TLatex(-zoom, 1.02 * zoom, (labeltitle + /*", " + labelzoom + */", Ratio" /*neurons =" + std::to_string(m_neurons)*/).c_str());
         lRatioTitle->SetTextSize(.03);
         lRatioTitle->SetTextFont(42);


         TH2F *hdummy = new TH2F(("hdummy" + histNameVec.at(ihist) + labelzoom).c_str(), "", 2, myr, 2, myr);
         hdummy->GetYaxis()->SetRangeUser(-float(zoom), float(zoom));
         hdummy->GetXaxis()->SetRangeUser(-float(zoom), float(zoom));
         hdummy->GetYaxis()->SetLabelSize(.025);
         hdummy->GetXaxis()->SetLabelSize(.025);

         hdummy->GetXaxis()->SetTitle("x [mm]");
         hdummy->GetXaxis()->SetTitleSize(0.035);
         hdummy->GetYaxis()->SetTitle("y [mm]");
         hdummy->GetYaxis()->SetTitleSize(0.035);



         href->SetContour(20);
         href->GetYaxis()->SetRangeUser(-float(zoom), float(zoom));
         //href->GetZaxis()->SetRangeUser(4000, 6500);
         href->GetZaxis()->SetLabelSize(0.025);
         href->GetZaxis()->SetTitle("Energy normalized to unity");
         href->GetZaxis()->SetTitleSize(0.035);
         href->GetZaxis()->SetTitleOffset(1.4);
         href->Sumw2();
         //href->Scale(1.0 / href->Integral());

         TFile *fNN = new TFile("fNN.root", "recreate");

         TCanvas *c1 = new TCanvas(("NNinput_" + histNameVec.at(ihist) + labelzoom).c_str(), ("NNinput_" + histNameVec.at(ihist) + labelzoom).c_str(), 900, 900);
         c1->cd();
         c1->SetLeftMargin(0.14);
         c1->SetRightMargin(0.17);
         hdummy->Draw("COLZ");
         href->Draw("COLZ POL SAME");

         l->Draw();
         l2->Draw();
         lInputTitle->Draw();

         c1->SaveAs((m_outputDirName + "NNinput_" + histNameVec.at(ihist) + "_" + labelzoom + ".png").c_str());

         fNN->cd();
         c1->Write();
         c1->Close();

         if (isOutput)
         {
            hout = (TH2F *)NNoutputFile->Get("hHits");
            //hout->Scale(1 / hout->Integral());
            hout->SetContour(20);
            hout->GetYaxis()->SetRangeUser(-float(zoom), float(zoom));
            hout->GetZaxis()->SetRangeUser(4000, 6500);
            hout->GetZaxis()->SetLabelSize(0.025);
            //hout->GetZaxis()->SetRangeUser(0.0, 0.011);
            hout->GetZaxis()->SetTitle("Number of hits");
            hout->GetZaxis()->SetTitleSize(0.035);
            hout->GetZaxis()->SetTitleOffset(1.4);
            hout->Sumw2();



            TCanvas *c2 = new TCanvas(("NNoutput_" + histNameVec.at(ihist) + labelzoom).c_str(), ("NNoutput_" + histNameVec.at(ihist) + labelzoom).c_str(), 900, 900);
            c2->cd();
            c2->SetLeftMargin(0.14);
            c2->SetRightMargin(0.17);
            hdummy->Draw("COLZ");
            hout->Draw("COLZ POL SAME");

            l->Draw();
            l2->Draw();
            lOutputTitle->Draw();

            c2->SaveAs((m_outputDirName + "NNoutput_" + histNameVec.at(ihist) + "_" + labelzoom + ".png").c_str());
            fNN->cd();
            c2->Write();
            c2->Close();
            fNN->Close();

            // * ratio
            TH2F *hratio = (TH2F *)href->Clone(("hratio_" + histNameVec.at(ihist) + labelzoom).c_str());
            hratio->Divide(hout);
            hratio->GetYaxis()->SetRangeUser(-float(zoom), float(zoom));
            hratio->GetZaxis()->SetTitle("Geant4 / NN output ");
            hratio->GetZaxis()->SetRangeUser(0.8, +1.2);
            hratio->GetZaxis()->SetLabelSize(0.025);
            hratio->GetZaxis()->SetTitleSize(0.035);
            hratio->GetZaxis()->SetTitleOffset(1.4);

            TCanvas *c3 = new TCanvas(("Ratio_" + histNameVec.at(ihist) + labelzoom).c_str(), ("Ratio_" + histNameVec.at(ihist) + labelzoom).c_str(), 900, 900);
            c3->cd();
            c3->SetRightMargin(0.17);
            hdummy->Draw("COLZ");
            hratio->Draw("COLZ POL SAME");

            l->Draw();
            l2->Draw();
            lRatioTitle->Draw();

            c3->SaveAs((m_outputDirName + "Ratio_" + histNameVec.at(ihist) + "_" + labelzoom + ".png").c_str());
            c3->Close();
         }
      }
   }
}


void ShowerShapePlotting::CreateValidationPlot()
{
   SetCustomPalette();



   TFile *NNinputFile = TFile::Open(m_NNinputName.c_str());

   TH2F *hist = (TH2F *)NNinputFile->Get("hValidation");
   hist->SetTitle("");


   TCanvas *c1 = new TCanvas("c1", "c1", 800, 800);

   c1->SetRightMargin(0.13);
   c1->cd();
   hist->Draw("COLZ");

   TLatex *l = new TLatex();
   l->SetTextSize(.02);
   l->DrawLatex(-1, -1, "1 = all OK, 2 = bad gradient, 3 = bad granularity, 4 = bad gradient and granularity");

   std::string labeltitle = CreateLabels();

   TLatex *l2 = new TLatex(0, 14.6, labeltitle.c_str());
   l2->SetTextSize(.02);
   l2->Draw();

   c1->SaveAs((m_outputDirName + "ValidationPlot.png").c_str());
   c1->Close();
}


std::string ShowerShapePlotting::CreateLabels()
{
   std::string energy;

   stringstream energyvalue;
   energyvalue << fixed << setprecision(0) << m_particleEnergy;
   energy = energyvalue.str() + " GeV";

   std::string eta;

   stringstream etamin, etamax;

   etamin << fixed << setprecision(2) << m_etamin;
   etamax << fixed << setprecision(2) << m_etamax;

   eta = etamin.str() + " < |#eta| < " + etamax.str();


   std::string particle;

   if (m_particle.compare("el_1mm") == 0)
   {
      particle = "e^{#pm} (1mm)";
   }
   else if (m_particle.compare("el_opt") == 0)
   {
      particle = "e^{#pm} (opt)";
   }
   else if (m_particle.compare("pion") == 0)
   {
      particle = "#pi^{#pm}";
   }
   else if (m_particle.compare("photon") == 0)
   {
      particle = "#gamma^{0}";
   }

   std::string calolayer;

   if (4 > m_calolayer > 0)
   {
      calolayer = "EMB" + std::to_string(m_calolayer);
   }
   else if (m_calolayer == 0)
   {
      calolayer = "PreSamplerB";
   }
   else if (m_calolayer == 12)
   {
      calolayer = "TileBar0";
   }
   else if (m_calolayer == 13)
   {
      calolayer = "TileBar1";
   }
   else if (m_calolayer == 14)
   {
      calolayer = "TileBar2";
   }

   std::string binpca;

   binpca = "bin(PCA) = " + std::to_string(m_PCAbin);

   TFile *NNinputFile  = TFile::Open(m_NNinputName.c_str());
   TH2F *h = (TH2F*)NNinputFile->Get("hLnEnergyDensity");
   int final_nbinsR = h->GetNbinsY();
   int final_nbinsAlpha = h->GetNbinsX();

   std::string bin;

   bin = "#alpha nbins = " + std::to_string(final_nbinsAlpha) + ", r nbins = " + std::to_string(final_nbinsR);

   std::string label = energy + " " + particle + ", " + eta /*+ ", " + calolayer + ", " + binpca + ", " + bin*/;

   return label;
}


void ShowerShapePlotting::SetCustomPalette()
{
   /*
    * const Int_t NRGBs = 6;
    * const Int_t NCont = 50;
    *
    * Double_t stops[NRGBs] = { 0.00, 0.1, 0.34, 0.61, 0.84, 1.00 };
    * Double_t red[NRGBs]   = { 0.99, 0.0, 0.00, 0.87, 1.00, 0.51 };
    * Double_t green[NRGBs] = { 0.00, 0.0, 0.81, 1.00, 0.20, 0.00 };
    * Double_t blue[NRGBs]  = { 0.99, 0.0, 1.00, 0.12, 0.00, 0.00 };
    *
    *
    * TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    * gStyle->SetNumberContours(NCont);
    *
    *
    */

   Double_t stops[5] = { 0.00, 0.24, 0.61, 0.84, 1.00 };
   Double_t red[5]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
   Double_t green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
   Double_t blue[5]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };

   UInt_t npoints   = 5;
   Int_t  ncontours = 20;

   TColor::CreateGradientColorTable(npoints, stops, red, green, blue, ncontours);
   gStyle->SetNumberContours(ncontours);
}


void ShowerShapePlotting::CreateHTML(std::vector<string> histNameVec)
{
   cout << " * Creating HTML with plots ... " << endl;

   //  std::string file = m_outputDirName + "../" + m_particle + "_" + std::to_string(m_particleEnergy) + "_eta_" + std::to_string(m_etamin) + "_" + std::to_string(m_etamax) + "_nbinsR" + std::to_string(m_nbinsR) + "_neuron" + std::to_string(m_neurons) + ".html";

   std::string file = m_outputDirName + "../" + m_particle + "_" + std::to_string(m_particleEnergy) + "_eta_" + std::to_string(m_etamin) + "_" + std::to_string(m_etamax) + "_nbinsR" + std::to_string(m_nbinsR) + "_layer" + std::to_string(m_calolayer) + "_pca" + std::to_string(m_PCAbin) + ".html";

   std::string labeltitle = CreateLabels();

   //  std::string outdir = "plots_nbinsR" + std::to_string(m_nbinsR) + "_neuron" + std::to_string(m_neurons) + "/";

   std::string outdir = "plots_layer" + std::to_string(m_calolayer) + "_pca" + std::to_string(m_PCAbin) + "/";

   ofstream myhtml;
   myhtml.open(file.c_str(), std::ios_base::trunc);

   if (myhtml.is_open())
   {
      myhtml << "<p align=\"center\" class=\"style3\"> Plots for : " << labeltitle.c_str() << "</p>\n";
      myhtml << "<table align=\"center\" border=\"1\" cellspacing=\"0\" cellpadding=\"3\"><tr>\n";

      myhtml << "<td align=\"center\" class=\"style2\">  </td>\n";
      myhtml << "<td align=\"center\" class=\"style2\">Validation</td>\n";
      myhtml << "<td align=\"center\" class=\"style2\">Gradient</td>\n";
      myhtml << "<td align=\"center\" class=\"style2\"> </td>\n";
      myhtml << "</tr><tr>\n";
      myhtml << "<td align=\"left\" class=\"style2\" ><br> " << "input" << " </td>\n";
      myhtml << "<td align=\"center\" class=\"style1\" ><a  href=\"" << outdir.c_str() << "/" << "ValidationPlot.png" << "\"><img src=\"" << outdir.c_str() << "/" << "ValidationPlot.png" << "\" width=\"450\" height=\"450\"  ></a><br> (input)</td>\n";
      myhtml << "<td align=\"center\" class=\"style1\" ><a  href=\"" << outdir.c_str() << "/" << "EnergyDensityGradient.png" << "\"><img src=\"" << outdir.c_str() << "/" << "EnergyDensityGradient.png" << "\" width=\"450\" height=\"450\"  ></a><br> (output)</td>\n";
      myhtml << "<td align=\"center\" class=\"style1\" ><a  href=\"" << outdir.c_str() << "/" << "" << "\"><img src=\"" << outdir.c_str() << "/" << "" << "\" width=\"450\" height=\"450\"  ></a><br> (output)</td>\n";
      myhtml << "</tr><tr>\n";

      myhtml << "<td align=\"center\" class=\"style2\">Variable</td>\n";
      myhtml << "<td align=\"center\" class=\"style2\">NN input</td>\n";
      myhtml << "<td align=\"center\" class=\"style2\">NN ouput</td>\n";
      myhtml << "<td align=\"center\" class=\"style2\">Ratio (input/ouput) </td>\n";
      myhtml << "</tr><tr>\n";



      for (unsigned int ihist = 0; ihist < histNameVec.size(); ihist++)
      {
         for (int izoom = 0; izoom < 3; izoom++)
         {
            std::string NNinputFig  = "NNinput_" + histNameVec.at(ihist) + "_" + "zoom" + std::to_string(izoom) + ".png";
            std::string NNoutputFig = "NNoutput_" + histNameVec.at(ihist) + "_" + "zoom" + std::to_string(izoom) + ".png";
            std::string NNratioFig  = "Ratio_" + histNameVec.at(ihist) + "_" + "zoom" + std::to_string(izoom) + ".png";

            myhtml << "<td align=\"left\" class=\"style2\" ><br> " << (histNameVec.at(ihist) + "_zoom" + std::to_string(izoom)).c_str() << " </td>\n";
            myhtml << "<td align=\"center\" class=\"style1\" ><a  href=\"" << outdir.c_str() << "/" << NNinputFig << "\"><img src=\"" << outdir.c_str() << "/" << NNinputFig << "\" width=\"450\" height=\"450\"  ></a><br> (input)</td>\n";
            myhtml << "<td align=\"center\" class=\"style1\" ><a  href=\"" << outdir.c_str() << "/" << NNoutputFig << "\"><img src=\"" << outdir.c_str() << "/" << NNoutputFig << "\" width=\"450\" height=\"450\"  ></a><br> (output)</td>\n";
            myhtml << "<td align=\"center\" class=\"style1\" ><a  href=\"" << outdir.c_str() << "/" << NNratioFig << "\"><img src=\"" << outdir.c_str() << "/" << NNratioFig << "\" width=\"450\" height=\"450\"  ></a><br> (output)</td>\n";

            myhtml << "</tr><tr>\n";
         }
      }
      myhtml << "</body>\n";
      myhtml << "</html>\n";
      myhtml.close();
   }
   else
   {
      std::cout << "Error opening html file";
   }
}


void ShowerShapePlotting::CreatePlotBook(std::vector<string> histNameVec)
{
   cout << " * Creating  plot book ... " << endl;

   //  std::string file = m_outputDirName + "../plot_books/" + m_particle + "_" + std::to_string(m_particleEnergy) + "_eta_" + std::to_string(m_etamin) + "_" + std::to_string(m_etamax) + "_nbinsR" + std::to_string(m_nbinsR) + "_layer" + std::to_string(m_calolayer) + "_pca" + std::to_string(m_PCAbin) + ".tex";

   std::string file = m_outputDirName + "../plot_books/" + m_particle + "_" + std::to_string(m_particleEnergy) + "_eta_" + std::to_string(m_etamin) + "_" + std::to_string(m_etamax) + /*"_nbinsR" + std::to_string(m_nbinsR) +*/ ".tex";

   std::string labeltitle = CreateLabels();


   // std::string outdir = "../plots_nbinsR" + std::to_string(m_nbinsR) + "_layer" + std::to_string(m_calolayer) + "_pca" + std::to_string(m_PCAbin) + "/";

   std::string outdir = "../plots_layer" + std::to_string(m_calolayer) + "_pca" + std::to_string(m_PCAbin) + "/";


   ofstream mylatex;
   mylatex.open(file.c_str());

   if (mylatex.is_open())
   {
      mylatex << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
      mylatex << "\\documentclass[english,professionalfonts]{beamer}" << endl;
      mylatex << "\\usefonttheme{serif}" << endl;
      mylatex << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
      mylatex << "\\usepackage{beamerthemesplit}" << endl;
      mylatex << "\\usepackage{multicol}" << endl;
      mylatex << "\\usepackage{amsmath}" << endl;
      mylatex << "\\usepackage{amssymb}" << endl;
      mylatex << "\\usepackage{array}" << endl;
      mylatex << "\\usepackage{graphicx}" << endl;
      mylatex << "\\usepackage{multimedia}" << endl;
      mylatex << "\\usepackage{hyperref}" << endl;
      mylatex << "\\usepackage{url}" << endl;
      mylatex << "%% Define a new 'leo' style for the package that will use a smaller font." << endl;
      mylatex << "\\makeatletter" << endl;
      mylatex << "\\def\\url@leostyle{\\@ifundefined{selectfont}{\\def\\UrlFont{\\sf}}{\\def\\UrlFont{\\small\\ttfamily}}}" << endl;
      mylatex << "\\makeatother" << endl;
      mylatex << "%% Now actually use the newly defined style." << endl;
      mylatex << "\\urlstyle{leo}" << endl;
      mylatex << "\\usepackage{cancel}" << endl;
      mylatex << "\\usepackage{color}" << endl;
      mylatex << "\\usepackage{verbatim}" << endl;
      mylatex << "\\usepackage{epsfig}" << endl;
      mylatex << "\\usepackage{fancybox}" << endl;
      mylatex << "\\usepackage{xcolor}" << endl;
      mylatex << "%\\usepackage{fontspec}" << endl;
      mylatex << "%\\usepackage{booktabs,caption}" << endl;
      mylatex << "%\\usepackage{eulervm}" << endl;
      mylatex << "\\usepackage{textpos}" << endl;
      mylatex << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
      mylatex << "\\usetheme{Warsaw}" << endl;
      mylatex << "\\usecolortheme{default}" << endl;
      mylatex << "%\\setbeamercovered{transparent}" << endl;
      mylatex << "\\beamertemplatenavigationsymbolsempty" << endl;
      mylatex << "%\\setbeamertemplate" << endl;
      mylatex << "\\setbeamertemplate{footline}[page number]" << endl;
      mylatex << "%%\\setsansfont{Fontin Sans}" << endl;
      mylatex << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
      mylatex << "\\definecolor{jgreen}{cmyk}{0.99,0,0.52,0}" << endl;
      mylatex << "\\definecolor{green}{rgb}{0.,.6,0.}" << endl;
      mylatex << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
      mylatex << "\\title{Shower shape parametrization : Plotbook}" << endl;
      mylatex << "\\author{ Hasib Ahmed}" << endl;
      mylatex << "\\institute{ University of Edinburgh}" << endl;
      mylatex << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
      mylatex << "\\begin{document}" << endl;


      mylatex << "\\begin{frame}" << endl;
      mylatex << "\\frametitle{Parameters}" << endl;
      mylatex << "\\begin{center}" << endl;
      mylatex << "\\begin{itemize}" << endl;
      // mylatex << "\\item Particle = " + m_particle << endl;
      mylatex << "\\item Energy = " + std::to_string(m_particleEnergy) + "GeV" << endl;
      mylatex << "\\item " + std::to_string(m_etamin) + "$< |\\eta| <$" + std::to_string(m_etamax) << endl;
      mylatex << "\\item Calorimeter layer = " + std::to_string(m_calolayer) << endl;
      mylatex << "\\item PCA bin = " + std::to_string(m_PCAbin) << endl;
      mylatex << "\\end{itemize}" << endl;
      mylatex << "\\end{center}" << endl;
      mylatex << "\\end{frame}" << endl;


      for (unsigned int ihist = 0; ihist < histNameVec.size(); ihist++)
      {
         for (int izoom = 0; izoom < 3; izoom++)
         {
            std::string NNinputFig  = "NNinput_" + histNameVec.at(ihist) + "_" + "zoom" + std::to_string(izoom) + ".png";
            std::string NNoutputFig = "NNoutput_" + histNameVec.at(ihist) + "_" + "zoom" + std::to_string(izoom) + ".png";
            std::string NNratioFig  = "Ratio_" + histNameVec.at(ihist) + "_" + "zoom" + std::to_string(izoom) + ".png";

            mylatex << "\\begin{frame}" << endl;
            mylatex << "\\frametitle{Input distro: " + histNameVec.at(ihist) + "Zoom " + std::to_string(izoom) + "}" << endl;
            mylatex << "\\begin{center}" << endl;
            mylatex << "\\includegraphics[width=8cm]{" + outdir + "/" + NNinputFig + "}" << endl;
            mylatex << "\\end{center}" << endl;
            mylatex << "\\end{frame}" << endl;
         }
      }
      mylatex << "\\end{document}" << endl;
      mylatex.close();
   }
   else
   {
      cout << " ERROR::The python file cannot be opened!" << endl;
   }
}
