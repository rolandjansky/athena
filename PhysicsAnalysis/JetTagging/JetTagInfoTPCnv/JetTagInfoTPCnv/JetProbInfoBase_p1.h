/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_JETPROBINFOBASE_P1_H
#define JETTAGINFOTPCNV_JETPROBINFOBASE_P1_H

///
/// Persitent class for the JetProbl class.
///

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class JetProbInfoBaseCnv_p1;

  class JetProbInfoBase_p1 {
    friend class JetProbInfoBaseCnv_p1;

  private:
    /// Basic info
    TPObjRef m_baseTagInfo;

    /// All of this data will be written out.
    int m_ntrk;
  };
}

#endif
