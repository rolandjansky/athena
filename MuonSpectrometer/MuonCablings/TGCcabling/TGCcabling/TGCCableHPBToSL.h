/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEHPBTOSL_HH
#define TGCCABLEHPBTOSL_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabasePPToSL.h" 
#include "TGCcabling/TGCModuleHPB.h"
#include "TGCcabling/TGCModuleSL.h"

namespace LVL1TGCCabling8
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
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxSignalType];
};
  
} // end of namespace
 
#endif
