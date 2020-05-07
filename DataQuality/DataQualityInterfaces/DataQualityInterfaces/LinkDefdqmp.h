/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/PAlgorithm.h"
#include "DataQualityInterfaces/PMetadata.h"
#include "DataQualityInterfaces/PParameter.h"
#include "DataQualityInterfaces/PRegion.h"
#include "DataQualityInterfaces/PResult.h"
#include "DataQualityInterfaces/PFunctions.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dqm_persistency ;

#pragma link C++ class std::map<std::string, std::string>;
#pragma link C++ class std::map<std::string, std::vector<float> > ;

#pragma link C++ class std::vector<std::pair<std::string,float> > ;
#pragma link C++ class std::vector<dqm_persistency::PParameter*> ;
#pragma link C++ class dqm_persistency::PAlgorithm+ ;
#pragma link C++ class dqm_persistency::PMetadata+ ;
#pragma link C++ class dqm_persistency::PParameter+ ;
#pragma link C++ class dqm_persistency::PRegion+ ;
#pragma link C++ class dqm_persistency::PResult+ ;

#pragma link C++ function dqm_persistency::Print ;
#pragma link C++ function dqm_persistency::WriteTree ;
#pragma link C++ function dqm_persistency::GetNode ;

//#pragma link C++ function dqm_persistency::thunk ;

#endif
