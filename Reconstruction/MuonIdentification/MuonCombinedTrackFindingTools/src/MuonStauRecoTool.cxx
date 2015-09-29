/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonStauRecoTool.h"
#include "MuonRecToolInterfaces/IMuonSystemExtensionTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonClusterization/RpcHitClustering.h"

#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonCombinedToolInterfaces/IMuonLayerSegmentMatchingTool.h"
#include "MuonCombinedToolInterfaces/IMuonLayerAmbiguitySolverTool.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTrackBuilderTool.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"
#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"

#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonLayerEvent/MuonCandidate.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "MuonCombinedEvent/MuGirlTag.h"
#include "MuGirlStau/MuonBetaCalculationUtils.h"

namespace MuonCombined {

  MuonStauRecoTool::MuonStauRecoTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_muonSystemExtentionTool("Muon::MuonSystemExtensionTool/MuonSystemExtensionTool"),
    m_segmentMaker("Muon::DCMathSegmentMaker/DCMathT0FitSegmentMaker"),
    m_segmentMatchingTool("Muon::MuonLayerSegmentMatchingTool/MuonLayerSegmentMatchingTool"),
    m_ambiguityResolver("Muon::MuonLayerAmbiguitySolverTool/MuonLayerAmbiguitySolverTool"),
    m_candidateTrackBuilder("Muon::MuonCandidateTrackBuilderTool/MuonCandidateTrackBuilderTool"),
    m_recoValidationTool("Muon::MuonRecoValidationTool/MuonRecoValidationTool"),
    m_trackFitter("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
    m_trackAmbibuityResolver("Trk::SimpleAmbiguityProcessorTool/MuonAmbiProcessor"),
    m_hitTimingTool("Muon::MuonHitTimingTool/MuonHitTimingTool"),
    m_layerHoughTool("Muon::MuonLayerHoughTool/MuonLayerHoughTool"),
    m_muonPRDSelectionTool("Muon::MuonPRDSelectionTool/MuonPRDSelectionTool")
  {
    declareInterface<IMuonCombinedInDetExtensionTool>(this);

    declareProperty("MuonIdHelperTool",m_idHelper );    
    declareProperty("MuonEDMPrinterTool",m_printer );    
    declareProperty("MuonEDMHelperTool",m_edmHelper );    
    declareProperty("MuonSystemExtensionTool",m_muonSystemExtentionTool );    
    declareProperty("MuonSegmentMaker",m_segmentMaker );    
    declareProperty("MuonLayerSegmentMatchingTool",m_segmentMatchingTool );    
    declareProperty("MuonLayerAmbiguitySolverTool",m_ambiguityResolver );    
    declareProperty("MuonCandidateTrackBuilderTool",m_candidateTrackBuilder );    
    declareProperty("MuonRecoValidationTool",m_recoValidationTool );    
    declareProperty("MuonTrackBuilder",           m_trackFitter );    
    declareProperty("TrackAmbiguityProcessor",m_trackAmbibuityResolver );    
    declareProperty("MuonRecoValidationTool",m_recoValidationTool);
    declareProperty("MuonHitTimingTool",m_hitTimingTool);
    declareProperty("MuonLayerHoughTool",m_layerHoughTool);
    declareProperty("MuonPRDSelectionTool", m_muonPRDSelectionTool );
  }

  MuonStauRecoTool::~MuonStauRecoTool() { }

  StatusCode MuonStauRecoTool::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode MuonStauRecoTool::initialize() {

    ATH_CHECK(m_idHelper.retrieve());    
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    ATH_CHECK(m_muonSystemExtentionTool.retrieve());
    ATH_CHECK(m_segmentMaker.retrieve());
    ATH_CHECK(m_segmentMatchingTool.retrieve());
    ATH_CHECK(m_ambiguityResolver.retrieve());
    ATH_CHECK(m_candidateTrackBuilder.retrieve());
    ATH_CHECK(m_recoValidationTool.retrieve());
    ATH_CHECK(m_trackFitter.retrieve());
    ATH_CHECK(m_trackAmbibuityResolver.retrieve());
    ATH_CHECK(m_hitTimingTool.retrieve());
    ATH_CHECK(m_layerHoughTool.retrieve());
    ATH_CHECK(m_muonPRDSelectionTool.retrieve());
    
    return StatusCode::SUCCESS;
  }

  void MuonStauRecoTool::extend( const InDetCandidateCollection& inDetCandidates ) const {
    ATH_MSG_DEBUG(" extending " << inDetCandidates.size() );

    InDetCandidateCollection::const_iterator it = inDetCandidates.begin();
    InDetCandidateCollection::const_iterator it_end = inDetCandidates.end();
    for( ; it!=it_end;++it ){
      handleCandidate( **it );
    }
  }
  
  float speedOfLight(299.792458);
  float inverseSpeedOfLight(1./speedOfLight);

  float calculateBeta( float time, const Amg::Vector3D& position ) {
    float dist = position.mag();
    float tottime = time + dist*inverseSpeedOfLight;
    float v = dist/tottime;
    float beta = v*inverseSpeedOfLight;
    return beta;
  }


  void MuonStauRecoTool::handleCandidate( const InDetCandidate& indetCandidate ) const {
    
    const xAOD::TrackParticle& indetTrackParticle = indetCandidate.indetTrackParticle();
    if( !indetTrackParticle.track() || indetTrackParticle.pt() < 10000. ) return;

    float betaTrack = indetTrackParticle.e()/sqrt(indetTrackParticle.e()*indetTrackParticle.e()+431000.*431000.);
    
    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << " ID track: pt "  << indetTrackParticle.pt() << " eta " << indetTrackParticle.eta() 
                      << " phi " << indetTrackParticle.phi() << " beta " << betaTrack;
      msg(MSG::DEBUG) << endreq;
    }
    
    // get intersections which precision layers in the muon system 
    const Muon::MuonSystemExtension* muonSystemExtension = 0;
    m_muonSystemExtentionTool->muonSystemExtension( indetTrackParticle, muonSystemExtension );
    if( !muonSystemExtension ) {
      //ATH_MSG_DEBUG("No MuonSystemExtension, aborting ");
      return;
    }
    
    // fill validation content
    if( !m_recoValidationTool.empty() ) m_recoValidationTool->addTrackParticle( indetTrackParticle, *muonSystemExtension );
    
    // get intersections
    const std::vector<Muon::MuonSystemExtension::Intersection>& layerIntersections = muonSystemExtension->layerIntersections();

    // loop over intersections, get associated data
    AssociatedData associatedData;

    // extract time measurements for intersection
    extractTimeMeasurements(*muonSystemExtension,associatedData);


    // build candidates
    CandidateVec candidates;
    createCandidates(associatedData,candidates);

    // TODO add ncandidates to the ntuple + best beta fit (first candidate for now)
    if( candidates.empty() ){
      ATH_MSG_DEBUG(" No candidates found for ID track particle ");
      return;
    }
    
    // refine: find segments using seed beta
    refineCandidates(candidates);


  }


  void MuonStauRecoTool::refineCandidates( MuonStauRecoTool::CandidateVec& candidates ) const {
    
    // loop over candidates and redo segments using beta estimate from candidate
  }

  void MuonStauRecoTool::createCandidates( const MuonStauRecoTool::AssociatedData& associatedData, 
                                           MuonStauRecoTool::CandidateVec& candidates ) const {

    // return if no layers were associated 
    if( associatedData.layerData.empty() ) return;

    // loop over layers and select seed maxima
    std::vector<const MaximumData*> seedMaximumDataVec;
    std::vector< LayerData >::const_iterator it=associatedData.layerData.begin();
    std::vector< LayerData >::const_iterator it_end=associatedData.layerData.end();
    for( ;it!=it_end;++it){
      
      // loop over maximumDataVec 
      for( const auto& maximumData : it->maximumDataVec ){
        // add all maximumData that have a time measurement 
        if( !maximumData.betaSeeds.empty() ) seedMaximumDataVec.push_back(&maximumData);
      }
    }
    
    if( seedMaximumDataVec.empty() ) {
      return;
    }
    // sorting lambda for MaximumData seeds
    auto SortMaximumDataVec = []( const MaximumData* max1, const MaximumData* max2 ){
      return max1->maximum->max < max2->maximum->max;
    };
    std::stable_sort(seedMaximumDataVec.begin(),seedMaximumDataVec.end(),SortMaximumDataVec);
    
    // ATH_MSG_DEBUG("Creating candidates from " << associatedData.layerData.size() << " layers, seeds  " << seedMaximumDataVec.size());


    // loop over seeds and create candidates
    Muon::TimePointBetaFit fitter;
    std::set<const MaximumData*> usedMaximumData;
    std::vector<const MaximumData*>::iterator sit = seedMaximumDataVec.begin();
    std::vector<const MaximumData*>::iterator sit_end = seedMaximumDataVec.end();
    for( ;sit!=sit_end;++sit ){
      // only use once
      if( usedMaximumData.count(*sit) ) continue;
      usedMaximumData.insert(*sit);      

      // collect maximumData
      std::vector< const MaximumData* > maximumDataVec;
      maximumDataVec.push_back(*sit);

      // get seed beta, for now assume there is only one
      Muon::TimePointBetaFit::HitVec hits;
      extractTimeHits(**sit,hits,&(*sit)->betaSeeds.front());

      // associated layers
      std::vector< LayerData >::const_iterator it=associatedData.layerData.begin();
      std::vector< LayerData >::const_iterator it_end=associatedData.layerData.end();
      for( ;it!=it_end;++it){

        // skip if same layer
        if( it->intersection.layerSurface.sector == (*sit)->intersection.layerSurface.sector && 
            it->intersection.layerSurface.regionIndex == (*sit)->intersection.layerSurface.regionIndex &&
            it->intersection.layerSurface.layerIndex == (*sit)->intersection.layerSurface.layerIndex ) continue;
        
        // loop over maximumDataVec 
        for( const auto& maximumData : it->maximumDataVec ){
          unsigned int prevSize = hits.size();
          extractTimeHits(maximumData,hits,&(*sit)->betaSeeds.front());
          if( prevSize != hits.size() ) {
            maximumDataVec.push_back(&maximumData);
            usedMaximumData.insert(&maximumData);
          }
        }
      }
      Muon::TimePointBetaFit::FitResult result = fitter.fitWithOutlierLogic(hits);
      float chi2ndof = (result.status != 0 && result.ndof != 0) ? result.chi2/result.ndof : 0;
      ATH_MSG_DEBUG(" New candidate: time measurements " << hits.size() << " status " << result.status 
                    << " beta " << result.beta << " chi2/ndof " << chi2ndof );
      Candidate candidate(nullptr,std::move(maximumDataVec));
      candidate.betaSeed = result.status != 0 ? result.beta : (*sit)->betaSeeds.front().beta;
      candidates.push_back(candidate);
      
    }
  }

  void MuonStauRecoTool::extractTimeMeasurements(  const Muon::MuonSystemExtension& muonSystemExtension, 
                                                   MuonStauRecoTool::AssociatedData& associatedData ) const {

    // get layer intersections
    const std::vector<Muon::MuonSystemExtension::Intersection>& layerIntersections = muonSystemExtension.layerIntersections();

    // find RPC time measurements and segments to seed the beta fit using t0 fitting
    for( std::vector<Muon::MuonSystemExtension::Intersection>::const_iterator it = layerIntersections.begin();it!=layerIntersections.end();++it ){

      // create layer data object and add maxima
      LayerData layerData(*it);
      associateHoughMaxima(layerData);

      // skip layer of not maxima are associated 
      if( layerData.maximumDataVec.empty() ) continue;
      
      associatedData.layerData.push_back(layerData);

      // loop over associated maxima
      for( auto& maximum : layerData.maximumDataVec ){
      
        // extract RPC timing
        extractRpcTimingFromMaximum( *it, maximum );

        // find segments for intersection
        std::vector< std::shared_ptr<const Muon::MuonSegment> > t0fittedSegments;
        findT0Segments( *it, maximum, t0fittedSegments );
        if( t0fittedSegments.empty() ) continue;

        // fill validation content
        if( !m_recoValidationTool.empty() ) {
          for( const auto& seg : t0fittedSegments ) m_recoValidationTool->add(*it,*seg,0);
        }

        // match segments to intersection, store the ones that match 
        m_segmentMatchingTool->select( *it, t0fittedSegments, maximum.t0fittedSegments );
      
        // get beta seeds for Maximum
        getBetaSeeds(maximum);

        // fill validation content
        if( !m_recoValidationTool.empty() ) {
          for( const auto& seg : maximum.t0fittedSegments ) {
            if( seg->hasFittedT0() ) m_recoValidationTool->addTimeMeasurement(*it,*seg);
            m_recoValidationTool->add(*it,*seg,1);
          }
        }    
      } 
    }
  }

  void MuonStauRecoTool::extractTimeHits( const MaximumData& maximumData, Muon::TimePointBetaFit::HitVec& hits,
                                          const BetaSeed* seed ) const {

    Muon::MuonBetaCalculationUtils muonBetaCalculationUtils;

    auto addHit = [&]( float distance, float time, float error ){
      if( seed ){
        float beta = muonBetaCalculationUtils.calculateBeta(time,distance);
        if( fabs(beta-seed->beta) > 0.2 ) return;
      }
      hits.push_back(Muon::TimePointBetaFit::Hit(distance,time,error));
    };

    // add rpc measurements
    for( const auto& rpc : maximumData.rpcTimeMeasurements ){
      addHit(rpc.rpcClusters.front()->globalPosition().mag(),rpc.time,rpc.error); 
    }
    
    // add segment t0 fits
    for( const auto& seg : maximumData.t0fittedSegments ){
      addHit(seg->globalPosition().mag(),seg->time(),seg->errorTime());
    }
  }

  void MuonStauRecoTool::getBetaSeeds( MaximumData& maximumData ) const {
    
    // skip maximumData if no timing information is available
    if( maximumData.rpcTimeMeasurements.empty() && maximumData.t0fittedSegments.empty() ) return;

    // fitter + hits
    Muon::TimePointBetaFit fitter;
    Muon::TimePointBetaFit::HitVec hits;
    extractTimeHits(maximumData,hits);
    
    Muon::TimePointBetaFit::FitResult result = fitter.fitWithOutlierLogic(hits);
    float chi2ndof = (result.status != 0 && result.ndof != 0) ? result.chi2/result.ndof : 0;

    ATH_MSG_DEBUG(" fitting beta for maximum: time measurements " << hits.size() << " status " << result.status 
                  << " beta " << result.beta << " chi2/ndof " << chi2ndof );
    if( result.status != 0 ) maximumData.betaSeeds.push_back( BetaSeed(result.beta,1.));

  }


  void MuonStauRecoTool::findT0Segments(  const Muon::MuonSystemExtension::Intersection& intersection, 
                                          MaximumData& maximumData, 
                                          std::vector< std::shared_ptr<const Muon::MuonSegment> >& t0fittedSegments ) const {

    const MuonHough::MuonLayerHough::Maximum& maximum = *maximumData.maximum;

    // lambda to handle calibration and selection of MDTs
    auto handleMdt = [this,intersection](const Muon::MdtPrepData& prd, std::vector<const Muon::MdtDriftCircleOnTrack*>& mdts){
      const Muon::MdtDriftCircleOnTrack* mdt = m_muonPRDSelectionTool->calibrateAndSelect( intersection, prd );
      if( mdt ) mdts.push_back(mdt);
    };


    // lambda to handle calibration and selection of clusters
    auto handleCluster = [this,intersection](const Muon::MuonCluster& prd, std::vector<const Muon::MuonClusterOnTrack*>& clusters){
      const Muon::MuonClusterOnTrack* cluster = m_muonPRDSelectionTool->calibrateAndSelect( intersection, prd );
      if( cluster ) clusters.push_back(cluster);
    };

    // loop over hits in maximum and add them to the hit list
    std::vector<const Muon::MdtDriftCircleOnTrack*> mdts;
    std::vector<const Muon::MuonClusterOnTrack*>    clusters;
    std::vector<MuonHough::Hit*>::const_iterator hit = maximum.hits.begin();
    std::vector<MuonHough::Hit*>::const_iterator hit_end = maximum.hits.end();
    for( ;hit!=hit_end;++hit ) {
      
      // treat the case that the hit is a composite TGC hit
      if( (*hit)->tgc ){
        for( const auto& prd : (*hit)->tgc->etaCluster.hitList ) handleCluster(*prd,clusters);
      }else if( (*hit)->prd ){
        Identifier id = (*hit)->prd->identify();
        if( m_idHelper->isMdt(id) ) handleMdt( static_cast<const Muon::MdtPrepData&>(*(*hit)->prd),mdts);
        else                        handleCluster( static_cast<const Muon::MuonCluster&>(*(*hit)->prd),clusters);
      }
    }

    // require at least 2 MDT hits
    if( mdts.size() > 2 ){

      // run segment finder
      std::unique_ptr<std::vector<const Muon::MuonSegment*> > foundSegments(m_segmentMaker->find( intersection.trackParameters->position(),intersection.trackParameters->momentum(),
                                                                                                  mdts, clusters,
                                                                                                  !clusters.empty(), intersection.trackParameters->momentum().mag() ));
      if( foundSegments ){
        for( auto seg : *foundSegments ){
          ATH_MSG_DEBUG( " " << m_printer->print(*seg) );
          t0fittedSegments.push_back( std::shared_ptr<const Muon::MuonSegment>(seg) );
        }
      }
    }
    // clean-up memory
    for( auto hit : mdts )     delete hit;
    for( auto hit : clusters ) delete hit;

  }


  void MuonStauRecoTool::extractRpcTimingFromMaximum( const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData ) const {
    
    const MuonHough::MuonLayerHough::Maximum& maximum = *maximumData.maximum;

    // extract trigger hits per chamber
    std::map<Identifier, std::vector<const Muon::RpcPrepData*> > rpcPrdsPerChamber;
    std::vector<MuonHough::Hit*>::const_iterator hit = maximum.hits.begin();
    std::vector<MuonHough::Hit*>::const_iterator hit_end = maximum.hits.end();
    for( ;hit!=hit_end;++hit ) {
      
      // store all RPCs 
      if( (*hit)->tgc || !(*hit)->prd || m_idHelper->isMdt((*hit)->prd->identify()) ) continue;
      const Muon::RpcPrepData* rpcPrd = dynamic_cast<const Muon::RpcPrepData*>((*hit)->prd);
      if( rpcPrd ) {
        Identifier chamberId = m_idHelper->chamberId(rpcPrd->identify());
        rpcPrdsPerChamber[chamberId].push_back(rpcPrd);
      }
    }
    // exit if no hits are found
    if( rpcPrdsPerChamber.empty() ) return;

    std::map<Identifier, std::vector<const Muon::RpcPrepData*> >::iterator chit = rpcPrdsPerChamber.begin();
    std::map<Identifier, std::vector<const Muon::RpcPrepData*> >::iterator chit_end = rpcPrdsPerChamber.end();
    for( ;chit!=chit_end;++chit ){
      // cluster hits
      Muon::RpcHitClusteringObj clustering(m_idHelper->rpcIdHelper());
      clustering.debug = true;
      if( !clustering.cluster( chit->second ) ){
        ATH_MSG_WARNING("Clustering failed");
        return;
      }

      ATH_MSG_DEBUG(" " << m_idHelper->toStringChamber(chit->first) << " clustered RPCs: " << chit->second.size() << " clusters " << clustering.clustersEta.size() );
      for( auto& cluster : clustering.clustersEta ){
        if( cluster.hitList.empty() || !cluster.hitList.front() ){
          ATH_MSG_WARNING("Cluster without hits: " << cluster.hitList.size());
          continue;
        }
        ATH_MSG_DEBUG("  new cluster: " << m_idHelper->toString(cluster.hitList.front()->identify()) << " size " << cluster.hitList.size() );
        
        std::vector<const Muon::MuonClusterOnTrack*> clusters;
        for( auto rpc : cluster.hitList ){
          const Muon::MuonClusterOnTrack* rot(m_muonPRDSelectionTool->calibrateAndSelect( intersection, *rpc ));
          if( rot ) {
            clusters.push_back(rot);
            ATH_MSG_DEBUG("   strip " << m_idHelper->toString(rot->identify()) << " time " << static_cast<const Muon::RpcClusterOnTrack*>(rot)->time());
          }
        }
        Muon::IMuonHitTimingTool::TimingResult result = m_hitTimingTool->calculateTimingResult(clusters);
        if( result.valid ){
          m_recoValidationTool->addTimeMeasurement(intersection, cluster.hitList.front()->identify(),cluster.hitList.front()->globalPosition(),
                                                   result.time, result.error );
          RpcTimeMeasurement rpcTimeMeasurement;
          rpcTimeMeasurement.time  = result.time;
          rpcTimeMeasurement.error = result.error;
          for( auto cl : clusters ) {
            const Muon::RpcClusterOnTrack* rcl = dynamic_cast<const Muon::RpcClusterOnTrack*>(cl);
            if( rcl ) rpcTimeMeasurement.rpcClusters.push_back( std::shared_ptr<const Muon::RpcClusterOnTrack>(rcl) );
          }
          maximumData.rpcTimeMeasurements.push_back(rpcTimeMeasurement);
        }else{
          for( auto cl : clusters ) delete cl;
        }
      }
    }
      // const Muon::MuonClusterOnTrack* cluster(m_muonPRDSelectionTool->calibrateAndSelect( intersection, *static_cast<const Muon::MuonCluster*>((*hit)->prd) ));
      // if( cluster ) intersection,*cluster);
      // delete cluster;

  }


  void MuonStauRecoTool::associateHoughMaxima( MuonStauRecoTool::LayerData& layerData ) const {
    
    // get intersection and layer identifiers
    const Muon::MuonSystemExtension::Intersection& intersection = layerData.intersection;
    int sector = intersection.layerSurface.sector;
    Muon::MuonStationIndex::DetectorRegionIndex regionIndex = intersection.layerSurface.regionIndex;
    Muon::MuonStationIndex::LayerIndex  layerIndex  = intersection.layerSurface.layerIndex;

    // sanity check
    if( static_cast<int>(m_layerHoughTool->houghData().size()) <= sector-1 ){
      ATH_MSG_WARNING( " sector " << sector << " larger than the available sectors in the Hough tool: " << m_layerHoughTool->houghData().size() );
      return;
    }

    // get hough maxima in the layer
    unsigned int sectorLayerHash = Muon::MuonStationIndex::sectorLayerHash( regionIndex,layerIndex );
    const Muon::MuonLayerHoughTool::HoughDataPerSector& houghDataPerSector = m_layerHoughTool->houghData()[sector-1];

    // sanity check
    if( houghDataPerSector.maxVec.size() <= sectorLayerHash ){
      ATH_MSG_WARNING(" houghDataPerSector.maxVec.size() smaller than hash " << houghDataPerSector.maxVec.size() << " hash " << sectorLayerHash );
      return;
    }
    const Muon::MuonLayerHoughTool::MaximumVec& maxVec = houghDataPerSector.maxVec[sectorLayerHash];
    if( maxVec.empty() ) return;

    
    // get local coordinates in the layer frame
    bool barrelLike = intersection.layerSurface.regionIndex == Muon::MuonStationIndex::Barrel;

    // in the endcaps take the r in the sector frame from the local position of the extrapolation
    float phi = intersection.trackParameters->position().phi();
    float r = barrelLike ?
        m_muonSectorMapping.transformRToSector(intersection.trackParameters->position().perp(),phi, intersection.layerSurface.sector,true) :
        intersection.trackParameters->parameters()[Trk::locX];

    float z = intersection.trackParameters->position().z();
    float errx = Amg::error(*intersection.trackParameters->covariance(),Trk::locX);
    float x = barrelLike ? z : r;
    float y = barrelLike ? r : z;
    float theta = atan2(y,x);

    ATH_MSG_DEBUG(" associateHoughMaxima: sector " << sector << " " << Muon::MuonStationIndex::regionName(regionIndex)
                  << " " << Muon::MuonStationIndex::layerName(layerIndex) << " maxima " << maxVec.size() 
                  << " x,y=(" << x << "," << y
                  << ") errorx " << errx << " " << " angle " << theta );

    // loop over maxima and associate them to the extrapolation
    Muon::MuonLayerHoughTool::MaximumVec::const_iterator mit = maxVec.begin();
    Muon::MuonLayerHoughTool::MaximumVec::const_iterator mit_end = maxVec.end();
    for( ;mit!=mit_end;++mit){
      const MuonHough::MuonLayerHough::Maximum& maximum = **mit;
      float residual = maximum.pos - x;
      float residualTheta = maximum.theta - theta;
      float refPos = (maximum.hough != nullptr) ? maximum.hough->m_descriptor.referencePosition : 0;
      float maxwidth = (maximum.binposmax-maximum.binposmin);
      if( maximum.hough ) maxwidth *= maximum.hough->m_binsize;

      float pull = residual/sqrt(errx*errx+maxwidth*maxwidth/12.);

      ATH_MSG_DEBUG("   Hough maximum " << maximum.max << " position (" << refPos
                    << "," << maximum.pos << ") residual " << residual << " pull " << pull
                    << " angle " << maximum.theta << " residual " << residualTheta );
      
      // fill validation content
      if( !m_recoValidationTool.empty() ) m_recoValidationTool->add( intersection, maximum );

      // select maximum and add it to LayerData
      if( std::abs(pull) > 5 ) continue;
      layerData.maximumDataVec.push_back(MaximumData(intersection,&maximum));
      
    }

  }

  

}
 
