/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  
     @class JetCaloVariables
     Calculates jet variables based on calorimeter information 
     @author Yingchun Zhu
     @date: Jan. 28, 2010
*/

#ifndef JETUTIL_JETCALOVARIABLES_H 
#define JETUTIL_JETCALOVARIABLES_H

//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/ServiceHandle.h"

//#include "JetEvent/Jet.h"

class TFile;
class TH1;
class TH2;

class JetCaloVariables {
  public:
  JetCaloVariables(const std::string& m_JetContainerName);
  ~JetCaloVariables(); 


  /* Rcone is calculated by using all cells with positive energy inside jet */
  static double  compute_RCone(const Jet* jet);

  /* energy fraction of the energetic cells inside jet. Only 2 energetic cells in 
   * had. calorimeter and 32 energetic cells in em calorimeeter are used here. */
  static double  compute_RatioLeadingCells(const Jet* jet);

  /* compute LLR obtained with jet EM fraction*/
  double  compute_LLREmFraction(const Jet* jet);

  /* compute LLR obtained with Jet Rcone and Ratio_{LeadingCells}
   * NOTE: we did 2-dim pdf for Rcone and Ratio_{LeadingCells} because of
   * the strong correlation between them. */
  double  compute_LLRRconeANDRatio(const Jet* jet);
  
  static double  compute_jetTime(const Jet* jet);

  /* 1-dim pdf obtained with jet EM fraction. 2-dim pdf obtained with jet Rcone and Ratio_{LeadingCells}.
   * Such pdfs are done in different bins of jet pt: 20-40GeV, 40-80GeV, 80-160GeV and >160GeV.
   * NOTE: Only pdfs for Cone4H1TopoJets and AntiKt4H1TopoJets are provided. */
  StatusCode getpdf();
              
 private:

  std::vector<TH1*> cosmicPdf_EMFraction;
  std::vector<TH1*> dijetPdf_EMFraction;
  std::vector<TH2*> cosmicPdf_RconeANDRatio;
  std::vector<TH2*> dijetPdf_RconeANDRatio;
  std::vector<double> ptbin_pdf;
  TFile *filePDF;  
  std::string JetContainerName;

};
#endif
