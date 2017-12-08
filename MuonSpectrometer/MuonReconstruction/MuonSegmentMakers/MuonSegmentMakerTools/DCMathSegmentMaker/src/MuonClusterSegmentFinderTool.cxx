/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterSegmentFinderTool.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/MuonPadDesign.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon {

  MuonClusterSegmentFinderTool::MuonClusterSegmentFinderTool (const std::string& type, const std::string& name,
				      const IInterface* parent)
    :
    AthAlgTool(type, name, parent),
    m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    m_ambiTool("Trk::SimpleAmbiguityProcessorTool/MuonAmbiProcessor"),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_storeGate(0) {

    declareInterface<IMuonClusterSegmentFinderTool>(this);

    declareProperty("SLFitter",            m_slTrackFitter);
    declareProperty("SegmentAmbiguityTool",m_ambiTool);
    declareProperty("TrackToSegmentTool",  m_trackToSegmentTool);
    declareProperty("IdHelper",            m_idHelperTool);
    declareProperty("TrackCleaner",        m_trackCleaner);
    //
    declareProperty("ClusterDistance", m_maxClustDist=5);
  }
    
  StatusCode MuonClusterSegmentFinderTool::initialize() {
    if( service("StoreGateSvc", m_storeGate).isFailure()) {
      ATH_MSG_ERROR ( "Unable to retrieve pointer to StoreGateSvc" );
      return StatusCode::FAILURE;
    }
    StoreGateSvc* detStore = 0;
    if( service("DetectorStore", detStore).isFailure()) {
      ATH_MSG_FATAL ( "DetectorStore service not found !" );
      return StatusCode::FAILURE;
    }   
    if( m_slTrackFitter.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_slTrackFitter );
      return StatusCode::FAILURE;
    }   
    if( m_printer.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_printer );
      return StatusCode::FAILURE;
    }   
    if( m_helper.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_helper );
      return StatusCode::FAILURE;
    }   
    if( m_ambiTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_ambiTool );
      return StatusCode::FAILURE;
    }
    if(m_trackToSegmentTool.retrieve().isFailure()) {
      ATH_MSG_FATAL ( "Could not retrieve " << m_trackToSegmentTool << ". Aborting ..." );
      return StatusCode::FAILURE;
    }
    if( m_idHelperTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_idHelperTool );
      return StatusCode::FAILURE;
    }       
    if (m_trackCleaner.retrieve().isFailure()) { 
      msg(MSG::FATAL) << "Could not get " << m_trackCleaner << endmsg;
      return StatusCode::FAILURE; 
    }
    ATH_MSG_DEBUG(" Max cut " << m_maxClustDist );
    return StatusCode::SUCCESS;
  }
  
  MuonClusterSegmentFinderTool::~MuonClusterSegmentFinderTool() {
    
  }

  std::vector<const Muon::MuonSegment*>* MuonClusterSegmentFinderTool::find(std::vector< const Muon::MuonClusterOnTrack* >& muonClusters) {
    ATH_MSG_DEBUG("Entering MuonClusterSegmentFinderTool with " << muonClusters.size() << " clusters to be fit" );
    if(muonClusters.size() < 4) return 0;
    std::vector<const Muon::MuonSegment*>* segs = findPrecisionSegments(muonClusters);
    if(segs == 0) return 0;
    if(segs->size() == 0)
    { 
      delete segs;
      return 0;
    }
    std::vector<const Muon::MuonSegment*>* segments = find3DSegments(muonClusters,segs);

    
    return segments;
  }

  StatusCode MuonClusterSegmentFinderTool::finalize() {
    return StatusCode::SUCCESS;
  }

  //find the precision (eta) segments
  std::vector<const Muon::MuonSegment*>* MuonClusterSegmentFinderTool::findPrecisionSegments(std::vector< const Muon::MuonClusterOnTrack* >& muonClusters) const {

    //clean the muon clusters -- select only the eta hits
    bool selectPhiHits(false);
    std::vector< const Muon::MuonClusterOnTrack* > clusters = cleanClusters(muonClusters,selectPhiHits);
    ATH_MSG_VERBOSE("After hit cleaning, there are " << clusters.size() << " clusters to be fit" );
    if(clusters.size() < 4){
      return 0;
    }
    
    TrackCollection* segTrkColl = new TrackCollection;
    //std::vector< const Muon::MuonClusterOnTrack* > clusters = muonClusters;
    //order the muon clusters by layer
    std::vector< std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters = orderByLayer(clusters);
    
    //create segment seeds
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds = segmentSeed(orderedClusters,false);
    ATH_MSG_VERBOSE("Found " << seeds.size() << " seeds to be fit" );
    
    //find all clusters near the seed and try to fit
    for(unsigned int i=0; i<seeds.size(); ++i) {
      std::vector< const Muon::MuonClusterOnTrack* > rioVec = getClustersOnSegment(orderedClusters,seeds[i]);
      if(rioVec.size() <= 6) continue;
      int nMM = 0;
      for(unsigned int k=0; k<rioVec.size(); ++k) {
        if(m_idHelperTool->isMM(rioVec[k]->identify())) nMM++;
      }

      //fit the segment!!
      std::vector<const Trk::MeasurementBase*> vec2;
      vec2.reserve(rioVec.size()+2);
      //create a psuedo phi hit
      double errPos = 0.1;
      if(nMM>0) errPos = 1000.;
      Amg::MatrixX cov(1,1);
      cov(0,0) = errPos*errPos;
      Trk::PseudoMeasurementOnTrack* pseudoSegPhi1 = new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters( Trk::DefinedParameter(0,Trk::locY) ),
										       cov,rioVec.front()->associatedSurface());
      Trk::PseudoMeasurementOnTrack* pseudoSegPhi2 = new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters( Trk::DefinedParameter(0,Trk::locY) ),
										       cov,rioVec.back()->associatedSurface());
      vec2.push_back(pseudoSegPhi1);
      std::copy( rioVec.begin(), rioVec.end(), std::back_inserter(vec2) );
      vec2.push_back(pseudoSegPhi2);      
      ATH_MSG_VERBOSE("fitting track with " << vec2.size() << " hits" );
      for(unsigned int k=0; k<rioVec.size(); ++k) {
	ATH_MSG_VERBOSE( m_idHelperTool->toString(rioVec[k]->identify()) << " " << rioVec[k]->globalPosition().perp() << ", " << rioVec[k]->globalPosition().z() 
		      << "  " << m_idHelperTool->measuresPhi(rioVec[k]->identify())  );
      }

      Trk::TrackParameters* startpar = 0;

      const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&rioVec.front()->associatedSurface());
      if( surf ){
	Amg::Vector3D posOnSurf = intersectPlane( *surf, seeds[i].first, seeds[i].second );
	Amg::Vector2D lpos;
	surf->globalToLocal(posOnSurf,posOnSurf,lpos);
	Trk::LocalDirection ldir;
	surf->globalToLocalDirection(seeds[i].second,ldir);
	Amg::Vector2D lp( lpos[Trk::locX], 0. );
	Trk::LocalDirection ld( ldir.angleXZ(), std::asin(1.) );
	Amg::Vector3D gd;
	surf->localToGlobalDirection(ld,gd);
	Amg::Vector3D gp;
	surf->localToGlobal(lp,gp,gp);

	Amg::Vector3D perpos = gp + -10*gd.unit();
	if(perpos.dot(gd) < 0) gd = -1*gd;
	startpar = new Trk::Perigee(perpos, gd, 0, perpos);
	ATH_MSG_VERBOSE(" start parameter " << perpos << " pp " << startpar->position() << " gd " << gd.unit() << " pp " << startpar->momentum().unit() );
      }
      // old method as fall back
      if( !startpar ){
	double phi = seeds[i].second.phi();
	double theta = seeds[i].second.theta();
	double qoverp=0;
	Trk::PerigeeSurface persurf(vec2.front()->globalPosition());
	startpar = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);
      }
      Trk::Track* segtrack = fit(vec2,*startpar);
      delete startpar;
      if(segtrack) {
        segTrkColl->push_back( segtrack );

        // Peter NEW calculate distance of segment to all muon clusters
        if( msgLvl(MSG::VERBOSE) ) {
          Amg::Vector3D  pos = segtrack->perigeeParameters()->position();
          Amg::Vector3D  dir = segtrack->perigeeParameters()->momentum();
          dir = dir / dir.mag();
          ATH_MSG_VERBOSE(" 2D eta segment found position " << pos << " direction " << dir << " hits " << vec2.size());
          for(unsigned int k=0; k<muonClusters.size(); ++k) {
             const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*> (&(muonClusters[k]->associatedSurface()));
             if(surf) {
               Amg::Vector3D posOnSurf = intersectPlane( *surf, pos, dir );
               Amg::Vector2D lpos;
               surf->globalToLocal(posOnSurf,posOnSurf,lpos);
               double residual = muonClusters[k]->localParameters()[Trk::locX] - lpos[0];
               double error = Amg::error(muonClusters[k]->localCovariance(),Trk::locX);
               double pull = residual / error;
               ATH_MSG_VERBOSE(" lay " << k << " residual " << residual  << " error  " << error
                               << " pull " << pull << "  " << m_idHelperTool->toString(muonClusters[k]->identify()) );
             }
          }
        }

      }
      else ATH_MSG_DEBUG( "segment fit failed" );
      //delete the pseudo hits
      delete pseudoSegPhi1;
      delete pseudoSegPhi2;
    }

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << "Tracks before ambi solving " << std::endl;
      
      for(TrackCollection::const_iterator it=segTrkColl->begin(); it!=segTrkColl->end(); ++it) {
	msg(MSG::DEBUG) << m_printer->print(**it) << std::endl;
	const DataVector<const Trk::MeasurementBase>* measu = (*it)->measurementsOnTrack();
	if( measu ) msg(MSG::DEBUG) << m_printer->print( measu->stdcont() ) << std::endl;
      }
      msg(MSG::DEBUG) << endmsg;
    }

    if( segTrkColl->empty() ){
      delete segTrkColl;
      return 0;
    }

    TrackCollection* resolvedTracks = m_ambiTool->process(segTrkColl);
    ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size() );
      //store the resolved segments

    std::vector<const Muon::MuonSegment*>* segments = new std::vector<const Muon::MuonSegment*>;
    for(TrackCollection::const_iterator it=resolvedTracks->begin(); it!=resolvedTracks->end(); ++it) {
      MuonSegment* seg = m_trackToSegmentTool->convert( **it );
      if( !seg ) {
        ATH_MSG_VERBOSE("Segment conversion failed, no segment created. ");
      }
      else {
        ATH_MSG_DEBUG(" adding " << m_printer->print(*seg) << std::endl << m_printer->print( seg->containedMeasurements() ) );
        segments->push_back(seg);
      }
    }
    
    //memory cleanup
    delete resolvedTracks;
    delete segTrkColl;

    return segments;
  }

  std::vector<const Muon::MuonSegment*>* MuonClusterSegmentFinderTool::find3DSegments(std::vector< const Muon::MuonClusterOnTrack* >& muonClusters, 
										      std::vector<const Muon::MuonSegment*>* etaSegs) const {
    bool selectPhiHits(true);
    std::vector< const Muon::MuonClusterOnTrack* > clusters = cleanClusters(muonClusters,selectPhiHits);
    ATH_MSG_DEBUG("After hit cleaning, there are " << clusters.size() << " clusters to be fit" );
    if(clusters.size() < 4) {
      ATH_MSG_DEBUG("Not enough phi hits present, cannot perform the fit!");
      return etaSegs;
    }
    std::vector<const Muon::MuonSegment*>* segments = new std::vector<const Muon::MuonSegment*>;
    TrackCollection* segTrkColl = new TrackCollection;
    //order the clusters by layer
    bool useWires(true);
    std::vector< std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters = orderByLayer(clusters,useWires);    
    if(orderedClusters.size() == 0) {
      ATH_MSG_DEBUG( "No phi wire hits found ... moving to pads" );
      useWires = false;
      orderedClusters = orderByLayer(clusters,useWires);
    }
    //get the segment seeds
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds;    
    if(useWires) {
      seeds = segmentSeed(orderedClusters,true);
      ATH_MSG_DEBUG("Found " << seeds.size() << " seeds for phi direction" );
    }
    //loop on the seeds and combine with the eta segments
    for(std::vector<const Muon::MuonSegment*>::const_iterator sit=etaSegs->begin(); sit!=etaSegs->end(); ++sit) {
      bool is3Dseg(false);
      if(!useWires) seeds = segmentSeedFromPads(orderedClusters,*sit);
      for(unsigned int i=0; i<seeds.size(); ++i) {
	std::pair<Amg::Vector3D,Amg::Vector3D> seed3D;
	Trk::TrackParameters* startpar = 0;
	// calculate start parameters
	const Trk::PlaneSurface& surf = (*sit)->associatedSurface(); 
	Amg::Vector3D posOnSeg = intersectPlane( surf, seeds[i].first, seeds[i].second );
	Amg::Vector2D lpos;
	surf.globalToLocal(posOnSeg,posOnSeg,lpos);
	Trk::LocalDirection ldir;
	surf.globalToLocalDirection(seeds[i].second,ldir);

	Amg::Vector2D lp( (*sit)->localParameters()[Trk::locX], lpos[Trk::locY] );
	Trk::LocalDirection ld( (*sit)->localDirection().angleXZ(), ldir.angleYZ() );
	Amg::Vector3D gd;
	surf.localToGlobalDirection(ld,gd);
	Amg::Vector3D gp;
	surf.localToGlobal(lp,gp,gp);
	Amg::Vector3D perpos = gp + -10*(gd.unit());
	if(perpos.dot(gd) < 0) gd = -1*gd;
	startpar = new Trk::Perigee(perpos, gd, 0, perpos);
	seed3D = std::pair<Amg::Vector3D,Amg::Vector3D>(perpos,gd);
	ATH_MSG_VERBOSE(" start parameter " << perpos << " pp " << startpar->position() << " gd " << gd.unit() << " pp " << startpar->momentum().unit() );

	// old method as fall back
	if( !startpar ){
	  double phi = seeds[i].second.phi();
	  double theta = (*sit)->globalDirection().theta();
	  double qoverp=0;
	  Trk::PerigeeSurface persurf((*sit)->containedROTs().front()->globalPosition());
	  startpar = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);
	  seed3D = seeds[i];
	}
	
	std::vector< const Muon::MuonClusterOnTrack* > phiHits = getClustersOnSegment(orderedClusters,seed3D);
	if(phiHits.size() < 2) {
          delete startpar;
          continue;
        }
	//interleave the phi hits
	std::vector<const Trk::MeasurementBase*> vec2;
	const std::vector<const Trk::RIO_OnTrack*> etaHits = (*sit)->containedROTs();
	vec2.reserve(phiHits.size()+etaHits.size());
	unsigned int iEta(0),iPhi(0);
	ATH_MSG_VERBOSE( "There are " << etaHits.size() << " & " << phiHits.size() << " eta and phi hits" );
	while(true) {
	  float phiZ(999999.),etaZ(999999.);
	  if(iPhi < phiHits.size()) phiZ = fabs(phiHits[iPhi]->globalPosition().z());
	  if(iEta < etaHits.size()) etaZ = fabs(etaHits[iEta]->globalPosition().z());
	  if( phiZ < etaZ ) {
	    vec2.push_back(phiHits[iPhi]);
	    iPhi++;
	  }
	  else {
	    vec2.push_back(etaHits[iEta]);
            iEta++;
	  }
	  if( iEta >= etaHits.size() && iPhi >= phiHits.size() ) break;
	}

	
	ATH_MSG_DEBUG( "Fitting a segment with " << phiHits.size() << " phi hits and " << vec2.size() << " eta hits" );
	for(unsigned int k=0; k<vec2.size(); ++k) {
	  Identifier id = m_helper->getIdentifier(*vec2[k]);
	  ATH_MSG_VERBOSE( m_idHelperTool->toString(id) << " " << vec2[k]->globalPosition().perp() << ", " << vec2[k]->globalPosition().z() );
	}
	Trk::Track* segtrack = fit(vec2,*startpar);
	delete startpar;
	if(segtrack) {
	  //store the track segment in track collection
	  segTrkColl->push_back( segtrack );
	  is3Dseg = true;

          // Peter NEW calculate distance of segment to all muon clusters
          if( msgLvl(MSG::VERBOSE) ) {
            Amg::Vector3D  pos = segtrack->perigeeParameters()->position();
            Amg::Vector3D  dir = segtrack->perigeeParameters()->momentum();
            dir = dir / dir.mag();
            ATH_MSG_VERBOSE("3D eta-phi segment found position " << pos << " direction " << dir << " hits " << vec2.size());
            for(unsigned int k=0; k<muonClusters.size(); ++k) {
               const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*> (&(muonClusters[k]->associatedSurface()));
               if(surf) {
                 Amg::Vector3D posOnSurf = intersectPlane( *surf, pos, dir );
                 Amg::Vector2D lpos;
                 surf->globalToLocal(posOnSurf,posOnSurf,lpos);
                 double residual = muonClusters[k]->localParameters()[Trk::locX] - lpos[0];
                 double error = Amg::error(muonClusters[k]->localCovariance(),Trk::locX);
                 double pull = residual / error;
                 ATH_MSG_VERBOSE(" lay " << k << " residual " << residual  << " error  " << error
                                 << " pull " << pull << "  " << m_idHelperTool->toString(muonClusters[k]->identify()) );
               }
            }
          }

	} else {
	  ATH_MSG_DEBUG( "segment fit failed" );
	}		
      }//end loop on phi seeds
      if( !is3Dseg ) segments->push_back( (*sit)->clone() );
    }//end loop on precision plane segments

    TrackCollection* resolvedTracks = m_ambiTool->process(segTrkColl);
    ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size() );
    //store the resolved segments
    for(TrackCollection::const_iterator it=resolvedTracks->begin(); it!=resolvedTracks->end(); ++it) {
      MuonSegment* seg = m_trackToSegmentTool->convert( **it );
      if( !seg ) {
        ATH_MSG_VERBOSE("Segment conversion failed, no segment created. ");
      }
      else {
        segments->push_back(seg);
      }
    }

    //memory cleanup
    delete segTrkColl;
    delete resolvedTracks;

    for(std::vector<const Muon::MuonSegment*>::const_iterator sit=etaSegs->begin(); sit!=etaSegs->end(); ++sit) {
      delete *sit;
    }
    delete etaSegs;

    return segments;
  }


  std::vector< const Muon::MuonClusterOnTrack* > MuonClusterSegmentFinderTool::cleanClusters(std::vector< const Muon::MuonClusterOnTrack* >& muonClusters, bool selectPhiHits) const {
    std::vector< const Muon::MuonClusterOnTrack* > clusters;
    //remove the phi hits
    for(std::vector< const Muon::MuonClusterOnTrack* >::const_iterator cit=muonClusters.begin(); cit!=muonClusters.end(); ++cit) {
      if(!(*cit)) continue;
      bool measPhi = m_idHelperTool->measuresPhi( (*cit)->identify());
      if( measPhi != selectPhiHits ) continue;      
      clusters.push_back(*cit);
    }
    return clusters;
  }

  std::vector< std::vector<const Muon::MuonClusterOnTrack*> > MuonClusterSegmentFinderTool::orderByLayer(std::vector< const Muon::MuonClusterOnTrack* >& clusters, bool useWires) const{
    //loop on the input clusters and order by layer
    std::vector< std::vector<const Muon::MuonClusterOnTrack*> > orderedClusters;
    unsigned int nOrdered(0);
    double minz(0.),maxz(9999999.);    
    while(nOrdered < clusters.size()) {
      std::vector< const Muon::MuonClusterOnTrack* > tmpclusters;
      for(std::vector< const Muon::MuonClusterOnTrack* >::const_iterator cit = clusters.begin(); cit!=clusters.end(); ++cit) {
	if( useWires && m_idHelperTool->issTgc((*cit)->identify()) && m_idHelperTool->stgcIdHelper().channelType( (*cit)->identify() ) == 0 ) {
	  if(minz < 1) nOrdered++;
	  continue;
	}
	else if( !useWires && m_idHelperTool->issTgc((*cit)->identify()) && m_idHelperTool->stgcIdHelper().channelType( (*cit)->identify() ) == 2 ) {
	  if(minz < 1) nOrdered++;
	  continue;
	}
	if(fabs((*cit)->prepRawData()->globalPosition().z()) <= minz) continue;
	if(fabs((*cit)->prepRawData()->globalPosition().z()) < maxz) {
	  maxz = fabs((*cit)->prepRawData()->globalPosition().z());
	  tmpclusters.clear();
	  tmpclusters.push_back(*cit);
	}
	else if ( fabs(fabs((*cit)->prepRawData()->globalPosition().z()) - maxz) < 0.1) {
	  tmpclusters.push_back(*cit); 
	}
      } 
      if(tmpclusters.size() == 0) break;
      orderedClusters.push_back(tmpclusters);
      nOrdered += tmpclusters.size();
      minz = maxz+0.1;
      maxz = 9999999.;      
    }
    return orderedClusters;
  }

  std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > 
  MuonClusterSegmentFinderTool::segmentSeed( std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, bool usePhi ) const {

    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds;
    if(orderedClusters.size() < 2) return seeds;

    //calculate the straight line between the two furthest points
    int seedlayers1 = 0;
    for( unsigned int i = 0; (i<orderedClusters.size() && seedlayers1 < 2); ++i ){
      
      bool usedLayer1 = false;
      for(std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit=orderedClusters[i].begin(); cit!=orderedClusters[i].end(); ++cit) {
	
	if( usePhi != m_idHelperTool->measuresPhi((*cit)->identify() ) ) continue;
	//if( !usePhi && !m_idHelperTool->isMM((*cit)->identify() ) )      continue;
	usedLayer1 = true;
	const Amg::Vector3D& gp1 = (*cit)->prepRawData()->globalPosition();

	int seedlayers2 = 0;
	for(unsigned int k=orderedClusters.size()-1; (k>i && seedlayers2 < 2) ; --k) {

	  bool usedLayer2 = false;
	  for(std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit2=orderedClusters[k].begin(); cit2!=orderedClusters[k].end(); ++cit2) {

	    if( usePhi != m_idHelperTool->measuresPhi((*cit2)->identify() ) ) continue;
	    //if( !usePhi && !m_idHelperTool->isMM((*cit2)->identify() ) )      continue;

	    usedLayer2 = true;
	    const Amg::Vector3D& gp2 = (*cit2)->prepRawData()->globalPosition();
	    Amg::Vector3D segdir( (gp2.x()-gp1.x()),(gp2.y()-gp1.y()),(gp2.z()-gp1.z()) );
	    seeds.push_back(std::pair<Amg::Vector3D,Amg::Vector3D>(gp1,segdir));
	  }
	  if( usedLayer2 ) ++seedlayers2;

	}
      }
      if( usedLayer1 ) ++seedlayers1;
    }
    if(usePhi) return seeds;

    //use the innermost and outermost MM hits to form the seeds
    unsigned int innerMM(999),outerMM(999);
    for( unsigned int i = 0; i<orderedClusters.size(); ++i ) {
      if(m_idHelperTool->isMM(orderedClusters[i].front()->identify())) {
	innerMM = i;
	break;
      }
    }
    for( unsigned int i=orderedClusters.size()-1; i > innerMM; --i ) {
      if(m_idHelperTool->isMM(orderedClusters[i].front()->identify())) {
	outerMM = i;
	break;
      }
    }
    if(innerMM == 999 || outerMM == 999) return seeds;
    //create the seeds
    for(std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit=orderedClusters[innerMM].begin(); cit!=orderedClusters[innerMM].end(); ++cit) {
      const Amg::Vector3D& gp1 = (*cit)->prepRawData()->globalPosition();      
      for(std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit2=orderedClusters[outerMM].begin(); cit2!=orderedClusters[outerMM].end(); ++cit2) {
	const Amg::Vector3D& gp2 = (*cit2)->prepRawData()->globalPosition();
	Amg::Vector3D segdir( (gp2.x()-gp1.x()),(gp2.y()-gp1.y()),(gp2.z()-gp1.z()) );
	seeds.push_back(std::pair<Amg::Vector3D,Amg::Vector3D>(gp1,segdir));
      }
    }

    return seeds;
  }

  std::vector< const Muon::MuonClusterOnTrack* > MuonClusterSegmentFinderTool::getClustersOnSegment(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& clusters, 
												    std::pair<Amg::Vector3D,Amg::Vector3D>& seed) const {
    ATH_MSG_VERBOSE(" getClustersOnSegment: layers " << clusters.size()  );
    std::vector< const Muon::MuonClusterOnTrack* > rios;
    int layer = 0;
    for(std::vector<std::vector<const Muon::MuonClusterOnTrack*> >::const_iterator cvecIt=clusters.begin(); cvecIt!=clusters.end(); ++cvecIt) {
      const Muon::MuonClusterOnTrack* rio = 0;      
      double bestDist(9999.);      
      for(std::vector< const Muon::MuonClusterOnTrack* >::const_iterator cit=(*cvecIt).begin(); cit!=(*cvecIt).end(); ++cit) {
	double dist = clusterDistanceToSeed( *cit, seed);
	double error = Amg::error((*cit)->localCovariance(),Trk::locX);
	if(m_idHelperTool->isMM((*cit)->identify())) error += 25.;
	ATH_MSG_VERBOSE(" lay " << layer << " dist " << dist << " pull " << dist/error
			<< " cut " << m_maxClustDist << "  " << m_idHelperTool->toString((*cit)->identify()) );
	if( fabs(dist/error) < m_maxClustDist) {
	  if(fabs(dist) < bestDist) {
	    bestDist = fabs(dist);
	    rio = (*cit);
	  }
	}	 
      }
      if(rio) {
	ATH_MSG_VERBOSE(" adding  " << bestDist << "  " << m_idHelperTool->toString(rio->identify()) );
	rios.push_back( rio );
      }else{
      }
      ++layer;
    }    
    ATH_MSG_VERBOSE(" getClustersOnSegment: returning hits " << rios.size() );
    return rios;
  }


  Amg::Vector3D MuonClusterSegmentFinderTool::intersectPlane( const Trk::PlaneSurface& surf, const Amg::Vector3D& pos,
							      const Amg::Vector3D& dir ) const {
    const Amg::Vector3D& planepostion = surf.center();
    const Amg::Vector3D& planenormal = surf.normal();
    double denom = dir.dot(planenormal);
    double u = planenormal.dot(planepostion - pos)/denom;
    return pos + u * dir;
  }

  double MuonClusterSegmentFinderTool::clusterDistanceToSeed(const Muon::MuonClusterOnTrack* clus, std::pair<Amg::Vector3D,Amg::Vector3D>& seed) const {
    const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&clus->associatedSurface());
    double dist(-99999);
    if( !surf ) {
      ATH_MSG_DEBUG(" clusterDistanceToSeed: no Trk::PlaneSurface found on cluster ");
      return dist;
    }
    Amg::Vector3D  piOnPlane = intersectPlane(*surf,seed.first,seed.second);
    // transform to local plane coordiantes
    Amg::Vector2D lpos;
    surf->globalToLocal(piOnPlane,piOnPlane,lpos);
    //check if the MuonCluster is pad hit
    if(m_idHelperTool->issTgc( clus->identify() ) && m_idHelperTool->stgcIdHelper().channelType( clus->identify() ) == 0 ) {
      const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>(clus->prepRawData());
      if (!prd) {
         ATH_MSG_DEBUG(" clusterDistanceToSeed: no sTgcPrepData found ");
         return dist;
      }
      const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign( clus->identify() );
      if(!design) {	
	ATH_MSG_WARNING( "MuonPadDesign not found for " << m_idHelperTool->toString( clus->identify() ) );
	return dist;
      }	
      //check the alignment in locX 
      double chWidth1 = 0.5*design->channelWidth(prd->localPosition(),false);
      double chWidth2 = 0.5*design->channelWidth(prd->localPosition(),true);
      chWidth1 = fabs(chWidth1);
      chWidth2 = fabs(chWidth2);
      Amg::Vector2D lp1(prd->localPosition().x()-chWidth2,prd->localPosition().y()-chWidth1);
      Amg::Vector2D lp2(prd->localPosition().x()+chWidth2,prd->localPosition().y()+chWidth1);
      ATH_MSG_DEBUG(" chWidth2 locX " << chWidth2 << " chWidth1 locY" << chWidth1 << " design->sPadWidth " << design->sPadWidth << " design->lPadWidth " << design->lPadWidth );
      ATH_MSG_DEBUG(" cluster locX " << prd->localPosition().x() << " lpos.x() " << lpos.x() << " cluster locY " << prd->localPosition().y() << " lpos.y() " << lpos.y());
      if(lp1.y() < lpos.y() && lp2.y() > lpos.y() && lp1.x() < lpos.x() && lp2.x() > lpos.x() ) {
	dist = 0;
      }            
    }
    else {
      dist = clus->localParameters()[Trk::locX] - lpos[Trk::locX];
      ATH_MSG_DEBUG(" clus [locX] " << clus->localParameters()[Trk::locX] << " clus [LocY] " << clus->localParameters()[Trk::locY] << " lpos[locX] " << lpos[Trk::locX] << " lpos[locY] " << lpos[Trk::locY] );
    }

    return dist;
  }

  Trk::Track* MuonClusterSegmentFinderTool::fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const {
    Trk::Track* segtrack = m_slTrackFitter->fit(vec2,startpar,false,Trk::nonInteracting);
    if(segtrack) {
      ATH_MSG_VERBOSE( "segment fit succeeded");
      Trk::Track* cleanedTrack = m_trackCleaner->clean(*segtrack);
      if( cleanedTrack && cleanedTrack != segtrack ){
	delete segtrack;
	segtrack = cleanedTrack;
      }
      if( !m_helper->goodTrack(*segtrack,10) ) {
	if(segtrack->fitQuality()) {
	  ATH_MSG_DEBUG( "segment fit with chi^2/nDoF = " << segtrack->fitQuality()->chiSquared() << "/" << segtrack->fitQuality()->numberDoF() );
	}
	delete segtrack;
	segtrack = 0;
      }
    }
    return segtrack;
  }
  std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > 
  MuonClusterSegmentFinderTool::segmentSeedFromPads(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters,
						    const Muon::MuonSegment* etaSeg) const {

    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > seeds;

    std::vector<std::vector<const Muon::MuonClusterOnTrack*> > sTgc1,sTgc2;
    for(unsigned int i=0; i<orderedClusters.size(); ++i) {
      std::vector<const Muon::MuonClusterOnTrack*> hits;
      for(std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit=orderedClusters[i].begin(); cit!=orderedClusters[i].end(); ++cit) {
	//check the hit is aligned with the segment	
	//segment position on the surface
	const Trk::PlaneSurface* surf = dynamic_cast<const Trk::PlaneSurface*>(&(*cit)->associatedSurface());
	if( !surf ) continue;
	Amg::Vector3D  piOnPlane = intersectPlane(*surf,etaSeg->globalPosition(),etaSeg->globalDirection());
	// transform to local plane coordiantes
	Amg::Vector2D lpos;
	surf->globalToLocal(piOnPlane,piOnPlane,lpos);
	//pad width
	const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>((*cit)->prepRawData());
	if (!prd) continue;
        const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign( (*cit)->identify() );
	if (!design) continue;
        double chWidth1 = 0.5*design->channelWidth(prd->localPosition(),false);
	Amg::Vector2D lp1(prd->localPosition().x(),prd->localPosition().y()-chWidth1);
	Amg::Vector2D lp2(prd->localPosition().x(),prd->localPosition().y()+chWidth1);	
	if(lp1.y() < lpos.y() && lp2.y() > lpos.y()) {
	  hits.push_back( *cit );
	}
      }
      if(hits.size() == 0) continue;
      if(m_idHelperTool->stgcIdHelper().multilayer(hits.front()->identify()) == 1) sTgc1.push_back(hits);
      else sTgc2.push_back( hits );
    }

    ATH_MSG_DEBUG(" sTgc1.size() " << sTgc1.size() << " sTgc2.size() " << sTgc2.size());

    if(sTgc1.size() == 0 || sTgc2.size() == 0) return seeds;
    //store reference surfaces
    const sTgcPrepData* prdL1 = dynamic_cast<const sTgcPrepData*>(sTgc1.front().front()->prepRawData());
    const sTgcPrepData* prdL2 = dynamic_cast<const sTgcPrepData*>(sTgc2.back().front()->prepRawData());
    const Trk::PlaneSurface* surf1 = dynamic_cast<const Trk::PlaneSurface*>(&(sTgc1.front().front())->associatedSurface());
    const Trk::PlaneSurface* surf2 = dynamic_cast<const Trk::PlaneSurface*>(&(sTgc2.back().front())->associatedSurface());
    

    //ensure there are 4 entries per sTgc chamber
    while(sTgc1.size() < 4) {
      ATH_MSG_DEBUG("Adding empty layer to the sTgc chlay 1");
      std::vector<const Muon::MuonClusterOnTrack*> hits;
      sTgc1.push_back(hits);
    }
    while(sTgc2.size() < 4) {
      ATH_MSG_DEBUG("Adding empty layer to the sTgc chlay 2");
      std::vector<const Muon::MuonClusterOnTrack*> hits;
      sTgc2.push_back(hits);
    }
    if( !prdL1 || !prdL2 ) {
      ATH_MSG_WARNING( "No prd associated to one of the sTgc pads!" );
      return seeds;
    }

    if( !surf1 || !surf2 ) {
      ATH_MSG_WARNING( "No surface associated to one of the sTgc pads!" );
      return seeds;
    }

    if(sTgc1.size() != 4 || sTgc2.size() != 4) {
      ATH_MSG_WARNING( "Something is wrong with the sTgc phi pads!  Found " << sTgc1.size() << "/" << sTgc2.size() << " layers" );
    }
    std::vector<std::pair<double,double> > sTgc1_phi = getPadPhiOverlap(sTgc1);
    std::vector<std::pair<double,double> > sTgc2_phi = getPadPhiOverlap(sTgc2);

    //make combinations of pad clusters
    std::vector<Amg::Vector3D> phiPos1,phiPos2;
    for(unsigned int i1=0; i1<sTgc1_phi.size(); ++i1) {
      double midPhi1 = 0.5*(sTgc1_phi[i1].first + sTgc1_phi[i1].second);
      //change to global position
      Amg::Vector2D lp1(midPhi1,prdL1->localPosition().y());
      Amg::Vector3D gpL1;
      prdL1->detectorElement()->surface().localToGlobal(lp1,gpL1,gpL1); 
      for(unsigned int i2=0; i2<sTgc2_phi.size(); ++i2) {
	//change to global position	  
	double midPhi2 = 0.5*(sTgc2_phi[i2].first + sTgc2_phi[i2].second);
	Amg::Vector2D lp2(midPhi2,prdL2->localPosition().y());
	Amg::Vector3D gpL2;
	prdL2->detectorElement()->surface().localToGlobal(lp2,gpL2,gpL2); 	  
	phiPos1.push_back(gpL1);
	phiPos2.push_back(gpL2);
      }
    }
    
    for(unsigned int i=0; i<phiPos1.size(); ++i) {      
      //create the seed pair
      Amg::Vector3D gdir( (phiPos2[i].x()-phiPos1[i].x()),(phiPos2[i].y()-phiPos1[i].y()),(phiPos2[i].z()-phiPos1[i].z()) );
      std::pair<Amg::Vector3D,Amg::Vector3D> seedPair(phiPos1[i],gdir);
      seeds.push_back(seedPair);
    }

    ATH_MSG_DEBUG(" segmentSeedFromPads: seeds.size() " << seeds.size());
    return seeds;

  }


  std::vector<std::pair<double,double> > MuonClusterSegmentFinderTool::getPadPhiOverlap(std::vector< std::vector<const Muon::MuonClusterOnTrack*> >& pads) const {
    std::vector<std::pair<double,double> > phiOverlap;

    //calculate the max/min phi values for each pad
    std::vector<std::vector<double> > padMax,padMin;    
    for(unsigned int i=0; i<pads.size(); ++i) {
      std::vector<double> padLayerMax,padLayerMin;
      for(std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cit=pads[i].begin(); cit!=pads[i].end(); ++cit) {
	Identifier id = (*cit)->identify();
	const sTgcPrepData* prd = dynamic_cast<const sTgcPrepData*>((*cit)->prepRawData()); 
	if (!prd) {
          ATH_MSG_WARNING("No prd found for " << m_idHelperTool->toString( id ) );
          continue;
        } 
        const MuonGM::MuonPadDesign* design = prd->detectorElement()->getPadDesign( id ); 
	if( !design ) { 
	  ATH_MSG_WARNING("No design found for " << m_idHelperTool->toString( id ) ); 
	  continue; 
	} 
	double chWidth = 0.5*design->channelWidth(prd->localPosition(),true); 	
	Amg::Vector2D lp1(prd->localPosition().x()+chWidth,prd->localPosition().y()); 
	Amg::Vector2D lp2(prd->localPosition().x()-chWidth,prd->localPosition().y()); 
	padLayerMax.push_back(lp1.x());
	padLayerMin.push_back(lp2.x());
      }
      padMax.push_back(padLayerMax);
      padMin.push_back(padLayerMin);
    }

    //find all combinations of pads (1 per layer)
    unsigned int nCombos(1);
    for(unsigned int i=1; i<padMax.size(); ++i) {
      if(padMax[i].size() != 0) nCombos = nCombos*padMax[i].size();
    }
    for(unsigned int l0=0; l0<padMax[0].size(); ++l0) {     
      std::vector<double> phiMax(nCombos,padMax[0][l0]);
      std::vector<double> phiMin(nCombos,padMin[0][l0]);      
      for(unsigned int l1=0; l1<padMax[1].size(); ++l1) {
    	for(unsigned int i=0; i<nCombos; ++i) {
    	  if(nCombos % padMax[1].size() == 0) {	    
	    if(padMax[1][l1] < phiMax[i]) phiMax[i] = padMax[1][l1];
	    if(padMin[1][l1] > phiMin[i]) phiMin[i] = padMin[1][l1];
	  }
	}
	for(unsigned int l2=0; l2<padMax[2].size(); ++l2) {
	  for(unsigned int i=0; i<nCombos; ++i) {
	    if(nCombos % padMax[2].size() == 0) {
	      if(padMax[2][l2] < phiMax[i]) phiMax[i] = padMax[2][l2];
	      if(padMin[2][l2] > phiMin[i]) phiMin[i] = padMin[2][l2];
	    }
	  }
	  for(unsigned int l3=0; l3<padMax[3].size(); ++l3) {
	    for(unsigned int i=0; i<nCombos; ++i) {
	      if(nCombos % padMax[3].size() == 0) {
		if(padMax[3][l3] < phiMax[i]) phiMax[i] = padMax[3][l3];
		if(padMin[3][l3] > phiMin[i]) phiMin[i] = padMin[3][l3];
	      }
	    }
	  }//end layer 3 loop
	}//end layer 2 loop
      }//end layer 1 loop
      //store the overlaps regions
      for(unsigned int i=0; i<nCombos; ++i) {
	phiOverlap.push_back( std::pair<double,double>(phiMin[i],phiMax[i]) );
      }
    }//end layer 0 loop


    return phiOverlap;
  }


}//end namespace
