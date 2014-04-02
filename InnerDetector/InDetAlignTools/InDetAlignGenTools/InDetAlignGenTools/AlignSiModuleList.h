/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENALGS_ALIGNSIMODULELIST_H
#define INDETALIGNGENALGS_ALIGNSIMODULELIST_H
// AlignSiModuleList.h
// Richard Hawkings, started 15/4/04
// Encapsulation of vector of AlignSiModules and modlist 
// for InDetAlignGenAlgs allowing it to be stored in TDS

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
#include <vector>
#include <map>
#include "InDetAlignTrkInfo/AlignSiModule.h"
#include "Identifier/Identifier.h"

class AlignSiModuleList {
 public:
  std::vector<AlignSiModule> vec;
  typedef std::less<Identifier> lessp;
  typedef std::map<Identifier,int,lessp> ModuleIDMap;
  ModuleIDMap idmap;
};

CLASS_DEF( AlignSiModuleList , 230164606 , 1 )

#endif // INDETALIGNGENALGS_ALIGNSIMODULELIST_H
