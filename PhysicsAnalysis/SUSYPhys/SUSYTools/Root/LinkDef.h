/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUSYTOOLS_LINKDEF_H
#define SUSYTOOLS_LINKDEF_H

#include <vector>
#include <map>
#include <utility>
#include "SUSYTools/SUSYCrossSection.h"
#include "SUSYTools/SUSYObjDef_xAOD.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ namespace SUSY;
#pragma link C++ class ST::SUSYObjDef_xAOD+;

#pragma link C++ class SUSY::CrossSectionDB+;
#pragma link C++ class SUSY::CrossSectionDB::Process+;

#pragma link C++ class vector<vector<unsigned int> >+;
#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<double> >+;
#pragma link C++ class pair<float,vector<float> >+;
#pragma link C++ class pair<vector<float>,vector<float> >+;
#pragma link C++ class pair<int,SUSY::CrossSectionDB::Process>+;
#pragma link C++ class map<int,SUSY::CrossSectionDB::Process>+;
#pragma link C++ class SUSY::CrossSectionDB::iterator-;

#endif

#endif
