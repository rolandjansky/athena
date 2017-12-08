/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtMathSegmentFinder.h"

#include "MuonSegmentMakerInterfaces/IDCSLFitProvider.h"

#include "TrkDriftCircleMath/SegmentFinder.h"
#include "TrkDriftCircleMath/Road.h"
#include "TrkDriftCircleMath/DCStatistics.h"
#include "TrkDriftCircleMath/DCSLFitter.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"


namespace Muon {

  MdtMathSegmentFinder::MdtMathSegmentFinder (const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool (t, n, p),
    m_dcslFitProvider(""),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_segmentFinder(0),
    m_dcslFitter(0),
    m_fitter(0)
  {
    declareInterface <IMdtSegmentFinder> (this);

    declareProperty("FinderDebugLevel",       m_finderDebugLevel = 0, "switch on debug output of finder");
    declareProperty("DoDrop",                 m_doDrop = true, "Recursive outlier removal");
    declareProperty("Chi2PerDofDropping",     m_chi2PerDofDrop        = 10.,  "Chi2 cut for recursive outlier removal" );
    declareProperty("RatioEmptyTubeCut",      m_ratioEmptyTubesCut    = 1.1,  "holes/hits cut - holes are all non-hits along the line" );
    declareProperty("EnableSeedCleaning",     m_enableSeedCleaning    = false, "only use dc witout neighbours as seeds" );
    declareProperty("OccupancyThreshold",     m_occupancyThreshold    = 0.3,  "occupancy threshold before enabling seed cleaning");    
    declareProperty("OccupancyCutoff",        m_occupancyCutOff       = 0.8,  "above the occupancy threshold no segment finding");    
    declareProperty("RPCAssocationPullcut",   m_rpcAssociationPullCut = 5.,   "Association cut for RPCs" );
    declareProperty("TGCAssocationPullcut",   m_tgcAssociationPullCut = 5.,   "Association cut for TGCs" );
    declareProperty("MDTAssocationPullcut",   m_mdtAssociationPullCut = 5.,   "Association cut for MDTs" );
    declareProperty("UseChamberTheta",        m_useChamberTheta       = true, "Always look for pointing segments" );
    declareProperty("SortSegmentWithAllHits", m_doAllHitSort          = true, "Including triggers in segment selection" );
    declareProperty("AssociationRoadWidth",   m_roadWidth = 1.5, "Road width used during hit association with seed lines" );
    declareProperty("SearchForPointingSegments", m_useChamberTheta = true, "Always search for pointing segments" );
    declareProperty("DoRoadSeeding",          m_doRoadAngleSeeding = true );
    declareProperty("DoIPSeeding",            m_doIPAngleSeeding = true );
    declareProperty("TightRoadCut",           m_tightRoadCut = 0.1 );
    declareProperty("MaxHitsPerFullSearch",   m_maxHitsPerFullSearch = 100 );
    declareProperty("DoSingleMultiLayerScan", m_doSingleMultiLayerScan = true,"Look for segments in one multi layer");
    declareProperty("RecoverMdtOutliers",     m_recoverMdtOutliers = true,"Recover MDT outliers after fit");
    declareProperty("RemoveSingleMdtOutliers",m_removeSingleOutliers = true,"Remove single MDT outliers");
    declareProperty("DoCurvedSegmentFinder",  m_doCurvedSegmentFinder = false,"Use the curved segment finding routine");
    declareProperty("DeltaCutT0Segments",     m_deltaCutT0Segments = 5.,"Delta cut for segments with T0 fit");
    declareProperty("ResidualCutT0Segments",  m_residualCutT0Segments = 1.,"Residual cut for segments with T0 fit");
    declareProperty("UseSegmentQuality",      m_useSegmentQuality = false,"Use segment quality in hit dropping");    
    declareProperty("DCFitProvider",          m_dcslFitProvider );
    declareProperty("IdHelper",               m_idHelperTool );
  }

  MdtMathSegmentFinder::~MdtMathSegmentFinder()
  {
  }

  StatusCode MdtMathSegmentFinder::initialize()
  {
    
    if( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;

    // setup finder
    m_segmentFinder = new TrkDriftCircleMath::SegmentFinder(m_roadWidth,m_mdtAssociationPullCut,false);

    // set debug level
    m_segmentFinder->debugLevel(m_finderDebugLevel);

    // configure uasge of chamber position for angular seeding
    m_segmentFinder->setUseChamberPhi( m_useChamberTheta );
    
    // enable dropping of hits
    m_segmentFinder->setDropHits( m_doDrop );

    // enable seed cleaing
    m_segmentFinder->setSeedCleaning( m_enableSeedCleaning );

    // enable seed cleaing
    m_segmentFinder->setSingleMultiLayerScan( m_doSingleMultiLayerScan );

    // set chi2/ndof threshold for cleaning of segments
    m_segmentFinder->setChi2DropCut( m_chi2PerDofDrop ); 

    // set ratio for dropping segments with many empty tubes
    m_segmentFinder->setRatioEmptyTubesCut( m_ratioEmptyTubesCut );

    // set sort mode segment finder
    m_segmentFinder->setSortSegmentsUsingAllHits( m_doAllHitSort );

    // set RPC pull cut
    m_segmentFinder->setRPCPullCut( m_rpcAssociationPullCut );

    // set TGC pull cut
    m_segmentFinder->setTGCPullCut( m_tgcAssociationPullCut );

    // set MDT outlier recovery
    m_segmentFinder->setRecoverMDT( m_recoverMdtOutliers );

    // set removal of single outliers
    m_segmentFinder->setRemoveSingleOutliers( m_removeSingleOutliers );    

    // set the curved segment finder
    m_segmentFinder->setCurvedSegmentFinder( m_doCurvedSegmentFinder );

    // set removal of single outliers
    m_segmentFinder->setDeltaCutT0( m_deltaCutT0Segments );

    // set removal of single outliers
    m_segmentFinder->setResidualCutT0( m_residualCutT0Segments );

    // set removal of single outliers
    m_segmentFinder->setUseSegmentQuality( m_useSegmentQuality );

    if( !m_dcslFitProvider.empty() ){
      if( m_dcslFitProvider.retrieve().isFailure() ){
	ATH_MSG_WARNING(" failed to retrieve " << m_dcslFitProvider);
	return StatusCode::FAILURE;
      }
      ATH_MSG_INFO(" Using fitter from " << m_dcslFitProvider);
      m_fitter = m_dcslFitProvider->getFitter();
      m_segmentFinder->setFitter( m_fitter );
      m_dcslFitter = new TrkDriftCircleMath::DCSLFitter();
    }

    if( !m_idHelperTool.empty() ){
      if( m_idHelperTool.retrieve().isFailure() ){
	ATH_MSG_WARNING(" failed to retrieve " << m_idHelperTool);
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode MdtMathSegmentFinder::finalize()
  {
    
    delete m_segmentFinder;
    delete m_dcslFitter;
    return AthAlgTool::finalize();
  }

  const TrkDriftCircleMath::SegVec MdtMathSegmentFinder::findSegments (const TrkDriftCircleMath::DCVec& dcvec,
								       const TrkDriftCircleMath::CLVec& clvec,
								       const TrkDriftCircleMath::Road& road,
								       const TrkDriftCircleMath::DCStatistics& dcstats,
								       const TrkDriftCircleMath::ChamberGeometry* multiGeo = 0 ) const
  {
    // reset defaults
    if( m_fitter ) {
      m_segmentFinder->setFitter( m_fitter );
    }
    m_segmentFinder->setSeedCleaning( m_enableSeedCleaning );

    // set angle prediction from road
    m_segmentFinder->setPhiRoad( road.angle(), road.chamberAngle(), road.width(), m_doRoadAngleSeeding, m_doIPAngleSeeding );

    // set pointer to geometry
    m_segmentFinder->setMdtGeometry( multiGeo );
    m_segmentFinder->setSingleMultiLayerScan( m_doSingleMultiLayerScan );

    // set seed cleaning
    bool highOccupancy = false;
    bool aboveOccupancyCut = false;
    double occupancyMax = 0.;
    unsigned int nmdtHits = 0;
    // calculate multi layer occupancy
    TrkDriftCircleMath::DCStatCit mit = dcstats.begin();
    TrkDriftCircleMath::DCStatCit mit_end = dcstats.end();
    for( ;mit!=mit_end;++mit ){
	
      unsigned int channels = mit->first->getNLayers()*mit->first->getNtubesperlayer();
      double occupancy = (double)mit->second/(double)channels;

      nmdtHits += mit->second;

      if( occupancy > occupancyMax ) occupancyMax = occupancy;

      if( occupancy > m_occupancyThreshold ) highOccupancy = true;
      
      if( occupancy > m_occupancyCutOff ) aboveOccupancyCut = true;

      ATH_MSG_VERBOSE(" multilayer occupancy: " << occupancy 
		      << "   " << m_idHelperTool->toStringDetEl(mit->first->identify()));

    }

    // sanity check
    if( nmdtHits != dcvec.size() ){
      ATH_MSG_WARNING(" inconsistent number of mdt hits " << nmdtHits << " from vec " << dcvec.size());
      nmdtHits = dcvec.size();
    }

    if( aboveOccupancyCut ) {
      ATH_MSG_DEBUG(" layer with occupancy above threshold, aborting segment finding " << occupancyMax << " cut " << m_occupancyCutOff
		    << " nhits " << nmdtHits << " cut " << m_maxHitsPerFullSearch);
      return TrkDriftCircleMath::SegVec();
    }
    
    // enable seed cleaning
    if( highOccupancy || nmdtHits > m_maxHitsPerFullSearch ) {

      
      ATH_MSG_DEBUG(" switch to fast search: occupancy " << occupancyMax << " cut " << m_occupancyThreshold
		    << " nhits " << nmdtHits << " cut " << m_maxHitsPerFullSearch);

      // to speed up reconstruction use default fitter if special is selected
      if( !m_dcslFitProvider.empty() ) {
	m_segmentFinder->setFitter( m_dcslFitter );
      }
      
      // use tight road cuts and only look for pointing segments
      m_segmentFinder->setPhiRoad( road.chamberAngle(), road.chamberAngle(), m_tightRoadCut );

      // only perform two layer scan
      m_segmentFinder->setSingleMultiLayerScan( false );

    }

    m_segmentFinder->setSingleMultiLayerScan( m_doSingleMultiLayerScan );

    return m_segmentFinder->findSegments(dcvec,clvec);
  }


}
