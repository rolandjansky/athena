/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCModuleROD.h"

namespace LVL1TGCCabling8
{
 
// Constructor
TGCModuleROD::TGCModuleROD (TGCIdBase::SideType side,
			    int octant)
  : TGCModuleId(TGCModuleId::ROD)
{
  setSideType(side);
  setOctant(octant);
}
  
bool TGCModuleROD::isValid (void) const
{
  if((getSideType()  >TGCIdBase::NoSideType)   &&
     (getSideType()  <TGCIdBase::MaxSideType)  &&
     (getOctant()    >=0)                  &&
     (getOctant()    <8)                   )
    return true;
  return false;
}

} // end of namespace
