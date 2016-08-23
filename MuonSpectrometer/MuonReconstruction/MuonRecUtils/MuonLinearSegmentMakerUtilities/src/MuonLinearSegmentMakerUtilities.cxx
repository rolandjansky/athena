/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLinearSegmentMakerUtilities/MuonLinearSegmentMakerUtilities.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include <sstream>
#include "EventPrimitives/EventPrimitivesToStringConverter.h"

namespace Muon
{



    MuonSegment* MuonLinearSegmentMakerUtilities::makeSegment(MsgStream& log,
                                                              Amg::Vector3D& globalPosition,
                                                              Amg::Vector3D& globalDirection,
                                                              const Trk::RIO_OnTrack* pBaseRio,
                                                              DataVector<const Trk::MeasurementBase>* pRios,
                                                              Trk::FitQuality* pFitQuality)
    {
        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "MuonLinearSegmentMakerUtilities::makeSegment" << endmsg;

        const MuonGM::MuonReadoutElement* pReadoutElement =
            dynamic_cast<const MuonGM::MuonReadoutElement*>(pBaseRio->detectorElement());
        if (pReadoutElement == NULL)
        {
            log << MSG::ERROR << "Cannot get a MuonReadoutElement from pBaseRio" << endmsg;
            return NULL;
        }
        const Trk::Surface& surface = pReadoutElement->surface();
        if (log.level() <= MSG::DEBUG)
        {
            log << MSG::DEBUG << "Surface at pos: " << Amg::toString(surface.center())
            << " dir: " << Amg::toString(surface.normal()) << endmsg;
        }
        const Trk::PlaneSurface* pSurface = dynamic_cast<const Trk::PlaneSurface*>(&surface);
        if (pSurface == NULL)
        {
            log << MSG::ERROR << "Cannot get a PlaneSurface from MuonReadoutElement" << endmsg;
            return NULL;
        }
        Amg::Vector2D pSegPos(0., 0.);
        
        if (!pSurface->globalToLocal(globalPosition, Amg::Vector3D(1., 1., 1.), pSegPos))
        {
            log << MSG::ERROR << "Cannot get a LocalPosition from surface for " << Amg::toString(globalPosition) << endmsg;
            return NULL;
        }
        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "Segment local position is"
            << " x=" << pSegPos[Trk::locX]
            << ",y=" << pSegPos[Trk::locY] << endmsg;
        Trk::LocalDirection  pSegDir;
        pSurface->globalToLocalDirection(globalDirection.unit(), pSegDir);
        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "Segment local direction is"
            << " angleXZ=" << pSegDir.angleXZ()
            << ",angleYZ=" << pSegDir.angleYZ() << endmsg;
	AmgMatrix(4,4) cov; cov.setZero();
        Muon::MuonSegment* pMuonSeg = new Muon::MuonSegment(pSegPos,
                                                            pSegDir,
                                                            cov,
                                                            const_cast<Trk::PlaneSurface*>(pSurface),
                                                            pRios,
                                                            pFitQuality);

        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "MuonLinearSegmentMakerUtilities::makeSegment ended" << endmsg;

        return pMuonSeg;
    }

}
