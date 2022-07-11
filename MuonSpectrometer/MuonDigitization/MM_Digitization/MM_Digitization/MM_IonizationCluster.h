/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_IONIZATIONCLUSTER_H
#define MM_DIGITIZATION_MM_IONIZATIONCLUSTER_H
//
// MM_IonizationCluster.cxx
//     Primary Ionization Cluster made up of primary electrons
//

#include <memory>
#include <vector>

#include "MM_Digitization/MM_Electron.h"
#include "TVector2.h"

class MM_IonizationCluster {
public:
    MM_IonizationCluster() = default;
    MM_IonizationCluster(float HitX, float IonizationX, float IonizationY);
    void createElectrons(int nElectrons);
    void propagateElectrons(float lorentzAngle, float driftVel);
    std::vector<std::unique_ptr<MM_Electron>>& getElectrons();
    float getHitX() const { return m_HitX; }
    TVector2 getIonizationStart() const { return m_IonizationStart; }

private:
    // Members supplied by user
    std::vector<std::unique_ptr<MM_Electron>> m_Electrons;
    float m_HitX{0.0F};
    TVector2 m_IonizationStart{0., 0.};
};

#endif
