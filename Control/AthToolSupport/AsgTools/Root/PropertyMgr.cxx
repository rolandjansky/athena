// PropertyMgr.cxx

#include "AsgTools/PropertyMgr.h"

//**********************************************************************

PropertyMgr::~PropertyMgr() {
  for ( PropMap::iterator ipro=m_props.begin(); ipro!=m_props.end(); ++ipro ) {
    delete ipro->second;
    ipro->second = 0;
  }
}

//**********************************************************************

const PropertyMgr::PropMap& PropertyMgr::getProperties() const {
  return m_props;
}

//**********************************************************************
