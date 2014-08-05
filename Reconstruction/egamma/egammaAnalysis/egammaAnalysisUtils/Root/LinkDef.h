/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef __CINT__


#include "egammaAnalysisUtils/EnergyRescaler.h"
#include "egammaAnalysisUtils/EnergyRescalerUpgrade.h"
#include "egammaAnalysisUtils/CaloIsoCorrection.h"
#include "egammaAnalysisUtils/ConvertedPhotonScaleTool.h"
#include "egammaAnalysisUtils/EisoTool.h"
#include "egammaAnalysisUtils/EisoTool2012.h"
#include "egammaAnalysisUtils/FsrPhotons.h"
#include "egammaAnalysisUtils/VertexPositionReweightingTool.h"
#include "egammaAnalysisUtils/BosonPtReweightingTool.h"
#include "egammaAnalysisUtils/PhotonIDTool.h"
#include "egammaAnalysisUtils/FudgeMCTool.h"
#include "egammaAnalysisUtils/PhotonEfficiencySFTool.h"
#include "egammaAnalysisUtils/CaloPointingCorrectionTool.h"
#include "egammaAnalysisUtils/DiphotonVertexIDTool.h"

#include<vector>

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ class eg2011::EnergyRescaler+; 
#pragma link C++ class eg2011::EnergyRescaler::calibMap+; 
#pragma link C++ class egRescaler::EnergyRescalerUpgrade+;
#pragma link C++ namespace CaloIsoCorrection;
#pragma link C++ class ConvertedPhotonScaleTool+;
#pragma link C++ class EisoTool;
#pragma link C++ class EisoTool2012;
#pragma link C++ class FsrPhotons+;
#pragma link C++ class VertexPositionReweightingTool;
#pragma link C++ class BosonPtReweightingTool;
#pragma link C++ class PhotonIDTool;
#pragma link C++ class FudgeMCTool;
#pragma link C++ class PhotonEfficiencySFTool;
#pragma link C++ class CaloPointingCorrectionTool;
#pragma link C++ class DiphotonVertexIDTool+;

#pragma link C++ class vector<vector<vector<int> > >+;
#pragma link C++ class vector<vector<vector<float> > >+;
#pragma link C++ class vector<vector<float> >+;

#endif
