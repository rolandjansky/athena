/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSPACEPOINTFORMATIONTOOL_PIXELSPACEPOINTFORMATIONTOOL_H
#define ACTSTRKSPACEPOINTFORMATIONTOOL_PIXELSPACEPOINTFORMATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ActsTrkToolInterfaces/IPixelSpacePointFormationTool.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkEvent/SpacePointData.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "xAODInDetMeasurement/PixelClusterAuxContainer.h"

#include <string>

class PixelID;

namespace ActsTrk {
  /// @class PixelSpacePointFormationTool
  /// Tool to produce pixel space points.
  /// Pixel space points are obtained directly from the clusters,
  /// with needed evaluation of the space point covariance terms
  /// Space points are then recorded to storegate as ActsTrk::SpacePoint
  /// into an ActsTrk::SpacePointContainer in the PixelSpacePointFormationAlgorithm

  class PixelSpacePointFormationTool: public extends<AthAlgTool, ActsTrk::IPixelSpacePointFormationTool> {
  public:
    /// @name AthAlgTool methods
    //@{
    PixelSpacePointFormationTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);
    virtual ~PixelSpacePointFormationTool() = default;
    virtual StatusCode initialize() override;
    //@}

    /// @name Production of space points
    //@{
    virtual std::unique_ptr<ActsTrk::SpacePoint> producePixelSpacePoint(const xAOD::PixelCluster& cluster,
                                                                        ActsTrk::SpacePointData& data,
                                                                        const boost::container::static_vector<std::size_t, 2>& measIndexes,
                                                                        const InDetDD::SiDetectorElement& element) const override;
    //@}

  private:
    /// @name Id helpers
    //@{
    const PixelID* m_pixelId{};
    //@}

    /// @name Static constant expression
    /// @brief Values used in calculating covariance terms
    static constexpr double s_oneOverTwelve{0.08333};

  };

}

#endif // ACTSTRKSPACEPOINTFORMATION_PIXELSPACEPOINTFORMATIONALG_H
