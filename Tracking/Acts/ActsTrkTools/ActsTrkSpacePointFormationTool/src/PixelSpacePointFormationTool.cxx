/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSpacePointFormationTool.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

namespace ActsTrk {

    PixelSpacePointFormationTool::PixelSpacePointFormationTool(const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent)
    : base_class(type, name, parent)
    {}

    StatusCode PixelSpacePointFormationTool::initialize() {

        ATH_CHECK(detStore()->retrieve(m_pixelId,"PixelID"));

        return StatusCode::SUCCESS;
    }

    std::unique_ptr<ActsTrk::SpacePoint>
    PixelSpacePointFormationTool::producePixelSpacePoint(const xAOD::PixelCluster& cluster,
                                                         ActsTrk::SpacePointData& data,
                                                         const boost::container::static_vector<std::size_t, 2>& measIndexes,
                                                         const InDetDD::SiDetectorElement& element) const
    {
        // evaluate the cluster width for space point covariance evaluation
        const InDetDD::PixelModuleDesign* design
          (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element.design()));
        if (not design){
            ATH_MSG_ERROR("Cast to InDetDD::PixelModuleDesign failed.");
            return nullptr;
        }

        auto rdoList = cluster.rdoList();
        auto min_max =
          std::minmax_element(rdoList.begin(), rdoList.end(),
                              [this] (Identifier const& lh, Identifier const& rh) {
                                  return m_pixelId->eta_index(lh) < m_pixelId->eta_index(rh);
                              });

        // this is the width expressed in mm
        float width =
        design->widthFromColumnRange(m_pixelId->eta_index(*min_max.first),
                                     m_pixelId->eta_index(*min_max.second));

        // using width to scale the cluster covariance for space points
        float covTerm = width*width*s_oneOverTwelve;
        auto localCov = cluster.localCovariance<2>();
        if( covTerm < localCov(1, 1) )
            covTerm = localCov(1, 1);

        Eigen::Matrix<double, 2, 1> variance(0.06, 9.*covTerm);
        // Swap r/z covariance terms for endcap clusters
        if (not element.isBarrel() )
            std::swap( variance(0, 0), variance(1, 0) );

        std::unique_ptr<ActsTrk::SpacePoint> spacePoint =
          std::make_unique<ActsTrk::SpacePoint>( cluster.globalPosition().cast <double> (),
                                                 variance,
                                                 data,
                                                 measIndexes );
        return spacePoint;
    }
}
