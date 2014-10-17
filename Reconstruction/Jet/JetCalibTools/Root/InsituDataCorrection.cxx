/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"
#include "PathResolver/PathResolver.h"

InsituDataCorrection::InsituDataCorrection()
  : asg::AsgTool( "InsituDataCorrection::InsituDataCorrection" ), JetCalibrationToolBase::JetCalibrationToolBase(),
    m_config(NULL), m_insituCorr(NULL)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_insituCorr(NULL)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name, TEnv * config, TString jetAlgo)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_insituCorr(NULL)
{ }

InsituDataCorrection::~InsituDataCorrection() {

  if(m_insituCorr) delete m_insituCorr;

}

StatusCode InsituDataCorrection::initializeTool(const std::string&) {

  m_jetStartScale = m_config->GetValue("InsituStartingScale","JetGSCScaleMomentum");

  //Retrieve the name of root file containing the in-situ calibration histograms from the config
  TString insitu_filename = m_config->GetValue("InsituCalibrationFile","None");
  //Retrieve the name of the histogram for the relative in-situ calibration
  TString rel_histoname = m_config->GetValue("RelativeInsituCalibrationHistogram","");
  //Retrieve the name of the histogram for the absolute in-situ calibration 
  TString abs_histoname = m_config->GetValue("AbsoluteInsituCalibrationHistogram","");
  //Retrieve the description/name of the in-situ calibration
  TString insitu_desc = m_config->GetValue("InsituCalibrationDescription","");

  //Find the absolute path to the insitu root file
  if ( !insitu_filename.EqualTo("None") )
    insitu_filename=PathResolverFindCalibFile(insitu_filename.Data());
    //insitu_filename=FindFile(insitu_filename);
  
  TFile *insitu_file = TFile::Open(insitu_filename);
  if ( !insitu_file ) { ATH_MSG_FATAL( "Cannot open InsituCalibrationFile: " << insitu_filename ); return StatusCode::FAILURE; }

  ATH_MSG_INFO("\n  Reading In-situ correction factors from:\n    " << insitu_filename << "\n\n");

  rel_histoname.ReplaceAll("JETALGO",m_jetAlgo); abs_histoname.ReplaceAll("JETALGO",m_jetAlgo);
  TH2D * rel_histo = (TH2D*)JetCalibUtils::GetHisto2(insitu_file,rel_histoname);
  TH1D * abs_histo = (TH1D*)JetCalibUtils::GetHisto(insitu_file,abs_histoname);
  if ( !rel_histo || !abs_histo ) {
    ATH_MSG_FATAL( "\n  Tool configured for data, but no residual in-situ histograms could be retrieved. Aborting..." );
    return StatusCode::FAILURE;
  }
  else {
    gROOT->cd();
    m_insituCorr = combineCalibration(rel_histo,abs_histo);
    m_insituEtaMax = m_insituCorr->GetYaxis()->GetBinLowEdge(m_insituCorr->GetNbinsY()+1);
    m_insituPtMin = m_insituCorr->GetXaxis()->GetBinLowEdge(1);
    m_insituPtMax = m_insituCorr->GetXaxis()->GetBinLowEdge(m_insituCorr->GetNbinsX()+1);
    insitu_file->Close();
      
	  ATH_MSG_INFO( "\n  Tool configured to calibrate data.\n  In-situ correction to be applied:\n    " << insitu_desc << "\n" );
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;

}

StatusCode InsituDataCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const {

  xAOD::JetFourMom_t jetStartP4;
  ATH_CHECK( setStartP4(jet) );
  jetStartP4 = jet.jetP4();

  float detectorEta = jet.getAttribute<float>("DetectorEta");
  xAOD::JetFourMom_t calibP4=jetStartP4*getInsituCorr( jetStartP4.pt(), detectorEta );

  //Transfer calibrated jet properties to the Jet object
  jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentum",calibP4);
  jet.setJetP4( calibP4 );

  return StatusCode::SUCCESS;
}

double InsituDataCorrection::getInsituCorr(double pt, double eta) const {
  if (m_insituCorr==NULL) return 1.0;
  double myEta = eta, myPt = pt/m_GeV;

  //protection against values outside the histogram range, snap back to the lowest/highest bin edge
  if (myEta <= -m_insituEtaMax) myEta = 1e-6 - m_insituEtaMax;
  else if (myEta >= m_insituEtaMax) myEta = m_insituEtaMax - 1e-6;
  if ( myPt <= m_insituPtMin ) myPt = m_insituPtMin + 1e-6;
  else if ( myPt >= m_insituPtMax ) myPt = m_insituPtMax - 1e-6;
  return m_insituCorr->Interpolate(myPt,myEta);
}

TH2D * InsituDataCorrection::combineCalibration(TH2D *h2d, TH1D *h) {
  TH2D *prod = (TH2D*)h2d->Clone();
  for (int xi=1;xi<=prod->GetNbinsX();xi++) {
    double pt=prod->GetXaxis()->GetBinCenter(xi);
    double R_abs=h->Interpolate(pt); // Rdata/RMC for the absolute scale
    //printf("pT = %7.1f GeV, abs calib: %.4f\n",pt,abs);
    for (int yi=1;yi<=prod->GetNbinsY();yi++) {
      double c_rel = h2d->GetBinContent(xi,yi); // 1/Rrel = RMC/Rdata
      prod->SetBinContent(xi,yi,c_rel/R_abs);
    }
  }
  return prod;
}
