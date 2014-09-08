/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ELECTRONPHOTONSELECTORTOOLS__ 
#define __ELECTRONPHOTONSELECTORTOOLS__
	
#include <vector>
#include <string>
#include "TString.h"
#include "ElectronPhotonSelectorTools/TElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/TPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/TElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/TElectronLikelihoodTool.h"


#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace Root ;

#pragma link C++ class Root::TElectronIsEMSelector+ ;
#pragma link C++ class Root::TPhotonIsEMSelector+ ;
#pragma link C++ class Root::TElectronMultiLeptonSelector+ ;
#pragma link C++ class Root::TElectronLikelihoodTool+ ;

#pragma link C++ namespace LikeEnum;
#pragma link C++ enum LikeEnum::Menu;

#endif 

#endif

