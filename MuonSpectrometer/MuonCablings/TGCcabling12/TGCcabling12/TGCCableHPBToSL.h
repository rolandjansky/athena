/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEHPBTOSL_HH
#define TGCCABLEHPBTOSL_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCDatabasePPToSL.h" 
#include "TGCcabling12/TGCModuleHPB.h"
#include "TGCcabling12/TGCModuleSL.h"

namespace LVL1TGCCabling12
{
  
class TGCCableHPBToSL : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableHPBToSL (std::string filename);

  virtual ~TGCCableHPBToSL (void);
  
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;

private:
  TGCCableHPBToSL (void) {}
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* sl) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* hpt) const;
  TGCDatabase* database[TGCIdBase::MaxRegionType][TGCIdBase::MaxSignalType];
};
  
} // end of namespace
 
#endif
