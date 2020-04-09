/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_READOUTGEOMETRY_HGTD_DETECTORMANAGER_H
#define HGTD_READOUTGEOMETRY_HGTD_DETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

// class StoreGateSvc;
// class Identifier;
// class IdentifierHash;
// class GeoAlignableTransform;
// class GeoVFullPhysVol;
// class GeoVPhysVol;

class HGTD_DetectorManager : public GeoVDetectorManager {
public:
    HGTD_DetectorManager();
    virtual ~HGTD_DetectorManager();

    virtual unsigned int getNumTreeTops()           const;
    virtual PVConstLink  getTreeTop(unsigned int i) const;
    virtual void addTreeTop (PVLink treeTop);

private:
    /** Prevent copy and assignment */
    const HGTD_DetectorManager & operator=(const HGTD_DetectorManager &right);
    HGTD_DetectorManager(const HGTD_DetectorManager &right);

    // Private member data
    std::vector<PVLink> m_volume;

};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(HGTD_DetectorManager, 73488296, 1)
#endif

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORMANAGER_H
