/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUSYTOOLS_LINKDEF_H
#define SUSYTOOLS_LINKDEF_H

#include <vector>
#include <map>
#include "SUSYTools/SUSYObjDef.h" 
#include "SUSYTools/egammaPIDdefs.h"
#include "SUSYTools/MuonTriggerSFTool.h"
#include "SUSYTools/CalibrationDataInterfaceBase.h"
#include "SUSYTools/CalibrationDataInterfaceROOT.h"
#include "SUSYTools/BTagCalib.h"
#include "SUSYTools/APEvtWeight.h"
#include "SUSYTools/APReweightND.h"
#include "SUSYTools/APWeightEntry.h"
#include "SUSYTools/FakeMetEstimator.h"
#include "SUSYTools/TJVFReweighting.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ namespace egammaPID;
#pragma link C++ namespace SUSY;
#pragma link C++ namespace BTagCalib;

#pragma link C++ class SUSYObjDef+;
//#pragma link C++ class Reco_dpd2+;
//#pragma link C++ class Reco0Lep+;
//#pragma link C++ class Reco2Lep+;
#pragma link C++ class SUSY::CalibrationDataVariables+;
#pragma link C++ class SUSY::CalibrationDataInterfaceBase+;
#pragma link C++ class SUSY::CalibrationDataInterfaceROOT+;
#pragma link C++ class FakeMetEstimator+;
#pragma link C++ class APWeightEntry-;
#pragma link C++ class APReweightND+;
#pragma link C++ class APEvtWeight+;
#pragma link C++ class MuonTriggerSFTool+;
#pragma link C++ class SUSY::TJVFReweighting+;

#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<double> >+;
#pragma link C++ class pair<float,vector<float> >+;

#endif

#endif
