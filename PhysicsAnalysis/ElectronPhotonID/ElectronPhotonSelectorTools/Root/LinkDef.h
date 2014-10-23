/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ELECTRONPHOTONSELECTORTOOLS__ 
#define __ELECTRONPHOTONSELECTORTOOLS__
	
#include <vector>
#include <string>
#include "TString.h"

#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronPhotonIsEMSelectorConfigHelper.h"

#include "ElectronPhotonSelectorTools/TElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/TPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/TElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace LikeEnum;
#pragma link C++ enum LikeEnum::Menu;
#pragma link C++ namespace egammaPID;
#pragma link C++ enum egammaPID::egammaIDQuality;

#pragma link C++ class AsgElectronIsEMSelector+ ;
#pragma link C++ class AsgPhotonIsEMSelector+ ;
#pragma link C++ class AsgElectronMultiLeptonSelector+ ;
#pragma link C++ class AsgElectronLikelihoodTool+;
#pragma link C++ namespace AsgConfigHelper;
#pragma link C++ function  AsgConfigHelper::HelperFloat;
#pragma link C++ function  AsgConfigHelper::HelperInt;

#pragma link C++ namespace Root;
#pragma link C++ class Root::TElectronIsEMSelector+ ;
#pragma link C++ class Root::TPhotonIsEMSelector+ ;
#pragma link C++ class Root::TElectronMultiLeptonSelector+ ;
#pragma link C++ class Root::ITElectronLikelihoodTool+;
#pragma link C++ class Root::TElectronLikelihoodTool+;

#endif 

#endif

