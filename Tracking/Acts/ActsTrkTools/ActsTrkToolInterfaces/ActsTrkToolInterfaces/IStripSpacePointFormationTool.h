/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKTOOLINTERFACES_ISTRIPSPACEPOINTFORMATIONTOOL_H
#define ACTSTRKTOOLINTERFACES_ISTRIPSPACEPOINTFORMATIONTOOL_H 1

// Athena
#include "GaudiKernel/IAlgTool.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkEvent/SpacePointData.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"

namespace ActsTrk {
    /// @class IPixelSpacePointFormationTool
    /// Base class for strip space point formation tool

    class IStripSpacePointFormationTool : virtual public IAlgTool {
    public:
        DeclareInterfaceID(IStripSpacePointFormationTool, 1, 0);

        virtual void produceStripSpacePoints(const xAOD::StripClusterContainer& clusterContainer,
                                             const InDet::SiElementPropertiesTable& properties,
                                             const InDetDD::SiDetectorElementCollection& elements,
                                             const Amg::Vector3D& beamSpotVertex,
                                             ActsTrk::SpacePointContainer& spacePoints,
                                             ActsTrk::SpacePointData& spacePointData,
                                             ActsTrk::SpacePointMeasurementDetails& spacePointDetails,
                                             ActsTrk::SpacePointContainer& overlapSpacePoints,
                                             ActsTrk::SpacePointData& overlapSpacePointData,
                                             ActsTrk::SpacePointMeasurementDetails& overlapSpacePointDetails,
                                             bool processOverlaps) const = 0;
    };

} // ACTSTRKTOOLINTERFACES_ISTRIPSPACEPOINTFORMATIONTOOL_H

#endif
