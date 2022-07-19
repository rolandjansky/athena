/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DERIVATIONFRAMEWORK_TRACKCALOCLUSTERDECORATOR_LOWPTE_H
#define DERIVATIONFRAMEWORK_TRACKCALOCLUSTERDECORATOR_LOWPTE_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class TrackCaloClusterDecorator_LowPtE : public AthAlgTool, public IAugmentationTool {
  public:
    TrackCaloClusterDecorator_LowPtE(const std::string& t, const std::string& n, const IInterface* p);
    StatusCode initialize();
    virtual StatusCode addBranches() const;

  private:
    std::string m_containerName;
    double m_upperPtLimit;
    double m_lowerPtLimit;

    // variables for track-calo position matching
    double m_dEtaCut;
    double m_cosdPhiCut;

    // Thresholds for sin(dPhi)_ext.
    // dPhi is phi difference between calo cluster and track at IP.
    // sin(dPhi)_ext corrects sin(dPhi) by taking into effects of magnetic curvature for barrel and endcap separately
    // barrel(|eta|<1.5)  : sin(dPhi)_ext = sin(dPhi) + 0.45*trackCharge/trackPt[GeV]
    // endcap (|eta|=>1.5): sin(dPhi)_ext = sin(dPhi) + 2/(exp(1.13*|eta|)-exp(-1.13*|eta|)) * trackCharge/trackPt[GeV]
    double m_sindPhiCut;

    std::string m_prefix;
  };
}



#endif //DERIVATIONFRAMEWORK_TRACKCALOCLUSTERDECORATOR_LOWPTE_H
