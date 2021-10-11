/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_DetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENTCOLLECTION_H
#define HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENTCOLLECTION_H

#include <vector>

namespace InDetDD {

    class HGTD_DetectorElement;

    /** @class HGTD_DetectorElementCollection

       Class to hold the HGTD_DetectorElement objects to be put in the detector store

    */

    class HGTD_DetectorElementCollection : public std::vector<HGTD_DetectorElement *>
{};

} // namespace InDetDD

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENTCOLLECTION_H
