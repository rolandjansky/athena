/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCModuleId.h"

namespace MuonTGC_Cabling {

bool TGCModuleId::operator ==(const TGCModuleId& moduleId) const
{
  if((this->getModuleIdType()==moduleId.getModuleIdType())&&
     (this->getSideType()    ==moduleId.getSideType())    &&
     (this->getRegionType()  ==moduleId.getRegionType())  &&
     (this->getSignalType()  ==moduleId.getSignalType())  &&
     (this->getModuleType()  ==moduleId.getModuleType())  &&
     (this->getSector()      ==moduleId.getSector())      &&
     (this->getOctant()      ==moduleId.getOctant())      &&
     (this->getId()          ==moduleId.getId())          )
    return true;
  return false;
}

} //end of namespace
