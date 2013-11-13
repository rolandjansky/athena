/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_IPINFOBASE_P1_H
#define JETTAGINFOTPCNV_IPINFOBASE_P1_H
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Analysis { 

class IPInfoBaseCnv_p1;

class IPInfoBase_p1 { 

friend class IPInfoBaseCnv_p1;

public:
 inline IPInfoBase_p1()
   : m_ntrk(0)
   {}
    
private:
    TPObjRef m_baseTagInfo;
    int m_ntrk; 

}
; // End class

} // End namespace
#endif
