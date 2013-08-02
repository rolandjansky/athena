/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLESSWTOROD_HH
#define TGCCABLESSWTOROD_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCDatabaseSLBToROD.h" 
#include "TGCcabling12/TGCModuleSSW.h"
#include "TGCcabling12/TGCModuleROD.h"

namespace LVL1TGCCabling12
{
  
class TGCCableSSWToROD : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableSSWToROD (std::string filename);

  virtual ~TGCCableSSWToROD (void);
  
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;

private:
  TGCCableSSWToROD (void) {}
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* rod) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* ssw) const;
  TGCDatabase* database;
};
  
} // end of namespace
 
#endif
