/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DCMathSegmentMaker.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/Timeout.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "MuonSegment/MuonSegmentQuality.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/JacobianCotThetaPtToThetaP.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkRoad/TrackRoad.h"

#include "TrkTrack/Track.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/PlaneSurface.h"

#include "MuonSegmentMakerInterfaces/IDCSLFitProvider.h"

#include "TrkDriftCircleMath/SegmentFinder.h"
#include "TrkDriftCircleMath/Road.h"
#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/ClusterId.h"
#include "TrkDriftCircleMath/ResidualWithSegment.h"
#include "TrkDriftCircleMath/MdtStationId.h"
#include "TrkDriftCircleMath/MdtMultiChamberGeometry.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtSegmentFinder.h"
#include "MuonRecToolInterfaces/IMuonSegmentFittingTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"

#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"
#include "MuonStationIntersectSvc/MdtIntersectGeometry.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonRecToolInterfaces/IAdjustableT0Tool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include <vector>
#include <cassert>
#include <iostream>
#include <functional>

namespace Muon {

  DCMathSegmentMaker::DCMathSegmentMaker( const std::string& t, const std::string& n, const IInterface*  p ) :
    AthAlgTool(t,n,p),
    m_detMgr(0),
    m_intersectSvc("MuonStationIntersectSvc", name()),
    m_mdtCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_mdtCreatorT0("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_compClusterCreator("Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_segmentFinder("Muon::MdtMathSegmentFinder/MdtMathSegmentFinder"),
    m_tofTool(""),
    m_segmentFitter("Muon::MuonSegmentFittingTool/MuonSegmentFittingTool"),
    m_segmentSelectionTool("Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"),
    m_dcslFitProvider(""),
    m_multiGeo(0),
    m_dcslFitter(0),
    m_phimin(0.),
    m_phimax(0.),
    m_refit(false),
    m_isEndcap(false),
    m_nmultipleHitWarnings(0),
    m_rpcKey("RPC_Measurements"),
    m_tgcKey("TGC_Measurements"),
    m_mdtKey("MDT_DriftCircles")
  {
    declareInterface<IMuonSegmentMaker>(this);
    declareInterface<IMuonSegmentTriggerHitAssociator>(this);

    declareProperty("MuonStationIntersectSvc", m_intersectSvc);
    declareProperty("MdtCreator",     m_mdtCreator);
    declareProperty("MdtCreatorT0",     m_mdtCreatorT0);
    declareProperty("MuonCompetingClustersCreator",     m_compClusterCreator);
    declareProperty("IdHelper", m_idHelperTool);
    declareProperty("EDMPrinter", m_printer);
    declareProperty("EDMHelper", m_helper);    
    declareProperty("MdtSegmentFinder",     m_segmentFinder);
    declareProperty("TofTool",     m_tofTool);
    declareProperty("SegmentFitter", m_segmentFitter);
    declareProperty("SegmentSelector", m_segmentSelectionTool);
    declareProperty("DCFitProvider", m_dcslFitProvider );

    
    declareProperty("SinAngleCut",m_sinAngleCut = 0.2);
    declareProperty("DoGeometry",m_doGeometry = true);
    declareProperty("DoDebugSpacePoints",m_debugSpacePoints = false);
    declareProperty("CurvedErrorScaling",m_curvedErrorScaling = true);
    declareProperty("UseTriggerSpacePoints",m_doSpacePoints = true);
    declareProperty("CreateCompetingROTsPhi",m_createCompetingROTsPhi = true);
    declareProperty("CreateCompetingROTsEta",m_createCompetingROTsEta = true);
    declareProperty("RefitSegment",m_refitParameters = false);
    declareProperty("AddUnassociatedPhiHits",m_addUnassociatedPhiHits = false);
    declareProperty("StrictRoadDirectionConsistencyCheck",m_strictRoadDirectionConsistencyCheck = true);
    declareProperty("MaxAssociateClusterDistance",m_maxAssociateClusterDistance=3000);
    declareProperty("AllMdtHoles",m_allMdtHoles = false, "flag to decide whether to apply bound checks during the hole search");
    declareProperty("RemoveDeltasFromSegmentQuality",m_removeDeltas = true, "flag to decide whether add deltas to SegmentSummary");
    declareProperty("Reject1DTgcSpacePoints",m_reject1DTgcSpacePoints = true ,"reject TGC eta hits that are not associated with a phi hit in the same gas gap" );
    declareProperty("UsePreciseError",m_usePreciseError = false );
    declareProperty("PreciseErrorScale",m_preciseErrorScale = 2. );
    declareProperty("OutputFittedT0",m_outputFittedT0 = false );
    declareProperty("UseTimeOutGard",m_doTimeOutChecks = true ); 
    declareProperty("RecoverBadRpcCabling",m_recoverBadRpcCabling = false );
    declareProperty("UpdatePhiUsingPhiHits",m_updatePhiUsingPhiHits = false );
    declareProperty("AssumePointingPhi",m_assumePointingPhi = false );
    declareProperty("Redo2DFit", m_redo2DFit = true );

    declareProperty("RpcPrepDataContainer", m_rpcKey);
    declareProperty("TgcPrepDataContainer", m_tgcKey);
    declareProperty("MdtPrepDataContainer", m_mdtKey);

  }

  DCMathSegmentMaker::~DCMathSegmentMaker()
  {
  }

  StatusCode DCMathSegmentMaker::initialize()
  {
    
    // retrieve MuonDetectorManager
    ATH_CHECK( detStore()->retrieve(m_detMgr,"Muon") );
    ATH_CHECK( m_intersectSvc.retrieve() );
    ATH_CHECK( m_mdtCreator.retrieve() );
    ATH_CHECK( m_mdtCreatorT0.retrieve() );
    ATH_CHECK( m_compClusterCreator.retrieve() );
    ATH_CHECK( m_idHelperTool.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_helper.retrieve() );
    ATH_CHECK( m_segmentFinder.retrieve() );
    ATH_CHECK( m_segmentSelectionTool.retrieve() );
    
    if( !m_tofTool.empty() ){
      ATH_CHECK( m_tofTool.retrieve() );
    }

    if( m_refitParameters ){
      ATH_CHECK( m_segmentFitter.retrieve() );
    }

    if( !m_dcslFitProvider.empty() ){
      ATH_CHECK( m_dcslFitProvider.retrieve() );
      m_dcslFitter = m_dcslFitProvider->getFitter();
      ATH_MSG_INFO(" Using " << m_dcslFitProvider);
    }

  //initialise for data handles

    if(!m_rpcKey.initialize()){
      ATH_MSG_ERROR("Couldn't initalize RPC ReadHandleKey");
      return StatusCode::FAILURE;
    }
    if(!m_tgcKey.initialize()){
      ATH_MSG_ERROR("Couldn't initalize TGC ReadHandleKey");
      return StatusCode::FAILURE;
    }
    if(!m_mdtKey.initialize()){
      ATH_MSG_ERROR("Couldn't initalize MDT ReadHandleKey");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode DCMathSegmentMaker::finalize()
  {
    // delete member pointers
    for(std::vector<const Trk::MeasurementBase*>::iterator it=m_measurementsToBeDeleted.begin(); it!=m_measurementsToBeDeleted.end(); it++)
      delete (*it);
    m_measurementsToBeDeleted.clear();

    return StatusCode::SUCCESS;
  }


  const MuonSegment* DCMathSegmentMaker::associateTriggerHits( const MuonSegment& seg, const std::vector<const MuonClusterOnTrack*>& clus,
							       bool includeEtaHits ) const {
    
    ATH_MSG_DEBUG("associateTriggerHits: clusters " << clus.size() );
    // extract hits
    std::vector<const MdtDriftCircleOnTrack*> mdts;
    std::vector<const Trk::MeasurementBase*>::const_iterator hit = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator hit_end = seg.containedMeasurements().end();
    for( ;hit!=hit_end;++hit ){
      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*hit);
      if( mdt ) mdts.push_back(mdt);
    }

    if( mdts.size() < 2 )return 0;
    const MdtDriftCircleOnTrack* firstRot = findFirstRotInChamberWithMostHits(mdts);

    if( !firstRot ) {
      if( m_nmultipleHitWarnings < 3 ) ATH_MSG_WARNING(" no Mdt selected, returning 0 ");
      return 0;
    }

    const MuonGM::MdtReadoutElement* detEl = firstRot->detectorElement();

    if( !detEl ) {
      ATH_MSG_WARNING(" no MdtReadoutElement found, returning 0 ");
      return 0;
    }
 
    // identifier
    Identifier chid = firstRot->identify();

    // endcap or barrel
    m_isEndcap = m_idHelperTool->mdtIdHelper().isEndcap(chid);

    // global to local transformation for chamber
    m_gToStation = seg.associatedSurface().transform().inverse();

    // define axis of chamber in global coordinates
    m_amdbToGlobal = seg.associatedSurface().transform();

    std::vector<const MuonClusterOnTrack*> clusters = clus;
    // if requested, retrieve trigger eta hits
    if( includeEtaHits ) {
      addEtaHits(clusters);
      ATH_MSG_DEBUG("adding eta hits: original clusters " << clus.size() << " extended " << clusters.size() );
    }


    /* ***** create cluster hits ******** */
    TrkDriftCircleMath::CLVec cls = createClusterVec( clusters, chid );
    
    /* ***** create MDT hits ************ */
    ATH_MSG_DEBUG(" adding mdts " << mdts.size());

    TrkDriftCircleMath::DCOnTrackVec dcs;
    // reset chamber statistics
    m_dcStatistics.clear();
    bool firstMdt = true;;
    m_phimin = 9999.;
    m_phimax = -9999.;
    unsigned index = 0;
    std::vector<const Trk::MeasurementBase*>::const_iterator it = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator it_end = seg.containedMeasurements().end();
    for(  ;it!=it_end;++it){

      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*it);

      if( !mdt ){
        continue;
      }
      Identifier id = mdt->identify();

      // calculate local AMDB position
      Amg::Vector3D locPos = m_gToStation*mdt->prepRawData()->globalPosition();
      TrkDriftCircleMath::LocPos lpos(locPos.y(),locPos.z());

      double r  = mdt->localParameters()[Trk::locR];
      double dr = Amg::error(mdt->localCovariance(),Trk::locR);

      // create identifier
      TrkDriftCircleMath::MdtId mdtid( m_idHelperTool->mdtIdHelper().isBarrel(id),m_idHelperTool->mdtIdHelper().multilayer(id)-1, 
                                       m_idHelperTool->mdtIdHelper().tubeLayer(id)-1, m_idHelperTool->mdtIdHelper().tube(id)-1 );

      // create new DriftCircle
      TrkDriftCircleMath::DriftCircle dc( lpos, r, dr, TrkDriftCircleMath::DriftCircle::InTime, mdtid, index, mdt );
      TrkDriftCircleMath::DCOnTrack dcOnTrack(dc,1.,1.);
      ATH_MSG_VERBOSE(" new MDT hit " << m_idHelperTool->toString( id )  << " x " << lpos.x() << " y " << lpos.y() );
    
      TubeEnds tubeEnds = localTubeEnds(*mdt,m_gToStation,m_amdbToGlobal);
      if( firstMdt ){
	m_phimin = tubeEnds.phimin; 
	m_phimax = tubeEnds.phimax;
	firstMdt = false;
      }else{
	updatePhiRanges(tubeEnds.phimin,tubeEnds.phimax,m_phimin,m_phimax);
      }
  
      dcs.push_back( dcOnTrack );

      ++index;
    }    
    
    // create line
    double angleYZ = seg.localDirection().angleYZ();
    const Amg::Vector3D lpos = m_gToStation*seg.globalPosition();
    TrkDriftCircleMath::LocPos segPos(lpos.y(),lpos.z());
    TrkDriftCircleMath::Line segPars(segPos,angleYZ);

    const Trk::FitQuality* fq = seg.fitQuality();
    double chi2 = fq ? fq->chiSquared() : 0.;
    int ndof = fq ? fq->numberDoF() : 1;

    TrkDriftCircleMath::Segment segment(segPars,dcs,chi2,ndof,Amg::error(seg.localCovariance(),Trk::locY),
					Amg::error(seg.localCovariance(),Trk::theta));   
    segment.clusters(cls);
    segment.hitsOnTrack(6); // hack just putting a number above the cut of at least 2

    MuonSegment* newSeg = createSegment(segment,chid,seg.globalPosition(),seg.globalDirection(),mdts,true);
    for(std::vector<const Trk::MeasurementBase*>::iterator it=m_measurementsToBeDeleted.begin(); it!=m_measurementsToBeDeleted.end(); it++)
      delete (*it);
    m_measurementsToBeDeleted.clear();
    
    return newSeg;
  }
  
  void DCMathSegmentMaker::addEtaHits( std::vector<const MuonClusterOnTrack*>& clusters ) const {
    
    std::set<IdentifierHash> chIdHs;
    std::vector<const MuonClusterOnTrack*>::iterator cit = clusters.begin();
    std::vector<const MuonClusterOnTrack*>::iterator cit_end = clusters.end();
    for( ;cit!=cit_end;++cit ) chIdHs.insert((*cit)->collectionHash());
    ATH_MSG_VERBOSE("Number of collections " << chIdHs.size() );

    if( m_isEndcap ){
      const Muon::TgcPrepDataContainer* prdContainer = 0;
      SG::ReadHandle<Muon::TgcPrepDataContainer> TgcCont(m_tgcKey);
      if( !TgcCont.isValid() ) {
	ATH_MSG_WARNING("Cannot retrieve TgcPrepDataContainer ");
	return;
      }
      else {
        prdContainer = TgcCont.cptr();
      }
      if(!prdContainer){
	ATH_MSG_WARNING("No TGC prd container retrieved");
	return;
      }
      
      // loop over chambers and get collections
      std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
      std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
      for( ;chit!=chit_end;++chit ){
	TgcPrepDataContainer::const_iterator colIt = prdContainer->indexFind(*chit);
	if( colIt == prdContainer->end() || !*colIt ) {
	  ATH_MSG_VERBOSE("Could not find collection ");
	  continue;
	}
	const TgcPrepDataCollection& col = **colIt;
	ATH_MSG_VERBOSE("Found collection: " << m_idHelperTool->toStringChamber(col.identify()) << " size " << col.size() );
	TgcPrepDataCollection::const_iterator hit = col.begin();
	TgcPrepDataCollection::const_iterator hit_end = col.end();
	for( ;hit!=hit_end;++hit ){
	  // skip phi hits
	  if( m_idHelperTool->measuresPhi((*hit)->identify()) ) continue;

	  const MuonClusterOnTrack* clus = m_clusterCreator->createRIO_OnTrack(**hit,(*hit)->globalPosition());
	  if( clus ) {
	    ATH_MSG_VERBOSE("  adding hit: " << m_idHelperTool->toString(clus->identify()) );
	    clusters.push_back(clus);
	    m_measurementsToBeDeleted.push_back(clus);
	  }
	}
      }

    }else{
      const Muon::RpcPrepDataContainer* prdContainer = 0;
      SG::ReadHandle<Muon::RpcPrepDataContainer> RpcCont(m_rpcKey);
      if(!RpcCont.isValid()){
	ATH_MSG_WARNING("Cannot retrieve RpcPrepDataContainer ");
	return;
      }
      else{
	prdContainer = RpcCont.cptr();
      }
      if(!prdContainer){
	ATH_MSG_WARNING("No RPC prd container retrieved");
	return;
      }
      
      // loop over chambers and get collections
      std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
      std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
      for( ;chit!=chit_end;++chit ){
	RpcPrepDataContainer::const_iterator colIt = prdContainer->indexFind(*chit);
	if( colIt == prdContainer->end() || !*colIt ) continue;
	
	const RpcPrepDataCollection& col = **colIt;
	RpcPrepDataCollection::const_iterator hit = col.begin();
	RpcPrepDataCollection::const_iterator hit_end = col.end();
	for( ;hit!=hit_end;++hit ){
	  // skip phi hits
	  if( m_idHelperTool->measuresPhi((*hit)->identify()) ) continue;
	  const MuonClusterOnTrack* clus = m_clusterCreator->createRIO_OnTrack(**hit,(*hit)->globalPosition());
	  if( clus ) {
	    clusters.push_back(clus);
	    m_measurementsToBeDeleted.push_back(clus);
	  }
	}
      }
    }
  }


  std::vector<const MuonSegment*>* DCMathSegmentMaker::find( const Amg::Vector3D& roadpos, const Amg::Vector3D& roaddir,
							     const std::vector<const MdtDriftCircleOnTrack*>& mdts,
							     const std::vector<const MuonClusterOnTrack*>&  clusters,
							     bool hasPhiMeasurements, double momentum) const
  {

    if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) { 
      ATH_MSG_DEBUG("Timeout reached. Aborting sequence."); 
      return 0; 
    } 
 		 
    ATH_MSG_DEBUG("In find, passed "<<mdts.size() <<" RIO_OnTracks");

    if( mdts.size() < 3 )return 0;

    const MdtDriftCircleOnTrack* firstRot = findFirstRotInChamberWithMostHits(mdts);

    if( !firstRot ) {
      if( m_nmultipleHitWarnings < 3 ) ATH_MSG_WARNING(" no Mdt selected, returning 0 ");
      return 0;
    }

    const MuonGM::MdtReadoutElement* detEl = firstRot->detectorElement();

    if( !detEl ) {
      ATH_MSG_WARNING(" no MdtReadoutElement found, returning 0 ");
      return 0;
    }
 
    // identifier
    Identifier chid = firstRot->identify();

    // endcap or barrel
    m_isEndcap = m_idHelperTool->mdtIdHelper().isEndcap(chid);

    // global to local transformation for chamber
    m_gToStation = detEl->GlobalToAmdbLRSTransform();

    // define axis of chamber in global coordinates
    m_amdbToGlobal = detEl->AmdbLRSToGlobalTransform();


    // transform nominal pointing chamber position into surface frame
    Amg::Vector3D globalDirCh = Amg::Vector3D(detEl->center().x(),detEl->center().y(),detEl->center().z());
    Amg::Vector3D dirCh(m_gToStation.linear()*globalDirCh);
    double chamber_angleYZ = atan2(dirCh.z(),dirCh.y());

    Amg::Vector3D roaddir2=roaddir;
    double dotprod= globalDirCh.perp()*sin(roaddir2.theta()) +globalDirCh.z()*cos(roaddir2.theta());
    if (dotprod<0) roaddir2=-roaddir2;

    // transform the global direction into the surface frame
    Amg::Vector3D d(m_gToStation.linear()*roaddir2);
    // calculate the local road angles in the surface frame
    double road_angleXZ = atan2(d.z(),d.x());
    double road_angleYZ = atan2(d.z(),d.y());

    if( !hasPhiMeasurements ) road_angleXZ =  acos(0.); // if no phi, take phi perpendicular to plane
    ATH_MSG_VERBOSE("global road dir " << Amg::toString(roaddir2) << " XZ " << road_angleXZ << " YZ " <<  road_angleYZ << " isEndcap " << m_isEndcap 
		    << " central phi " << detEl->center().phi() << " r " << detEl->center().perp() << " z " << detEl->center().z() );

    // rescale errors for low momentum
    double errorScale = errorScaleFactor( chid, momentum, hasPhiMeasurements );


    /* ***** create cluster hits ******** */
    ATH_MSG_DEBUG(" adding clusters " << clusters.size());
    TrkDriftCircleMath::CLVec cls = createClusterVec( clusters, chid );
    
    /* ***** create MDT hits ************ */
    ATH_MSG_DEBUG(" adding mdts " << mdts.size());
    TrkDriftCircleMath::DCVec dcs = createDCVec( mdts, errorScale );
    
    TrkDriftCircleMath::Road road(TrkDriftCircleMath::LocPos(0.,0.),road_angleYZ,chamber_angleYZ,m_sinAngleCut);
 
    // call segment finder
    TrkDriftCircleMath::SegVec segs = m_segmentFinder->findSegments(dcs,cls,road,m_dcStatistics,m_multiGeo);

    ATH_MSG_DEBUG("Found " << segs.size() <<" segments");

    // clean up and return
    if( segs.empty() ) {
      delete m_multiGeo;
      m_multiGeo = 0;
      for(std::vector<const Trk::MeasurementBase*>::iterator it=m_measurementsToBeDeleted.begin(); it!=m_measurementsToBeDeleted.end(); it++)
	delete (*it);
      m_measurementsToBeDeleted.clear();
      return 0;
    }

    /* **** convert found segment into MuonSegments ***** */

    std::vector<const MuonSegment*>* segments = new std::vector<const MuonSegment*>();
    segments->reserve( segs.size() );


    // loop over segments 
    TrkDriftCircleMath::SegIt sit = segs.begin();
    TrkDriftCircleMath::SegIt sit_end = segs.end();
    for( ;sit!=sit_end;++sit ){

      MuonSegment* segment = createSegment(*sit,chid,roadpos,roaddir2,mdts,hasPhiMeasurements);
      if( segment ) segments->push_back( segment );
    }
    ATH_MSG_DEBUG(" Done ");

    delete m_multiGeo;
    m_multiGeo = 0;

    for(std::vector<const Trk::MeasurementBase*>::iterator it=m_measurementsToBeDeleted.begin(); it!=m_measurementsToBeDeleted.end(); it++)
      delete (*it);
    m_measurementsToBeDeleted.clear();

    if( segments->empty() ){
      delete segments;
      return 0;
    }
 
    return segments;
  }

  MuonSegment* DCMathSegmentMaker::createSegment( TrkDriftCircleMath::Segment& segment, const Identifier& chid, 
						  const Amg::Vector3D& roadpos, const Amg::Vector3D& roaddir2,
						  const std::vector<const MdtDriftCircleOnTrack*>& mdts, 
						  bool hasPhiMeasurements ) const {
    
    //find all curved segments
    bool isCurvedSegment(false);
    MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chid);    

    //MuonStationIndex::ChIndex chIndex;
    if(segment.hasCurvatureParameters()) {
      if(chIndex == Muon::MuonStationIndex::BIL || chIndex == MuonStationIndex::BML || chIndex == MuonStationIndex::BMS || chIndex == MuonStationIndex::BOL)
	isCurvedSegment = true;	    
    }

    // remove segments with too few hits
    if( segment.hitsOnTrack() < 3 ) return 0;

    // convert segment parameters + x position from road
    const TrkDriftCircleMath::Line& line = segment.line(); 


    ATH_MSG_DEBUG("New segment: chi2 " << segment.chi2() << " ndof " << segment.ndof() 
		  << " line " << line.position().x() << "," << line.position().y() << " phi " << line.phi()
		  << " associated clusters " << segment.clusters().size());
      
    // local position along x from road
    Amg::Vector3D lroadpos  = m_gToStation*roadpos;
    Amg::Vector3D lroaddir = m_gToStation.linear()*roaddir2;

    // local x position of first tube used if no phi measurement is present
    double lxroad = 0.;
  
    if( hasPhiMeasurements ){

      // calculate local position of segment along tube using the road
      // calculate intersect pattern measurement plane
      double sphi = 0.;
      double cphi = lroaddir.x();
      // swap local y and z in the endcaps
      if ( m_isEndcap ) {
	sphi = lroaddir.y();
	lxroad = lroadpos.x()+(-lroadpos.y()+line.position().x())*cphi/sphi;
      }else{
	sphi = lroaddir.z();
	lxroad = lroadpos.x()+(-lroadpos.z()+line.position().y())*cphi/sphi;
      }
    
      double shortestTubeLen = 1e9;
      // loop over hits and get the shortest tube on the segment
      for( TrkDriftCircleMath::DCOnTrackCit dcit = segment.dcs().begin();dcit!=segment.dcs().end();++dcit ){
      
	if( dcit->state() != TrkDriftCircleMath::DCOnTrack::OnTrack )  continue;
      
	const MdtDriftCircleOnTrack* riodc = mdts[dcit->index()];
	if( !riodc ) continue;
	int lay = m_idHelperTool->mdtIdHelper().tubeLayer(riodc->identify());
	int tube = m_idHelperTool->mdtIdHelper().tube(riodc->identify());
	double tubelen = 0.5*riodc->prepRawData()->detectorElement()->getActiveTubeLength(lay,tube);
	if( tubelen < shortestTubeLen ) shortestTubeLen = tubelen;
      }
      // if the predicted position lies outside the chamber move it back inside
      if ( fabs(lxroad) > shortestTubeLen) {
	ATH_MSG_DEBUG("coordinates far outside chamber! using global position of first hit ");
	if( lxroad < 0. ) shortestTubeLen *= -1.; 
	lxroad = shortestTubeLen;
      }
    }else{
      lxroad = (m_gToStation*mdts[0]->prepRawData()->detectorElement()->surface(mdts[0]->identify()).center()).x();
    }

    // calculate local direction vector
    Amg::Vector3D  lpos( lxroad,line.position().x(),line.position().y() );
  
    // global position segment
    Amg::Vector3D gpos = m_amdbToGlobal*lpos;

    // create new surface 
    Amg::Transform3D* surfaceTransform = new Amg::Transform3D(m_amdbToGlobal.rotation());
    surfaceTransform->pretranslate(gpos);
    double surfDim = 500.;
    Trk::PlaneSurface* surf = new Trk::PlaneSurface( surfaceTransform, surfDim, surfDim );
  
    // measurements
    Amg::Vector2D segLocPos(0.,0.);
    double linephi = line.phi();

    // now update the global direction using the local precision angle of the segment and the global phi angle of the road.
    Amg::Vector3D gdir = updateDirection( linephi, *surf, roaddir2, isCurvedSegment );

    // extract RIO_OnTracks
    m_rioDistVec.clear();

    // associate MDT hits to segment
    associateMDTsToSegment( gdir, segment, mdts );

    if( m_redo2DFit && !isCurvedSegment ){

      // refit segment after recalibration
      bool goodFit = m_defaultFitter.fit( line, segment.dcs(), m_hitSelector.selectHitsOnTrack(segment.dcs()) );
      if( goodFit ){
	if( fabs(segment.line().phi() - m_defaultFitter.result().line().phi()) > 0.01 || 
	    fabs(segment.line().x0() - m_defaultFitter.result().line().x0()) > 0.01 || 
	    fabs(segment.line().y0() - m_defaultFitter.result().line().y0()) > 0.01 ) {
	
	  // update local position and global
	  linephi = m_defaultFitter.result().line().phi();
	  lpos[1] =  m_defaultFitter.result().line().position().x() ;
	  lpos[2] =  m_defaultFitter.result().line().position().y() ;
	  gpos = m_amdbToGlobal*lpos;
	
	  // recreate  surface 
	  delete surf;
	  surfaceTransform = new Amg::Transform3D(m_amdbToGlobal.rotation());
	  surfaceTransform->pretranslate(gpos);
	  surf = new Trk::PlaneSurface( surfaceTransform, surfDim, surfDim );
	
	  // finally update global direction
	  gdir = updateDirection( linephi, *surf, roaddir2, isCurvedSegment );
	}
      }
    }
  
    // create local segment direction
    Trk::LocalDirection segLocDir;
    surf->globalToLocalDirection(gdir,segLocDir);
  
    // sanity checks
    double diff_phi = roaddir2.phi() - gdir.phi();
    double diff_prec =  linephi - segLocDir.angleYZ();
    if (fabs(sin(diff_phi)) > 1.e-3 ||  fabs(sin(diff_prec)) > 1.e-3 ) {
      ATH_MSG_WARNING(" ALARM updated angles wrong: diff phi " << diff_phi << "  prec " << diff_prec << " phi rdir " << roaddir2.phi()
		      << " gdir " << gdir.phi() << " lphi " << linephi << " seg " << segLocDir.angleYZ() );
    }
  
    // associate Clusters to segment, uses m_spVec and m_phiVec to get clusters
    std::pair<int,int> netaPhiHits = associateClustersToSegment( segment, chid );
  
    // copy hits into vector
    DataVector<const Trk::MeasurementBase>* rioVec = createROTVec();

    double dlocx = 1000.;
    double dangleXZ = 1000.;
    double qoverp = -99999.;
    double dqoverp = -99999.;
    bool hasMeasuredCoordinate = false;
    if( m_refitParameters && m_refit ){
      ATH_MSG_DEBUG(" distance between first and last phi hit sufficient to perform 4D fit: phi  " 
		    << gdir.phi() << " theta " << gdir.theta());
    
      Trk::Track* track = m_segmentFitter->fit( gpos, gdir, *surf, rioVec->stdcont() );
    
      if( track ) {
	if( isCurvedSegment && track->perigeeParameters() && track->perigeeParameters()->covariance() ) {
	  qoverp = track->perigeeParameters()->parameters()[Trk::qOverP];
	  dqoverp = Amg::error(*track->perigeeParameters()->covariance(),Trk::qOverP);
	}
	hasMeasuredCoordinate = true;
	// hack to update the second coordinate errors
	Amg::MatrixX updatedCov(5,5);
	updatedCov.setZero();
	m_segmentFitter->updateSegmentParameters( *track, *surf, segLocPos, segLocDir, updatedCov );
	if( Amg::error(updatedCov,Trk::locX) > 0 && Amg::error(updatedCov,Trk::phi) > 0. ){
	  dlocx = Amg::error(updatedCov,Trk::locX);
	  dangleXZ = Amg::error(updatedCov,Trk::phi); // hack (2): phi not always angleXZ
	}else{
	  ATH_MSG_WARNING(" Corrupt error matrix returned from fitter " << Amg::toString(updatedCov));
	}
      
	/// recalculate global direction and position
	surf->localToGlobal(segLocPos,gdir,gpos);
	surf->localToGlobalDirection(segLocDir,gdir);
      
	if( track->measurementsOnTrack() && rioVec->size() != track->measurementsOnTrack()->size() ){
	  ATH_MSG_DEBUG(" ROT vector size changed after fit, updating ");
	  delete rioVec;
	  m_rioDistVec.clear();
	  rioVec = new DataVector<const Trk::MeasurementBase>();
	  rioVec->reserve(track->measurementsOnTrack()->size());
	  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = track->trackStateOnSurfaces()->begin();
	  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = track->trackStateOnSurfaces()->end();
	  const Trk::TrackParameters* firstPars = 0;
	  for( ;tsit!=tsit_end;++tsit ) {
	  
	    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
	    if( !pars ) continue;
	    if( !firstPars ) firstPars = pars;
	  
	    // check whether state is a measurement, skip outliers if they are not MDT
	    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
	    if( !meas ) continue;
	    if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) && !dynamic_cast<const MdtDriftCircleOnTrack*>(meas) ) continue;
	  
	    const Trk::MeasurementBase* measNew = meas->clone();
	    rioVec->push_back( measNew );    
	    double dist = (pars->position()-firstPars->position()).dot(firstPars->momentum().unit());
	    m_rioDistVec.push_back( std::make_pair(dist,measNew) );
	  }
	}

	delete track;
      }else {
	ATH_MSG_DEBUG(" refit of segment failed!! ");
	m_refit = false;
      }
    }

    // optional update of phi position and direction, only performed if the segment was not refitted and there are phi hits
    if( m_updatePhiUsingPhiHits && !m_refit ){
      if( updateSegmentPhi( gpos, gdir, segLocPos, segLocDir, *surf, rioVec->stdcont() ) ){
	surf->localToGlobal(segLocPos,gpos,gpos);
	surf->localToGlobalDirection(segLocDir,gdir);
	hasMeasuredCoordinate = true;
	dlocx = 100.;
	dangleXZ = 0.1;
      }
    }


    if( msgLvl(MSG::DEBUG) ){
      ATH_MSG_DEBUG(" number of hits " << rioVec->size() << " of which trigger " << netaPhiHits.first 
		    << " eta and " << netaPhiHits.second << " phi ");
      DataVector<const Trk::MeasurementBase>::const_iterator mit = rioVec->begin();
      DataVector<const Trk::MeasurementBase>::const_iterator mit_end = rioVec->end();
      for( ;mit!=mit_end;++mit ){
	const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*mit);
	if( rot ){
	  msg(MSG::DEBUG) << m_idHelperTool->toString( rot->identify() );
	  const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(rot);
	  if( mdt ) msg() << std::setprecision(4) << " radius " << std::setw(6) << mdt->driftRadius() << " time " << std::setw(6) << mdt->driftTime();
	  msg(MSG::DEBUG) << endmsg;
	  continue;
	}
	const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*mit);
	if( crot ){
	  ATH_MSG_DEBUG(m_idHelperTool->toString( crot->rioOnTrack(0).identify() ) 
			<< " comp rot with hits " << crot->containedROTs().size());
	  continue;
	}
	ATH_MSG_WARNING("failed to dynamic_cast to ROT ");
	  
      }
    }      
    // recalculate holes
      
    calculateHoles( chid, gpos, gdir, hasMeasuredCoordinate );

    // currently not taking into account masked channels
    m_holeVec.insert(m_holeVec.end(),m_outoftimeVec.begin(),m_outoftimeVec.end());
    MuonSegmentQuality* quality = new MuonSegmentQuality( segment.chi2(), segment.ndof(), m_holeVec );

    if( m_dcslFitter && !segment.hasT0Shift() && m_outputFittedT0 ){
      if( !m_dcslFitter->fit( segment.line(), segment.dcs(), m_hitSelector.selectHitsOnTrack( segment.dcs() ) ) ) {
	ATH_MSG_DEBUG( " T0 refit failed ");
      }else{
	if( msgLvl(MSG::DEBUG) ) {
	  if( m_dcslFitter->result().hasT0Shift() ) ATH_MSG_DEBUG(" Fitted T0 " << m_dcslFitter->result().t0Shift());
	  else                                      ATH_MSG_DEBUG(" No fitted T0 ");
	}
      }
    }
    bool hasFittedT0 = false;
    double fittedT0  = 0;
    double errorFittedT0  = 1.;
    if( m_outputFittedT0 && ( segment.hasT0Shift() || ( m_dcslFitter && m_dcslFitter->result().hasT0Shift() ) ) ){
      hasFittedT0 = true;
      if( segment.hasT0Shift() ){
	fittedT0 = segment.t0Shift();
	errorFittedT0 = segment.t0Error();
      }else if( m_dcslFitter && m_dcslFitter->result().hasT0Shift() ) {
	fittedT0 = m_dcslFitter->result().t0Shift();
	errorFittedT0 = m_dcslFitter->result().t0Error();
      }else{
	ATH_MSG_WARNING(" Failed to access fitted t0 ");
	hasFittedT0 = false;
      }
    }
    // create new segment 
    MuonSegment* msegment = 0;
    if(isCurvedSegment) {//curved segments
      if(qoverp == -99999.) {
	double charge = gpos.z()*tan(gdir.theta());
	charge = charge/fabs(charge);
	//if the curved segment was not refit, then use a momentum estimate
	double BILALPHA(28.4366),BMLALPHA(62.8267),BMSALPHA(53.1259),BOLALPHA(29.7554);
	if(chIndex == MuonStationIndex::BIL) {
	  qoverp = (charge*segment.deltaAlpha())/BILALPHA;
	  dqoverp = sqrt(2)*segment.dtheta()/BILALPHA;
	}	  
	else if(chIndex == MuonStationIndex::BML) {
	  qoverp = (charge*segment.deltaAlpha())/BMLALPHA;
	  dqoverp = sqrt(2)*segment.dtheta()/BMLALPHA;
	}
	else if(chIndex == MuonStationIndex::BMS) {
	  qoverp = (charge*segment.deltaAlpha())/BMSALPHA;
	  dqoverp = sqrt(2)*segment.dtheta()/BMSALPHA;
	}
	else if(chIndex == MuonStationIndex::BOL) {
	  qoverp = (charge*segment.deltaAlpha())/BOLALPHA;
	  dqoverp = sqrt(2)*segment.dtheta()/BOLALPHA;
	}
      }
      Amg::MatrixX covMatrix(5,5);
      covMatrix.setIdentity();
      covMatrix(0,0) = dlocx*dlocx;
      covMatrix(1,1) = segment.dy0()*segment.dy0();
      covMatrix(2,2) = dangleXZ*dangleXZ;
      covMatrix(3,3) = segment.dtheta()*segment.dtheta();	
      covMatrix(4,4) = dqoverp*dqoverp;

      std::vector<Trk::DefinedParameter> defPars;
      defPars.push_back( Trk::DefinedParameter(segLocPos[Trk::loc1],Trk::loc1) );
      defPars.push_back( Trk::DefinedParameter(segLocPos[Trk::loc2],Trk::loc2) );
      defPars.push_back( Trk::DefinedParameter(gdir.phi(), Trk::phi) );
      defPars.push_back( Trk::DefinedParameter(gdir.theta(), Trk::theta) );
      defPars.push_back( Trk::DefinedParameter(qoverp,Trk::qOverP) );
      Trk::LocalParameters segLocPar( defPars );
      msegment = new MuonSegment( segLocPar, covMatrix, surf, rioVec, quality, Trk::Segment::DCMathSegmentMakerCurved );
    }
    else {//straight segments
      // errors (for now no correlations)
      Amg::MatrixX covMatrix(4,4);
      covMatrix.setIdentity();
      covMatrix(0,0) = dlocx*dlocx;
      covMatrix(1,1) = segment.dy0()*segment.dy0();
      covMatrix(2,2) = dangleXZ*dangleXZ;
      covMatrix(3,3) = segment.dtheta()*segment.dtheta();	
      msegment = new MuonSegment( segLocPos, segLocDir, covMatrix, surf, rioVec, quality, Trk::Segment::DCMathSegmentMaker );
    }

    if( hasFittedT0 ) msegment->setT0Error(fittedT0,errorFittedT0);

    // check whether segment satisfies minimum quality criteria
    int segmentQuality = m_segmentSelectionTool->quality(*msegment);
      
    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << m_printer->print(*msegment) << " quality " << segmentQuality;
      if( segmentQuality < 0 ) msg(MSG::DEBUG) << " BAD segment ";
      if( hasFittedT0 )        msg(MSG::DEBUG) << " T0 " << fittedT0;
      if( isCurvedSegment )    msg(MSG::DEBUG) << " Curved " << fittedT0;
      msg(MSG::DEBUG) << endmsg;
    }
    if( segmentQuality < 0 ){
      delete msegment;
      msegment = 0;
    }
    return msegment;
  }


  std::vector<const MuonSegment*>* DCMathSegmentMaker::find( const std::vector<const Trk::RIO_OnTrack*>& rios) const
  {
    std::vector<const MdtDriftCircleOnTrack*> mdts;
    std::vector<const MuonClusterOnTrack*>    clusters;

    std::vector<const Trk::RIO_OnTrack*>::const_iterator it = rios.begin();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator it_end = rios.end();
    for( ; it!=it_end;++it ){

      Identifier id = (*it)->identify();
      if( m_idHelperTool->mdtIdHelper().is_mdt(id) ){
	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*it);
	if( !mdt ){
	  ATH_MSG_WARNING("failed dynamic_cast, not a MDT but hit has MDT id!!!");
	}
	mdts.push_back(mdt);
      }else if( m_idHelperTool->isTrigger(id) ){
	const MuonClusterOnTrack* clus = dynamic_cast<const MuonClusterOnTrack*>(*it);
	if( !clus ){
	  ATH_MSG_WARNING("failed dynamic_cast, not a cluster but hit has RPC/TGC id!!!");
	}
	clusters.push_back(clus);
      }
    }
    return find(mdts,clusters);
  }

  std::vector<const MuonSegment*>* DCMathSegmentMaker::find( const std::vector<const MdtDriftCircleOnTrack*>& mdts,
							     const std::vector<const MuonClusterOnTrack*>&  clusters) const
  {
    if( mdts.empty() ) return 0;

    const MdtDriftCircleOnTrack* mdt = mdts.front();
    if( !mdt ) return 0;

    bool hasPhiMeasurements = false;
    Amg::Vector3D  gpos = mdt->globalPosition();
    Amg::Vector3D gdir =  Amg::Vector3D(gpos.x(),gpos.y(),gpos.z()).unit();
    return find( gpos, 
		 gdir, 
		 mdts, 
		 clusters, 
		 hasPhiMeasurements );    
  }


  std::vector<const MuonSegment*>* DCMathSegmentMaker::find( const std::vector<const Trk::RIO_OnTrack*>& rios1,
							     const std::vector<const Trk::RIO_OnTrack*>& rios2 ) const
  {
    std::vector<const Trk::RIO_OnTrack*> rios = rios1;
    rios.insert( rios.end(), rios2.begin(), rios2.end() );
    return find(rios); 
  }

  std::vector<const MuonSegment*>* DCMathSegmentMaker::find( const Trk::TrackRoad& road,
							     const std::vector< std::vector< const MdtDriftCircleOnTrack* > >& mdts,
							     const std::vector< std::vector< const MuonClusterOnTrack* > >&  clusters, 
							     bool hasPhiMeasurements, double momentum ) const
  {
    // copy all mdt hits into one vector
    std::vector< const MdtDriftCircleOnTrack* > all_mdts;
    std::vector< std::vector< const MdtDriftCircleOnTrack* > >::const_iterator mit = mdts.begin();
    std::vector< std::vector< const MdtDriftCircleOnTrack* > >::const_iterator mit_end = mdts.end();
    for(; mit!=mit_end;++mit ){
      std::copy( mit->begin(), mit->end(), std::back_inserter(all_mdts) );
    }

    // copy all clusters into one vector
    std::vector< const MuonClusterOnTrack* > all_clus;
    std::vector< std::vector< const MuonClusterOnTrack* > >::const_iterator cit = clusters.begin();
    std::vector< std::vector< const MuonClusterOnTrack* > >::const_iterator cit_end = clusters.end();
    for(; cit!=cit_end;++cit ){
      std::copy( cit->begin(), cit->end(), std::back_inserter(all_clus) );
    }
    
    // set angle cut from road, keep old value
    double sinAngleCut = m_sinAngleCut;
	// FIXME - probably need to change interface to pass in sinAngleCut. EJWM.
    m_sinAngleCut = road.deltaEta();
    
    const Amg::Vector3D& gpos = road.globalPosition();
    const Amg::Vector3D& gdir = road.globalDirection();
    std::vector<const MuonSegment*>* segments = find( gpos, 
						      gdir, 
						      all_mdts, 
						      all_clus, 
						      hasPhiMeasurements,
						      momentum );
    m_sinAngleCut = sinAngleCut;
    return segments;
  }       


  double DCMathSegmentMaker::errorScaleFactor( const Identifier& id, double curvature, bool hasPhiMeasurements ) const 
  {
    double scale = 1.;
    if( !m_curvedErrorScaling ) return scale;

    if( !errorScalingRegion( id ) ) return scale;

    double scaleMax = 5.;
    if( m_curvedErrorScaling && curvature > 2 ){
      scale = 1. + curvature/10000; // 3*(curvature/30000)
      if( scale > scaleMax ) scale = scaleMax; 
      ATH_MSG_DEBUG(" rescaled errors " << scale << " curvature " << curvature);

    }
    scale *= 2;

    // rescale errors is no phi measurement was found 
    if( !hasPhiMeasurements ){

      double phiScale = 1.;
      // rescale errors 
      int stRegion = m_idHelperTool->mdtIdHelper().stationRegion(id);
      if( stRegion == 0 ) phiScale = 2.;        // inner
      else if( stRegion == 1 ) phiScale = 2.5;  // extended
      else if( stRegion == 2 ) phiScale = 2.5;  // middle
      else phiScale = 3.;                       // outer
      scale = sqrt( scale*scale + phiScale*phiScale );
      ATH_MSG_DEBUG(" rescaled error for missing phi road " << scale);
    }

    return scale;
  }


  bool DCMathSegmentMaker::errorScalingRegion( const Identifier& id ) const

  {

    // simple division of MuonSpectrometer in regions using barrel/endcap seperation plus
    // inner/middle/outer seperation


    if( m_isEndcap ){

      std::string stName = m_idHelperTool->mdtIdHelper().stationNameString(m_idHelperTool->mdtIdHelper().stationName(id) );
      if( stName[1] == 'I' ) return true;

    }else{

      return true;

    }
    return false;
  }


  DCMathSegmentMaker::ClusterVec DCMathSegmentMaker::create1DClusters( const std::vector<const MuonClusterOnTrack*>& clusters ) const
  {
    // if empty return
    if( clusters.empty() ) return ClusterVec();
    // some useful typedefs...
    typedef std::vector<const MuonClusterOnTrack*>::const_iterator ROTCit;
 
    // create a vector to hold the clusters
    ClusterVec clVec;
    clVec.reserve(clusters.size());

    ROTCit cit = clusters.begin();
    ROTCit cit_end = clusters.end();
    for( ;cit!=cit_end;++cit ){
      Identifier id = (*cit)->identify();
      Identifier gasGapId = m_idHelperTool->gasGapId(id);
      int measuresPhi = m_idHelperTool->measuresPhi(id);
      
      if( measuresPhi ) {
	m_phiVec.push_back( createSpacePoint( gasGapId, 0, *cit ) );
	if( m_phiVec.back().corrupt() ) m_phiVec.pop_back();
      }else{
	clVec.push_back(  createSpacePoint( gasGapId, *cit, 0 ) );
	if( clVec.back().corrupt() ) clVec.pop_back();
      }
    }

    return clVec;
  }


  DCMathSegmentMaker::ClusterVec DCMathSegmentMaker::create2DClusters( const std::vector<const MuonClusterOnTrack*>& clusters ) const
  {
    // if empty return
    if( clusters.empty() ) return ClusterVec();
    
    ChIdHitMap gasGapHitMap;

    ROTCit cit = clusters.begin();
    ROTCit cit_end = clusters.end();
    for( ;cit!=cit_end;++cit ){
      Identifier id = (*cit)->identify();
      ATH_MSG_VERBOSE(" new trigger hit " << m_idHelperTool->toString( id ));

      Identifier chId = m_idHelperTool->chamberId(id);
      Identifier gasGapId = m_idHelperTool->gasGapId(id);
      int measuresPhi = m_idHelperTool->measuresPhi(id);
      
      // eta hits first than phi hits
      if( !measuresPhi ) gasGapHitMap[chId][gasGapId].first.push_back(*cit);
      else               gasGapHitMap[chId][gasGapId].second.push_back(*cit);
    }
    
    return createSpacePoints( gasGapHitMap );
  }

  void DCMathSegmentMaker::handleChamber( DCMathSegmentMaker::IdHitMap& gasGapHitMap ) const {
    
    if( m_debugSpacePoints ) std::cout << "  new chamber " << m_idHelperTool->toString( gasGapHitMap.begin()->first ) << std::endl;
    std::list<const Trk::PrepRawData*> prds;
    
    IdHitMapIt git = gasGapHitMap.begin();
    IdHitMapIt git_end = gasGapHitMap.end();
    for( ;git!=git_end;++git ){
      ROTCit hit = git->second.first.begin();
      ROTCit hit_end = git->second.first.end();
      for( ;hit!=hit_end;++hit ){
	double lpos = (*hit)->localParameters()[Trk::locX];
	if( m_debugSpacePoints ) std::cout << "     new eta hit  " << lpos << "  " << m_idHelperTool->toString( (*hit)->identify() ) << std::endl;
	prds.push_back( (*hit)->prepRawData() );
      }
    }    
    const CompetingMuonClustersOnTrack* rotEta = 0; 
    if( prds.size() > 1 ) {
      rotEta = m_compClusterCreator->createBroadCluster( prds, 0. );
      if( m_debugSpacePoints ) std::cout << "     --> new competing eta rot  " << rotEta->localParameters()[Trk::locX] << " " 
					 << Amg::error(rotEta->localCovariance(),Trk::locX) << std::endl;
    }
    prds.clear();
    git = gasGapHitMap.begin();
    git_end = gasGapHitMap.end();
    for( ;git!=git_end;++git ){
      ROTCit hit = git->second.second.begin();
      ROTCit hit_end = git->second.second.end();
      for( ;hit!=hit_end;++hit ){
	double lpos = (*hit)->localParameters()[Trk::locX];
	if( m_debugSpacePoints ) std::cout << "     new phi hit  " << lpos << "  " << m_idHelperTool->toString( (*hit)->identify() ) << std::endl;
	prds.push_back( (*hit)->prepRawData() );
      }
    }
    const CompetingMuonClustersOnTrack* rotPhi = 0;
    if( prds.size() > 1 ) {
      rotPhi = m_compClusterCreator->createBroadCluster( prds, 0. );
      if( m_debugSpacePoints ) std::cout << "     --> new competing phi rot  " << rotPhi->localParameters()[Trk::locX] 
					 << " " << Amg::error(rotPhi->localCovariance(),Trk::locX) << std::endl;
    }
  }

  DCMathSegmentMaker::ClusterVec DCMathSegmentMaker::createSpacePoints( DCMathSegmentMaker::ChIdHitMap& chIdHitMap ) const {
    
    // vector to store output
    ClusterVec clusters;
    clusters.reserve(20);
    
    // loop over chambers
    ChIdHitMapIt cit = chIdHitMap.begin();
    ChIdHitMapIt cit_end = chIdHitMap.end();
    for( ;cit!=cit_end;++cit ){

      // create clusters per chamber and copy them in to result vector
      ClusterVec cls = createSpacePoints( cit->second );
      std::copy( cls.begin(), cls.end(), std::back_inserter(clusters) );

      //handleChamber( cit->second );
    }

    return clusters;
  }

  DCMathSegmentMaker::ClusterVec DCMathSegmentMaker::createSpacePoints( DCMathSegmentMaker::IdHitMap& gasGapHitMap ) const {

    ClusterVec spacePoints;
    
    ATH_MSG_VERBOSE(" creating Space points for " << gasGapHitMap.size() << " gas gaps "); 

    IdHitMapIt git = gasGapHitMap.begin();
    IdHitMapIt git_end = gasGapHitMap.end();
    for( ;git!=git_end;++git ){

      // flag whether phi hits are matched with a eta hit
      std::vector<int> flagPhihit(git->second.second.size(),0);

      // store Identifier of previous hit to remove duplicates
      Identifier prevEtaId;
      
      if( m_debugSpacePoints ) ATH_MSG_VERBOSE(" New gasgap " << m_idHelperTool->toString(git->first) 
					       << " neta " << git->second.first.size()
					       << " nphi " << git->second.second.size());

      ROTCit etaIt = git->second.first.begin();
      ROTCit etaIt_end = git->second.first.end();
      for( ;etaIt!=etaIt_end;++etaIt ){
	
	// check whether we are not dealing with a duplicate hit
	if( (*etaIt)->identify() == prevEtaId ) continue;
	prevEtaId = (*etaIt)->identify();
	
	if( m_debugSpacePoints ) ATH_MSG_VERBOSE(" Eta hit " << m_idHelperTool->toString((*etaIt)->identify()));

	if( m_isEndcap ) {
	  
	  // check whether match with phi hits was found
	  bool foundSP = false;
	  Identifier prevPhiId;
	  ROTCit phiIt = git->second.second.begin();
	  ROTCit phiIt_begin = git->second.second.begin();
	  ROTCit phiIt_end = git->second.second.end();
	  for( ;phiIt!=phiIt_end;++phiIt ){

	    // check for duplicate phi hits
	    if( (*phiIt)->identify() == prevPhiId ) continue;
	    prevPhiId = (*phiIt)->identify();
	    
	    if( m_debugSpacePoints ) ATH_MSG_VERBOSE(" Phi hit " << m_idHelperTool->toString((*phiIt)->identify()));
	    
	    Cluster2D sp = createTgcSpacePoint( git->first, *etaIt, *phiIt ); 
	    if( sp.corrupt() ) continue;
	    spacePoints.push_back( sp );
	    
	    // mark as used
	    foundSP = true;
	    flagPhihit[ std::distance( phiIt_begin, phiIt ) ] = 1;
	    
	  }
	  
	  // add single eta hit if not matching phi hit was found
	  if( !foundSP ){
	    Cluster2D sp = createSpacePoint( git->first, *etaIt, 0 ); 
	    if( sp.corrupt() ) continue;
	    spacePoints.push_back( sp );
	  }
	}else{
	  Cluster2D sp = createRpcSpacePoint( git->first, *etaIt, git->second.second );
	  if( sp.corrupt() ) continue;
	  // flag all phi hits, not very elegant, but works
	  flagPhihit = std::vector<int>(git->second.second.size(),1);
	  spacePoints.push_back( sp );

	}
      }
      if( m_isEndcap ){
	// loop over flag vector and add unmatched phi hits to m_phiVec;
	Identifier prevPhiId;
	for( unsigned int i=0;i<flagPhihit.size();++i ){
	  if( flagPhihit[i] ) continue;
	  
	  // check for duplicate phi hits
	  if( git->second.second[i]->identify() == prevPhiId ) continue;
	  prevPhiId = git->second.second[i]->identify();
	  
	  Cluster2D sp = createTgcSpacePoint( git->first, 0, git->second.second[i] ); 
	  if( sp.corrupt() ) continue;
	  m_phiVec.push_back( sp );
	  
	}
      }else if( git->second.first.empty() && !git->second.second.empty() ) {
	// if there were no eta hits create one phi spacePoint of all phi hits in gasgap
	Cluster2D sp = createRpcSpacePoint( git->first, 0, git->second.second );
	if( sp.corrupt() ) continue;
	m_phiVec.push_back( sp );
      }
      
    }

    ATH_MSG_VERBOSE(" Creating space points, number of gas-gaps " << gasGapHitMap.size() 
		    << "  space points " << spacePoints.size());
    
    return spacePoints;
  }

  DCMathSegmentMaker::Cluster2D DCMathSegmentMaker::createSpacePoint( const Identifier& gasGapId, const MuonClusterOnTrack* etaHit, 
								      const MuonClusterOnTrack* phiHit ) const
  {
    double error(1.);
    double lpx(0.);
    double lpy(0.);
    // case one hit missing. Take position and error of the available hit
    if( !etaHit ){
      lpx = phiHit->localParameters()[Trk::locX];
      error = Amg::error(phiHit->localCovariance(),Trk::locX);
    }else if( !phiHit ) {
      lpx = etaHit->localParameters()[Trk::locX];
      error = Amg::error(etaHit->localCovariance(),Trk::locX);
    }else if( etaHit && phiHit ) {
      if( m_isEndcap ){
	return createTgcSpacePoint( gasGapId,etaHit,phiHit );
      }else{
	std::vector<const MuonClusterOnTrack*> phiVec;
	phiVec.push_back(phiHit);
	return createRpcSpacePoint( gasGapId,etaHit,phiVec );
      }
    }
    Identifier detElId = m_idHelperTool->detElId(gasGapId);
    if( fabs(error) < 0.001 ){
      ATH_MSG_WARNING(" Unphysical error assigned for gasgap " << m_idHelperTool->toString(gasGapId));
      error = 0.;
    }
    return Cluster2D(detElId,gasGapId,Amg::Vector2D(lpx,lpy),error,etaHit,phiHit);
  }


  DCMathSegmentMaker::Cluster2D DCMathSegmentMaker::createTgcSpacePoint( const Identifier& gasGapId, const MuonClusterOnTrack* etaHit, 
									 const MuonClusterOnTrack* phiHit ) const
  {
    double error(1.);
    double lpx(0.);
    double lpy(0.);
    Identifier detElId = m_idHelperTool->detElId(gasGapId);
    // case one hit missing. Take position and error of the available hit
    if( !etaHit ){
      lpx = phiHit->localParameters()[Trk::locX];
      error = Amg::error(phiHit->localCovariance(),Trk::locX);
    }else if( !phiHit ) {
      lpx = etaHit->localParameters()[Trk::locX];
      error = Amg::error(etaHit->localCovariance(),Trk::locX);
    }else if( etaHit && phiHit ){
      
	
      // get orientation angle of strip to rotate back from local frame to strip
      // copy code from ROT creator
      int stripNo = m_idHelperTool->tgcIdHelper().channel(phiHit->identify());
      int gasGap = m_idHelperTool->tgcIdHelper().gasGap(phiHit->identify());

      const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(etaHit->detectorElement());
      if (!detEl) {
        ATH_MSG_WARNING("dynamic cast error returning");
        return Cluster2D(detElId,gasGapId,Amg::Vector2D(lpx,lpy),error,etaHit,phiHit);
      }	
      // calculate local position of endpoint of strip
      Amg::Vector3D lEtapos = detEl->localChannelPos(etaHit->identify());
      double localEtaY = detEl->stripCtrX(gasGap, stripNo, lEtapos.z() );
      if (0 < detEl->getStationEta()) {
	localEtaY *= -1.;
      }
      Amg::Vector3D lSppos = lEtapos;
      lSppos[1] = localEtaY;

      // transform to global
      const Amg::Transform3D tgcTrans = detEl->absTransform();
      Amg::Vector3D gposSp    = tgcTrans*lSppos;
      lpx = etaHit->localParameters()[Trk::locX];
      error = Amg::error(etaHit->localCovariance(),Trk::locX);
      if( error == 0. ){
	msg(MSG::WARNING) << " Unphysical error assigned for " << m_idHelperTool->toString(etaHit->identify());
	if( etaHit->prepRawData() ) msg(MSG::WARNING) << " PRD error " << Amg::error(etaHit->prepRawData()->localCovariance(),Trk::locX); 
	msg(MSG::WARNING) << endmsg;
      }
      Amg::Vector2D lspPos(0.,0.);
      if( etaHit->associatedSurface().globalToLocal( gposSp, gposSp,lspPos ) ){
	lpy = lspPos[Trk::locY];
      }else{
	ATH_MSG_WARNING(" globalToLocal failed ");
      }
	
      if( m_debugSpacePoints ) std::cout << " TGC space point: error " << error << " stripWith " << error*sqrt(12) << std::endl 
					 << "   " << m_idHelperTool->toString( etaHit->identify() ) << std::endl
					 << "   " << m_idHelperTool->toString( phiHit->identify() ) << std::endl;

    }
    if( fabs(error) < 0.001 ){
      ATH_MSG_WARNING(" Unphysical error assigned for gasgap " << m_idHelperTool->toString(gasGapId));
      error = 1.;
    }
    return Cluster2D(detElId,gasGapId,Amg::Vector2D(lpx,lpy),error,etaHit,phiHit);
  }

  
  DCMathSegmentMaker::Cluster2D DCMathSegmentMaker::createRpcSpacePoint( const Identifier& gasGapId, const MuonClusterOnTrack* etaHit, 
									 const std::vector<const MuonClusterOnTrack*>& phiHits ) const
  {
    // create vector to store phi hits after removal of duplicate hits
    std::vector<const MuonClusterOnTrack*> cleanPhihits;
    cleanPhihits.reserve(phiHits.size());

    double error(1.);
    double lpx(0.);
    double lpy(0.);
    // case one hit missing. Take position and error of the available hit
    if( !etaHit ){
      lpx = phiHits.front()->localParameters()[Trk::locX];
      error = Amg::error(phiHits.front()->localCovariance(),Trk::locX);
      // loop over phi hits, remove duplicate phi hits
      Identifier prevId;
      std::vector<const MuonClusterOnTrack*>::const_iterator pit = phiHits.begin();
      std::vector<const MuonClusterOnTrack*>::const_iterator pit_end = phiHits.end();
      for( ;pit!=pit_end;++pit ){
	
	// remove duplicate phi hits
	if( (*pit)->identify() == prevId ) continue;
	prevId = (*pit)->identify();
	
	cleanPhihits.push_back(*pit);
      }
    }else if( phiHits.empty() ) {
      lpx = etaHit->localParameters()[Trk::locX];
      error = Amg::error(etaHit->localCovariance(),Trk::locX);
    }else if( etaHit && !phiHits.empty() ) {
      lpx = etaHit->localParameters()[Trk::locX];
      error = Amg::error(etaHit->localCovariance(),Trk::locX);
      if( m_debugSpacePoints ) std::cout << " RPC space point: error " << error << " stripWith " << error*sqrt(12) << std::endl
					 << "   " << m_idHelperTool->toString( etaHit->identify() ) << std::endl;
      
      double minPos = 1e9;
      double maxPos = -1e9;
      Identifier prevId;

      // loop over phi hits, calculate average position + cluster width, remove duplicate phi hits
      std::vector<const MuonClusterOnTrack*>::const_iterator pit = phiHits.begin();
      std::vector<const MuonClusterOnTrack*>::const_iterator pit_end = phiHits.end();
      for( ;pit!=pit_end;++pit ){
	
	// remove duplicate phi hits
	if( (*pit)->identify() == prevId ) continue;
	prevId = (*pit)->identify();

	// calculate phi hit position in local eta hit reference frame
	Amg::Vector2D phiLocPos(0.,0.);
	if( etaHit->associatedSurface().globalToLocal( (*pit)->globalPosition(),(*pit)->globalPosition(),phiLocPos ) ){
	  lpy = phiLocPos[Trk::locY];
	  if( lpy < minPos ) minPos = lpy;
	  if( lpy > maxPos ) maxPos = lpy;
	  if( m_debugSpacePoints ) std::cout << "    " << m_idHelperTool->toString( (*pit)->identify() ) << std::endl;
	  cleanPhihits.push_back(*pit);
	}
      }
      if( m_debugSpacePoints && cleanPhihits.size() > 1 ) std::cout << "  multiple phi hits: nhits " << cleanPhihits.size() 
								    << " cl width " << maxPos - minPos << std::endl;
    }else{
      ATH_MSG_DEBUG(" ARRRGGG got two empty pointers!!! ");
    }
    Identifier detElId = m_idHelperTool->detElId(gasGapId);
    if( fabs(error) < 0.001 ){
      ATH_MSG_WARNING(" Unphysical error assigned for gasgap " << m_idHelperTool->toString(gasGapId));
      error = 1.;
    }
    return Cluster2D(detElId,gasGapId,Amg::Vector2D(lpx,lpy),error,etaHit, !cleanPhihits.empty() ? cleanPhihits : phiHits );
  }
    

  TrkDriftCircleMath::CLVec DCMathSegmentMaker::createClusterVec( const std::vector<const MuonClusterOnTrack*>& clusters, const Identifier& chid ) const {
    
    TrkDriftCircleMath::CLVec cls;
    m_spVec.clear();
    m_phiVec.clear();
    if( m_doSpacePoints ) m_spVec = create2DClusters( clusters );
    else                  m_spVec = create1DClusters( clusters );

    int chPhi     = m_idHelperTool->mdtIdHelper().stationPhi(chid);

    // loop over clusters
    ClusterIt cit=m_spVec.begin();
    ClusterIt cit_end=m_spVec.end();
    int index = 0;
    cls.reserve(m_spVec.size());
    for( ; cit!=cit_end;++cit ){

      const MuonClusterOnTrack* meas = cit->etaHit ? cit->etaHit : cit->phiHit;
      // construct cluster id
      Identifier id = meas->identify();
      int measuresPhi = m_idHelperTool->measuresPhi(id);
      int eta = m_idHelperTool->stationEta(id);
      int phi = m_idHelperTool->stationPhi(id);;
      int isTgc = m_idHelperTool->isTgc(id);
      int name = isTgc ? m_idHelperTool->tgcIdHelper().stationName(id) : m_idHelperTool->rpcIdHelper().stationName(id);
      if( !isTgc ){
	if( chPhi != phi ){
	  ATH_MSG_VERBOSE(" Discarding cluster, wrong station phi " << m_idHelperTool->toString(id));
	  ++index;
	  continue;
	}
      }
      TrkDriftCircleMath::ClusterId clid( name,eta,phi,isTgc,measuresPhi );

      // calculate local cluster position
      Amg::Vector3D locPos = m_gToStation*cit->globalPos;
      TrkDriftCircleMath::LocPos lp(locPos.y(),locPos.z());
      
      if( fabs(lp.y()) > m_maxAssociateClusterDistance ) {
	ATH_MSG_VERBOSE(" Discarding cluster with large distance from chamber " << m_idHelperTool->toString(id));
	++index;
	continue;
      }
      ATH_MSG_VERBOSE(" " << m_idHelperTool->toString(id) 
		      << "  clid: " << clid.id() << " central phi " << meas->detectorElement()->center().phi() << " index " << index);
      double err = cit->error;
      TrkDriftCircleMath::Cluster cl( lp, err, clid, index );
      cls.push_back( cl);
      
      ++index;
    }
    return cls;
  }


  
  TrkDriftCircleMath::DCVec DCMathSegmentMaker::createDCVec( const std::vector<const MdtDriftCircleOnTrack*>& mdts, double errorScale ) const {
    
    TrkDriftCircleMath::DCVec dcs;
    /* ********  Mdt hits  ******** */

    //     typedef std::map< Identifier, TrkDriftCircleMath::MdtChamberGeometry > GeoMap;
    //     typedef GeoMap::iterator GeoIt;
    //     GeoMap geoMap;

    // reset chamber statistics
    m_dcStatistics.clear();
    bool firstMdt = true;;
    m_phimin = 9999.;
    m_phimax = -9999.;

    // set to get Identifiers of chambers with hits
    std::set<Identifier> chamberSet;
    
 
    unsigned index = 0;
    for( std::vector<const MdtDriftCircleOnTrack*>::const_iterator it=mdts.begin();it!=mdts.end();++it){

      const MdtDriftCircleOnTrack* rot = *it;

      if( !rot ){
	ATH_MSG_WARNING(" rot not a MdtDriftCircleOnTrack ");
	++index;
	continue;
      }
      const MuonGM::MdtReadoutElement* detEl = rot->prepRawData()->detectorElement();

      if( !detEl ){
	ATH_MSG_WARNING(" aborting not detEl found ");
	return TrkDriftCircleMath::DCVec();
      }

      Identifier id = (*it)->identify();
      Identifier elId = m_idHelperTool->mdtIdHelper().elementID( id );


      // calculate local AMDB position
      Amg::Vector3D locPos = m_gToStation*rot->prepRawData()->globalPosition();
      TrkDriftCircleMath::LocPos lpos(locPos.y(),locPos.z());

      double r  = rot->localParameters()[Trk::locR];
      double dr = Amg::error(rot->localCovariance(),Trk::locR)*errorScale;

      // create identifier
      TrkDriftCircleMath::MdtId mdtid( m_idHelperTool->mdtIdHelper().isBarrel(id),m_idHelperTool->mdtIdHelper().multilayer(id)-1, 
				       m_idHelperTool->mdtIdHelper().tubeLayer(id)-1, m_idHelperTool->mdtIdHelper().tube(id)-1 );

      // 
      double preciseError = dr;
      if( m_usePreciseError ) {
	preciseError = m_preciseErrorScale*(0.23*exp(-fabs(r)/6.06)+0.0362);
      }
      // create new DriftCircle
      TrkDriftCircleMath::DriftCircle dc( lpos, r, dr, preciseError, TrkDriftCircleMath::DriftCircle::InTime, mdtid, index, rot );


      
      TubeEnds tubeEnds = localTubeEnds(*rot,m_gToStation,m_amdbToGlobal);
      if( firstMdt ){
	m_phimin = tubeEnds.phimin; 
	m_phimax = tubeEnds.phimax;
	firstMdt = false;
      }else{
	updatePhiRanges(tubeEnds.phimin,tubeEnds.phimax,m_phimin,m_phimax);
      }

      if( msgLvl(MSG::VERBOSE) ) {
	msg(MSG::VERBOSE) << " new MDT hit " << m_idHelperTool->toString( id )  << " x " << lpos.x() << " y " << lpos.y()
			  << " time " << rot->driftTime() << " r " << r << " dr " << dr << " phi range " << tubeEnds.phimin << " " << tubeEnds.phimax;
	if( m_usePreciseError ) msg(MSG::VERBOSE) << " dr(2) " << preciseError << endmsg;
	else msg(MSG::VERBOSE) << endmsg;
      }
      dcs.push_back( dc );
      
      chamberSet.insert(elId);
      
      ++m_dcStatistics[detEl];

      ++index;
      
    }
      
    if( m_doGeometry ) {
      
      ATH_MSG_VERBOSE(" using chamber geometry with #chambers " << chamberSet.size());
      
      // set to remove duplicate geometry descriptions, will happen if hits in multiple chambers are present
      std::set<const Muon::MuonIntersectGeometry*> geoSet;
      
      // vector to store chamber geometries 
      std::vector<TrkDriftCircleMath::MdtChamberGeometry> geos;
      
      // loop over chambers
      std::set<Identifier>::iterator it = chamberSet.begin();
      std::set<Identifier>::iterator it_end = chamberSet.end();
      for( ;it!=it_end;++it ){

	// this does not work yet....
	// get geometry of chamber
	//const Muon::MuonIntersectGeometry* intersectGeometry = m_intersectSvc->getChamberGeometry( *it );
	  
	//const MdtIntersectGeometry* mdtGeo = dynamic_cast<const MdtIntersectGeometry*>(intersectGeometry);
	//if( !mdtGeo ) continue;
	//geos.push_back( *mdtGeo->mdtChamberGeometry() );
	
	geos.push_back( createChamberGeometry( *it, m_gToStation ) );
      }
      
      // create new geometry
      m_multiGeo = new TrkDriftCircleMath::MdtMultiChamberGeometry( geos );
    }
    return dcs;
  }

  TrkDriftCircleMath::MdtChamberGeometry DCMathSegmentMaker::createChamberGeometry( const Identifier& chid, 
										    const Amg::Transform3D& gToStation ) const {

    /* calculate chamber geometry
       it takes as input:
       distance between the first and second tube in the chamber within a layer along the tube layer (tube distance)
       distance between the first tube in the first layer and the first tube in the second layer along the tube layer (tube stagering)
       distance between the first and second layer perpendicular to the tube layers (layer distance)
       position of the first hit in ml 0 and ml 1 (2D in plane)
       total number of multilayers
       total number of layers
       total number of tubes per layer for each multilayer
       an identifier uniquely identifying the chamber
    */

    Amg::Vector3D firstTubeMl0(0., 0., 0.);
    Amg::Vector3D firstTubeMl1(0., 0., 0.);

    // get id
    int eta =  m_idHelperTool->mdtIdHelper().stationEta(chid);
    int phi = m_idHelperTool->mdtIdHelper().stationPhi(chid);
    int name = m_idHelperTool->mdtIdHelper().stationName(chid);
    int isBarrel = m_idHelperTool->mdtIdHelper().isBarrel(chid);
    TrkDriftCircleMath::MdtStationId  stationId( isBarrel, name, eta, phi );


    // get detEL for first ml (always there)
    const MuonGM::MdtReadoutElement* detEl1 = m_detMgr->getMdtReadoutElement( m_idHelperTool->mdtIdHelper().channelID( name,eta,phi,1,1,1 ) );
    const MuonGM::MdtReadoutElement* detEl2 = 0;
    int ntube2 = 0;
    // number of multilayers in chamber
    int nml = detEl1->nMDTinStation();
    
    // treament of chambers with two ml
    if( nml == 2 ){
      Identifier firstIdml1 = m_idHelperTool->mdtIdHelper().channelID( name,eta,phi,2,1,1 );
      detEl2 = m_detMgr->getMdtReadoutElement( firstIdml1 ); 
      firstTubeMl1 = gToStation*(detEl2->surface( firstIdml1 ).center());
      ntube2 = detEl2->getNtubesperlayer();
    }

    // number of layers and tubes
    int nlay = detEl1->getNLayers();
    int ntube1 = detEl1->getNtubesperlayer();

    // position first tube in ml 0
    Identifier firstIdml0 = m_idHelperTool->mdtIdHelper().channelID( name,eta,phi,1,1,1 );
    firstTubeMl0 = gToStation*(detEl1->surface( firstIdml0 ).center());

    // position second tube in ml 0
    Identifier secondIdml0 = m_idHelperTool->mdtIdHelper().channelID( name,eta,phi,1,1,2 );
    Amg::Vector3D secondTubeMl0 = gToStation*(detEl1->surface( secondIdml0 ).center());

    TrkDriftCircleMath::LocPos firstTube0( firstTubeMl0.y(), firstTubeMl0.z() );
    TrkDriftCircleMath::LocPos firstTube1( firstTubeMl1.y(), firstTubeMl1.z() );

    // position first tube ml 0 and 1
    Identifier firstIdml0lay1 = m_idHelperTool->mdtIdHelper().channelID( name,eta,phi,1,2,1 );
    Amg::Vector3D firstTubeMl0lay1 = gToStation*(detEl1->surface( firstIdml0lay1 ).center());


    double tubeDist = (secondTubeMl0 - firstTubeMl0).y();      // distance between tube in a given layer
    double tubeStage = (firstTubeMl0lay1 - firstTubeMl0).y();  // tube stagering distance
    double layDist = (firstTubeMl0lay1 - firstTubeMl0).z();    // distance between layers


    TrkDriftCircleMath::MdtChamberGeometry mdtgeo(stationId,nml,nlay,ntube1,ntube2,firstTube0,firstTube1,
						  tubeDist,tubeStage,layDist,detEl1->surface().center().theta());

    if( msgLvl(MSG::VERBOSE) ) mdtgeo.print();

    return mdtgeo;
  }


  void DCMathSegmentMaker::associateMDTsToSegment( const Amg::Vector3D& gdir, TrkDriftCircleMath::Segment& segment, 
						   const std::vector<const MdtDriftCircleOnTrack*>& mdts ) const {
    
    // clear result vectors
    m_deltaVec.clear();
    m_outoftimeVec.clear();

    // convert segment parameters + x position from road
    const TrkDriftCircleMath::Line& line = segment.line(); 
    TrkDriftCircleMath::TransformToLine toLineml1(line);
    TrkDriftCircleMath::TransformToLine toLineml2(line);
    if(segment.hasCurvatureParameters()) {
      //ml2 segment direction
      double ml2phi = line.phi() - segment.deltaAlpha();
      TrkDriftCircleMath::LocDir ml2dir(cos(ml2phi),sin(ml2phi));
      //ml2 segment position
      const TrkDriftCircleMath::LocPos ml1LocPos = m_multiGeo->tubePosition(0,m_multiGeo->nlay(),0);
      const TrkDriftCircleMath::LocPos ml2LocPos = m_multiGeo->tubePosition(1,1,0);           
      double chamberMidPtY = (ml1LocPos.y() + ml2LocPos.y())/2.0;
      TrkDriftCircleMath::LocPos ml2pos(segment.deltab(),chamberMidPtY);
      //construct the new ml2 segment line & transform
      const TrkDriftCircleMath::Line ml2line(ml2pos,ml2dir);
      TrkDriftCircleMath::TransformToLine tmptoLine(ml2line);      
      //set the ml2 line
      toLineml2 = tmptoLine;
    }

    
    for( TrkDriftCircleMath::DCOnTrackIt dcit = segment.dcs().begin();dcit!=segment.dcs().end();++dcit ){
	
      if( dcit->state() == TrkDriftCircleMath::DCOnTrack::Delta ) {
	m_deltaVec.insert( mdts[dcit->index()]->identify() );
      }
      
      if( dcit->state() == TrkDriftCircleMath::DCOnTrack::OutOfTime ){
	m_outoftimeVec.insert( mdts[dcit->index()]->identify() );
      }
      
      if( dcit->state() != TrkDriftCircleMath::DCOnTrack::OnTrack ) continue;
	
      const MdtDriftCircleOnTrack* riodc = dynamic_cast<const MdtDriftCircleOnTrack*>( mdts[dcit->index()] );
      if( !riodc ) continue;


      //choose which line to use (ml1 or ml2)
      TrkDriftCircleMath::TransformToLine toLine = toLineml1;
      if(m_idHelperTool->mdtIdHelper().multilayer(riodc->identify()) == 2) toLine = toLineml2;
      // calculate position of hit in line frame
      TrkDriftCircleMath::LocPos pointOnHit  = toLine.toLine( dcit->position() ); 
      
      // calculate position of hit on line in line frame
      TrkDriftCircleMath::LocPos pointOnLine( pointOnHit.x(), 0. );
      
      // transform back to local AMDB coordinates
      TrkDriftCircleMath::LocPos pointOnLineAMDB = toLine.toLocal( pointOnLine );
      
      // get position along wire from ROT
      Amg::Vector3D posAlong = m_gToStation*riodc->globalPosition();
      
      // set yz components
      posAlong[1] = pointOnLineAMDB.x();
      posAlong[2] = pointOnLineAMDB.y();
      
      // back to global
      Amg::Vector3D mdtGP = m_amdbToGlobal*posAlong;
      
      const Trk::StraightLineSurface* surf = dynamic_cast<const Trk::StraightLineSurface*>( &riodc->associatedSurface() );
      if( ! surf ){
	ATH_MSG_WARNING(" dynamic cast to StraightLineSurface failed for mdt!!! ");
	continue;
      }
	  
      // calculate Amg::Vector2D using surf to obtain sign
      Amg::Vector2D locPos(0.,0.);
      if( ! surf->globalToLocal(mdtGP,gdir,locPos) ) ATH_MSG_WARNING(" globalToLocal failed ");
	  
      // calculate side
      Trk::DriftCircleSide side = locPos[Trk::driftRadius] < 0 ? Trk::LEFT : Trk::RIGHT;
	  
      const MdtDriftCircleOnTrack* constDC = 0;
      bool hasT0 = segment.hasT0Shift() && !m_tofTool.empty();
      if( !hasT0 ){
	//ATH_MSG_VERBOSE(" recalibrate MDT hit");
	constDC = m_mdtCreator->createRIO_OnTrack(*riodc->prepRawData(),mdtGP,&gdir);
      }else{
	ATH_MSG_VERBOSE(" recalibrate MDT hit with shift " << segment.t0Shift());
	m_tofTool->ResetSetTShift( segment.t0Shift() );
	constDC = m_mdtCreatorT0->createRIO_OnTrack(*riodc->prepRawData(),mdtGP,&gdir);
      }
      
      if( !constDC ){
	dcit->state( TrkDriftCircleMath::DCOnTrack::OutOfTime );
	continue;
      }

      // update the drift radius after recalibration, keep error
      MdtDriftCircleOnTrack *new_drift_circle=new MdtDriftCircleOnTrack(*constDC);
      m_measurementsToBeDeleted.push_back(new_drift_circle);
      TrkDriftCircleMath::DriftCircle new_dc(dcit->position(), fabs(constDC->driftRadius()), dcit->dr(), dcit->drPrecise(), 
					     static_cast<TrkDriftCircleMath::DriftCircle *>(&(*dcit))->state()
					     , dcit->id(), dcit->index(),new_drift_circle);
      TrkDriftCircleMath::DCOnTrack new_dc_on_track(new_dc, dcit->residual(), dcit->errorTrack());
      (*dcit)=new_dc_on_track;

      MdtDriftCircleOnTrack* dcOn = const_cast<MdtDriftCircleOnTrack*>(constDC);
      if( hasT0 ) {
	if( msgLvl(MSG::VERBOSE) ){
	  double shift = riodc->driftTime() - dcOn->driftTime();
	  ATH_MSG_VERBOSE(" t0 shift " << segment.t0Shift() << " from hit " << shift 
			  << " recal " << dcOn->driftRadius() << " t " << dcOn->driftTime() << "  from fit " << dcit->r() 
			  << " old " << riodc->driftRadius() << " t " << riodc->driftTime());
	  if( fabs( fabs(dcOn->driftRadius()) - fabs(dcit->r()) ) > 0.1 && dcOn->driftRadius() < 19. && dcOn->driftRadius() > 1. ) {
	    ATH_MSG_WARNING("Detected invalid recalibration after T0 shift");
	  }
	}
      }
      m_mdtCreator->updateSign( *dcOn, side );
      double dist = pointOnHit.x();
      m_rioDistVec.push_back( std::make_pair(dist,dcOn) );
    }
    ATH_MSG_DEBUG(" Segment phi range: min " << m_phimin << " max " << m_phimax);
  }
  
  
  std::string DCMathSegmentMaker::printSP( std::pair<double,double> resPull,const DCMathSegmentMaker::Cluster2D& spacePoint ) const {
    std::ostringstream sout;
    Identifier id;
    if( spacePoint.etaHit != 0 && spacePoint.phiHit != 0 ){
      sout << " space point: ";
      id = spacePoint.etaHit->identify();
    }else if( spacePoint.etaHit != 0 && spacePoint.phiHit == 0 ){
      sout << " eta hit:     ";
      id = spacePoint.etaHit->identify();
    }else if( spacePoint.etaHit == 0 && spacePoint.phiHit != 0 ){
      sout << " phi hit:     ";
      id = spacePoint.phiHit->identify();
    }else sout << " UNKNOWN TYPE ";
    sout << " residual  " << std::setw(5) << std::setprecision(3) << resPull.first << " pull " << std::setw(6) << resPull.second 
	 << " " << m_idHelperTool->toString( id );
    return sout.str();
  }


  template<class T>
  struct IdDataVec {
    typedef T                   Entry;
    typedef std::vector<Entry>  EntryVec;
    typedef typename EntryVec::iterator EntryIt;
    
    IdDataVec<T>() {}
    IdDataVec<T>( const Identifier& i ) : id(i) {}

    Identifier id;
    EntryVec   data;
  };

  template<class T>
  struct SortIdDataVec : public std::binary_function<const IdDataVec<T>&,const IdDataVec<T>&,bool> {
    bool operator()(const IdDataVec<T>& d1, const IdDataVec<T>& d2 ){
      return d1.id < d2.id;
    }
  };

  struct SortClByPull : public std::binary_function<const std::pair<double,DCMathSegmentMaker::Cluster2D>&,const std::pair<double,DCMathSegmentMaker::Cluster2D>&,bool> {
    bool operator()(const std::pair<double,DCMathSegmentMaker::Cluster2D>& d1, const std::pair<double,DCMathSegmentMaker::Cluster2D>& d2 ){
      return fabs(d1.first) < fabs(d2.first);
    }  
  };

  std::pair<int,int> DCMathSegmentMaker::associateClustersToSegment( const TrkDriftCircleMath::Segment& segment, const Identifier& chid ) const {
    
    typedef IdDataVec<std::pair<double,Cluster2D> > GasGapData;
    typedef IdDataVec<GasGapData>                   ChamberData;
    typedef std::vector<ChamberData>                ChamberDataVec;
    typedef ChamberDataVec::iterator                ChamberDataIt;
    ChamberDataVec chamberDataVec;

    // keep track of the number of eta/phi hits on the segment
    m_refit = false;
    std::pair<int,int> netaPhiHits(0,0);
    if( segment.clusters().empty() ) return netaPhiHits;

    std::vector<const Trk::MeasurementBase*> phiHits;

    // only refit if there are sufficient phi hits and no multiple phi hits per surface
    m_refit = true; 

    // keep track of detector elements which space points added to the track
    std::set<Identifier> detElOnSegments;
    std::set<MuonStationIndex::PhiIndex> phiIndices;

    ATH_MSG_DEBUG(" Associating clusters: " << segment.clusters().size() << " number of space points " << m_spVec.size());

    // associate space points and sort them per detector element and gas gap
    for( TrkDriftCircleMath::CLCit ccit = segment.clusters().begin();ccit!=segment.clusters().end();++ccit ){

      ATH_MSG_VERBOSE(" accessing cluster: " << ccit->index());
      const Cluster2D& spacePoint = m_spVec[ccit->index()];

      // skip corrupt space points
      if( spacePoint.corrupt() ) {
	ATH_MSG_DEBUG(" Found corrupt space point: index " << ccit->index());	
	continue;
      }
      // reject TGC clusters that are not 2D
      if( m_reject1DTgcSpacePoints && !spacePoint.is2D() && m_idHelperTool->isTgc(spacePoint.identify()) ) {
	ATH_MSG_DEBUG(" Rejecting 1D tgc space point " << m_idHelperTool->toString(spacePoint.identify()));		
	continue;
      }
      if( m_assumePointingPhi && spacePoint.is2D() && !checkPhiConsistency(spacePoint.globalPos.phi()) ){
	ATH_MSG_DEBUG(" Inconsistent phi angle, dropping space point: phi " << spacePoint.globalPos.phi()
		      << " range " << m_phimin << " " << m_phimax);
	continue;
      } 

      std::pair<double,double> resPull = residualAndPullWithSegment( segment, spacePoint );
      
      // if empty or new chamber, add chamber
      if( chamberDataVec.empty() || chamberDataVec.back().id != spacePoint.detElId ){
	detElOnSegments.insert(spacePoint.detElId);
	chamberDataVec.push_back( ChamberData(spacePoint.detElId) );
	MuonStationIndex::PhiIndex phiIndex = m_idHelperTool->phiIndex(spacePoint.detElId);
	phiIndices.insert(phiIndex);
      }
      
      // reference to current chamber data
      ChamberData& chamber = chamberDataVec.back();

      // if same detector element
      if( spacePoint.detElId == chamber.id ){
	// if chamber empty or new gas gap, add gasp gap 
	if( chamber.data.empty() || chamber.data.back().id != spacePoint.gasGapId ){
	  chamber.data.push_back( GasGapData(spacePoint.gasGapId) );
	}	
      }
      
      // reference to current gas gap data
      GasGapData& gasGap = chamber.data.back();
      gasGap.data.push_back( std::make_pair(resPull.second,spacePoint) );
    }

    // calculate the distance between the first and last station, use r in barrel and z in endcaps
    double posFirstPhiStation = 1e6;
    double posLastPhiStation  = 0;


    // loop over chambers and create competing ROTs per chamber
    ChamberDataIt cit = chamberDataVec.begin();
    ChamberDataIt cit_end = chamberDataVec.end();
    for( ;cit!=cit_end;++cit ){

      // select best clusters per gas gap in chamber
      std::list<const Trk::PrepRawData*> etaClusterVec;
      std::list<const Trk::PrepRawData*> phiClusterVec;
      std::set<Identifier>               etaIds;
      // loop over gas gaps
      ChamberData::EntryIt git = cit->data.begin();
      ChamberData::EntryIt git_end = cit->data.end();
      for( ;git!=git_end;++git ){

	// sort space points by their pull with the segment
	std::sort(git->data.begin(),git->data.end(),SortClByPull());
	
	// select all space points with a pull that is within 1 of the best pull
	double bestPull = fabs(git->data.front().first);

	// count number of associated clusters in gas gap
	unsigned int nassociatedSp = 0;
	GasGapData::EntryIt clit = git->data.begin();
	while( clit != git->data.end() && fabs(clit->first) - bestPull < 1.  ){

	  Cluster2D& sp = clit->second;

	  // calculate distance to segment
	  double dist = distanceToSegment( segment, sp.globalPos );
	  ATH_MSG_VERBOSE("    selected space point:  " << m_idHelperTool->toString( sp.identify() ) 
			  << " pull " << fabs(clit->first) << " distance to segment " << dist << " phi " << sp.globalPos.phi());
	  
	  // here keep open the option not to create CompetingMuonClustersOnTrack
	  if( sp.etaHit ){
	    
	    if( !etaIds.count(sp.etaHit->identify()) ) {
	      etaIds.insert(sp.etaHit->identify());
	      
	      if( m_createCompetingROTsEta ) etaClusterVec.push_back( sp.etaHit->prepRawData() );
	      else{
		m_rioDistVec.push_back( std::make_pair(dist,sp.etaHit->clone()) );
		++netaPhiHits.first;
	      }
	    }
	  }
	  if( !sp.phiHits.empty() ) {
	    if( m_createCompetingROTsPhi ){
	      // can have multiple phi hits per cluster, loop over phi hits and add them
	      for( std::vector<const MuonClusterOnTrack*>::iterator pit = sp.phiHits.begin();pit!=sp.phiHits.end();++pit ){
		phiClusterVec.push_back( (*pit)->prepRawData() );
	      }
	    }else{
	      // can have multiple phi hits per cluster, loop over phi hits and add them
	      for( std::vector<const MuonClusterOnTrack*>::iterator pit = sp.phiHits.begin();pit!=sp.phiHits.end();++pit ){
		m_rioDistVec.push_back( std::make_pair(dist,(*pit)->clone()) );
		++netaPhiHits.second;
		phiHits.push_back(*pit);

		// calculate position
		double phiPos = m_isEndcap ? fabs( (*pit)->globalPosition().z() ) : fabs( (*pit)->globalPosition().perp() ); 
		if( phiPos < posFirstPhiStation ) posFirstPhiStation = phiPos;
		if( phiPos > posLastPhiStation  ) posLastPhiStation  = phiPos;
	      }
	      if( sp.phiHits.size() > 1 ) m_refit = false;
	    }
	  }
	  ++nassociatedSp;
	  ++clit;
	}
	// multiple clusters in same gas gap, don't refit
	if( !m_createCompetingROTsPhi && nassociatedSp > 1 ) m_refit = false;
      }
      
      if( m_createCompetingROTsEta ){
	// create competing ROT for eta hits
	if( !etaClusterVec.empty() ){
	  const CompetingMuonClustersOnTrack* etaCompCluster = m_compClusterCreator->createBroadCluster(etaClusterVec,0.);
	  if( !etaCompCluster ){
	    ATH_MSG_DEBUG(" failed to create competing ETA ROT " << etaClusterVec.size());
	  }else{
	    double dist = distanceToSegment( segment, etaCompCluster->globalPosition() );
	    m_rioDistVec.push_back( std::make_pair(dist,etaCompCluster) );
	    ++netaPhiHits.first;

	    if( msgLvl(MSG::VERBOSE) ){
	      msg(MSG::VERBOSE) << "    selected cluster:  " << m_idHelperTool->toString( etaClusterVec.front()->identify() ) << endmsg;
	      for( unsigned int i=0;i<etaCompCluster->containedROTs().size();++i){
		msg(MSG::VERBOSE) << "               content:  " << m_idHelperTool->toString( etaCompCluster->containedROTs()[i]->identify() ) << endmsg;
	      }
	    }
	  }
	}
      }

      if( m_createCompetingROTsPhi ){
	// create competing ROT for phi hits
	if( !phiClusterVec.empty() ) {
	  const CompetingMuonClustersOnTrack* phiCompCluster = m_compClusterCreator->createBroadCluster(phiClusterVec,0.);
	  if( !phiCompCluster ) {
	    ATH_MSG_DEBUG(" failed to create competing PHI ROT " << phiClusterVec.size()); 
	  }else{
	    double dist = distanceToSegment( segment, phiCompCluster->globalPosition() );
	    m_rioDistVec.push_back( std::make_pair(dist,phiCompCluster) );
	    phiHits.push_back(phiCompCluster);
	    ++netaPhiHits.second;

	    if( msgLvl(MSG::VERBOSE) ){
	      msg(MSG::VERBOSE) << "    selected cluster:  " << m_idHelperTool->toString( phiClusterVec.front()->identify() ) << endmsg;
	      for( unsigned int i=0;i<phiCompCluster->containedROTs().size();++i){
		msg(MSG::VERBOSE) << "               content:  " << m_idHelperTool->toString( phiCompCluster->containedROTs()[i]->identify() ) <<endmsg;
	      }
	    }
	    
	    // calculate position
	    double phiPos = m_isEndcap ? fabs( phiCompCluster->globalPosition().z() ) : fabs( phiCompCluster->globalPosition().perp() ); 
	    if( phiPos < posFirstPhiStation ) posFirstPhiStation = phiPos;
	    if( phiPos > posLastPhiStation  ) posLastPhiStation  = phiPos;
	  }
	}
      }
    }

    // add phi hits that were not associated with an eta hit (only in barrel)
    if( (!m_phiVec.empty()||m_recoverBadRpcCabling) && m_addUnassociatedPhiHits && !m_idHelperTool->isEndcap(chid) ) {

      TrkDriftCircleMath::ResidualWithSegment resWithSegment(segment);

      std::map<Identifier,std::list<const Trk::PrepRawData*> > phiClusterMap;

      std::set<const MuonClusterOnTrack*> selectedClusters;
      std::vector<Cluster2D*> phiClusters;
      phiClusters.reserve(m_phiVec.size());

      // create lists of PrepRawData per detector element
      ClusterIt pit = m_phiVec.begin();
      ClusterIt pit_end = m_phiVec.end();
      for( ;pit!=pit_end;++pit ){
	if( !pit->phiHit || pit->corrupt() ) {
	  ATH_MSG_WARNING(" phi clusters without phi hit!!");
	  continue;
	}
	phiClusters.push_back( &*pit);
	selectedClusters.insert(pit->phiHit);
      }

      unsigned int recoveredUnassociatedPhiHits(0);
      if( m_recoverBadRpcCabling ){
	// now loop over 2D space points and add the phi hits to the list if the detEl is not yet added to the segment
	ClusterIt spit = m_spVec.begin();
	ClusterIt spit_end = m_spVec.end();
	for( ;spit!=spit_end;++spit ){
	
	  // skip clusters without phi hit
	  if( !spit->phiHit || spit->corrupt() ) continue;
	  
	  // skip clusters in detector elements that are already associated (ok as this is only done for RPC)
	  if( detElOnSegments.count(spit->detElId) ) continue;

	  MuonStationIndex::PhiIndex phiIndex = m_idHelperTool->phiIndex(spit->detElId);
	  // skip clusters in detector layer
	  if( phiIndices.count(phiIndex) ) continue;


	  bool wasFound = false;
	  for( std::vector<const MuonClusterOnTrack*>::iterator it = spit->phiHits.begin();it!=spit->phiHits.end();++it ){

	    // now to avoid duplicate also skip if the given ROT is already in the list
	    std::pair<std::set<const MuonClusterOnTrack*>::iterator,bool> insertResult = selectedClusters.insert(*it); 
	    if( !insertResult.second ) {
	      // flag as found
	      wasFound = true;
	      
	      // just because I'm paranoid, remove the hits from this cluster that were already inserted up to here
	      for( std::vector<const MuonClusterOnTrack*>::iterator rmit = spit->phiHits.begin();rmit!=it;++rmit ) selectedClusters.erase(*rmit);
	      break;
	    }
	  }
	  if( wasFound ) continue;

	  // if we get here we should add the hit
	  phiClusters.push_back( &*spit);
	  ++recoveredUnassociatedPhiHits;
	}
      }      
      
      unsigned int addedPhiHits(0);
      std::vector<Cluster2D*>::iterator cit = phiClusters.begin();
      std::vector<Cluster2D*>::iterator cit_end = phiClusters.end();
      for( ;cit!=cit_end;++cit ){

	const Identifier& detElId = (*cit)->detElId;

	// check that detector element is not already added to segment
	if( detElOnSegments.count(detElId) ) continue;

	MuonStationIndex::PhiIndex phiIndex = m_idHelperTool->phiIndex(detElId);
	// skip clusters in detector layer
	if( phiIndices.count(phiIndex) ) continue;

	// calculate local cluster position
	Amg::Vector3D locPos = m_gToStation*(*cit)->globalPos;
	
	// calculate intersect of segment with cluster 
	TrkDriftCircleMath::Cluster cl(TrkDriftCircleMath::LocPos(locPos.y(),locPos.z()),1.);
	double residual = resWithSegment.residual(cl);
	double segError = sqrt(resWithSegment.trackError2(cl));
	const MuonGM::RpcReadoutElement* detEl = dynamic_cast<const MuonGM::RpcReadoutElement*>((*cit)->phiHit->detectorElement());
	if( !detEl ){
	  ATH_MSG_WARNING("found RPC prd without RpcReadoutElement");
	  continue;
	}

	// perform bound check
	double stripLength = detEl->StripLength(1);
	bool inBounds = fabs(residual) < 0.5*stripLength + 2. + segError ? true : false;
	if( msgLvl(MSG::DEBUG) ) {
	  msg(MSG::DEBUG) << " Unassociated " << m_idHelperTool->toString((*cit)->phiHit->identify()) 
			  << " pos x " << cl.position().x() << " pos y " << cl.position().y() 
			  << " : residual " << residual << " strip half length " << 0.5*stripLength << " segment error " << segError;
	  if( inBounds ) msg(MSG::DEBUG) << " inBounds" << endmsg;
	  else           msg(MSG::DEBUG) << " outBounds" << endmsg;
	}
	if( inBounds ){
	  // can have multiple phi hits per cluster, loop over phi hits and add them
	  for( std::vector<const MuonClusterOnTrack*>::iterator pit = (*cit)->phiHits.begin();pit!=(*cit)->phiHits.end();++pit ){
	    phiClusterMap[detElId].push_back( (*pit)->prepRawData() );
	  }
	}
      }
    
      // loop over detector elements and created competing ROTs

      std::map<Identifier,std::list<const Trk::PrepRawData*> >::iterator chit = phiClusterMap.begin();
      std::map<Identifier,std::list<const Trk::PrepRawData*> >::iterator chit_end = phiClusterMap.end();
      for( ;chit!=chit_end;++chit ){

	std::list<const Trk::PrepRawData*>& prds = chit->second;
	if( prds.empty() ) {
	  ATH_MSG_WARNING(" chamber without phi hits " << m_idHelperTool->toString(chit->first));
	  continue;
	}

	const CompetingMuonClustersOnTrack* phiCompCluster = m_compClusterCreator->createBroadCluster(prds,0.);
	if( !phiCompCluster ) {
	  ATH_MSG_DEBUG(" failed to create competing PHI ROT " << prds.size()); 
	}else{
	  double dist = distanceToSegment( segment, phiCompCluster->globalPosition() );

	  if( fabs(dist) > m_maxAssociateClusterDistance ){
	    delete phiCompCluster;
	    ATH_MSG_VERBOSE("    rejected unassociated cluster:  " << m_idHelperTool->toString( prds.front()->identify() ) << "  distance to segment " << dist);
	  continue;
	  }
	  m_rioDistVec.push_back( std::make_pair(dist,phiCompCluster) );
	  phiHits.push_back(phiCompCluster);
	  ++netaPhiHits.second;
	  ++addedPhiHits;
	  if( msgLvl(MSG::VERBOSE) ){
	    msg(MSG::VERBOSE) << "    selected unassociated cluster:  " << m_idHelperTool->toString( prds.front()->identify() ) << "  distance to segment " << dist << endmsg;
	    for( unsigned int i=0;i<phiCompCluster->containedROTs().size();++i){
	      msg(MSG::VERBOSE) << "               content:  " << m_idHelperTool->toString( phiCompCluster->containedROTs()[i]->identify() ) <<endmsg;
	    }
	  }
	}
      }
      ATH_MSG_VERBOSE("Added "  << addedPhiHits << " unass phi hits out of " << m_phiVec.size() << " phi hits without eta hit and "
		      << recoveredUnassociatedPhiHits << " with unassociated eta hit ");
    }
    
    // calculate distance between first and last phi trigger hit, refit if there is a good constraint on phi
    double phiDistanceMax = posLastPhiStation - posFirstPhiStation; 
    if( m_isEndcap && phiDistanceMax < 1000. ) m_refit = false;
    else if( phiDistanceMax < 400. )           m_refit = false;
    
    return netaPhiHits;
  }
    
  double DCMathSegmentMaker::distanceToSegment( const TrkDriftCircleMath::Segment& segment, const Amg::Vector3D& hitPos ) const 
  {

    const TrkDriftCircleMath::Line& line = segment.line(); 
    TrkDriftCircleMath::TransformToLine toLine(line);
    double cos_sinLine = cos( line.phi() )/sin( line.phi() );

    // calculate local AMDB position
    Amg::Vector3D locPos = m_gToStation*hitPos;
      
    TrkDriftCircleMath::LocPos lpos(locPos.y(),locPos.z());
      
    // calculate distance of segment to measurement surface
    double delta_y = lpos.y() - line.position().y();
      
    // calculate position of hit in line frame
    TrkDriftCircleMath::LocPos lineSurfaceIntersect( delta_y*cos_sinLine + line.position().x(), lpos.y() );
      
    // calculate position of hit in line frame
    TrkDriftCircleMath::LocPos pointOnHit  = toLine.toLine( lineSurfaceIntersect ); 
      
    return pointOnHit.x();   
  }
    

  DataVector<const Trk::MeasurementBase>* DCMathSegmentMaker::createROTVec() const {

    // sort hits according to they distance to the segment position
    std::sort( m_rioDistVec.begin(),m_rioDistVec.end(),SortByDistanceToSegment() );
      
    DataVector<const Trk::MeasurementBase>* rioVec = new DataVector<const Trk::MeasurementBase>();
    rioVec->reserve(m_rioDistVec.size());
    std::vector<std::pair<double,const Trk::MeasurementBase*> >::iterator rdit = m_rioDistVec.begin();
    std::vector<std::pair<double,const Trk::MeasurementBase*> >::iterator rdit_end = m_rioDistVec.end();
    for( ;rdit!=rdit_end;++rdit ){
      rioVec->push_back( rdit->second );
    }
    return rioVec;
  }

  std::pair<double,double> DCMathSegmentMaker::residualAndPullWithSegment( const TrkDriftCircleMath::Segment& segment, const Cluster2D& spacePoint ) const 
  {
    const TrkDriftCircleMath::Line& line = segment.line(); 
    double phi = line.phi();

    if( fabs(phi) < 0.0000001 ) {
      ATH_MSG_WARNING("Unphysical phi angle " << phi);
      phi = 0.0000001;
      if( line.phi() < 0. ) phi *= -1.;
    }
    double cos_sinLine = cos( phi )/sin( phi );

    // calculate sp postion in AMDB reference frame
    Amg::Vector3D locPos = m_gToStation*spacePoint.globalPos;
    TrkDriftCircleMath::LocPos lpos(locPos.y(),locPos.z());
	
    // calculate distance of segment to measurement surface
    double delta_y = lpos.y() - line.position().y();

    // calculate position of hit in line frame
    TrkDriftCircleMath::LocPos lineSurfaceIntersect( delta_y*cos_sinLine + line.position().x(), lpos.y() );
	
    // calculate position of hit in line frame
    double residual  = lpos.x() - lineSurfaceIntersect.x();
    double pull = residual/spacePoint.error;
    return std::make_pair(residual,pull);
  }	



  void DCMathSegmentMaker::calculateHoles( Identifier chid, Amg::Vector3D gpos, Amg::Vector3D gdir, bool hasMeasuredCoordinate ) const {
    
    // calculate crossed tubes
    const MuonStationIntersect& intersect = m_intersectSvc->tubesCrossedByTrack(chid, gpos, gdir );


    // set to identify the hit on the segment
    std::set<Identifier> hitsOnSegment;
    std::set<Identifier> chambersOnSegment;
    int firstLayer = -1;
    int lastLayer  = -1;
    std::vector<std::pair<double,const Trk::MeasurementBase*> >::iterator rdit = m_rioDistVec.begin();
    std::vector<std::pair<double,const Trk::MeasurementBase*> >::iterator rdit_end = m_rioDistVec.end();
    for( ;rdit!=rdit_end;++rdit ){
      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(rdit->second);
      if( mdt ) {
	const Identifier& id = mdt->identify();
	int layer = (m_idHelperTool->mdtIdHelper().tubeLayer(id)-1) + 4*(m_idHelperTool->mdtIdHelper().multilayer(id)-1);
	if( firstLayer == -1 ) firstLayer = layer;
	else                   lastLayer = layer;

	hitsOnSegment.insert( mdt->identify() );
	chambersOnSegment.insert(m_idHelperTool->chamberId(mdt->identify()));
      }
    }

    // cross check for cosmic case
    if( firstLayer > lastLayer ){
      int tmpLayer = firstLayer;
      firstLayer = lastLayer;
      lastLayer = tmpLayer;
    }
    if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << " Tube layer ranges: " << firstLayer << " -- " 
						  << lastLayer << " crossed tubes " << intersect.tubeIntersects().size() << std::endl;
    // clear hole vector
    m_holeVec.clear();
    for( unsigned int ii=0;ii<intersect.tubeIntersects().size();++ii ){

      const MuonTubeIntersect& tint = intersect.tubeIntersects()[ii];
      if( !chambersOnSegment.count( m_idHelperTool->chamberId(tint.tubeId) ) ) {
	if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << " chamber not on segment, not counting tube  " << tint.rIntersect << " l " << tint.xIntersect 
						      << " " << m_idHelperTool->toString(tint.tubeId) << std::endl;
	continue;
      }

      const Identifier& id = tint.tubeId;
      int layer = (m_idHelperTool->mdtIdHelper().tubeLayer(id)-1) + 4*(m_idHelperTool->mdtIdHelper().multilayer(id)-1);

      bool notBetweenHits = layer < firstLayer || layer > lastLayer;
      double distanceCut = hasMeasuredCoordinate ? -20 : -200.;
      if( notBetweenHits && (fabs( tint.rIntersect ) > 14.4 || (!m_allMdtHoles && tint.xIntersect > distanceCut ) ) ){
	if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << " not counting tube:  distance to wire " << tint.rIntersect << " dist to tube end " << tint.xIntersect 
						      << " " << m_idHelperTool->toString(tint.tubeId) << std::endl;
      }else{
	// check whether there is a hit in this tube 
	if( hitsOnSegment.count( tint.tubeId ) ) {
	  if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << " tube on segment:  distance to wire " << tint.rIntersect << " dist to tube end " << tint.xIntersect 
							<< " " << m_idHelperTool->toString(tint.tubeId) << std::endl;
	  continue;
	}
	// check whether there is a delta electron in this tube 
	if( m_removeDeltas ){
	  if( m_deltaVec.count( tint.tubeId ) ) {
	    if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << " removing delta, distance to wire " << tint.rIntersect << " dist to tube end " << tint.xIntersect 
							  << " " << m_idHelperTool->toString(tint.tubeId) << std::endl;
	    continue;
	  }

	  const MdtPrepData* prd = findMdt(id);
	  if( prd && fabs(prd->localPosition()[Trk::locR]) < fabs(tint.rIntersect) ){
	    if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << " found and removed delta, distance to wire " << tint.rIntersect << " dist to tube end " << tint.xIntersect 
							  << " " << m_idHelperTool->toString(tint.tubeId) << std::endl;
	    continue;
	  }
	}
	if( msgLvl(MSG::VERBOSE)  ) {
	  std::string holeType = "hole ";
	  if( m_outoftimeVec.count(tint.tubeId) ) holeType = "Out-of-time ";
	  msg(MSG::VERBOSE) << holeType << " distance to wire " << tint.rIntersect << " dist to tube end " << tint.xIntersect 
			    << " " << m_idHelperTool->toString(tint.tubeId);
	  if( !notBetweenHits ) msg(MSG::VERBOSE) << " between hits";
	  msg(MSG::VERBOSE) << std::endl;
	}

	m_holeVec.push_back( tint.tubeId );
      }
    }
    if( msgLvl(MSG::VERBOSE)  ) msg(MSG::VERBOSE) << endmsg;
  }

  const MdtPrepData* DCMathSegmentMaker::findMdt( const Identifier& id ) const {
    IdentifierHash colHash;
    if( m_idHelperTool->mdtIdHelper().get_module_hash(m_idHelperTool->chamberId(id),colHash) == 0 ){
      const MdtPrepDataContainer* prdContainer = 0;
      SG::ReadHandle<Muon::MdtPrepDataContainer> MdtCont(m_mdtKey);
      if(!MdtCont.isValid()){
	ATH_MSG_WARNING("Cannot retrieve MdtPrepDataContainer ");
	return 0;
      }
      else{
	prdContainer = MdtCont.cptr();
      }
      if(!prdContainer){
	ATH_MSG_WARNING("No MDT prd collection retrieved");
	return 0;
      }
      MdtPrepDataContainer::const_iterator colIt = prdContainer->indexFind(colHash);
      if( colIt == prdContainer->end() || !*colIt ) return 0;
      const MdtPrepDataCollection& col = **colIt;
      MdtPrepDataCollection::const_iterator mdtIt = col.begin();
      MdtPrepDataCollection::const_iterator mdtIt_end = col.end();
      for( ;mdtIt!=mdtIt_end;++mdtIt ){
	if( (*mdtIt)->identify() == id ) return *mdtIt;
      }
    }
    return 0;
  }

  const MdtDriftCircleOnTrack* DCMathSegmentMaker::findFirstRotInChamberWithMostHits( const std::vector<const MdtDriftCircleOnTrack*>& mdts ) const {
     
    int hitsInChamberWithMostHits = 0;
    std::map<Identifier,int> hitsPerChamber;
    int currentSector = -1;
    const MdtDriftCircleOnTrack* rotInChamberWithMostHits = 0;

    // loop over all MDTs and count number of MDTs per chamber
    for( std::vector<const MdtDriftCircleOnTrack*>::const_iterator it=mdts.begin();it!=mdts.end();++it){

      const MdtDriftCircleOnTrack* rot = *it;

      if( !rot ){
	ATH_MSG_WARNING(" rot not a MdtDriftCircleOnTrack ");
	continue;
      }
      Identifier chId = m_idHelperTool->chamberId((*it)->identify());
      int sector = m_idHelperTool->sector(chId);
      if( currentSector == -1 ){
	currentSector = sector;
      }else if( sector != currentSector ){
	++m_nmultipleHitWarnings;
	if( m_nmultipleHitWarnings < 3 ) ATH_MSG_WARNING("Found multiple sectors on input, cannot process data ");
	return 0; 
      }
      int& hitsInCh = hitsPerChamber[chId];
      ++hitsInCh;
      if( hitsInCh > hitsInChamberWithMostHits ){
	hitsInChamberWithMostHits = hitsInCh;
	rotInChamberWithMostHits  = rot;
      } 
    }
    return rotInChamberWithMostHits;
  }

  std::pair<bool,double> DCMathSegmentMaker::rotateLocalAngleXZIntoBounds( double xline, double zline, double dXdZ, std::vector<DCMathSegmentMaker::HitInXZ>& hits ) const {

    // routine checks whether hits are in bounds, if not rotates the local angle XZ until all hits are in bounds
    
    std::pair<bool,double> result(false,dXdZ);
    if( hits.empty() ) return result;

    
    double dXdZMinLeft  = 1e9;
    double dXdZMinRight = 1e9;
    std::vector<HitInXZ>::iterator hit = hits.begin();
    std::vector<HitInXZ>::iterator hit_end = hits.end();
    if(msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " recalculating using phi measurement " << m_idHelperTool->toString(hits.front().id) 
						<< " pos (" << xline << "," << zline << ")" << std::endl;
    for( ;hit!=hit_end;++hit ){

      // sl extrapolation
      double locExX = xline + dXdZ*(hit->z-zline);

      
      bool newBoundLeft = false;
      bool newBoundRight = false;
      
      // sanity checks to avoid division + only for MDT for now
      if( hit->isMdt ){

	// now calculate, if out of bounds, for which angle the situation would be ok
	double dz = (hit->z-zline);
	if( fabs(dz) > 0.01 ) {
	  if( locExX < hit->xmin ){
	    double inBoundsdXdZ = (hit->xmin-xline)/dz;
	    if( fabs(inBoundsdXdZ) < fabs(dXdZMinLeft) ) dXdZMinLeft = inBoundsdXdZ;
	    newBoundLeft = true;
	  }
	  if( locExX > hit->xmax ) {
	    double inBoundsdXdZ = (hit->xmax-xline)/dz;
	    if( fabs(inBoundsdXdZ) < fabs(dXdZMinRight) ) dXdZMinRight = inBoundsdXdZ;
	    newBoundRight = true;
	  }
	}
      }
      if(msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << "  " << std::setw(65) << m_idHelperTool->toString( hit->id )
			  << " pos (" << std::setw(6) << (int)hit->x << "," << std::setw(6) << (int)hit->z 
			  << ")  ex pos " << std::setw(6) << (int)locExX
			  << " min " << std::setw(6) << (int)hit->xmin << " max " << std::setw(6) << (int)hit->xmax
			  << " phimin " << std::setw(6) << hit->phimin << " phimax " << std::setw(6) << hit->phimax;
	if( locExX < hit->xmin || locExX > hit->xmax ) msg(MSG::VERBOSE) << " outBounds ";
	if( newBoundRight ) msg(MSG::VERBOSE) << " right " << dXdZMinRight;
	if( newBoundLeft  ) msg(MSG::VERBOSE) << " left " << dXdZMinLeft;
	msg(MSG::VERBOSE) << std::endl;
      }
    }
  
    if(msgLvl(MSG::VERBOSE) ) {
      if( fabs(dXdZMinRight) < 1e8 ) msg(MSG::VERBOSE) << " selected right bound " << dXdZMinRight;
      if( fabs(dXdZMinLeft)  < 1e8 ) msg(MSG::VERBOSE) << " selected left bound " << dXdZMinLeft;
      msg(MSG::VERBOSE) << endmsg;
    }

    if( fabs(dXdZMinRight) < 1e8 ) {
      result.first = true;
      result.second = dXdZMinRight;
    }else if( fabs(dXdZMinLeft)  < 1e8 ){
      result.first = true;      
      result.second = dXdZMinLeft;
    }

    return result;
  }

  bool DCMathSegmentMaker::checkBoundsInXZ( double xline, double zline, double dXdZ, std::vector<DCMathSegmentMaker::HitInXZ>& hits ) const {

    bool ok = true;

    // look over hits and check whether all are in bounds
    std::vector<HitInXZ>::iterator hit = hits.begin();
    std::vector<HitInXZ>::iterator hit_end = hits.end();
    for( ;hit!=hit_end;++hit ){

      bool outBounds = false;
      double locExX = xline + dXdZ*(hit->z-zline);
      if( hit->isMdt && (locExX < hit->xmin - 1. || locExX > hit->xmax + 1.) ){
	ok = false;
	outBounds = true;
	if( !msgLvl(MSG::DEBUG) ) break;
      }

      if( outBounds &&msgLvl(MSG::DEBUG) ){
	msg(MSG::DEBUG) << "  " << std::setw(65) << m_idHelperTool->toString( hit->id )
			<< " pos (" << std::setw(6) << (int)hit->x << "," << std::setw(6) << (int)hit->z 
			<< ")  ex pos " << std::setw(6) << (int)locExX
			<< " min " << std::setw(6) << (int)hit->xmin << " max " << std::setw(6) << (int)hit->xmax
			<< " phimin " << std::setw(6) << hit->phimin << " phimax " << std::setw(6) << hit->phimax
			<< " outBounds, cross-check" << endmsg;
      }
    }
    return ok;
  }

  bool DCMathSegmentMaker::updateSegmentPhi( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir, 
					     Amg::Vector2D& segLocPos, Trk::LocalDirection& segLocDir, 
					     Trk::PlaneSurface& surf, const std::vector<const Trk::MeasurementBase*>& rots ) const {

    bool hasUpdated = false;

    const Amg::Transform3D& segmentToGlobal = surf.transform();
    Amg::Transform3D gToSegment = surf.transform().inverse();
    Amg::Vector3D ldir = gToSegment*gdir;
    
    // ensure that we can extrapolate 
    if( ldir.z() < 0.0001 ) return false;

    double dXdZ = ldir.x()/ldir.z();
    double dYdZ = ldir.y()/ldir.z();
    Amg::Vector3D lsegPos = gToSegment*gpos;
    double xline =  lsegPos.x();
    double yline =  lsegPos.y();
    double zline =  lsegPos.z();
    if(msgLvl(MSG::VERBOSE) )  msg(MSG::VERBOSE) << " Associated  hits " << rots.size() 
						 << " angleXZ " << 90.*segLocDir.angleXZ()/acos(0.) 
						 << " dXdZ " << dXdZ << " seg Pos (" << xline << " " << zline << ") " 
						 << segLocPos << std::endl;

    std::vector<HitInXZ> hits;
    hits.reserve(rots.size());

    unsigned int nphiHits(0);
    HitInXZ* firstPhiHit = 0;
    HitInXZ* lastPhiHit = 0;

    for( std::vector<const Trk::MeasurementBase*>::const_iterator it = rots.begin();it!=rots.end();++it ){

      Identifier id = m_helper->getIdentifier(**it);
      if( !id.is_valid() ) continue;
      Amg::Vector3D lpos;
      double lxmin(0),lxmax(0),phimin(0.),phimax(0.);
      bool isMdt = m_idHelperTool->isMdt(id);
      bool measuresPhi = m_idHelperTool->measuresPhi(id);
      if( isMdt ){

	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*it);
        if (!mdt) continue;
	TubeEnds tubeEnds = localTubeEnds(*mdt,gToSegment,segmentToGlobal);

	lxmin = tubeEnds.lxmin;
	lxmax = tubeEnds.lxmax;
	phimin = tubeEnds.phimin;
	phimax = tubeEnds.phimax;
      }else{

	lpos = gToSegment*(*it)->globalPosition();
	lxmin = lpos.x() - 5*Amg::error((*it)->localCovariance(),Trk::locX);
	lxmax = lpos.x() + 5*Amg::error((*it)->localCovariance(),Trk::locX);
	
	const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*it);
	if( !measuresPhi ){
	  if( crot ){
	    const MuonGM::RpcReadoutElement* detEl = dynamic_cast<const MuonGM::RpcReadoutElement*>(crot->containedROTs().front()->prepRawData()->detectorElement());
	    if( detEl ){
	      // perform bound check
	      double stripLength = detEl->StripLength(0);
	      
	      lxmin = lpos.x() - 0.5*stripLength;
	      lxmax = lpos.x() + 0.5*stripLength;
	    }
	  }
	  Amg::Vector3D locPosition = lpos;
	  locPosition[0] = lxmin;
	  Amg::Vector3D globalPos = segmentToGlobal*locPosition;
	  double phi1 = globalPos.phi(); 

	  locPosition[0] = lxmax;
	  globalPos = segmentToGlobal*locPosition;
	  double phi2 = globalPos.phi(); 
	  phimin = phi1 < phi2 ? phi1 : phi2;
	  phimax = phi1 < phi2 ? phi2 : phi1;

	}else{
	  if( m_idHelperTool->isTgc(id) ){

	    // need some special tricks for TGC phi hits as their reference plane can be rotated 
	    // with respect to the MDT frame

	    // get orientation angle of strip to rotate back from local frame to strip
	    // copy code from ROT creator
	    int stripNo = m_idHelperTool->tgcIdHelper().channel(id);
	    int gasGap = m_idHelperTool->tgcIdHelper().gasGap(id);
	    if (!crot) {
	      ATH_MSG_WARNING("dynamic cast failed for CompetingMuonClustersOnTrack");
	      continue;
	    }
	    const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(crot->containedROTs().front()->prepRawData()->detectorElement());
	    if (!detEl) {
	      ATH_MSG_WARNING("dynamic cast failed for TgcReadoutElement");
	      continue;
	    }
	    // calculate two points along the tgc phi strip in the local tgc reference frame
	    Amg::Vector3D lposTGC = detEl->localChannelPos(id);
	    double z_shift = lposTGC.z()+10;
	    double locy_shift = detEl->stripCtrX(gasGap, stripNo, z_shift );
	    if (0 < detEl->getStationEta()) {
	      locy_shift *= -1.;
	    }
	    Amg::Vector3D lpos_shift(lposTGC.x(),locy_shift,z_shift);
	    
	    // 	    std::cout << " local phi pos " << lposTGC << " shifted pos " << lpos_shift << std::endl;

	    // transform the two points to global coordinates
	    const Amg::Transform3D tgcTrans = detEl->absTransform();
	    Amg::Vector3D gposL    = tgcTrans*lposTGC;
	    Amg::Vector3D gposL_shift    = tgcTrans*lpos_shift;

	    // now transform them into the segment frame
	    Amg::Vector3D lposSeg = gToSegment*gposL;
	    Amg::Vector3D lposSeg_shift = gToSegment*gposL_shift;

	    // calculate the y coordinate of the intersect of the segment with the TGC plane in the segment frame
	    double segYAtHit = yline + dYdZ*(lposSeg.z()-zline); 

	    // the TGC phi strip is a line in the xy plane, calculate the x position of the point on the line
	    // at the y intersect position of the segment
	    double tgcdX = lposSeg_shift.x() - lposSeg.x();
	    double tgcdY = lposSeg_shift.y() - lposSeg.y();
	    if( fabs(tgcdY) < 0.0001 ) {
	      ATH_MSG_WARNING(" Bad TGC phi strip orientation ");
	      continue;
	    }
	    double tgcExX  = tgcdX/tgcdY*(segYAtHit-lposSeg.y()) + lposSeg.x(); 
	    lpos[0] = tgcExX;
	    lpos[1] = segYAtHit;
	    if(msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " In seg frame: phi pos " << lposSeg << " shifted pos " << lposSeg_shift 
							<< " intersect with segment " << lpos << std::endl;


	  }
	  Amg::Vector3D globalPos = segmentToGlobal*lpos;
	  phimin = globalPos.phi(); 
	  phimax = phimin;
	  
	  // check whether phi is consistent with segment phi range
	  bool phiOk = checkPhiConsistency(phimin);
	  if( !phiOk ){
	    if(msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " Inconsistent phi "  << phimin << " range " << m_phimin << " " << m_phimax << std::endl;
	  }
	}
      }
      
      hits.push_back( HitInXZ(id,isMdt,measuresPhi,lpos.x(),lpos.z(),lxmin,lxmax,phimin,phimax) );
      if( measuresPhi ){
	++nphiHits;
	if( !firstPhiHit ) firstPhiHit = new HitInXZ(hits.back());
	else{
	  double distPhiHits = fabs(firstPhiHit->z - hits.back().z);
	  if( distPhiHits > 500. ){
	    if( lastPhiHit ) delete lastPhiHit;
	    lastPhiHit = new HitInXZ(hits.back());
	  }else{
	    // not count this phi hit
	    --nphiHits;
	    if(msgLvl(MSG::DEBUG) ){
	      msg(MSG::DEBUG) << " close phi hits, distance " << distPhiHits << std::endl;
	    }
	  }
	}
      }
      if(msgLvl(MSG::VERBOSE) ){
	double locExX = xline + dXdZ*(lpos.z()-zline);
	msg(MSG::VERBOSE) << "  " << std::setw(65) << m_idHelperTool->toString( id )
			  << " pos (" << std::setw(6) << (int)lpos.x() << "," << std::setw(6) << (int)lpos.z() 
			  << ")  ex pos " << std::setw(6) << (int)locExX
			  << " min " << std::setw(6) << (int)lxmin << " max " << std::setw(6) << (int)lxmax
			  << " phimin " << std::setw(6) << phimin << " phimax " << std::setw(6) << phimax;
	if( lpos.x() < lxmin || lpos.x() > lxmax ) msg(MSG::VERBOSE) << " outBounds";
	msg(MSG::VERBOSE) << std::endl;
      }
    } 


    if( nphiHits == 1 ){
      
      if( !firstPhiHit ){
	ATH_MSG_WARNING(" Pointer to first phi hit not set, this should not happen! ");
      }else{
	if( xline != firstPhiHit->x ) {
	  hasUpdated = true;

	  // use phi position of the phi hit
	  xline =  firstPhiHit->x;
	  zline =  firstPhiHit->z;

	  if( m_assumePointingPhi ){
	    Amg::Vector3D ipLocPos = gToSegment*Amg::Vector3D(0.,0.,0.);
	    if(msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " IP position in local frame " << ipLocPos;
	    
	    double dz = ipLocPos.z()-zline;
	    if( fabs(dz) > 0.001 ){
	      if(msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " hit (" << xline << "," << zline << ")  IP (" << ipLocPos.x() << "," << ipLocPos.z()
							  << ")  dXdZ " << (ipLocPos.x()-xline)/dz << " old " << dXdZ << std::endl;
	      dXdZ = (ipLocPos.x()-xline)/dz;
	    }
	  }
	} 
      }
    }else if( nphiHits == 2 ){
      
      if( !firstPhiHit || !lastPhiHit ){
	ATH_MSG_WARNING(" Pointer to one of the two phi hit not set, this should not happen! ");
      }else{
	double dz = lastPhiHit->z - firstPhiHit->z;
	// use phi position of the first hit
	xline =  firstPhiHit->x;
	zline =  firstPhiHit->z;
	if( fabs(dz) > 300. ){
	  double dx = lastPhiHit->x - firstPhiHit->x;
	  hasUpdated = true;
	  
	  // if the two hits are far enough apart, also use the direction of the line connecting the two hits.
	  dXdZ  = dx/dz;
	}
      } 
    }else{
      // in all other cases just rotate until the MDTs are ok

    }

    if( hasUpdated ){
      // move segment to position of phi hit
      double segX = xline - dXdZ*zline;
      
      // finally check whether now everything is in bounds
      bool ok = checkBoundsInXZ(segX,0.,dXdZ,hits);
      if( !ok ){
	// give WARNING and give up for now
	ATH_MSG_DEBUG("still several out of bounds hits after rotation: posx(" << segX << ") dXdZ " << dXdZ << " keeping old result ");
      }

      // update segment parameters
      double alphaYZ = segLocDir.angleYZ();
      double alphaXZ = atan2(1,dXdZ);
      
      segLocPos[Trk::locX] = segX;
      segLocDir = Trk::LocalDirection(alphaXZ,alphaYZ);
    }

    delete firstPhiHit;
    delete lastPhiHit;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << endmsg;
    return hasUpdated;
  } 



  DCMathSegmentMaker::TubeEnds DCMathSegmentMaker::localTubeEnds( const MdtDriftCircleOnTrack& mdt, 
								  const Amg::Transform3D& gToSegment,
								  const Amg::Transform3D& segmentToG ) const {

    TubeEnds tubeEnds;
    const Identifier& id = mdt.identify();
    Amg::Vector3D lpos = gToSegment*mdt.prepRawData()->globalPosition();

    // use readout and hv side as the surface frame is not that of the chamber
    Amg::Vector3D lropos = gToSegment*mdt.prepRawData()->detectorElement()->ROPos(id);
    Amg::Vector3D lhvpos = lpos + (lpos-lropos);

    // rescale to correctly take into account active tube length
    double tubeLen = (lropos-lhvpos).mag();
    double activeTubeLen = mdt.detectorElement()->getActiveTubeLength(m_idHelperTool->mdtIdHelper().tubeLayer(id),m_idHelperTool->mdtIdHelper().tube(id));
    double scaleFactor = activeTubeLen/tubeLen;
    lropos[0] = scaleFactor*lropos.x();
    lhvpos[0] = scaleFactor*lhvpos.x();

    if( lropos.x() < lhvpos.x() ){
      tubeEnds.lxmin = lropos.x();
      tubeEnds.lxmax = lhvpos.x();
    }else{
      tubeEnds.lxmin = lhvpos.x();
      tubeEnds.lxmax = lropos.x();
    }

    Amg::Vector3D ropos = segmentToG*lropos;
    Amg::Vector3D hvpos = segmentToG*lhvpos;
    double phiRO = ropos.phi();
    double phiHV = hvpos.phi();
    tubeEnds.phimin = phiRO < phiHV ? phiRO : phiHV;
    tubeEnds.phimax = phiRO < phiHV ? phiHV : phiRO;
    return tubeEnds;
  }

  void DCMathSegmentMaker::updatePhiRanges( double phiminNew, double phimaxNew, double& phiminRef, double& phimaxRef ) const {
    
    // check whether we are at the boundary where phi changes sign
    if( phiminRef*phimaxRef < 0. ){
      if( phiminRef < -1.1 ){
	if( phiminRef > phiminNew ) phiminRef = phiminNew;
	if( phimaxRef < phimaxNew ) phimaxRef = phimaxNew;
      }else{
	if( phiminRef < phiminNew ) phiminRef = phiminNew;
	if( phimaxRef > phimaxNew ) phimaxRef = phimaxNew;
      }
    }else{
      // if not life is 'easy'
      if( phiminRef < 0. ){
	if( phiminRef < phiminNew ) phiminRef = phiminNew;
	if( phimaxRef > phimaxNew ) phimaxRef = phimaxNew;
      }else{
	if( phiminRef > phiminNew ) phiminRef = phiminNew;
	if( phimaxRef < phimaxNew ) phimaxRef = phimaxNew;
      }
    }
  }

  bool DCMathSegmentMaker::checkPhiConsistency( double phi ) const {
    // only if assuming pointing phi
    if( !m_assumePointingPhi ) return true;

    bool phiOk = true;
    double offset = 0.05;
    if( m_phimin*m_phimax < 0. ){
      if( phi < 0. ){
	if( phi > -1.1 ){
	  if( phi < m_phimin - offset ) phiOk = false; 
	}else{
	  if( phi > m_phimin + offset ) phiOk = false; 
	}
      }else{
	if( phi > 1.1 ){
	  if( phi < m_phimax - offset ) phiOk = false; 
	}else{
	  if( phi > m_phimax + offset ) phiOk = false; 
	}
      }
    }else{
      if( phi < m_phimin - offset || phi > m_phimax + offset ) phiOk = false;
    }
    return phiOk;
  }


  Amg::Vector3D DCMathSegmentMaker::updateDirection( double linephi, const Trk::PlaneSurface& surf, 
						     const Amg::Vector3D& roaddir, bool isCurvedSegment ) const {

    // Local direction along precision measurement (0, dy, dz)
    Trk::LocalDirection segLocDirs( std::asin(1.), linephi );
    Amg::Vector3D gdirs;
    surf.localToGlobalDirection(segLocDirs,gdirs);
    // Local direction in plane  (1,0,0)       
    Trk::LocalDirection segLocDiro( 0.,std::asin(1.));
    Amg::Vector3D gdiro;
    surf.localToGlobalDirection(segLocDiro,gdiro);

    // recalculate the value of the local XZ angle for the give YZ angle of the segment such that the global phi direction remains unchanged
    double dx = std::sin(gdirs.theta())*std::cos(gdirs.phi()); 
    double dy = std::sin(gdirs.theta())*std::sin(gdirs.phi()); 
    double dz = std::cos(gdirs.theta()); 

    // vector gdiro

    double dxo = std::sin(gdiro.theta())*std::cos(gdiro.phi()); 
    double dyo = std::sin(gdiro.theta())*std::sin(gdiro.phi()); 
    double dzo = std::cos(gdiro.theta()); 

    // solve system real direction = A * gdir + B * gdiro 
    //            phi global  constraint: (1)*sin(phi road) - (2)*cos(phi road) = 0
    //            ( A * dx + B * dxo ) sin (phi ) - (A * dy + B *dyo ) cos (phi) = 0
    //              A ( dx sin - dy cos ) + B (dx0 sin -dy0 cos) = A * a0 + B * b0 = 0
    //              psi = atan (-b0 , a0)  

    double a0 = dx * std::sin (roaddir.phi()) - dy * std::cos(roaddir.phi());
    double b0 = dxo * std::sin (roaddir.phi()) - dyo * std::cos(roaddir.phi());
    if (b0 < 1e-8 && b0 > 0) b0 = 1e-8; 
    if (b0 > -1e-8 && b0 < 0) b0 = -1e-8; 
    double dxn = dx -a0*dxo/b0;
    double dyn = dy -a0*dyo/b0;
    double dzn = dz -a0*dzo/b0;
    double norm = std::sqrt(dxn*dxn+dyn*dyn+dzn*dzn);

    // flip the sign if the direction NOT parallel to road 
    if( m_assumePointingPhi ){
      if (dxn*roaddir.x() + dyn*roaddir.y() + dzn*roaddir.z()< 0.) {
	norm = -norm;
      }     
    }else{
      if (dxn*roaddir.x() + dyn*roaddir.y() < 0.) {
	norm = -norm;
      }
    }

    if(isCurvedSegment) norm = norm/2.;
      

    // 
    // Follow segment fit direction
    //
    dxn = dxn/norm;
    dyn = dyn/norm;
    dzn = dzn/norm;

    return Amg::Vector3D(dxn,dyn,dzn);
  }
}

