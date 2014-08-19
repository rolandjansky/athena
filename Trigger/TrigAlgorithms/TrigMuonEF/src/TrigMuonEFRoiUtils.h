/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEFROIUTILS_H__
#define TRIGMUONEFROIUTILS_H__

// fwd declare IRoiDescriptor class
class IRoiDescriptor;

/// Utility function that takes input ROI descriptor and makes a new one with the specified range.
const IRoiDescriptor* createRoiDescriptor(const IRoiDescriptor* inmuonRoI, const double etaWidth, const double phiWidth, bool debug=false);

#endif //TRIGMUONEFROIUTILS_H__
