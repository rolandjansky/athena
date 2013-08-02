/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCModuleSSW.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCModuleSSW::TGCModuleSSW (TGCIdBase::SideType side,
			    int octant,
			    int id)
  : TGCModuleId(TGCModuleId::SSW)
{
  setSideType(side);
  setOctant(octant);
  setId(id);
}
  
bool TGCModuleSSW::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   &&
     (getId()        >=0)                  )
    return true;
  return false;
}

} // end of namespace
