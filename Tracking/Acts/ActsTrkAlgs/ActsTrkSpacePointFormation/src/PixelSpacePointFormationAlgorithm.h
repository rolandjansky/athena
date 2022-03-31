/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSPACEPOINTFORMATION_PIXELSPACEPOINTFORMATIONALG_H
#define ACTSTRKSPACEPOINTFORMATION_PIXELSPACEPOINTFORMATIONALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkToolInterfaces/IPixelSpacePointFormationTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "xAODInDetMeasurement/PixelClusterContainer.h"

#include <string>

namespace ActsTrk {

    /// @class PixelSpacePointFormationAlgorithm
    /// This version of PixelSpacePointFormationAlgorithm uses xAOD pixel clusters
    /// to find space points in the ITk pixeldetectors.
    /// Pixel space points are obtained directly from the clusters,
    /// with needed evaluation of the space point covariance terms
    /// Space points are then recorded to storegate as ActsTrk::SpacePoint
    /// into an ActsTrk::SpacePointContainer.

    class PixelSpacePointFormationAlgorithm : public AthReentrantAlgorithm {
    public:
        /// @name AthReentrantAlgorithm methods
        //@{
        PixelSpacePointFormationAlgorithm(const std::string& name,
                                     ISvcLocator* pSvcLocator);
        virtual ~PixelSpacePointFormationAlgorithm() = default;
        virtual StatusCode initialize() override;
        virtual StatusCode execute (const EventContext& ctx) const override;
        //@}

    private:
        /// @name Disallow constructor without parameters, copy constructor, assignment operator
        //@{
        PixelSpacePointFormationAlgorithm() = delete;
        PixelSpacePointFormationAlgorithm(const PixelSpacePointFormationAlgorithm&) = delete;
        PixelSpacePointFormationAlgorithm &operator=(const PixelSpacePointFormationAlgorithm&) = delete;
        //@}

        /// @name Input data using SG::ReadHandleKey
        //@{
        SG::ReadHandleKey<xAOD::PixelClusterContainer> m_pixelClusterContainerKey{this, "PixelClusters", "ITkPixelClusters", "name of the input pixel cluster container"};
        //@}

        /// @name Input condition data using SG::ReadCondHandleKey
        //@{
        /// To get detector elements
        SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetectorElements", "ITkPixelDetectorElementCollection", "Key of input SiDetectorElementCollection for Pixel"};
        //@}

        ///@name Output data using SG::WriteHandleKey
        //@{
        SG::WriteHandleKey<ActsTrk::SpacePointContainer> m_pixelSpacePointContainerKey{this, "PixelSpacePoints", "ITkPixelSpacePoints", "name of the output pixel space point container"};
        SG::WriteHandleKey<ActsTrk::SpacePointData> m_pixelSpacePointDataKey {this,"PixelSpacePointData","ITkPixelSpacePointData","name of the output pixel space point data"};
        //@}

        /// @name ToolHandle
        //@{
        /// For space point formation
        ToolHandle<ActsTrk::IPixelSpacePointFormationTool> m_spacePointMakerTool{this, "SpacePointFormationTool", "", "Tool dedicated to the creation of pixel space points"};
        /// For monitoring
        ToolHandle<GenericMonitoringTool> m_monTool{this, "monTool", "", "Monitoring tool"};
        //@}

  };

}

#endif // ACTSTRKSPACEPOINTFORMATION_PIXELSPACEPOINTFORMATIONALG_H
