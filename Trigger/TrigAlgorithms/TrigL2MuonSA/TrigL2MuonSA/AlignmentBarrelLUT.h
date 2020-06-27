/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_ALIGNMENTBARRELLUT_H
#define TRIGL2MUONSA_ALIGNMENTBARRELLUT_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TMath.h"

#include "GaudiKernel/Service.h"

#include <string>

namespace TrigL2MuonSA {
  
class AlignmentBarrelLUT: public AthAlgTool
{

  public:

    AlignmentBarrelLUT(const std::string& type, 
                       const std::string& name,
                       const IInterface*  parent);

    StatusCode readLUT(std::string lut_fileName);

    double GetDeltaZ(int& saddress, double& etaMap, double& phiMap, double& MFphi, float& sp1R ) const;

    std::pair<int, int> GetBinNumber(int saddress, int innerR, double etaMap, double phiMap) const;

  private:

    static const int s_saddress = 4;
    static const int s_innerR = 2;
    static const int s_eta = 15;
    static const int s_phi = 30;
    static const int s_etaQ = 2;

    double m_dZ[s_saddress][s_innerR][s_eta][s_phi][s_etaQ];

    int m_NbinEta[s_saddress][s_innerR];
    float m_EtaMin[s_saddress][s_innerR];
    float m_EtaMax[s_saddress][s_innerR];
    float m_EtaStep[s_saddress][s_innerR];
    int m_NbinPhi[s_saddress][s_innerR];
    float m_PhiMin[s_saddress][s_innerR];
    float m_PhiMax[s_saddress][s_innerR];
    float m_PhiStep[s_saddress][s_innerR];

  };

}

#endif
