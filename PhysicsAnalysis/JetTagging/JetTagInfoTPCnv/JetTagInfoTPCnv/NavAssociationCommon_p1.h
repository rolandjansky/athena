/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_NavAssocationCommon_p1_H
#define JETTAGINFOTPCNV_NavAssocationCommon_p1_H

///
/// Store the required info for b-tagging assocaiations that are
/// a Navigable dude.
///

#include "AthenaPoolUtilities/TPObjRef.h"
#include <string>

#include <string>

namespace Analysis {

  class NavAssociationCommon_p1 {
  public:
    /// Store the nav base classes
    TPObjRef m_navBase;

    unsigned int m_keyIndex;
    std::string m_name;
  };
}

#endif
