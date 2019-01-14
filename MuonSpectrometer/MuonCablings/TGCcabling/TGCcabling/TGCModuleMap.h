/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULEMAP_HH
#define TGCMODULEMAP_HH

#include <map>

#include "TGCcabling/TGCModuleId.h"

namespace LVL1TGCCabling8
{
 
class TGCModuleMap
{
public:
  // Constructor & Destructor
  TGCModuleMap (void)
  {}

  virtual ~TGCModuleMap (void);
  
  int connector (int entry);

  TGCModuleId* moduleId (int entry);

  TGCModuleId* popModuleId (int entry);

  void insert (int connector, TGCModuleId* moduleId);

  int find (int connector);
  
  int size (void);

  void clear (void);

private:
  std::map<int,TGCModuleId*> m_moduleMap;
};
  
} // end of namespace
 
#endif
