/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEREADOUTGEOMETRY_BCMPRIMEDETECTORMANAGER_H
#define BCMPRIMEREADOUTGEOMETRY_BCMPRIMEDETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

#include "GeoModelKernel/GeoAlignableTransform.h"

// Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"

#include "AthenaKernel/IIOVSvc.h"

class StoreGateSvc;

/** @class BCMPrimeDetectorManager

    The Detector manager registers the call backs and infrastructure to
    associate the alignment transforms with the appropriate alignable
    transform in GeoModel.

    @author Jakob Novak <jakob.novak@cern.ch>

    */

namespace InDetDD {

    class BCMPrimeDetectorManager : public GeoVDetectorManager {
    public:

        /** Constructor */
        BCMPrimeDetectorManager(StoreGateSvc* detStore, const std::string & name);

        /** Destructor */
        ~BCMPrimeDetectorManager();

        /** Access to raw geometry: */
        virtual unsigned int getNumTreeTops()           const;
        virtual PVConstLink  getTreeTop(unsigned int i) const;

        /** Add a Tree top: */
        virtual void addTreeTop (PVLink treeTop);

        void addAlignableTransform (int /*id*/, GeoAlignableTransform * /*transform*/, const GeoVPhysVol * /*child*/);
        StatusCode align( IOVSVC_CALLBACK_ARGS ) const;

        /** Declaring the Message method for further use */
        MsgStream& msg (MSG::Level lvl) const { return m_msg.get() << lvl; }

        /** Declaring the Method providing Verbosity Level */
        bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

    private:

        /** Prevent copy and assignment */
        const BCMPrimeDetectorManager & operator=(const BCMPrimeDetectorManager &right);
        BCMPrimeDetectorManager(const BCMPrimeDetectorManager &right);

        /** Private member data */
        std::vector<PVLink>              m_volume;

        /** Detector store */
        StoreGateSvc * m_detStore;

        /** Declaring private message stream member */
        mutable Athena::MsgStreamMember  m_msg;

    };

} // namespace InDetDD

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(InDetDD::BCMPrimeDetectorManager, 162824294, 1)
#endif

#endif // BCMPRIMEREADOUTGEOMETRY_BCMPRIMEDETECTORMANAGER_H
