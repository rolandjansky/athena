/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 /*
 *  Class definition of GSCTool - see Root/GSC.cxx for more details
 *  Joe Taenzer (joseph.taenzer@cern.ch), Nov 2012
 */

#ifndef JETCALIBTOOLS_NPVBEAMSPOTCORRECTION_H
#define JETCALIBTOOLS_NPVBEAMSPOTCORRECTION_H 1

#include <iostream>
#include <fstream>
#include <TROOT.h>
#include <TFile.h>
#include <TMath.h>
#include <TGraph.h>
#include <vector>

class NPVBeamspotCorrection {

 public:

  NPVBeamspotCorrection();
  virtual ~NPVBeamspotCorrection();

  void initNPVBeamspotCorrection();
  double GetNVertexBsCorrection(double nRecoVtx);

 private:

  void error(TString msg) 
  { printf("\nERROR - NPVBeamspotCorrection:\n\n  %s\n\n",msg.Data()); abort(); }

  TGraph* NVtx_NReconstructible_bs66mm();
  TGraph* NVtx_NReconstructible_bs47mm();

  //Graph which will be filled with expected number of reconstructed vertices for beamspot length = 66mm
  TGraph *m_g_nvtx_nreco_bs66mm; 
  //Graph which will be filled with expected number of reconstructed vertices for beamspot length = 47mm
  TGraph *m_g_nvtx_nreco_bs47mm;

  TGraph *m_invGraph;

  double m_NPVmin, m_NPVmax;

};

#endif
