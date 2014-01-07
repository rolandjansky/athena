/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLINEARSEGMENTMAKERUTILITIES_UTILITIES_H
#define MUONLINEARSEGMENTMAKERUTILITIES_UTILITIES_H

#include "DataModel/DataVector.h"
#include <string>
#include "GeoPrimitives/GeoPrimitives.h"
class MsgStream;

namespace Trk
{
    class RIO_OnTrack;
    class FitQuality;
    class MeasurementBase;
}

namespace Muon
{

    class MuonSegment;

    class MuonLinearSegmentMakerUtilities
    {
    public:

        static std::string HepPoint3DToString(const Amg::Vector3D& pt);
        static std::string HepVector3DToString(const Amg::Vector3D& dir);
        static MuonSegment* makeSegment(MsgStream& log,
                                        Amg::Vector3D& globalPosition,
                                        Amg::Vector3D& globalDirection,
                                        const Trk::RIO_OnTrack* pBaseRio,
                                        DataVector<const Trk::MeasurementBase>* pRios,
                                        Trk::FitQuality* pFitQuality);
    };

}

#endif //MUONLINEARSEGMENTMAKERUTILITIES_UTILITIES_H
