/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPREWEIGHTINGDICT_H
#define PILEUPREWEIGHTINGDICT_H

#include "PileupReweighting/PileupReweightingTool.h"
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

#ifdef XAOD_ANALYSIS
#include "AsgTools/AnaToolHandle.h"
namespace {
   struct GCCXML_DUMMY_INSTATIATION {
      asg::AnaToolHandle<CP::IPileupReweightingTool> t;
   };
}
#endif


#endif 
