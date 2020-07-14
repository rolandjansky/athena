/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTBARRELLUT_H
#define TRIGL2MUONSA_PTBARRELLUT_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/Service.h"

#include <string>

namespace TrigL2MuonSA {
  
class PtBarrelLUT: public AthAlgTool 
{
 public:    
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

  public:

    PtBarrelLUT(const std::string& type, 
		const std::string& name,
                const IInterface*  parent);

    const LUT&   lut(void) const  { return m_lut; };
    const LUTsp& lutSP(void) const { return m_lutSP; };
    
    StatusCode readLUT(std::string lut_fileName,
		       std::string lutSP_fileName);
    
  private:
    LUT    m_lut;
    LUTsp  m_lutSP;

};

}

#endif // PHYSICS_MU_PTBARRELLUT_H
