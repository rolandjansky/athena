/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSpacePointFormationAlgorithm.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "xAODInDetMeasurement/PixelClusterAuxContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"

namespace ActsTrk {

  //------------------------------------------------------------------------
  PixelSpacePointFormationAlgorithm::PixelSpacePointFormationAlgorithm(const std::string& name,
                                                                       ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
    {}

  //-----------------------------------------------------------------------
  StatusCode PixelSpacePointFormationAlgorithm::initialize()
  {
    ATH_MSG_DEBUG( "PixelSpacePointFormationAlgorithm::initialize()" );

    ATH_CHECK( m_pixelClusterContainerKey.initialize() );
    ATH_CHECK( m_pixelSpacePointContainerKey.initialize() );
    ATH_CHECK( m_pixelSpacePointDataKey.initialize() );
    ATH_CHECK( m_pixelDetEleCollKey.initialize() );
    ATH_CHECK( m_spacePointMakerTool.retrieve() );

    if ( not m_monTool.empty() )
      ATH_CHECK( m_monTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------
  StatusCode PixelSpacePointFormationAlgorithm::execute (const EventContext& ctx) const
  {
    auto nReceivedSPsPixel = Monitored::Scalar<int>( "numPixSpacePoints" , 0 );
    auto mon = Monitored::Group( m_monTool, nReceivedSPsPixel );

    auto pixelSpacePointContainer = SG::WriteHandle<ActsTrk::SpacePointContainer>( m_pixelSpacePointContainerKey, ctx );
    ATH_MSG_DEBUG( "--- Pixel Space Point Container `" << m_pixelSpacePointContainerKey.key() << "` created ..." );
    auto pixelSpacePointData = SG::WriteHandle<ActsTrk::SpacePointData>( m_pixelSpacePointDataKey, ctx );
    ATH_MSG_DEBUG( "--- Pixel Space Point Data `" << m_pixelSpacePointDataKey.key() << "` created ..." );

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
    const InDetDD::SiDetectorElementCollection* pixelElements(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixelElements==nullptr) {
      ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::FAILURE;
    }

    std::unique_ptr<ActsTrk::SpacePointContainer> spacePoints =
        std::make_unique<ActsTrk::SpacePointContainer>();
    std::unique_ptr<ActsTrk::SpacePointData> spacePointData =
        std::make_unique<ActsTrk::SpacePointData>();

    SG::ReadHandle<xAOD::PixelClusterContainer> inputPixelClusterContainer( m_pixelClusterContainerKey, ctx );
    if (!inputPixelClusterContainer.isValid()){
        ATH_MSG_FATAL("xAOD::PixelClusterContainer with key " << m_pixelClusterContainerKey.key() << " is not available...");
        return StatusCode::FAILURE;
    }

    /// production of ActsTrk::SpacePoint from pixel clusters
    /// Pixel space points are created directly from the clusters global position
    /// Counter represents the index of the cluster in the xAOD::PixelClusterContainer
    std::size_t counter = 0;
    for ( auto cluster : *inputPixelClusterContainer ) {
      boost::container::static_vector<std::size_t, 2> measIndexes({counter++});
      const InDetDD::SiDetectorElement* pixelElement = pixelElements->getDetectorElement(cluster->identifierHash());
      if (pixelElement == nullptr) {
        ATH_MSG_FATAL("Element pointer is nullptr");
        return StatusCode::FAILURE;
      }
      std::unique_ptr<ActsTrk::SpacePoint> pixelSpacePoint =
        m_spacePointMakerTool->producePixelSpacePoint(*cluster,
                                                      *spacePointData.get(),
                                                      measIndexes,
                                                      *pixelElement );
      spacePoints->push_back( std::move(pixelSpacePoint) );
    }

    ATH_CHECK( pixelSpacePointContainer.record( std::move( spacePoints ) ) );
    ATH_CHECK( pixelSpacePointData.record( std::move( spacePointData ) ) );

    nReceivedSPsPixel = pixelSpacePointContainer->size();

    return StatusCode::SUCCESS;
  }

} //namespace
