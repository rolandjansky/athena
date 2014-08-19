/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFRoiUtils.h"

#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <iostream>

using std::cout;
using std::endl;

/**
 * Take input RoI and create new TrigRoiDescriptor with fixed width.
 * This function is to migrate from old to new RegionSelection interface.
 * Eventually it should be removed and the algorithms should use dynamic widths
 * stored in the RoIs.
 */
const IRoiDescriptor* createRoiDescriptor(const IRoiDescriptor* inmuonRoI, const double etaWidth, const double phiWidth, bool debug) {

  TrigRoiDescriptor* newRoI = 0;
  if (inmuonRoI) {

    const double eta = inmuonRoI->eta();
    const double phi = inmuonRoI->phi();

    double minphi = phi - phiWidth;
    if(minphi < -M_PI) minphi = 2.0*M_PI + minphi;
    double maxphi = phi + phiWidth;
    if(maxphi > M_PI) maxphi = maxphi - 2.0*M_PI;
    newRoI = new TrigRoiDescriptor(eta, eta - etaWidth, eta + etaWidth, phi, minphi, maxphi);

    if(debug) {
      cout << "createRoiDescriptor: input Roi eta, phi = " << eta << ", " << phi << endl;
      cout << "createRoiDescriptor: etaWidth, phiWidth = " << etaWidth << ", " << phiWidth << endl;
      cout << "createRoiDescriptor: etaRange = " << eta - etaWidth << " -> " << eta + etaWidth << endl;
      cout << "createRoiDescriptor: phiRange = " << minphi << " -> " << maxphi << endl;
    } // debug

  } else { // assume we are in full scan mode

    if(debug) {
      cout << "createRoiDescriptor: 0 input, will create 'full scan' RoI" << endl;
    }

    newRoI = new TrigRoiDescriptor(true);
  }
  return newRoI;
}
