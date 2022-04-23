/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "StripSpacePointFormationAlgorithm.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "xAODInDetMeasurement/StripClusterAuxContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"

namespace ActsTrk {

  //------------------------------------------------------------------------
  StripSpacePointFormationAlgorithm::StripSpacePointFormationAlgorithm(const std::string& name,
                                                                       ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
    {}

  //-----------------------------------------------------------------------
  StatusCode StripSpacePointFormationAlgorithm::initialize()
  {
    ATH_MSG_DEBUG( "StripSpacePointFormationAlgorithm::initialize()" );

    ATH_CHECK( m_stripClusterContainerKey.initialize() );
    ATH_CHECK( m_stripSpacePointContainerKey.initialize() );
    ATH_CHECK( m_stripSpacePointDataKey.initialize() );
    ATH_CHECK( m_stripSpacePointsDetailsKey.initialize() );
    ATH_CHECK( m_stripOverlapSpacePointContainerKey.initialize( m_processOverlapForStrip) );
    ATH_CHECK( m_stripOverlapSpacePointDataKey.initialize( m_processOverlapForStrip) );
    ATH_CHECK( m_stripOverlapSpacePointsDetailsKey.initialize( m_processOverlapForStrip) );
    ATH_CHECK( m_stripDetEleCollKey.initialize() );
    ATH_CHECK( m_stripPropertiesKey.initialize() );

    ATH_CHECK( m_beamSpotKey.initialize() );

    if ( not m_monTool.empty() )
      ATH_CHECK( m_monTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------
  StatusCode StripSpacePointFormationAlgorithm::execute (const EventContext& ctx) const
  {
    auto nReceivedSPsStrip = Monitored::Scalar<int>( "numStripSpacePoints" , 0 );
    auto nReceivedSPsStripOverlap = Monitored::Scalar<int>( "numStripOverlapSpacePoints" , 0 );
    auto mon = Monitored::Group( m_monTool, nReceivedSPsStrip, nReceivedSPsStripOverlap );

    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    const InDet::BeamSpotData* beamSpot = *beamSpotHandle;
    auto vertex = beamSpot->beamVtx().position();

    auto stripSpacePointContainer = SG::WriteHandle<ActsTrk::SpacePointContainer>( m_stripSpacePointContainerKey, ctx );
    ATH_MSG_DEBUG( "--- Strip Space Point Container `" << m_stripSpacePointContainerKey.key() << "` created ..." );
    auto stripSpacePointData = SG::WriteHandle<ActsTrk::SpacePointData>( m_stripSpacePointDataKey, ctx );
    ATH_MSG_DEBUG( "--- Strip Space Point Data `" << m_stripSpacePointDataKey.key() << "` created ..." );
    auto stripSpacePointDetails = SG::WriteHandle<ActsTrk::SpacePointMeasurementDetails>( m_stripSpacePointsDetailsKey, ctx );
    ATH_MSG_DEBUG( "--- Strip Space Point Measurement Details `" << m_stripSpacePointsDetailsKey.key() << "` created ..." );


    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> stripDetEleHandle(m_stripDetEleCollKey, ctx);
    const InDetDD::SiDetectorElementCollection* stripElements(*stripDetEleHandle);
    if (not stripDetEleHandle.isValid() or stripElements==nullptr) {
      ATH_MSG_FATAL(m_stripDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::FAILURE;
    }

    SG::ReadCondHandle<InDet::SiElementPropertiesTable> stripProperties(m_stripPropertiesKey, ctx);
    const InDet::SiElementPropertiesTable* properties = stripProperties.retrieve();
    if (properties==nullptr) {
      ATH_MSG_FATAL("Pointer of SiElementPropertiesTable (" << m_stripPropertiesKey.fullKey() << ") could not be retrieved");
      return StatusCode::FAILURE;
    }

    std::unique_ptr<ActsTrk::SpacePointContainer> spacePoints =
        std::make_unique<ActsTrk::SpacePointContainer>();
    std::unique_ptr<ActsTrk::SpacePointData> spacePointData =
        std::make_unique<ActsTrk::SpacePointData>();
    std::unique_ptr<ActsTrk::SpacePointMeasurementDetails> spacePointDetails =
        std::make_unique<ActsTrk::SpacePointMeasurementDetails>();
    std::unique_ptr<ActsTrk::SpacePointContainer> overlapSpacePoints =
        std::make_unique<ActsTrk::SpacePointContainer>();
    std::unique_ptr<ActsTrk::SpacePointData> overlapSpacePointData =
        std::make_unique<ActsTrk::SpacePointData>();
    std::unique_ptr<ActsTrk::SpacePointMeasurementDetails> overlapSpacePointDetails =
        std::make_unique<ActsTrk::SpacePointMeasurementDetails>();

    SG::ReadHandle<xAOD::StripClusterContainer> inputStripClusterContainer( m_stripClusterContainerKey, ctx );
    if (!inputStripClusterContainer.isValid()){
        ATH_MSG_FATAL("xAOD::StripClusterContainer with key " << m_stripClusterContainerKey.key() << " is not available...");
        return StatusCode::FAILURE;
    }

    m_spacePointMakerTool->produceStripSpacePoints(*inputStripClusterContainer.cptr(),
                                                   *properties,
                                                   *stripElements,
                                                   vertex,
                                                   *spacePoints.get(), *spacePointData.get(),
                                                   *spacePointDetails.get(),
                                                   *overlapSpacePoints.get(), *overlapSpacePointData.get(),
                                                   *overlapSpacePointDetails.get(),
                                                   m_processOverlapForStrip);

    ATH_CHECK( stripSpacePointContainer.record( std::move( spacePoints ) ) );
    ATH_CHECK( stripSpacePointData.record( std::move( spacePointData ) ) );
    ATH_CHECK( stripSpacePointDetails.record( std::move( spacePointDetails ) ) );

    nReceivedSPsStrip = stripSpacePointContainer->size();

    if (m_processOverlapForStrip) {
      auto stripOverlapSpacePointContainer = SG::WriteHandle<ActsTrk::SpacePointContainer>( m_stripOverlapSpacePointContainerKey, ctx );
      ATH_MSG_DEBUG( "--- Strip Overlap Space Point Container `" << m_stripOverlapSpacePointContainerKey.key() << "` created ..." );
      auto stripOverlapSpacePointData = SG::WriteHandle<ActsTrk::SpacePointData>( m_stripOverlapSpacePointDataKey, ctx );
      ATH_MSG_DEBUG( "--- Strip Overlap Space Point Data `" << m_stripOverlapSpacePointDataKey.key() << "` created ..." );
      auto stripOverlapSpacePointDetails = SG::WriteHandle<ActsTrk::SpacePointMeasurementDetails>( m_stripOverlapSpacePointsDetailsKey, ctx );
      ATH_MSG_DEBUG( "--- Strip Overlap Space Point Measurement Details `" << m_stripOverlapSpacePointsDetailsKey.key() << "` created ..." );
      ATH_CHECK( stripOverlapSpacePointContainer.record( std::move( overlapSpacePoints ) ) );
      ATH_CHECK( stripOverlapSpacePointData.record( std::move( overlapSpacePointData ) ) );
      ATH_CHECK( stripOverlapSpacePointDetails.record( std::move( overlapSpacePointDetails ) ) );

      nReceivedSPsStripOverlap = stripOverlapSpacePointContainer->size();
    }

    return StatusCode::SUCCESS;
  }

}
