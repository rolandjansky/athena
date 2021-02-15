/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Code to compare JES correlations for various reduced
 *  nuisance paramter lists
 *
 */

#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TError.h"
#include <iostream>
#include "../atlasstyle/AtlasStyle.C"
#include "../atlasstyle/AtlasLabels.C"


using namespace std;

typedef TString Str;
typedef MultijetJESUncertaintyProvider MultijetProvider;
typedef JESUncertaintyProvider JESprov;

typedef enum { ptE, etaE } Variable;

TLatex *tex;
const bool MAKE_CONF_PLOTS = true;
  

double getCov(JESprov *mjp, StrV names, Variable var, double vary1, double vary2, double fixed)
{
  double cov=0;
  
  for (unsigned int i = 0; i < names.size(); ++i)
    if (names[i].Contains("Pileup"))
      continue;
    else if (var==ptE)
      cov += mjp->getRelUncertComponent(i,vary1,fixed)*mjp->getRelUncertComponent(i,vary2,fixed);
    else if (var==etaE)
      cov += mjp->getRelUncertComponent(i,fixed,vary1)*mjp->getRelUncertComponent(i,fixed,vary2);

  return cov;
}

double getCorr(JESprov *mjp, StrV names, Variable var, double vary1, double vary2, double fixed) {
  return getCov(mjp,names,var,vary1,vary2,fixed)/sqrt(getCov(mjp,names,var,vary1,vary1,fixed)*getCov(mjp,names,var,vary2,vary2,fixed));
}

TH2D *MakeCorrMatrix(JESprov *mjp, StrV names, Variable var, VecD variableBins, double fixed) {
  static int icorr=0; int Nbins=variableBins.size()-1;
  TH2D *hcorr = new TH2D(Form("corr%d",++icorr),"",Nbins,&variableBins[0],Nbins,&variableBins[0]);
  for (int xi=1;xi<=Nbins;++xi)
    for (int yi=1;yi<=Nbins;++yi) {
      double vary1=hcorr->GetXaxis()->GetBinCenter(xi);
      double vary2=hcorr->GetYaxis()->GetBinCenter(yi);
      //printf("(%2d,%2d), %s: (%6.1f,%6.1f), corr: %.5f\n",xi,yi,var==ptE ? "pT" : "eta",vary1,vary2,getCorr(mjp,names,var,vary1,vary2,fixed));
      hcorr->SetBinContent(xi,yi,getCorr(mjp,names,var,vary1,vary2,fixed));
    }
  hcorr->SetStats(0); hcorr->SetMinimum(0); hcorr->SetMaximum(1);
  TString axisTitle = (var==ptE)? "#it{p}_{T}^{jet} [GeV]" : "jet #eta";
  hcorr->SetXTitle(axisTitle); hcorr->SetYTitle(axisTitle); hcorr->SetZTitle("correlation");
  return hcorr;
}

TH2D *Subtract(TH2D *h1, TH2D *h2) {
  TH2D *diff = (TH2D*)h2->Clone();
  int Nbins=diff->GetNbinsX();
  for (int xi=1;xi<=Nbins;++xi)
    for (int yi=1;yi<=Nbins;++yi) {
      diff->SetBinContent(xi,yi,h2->GetBinContent(xi,yi)-h1->GetBinContent(xi,yi));
    }
  diff->SetStats(0);
  diff->GetZaxis()->SetRangeUser(-0.30,0.30);  //diff->SetMinimum(-0.30); diff->SetMaximum(0.30);
  //diff->SetXTitle("jet p_{T} [GeV]"); diff->SetYTitle("jet p_{T} [GeV]"); diff->SetZTitle("correlation difference");
  diff->SetXTitle(h1->GetXaxis()->GetTitle()); diff->SetYTitle(h1->GetYaxis()->GetTitle()); diff->SetZTitle(h1->GetZaxis()->GetTitle());
  return diff;
}

// In arrays, min before max, x before y (ex: min x, min y, max x, max y)
// Returns 0 on success, else on error
int GetExtremaForTH2D(TH2D *h, int extremalIndices[4], double extremalAxisValues[4], double extremalValues[2])
{
  // Set values to enforce searching
  for (int i = 0; i < 4; i++)
    extremalIndices[i] = -1;

  extremalValues[0] = 1e99;
  extremalValues[1] = -1e99;
  
  int Nbins=h->GetNbinsX();
  for (int xi=1;xi<=Nbins;++xi)   
    for (int yi=1;yi<=Nbins;++yi)
    {
      if (h->GetBinContent(xi,yi) > extremalValues[1])
      {
        extremalValues[1] = h->GetBinContent(xi,yi);
        extremalIndices[2] = xi;
        extremalIndices[3] = yi;
        extremalAxisValues[2] = h->GetXaxis()->GetBinCenter(xi);
        extremalAxisValues[3] = h->GetYaxis()->GetBinCenter(yi);
      }
      else if (h->GetBinContent(xi,yi) < extremalValues[0])
      {
        extremalValues[0] = h->GetBinContent(xi,yi);
        extremalIndices[0] = xi;
        extremalIndices[1] = yi;
        extremalAxisValues[0] = h->GetXaxis()->GetBinCenter(xi);
        extremalAxisValues[1] = h->GetXaxis()->GetBinCenter(yi);
      }
    }

  //if (minBinIndices[0] > 0)
  //  printf("Minimum bin value: %f at indices=(%d,%d), pt=(%f,%f)\n",minBin,minBinIndices[0],minBinIndices[1],minBinValues[0],minBinValues[1]);
  //if (maxBinIndices[0] > 0)
  //  printf("Maximum bin value: %f at indices=(%d,%d), pt=(%f,%f)\n",maxBin,maxBinIndices[0],maxBinIndices[1],maxBinValues[0],maxBinValues[1]);
  if (extremalIndices[0] < 0 || extremalIndices[1] < 0 || extremalIndices[2] < 0 || extremalIndices[3] < 0)
    return -1;
  return 0;
}

TH1D *GetNuis(JESprov *mjp, Str name, Variable var, VecD variableBins, double fixed) {
  static int i=0;
  TH1D *nuis = new TH1D(Form("nuis%d",++i),"",variableBins.size()-1,&variableBins[0]);
  for (int i=1;i<=nuis->GetNbinsX();++i) {
    double vary=nuis->GetBinCenter(i);
    if (var==ptE)
      nuis->SetBinContent(i,mjp->getRelUncertComponent(name,vary,fixed));
    else if (var==etaE)
      nuis->SetBinContent(i,mjp->getRelUncertComponent(name,fixed,vary));
  }
  nuis->SetStats(0);
  TString axisTitle = (var==ptE)? "#it{p}_{T}^{jet} [GeV]" : "jet #eta";
  nuis->GetYaxis()->SetRangeUser(-0.02,0.05); nuis->SetXTitle(axisTitle);
  return nuis;
}

TH1D *GetTotUncert(JESprov *mjp, Variable var, VecD variableBins, double fixed) {
  static int i=0;
  TH1D *tot = new TH1D(Form("tot%d",++i),"",variableBins.size()-1,&variableBins[0]);
  for (int i=1;i<=tot->GetNbinsX();++i) {
    double vary=tot->GetBinCenter(i);
    if (var==ptE)
      tot->SetBinContent(i,mjp->getRelUncert(vary,fixed));
    else if (var==etaE)
      tot->SetBinContent(i,mjp->getRelUncert(fixed,vary));
  }
  tot->SetStats(0);
  TString axisTitle = (var==ptE)? "#it{p}_{T}^{jet} [GeV]" : "jet #eta";
  tot->GetYaxis()->SetRangeUser(0,0.05); tot->SetXTitle(axisTitle);
  return tot;
}

void *DrawNuis(JESprov *mjp, StrV names, Variable var, VecD variableBins, double fixed) {
  vector<TH1*> nuis; StrV myNames;
  for (int i=0;i<names.size();++i) {
    if (!names[i].Contains("Pile")) {
      nuis.push_back(GetNuis(mjp,names[i],var,variableBins,fixed));
      myNames.push_back(names[i]);
    }
  }

  int cols[] = {kBlue,kRed,kGreen+1,kOrange+2,kViolet+1,kSpring,kCyan+2,kGray+1};
  for (int i=0;i<nuis.size();++i) 
    nuis[i]->SetLineColor(cols[i%8]);
  nuis[0]->SetMinimum(-0.02);
  nuis[0]->Draw("L");
  for (int i=1;i<nuis.size();++i) nuis[i]->Draw("Lsame");
  tex->SetTextAlign(11);
  for (int i=0;i<nuis.size();++i) { tex->SetTextColor(cols[i%8]); tex->DrawLatex(0.18,0.88-0.05*i,myNames[i]); }
}

void FormatAndDrawCorrMatrix(TH2D* corrMatrix,TString JESconfig,TString jetDesc,Variable var, double fixed)
{
  gPad->SetMargin(0.01,0.01,0.01,0.01); 
  //gPad->SetMargin(0.15,0.05,0.15,0.05);
  if (var==ptE)
  {
    gPad->SetLogx();
    gPad->SetLogy();
  }
  else if (var==etaE)
  {
    gPad->SetLogx(false);
    gPad->SetLogy(false);
  }
  corrMatrix->Draw("colz");
  tex->SetTextColor(kBlack);
  TString titleWithoutPath = JESconfig;
  if (titleWithoutPath.Contains('/'))
    //titleWithoutPath.Remove(TString::EStripType(TString::kLeading),titleWithoutPath.Last('/'));
    titleWithoutPath.Replace(0,titleWithoutPath.Last('/')+1,0,0);
  tex->DrawLatex(0.18,0.96,titleWithoutPath); tex->DrawLatex(0.18,0.88,jetDesc);
  tex->DrawLatex(0.18,0.82,var==ptE ? Form("#eta = %.2f",fixed) : Form("p_{T} = %.1f GeV",fixed));
}

void FormatAndDrawCorrMatrixForConfNote(TH2D* corrMatrix,TString JESconfig,TString jetDesc,Variable var, double fixed)
{
  gPad->SetMargin(0.15,0.15,0.2,0.1); 
  //gPad->SetMargin(0.15,0.05,0.15,0.05);
  if (var==ptE)
  {
    gPad->SetLogx(true);
    gPad->SetLogy(true);
  }
  else if (var==etaE)
  {
    gPad->SetLogx(false);
    gPad->SetLogy(false);
  }
  
  corrMatrix->SetTitleSize(0.06,"z");
  corrMatrix->SetLabelSize(0.04,"z");
  corrMatrix->SetTitleOffset(1.35,"x");
  corrMatrix->SetTitleOffset(0.9,"y");
  corrMatrix->SetTitleOffset(0.85,"z");
  corrMatrix->SetLabelOffset(0.003,"x");
  corrMatrix->GetXaxis()->SetMoreLogLabels();
//  corrMatrix->GetYaxis()->SetMoreLogLabels();
  
  corrMatrix->Draw("colz");
  
  tex->SetTextColor(kBlack);
  TString titleWithoutPath = JESconfig;
  if (titleWithoutPath.Contains('/'))
    titleWithoutPath.Replace(0,titleWithoutPath.Last('/')+1,0,0);
  //tex->DrawLatex(0.18,0.96,titleWithoutPath);
  titleWithoutPath.ReplaceAll("InsituJES2011_","");
  titleWithoutPath.ReplaceAll(".config","");
//  tex->DrawLatex(0.18,0.88,titleWithoutPath);
//  tex->DrawLatex(0.18,0.88,"ATLAS Internal");
//  tex->DrawLatex(0.18,0.82,jetDesc);
//  tex->DrawLatex(0.18,0.76,var==ptE ? Form("#eta = %.2f",fixed) : Form("p_{T} = %.1f GeV",fixed));

  ATLASLabel(0.15,0.96,(char*)"Internal",kBlack);
  tex->DrawLatex(0.18,0.916,var==ptE ? Form("%s, #eta = %.2f, Data 2012",jetDesc.Data(),fixed) : Form("%s, #it{p}_{T}^{jet} = %.1f GeV, Data 2012",jetDesc.Data(),fixed));

}

void AddExtremaLabels(TH2D* matrix, Variable var)
{
  int extremalIndices[4];
  double extremalAxisValues[4], extremalValues[2];
  TString labelVar = (var==ptE) ? "p_{Tx},p_{Ty}" : "#eta_{x},#eta_{y}";
  if (!GetExtremaForTH2D(matrix,extremalIndices,extremalAxisValues,extremalValues))
  {
    if ( extremalValues[1] > fabs(extremalValues[0]) )
      tex->DrawLatex(0.10,0.96,Form("max=%.4lf at (x,y)=(%d,%d), (%s)=(%.1f,%.1f)",extremalValues[1],extremalIndices[2],extremalIndices[3],labelVar.Data(),extremalAxisValues[2],extremalAxisValues[3]));
    else
      tex->DrawLatex(0.10,0.96,Form("min=%.4lf at (x,y)=(%d,%d), (%s)=(%.1f,%.1f)",extremalValues[0],extremalIndices[0],extremalIndices[1],labelVar.Data(),extremalAxisValues[0],extremalAxisValues[1]));
    //tex->DrawLatex(0.13,0.17,Form("min=%.4lf at (x,y)=(%d,%d), (%s)=(%.1f,%.1f)",extremalValues[0],extremalIndices[0],extremalIndices[1],labelVar.Data(),extremalAxisValues[0],extremalAxisValues[1]));
  }
}
        
void FormatAndDrawNuisanceParamPlot(JESprov* jes1,JESprov* jes2,TString JESconfig2,TString jetDesc,StrV compNames2,Variable var,VecD variableBins,double fixed)
{
  // Only eta intercalibration component has any eta dependence, so this method doesn't really make sense to have enumerated functionality
  // Just don't plot the nuisance parameters for eta correlation matrix plots
  gPad->SetMargin(0.15,0.15,0.15,0.05);
  //gPad->SetMargin(0.15,0.05,0.15,0.05);
  tex->SetTextColor(kBlack);
  double size = tex->GetTextSize();
  tex->SetTextSize(0.04);
  if (var == ptE)
    gPad->SetLogx();
  else if (var == etaE)
    gPad->SetLogx(false);
  gPad->SetLogy(false);
  DrawNuis(jes2,compNames2,var,variableBins,fixed);
  TH1 *tot_orig = GetTotUncert(jes1,var,variableBins,fixed);
  TH1 *tot = GetTotUncert(jes2,var,variableBins,fixed);
  tot_orig->SetLineColor(kBlack); tot_orig->SetLineWidth(3); tot_orig->SetLineStyle(2); tot_orig->Draw("sameL");
  tot->SetLineColor(kRed); tot->SetLineWidth(2); tot->SetLineStyle(2); tot->Draw("sameL");

  tex->DrawLatex(0.5,0.6,"Reference tot uncert"); 

  tex->SetTextColor(kRed);
  tex->DrawLatex(0.5,0.55,"New tot uncert"); 

  tex->SetTextSize(size);
  tex->SetTextColor(kBlack);
  tex->DrawLatex(0.18,0.96,JESconfig2); 
  tex->DrawLatex(0.52,0.88,jetDesc);
  tex->DrawLatex(0.52,0.82,(var==ptE) ? Form("#eta = %.2f",fixed) : Form("p_{T} = %.1f GeV",fixed));
}

void CreateFormatAndPlotMatrices(JESprov* jes1,JESprov* jes2,StrV compNames1,StrV compNames2,TString JESconfig1,TString JESconfig2,TString jetDesc,TString psName,Variable var,VecD varyBins,VecD fixedVals)
{
  TCanvas *c = new TCanvas();
  c->Print(psName+"[");
  c->Divide(2,2);
  
  TCanvas *confCanvas;
  const TString CONFPREFIX = "CONF_";
  TString confpsName;
  if (MAKE_CONF_PLOTS)
  {
    confpsName = psName;
    if (! confpsName.Contains('/'))
      confpsName = CONFPREFIX+confpsName;
    else if (confpsName.First('/') == confpsName.Last('/'))
      confpsName = confpsName.ReplaceAll("/","/"+CONFPREFIX);
    else
      confpsName = confpsName.ReplaceAll(".ps",CONFPREFIX+".ps");

    confCanvas = new TCanvas();
    confCanvas->Print(confpsName+"[");
  }

  // Loop over the fixed values
  for (int ifix=0; ifix<fixedVals.size(); ifix++)
  {
    double fixed = fixedVals[ifix];

    // Create and plot the first matrix (upper left)
    TH2D* corr1 = MakeCorrMatrix(jes1,compNames1,var,varyBins,fixed);
    c->cd(1);
    FormatAndDrawCorrMatrix(corr1,JESconfig1,jetDesc,var,fixed);
    if (MAKE_CONF_PLOTS)
    {
      confCanvas->cd();
      FormatAndDrawCorrMatrixForConfNote(corr1,JESconfig1,jetDesc,var,fixed);
      confCanvas->Print(confpsName);
    }

    // Create and plot the second matrix (upper right)
    TH2D* corr2 = MakeCorrMatrix(jes2,compNames2,var,varyBins,fixed);
    c->cd(2);
    FormatAndDrawCorrMatrix(corr2,JESconfig2,jetDesc,var,fixed);
    if (MAKE_CONF_PLOTS)
    {
      confCanvas->cd();
      FormatAndDrawCorrMatrixForConfNote(corr2,JESconfig2,jetDesc,var,fixed);
      confCanvas->Print(confpsName);
    }

    // Create and plot the difference matrix (bottom left)
    // Also add extremal value labels to the plot
    TH2D* diff = Subtract(corr1,corr2);
    c->cd(3);
    FormatAndDrawCorrMatrix(diff,"",jetDesc,var,fixed);
    AddExtremaLabels(diff,var);
    if (MAKE_CONF_PLOTS)
    {
      confCanvas->cd();
      FormatAndDrawCorrMatrixForConfNote(diff,"",jetDesc,var,fixed);
      confCanvas->Print(confpsName);
    }

    // Nuisance parameter plot (bottom right) doesn't really make sense for eta
    // Only create if pt
    //if (var==ptE)
    {
      c->cd(4);
      FormatAndDrawNuisanceParamPlot(jes1,jes2,JESconfig2,jetDesc,compNames2,var,varyBins,fixed);
    }

    // Done, now print
    c->Print(psName);

  }
  c->Print(psName+"]"); // Close the ps file
  if (MAKE_CONF_PLOTS)
    confCanvas->Print(confpsName+"]");
  
  // Convert the ps file(s) to a pdf file
  Str pdf=Str(psName).ReplaceAll(".ps",".pdf");
  int stat = gSystem->Exec(Form("ps2pdf %s %s",psName.Data(),pdf.Data()));
  if (stat==0) {
    gSystem->Exec(Form("rm %s",psName.Data()));
    printf("\nProduced\n  %s\n",pdf.Data());
  }
  if (MAKE_CONF_PLOTS)
  {
    Str confpdf = Str(confpsName).ReplaceAll(".ps",".pdf");
    stat = gSystem->Exec(Form("ps2pdf %s %s",confpsName.Data(),confpdf.Data()));
    if (stat==0)
    {
      gSystem->Exec(Form("rm %s",confpsName.Data()));
      printf("  %s\n",confpdf.Data());
    }
  }
}

int main(int argc, char **argv) {

  SetAtlasStyle();
  gErrorIgnoreLevel=2000;
  gStyle->SetPalette(1);

  // Defaults
  TString sharePath = "../share";
  TString JESconfig1="InsituJES2011_AllNuisanceParameters.config";
  TString JESconfig2="InsituJES2011_AlternativeCorrelation1.config";
  StrV jetAlgos = JESUtils::Vectorize("AntiKt4TopoEM",",");
  TString MCtype="MC11b";
  bool varyPt  = true;
  bool varyEta = true;
  bool varyEtaCentral = true;

  // Override defaults with user input if provided
  if (argc>3) {
    sharePath =argv[1];
    JESconfig1=argv[2];
    JESconfig2=argv[3];
  }
  if (argc>4) jetAlgos = JESUtils::Vectorize(argv[4],",");
  if (argc>5) MCtype=argv[5];
  
  // Define the binning to be used
  VecD varyPtBins         = JESUtils::GetLogBins(100,15,2500);
  VecD fixedEtaVals       = JESUtils::VectorizeD("0.1,0.5,1.0,2.0,3.0,4.0",",");
  VecD varyEtaBins        = JESUtils::GetUniformBins(90,0,4.5);
  VecD fixedPtVals        = JESUtils::VectorizeD("25,45,80",",");
  VecD varyEtaCentralBins = JESUtils::GetUniformBins(30,0,1.5);
  VecD fixedPtCentralVals = JESUtils::VectorizeD("250,750",",");
  
  tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.055);
  gStyle->SetTitleSize(0.06,"xy");
  gStyle->SetTitleSize(0.06,"t");
  gStyle->SetLabelSize(0.06,"xy");
  //gErrorIgnoreLevel=2000;
  
  // Loop over all algorithms
  for (int jeti = 0; jeti < jetAlgos.size(); ++jeti)
  {
    
    JESprov *jes1, *jes2;
    //jes1 = new MultijetProvider("MultijetJES.config",JESconfig1,jetAlgo,MCtype);
    //jes2 = new MultijetProvider("MultijetJES.config",JESconfig2,jetAlgo,MCtype);
    jes1 = new JESprov(JESconfig1,jetAlgos[jeti],MCtype,sharePath);
    jes2 = new JESprov(JESconfig2,jetAlgos[jeti],MCtype,sharePath);
    jes1->useGeV(); jes2->useGeV();

    StrV compNames1 = jes1->getComponentNames();
    StrV compNames2 = jes2->getComponentNames();
    Str jetDesc(Form("Anti-#it{k}_{t} R = 0.%d, %s+JES",
          	   jetAlgos[jeti].Contains("Kt4")?4:6,
          	   jetAlgos[jeti].Contains("EM")?"EM":"LCW"));
    
    Str name=Str(gSystem->BaseName(JESconfig2)).ReplaceAll(".config",".ps");
    Str dir="Correlation_Plots";
    gSystem->Exec(Form("mkdir -p %s",dir.Data()));
    Variable var;
    if (varyPt)
    {
      var = ptE;
      // Create the output file name
      Str psName = dir+"/Corr_vs_pt_"+jetAlgos[jeti]+"_"+name;
      
      // Do all the work
      CreateFormatAndPlotMatrices(jes1,jes2,compNames1,compNames2,JESconfig1,JESconfig2,jetDesc,psName,var,varyPtBins,fixedEtaVals);
      
    }
    if (varyEta)
    {
      var = etaE;
      // Create the output file name
      Str psName = dir+"/Corr_vs_eta_"+jetAlgos[jeti]+"_"+name;
      
      // Do all the work
      CreateFormatAndPlotMatrices(jes1,jes2,compNames1,compNames2,JESconfig1,JESconfig2,jetDesc,psName,var,varyEtaBins,fixedPtVals);
    }
    if (varyEtaCentral)
    {
      var = etaE;
      // Create hte output file name
      Str psName = dir+"/Corr_vs_etaCentral_"+jetAlgos[jeti]+"_"+name;

      // Do all the work
      CreateFormatAndPlotMatrices(jes1,jes2,compNames1,compNames2,JESconfig1,JESconfig2,jetDesc,psName,var,varyEtaCentralBins,fixedPtCentralVals);
    }
    
    // Done with the providers, so remove them
    delete jes1;
    delete jes2;
  }
}
