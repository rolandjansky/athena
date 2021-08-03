/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/BaseObject.h"

BaseObject::BaseObject(ObjectType tag, const std::string& obj_name) :
  m_tag{tag},
  m_name{obj_name} {
}
