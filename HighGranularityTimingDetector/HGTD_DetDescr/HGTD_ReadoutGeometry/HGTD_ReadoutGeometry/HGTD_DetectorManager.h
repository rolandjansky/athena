/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_READOUTGEOMETRY_HGTD_DETECTORMANAGER_H
#define HGTD_READOUTGEOMETRY_HGTD_DETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"

#include "ReadoutGeometryBase/SiCommonItems.h"

#include "HGTD_Identifier/HGTD_ID.h"

// Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"

using namespace InDetDD;

class StoreGateSvc;

/** @class HGTD_DetectorManager

    The Detector manager has methods to retrieve the Identifier
    helper and methods to retrieve the detector elements, as well
    as the relevant physical volumes.

    */

class HGTD_DetectorManager : public GeoVDetectorManager {
public:

    /** Constructor */
    HGTD_DetectorManager(StoreGateSvc* detStore);

    /** Destructor */
    ~HGTD_DetectorManager();

    /** Access to raw geometry: */
    virtual unsigned int getNumTreeTops()           const override;
    virtual PVConstLink  getTreeTop(unsigned int i) const override;

    /** Add a Tree top: */
    virtual void addTreeTop (PVLink treeTop);

    //
    // Access Readout Elements
    //

    /** access to individual elements : via Identifier */
    virtual HGTD_DetectorElement * getDetectorElement(const Identifier &id) const;

    /** access to individual elements : via IdentifierHash */
    virtual HGTD_DetectorElement * getDetectorElement(const IdentifierHash &idHash) const;

    /** access to individual elements : via element identification */
    HGTD_DetectorElement * getDetectorElement(int endcap,
                                              int layer,
                                              int phi_module,
                                              int eta_module) const;

    /** access to whole collection via Iterators */
    virtual const HGTD_DetectorElementCollection * getDetectorElementCollection() const;
    virtual HGTD_DetectorElementCollection::const_iterator getDetectorElementBegin() const;
    virtual HGTD_DetectorElementCollection::const_iterator getDetectorElementEnd() const;

    /** Add elememts */
    virtual void addDetectorElement(HGTD_DetectorElement * element);

    /** Invalidate cache for all detector elements */
    virtual void invalidateAll() const;

    /** Update all caches */
    virtual void updateAll() const;

    /** Set SiCommonItems */
    void setCommonItems(std::unique_ptr<const SiCommonItems>&& commonItems);

    /** Declaring the Message method for further use */
    MsgStream& msg (MSG::Level lvl) const { return m_msg.get() << lvl; }

    /** Declaring the Method providing Verbosity Level */
    bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

private:

    /** Prevent copy and assignment */
    const HGTD_DetectorManager & operator=(const HGTD_DetectorManager &right);
    HGTD_DetectorManager(const HGTD_DetectorManager &right);

    virtual const HGTD_ID  * getIdHelper() const;

    // Private member data
    std::vector<PVLink>              m_volume;
    HGTD_DetectorElementCollection   m_elementCollection;
    const HGTD_ID*                   m_idHelper;

    std::unique_ptr<const SiCommonItems> m_commonItems;

    //Declaring private message stream member.
    mutable Athena::MsgStreamMember  m_msg;

};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(HGTD_DetectorManager, 73488296, 1)
#endif

#endif // HGTD_READOUTGEOMETRY_HGTD_DETECTORMANAGER_H
