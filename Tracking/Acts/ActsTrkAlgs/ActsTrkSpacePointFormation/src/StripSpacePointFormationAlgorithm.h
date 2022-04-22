/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSPACEPOINTFORMATION_STRIPSPACEPOINTFORMATIONALG_H
#define ACTSTRKSPACEPOINTFORMATION_STRIPSPACEPOINTFORMATIONALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "ActsTrkToolInterfaces/IStripSpacePointFormationTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "xAODInDetMeasurement/StripClusterContainer.h"

#include <string>

namespace ActsTrk {

    /// @class StripSpacePointFormationAlgorithm
    /// This version of StripSpacePointFormationAlgorithm uses xAOD strip clusters
    /// to find space points in the ITk strip detectors.
    /// Strip space points are made by combining clusters from pairs of
    /// overlapping detectors. The access to overlapping detector elements is
    /// possible using the ContainerAccessor.
    /// The user can choose just to process the detector element and
    /// its opposite on the stereo layer, or also to consider overlaps with the
    /// four nearest neighbours of the opposite elements.
    ///
    /// Space points are then recorded to storegate as ActsTrk::SpacePoint
    /// into an ActsTrk::SpacePointContainer.

    class StripSpacePointFormationAlgorithm : public AthReentrantAlgorithm {
    public:
        /// @name AthReentrantAlgorithm methods
        //@{
        StripSpacePointFormationAlgorithm(const std::string& name,
                                     ISvcLocator* pSvcLocator);
        virtual ~StripSpacePointFormationAlgorithm() = default;
        virtual StatusCode initialize() override;
        virtual StatusCode execute (const EventContext& ctx) const override;
        //@}

    private:
        /// @name Disallow constructor without parameters, copy constructor, assignment operator
        //@{
        StripSpacePointFormationAlgorithm() = delete;
        StripSpacePointFormationAlgorithm(const StripSpacePointFormationAlgorithm&) = delete;
        StripSpacePointFormationAlgorithm &operator=(const StripSpacePointFormationAlgorithm&) = delete;
        //@}

        /// @name Input data using SG::ReadHandleKey
        //@{
        SG::ReadHandleKey<xAOD::StripClusterContainer>  m_stripClusterContainerKey{this, "StripClusters", "ITkStripClusters", "name of the input strip cluster container"};
        //@}

        /// @name Input condition data using SG::ReadCondHandleKey
        //@{
        /// To get beam spot data
        SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

        /// To get detector elements
        SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_stripDetEleCollKey{this, "StripDetectorElements", "ITkStripDetectorElementCollection", "Key of input SiDetectorElementCollection for Strip"};
        /// To get strip module neighbours
        SG::ReadCondHandleKey<InDet::SiElementPropertiesTable> m_stripPropertiesKey{this, "StripElementPropertiesTable", "ITkStripElementPropertiesTable", "Key of input SiElementPropertiesTable for Strip"};
        //@}

        ///@name Output data using SG::WriteHandleKey
        //@{
        SG::WriteHandleKey<ActsTrk::SpacePointContainer> m_stripSpacePointContainerKey{this, "StripSpacePoints", "ITkStripSpacePoints", "name of the output strip space point container"};
        SG::WriteHandleKey<ActsTrk::SpacePointData> m_stripSpacePointDataKey {this,"StripSpacePointData","ITkStripSpacePointData","name of the output strip space point data"};
        SG::WriteHandleKey<ActsTrk::SpacePointMeasurementDetails> m_stripSpacePointsDetailsKey {this,"StripSpacePointDetails","ITkStripSpacePointDetails","name of the output strip space point measurement details"};
        SG::WriteHandleKey<ActsTrk::SpacePointContainer> m_stripOverlapSpacePointContainerKey{this, "StripOverlapSpacePoints", "ITkStripOverlapSpacePoints", "name of the strip overlap strip space point container"};
        SG::WriteHandleKey<ActsTrk::SpacePointData> m_stripOverlapSpacePointDataKey {this,"StripOverlapSpacePointData","ITkStripOverlapSpacePointData","name of the output strip overlap space point data"};
        SG::WriteHandleKey<ActsTrk::SpacePointMeasurementDetails> m_stripOverlapSpacePointsDetailsKey {this,"StripOverlapSpacePointDetails","ITkStripOverlapSpacePointDetails","name of the output strip overlap space point measurement details"};
        //@}

        /// @name ToolHandle
        //@{
        /// For space point formation
        ToolHandle<ActsTrk::IStripSpacePointFormationTool> m_spacePointMakerTool{this, "SpacePointFormationTool", "", "Tool dedicated to the creation of pixel space points"};
        /// For monitoring
        ToolHandle<GenericMonitoringTool> m_monTool{this, "monTool", "", "Monitoring tool"};
        //@}

        /// @name Configuration flags
        //@{
        BooleanProperty m_processOverlapForStrip{this, "ProcessOverlapForStrip", true, "Enable production of eta/phi overlapping strip space points."};
        //@}

  };

}

#endif // ACTSTRKSPACEPOINTFORMATION_STRIPSPACEPOINTFORMATIONALG_H
