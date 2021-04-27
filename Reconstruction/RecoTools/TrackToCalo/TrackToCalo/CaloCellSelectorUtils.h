/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLSELECTORUTILS_H
#define CALOCELLSELECTORUTILS_H

#include "TrkCaloExtension/CaloExtension.h"

namespace Utils {
    double deltaPhi(double phi1, double phi2);

    double deltaR2(double eta1, double eta2, double phi1, double phi2);

    double deltaR(double eta1, double eta2, double phi1, double phi2);

    void findNearestPoint(const Amg::Vector3D& inputPos, const Trk::CaloExtension* caloExtension, int& nearestIdx,
                          Amg::Vector3D& nearestPos, Amg::Vector3D& nearestMom);
}  // namespace Utils

#endif
