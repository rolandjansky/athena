/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecoValidationTool.h"

#include "TFile.h"
#include "TTree.h"

#include "MuonClusterization/TgcHitClustering.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonRecValidationNtuples/MuonBetaCalculationUtils.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkTrack/Track.h"
#include "MuonLayerEvent/MuonCandidate.h"

#define SIG_VEL 4.80000  // ns/m
#define C_VEL   3.33564  // ns/m


namespace Muon {

  MuonRecoValidationTool::MuonRecoValidationTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_segmentHitSummaryTool("Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool"),
      m_hitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
      m_truthSummaryTool("Muon::MuonTruthSummaryTool/MuonTruthSummaryTool"),
      m_extrapolator("Trk::Extrapolation/AtlasExtrapolator"),
      m_matchingTool("MuTagMatchingTool/MuTagMatchingTool"),
      m_hitTimingTool("Muon::MuonHitTimingTool/MuonHitTimingTool"),
      m_incidentSvc("IncidentSvc",n),
      m_candidateCounter(0)
  {
    declareInterface<IMuonRecoValidationTool>(this);
    declareProperty("MuonSegmentHitSummaryTool",m_segmentHitSummaryTool );    
    declareProperty("MuonHitSummaryTool",m_hitSummaryTool );    
    declareProperty("MuonTruthSummaryTool",m_truthSummaryTool );    
    declareProperty("Extrapolator",m_extrapolator );    
    declareProperty("MatchTool",m_matchingTool );    
    declareProperty("MuonHitTimingTool",m_hitTimingTool );    
    declareProperty("IncidentSvc",m_incidentSvc );
    declareProperty("isMC",m_isMC=false);
  }

  StatusCode MuonRecoValidationTool::initialize() {

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_segmentHitSummaryTool.retrieve());
    ATH_CHECK(m_hitSummaryTool.retrieve());
    if(m_isMC){
      ATH_CHECK(m_truthSummaryTool.retrieve());
    }
    else{
      m_truthSummaryTool.disable();
    }
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_matchingTool.retrieve());
    ATH_CHECK(m_hitTimingTool.retrieve());
    ITHistSvc* thistSvc = nullptr;
    ATH_CHECK( service("THistSvc", thistSvc) );

    m_tree = new TTree("data", "RecoValidation");
    ATH_CHECK( thistSvc->regTree("/MuonRecoVal/data", m_tree) );

    m_ntuple.init("",m_tree);

    // call handle in case of EndEvent
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    return StatusCode::SUCCESS;
  }

  void MuonRecoValidationTool::clear() {
    m_candidateCounter = 0;
    m_ntuple.clear();
    m_trackParticles.clear();
    m_trackParticleIndexLookup.clear();
  }

  void MuonRecoValidationTool::handle(const Incident& inc) {
    // Only clear cache for EndEvent incident
    if (inc.type()  == IncidentType::EndEvent){
      ATH_MSG_DEBUG(" clearing cache at end of event " );
      m_tree->Fill();
      clear();
    }  
  }

  bool MuonRecoValidationTool::addTrackParticle(  const xAOD::TrackParticle& indetTrackParticle,
                                                  const MuonSystemExtension& muonSystemExtension ) const {
    
    m_ntuple.trackParticleBlock.pt->push_back(indetTrackParticle.pt());
    m_ntuple.trackParticleBlock.p->push_back(1./indetTrackParticle.qOverP());
    m_ntuple.trackParticleBlock.eta->push_back(indetTrackParticle.eta());
    m_ntuple.trackParticleBlock.phi->push_back(indetTrackParticle.phi());
    int pdg = 0;
    int barcode = -1;
    float beta = 1.;
    // set truth
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    if( indetTrackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink") ) {
      const ElementTruthLink_t link = indetTrackParticle.auxdata<ElementTruthLink_t>("truthParticleLink");
      if( link.isValid() ){
        pdg = (*link)->pdgId();
        barcode = (*link)->barcode();
        beta = (*link)->p4().Beta();
      }
    }
    m_ntuple.trackParticleBlock.truth.fill(pdg,barcode,beta);

    // try to find the pointer of the indetTrackParticle
    bool found = false;
    for(unsigned int index = 0 ;index < m_trackParticles.size(); ++index ){
      if( &indetTrackParticle == m_trackParticles[index] ){
        found = true;
        break;
      }
    }
    if( !found ){
      // set index to the last element
      unsigned int index = m_trackParticles.size();
      m_trackParticles.push_back(&indetTrackParticle);

      const std::vector<Muon::MuonSystemExtension::Intersection>& layerIntersections = muonSystemExtension.layerIntersections();
      ATH_MSG_DEBUG("Adding ID track: pt "  << indetTrackParticle.pt() << " eta " << indetTrackParticle.eta() 
                    << " phi " << indetTrackParticle.phi() << " layers " << layerIntersections.size());

      for( std::vector<Muon::MuonSystemExtension::Intersection>::const_iterator it = layerIntersections.begin();it!=layerIntersections.end();++it ){
        m_trackParticleIndexLookup[it->trackParameters.get()]=index;
      }
    }
    return true;
  }
  
  int MuonRecoValidationTool::getBarcode( const std::set<Identifier>& ids ) const {

    if(!m_isMC) return -1;

    // count how often a barcode occurs 
    std::map<int,int> counters;
    for( std::set<Identifier>::const_iterator it=ids.begin();it!=ids.end();++it ){
      int bc = m_truthSummaryTool->getBarcode(*it);
      if( bc != -1 ) ++counters[bc];
    }
    
    // pick the most frequent
    int barcode = -1;
    int max = -1;
    for( std::map<int,int>::iterator it=counters.begin();it!=counters.end();++it ){
      if( it->second > max ){
        barcode = it->first;
        max = it->second;
      }
    }
    
    return barcode;
  }

  bool MuonRecoValidationTool::addTimeMeasurements( const xAOD::TrackParticle& indetTrackParticle, const MuGirlNS::StauHits& stauHits ) const {

    Muon::MuonBetaCalculationUtils muonBetaCalculationUtils;

    auto pos = std::find(m_trackParticles.begin(),m_trackParticles.end(),&indetTrackParticle);
    if( pos == m_trackParticles.end() ) {
      ATH_MSG_WARNING("addTimeMeasurement: indetTrackParticle not found ");
      return false;
    }
    int index = std::distance(m_trackParticles.begin(),pos);

    for( const auto& stauHit : stauHits ){
      Identifier id = stauHit.id;
      int type = m_idHelperSvc->technologyIndex(id) + 10;
      double r = sqrt(stauHit.x*stauHit.x + stauHit.y*stauHit.y);
      double tof = muonBetaCalculationUtils.calculateTof(1,sqrt(r*r+stauHit.z*stauHit.z));
      // track index 
      m_ntuple.timeBlock.track.fill(index);
    
      // identifier info
      m_ntuple.timeBlock.id.fill(m_idHelperSvc->sector(id),m_idHelperSvc->chamberIndex(id));

      // position + time information
      m_ntuple.timeBlock.fill(type,m_idHelperSvc->gasGapId(id).get_identifier32().get_compact(),r,stauHit.z,stauHit.mToF-tof,stauHit.error,
                              stauHit.propagationTime, stauHit.e, tof, 0., stauHit.shift, 1000 * m_candidateCounter);
      
      // barcode + pdg
      int barcode=-1,pdg=0;
      if(m_isMC){
	barcode = m_truthSummaryTool->getBarcode(id);
	pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
      }
      m_ntuple.timeBlock.truth.fill(pdg,barcode);
    }    
    ++m_candidateCounter;
    return true;
  }

  bool MuonRecoValidationTool::addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Identifier& id,
                                                   const Amg::Vector3D& gpos, float time, float errorTime ) const {

    // track index 
    m_ntuple.timeBlock.track.fill(getIndex(intersection));
    
    // identifier info
    m_ntuple.timeBlock.id.fill(m_idHelperSvc->sector(id),m_idHelperSvc->chamberIndex(id));

    // position information
    m_ntuple.timeBlock.fill(2,m_idHelperSvc->gasGapId(id).get_identifier32().get_compact(),gpos.perp(),gpos.z(),time,errorTime);

    // barcode + pdg
    int barcode=-1,pdg=0;
    if(m_isMC){
      barcode = m_truthSummaryTool->getBarcode(id);
      pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
    }
    m_ntuple.timeBlock.truth.fill(pdg,barcode);
    
    return true;
  }


  bool MuonRecoValidationTool::addTimeMeasurement( const MuonSystemExtension::Intersection& intersection, const Trk::MeasurementBase& meas ) const {

    float segmentTimeCorrection = 0.;
    
    const MuonSegment* seg = dynamic_cast<const MuonSegment*>(&meas);
    if( seg && seg->hasFittedT0() ){

      m_ntuple.timeBlock.track.fill(getIndex(intersection));
    
      Identifier id = m_edmHelperSvc->chamberId(*seg);
      m_ntuple.timeBlock.id.fill(m_idHelperSvc->sector(id),m_idHelperSvc->chamberIndex(id));

      // position information
      m_ntuple.timeBlock.fill(1,m_idHelperSvc->chamberId(id).get_identifier32().get_compact(),seg->globalPosition().perp(),seg->globalPosition().z(),
                              seg->time()-segmentTimeCorrection,seg->errorTime() );

      // barcode + pdg
      std::set<Identifier> ids;
      std::vector<const MuonClusterOnTrack*> clusters;
      extract(*seg,ids,clusters);
      int barcode = getBarcode(ids);
      int pdg=0;
      if(m_isMC) pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
      m_ntuple.timeBlock.truth.fill(pdg,barcode);
      
      return true;
    }

    const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(&meas);
    if( rpc ) {

      m_ntuple.timeBlock.track.fill(getIndex(intersection));

      Identifier id = rpc->identify();
      m_ntuple.timeBlock.id.fill(m_idHelperSvc->sector(id),m_idHelperSvc->chamberIndex(id));


      // barcode + pdg
      int barcode=-1,pdg=0;
      if(m_isMC){
	barcode = m_truthSummaryTool->getBarcode(id);
	pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
      }
      m_ntuple.timeBlock.truth.fill(pdg,barcode);
      
      bool measphi = m_idHelperSvc->measuresPhi(id);
      const Amg::Vector3D& GP = rpc->globalPosition();
      const Muon::RpcPrepData* MClus = rpc->prepRawData();
      const MuonGM::RpcReadoutElement* rpc_readout_element = MClus->detectorElement(); 
      Amg::Vector3D posi          = rpc_readout_element->stripPos(id);
      
      //let's correct rpc time subtracting delay due to the induced electric signal propagation along strip   
      double correct_time_along_strip = 0;
      if(measphi==0){correct_time_along_strip = rpc_readout_element -> distanceToEtaReadout( GP )/1000.* SIG_VEL;}
      else {correct_time_along_strip = rpc_readout_element -> distanceToPhiReadout( GP )/1000.* SIG_VEL;}
      
      //let's evaluate the average  delay due to the induced electric signal propagation along strip 
      double av_correct_time_along_strip = 0;
      if(measphi==0){av_correct_time_along_strip = rpc_readout_element -> distanceToEtaReadout( posi )/1000.* SIG_VEL;}
      else {av_correct_time_along_strip = rpc_readout_element -> distanceToPhiReadout( posi )/1000.* SIG_VEL;}
      
      //let's evaluate [real TOF - nominal TOF]
      double real_TOF_onRPCgap    = GP.mag()  /1000.* C_VEL;
      double nominal_TOF_onRPCgap = posi.mag()/1000.* C_VEL;
      
      //let's evaluate the total time correction
      double correct_time_tot = real_TOF_onRPCgap-nominal_TOF_onRPCgap+correct_time_along_strip-av_correct_time_along_strip;

      // time and position information
      m_ntuple.timeBlock.fill(0,m_idHelperSvc->gasGapId(id).get_identifier32().get_compact(),rpc->globalPosition().perp(),rpc->globalPosition().z(),rpc->time(),2.,
                              correct_time_along_strip,av_correct_time_along_strip,real_TOF_onRPCgap,nominal_TOF_onRPCgap,correct_time_tot );

      return true;
    }
    
    return true;
  }


  bool MuonRecoValidationTool::add( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment, int stage ) const {


    m_ntuple.segmentBlock.stage->push_back(stage);

    Identifier id = m_edmHelperSvc->chamberId(segment);
    m_ntuple.segmentBlock.id.fill(m_idHelperSvc->sector(id),m_idHelperSvc->chamberIndex(id));
    
    // position information
    m_ntuple.segmentBlock.r->push_back(segment.globalPosition().perp());
    m_ntuple.segmentBlock.z->push_back(segment.globalPosition().z());

    // timing information
    float t0      = segment.hasFittedT0() ? segment.time()      : -99999;
    float t0Error = segment.hasFittedT0() ? segment.errorTime() : -99999;
    m_ntuple.segmentBlock.t0->push_back(t0);
    m_ntuple.segmentBlock.t0Error->push_back(t0Error);

    // extract clusters and ids
    std::set<Identifier> ids;
    std::vector<const MuonClusterOnTrack*> clusters;
    extract(segment,ids,clusters);

    IMuonHitTimingTool::TimingResult result = m_hitTimingTool->calculateTimingResult(clusters);
    t0      = result.valid ? result.time  : -99999;
    t0Error = result.valid ? result.error : -99999;
    m_ntuple.segmentBlock.t0Trig->push_back(t0);
    m_ntuple.segmentBlock.t0TrigError->push_back(t0Error);

    // hit counts
    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_segmentHitSummaryTool->getHitCounts(segment);
    m_ntuple.segmentBlock.nmdtHits->push_back(hitCounts.nmdtHitsMl1+hitCounts.nmdtHitsMl2+hitCounts.ncscHitsEta);
    m_ntuple.segmentBlock.ntrigEtaHits->push_back(hitCounts.netaTrigHitLayers);
    m_ntuple.segmentBlock.ntrigPhiHits->push_back(hitCounts.nphiTrigHitLayers);

    // barcode + pdg
    int barcode = getBarcode(ids);
    int pdg=0;
    if(m_isMC) pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
    m_ntuple.segmentBlock.truth.fill(pdg,barcode);

    m_ntuple.segmentBlock.track.fill(getIndex(intersection));

    // extrapolate and create an intersection @ the segment surface. 
    std::shared_ptr<const Trk::TrackParameters> exPars(m_extrapolator->extrapolate(*intersection.trackParameters,segment.associatedSurface(),Trk::anyDirection,false,Trk::muon));
    if( !exPars ){
      ATH_MSG_VERBOSE(" extrapolation failed ");
      m_ntuple.segmentBlock.quality->push_back(-2);
      m_ntuple.segmentBlock.xresiduals.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.yresiduals.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.angleXZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.angleYZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.combinedYZ.fill(0.,1.,0.,1.,-1);
      return false;
    }
    
    
    // cast to AtaPlane so we can get the segment info 
    const Trk::AtaPlane* ataPlane = dynamic_cast<const Trk::AtaPlane*>(exPars.get());
    if(!ataPlane){
      ATH_MSG_WARNING(" dynamic_cast<> failed ");
      m_ntuple.segmentBlock.quality->push_back(-2);
      m_ntuple.segmentBlock.angleXZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.angleYZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.combinedYZ.fill(0.,1.,0.,1.,-1);
      return false;
    }
    MuonCombined::MuonSegmentInfo segmentInfo = m_matchingTool->muTagSegmentInfo( nullptr, &segment, ataPlane );
    m_ntuple.segmentBlock.quality->push_back(segmentInfo.quality);
    m_ntuple.segmentBlock.xresiduals.fillResPull(segmentInfo.resX,segmentInfo.pullX,segmentInfo.segErrorX,segmentInfo.exErrorX,1);
    m_ntuple.segmentBlock.yresiduals.fillResPull(segmentInfo.resY,segmentInfo.pullY,segmentInfo.segErrorY,segmentInfo.exErrorY,1);
    m_ntuple.segmentBlock.angleXZ.fillResPull(segmentInfo.dangleXZ,segmentInfo.pullXZ,segmentInfo.segErrorXZ,segmentInfo.exErrorXZ,1);
    m_ntuple.segmentBlock.angleYZ.fillResPull(segmentInfo.dangleYZ,segmentInfo.pullYZ,segmentInfo.segErrorYZ,segmentInfo.exErrorYZ,1);
    m_ntuple.segmentBlock.combinedYZ.fillResPull(segmentInfo.resCY,segmentInfo.pullCY,1);
 
    ATH_MSG_DEBUG(" Adding Segment to ntuple: stage " << stage);
   
    return true;
  }

  bool MuonRecoValidationTool::add( const MuonSystemExtension::Intersection& intersection,
                                    const MuonHough::MuonLayerHough::Maximum& maximum ) const {

    m_ntuple.houghBlock.maximum->push_back(maximum.max);
    
    m_ntuple.houghBlock.track.fill(getIndex(intersection));

    int sector = -1;
    int chIndex = -1;
    float maxwidth = (maximum.binposmax-maximum.binposmin);
    if( maximum.hough ) {
      maxwidth *= maximum.hough->m_binsize;
      sector   = maximum.hough->m_descriptor.sector;
      chIndex  = maximum.hough->m_descriptor.chIndex;
    }
    m_ntuple.houghBlock.id.fill(sector,chIndex);
    m_ntuple.houghBlock.residuals.fill(maximum.pos,maxwidth,intersection,Trk::loc1);

    // get truth from hits
    std::set<Identifier> ids;

    std::vector<MuonHough::Hit*>::const_iterator hit = maximum.hits.begin();
    std::vector<MuonHough::Hit*>::const_iterator hit_end = maximum.hits.end();
    for( ;hit!=hit_end;++hit ) {
      // treat the case that the hit is a composite TGC hit
      if( (*hit)->tgc ){
        for( const auto& prd : (*hit)->tgc->etaCluster.hitList ) ids.insert(prd->identify());
      }else if( (*hit)->prd ){
        ids.insert((*hit)->prd->identify());
      }
    }
    int barcode=-1,pdg=0;
    if(m_isMC){
      barcode = getBarcode(ids);
      pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
    }
    m_ntuple.houghBlock.truth.fill(pdg,barcode);

    ATH_MSG_DEBUG(" Adding Hough maximum to ntuple ");

    return true;
  }

  bool MuonRecoValidationTool::add( const MuonSystemExtension::Intersection& intersection, const Trk::PrepRawData& prd,
                                    float expos, float expos_err ) const {

    Identifier id = prd.identify();
    m_ntuple.hitBlock.id.fill(m_idHelperSvc->sector(id),m_idHelperSvc->chamberIndex(id));
    m_ntuple.hitBlock.track.fill(getIndex(intersection));
    
    int barcode=-1,pdg=0;
    if(m_isMC){
      barcode = m_truthSummaryTool->getBarcode(id);
      pdg = barcode != -1 ? m_truthSummaryTool->getPdgId(barcode) : 0;
    }
    m_ntuple.hitBlock.truth.fill(pdg,barcode);

    float sign = expos < 0 ? -1. : 1.;
    m_ntuple.hitBlock.residuals.fill(sign*prd.localPosition()[Trk::locX],Amg::error(prd.localCovariance(),Trk::locX),expos,expos_err);

    return true;
  }


  void MuonRecoValidationTool::extract( const MuonSegment& segment, std::set<Identifier>& ids, std::vector<const MuonClusterOnTrack*>& clusters ) const {

    // loop over hits and extract clusters and ids
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = segment.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = segment.containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){
 
      // get Identifier and remove MDT hits
      Identifier id = m_edmHelperSvc->getIdentifier(**mit);
      if( !id.is_valid() ) continue;
      ids.insert(id);
      if( !m_idHelperSvc->isTrigger(id) ) continue;
       
      // cast to  MuonClusterOnTrack
      const MuonClusterOnTrack* clus = dynamic_cast<const MuonClusterOnTrack*>(*mit);
      if( clus ) clusters.push_back(clus);
      else{
        const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*mit);
        if( !crot || crot->containedROTs().empty() ) continue;
        clusters.insert(clusters.end(),crot->containedROTs().begin(),crot->containedROTs().end());
      }
    }
  }

  bool MuonRecoValidationTool::addMuonCandidate( const xAOD::TrackParticle& indetTrackParticle, const MuonCandidate* candidate, 
                                                 Trk::Track* combinedTrack, int ntimes, float beta, float chi2ndof, int stage ) const {

    auto pos = std::find(m_trackParticles.begin(),m_trackParticles.end(),&indetTrackParticle);
    if( pos == m_trackParticles.end() ) {
      ATH_MSG_WARNING("addMuonCandidate: indetTrackParticle not found ");
      return false;
    }
    
    m_ntuple.candidateBlock.track.fill(std::distance(m_trackParticles.begin(),pos));
    int nprec = 0;
    int ntrigPhi = 0;
    int ntrigEta = 0;
    int nseg = candidate ? candidate->layerIntersections.size() : 0;
    const Trk::Track* track=combinedTrack;
    if( combinedTrack ){
      IMuonHitSummaryTool::CompactSummary summary = m_hitSummaryTool->summary(*track);
      nprec = summary.nprecisionLayers;
      ntrigPhi = summary.nphiLayers;  
      ntrigEta = summary.ntrigEtaLayers;  
    }
    m_ntuple.candidateBlock.fill(ntimes,beta,chi2ndof,nseg,nprec,ntrigPhi,ntrigEta,stage);
    return true;
  }


} //end of namespace
