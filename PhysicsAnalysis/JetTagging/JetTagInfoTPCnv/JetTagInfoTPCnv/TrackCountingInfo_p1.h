/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_TRACKCOUNTINGINFO_P1_H
#define JETTAGINFOTPCNV_TRACKCOUNTINGINFO_P1_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Analysis { 

  class TrackCountingInfoCnv_p1;
  
  class TrackCountingInfo_p1 { 
    
    friend class TrackCountingInfoCnv_p1;
      
  private:
    /// Basic info
    TPObjRef m_baseTagInfo;

    /// All of this data will be written out.
    int m_ntrk;           
    float m_d0sig_2nd;    
    float m_d0sig_abs_2nd;
    float m_d0sig_3rd;    
    float m_d0sig_abs_3rd;
  
  }; // End class
  
} // End namespace
#endif
