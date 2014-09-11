/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JES__
#define __JES__

#include <map>
#include <TString.h>
#include "../ApplyJetCalibration/ApplyJetCalibration.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class JetAnalysisCalib::JetCalibrationTool+;
#pragma link C++ class GSCTool+;
#pragma link C++ class NPVBeamspotCorrectionTool+;
#pragma link C++ class KojiOriginCorrectionTool+;

#endif

#endif
