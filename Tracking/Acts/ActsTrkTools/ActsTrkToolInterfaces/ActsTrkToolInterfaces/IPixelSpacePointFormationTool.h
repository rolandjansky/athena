/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKTOOLINTERFACES_IPIXELSPACEPOINTFORMATIONTOOL_H
#define ACTSTRKTOOLINTERFACES_IPIXELSPACEPOINTFORMATIONTOOL_H 1

// Athena
#include "GaudiKernel/IAlgTool.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkEvent/SpacePointData.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "xAODInDetMeasurement/PixelCluster.h"

namespace ActsTrk {

    /// @class IPixelSpacePointFormationTool
    /// Base class for pixel space point formation tool

    class IPixelSpacePointFormationTool : virtual public IAlgTool {
    public:
        DeclareInterfaceID(IPixelSpacePointFormationTool, 1, 0);

        /// @name Production of space points
        //@{
        virtual std::unique_ptr<ActsTrk::SpacePoint> producePixelSpacePoint(const xAOD::PixelCluster& cluster,
                                                                            ActsTrk::SpacePointData& data,
                                                                            const boost::container::static_vector<std::size_t, 2>& measIndexes,
                                                                            const InDetDD::SiDetectorElement& element) const = 0;
        //@}

    };

} // ACTSTRKTOOLINTERFACES_IPIXELSPACEPOINTFORMATIONTOOL_H

#endif


