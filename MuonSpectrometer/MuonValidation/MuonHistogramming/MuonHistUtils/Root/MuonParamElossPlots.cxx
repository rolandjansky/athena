/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonParamElossPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

MuonParamElossPlots::MuonParamElossPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir)
								   ,msInnerMatchChi2(NULL)
								   ,msOuterMatchChi2(NULL)
								   ,ELoss(NULL)
								   ,ELossDiffTruth(NULL)
								   ,ELossDiffTruthEta0_1p35(NULL)
								   ,ELossDiffTruthEta1p35_1p55(NULL)
								   ,ELossDiffTruthEta1p55_end(NULL)
								   ,ELossSigma(NULL)
								   ,paramELoss(NULL)
								   ,paramELossDiff(NULL)
   							           ,paramELossDiffTruth(NULL)
								   ,paramELossDiffTruthEta0_1p35(NULL)
								   ,paramELossDiffTruthEta1p35_1p55(NULL)
								   ,paramELossDiffTruthEta1p55_end(NULL)
								   ,measELoss(NULL)
								   ,measELossDiff(NULL)
								   ,measELossDiffTruth(NULL)
								   ,measELossDiffTruthEta0_1p35(NULL)
								   ,measELossDiffTruthEta1p35_1p55(NULL)
								   ,measELossDiffTruthEta1p55_end(NULL)
								   ,ELossType(NULL)
								   ,ELossTypeParametrPt(NULL)
								   ,ELossTypeNotIsoPt(NULL)
								   ,ELossTypeTailPt(NULL)
								   ,ELossTypeAllPt(NULL)
{}

void MuonParamElossPlots::initializePlots()
{
  msInnerMatchChi2 = Book1D("msInnerMatchChi2","inner match #chi^{2};inner match #chi^{2};;Entries",100,0.,100.);
  msOuterMatchChi2 = Book1D("msOuterMatchChi2","outer match #chi^{2};outer match #chi^{2};;Entries",100,0.,100.);
    
  ELoss = Book1D("ELoss","ELoss;ELoss [GeV];Entries",50,0,10);
  ELossDiffTruth = Book1D("ELossDiffTruth","Difference ELoss to truth ELoss);#Delta(fitted - truth) [GeV];Entries",100,-10.,10.);
  ELossDiffTruthEta0_1p35 = Book1D("ELossDiffTruthEta0_1p35","Difference ELoss to truth ELoss for 0<|#eta|<1.35;#Delta(fitted - truth) [GeV];Entries",100,-10.,10.);
  ELossDiffTruthEta1p35_1p55 = Book1D("ELossDiffTruthEta1p35_1p55","Difference ELoss to truth ELoss for 1.35<|#eta|<1.55;#Delta(fitted - truth) [GeV];Entries",100,-10.,10.);
  ELossDiffTruthEta1p55_end = Book1D("ELossDiffTruthEta1p55_end","Difference ELoss to truth ELoss for 1.55<|#eta|;#Delta(fitted - truth) [GeV];Entries",100,-10.,10.);
  ELossSigma = Book1D("ELossSigma","ELossSigma;ELossSgima [GeV];Entries",50,0,5);

  paramELoss = Book1D("paramELoss","paramELoss;paramELoss [GeV];Entries",50,0,10);
  paramELossDiff = Book1D("paramELossDiff","Difference paramELoss to fitted ELoss;#Delta(param-fitted) [GeV];Entries",100,-3.,1.);
  paramELossDiffTruth = Book1D("paramELossDiffTruth","Difference paramELoss to truth ELoss;#Delta(param-truth) [GeV];Entries",100,-10.,10.);
  paramELossDiffTruthEta0_1p35 = Book1D("paramELossDiffTruthEta0_1p35","Difference paramELoss to truth ELoss for 0<|#eta|<1.35;#Delta(param-truth) [GeV];Entries",100,-10.,10.);
  paramELossDiffTruthEta1p35_1p55 = Book1D("paramELossDiffTruthEta1p35_1p55","Difference paramELoss to truth ELoss for 1.35<|#eta|<1.55;#Delta(param-truth) [GeV];Entries",100,-10.,10.);
  paramELossDiffTruthEta1p55_end = Book1D("paramELossDiffTruthhEta1p55_end","Difference paramELoss to truth ELoss for 1.55<|#eta|;#Delta(param-truth) [GeV];Entries",100,-10.,10.);
 
  measELoss = Book1D("measELoss","measELoss;measELoss [GeV];Entries",50,0,10);
  measELossDiff = Book1D("measELossDiff","Difference measELoss to fitted ELoss);#Delta(meas- fitted) [GeV];Entries",100,-3.,1.);
  measELossDiffTruth = Book1D("measELossDiffTruth","Difference measELoss to truth ELoss);#Delta(meas- truth) [GeV];Entries",100,-10.,10.);
  measELossDiffTruthEta0_1p35 = Book1D("measELossDiffTruthEta0_1p35","Difference measELoss to truth ELoss for 0<|#eta|<1.35;#Delta(meas- truth) [GeV];Entries",100,-10.,10.);
  measELossDiffTruthEta1p35_1p55 = Book1D("measELossDiffTruthEta1p35_1p55","Difference measELoss to truth ELoss for 1.35<|#eta|<1.55;#Delta(meas- truth) [GeV];Entries",100,-10.,10.);
  measELossDiffTruthEta1p55_end = Book1D("measELossDiffTruthEta1p55_end","Difference measELoss to truth ELoss for 1.55<|#eta|;#Delta(meas- truth) [GeV];Entries",100,-10.,10.);
  
  ELossType = Book1D("ELossType","ELossType;Type;Entries",5,-0.5,4.5);
  ELossTypeParametrPt = Book1D("ELossTypeParametrPt","ELossType:Parametrized Pt;Pt [GeV];Entries",50,0,500);
  ELossTypeNotIsoPt = Book1D("ELossTypeNotIsoPt","ELossType:NotIsolated Pt;Pt [GeV];Entries",50,0,500);
  ELossTypeTailPt = Book1D("ELossTypeTailPt","ELossType:Tail Pt;Pt [GeV];Entries",50,0,500);
  ELossTypeAllPt = Book1D("ELossTypeAllPt","All ELossTypes Pt;Pt [GeV];Entries",50,0,500);
}

  void MuonParamElossPlots::fill(const xAOD::TruthParticle& truthmu, const xAOD::Muon& mu)
{
  FillPlot(msInnerMatchChi2,mu,xAOD::Muon::msInnerMatchChi2);
  FillPlot(msOuterMatchChi2,mu,xAOD::Muon::msOuterMatchChi2);
#ifndef XAOD_ANALYSIS
  FillPlot(ELoss,ELossDiffTruth,ELossDiffTruthEta0_1p35,ELossDiffTruthEta1p35_1p55,ELossDiffTruthEta1p55_end,truthmu,mu,xAOD::Muon::EnergyLoss,0.001);
  FillPlot(measELoss,measELossDiffTruth,measELossDiffTruthEta0_1p35,measELossDiffTruthEta1p35_1p55,measELossDiffTruthEta1p55_end,truthmu,mu,xAOD::Muon::MeasEnergyLoss,0.001);  
  FillPlot(ELossSigma,mu,xAOD::Muon::EnergyLossSigma,0.001);
  FillPlot(paramELoss,paramELossDiffTruth,paramELossDiffTruthEta0_1p35,paramELossDiffTruthEta1p35_1p55,paramELossDiffTruthEta1p55_end,truthmu,mu,xAOD::Muon::ParamEnergyLoss,0.001);
  FillPlotELossType(ELossType,mu,0.001);
  FillPlotELossType(ELossTypeNotIsoPt,mu,xAOD::Muon::NotIsolated,0.001);
  FillPlotELossType(ELossTypeParametrPt,mu,xAOD::Muon::Parametrized,0.001);
  FillPlotELossType(ELossTypeTailPt,mu,xAOD::Muon::Tail,0.001);
#endif // not XAOD_ANALYSIS
}
   void MuonParamElossPlots::FillPlot(TH1* hist, const xAOD::Muon& mu,const xAOD::Muon::ParamDef paramDef,float scale) {
  if (mu.author()==xAOD::Muon::CaloTag || mu.author()==xAOD::Muon::CaloLikelihood || mu.author()==xAOD::Muon::ExtrapolateMuonToIP) return; //protection
  float fpar = 0;
  if (mu.isAvailable<float>("EnergyLoss")) {
     if (mu.parameter(fpar, paramDef)) 
       hist->Fill(scale*fpar); //scale to GeV, if needed
  }
  return;
} 
  void MuonParamElossPlots::FillPlot(TH1* hist, TH1* hist_DiffTruth, TH1* hist_DiffTruthEta0_1p35,  TH1* hist_DiffTruthEta1p35_1p55, TH1* hist_DiffTruthEta1p55_end, const xAOD::TruthParticle& truthprt, const xAOD::Muon& mu,const xAOD::Muon::ParamDef paramDef,float scale) {
  if (mu.author()==xAOD::Muon::CaloTag || mu.author()==xAOD::Muon::CaloLikelihood || mu.author()==xAOD::Muon::ExtrapolateMuonToIP) return; //protection
  float fpar = 0;
  if (mu.isAvailable<float>("EnergyLoss")) {
     if (mu.parameter(fpar, paramDef)) 
       hist->Fill(scale*fpar); //scale to GeV, if needed
 
     //get true energy loss
     if (/*!truthprt.isAvailable<float>("CaloEntryLayer_px") ||
	 !truthprt.isAvailable<float>("CaloEntryLayer_py") ||
	 !truthprt.isAvailable<float>("CaloEntryLayer_pz") ||*/
	 !truthprt.isAvailable<float>("px") ||
	 !truthprt.isAvailable<float>("py") ||
	 !truthprt.isAvailable<float>("pz") ||
	 !truthprt.isAvailable<float>("MuonEntryLayer_px") ||
	 !truthprt.isAvailable<float>("MuonEntryLayer_py") ||
	 !truthprt.isAvailable<float>("MuonEntryLayer_pz")) return;

     //workaround for missing caloentry
     TVector3 vecCaloEntry(truthprt.auxdata<float>("px"),
			   truthprt.auxdata<float>("py"),
			   truthprt.auxdata<float>("pz"));

     /* TVector3 vecCaloEntry(truthprt.auxdata<float>("CaloEntryLayer_px"),
			  truthprt.auxdata<float>("CaloEntryLayer_py"),
			  truthprt.auxdata<float>("CaloEntryLayer_pz"));*/

     TVector3 vecMuonExit(truthprt.auxdata<float>("MuonEntryLayer_px"),
			truthprt.auxdata<float>("MuonEntryLayer_py"),
			truthprt.auxdata<float>("MuonEntryLayer_pz"));
     float dpTruth=vecCaloEntry.Mag()-vecMuonExit.Mag();
     hist_DiffTruth->Fill(scale*(fpar-dpTruth)); //scale to GeV, if needed
     //again in eta ranges
     if (fabs(mu.eta())<1.35) hist_DiffTruthEta0_1p35->Fill(scale*(fpar-dpTruth)); 
     else  {
       if (fabs(mu.eta())<1.55) hist_DiffTruthEta1p35_1p55->Fill(scale*(fpar-dpTruth)); 
       else hist_DiffTruthEta1p55_end->Fill(scale*(fpar-dpTruth)); 
     }
  }
  return;
}
  void MuonParamElossPlots::FillPlotELossType(TH1* hist, const xAOD::Muon& mu, float scale) {
  if (mu.author()==xAOD::Muon::CaloTag || mu.author()==xAOD::Muon::CaloLikelihood || mu.author()==xAOD::Muon::ExtrapolateMuonToIP) return; //protection
  if (mu.isAvailable<float>("EnergyLoss")) {
    hist->Fill(mu.energyLossType()); 
    ELossTypeAllPt->Fill(mu.pt()*scale);
    float used=0;
    float meas=-100000;
    float param=-100000;
    if (mu.parameter(used, xAOD::Muon::EnergyLoss) &&  mu.parameter(param, xAOD::Muon::ParamEnergyLoss) && mu.parameter(meas, xAOD::Muon::MeasEnergyLoss ) ){
      //fill diff plots
      if (mu.energyLossType()==xAOD::Muon::Parametrized){
	paramELossDiff->Fill((param-used)*scale);
      }
      if (mu.energyLossType()==xAOD::Muon::Tail){
	measELossDiff->Fill((meas-used)*scale);
      }
    }
  }
  return;
}
void MuonParamElossPlots::FillPlotELossType(TH1* hist, const xAOD::Muon& mu, const xAOD::Muon::EnergyLossType type,  float scale) {
  if (mu.author()==xAOD::Muon::CaloTag || mu.author()==xAOD::Muon::CaloLikelihood || mu.author()==xAOD::Muon::ExtrapolateMuonToIP) return; //protection
  if (mu.isAvailable<float>("EnergyLoss")) {
    if (mu.energyLossType()==type) hist->Fill(mu.pt()*scale); //scale to GeV, if needed
  }
  return;
}
}//namespace Muon
