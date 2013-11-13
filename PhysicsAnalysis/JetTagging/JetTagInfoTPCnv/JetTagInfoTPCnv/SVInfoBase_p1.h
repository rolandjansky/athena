/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SVInfoBase_P1_H
#define JETTAGINFOTPCNV_SVInfoBase_P1_H

///
/// Persitent class for the truth info
///
#include "EventPrimitives/EventPrimitives.h"
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class SVInfoBaseCnv_p1;

  class SVInfoBase_p1 {
    friend class SVInfoBaseCnv_p1;

  private:
    /// Basic info
    TPObjRef m_baseTagInfo;

    /// All of this data will be written out.
  };
}

#endif
