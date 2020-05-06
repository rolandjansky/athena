/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceBase.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "CalibrationDataInterface/CalibrationDataEigenVariations.h"
#include "CalibrationDataInterface/CalibrationDataBackend.h"
#include "CalibrationDataInterface/CalibrationDataUtilities.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ namespace Analysis;

#pragma link C++ class Analysis::CalibrationDataInterfaceBase;
#pragma link C++ class Analysis::CalibrationDataInterfaceROOT+;
#pragma link C++ class Analysis::CalibrationDataInterfaceROOT::HadronisationReferenceHelper+;
#pragma link C++ class Analysis::CalibrationDataVariables;
#pragma link C++ class Analysis::CalibrationDataEigenVariations;

#pragma link C++ class Analysis::CalibrationDataContainer+;
#pragma link C++ class Analysis::CalibrationDataHistogramContainer+;
#pragma link C++ class Analysis::CalibrationDataMappedHistogramContainer+;
#pragma link C++ class Analysis::CalibrationDataMappedHistogramContainer::Bin+;
#pragma link C++ class Analysis::CalibrationDataFunctionContainer+;

#pragma link C++ function Analysis::addContainer(Analysis::CalibrationDataContainer*, TFile*, const char*, const char*, const char*, const char*, const char*);

#pragma link C++ function Analysis::showHadronisations(const char*, const char*);
#pragma link C++ function Analysis::getHadronisations(const char*, const char*);
#pragma link C++ function Analysis::showBuildNumber(const char*);

#endif
