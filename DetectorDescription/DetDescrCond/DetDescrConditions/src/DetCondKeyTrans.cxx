/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DetCondKeyTrans.cxx - set of HepGeom::Transform3Ds keyed by string
// Richard Hawkings, started 23/6/05

#include <iostream>
#include "DetDescrConditions/DetCondKeyTrans.h"

DetCondKeyTrans::DetCondKeyTrans() :
  m_keytrans() {}

bool DetCondKeyTrans::getTransform(const std::string& key,
              HepGeom::Transform3D& result) const {
  KeyTrans::const_iterator ik=m_keytrans.lower_bound(key);
  if (ik!=m_keytrans.end()) {
    result=(ik->second);
    return true;
  } else {
    return false;
  }
}

bool DetCondKeyTrans::exists(const std::string& key) const {
  KeyTrans::const_iterator ik=m_keytrans.lower_bound(key);
  return (ik!=m_keytrans.end());
}

void DetCondKeyTrans::print() const {
  for (KeyTrans::const_iterator ik=m_keytrans.begin(); ik!=m_keytrans.end();
       ++ik) {
    CLHEP::Hep3Vector trans=ik->second.getTranslation();
    CLHEP::HepRotation rot=ik->second.getRotation();
    std::cout << "Dump DetCondKeyTrans object:" << std::endl;
    std::cout << "Key " << ik->first << " transl [" << trans.x()
	      << "," << trans.y() << "," << trans.z() << "] rotation ("
	      << rot.phi() << "," << rot.theta() << "," <<  rot.psi() << ")"
	      << std::endl;
  }
}
