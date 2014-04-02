/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_PERSISTENCY_PFUNCTIONS_H
#define DQM_PERSISTENCY_PFUNCTIONS_H

#include <TObject.h>

class TDirectory;

namespace dqm_persistency {

  class PParameter;
  class PRegion;
  class PMetadata;

  void Print(const PParameter* param, TDirectory* topdir, 
	     Option_t* opt="");

  void WriteTree(PRegion& top_level, TDirectory& topdir, PMetadata& topmap);

  PParameter* GetNode(TDirectory& topdir, const std::string& nodename);

  PParameter* GetNode(TDirectory& topdir, int key);

}

#endif //DQM_PERSISTENCY_PFUNCTIONS_H
