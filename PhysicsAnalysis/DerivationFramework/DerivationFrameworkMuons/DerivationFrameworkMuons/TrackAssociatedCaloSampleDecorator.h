/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DERIVATIONFRAMEWORK_TRACKASSOCIATEDCALOSAMPLEDECORATOR_H
#define DERIVATIONFRAMEWORK_TRACKASSOCIATEDCALOSAMPLEDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class TrackAssociatedCaloSampleDecorator : public AthAlgTool, public IAugmentationTool {
  public:
    TrackAssociatedCaloSampleDecorator(const std::string& t, const std::string& n, const IInterface* p);
    StatusCode initialize();
    virtual StatusCode addBranches() const;

  private:
    std::string m_containerName;
    double m_upperPtLimit;
    double m_lowerPtLimit;

    /** variables for track-calo position matching */
    double m_dEtaCut;
    double m_cosdPhiCut;

    /**  Thresholds for dSin(dPhi).
         dPhi is phi diffarence between calo cluster and track at IP.
         dSin(dPhi) is sin(dPhi) diffarence between sin(dphi) calculated from actual dPhi and estimated by a function with pT 
         estimated sin(dPhi) = -0.5/pT (positive charge): 0.5/pT (negative charge) 
         dSin(dPhi) distribution is not symmetric. So, two thresholds are set.
    */
    double m_sindPhiLongTailCut;
    double m_sindPhiShortTailCut;



  };
}



#endif //DERIVATIONFRAMEWORK_TRACKASSOCIATEDCALOSAMPLEDECORATOR_H

