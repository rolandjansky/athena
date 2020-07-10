/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_SeededTrackFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 02/20/2006 Thomas Koffas
// modified by Markus ELsing
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "TRT_SeededTrackFinder/TRT_SeededTrackFinder.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

///Needed for my scoring
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "SiSPSeededTrackFinderData/SiCombinatorialTrackFinderData_xk.h"

#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "RoiDescriptor/RoiDescriptor.h"

#include <memory>
using namespace std;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededTrackFinder::TRT_SeededTrackFinder
(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_nprint(0),
    m_ntracks(0),
    m_trackmaker("InDet::TRT_SeededTrackFinderTool"),
    m_fitterTool("Trk::KalmanFitter/InDetTrackFitter"),
    m_SegmentsKey("TRTSegments"),
    m_outTracksKey("TRTSeededTracks")
{
  m_doRefit          = false                                ;       //Do a final careful refit of tracks
  m_doExtension      = false                                ;       //Find the track TRT extension
  m_rejectShortExten = false                                ;
  m_doStat           = false                                ;       //Statistics of final tracks
  m_saveTRT          = true                                 ;       //Save stand-alone TRT segments
  m_MaxSegNum        = 5000                                 ;

  declareProperty("TrackTool"                  ,m_trackmaker       ); //Back tracking tool
  declareProperty("RefitterTool"               ,m_fitterTool       ); //Track refit tool
  declareProperty("InputSegmentsLocation"      ,m_SegmentsKey      ); //Input track collection
  declareProperty("OutputTracksLocation"       ,m_outTracksKey     ); //Output track collection
  declareProperty("FinalRefit"                 ,m_doRefit          ); //Do a final careful refit of tracks
  declareProperty("TrtExtension"               ,m_doExtension      ); //Find the TRT extension of the track
  declareProperty("RejectShortExtension"       ,m_rejectShortExten ); //Reject short extensions
  declareProperty("FinalStatistics"            ,m_doStat           ); //Statistics of final tracks
  declareProperty("OutputSegments"             ,m_saveTRT          ); //Save stand-alone TRT segments
  declareProperty("MaxNumberSegments"          ,m_MaxSegNum        ); //Maximum Number of segments to be handled
  declareProperty("MinTRTonSegment"            ,m_minTRTonSegment = 10); //Minimum Number of Hits on segment
  declareProperty("MinTRTonly"                 ,m_minTRTonly      = 15); //Minimum Number of Hits on TRT only
  // --- selection cuts after SI extension
  declareProperty("SiExtensionCuts",   m_SiExtensionCuts    = false);
  declareProperty("minPt"             ,m_minPt              = 500. );
  declareProperty("maxEta"            ,m_maxEta             = 2.7  );
  declareProperty("maxRPhiImp"        ,m_maxRPhiImp         = 10.  );
  declareProperty("maxZImp"           ,m_maxZImp            = 250. );
  declareProperty("Extrapolator"      ,m_extrapolator              );
  declareProperty("CaloSeededRoI"     ,m_caloSeededRoI=false       );
  declareProperty("CaloClusterEt"     ,m_clusterEt=3000.           );
  declareProperty("dEtaCaloRoI"       ,m_deltaEta=0.1              );
  declareProperty("dPhiCaloRoI"       ,m_deltaPhi=0.25             );
  declareProperty("dZCaloRoI"         ,m_deltaZ=300                );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededTrackFinder::initialize()
{
  StatusCode  sc;

  msg(MSG::DEBUG) << "Initializing TRT_SeededTrackFinder" << endmsg;
 
  //Get the TRT seeded track maker tool
  //
  ATH_CHECK(m_trackmaker.retrieve());

  //Get the refitting tool
  //
  ATH_CHECK( m_fitterTool.retrieve(      DisableTool{ !m_doRefit } ));
  ATH_CHECK( m_extrapolator.retrieve(    DisableTool{ !m_SiExtensionCuts } ));
  ATH_CHECK( m_beamSpotKey.initialize(   m_SiExtensionCuts) );

  // optional PRD to track association map
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty() ) );

  ATH_CHECK( m_trackSummaryTool.retrieve( DisableTool{ m_trackSummaryTool.name().empty() } ) );

  // Get tool for track extension to TRT
  //
  ATH_CHECK( m_trtExtension.retrieve(    DisableTool{ !m_doExtension} ));


  ATH_CHECK(  m_SegmentsKey.initialize()) ;  /** TRT segments to use */
  ATH_CHECK( m_outTracksKey.initialize());

  if(m_caloSeededRoI){
    ATH_CHECK( m_regionSelector.retrieve());
    ATH_CHECK( m_caloKey.initialize(m_caloSeededRoI) );
  } else {
    m_regionSelector.disable();
  }
  
  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)) {
     dumptools(msg(MSG::DEBUG));
  }
   //Global counters. See the include file for definitions
  m_totalStat = Stat_t();
  return sc;

}

namespace InDet {
  class MyPRDtoTrackMap  : public Trk::PRDtoTrackMap {
  public:
    unsigned int size() const {
      return m_prepRawDataTrackMap.size();
    }
  };

  class ExtendedSiCombinatorialTrackFinderData_xk : public InDet::SiCombinatorialTrackFinderData_xk
  {
  public:
    ExtendedSiCombinatorialTrackFinderData_xk(const SG::ReadHandleKey<Trk::PRDtoTrackMap> &key) { 
      if (!key.key().empty()) {
        m_prdToTrackMap = SG::ReadHandle<Trk::PRDtoTrackMap>(key);
        setPRDtoTrackMap(m_prdToTrackMap.cptr());
      }
    }
  private:
    void dummy() {}
    SG::ReadHandle<Trk::PRDtoTrackMap> m_prdToTrackMap;
  };
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode InDet::TRT_SeededTrackFinder::execute() {
   return execute_r( Gaudi::Hive::currentContext() );
}

StatusCode InDet::TRT_SeededTrackFinder::execute_r (const EventContext& ctx) const
{

  //Counters. See the include file for definitions
  Stat_t ev_stat;

  // counter
  int nTrtSegCur = 0;

  SG::ReadHandle<Trk::SegmentCollection>  segments(m_SegmentsKey,ctx);
  if(!segments.isValid()){
    ATH_MSG_FATAL ("No segment with name " << segments.name() << " found in StoreGate!");
    return StatusCode::FAILURE;
  }else{
    ATH_MSG_DEBUG ("Found segments collection " << segments.name() << " in StoreGate!");
  }


  // number of segments + statistics
  ev_stat.m_counter[Stat_t::kNTrtSeg] = int(segments->size());
  ATH_MSG_DEBUG ("TRT track container size " << ev_stat.m_counter[Stat_t::kNTrtSeg]);
  if(ev_stat.m_counter[Stat_t::kNTrtSeg]>m_MaxSegNum) {
    ATH_MSG_DEBUG ("TRT track container size huge; will process event partially if number of max segments reached !!!");
  }

  // Event dependent data of SiCombinatorialTrackFinder_xk
  InDet::ExtendedSiCombinatorialTrackFinderData_xk combinatorialData(m_prdToTrackMap);

  std::unique_ptr<InDet::ITRT_SeededTrackFinder::IEventData> event_data_p;
   
  if(m_caloSeededRoI ) {
    SG::ReadHandle calo(m_caloKey,ctx);
    std::unique_ptr<RoiDescriptor> roiComp = std::make_unique<RoiDescriptor>(true);

    if (calo.isValid()) {
      RoiDescriptor * roi =0;
      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
      double beamZ = beamSpotHandle->beamVtx().position().z();
      roiComp->clear();
      roiComp->setComposite();
      for (const Trk::CaloClusterROI *c: *calo) {
        if ( c->energy()/cosh(c->globalPosition().eta()) < m_clusterEt) {
          continue;
        }

        double eta = c->globalPosition().eta();
        double phi = c->globalPosition().phi();
        double roiPhiMin = phi -m_deltaPhi;
        double roiPhiMax = phi +m_deltaPhi;
        double roiEtaMin = eta -m_deltaEta;
        double roiEtaMax = eta +m_deltaEta;
        double roiZMin = beamZ -m_deltaZ;
        double roiZMax = beamZ +m_deltaZ;
        roi = new RoiDescriptor( eta, roiEtaMin, roiEtaMax,phi, roiPhiMin ,roiPhiMax, beamZ, roiZMin,roiZMax);
        roiComp->push_back(roi);
      }
    }
    std::vector<IdentifierHash> listOfSCTIds;
    std::vector<IdentifierHash> listOfPixIds;
    m_regionSelector->HashIDList(*roiComp, listOfSCTIds );
    event_data_p = m_trackmaker->newRegion(ctx, combinatorialData, listOfPixIds, listOfSCTIds);
  } else {
    event_data_p = m_trackmaker->newEvent(ctx, combinatorialData);
  }
   
  std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData> ext_event_data_p( m_trtExtension->newEvent(ctx) );

//  TrackCollection* outTracks  = new TrackCollection;           //Tracks to be finally output
  std::unique_ptr<TrackCollection> outTracks = std::make_unique<TrackCollection>();

  std::vector<Trk::Track*> tempTracks;                           //Temporary track collection
  tempTracks.reserve(128);
  // loop over event
  ATH_MSG_DEBUG ("Begin looping over all TRT segments in the event");
  Trk::SegmentCollection::const_iterator iseg    = segments->begin();
  Trk::SegmentCollection::const_iterator isegEnd = segments->end();
  for(; iseg != isegEnd; ++ iseg) {

    // Get the track segment
    const Trk::TrackSegment *trackTRT = dynamic_cast<const Trk::TrackSegment*>(*iseg);
    if(!trackTRT){
      ATH_MSG_ERROR ("No pointer to segment !");
      continue;
    }else{

      // the segment finder is applying a TRT(eta) cut and a pt preselection, so we don't do that here

      //Ask for at least 10 TRT hits in order to process
      if(trackTRT->numberOfMeasurementBases() < m_minTRTonSegment) {
	ATH_MSG_DEBUG ("TRT segment fails nTRT hit cut, reject.");

	// statistics
	ev_stat.m_counter[Stat_t::kNTrtFailSel]++;

      }	else {

	// do we continue to process ?
	nTrtSegCur++;
	if(nTrtSegCur>=m_MaxSegNum) {
	  ATH_MSG_INFO ("====> Reached maximal number of segments in event, stop !!!");
	  // statistics
	  ev_stat.m_counter[Stat_t::kNTrtLimit]++;
	  break;
	}

	// Get the number of the TRT track segment ROTs
	ATH_MSG_DEBUG ("=> New segment to process, number Of TRT ROTs : " << (trackTRT->numberOfMeasurementBases()));

	// statistics
        ev_stat.m_counter[Stat_t::Stat_t::kNTrtSegGood]++;

	// ok, call track maker and get list of possible track candidates
        std::list<Trk::Track*> trackSi = m_trackmaker->getTrack(ctx, *event_data_p, *trackTRT); //Get the possible Si extensions

        if (trackSi.size()==0) {
          ATH_MSG_DEBUG ("No Si track candidates associated to the TRT track ");

	  // statistics
	  ev_stat.m_counter[Stat_t::kNTrtNoSiExt]++;

	  // obsolete backup of TRT only
          if(m_saveTRT && trackTRT->numberOfMeasurementBases() > m_minTRTonly){
            ///Transform the original TRT segment into a track
            Trk::Track* trtSeg = 0;trtSeg = segToTrack(ctx, *trackTRT);
            if(!trtSeg) {
	      ATH_MSG_DEBUG ("Failed to make a track out of the TRT segment!");
	      continue;
	    }
	    // statistics
            ev_stat.m_counter[Stat_t::kNBckTrk]++; ev_stat.m_counter[Stat_t::Stat_t::kNBckTrkTrt]++;
	    // add track to output list
	    outTracks->push_back(trtSeg);
          }
          continue;

        } else {

	  // Found useful extensions
          ATH_MSG_DEBUG ("Found " << (trackSi.size()) << " Si tracks associated to the TRT track ");

          // Merge the resolved Si extensions with the original TRT track segment
          std::list<Trk::Track*>::const_iterator itt    = trackSi.begin();
          std::list<Trk::Track*>::const_iterator ittEnd = trackSi.end();
          for (; itt != ittEnd ; ++itt){

	    tempTracks.push_back(*itt);

	    // get list of TSOS
            const DataVector<const Trk::TrackStateOnSurface>* temptsos = (*itt)->trackStateOnSurfaces();
            if (!temptsos) {
	      ATH_MSG_DEBUG ("Silicon extension empty ???");
	      // cleanup
	      //delete *itt;
	      continue;
	    }

	    // Add the track to the list of tracks in the event
	    ATH_MSG_DEBUG ("Silicon extension found has length of : " << temptsos->size());

	    // do we do a preselection ?
	    if (m_SiExtensionCuts) {

             // get parameters without errors
	      const Trk::TrackParameters* input = (*itt)->trackParameters()->front()->clone();

	      // cuts on parameters
	      if (fabs(input->pT()) < m_minPt) {
	      	ATH_MSG_DEBUG ("Track pt < "<<m_minPt<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete input;
	        // statistics
	        ev_stat.m_counter[Stat_t::kNExtCut]++;
	        continue;
	      }
	      if (fabs(input->eta()) > m_maxEta) {
	        ATH_MSG_DEBUG ("Track eta > "<<m_maxEta<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete input;
	        // statistics
	        ev_stat.m_counter[Stat_t::kNExtCut]++;
	        continue;
	      }

	      // --- beam spot position
	      Amg::Vector3D beamSpotPosition(0,0,0);
	      if (m_SiExtensionCuts){
                 SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
                 beamSpotPosition = beamSpotHandle->beamVtx().position();
              }
	      // --- create surface
	      Trk::PerigeeSurface perigeeSurface(beamSpotPosition);

	      // uses perigee on track or extrapolates, no material in any case, we cut on impacts
	      const Trk::TrackParameters* parm = m_extrapolator->extrapolateDirectly(*input, perigeeSurface);
	      delete input;

	      const Trk::Perigee*extrapolatedPerigee = dynamic_cast<const Trk::Perigee*> (parm );
	      if (!extrapolatedPerigee) {
	        msg(MSG::WARNING) << "Extrapolation of perigee failed, this should never happen" << endmsg;
	        // cleanup
	        // delete *itt;
	        delete parm;
	        // statistics
	        ev_stat.m_counter[Stat_t::kNExtCut]++;
	        continue;
	      }

	      ATH_MSG_VERBOSE ("extrapolated perigee: "<<*extrapolatedPerigee);

	      //if (fabs(extrapolatedPerigee->parameters()[Trk::d0]*extrapolatedPerigee->sinTheta()) > m_maxRPhiImp) {
	      if (fabs(extrapolatedPerigee->parameters()[Trk::d0]) > m_maxRPhiImp) {
	        ATH_MSG_DEBUG ("Track Rphi impact > "<<m_maxRPhiImp<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete extrapolatedPerigee;
	        // statistics
	        ev_stat.m_counter[Stat_t::kNExtCut]++;
	        continue;
	      }
	      // if (fabs(extrapolatedPerigee->parameters()[Trk::z0]*extrapolatedPerigee->sinTheta()) > m_maxZImp) {
	      if (fabs(extrapolatedPerigee->parameters()[Trk::z0]) > m_maxZImp) {
	        ATH_MSG_DEBUG ("Track Z impact > "<<m_maxZImp<<", reject it");
	        // cleanup
	        // delete *itt;
	        delete extrapolatedPerigee;
	        // statistics
	        ev_stat.m_counter[Stat_t::kNExtCut]++;
	        continue;
	      }
	      delete extrapolatedPerigee;
	    }

	    // do re run a Track extension into TRT ?
	    Trk::Track* globalTrackNew = 0;

	    // do we have 4 and extension is enabled ?
            if(int(temptsos->size())>=4 && m_doExtension){

	      // Add the track to the list of tracks in the event
	      ATH_MSG_DEBUG ("Try to improve TRT calling extension tool.");

	      // statistics
	      ev_stat.m_counter[Stat_t::Stat_t::kNTrtExtCalls]++;

	      // call extension tool
              std::vector<const Trk::MeasurementBase*>& tn = m_trtExtension->extendTrack(ctx, *(*itt), *ext_event_data_p);

              if(!tn.size()) {

		// Fallback if extension failed
		ATH_MSG_DEBUG ("No new segment found, use input segment as fallback.");

		// statistics
		ev_stat.m_counter[Stat_t::Stat_t::kNTrtExtFail]++;

		// merge Si with input track segments
		globalTrackNew = mergeSegments(**itt,*trackTRT);

	      } else if (!m_rejectShortExten || tn.size() >= trackTRT->numberOfMeasurementBases() ) {

 		// Use the extension to instead of the segment
		ATH_MSG_DEBUG ("Successful extension, number of TRT hits : " << tn.size() << " was : " << (trackTRT->numberOfMeasurementBases()));

		// merge the extension with the Si track
                globalTrackNew = mergeExtension(**itt,tn);

		// Add the track to the list of tracks in the event
		ATH_MSG_DEBUG ("Merged extension with Si segment");
		// statistics
		ev_stat.m_counter[Stat_t::kNTrtExt]++;

		// clean up
		std::vector<const Trk::MeasurementBase*>::const_iterator iv, ive=tn.end();
                for(iv=tn.begin(); iv!=ive; ++iv) delete (*iv);

	      } else {

 		// Extension is shorter, let's fall back onto the original
		ATH_MSG_DEBUG ("Extension too short, number of TRT hits : " << tn.size() << " was : " << (trackTRT->numberOfMeasurementBases()) << ". Use Segement !");

		// merge segments
		globalTrackNew = mergeSegments(**itt,*trackTRT);

		// Add the track to the list of tracks in the event
		ATH_MSG_DEBUG ("Merged TRT segment with Si segment");

		// statistics
		ev_stat.m_counter[Stat_t::Stat_t::kNTrtExtBad]++;

		// clean up
		std::vector<const Trk::MeasurementBase*>::const_iterator iv, ive=tn.end();
                for(iv=tn.begin(); iv!=ive; ++iv) delete (*iv);

	      }

	    } else {
	      // no extension tool, jsut add the two
	      ATH_MSG_DEBUG ("Do not try to extend Si track, merging it with input TRT.");

	      // merge segments
	      globalTrackNew = mergeSegments(**itt,*trackTRT);
	    }

	    // do we have an track candidate ?
            if(!globalTrackNew){
              ATH_MSG_DEBUG ("Failed to merge TRT+Si track segment !");

	      if(m_saveTRT && trackTRT->numberOfMeasurementBases() > m_minTRTonly) {
                Trk::Track* trtSeg = 0;trtSeg = segToTrack(ctx, *trackTRT);
                if(!trtSeg){
		  ATH_MSG_DEBUG ("Failed to make a track out of the  TRT segment!");
		  continue;
		}
		ATH_MSG_DEBUG ("Add TRT only to output list");

		// statistis
                ev_stat.m_counter[Stat_t::kNBckTrk]++; ev_stat.m_counter[Stat_t::Stat_t::kNBckTrkTrt]++;

		// add it to output list
                if (m_trackSummaryTool.isEnabled()) {
                   m_trackSummaryTool->computeAndReplaceTrackSummary(*trtSeg,
                                                                     combinatorialData.PRDtoTrackMap(),
                                                                     false /* DO NOT suppress hole search*/);
                }
		outTracks->push_back(trtSeg);
              }

	    } else {

              ATH_MSG_DEBUG ("Save merged TRT+Si track segment!");

	      // statistics
	      ev_stat.m_counter[Stat_t::kNBckTrk]++; ev_stat.m_counter[Stat_t::Stat_t::kNBckTrkSi]++;

	      // add it to output list
              if (m_trackSummaryTool.isEnabled()) {
                 m_trackSummaryTool->computeAndReplaceTrackSummary(*globalTrackNew,
                                                                   combinatorialData.PRDtoTrackMap(),
                                                                   false /* DO NOT suppress hole search*/);
              }
	      outTracks->push_back(globalTrackNew);
            }
	  }
        }
      }
    }
  }

  // further debugging of results
  if(m_doStat){
    Analyze(outTracks.get());
  }

  if (SG::WriteHandle<TrackCollection>(m_outTracksKey,ctx).record(std::move(outTracks)).isFailure()){
     ATH_MSG_ERROR("Failed to record " << m_outTracksKey.key());
     return StatusCode::FAILURE;
  }

  // Update the total counters
  {
     std::lock_guard<std::mutex> lock(m_statMutex);
     m_totalStat += ev_stat;
  }

  for (auto p : tempTracks){
    delete p;
  }
  m_trackmaker->endEvent(*event_data_p);

  //Print common event information
  if(msgLvl(MSG::DEBUG)){
     dumpevent(msg(MSG::DEBUG), ev_stat);
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededTrackFinder::finalize()
{
  if(msgLvl(MSG::INFO)){
     msg(MSG::INFO) << std::endl;
     dumpevent(msg(MSG::INFO), m_totalStat);
     // dumptools(msg(MSG::INFO));
     msg(MSG::INFO) << endmsg;
  }
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededTrackFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_trackmaker.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_fitterTool.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trtExtension.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_SegmentsKey.key().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 65-m_outTracksKey.key().size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
       <<std::endl;
  out<<"| Tool for TRT seeded track finding | "<<m_trackmaker.type()    <<s1<<std::endl;
  out<<"| Tool for final track refitting    | "<<m_fitterTool.type()    <<s2<<std::endl;
  out<<"| Tool for TRT trac extension       | "<<m_trtExtension.type()  <<s3<<std::endl;
  out<<"| Location of input tracks          | "<<m_SegmentsKey.key()      <<s4<<std::endl;
  out<<"| Location of output tracks         | "<<m_outTracksKey.key()     <<s5<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
      <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededTrackFinder::dumpevent( MsgStream& out, const InDet::TRT_SeededTrackFinder::Stat_t &stat ) const
{
  out<<"|-------------------------------------------------------------------"<<std::endl;
  out<<"|  Investigated :"<<std::endl
     <<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNTrtSeg]       <<" TRT Segments on input"<<std::endl
     <<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNTrtFailSel]   <<" TRT Segments fail selection on input"<<std::endl
     <<"| "<<std::setw(7)<<stat.m_counter[Stat_t::Stat_t::kNTrtSegGood]   <<" TRT Segments after selection"<<std::endl;
  if (stat.m_counter[Stat_t::kNTrtLimit]>0) {
    out<<"|-------------------------------------------------------------------"<<std::endl;
    out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNTrtLimit]   <<" TRT segments lost because of processing limit"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------"<<std::endl;
  out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNTrtNoSiExt]   <<" TRT segments without Si extension"<<std::endl;
  if (m_SiExtensionCuts) {
    out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNExtCut]     <<" number of Si extension failing cuts"<<std::endl;
  }
  if (stat.m_counter[Stat_t::Stat_t::kNBckTrkTrt]>0) {
    out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::Stat_t::kNBckTrkTrt]  <<" number ot TRT only tracks created"<<std::endl;
  }
  if (m_doExtension) {
    out<<"|-------------------------------------------------------------------"<<std::endl;
    out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::Stat_t::kNTrtExtCalls]<<" number of times TRT extension is called"<<std::endl
       <<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNTrtExt]     <<" number of good TRT extension"<<std::endl;
    if (m_rejectShortExten) {
      out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::Stat_t::kNTrtExtBad]<<" number of bad TRT extension"<<std::endl;
    }
    out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::Stat_t::kNTrtExtFail]<<" number of failed TRT extension and fallback"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------"<<std::endl;
  out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::Stat_t::kNBckTrkSi]    <<" TRT+Si tracks created of output"<<std::endl;
  if (stat.m_counter[Stat_t::kNBckTrkSi] != stat.m_counter[Stat_t::kNBckTrk]) {
    out<<"| "<<std::setw(7)<<stat.m_counter[Stat_t::kNBckTrk]    <<" total number of tracks on output"<<std::endl;
  }
  out<<"|-------------------------------------------------------------------";
  return out;
}


///////////////////////////////////////////////////////////////////
// Merge a Si extension and a TRT segment.Refit at the end
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_SeededTrackFinder::mergeSegments(const Trk::Track& tT, const Trk::TrackSegment& tS) const {
	// TSOS from the track
	const DataVector<const Trk::TrackStateOnSurface>* stsos = tT.trackStateOnSurfaces();
	// fitQuality from track
	const Trk::FitQuality* fq = tT.fitQuality()->clone();
	// output datavector of TSOS
	std::unique_ptr<DataVector<const Trk::TrackStateOnSurface> >
           ntsos(std::make_unique< DataVector<const Trk::TrackStateOnSurface> >());

	int siHits = 0;
	// copy track Si states into track
	DataVector<const Trk::TrackStateOnSurface>::const_iterator p_stsos;
	for (p_stsos=stsos->begin(); p_stsos != stsos->end(); ++p_stsos) {
		ntsos->push_back( (*p_stsos)->clone() );
		if ((*p_stsos)->type(Trk::TrackStateOnSurface::Measurement)) siHits++;
	}

	// loop over segment
	for (int it = 0; it < int(tS.numberOfMeasurementBases()); it++) {
		//test if it is a pseudo measurement
		if ( dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tS.measurement(it)) ) {
			if (siHits < 4) {
				ATH_MSG_DEBUG ("Too few Si hits.Will keep pseudomeasurement...");
				const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(), 0);
				ntsos->push_back(seg_tsos);
			}
		} else {
			std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
			typePattern.set(Trk::TrackStateOnSurface::Measurement);
			// Possible memory leak
			const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(), 0, 0, 0, typePattern);
			ntsos->push_back(seg_tsos);
		}
	}

	///Construct the new track
	Trk::TrackInfo info;
	info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
        std::unique_ptr<Trk::Track> newTrack(std::make_unique<Trk::Track>(info, ntsos.release(), fq));

	//Careful refitting at the end
	if (m_doRefit) {
                newTrack=std::unique_ptr<Trk::Track>( m_fitterTool->fit(*newTrack, false, Trk::pion) );
		if (!newTrack) {
			ATH_MSG_DEBUG ("Refit of TRT+Si track segment failed!");
			return nullptr;
		}

		//Protect for tracks that have no really defined locz and theta parameters
		//const Trk::MeasuredPerigee* perTrack=dynamic_cast<const Trk::MeasuredPerigee*>(newTrack->perigeeParameters());

		const Trk::Perigee* perTrack=newTrack->perigeeParameters();

		if (perTrack) {
			//const Trk::CovarianceMatrix& CM = perTrack->localErrorMatrix().covariance();
			const AmgSymMatrix(5)* CM = perTrack->covariance();
			if (!CM || sqrt((*CM)(1,1)) == 0. || sqrt((*CM)(3,3)) == 0.) {
                                return nullptr;
                        }
                }
        }
        return newTrack.release();
}

///////////////////////////////////////////////////////////////////
// Transform a TRT segment to track
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_SeededTrackFinder::segToTrack(const EventContext&, const Trk::TrackSegment& tS) const {
	ATH_MSG_DEBUG ("Transforming the TRT segment into a track...");

	//Get the track segment information and build the initial track parameters
	const Trk::StraightLineSurface* surf = dynamic_cast<const Trk::StraightLineSurface*>(&(tS.associatedSurface()));
	if (!surf) {
		throw std::logic_error("Unhandled surface.");
	}
	const AmgVector(5)& p = tS.localParameters();
	AmgSymMatrix(5)* ep = new AmgSymMatrix(5)(tS.localCovariance());

        std::unique_ptr<DataVector<const Trk::TrackStateOnSurface> > 
           ntsos = std::make_unique<DataVector<const Trk::TrackStateOnSurface> >();

	std::unique_ptr<const Trk::TrackParameters> segPar(surf->createParameters<5, Trk::Charged>(p(0), p(1), p(2), p(3), p(4), ep));
	if (segPar) {
		if (msgLvl(MSG::DEBUG)) {
			msg(MSG::DEBUG) << "Initial TRT Segment Parameters for refitting " << (*segPar) << endmsg;
		}
	} else {
		if (msgLvl(MSG::DEBUG)) {
			msg(MSG::DEBUG) << "Could not get initial TRT segment parameters! " << endmsg;
		}
		return nullptr;
	}

	for (int it = 0; it < int(tS.numberOfMeasurementBases()); it++) {
		// on first measurement add parameters
		const Trk::TrackStateOnSurface* seg_tsos = 0;
		std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
		typePattern.set(Trk::TrackStateOnSurface::Measurement);
		if (it == 0)
			seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(), segPar.release(), 0, 0, typePattern);
		else
			seg_tsos = new Trk::TrackStateOnSurface(tS.measurement(it)->clone(), 0, 0, 0, typePattern);
		ntsos->push_back(seg_tsos);
	}

	// Construct the new track
	//  Trk::Track* newTrack = new Trk::Track(Trk::Track::TRT_SeededTrackFinder, ntsos, 0);

	Trk::TrackInfo info;
	info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
        std::unique_ptr<Trk::Track> newTrack = std::make_unique<Trk::Track>(info, ntsos.release(), nullptr);

	// Careful refitting of the TRT stand alone track
	if (m_doRefit) {
                newTrack = std::unique_ptr<Trk::Track>( m_fitterTool->fit(*newTrack, false, Trk::pion));
		if (!newTrack) {
			ATH_MSG_DEBUG ("Refit of TRT track segment failed!");
			return nullptr;
		}

		//Protect for tracks that have no really defined locz and theta parameters
		const Trk::Perigee* perTrack=newTrack->perigeeParameters();
		if (perTrack) {
			const AmgSymMatrix(5)* CM = perTrack->covariance();
                        if (!CM || sqrt((*CM)(1,1)) == 0. || sqrt((*CM)(3,3)) == 0.) {
                           return nullptr;
                        }
                }
        }

        return newTrack.release();
}

///////////////////////////////////////////////////////////////////
// Merge a Si extension and a TRT track extension.Refit at the end
///////////////////////////////////////////////////////////////////

Trk::Track* InDet::TRT_SeededTrackFinder::
mergeExtension(const Trk::Track& tT, std::vector<const Trk::MeasurementBase*>& tS) const {
  // TSOS from the track
  const DataVector<const Trk::TrackStateOnSurface>* stsos = tT.trackStateOnSurfaces();
  // fitQuality from track
  const Trk::FitQuality* fq = tT.fitQuality()->clone();
  // output datavector of TSOS
  std::unique_ptr<DataVector<const Trk::TrackStateOnSurface> >
     ntsos = std::make_unique<DataVector<const Trk::TrackStateOnSurface> >();

  int siHits = 0;
  // copy track Si states into track
  DataVector<const Trk::TrackStateOnSurface>::const_iterator p_stsos;
  for (p_stsos = stsos->begin(); p_stsos != stsos->end(); ++p_stsos) {
    ntsos->push_back((*p_stsos)->clone());
    if ((*p_stsos)->type(Trk::TrackStateOnSurface::Measurement)) siHits++;
  }

  // loop over TRT track extension
  for (int it = 0; it < int(tS.size()); it++) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
    const Trk::TrackStateOnSurface* seg_tsos = new Trk::TrackStateOnSurface(tS[it]->clone(), 0, 0, 0, typePattern);
    ntsos->push_back(seg_tsos);
  }

  ///Construct the new track

  Trk::TrackInfo info;
  info.setPatternRecognitionInfo(Trk::TrackInfo::TRTSeededTrackFinder);
  std::unique_ptr<Trk::Track> newTrack( std::make_unique<Trk::Track>(info, ntsos.release(), fq) );

  //Careful refitting at the end
  if (m_doRefit) {
    newTrack = std::unique_ptr<Trk::Track>( m_fitterTool->fit(*newTrack, false, Trk::pion) ) ;
    if (!newTrack) {
      ATH_MSG_DEBUG ("Refit of TRT+Si track segment failed!");
      return nullptr;
    }

    //Protect for tracks that have no really defined locz and theta parameters
    const Trk::Perigee* perTrack=newTrack->perigeeParameters();
    if (perTrack) {
      const AmgSymMatrix(5)* CM = perTrack->covariance();
      if (!CM || sqrt((*CM)(1,1)) == 0. || sqrt((*CM)(3,3)) == 0.) {
	      return nullptr;
      }
    }
  }

  return newTrack.release();
}

///////////////////////////////////////////////////////////////////
// Analysis of tracks
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededTrackFinder::Analyze(TrackCollection* tC) const
{

  if(msgLvl(MSG::DEBUG)) {
  if(msgLvl(MSG::DEBUG)) {msg(MSG::DEBUG) << "Analyzing tracks..." << endmsg;}

  if(msgLvl(MSG::DEBUG)) {msg(MSG::DEBUG) << "Number of back tracks " << (tC->size()) << endl;}
  int tc = 0; //Track counter
  int nsct1, nsct2, nsct3, nsct4; //SCT layer counters
  int nsctTot1, nsctTot2, nsctTot3, nsctTot4; //SCT layer counters
  nsctTot1=nsctTot2=nsctTot3=nsctTot4=0;
  int npix1, npix2, npix3; //Pixel layer counters
  int npixTot1, npixTot2, npixTot3; //Pixel layer counters
  npixTot1=npixTot2=npixTot3=0;
  int nhits, nholes, noutl;
  ///Loop over tracks in track collection
  TrackCollection::const_iterator r    = tC->begin();
  TrackCollection::const_iterator re = tC->end();
  for (; r != re ; ++r){
    tc++; nsct1=nsct2=nsct3=nsct4=0; npix1=npix2=npix3=0; nhits=nholes=noutl=0;
    const DataVector<const Trk::TrackStateOnSurface>* newtsos = (*r)->trackStateOnSurfaces();
    if(!newtsos) continue;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itp, itpe=newtsos->end();
    for(itp=newtsos->begin(); itp!=itpe; itp++){
      ///Concentrate on the Si component of the track
      const InDet::SiClusterOnTrack* clus = dynamic_cast<const InDet::SiClusterOnTrack*>((*itp)->measurementOnTrack());
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Measurement))){  //Count the number of hits used in the track
	double rc = clus->globalPosition().perp();
        if((40.<=rc)&&(rc<80.)){npix1++;}    //1st pixel layer
        if((80.<=rc)&&(rc<100.)){npix2++;}   //2nd pixel layer
        if((100.<=rc)&&(rc<150.)){npix3++;}  //3rd pixel layer
        if((280.<=rc)&&(rc<340.)){nsct1++;}  //1st SCT layer
        if((340.<=rc)&&(rc<390.)){nsct2++;}  //2nd SCT layer
        if((390.<=rc)&&(rc<460.)){nsct3++;}  //3rd SCT layer
        if((460.<=rc)&&(rc<550.)){nsct4++;}  //4th SCT layer
        nhits++;
      }
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Outlier))){  //Count the total number of outliers per track
        noutl++;
      }
      if(clus && ((*itp)->type(Trk::TrackStateOnSurface::Hole))){     //Count the total number of holes per track
        nholes++;
      }
    }
    nsctTot1+=nsct1; nsctTot2+=nsct2; nsctTot3+=nsct3; nsctTot4+=nsct4;
    npixTot1+=npix1; npixTot2+=npix2; npixTot3+=npix3;
    //cout << "HITS IN TRACK " << nhits << " OUTLIERS " << noutl << " HOLES " << nholes << endl;
  }
  msg(MSG::DEBUG)<<"Total hits on 1st SCT: "<<nsctTot1<<" 2nd SCT: "<<nsctTot2<<" 3rd SCT: "<<nsctTot3<<" 4th SCT: "<<nsctTot4<<endmsg;
  msg(MSG::DEBUG)<<"Total hits on 1st Pixel: "<<npixTot1<<" 2nd Pixel: "<<npixTot2<<" 3rd Pixel: "<<npixTot3<<endmsg;
  }

}

