// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "HTTLogicalHitsProcessAlg.h"

#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTDataFlowInfo.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"

#include "HTTDataFlowTool.h"
#include "HTTNNTrackTool.h"
#include "HTTOverlapRemovalTool.h"
#include "HTTTrackFitterTool.h"

#include "TrigHTTConfTools/HTTRegionSlices.h"

#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"
#include "TrigHTTInput/HTTReadRawRandomHitsTool.h"

#include "TrigHTTMaps/HTTRegionMap.h"

#include "GaudiKernel/IEventProcessor.h"


#ifdef BENCHMARK_LOGICALHITSALG
#define TIME(name) \
    t_1 = std::chrono::steady_clock::now(); \
    (name) += std::chrono::duration_cast<std::chrono::microseconds>(t_1 - t_0).count(); \
    t_0 = t_1;

size_t m_tread = 0, m_tprocess = 0, m_troads = 0, m_troad_filter = 0, m_tlrt = 0, m_ttracks = 0, m_tOR = 0, m_t2ndStage = 0, m_tmon = 0, m_tfin = 0;
#else
#define TIME(name)
#endif


///////////////////////////////////////////////////////////////////////////////
// Initialize

HTTLogicalHitsProcessAlg::HTTLogicalHitsProcessAlg (const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}


StatusCode HTTLogicalHitsProcessAlg::initialize()
{
    std::stringstream ss(m_description);
    std::string line;
    ATH_MSG_INFO("Tag config:");
    if (m_description != nullptr) {
        while (std::getline(ss, line, '\n')) {
            ATH_MSG_INFO('\t' << line);
        }
    }

    ATH_CHECK(m_hitInputTool.retrieve());
    if (m_secondInputToolN > 0) ATH_CHECK(m_hitInputTool2.retrieve());
    ATH_CHECK(m_hitMapTool.retrieve());
    if (m_doHitFiltering) ATH_CHECK(m_hitFilteringTool.retrieve());
    if (m_clustering) ATH_CHECK(m_clusteringTool.retrieve());
    if (m_doSpacepoints) ATH_CHECK(m_spacepointsTool.retrieve());
    ATH_CHECK(m_roadFinderTool.retrieve());
    
    if (m_doLRT) {
        ATH_CHECK(m_LRTRoadFilterTool.retrieve());
        ATH_CHECK(m_LRTRoadFinderTool.retrieve());
    }
    if (m_doHoughRootOutput) ATH_CHECK(m_houghRootOutputTool.retrieve());
    if (m_doNNTrack) ATH_CHECK(m_NNTrackTool.retrieve());
    if (m_filterRoads) ATH_CHECK(m_roadFilterTool.retrieve());
    if (m_filterRoads2) ATH_CHECK(m_roadFilterTool2.retrieve());
    if (m_doTracking) ATH_CHECK(m_trackFitterTool_1st.retrieve());
    ATH_CHECK(m_overlapRemovalTool_1st.retrieve());
    if (m_runSecondStage) {
        if (m_doTracking) ATH_CHECK(m_trackFitterTool_2nd.retrieve());
        ATH_CHECK(m_overlapRemovalTool_2nd.retrieve());
    }
    ATH_CHECK(m_dataFlowTool.retrieve());
    ATH_CHECK(m_writeOutputTool.retrieve());

    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_evtSel.retrieve());

    ATH_MSG_DEBUG("initialize() Instantiating root objects");
    m_logicEventHeader_1st   = m_writeOutputTool->getLogicalEventInputHeader_1st();
    m_logicEventOutputHeader = m_writeOutputTool->getLogicalEventOutputHeader();
    if (m_runSecondStage) m_logicEventHeader_2nd = m_writeOutputTool->getLogicalEventInputHeader_2nd();

    ATH_MSG_DEBUG("initialize() Setting branch");

    if (m_outputHitTxt) {
      ATH_MSG_INFO("writing road hits to " << m_outputHitTxtName);
      m_outputHitTxtStream.open(m_outputHitTxtName);
    }

    ATH_MSG_DEBUG("initialize() Finished");
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//                          MAIN EXECUTE ROUTINE                             //
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLogicalHitsProcessAlg::execute()
{
#ifdef BENCHMARK_LOGICALHITSALG
    std::chrono::time_point<std::chrono::steady_clock> t_0, t_1;
    t_0 = std::chrono::steady_clock::now();
#endif

    // Read inputs
    bool done = false;
    ATH_CHECK(readInputs(done));

    if (done) {
      IEventProcessor* appMgr = nullptr;
      ATH_CHECK(service("ApplicationMgr",appMgr));
      if (!appMgr) {
          ATH_MSG_ERROR("Failed to retrieve ApplicationMgr as IEventProcessor");
          return StatusCode::FAILURE;
      }
      return appMgr->stopRun();
    }

    // Apply truth track cuts
    if (!m_evtSel->selectEvent(&m_eventHeader))
    {
        ATH_MSG_DEBUG("Event skipped by HTTEventSelectionSvc");
        return StatusCode::SUCCESS;
    }

    TIME(m_tread);

    // Map, cluster, and filter hits
    ATH_CHECK(processInputs());

    // Get reference to hits
    unsigned regionID = m_evtSel->getRegionID();
    std::vector<HTTHit> const & hits_1st = m_logicEventHeader_1st->towers().at(regionID).hits();

    TIME(m_tprocess);

    // Get roads
    std::vector<HTTRoad*> prefilter_roads;
    std::vector<HTTRoad*>& roads_1st = prefilter_roads;
    std::vector<const HTTHit*> phits_1st;
    for (HTTHit const & h : hits_1st) phits_1st.push_back(&h);
    ATH_CHECK(m_roadFinderTool->getRoads(phits_1st, roads_1st));

    TIME(m_troads);

    // Standard road Filter
    std::vector<HTTRoad*> postfilter_roads;
    if (m_filterRoads)
    {
        ATH_MSG_DEBUG("Filtering roads");
        ATH_CHECK(m_roadFilterTool->filterRoads(roads_1st, postfilter_roads));
        roads_1st = postfilter_roads;
    }
    ATH_CHECK(m_overlapRemovalTool_1st->runOverlapRemoval(roads_1st));

    // Road Filter2
    std::vector<HTTRoad*> postfilter2_roads;
    if (m_filterRoads2) {
        ATH_CHECK(m_roadFilterTool2->filterRoads(roads_1st, postfilter2_roads));
        roads_1st = postfilter2_roads;
    }

    TIME(m_troad_filter);

    // Get tracks
    std::vector<HTTTrack> tracks_1st;
    if (m_doTracking) {
        if (m_doNNTrack) {
            ATH_MSG_DEBUG("Performing NN tracking");
            const HTTNNMap* nnMap = m_HTTMapping->NNMap();
            ATH_CHECK(m_NNTrackTool->getTracks(roads_1st, tracks_1st, nnMap));
        }
        else {
            ATH_CHECK(m_trackFitterTool_1st->getTracks(roads_1st, tracks_1st));
        }
    }

    TIME(m_ttracks);

    // Overlap removal
    ATH_CHECK(m_overlapRemovalTool_1st->runOverlapRemoval(tracks_1st));

    TIME(m_tOR);

    // Now, we may want to do large-radius tracking on the hits not used by the first stage tracking.
    // This follows overlap removal.
    std::vector<HTTRoad*> roadsLRT;
    std::vector<HTTTrack> tracksLRT; // currently empty
    if (m_doLRT) {
        // Filter out hits that are on successful first-stage tracks
        std::vector<const HTTHit*> remainingHits;

        if (m_doLRTHitFiltering) {
            ATH_MSG_DEBUG("Doing hit filtering based on prompt tracks.");
            ATH_CHECK(m_LRTRoadFilterTool->filterUsedHits(tracks_1st, phits_1st, remainingHits));
        } else {
            ATH_MSG_DEBUG("No hit filtering requested; using all hits for LRT.");
            remainingHits = phits_1st;
        }

        // Get LRT roads with remaining hits
        ATH_MSG_DEBUG("Finding LRT roads");
        ATH_CHECK(m_LRTRoadFinderTool->getRoads( remainingHits, roadsLRT ));
    }

    TIME(m_tlrt);

    HTTDataFlowInfo* dataFlowInfo = new HTTDataFlowInfo();

    // Second stage fitting
    std::vector<HTTRoad*> roads_2nd;
    std::vector<HTTTrack> tracks_2nd;
    if (m_runSecondStage) {
        ATH_CHECK(secondStageProcessing(tracks_1st, roads_2nd, tracks_2nd));
        TIME(m_t2ndStage);
    }

    // Calculate data flow quantities
    ATH_CHECK(m_dataFlowTool->calculateDataFlow(dataFlowInfo, m_logicEventHeader_1st, m_clusters_1st, roads_1st, tracks_1st, roads_2nd, tracks_2nd));

    // Write the output and reset
    ATH_CHECK(writeOutputData(roads_1st, tracks_1st, roads_2nd, tracks_2nd, dataFlowInfo));

    if (m_doHoughRootOutput) {
      ATH_CHECK(m_houghRootOutputTool->fillTree(roads_1st, m_logicEventHeader_1st->optional().getTruthTracks(), m_logicEventHeader_1st->optional().getOfflineTracks()));
    }


    // dump hit identifiers per road/track to text file
    if(m_outputHitTxt) {

      ATH_MSG_DEBUG("Dumping hits in roads to text file " << m_outputHitTxtName);
      m_outputHitTxtStream << std::endl << "event " << m_ev << ", "
                           << roads_1st.size() << " roads"; // endl moved below to deal with trailing comma

      // m_clusters_original exists, we just need to know which ones to access
      // HTTRoad::getHits returns a vector of HTTHit, but each of them is actually a cluster
      // m_parentageMask has been co-opted to store the cluster index (set in clustering tool)
      // this lets us get hold of the actual HTTCluster via m_clusters_original[index]

      int roadCount = 0;
      for(auto &road : roads_1st) {
        // to avoid empty entries if a layer has no hits in a road, save the hits to vector first
        std::vector< std::vector<HTTHit> > clusters_in_road;
        for(int layer = 0; layer < int(road->getNLayers()); layer ++) {
          for(auto &cluster_as_hit : road->getHits(layer)) {
            unsigned clusterIndex = (unsigned)cluster_as_hit->getParentageMask();
            HTTCluster thisCluster = m_clusters_1st_original.at(clusterIndex);
            // check that they match (might slow things down unnecessarily - remove later perhaps)
            if( (cluster_as_hit->getR() != thisCluster.getClusterEquiv().getR()) ||
                (cluster_as_hit->getZ() != thisCluster.getClusterEquiv().getZ()) ||
                (cluster_as_hit->getGPhi() != thisCluster.getClusterEquiv().getGPhi()) ){
              ATH_MSG_ERROR("cluster retrieved from index seems not to match");
              ATH_MSG_DEBUG("road ID " << road->getRoadID() << ", layer " << layer
			    << ", cluster index " << clusterIndex << "road.getHits(layer):  " << cluster_as_hit->getR() << " "
			    << cluster_as_hit->getZ() << " " << cluster_as_hit->getGPhi());
              ATH_MSG_DEBUG("m_cluster[index]: " << thisCluster.getClusterEquiv().getR() << " "
			    << thisCluster.getClusterEquiv().getZ() << " "
			    << thisCluster.getClusterEquiv().getGPhi());
            }
            clusters_in_road.push_back(thisCluster.getHitList());
          }
        }

        if(roadCount != 0) m_outputHitTxtStream << ",";
        m_outputHitTxtStream << std::endl << "[ ";
        bool firstCluster = true;
        for(auto &clusterHits : clusters_in_road) {
          if(firstCluster) firstCluster = false;
          else             m_outputHitTxtStream << ", ";
          bool firstHit = true;
          m_outputHitTxtStream << "[ ";
          for(auto &hit : clusterHits) {
            if(firstHit) firstHit = false;
            else         m_outputHitTxtStream << ", ";

            // In the absence of hit.getIdentifier hash, need to get it back from
            // HTTIdentifierHash = (m_identifierHash << 2) + m_etaIndex; // see HTTHit.cxx
            // unsigned identifierHash = hit.getHTTIdentifierHash();
            // if(hit.isStrip())
            //   identifierHash = (hit.getHTTIdentifierHash() - hit.getEtaIndex()) >> 2;
            // But instead can just use hit.getIdentifierHash() having uncommented it from HTTHit.h
            m_outputHitTxtStream << "[" << hit.isStrip() << ", " << hit.getIdentifierHash() << ", "
                                 << hit.getEtaIndex() << ", " << hit.getPhiIndex() << "]";
          }
          m_outputHitTxtStream << " ]";
        }
        m_outputHitTxtStream << " ]";
        roadCount++;
      }
    }

    // Reset data pointers
    m_eventHeader.reset();
    m_logicEventHeader_1st->reset();
    m_logicEventOutputHeader->reset();
    if (m_runSecondStage) m_logicEventHeader_2nd->reset();

    TIME(m_tfin);

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//                  INPUT PASSING, READING AND PROCESSING                    //
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTLogicalHitsProcessAlg::readInputs(bool & done)
{
    if (m_ev % m_firstInputToolN == 0)
    {
        // Read primary input
        ATH_CHECK(m_hitInputTool->readData(&m_firstInputHeader, done));
        if (done)
        {
            ATH_MSG_INFO("Cannot read more events from file, returning");
            return StatusCode::SUCCESS; // end of loop over events
        }
    }

    m_eventHeader = m_firstInputHeader;

    // Read secondary input
    for (int i = 0; i < m_secondInputToolN; i++)
    {
        ATH_CHECK(m_hitInputTool2->readData(&m_eventHeader, done, false));
        if (done)
        {
            ATH_MSG_INFO("Cannot read more events from file, returning");
            return StatusCode::SUCCESS;
        }
    }

    m_ev++;

    return StatusCode::SUCCESS;
}


// Applies clustering, mapping, hit filtering, and space points
StatusCode HTTLogicalHitsProcessAlg::processInputs()
{
    m_clusters_1st.clear();
    m_spacepoints_1st.clear();
    m_hits_1st_miss.clear();

    // Map hits
    m_logicEventHeader_1st->reset();
    ATH_CHECK(m_hitMapTool->convert(1, m_eventHeader, *m_logicEventHeader_1st));
    if (!m_runSecondStage) m_eventHeader.clearHits();

    // Random removal of hits
    if (m_doHitFiltering) {
        ATH_CHECK(m_hitFilteringTool->DoRandomRemoval(*m_logicEventHeader_1st, true));
    }

    // Clustering
    if (m_clustering)
    {
        ATH_CHECK(m_clusteringTool->DoClustering(*m_logicEventHeader_1st, m_clusters_1st));
        m_clusters_1st_original = m_clusters_1st;
        // I think I also want to pass m_clusters to random removal (but won't work currently)
        if (m_doHitFiltering) ATH_CHECK(m_hitFilteringTool->DoRandomRemoval(*m_logicEventHeader_1st, false));
    }

    // Filter hits/clusters (untested for hits, ie with m_clustering = false)
    if (m_doHitFiltering)
    {
        // get the sets of layers that we want to filter hits from
        std::vector<int> filter_pixel_physLayers, filter_strip_physLayers;
        const HTTPlaneMap *planeMap_1st = m_HTTMapping->PlaneMap_1st();
        ATH_CHECK(m_hitFilteringTool->GetPairedStripPhysLayers(planeMap_1st, filter_strip_physLayers));
        m_clusters_1st.clear();
        ATH_CHECK(m_hitFilteringTool->DoHitFiltering(*m_logicEventHeader_1st, filter_pixel_physLayers, filter_strip_physLayers, m_clusters_1st));
    }

    // Space points
    if (m_doSpacepoints) ATH_CHECK(m_spacepointsTool->DoSpacePoints(*m_logicEventHeader_1st, m_spacepoints_1st));

    return StatusCode::SUCCESS;
}


StatusCode HTTLogicalHitsProcessAlg::secondStageProcessing(std::vector<HTTTrack> const & tracks_1st,
                                                           std::vector<HTTRoad*> & roads_2nd, std::vector<HTTTrack> & tracks_2nd)
{
    m_clusters_2nd.clear();
    m_spacepoints_2nd.clear();
    m_hits_2nd_miss.clear();

    // Map hits
    m_logicEventHeader_2nd->reset();
    ATH_CHECK(m_hitMapTool->convert(2, m_eventHeader, *m_logicEventHeader_2nd));
    m_eventHeader.clearHits();

    if (m_clustering) ATH_CHECK(m_clusteringTool->DoClustering(*m_logicEventHeader_2nd, m_clusters_2nd));

    // Space points
    if (m_doSpacepoints) ATH_CHECK(m_spacepointsTool->DoSpacePoints(*m_logicEventHeader_2nd, m_spacepoints_2nd));

    std::vector<HTTHit> const & hits_2nd = m_logicEventHeader_2nd->towers().at(0).hits();

    ATH_MSG_DEBUG("Number of second stage hits = " << hits_2nd.size());

    std::vector<const HTTHit*> phits_2nd;
    for (HTTHit const & h : hits_2nd) phits_2nd.push_back(&h);

    // Get the first stage tracks after OR
    std::vector<HTTTrack> tracks_1st_OR;
    for (auto track : tracks_1st) {
        if (track.passedOR() == 1) tracks_1st_OR.push_back(track);
    }

    ATH_MSG_DEBUG("Number of roads for second stage fitting " << roads_2nd.size());

    // Get second stage tracks
    if (m_doTracking) ATH_CHECK(m_trackFitterTool_2nd->getTracks(roads_2nd, tracks_2nd));

    ATH_MSG_DEBUG("Number of tracks from the second stage fitting = " << tracks_2nd.size() << " and truth tracks = " << m_logicEventHeader_1st->optional().getTruthTracks().size());

    // Overlap removal
    ATH_CHECK(m_overlapRemovalTool_2nd->runOverlapRemoval(tracks_2nd));

    return StatusCode::SUCCESS;
}

StatusCode HTTLogicalHitsProcessAlg::writeOutputData(std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack> const & tracks_1st,
                                                     std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack> const & tracks_2nd,
                                                     HTTDataFlowInfo const * dataFlowInfo)
{
  m_logicEventOutputHeader->reset();
  
  ATH_MSG_DEBUG("NHTTRoads_1st = " << roads_1st.size() << ", NHTTTracks_1st = " << tracks_1st.size() << ", NHTTRoads_2nd = " << roads_2nd.size() << ", NHTTTracks_2nd = " << tracks_2nd.size());
  
  if (!m_writeOutputData) return StatusCode::SUCCESS;
  
  m_logicEventOutputHeader->reserveHTTRoads_1st(roads_1st.size());
  m_logicEventOutputHeader->reserveHTTTracks_1st(tracks_1st.size());
  
  m_logicEventOutputHeader->addHTTRoads_1st(roads_1st);
  m_logicEventOutputHeader->addHTTTracks_1st(tracks_1st);
  
  if (m_runSecondStage) {
    m_logicEventOutputHeader->reserveHTTRoads_2nd(roads_2nd.size());
    m_logicEventOutputHeader->reserveHTTTracks_2nd(tracks_2nd.size());
    
    m_logicEventOutputHeader->addHTTRoads_2nd(roads_2nd);
    m_logicEventOutputHeader->addHTTTracks_2nd(tracks_2nd);
  }

  m_logicEventOutputHeader->setDataFlowInfo(*dataFlowInfo);
  ATH_MSG_DEBUG(m_logicEventOutputHeader->getDataFlowInfo());

  ATH_CHECK(m_writeOutputTool->writeData(m_logicEventHeader_1st, m_logicEventHeader_2nd, m_logicEventOutputHeader));

  return StatusCode::SUCCESS;
}




///////////////////////////////////////////////////////////////////////////////
// Finalize

StatusCode HTTLogicalHitsProcessAlg::finalize()
{
#ifdef BENCHMARK_LOGICALHITSALG
    ATH_MSG_INFO("Timings:" <<
            "\nread input:   " << std::setw(10) << m_tread <<
            "\nprocess hits: " << std::setw(10) << m_tprocess <<
            "\nroads:        " << std::setw(10) << m_troads <<
            "\nroad filter:  " << std::setw(10) << m_troad_filter <<
            "\nllp:          " << std::setw(10) << m_tlrt <<
            "\ntracks:       " << std::setw(10) << m_ttracks <<
            "\nOR:           " << std::setw(10) << m_tOR <<
            (m_runSecondStage ? : ("\n2ndStage:           " << std::setw(10) << m_t2ndStage) : "") <<
            "\nmon:          " << std::setw(10) << m_tmon <<
            "\nfin:          " << std::setw(10) << m_tfin
    );
#endif

    if (m_outputHitTxt) m_outputHitTxtStream.close();

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers

void HTTLogicalHitsProcessAlg::printHitSubregions(std::vector<HTTHit> const & hits)
{
    ATH_MSG_WARNING("Hit regions:");
    for (auto hit : hits)
    {
        std::vector<uint32_t> regions = m_HTTMapping->SubRegionMap()->getRegions(hit);
        std::stringstream ss;
        for (auto r : regions)
            ss << r << ",";
        ATH_MSG_WARNING("\t[" << ss.str() << "]");
    }
}
