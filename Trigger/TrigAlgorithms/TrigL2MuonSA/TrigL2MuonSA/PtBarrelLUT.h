/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTBARRELLUT_H
#define TRIGL2MUONSA_PTBARRELLUT_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

#include <string>

namespace TrigL2MuonSA {
  
  struct LUT {
    int NbinEta[4];
    int NbinPhi[4];
    float EtaMin[4];
    float EtaMax[4];
    float EtaStep[4];
    float PhiMin[4];
    float PhiMax[4];
    float PhiStep[4];
    float table[4][2][60][60][2];
  };  
  
  struct LUTsp {
    // [innerR][charge][eta][phi][parms]
    float table_LargeSP[2][2][30][30][2];
  };

  class PtBarrelLUT {
    
  public:
    PtBarrelLUT(MsgStream* msg);
    ~PtBarrelLUT();

    const LUT&   lut(void) const;
    const LUTsp& lutSP(void) const;
    
    StatusCode readLUT(std::string lut_fileName,
		       std::string lutSP_fileName);
    
    MsgStream*  m_msg;
    inline MsgStream& msg() const { return *m_msg; }
    
  private:
    LUT    m_lut;
    LUTsp  m_lutSP;
  };

  inline const LUT& PtBarrelLUT::lut(void) const
  {
    return m_lut;
  }

  inline const LUTsp& PtBarrelLUT::lutSP(void) const
  {
    return m_lutSP;
  }
  
}
#endif // PHYSICS_MU_PTBARRELLUT_H
