/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCMODULEID_HH
#define TGCMODULEID_HH
 
#include <map>
#include "TGCcabling/TGCId.h"
 
namespace LVL1TGCCabling8
{
  
class TGCModuleId : public TGCId
{
public:
  enum ModuleIdType {NoModuleIdType=-1,
		     PP,SLB,HPB,SL,SSW,ROD,
		     MaxModuleIdType};
  
  // Constructor & Destructor
  TGCModuleId (ModuleIdType type=NoModuleIdType)
    : TGCId(TGCIdBase::Module)
  {
    this->m_type = type;
  }
  virtual ~TGCModuleId (void) {}

  ModuleIdType getModuleIdType (void) const { return m_type; }

  virtual bool operator ==(const TGCModuleId& moduleId) const;

  virtual bool isValid (void) const { return true; }

private:
  ModuleIdType m_type;
  
};

} // end of namespace
 
#endif
