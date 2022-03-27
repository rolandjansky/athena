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

class IdentifierHash;

namespace InDetDD {

    class HGTD_DetectorElement;

    /** @class HGTD_DetectorElementCollection

       Class to hold the HGTD_DetectorElement objects to be put in the detector store

    */

    class HGTD_DetectorElementCollection : public std::vector<HGTD_DetectorElement *> {
        public:
            const HGTD_DetectorElement* getDetectorElement(const IdentifierHash& hash) const;
    };

} // namespace InDetDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDetDD::HGTD_DetectorElementCollection, 1266958207, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_MIXED_DEF( InDetDD::HGTD_DetectorElementCollection, 1258619755);

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORELEMENTCOLLECTION_H
