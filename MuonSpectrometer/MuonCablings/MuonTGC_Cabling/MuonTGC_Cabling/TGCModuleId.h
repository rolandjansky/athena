/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCMODULEID_HH
#define MUONTGC_CABLING_TGCMODULEID_HH
 
#include "MuonTGC_Cabling/TGCId.h"
 
namespace MuonTGC_Cabling
{
  
class TGCModuleId : public TGCId
{
public:
  enum ModuleIdType {NoModuleIdType=-1, 
		     PP, SLB, HPB, SL, SSW, ROD, 
		     MaxModuleIdType};
  
  // Constructor & Destructor
  TGCModuleId(ModuleIdType type=NoModuleIdType)
    : TGCId(TGCIdBase::Module)
    {
      this->type = type;
    }
  virtual ~TGCModuleId(void) {}
  
  ModuleIdType getModuleIdType(void) const { return type; }

  virtual bool operator ==(const TGCModuleId& moduleId) const;

  virtual bool isValid(void) const { return true; }

private:
  ModuleIdType type;
  
};

} // end of namespace
 
#endif
