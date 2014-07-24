/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <map>
#include <utility>
#include "TH1F.h"
#include "TH1D.h"
#include "TauCorrUncert/TauSF.h"
#include "TauCorrUncert/BDTSF.h"
#include "TauCorrUncert/TESUncertainty.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace TauCorrUncert;
#pragma link C++ class TauCorrUncert::CommonSF+ ;
#pragma link C++ class TauCorrUncert::TauSF+ ;
#pragma link C++ class TauCorrUncert::BDTSF+ ;
#pragma link C++ class TauCorrUncert::TESUncertainty+ ;

#pragma link C++ class std::pair<std::string, TH1D*>+ ;
#pragma link C++ class std::pair<std::string, TH1F*>+ ;
#pragma link C++ class std::map<std::string, TH1D*>+ ;
#pragma link C++ class std::map<std::string, TH1F*>+ ;

#endif
