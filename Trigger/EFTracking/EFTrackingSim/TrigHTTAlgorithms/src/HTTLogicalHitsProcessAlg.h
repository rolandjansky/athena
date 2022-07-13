// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTT_LOGICALHITSPROCESSALG_H
#define HTT_LOGICALHITSPROCESSALG_H

/*
 * Please put a description on what this class does
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTInput/IHTTEventOutputHeaderTool.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"
#include "TrigHTTHough/IHTTRoadFilterTool.h"
#include "TrigHTTHough/IHTTRoadFinderTool.h"
#include "TrigHTTMaps/HTTSpacePointsToolI.h"
#include "TrigHTTMaps/IHTTHitFilteringTool.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"
#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"
#include "TrigHTTInput/HTTReadRawRandomHitsTool.h"
#include "TrigHTTHough/HTTHoughRootOutputTool.h"
#include "TrigHTTLRT/HTTLLPRoadFilterTool.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"

#include <fstream>

class HTTDataFlowTool;
class HTTHoughRootOutputTool;
class HTTLLPRoadFilterTool;
class HTTNNTrackTool;
class HTTOverlapRemovalTool;
class HTTTrackFitterTool;
class HTTEtaPatternFilterTool;

class HTTCluster;
class HTTHit;
class HTTLogicalEventInputHeader;
class HTTLogicalEventOutputHeader;
class HTTRoad;
class HTTTrack;
class HTTDataFlowInfo;


class HTTLogicalHitsProcessAlg : public AthAlgorithm
{
    public:
        HTTLogicalHitsProcessAlg(const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~HTTLogicalHitsProcessAlg() = default;

        virtual StatusCode initialize() override;
        virtual StatusCode execute() override;
        virtual StatusCode finalize() override;

    private:

        std::string m_description;
        int m_ev = 0;

        // Handles
        ToolHandle<IHTTEventInputHeaderTool>    m_hitInputTool {this, "InputTool", "HTTSGToRawHitsTool/HTTSGToRawHitsTool", "Input Tool"};
	ToolHandle<HTTReadRawRandomHitsTool>    m_hitInputTool2 {this, "InputTool2", "HTTReadRawRandomHitsTool/HTTReadRawRandomHitsTool", "Potential 2nd input Tool to load data from more than one source"};
        ToolHandle<HTTRawToLogicalHitsTool>     m_hitMapTool {this, "RawToLogicalHitsTool", "HTT_RawToLogicalHitsTool/HTT_RawToLogicalHitsTool", "Raw To Logical Tool"};
        ToolHandle<IHTTHitFilteringTool>        m_hitFilteringTool {this, "HitFilteringTool", "HTTHitFilteringTool/HTTHitFilteringTool", "Hit Filtering Tool"};
        ToolHandle<HTTClusteringToolI>          m_clusteringTool {this, "ClusteringTool", "HTTClusteringTool/HTTClusteringTool", "Hit Clustering Tool"};
        ToolHandle<HTTSpacePointsToolI>         m_spacepointsTool {this, "SpacePointTool", "HTTSpacePointsTool/HTTSpacePointsTool", "Space Points Tool"};
        ToolHandle<IHTTRoadFinderTool>          m_roadFinderTool {this, "RoadFinder", "HTTPatternMatchTool", "Road Finder Tool"};
        ToolHandle<HTTLLPRoadFilterTool>        m_LRTRoadFilterTool {this, "LRTRoadFilter", "HTTLLPRoadFilterTool/HTTLLPRoadFilterTool", "LRT Road Filter Tool"};
        ToolHandle<IHTTRoadFinderTool>          m_LRTRoadFinderTool {this, "LRTRoadFinder", "HTTHoughTransform_d0phi0_Tool/HTTHoughTransform_d0phi0_Tool", "LRT Road Finder Tool"};
        ToolHandle<IHTTRoadFilterTool>          m_roadFilterTool {this, "RoadFilter", "HTTEtaPatternFilterTool", "Road Filter Tool"};
        ToolHandle<IHTTRoadFilterTool>          m_roadFilterTool2 {this, "RoadFilter2", "HTTPhiRoadFilterTool", "Road Filter2 Tool"};
        ToolHandle<HTTNNTrackTool>              m_NNTrackTool {this, "NNTrackTool", "HTTNNTrackTool/HTTNNTrackTool", "NN Track Tool"};
        ToolHandle<HTTHoughRootOutputTool>      m_houghRootOutputTool {this, "HoughRootOutputTool", "HTTHoughRootOutputTool/HTTHoughRootOutputTool", "Hough ROOT Output Tool"};
        ToolHandle<HTTTrackFitterTool>          m_trackFitterTool_1st {this, "TrackFitter_1st", "HTTTrackFitterTool/HTTTrackFitterTool_1st", "1st stage track fit tool"};
        ToolHandle<HTTTrackFitterTool>          m_trackFitterTool_2nd {this, "TrackFitter_2nd", "HTTTrackFitterTool/HTTTrackFitterTool_2nd", "2nd stage track fit tool"};
        ToolHandle<HTTOverlapRemovalTool>       m_overlapRemovalTool_1st {this, "OverlapRemoval_1st", "HTTOverlapRemovalTool/HTTOverlapRemovalTool_1st", "1st stage overlap removal tool"};
        ToolHandle<HTTOverlapRemovalTool>       m_overlapRemovalTool_2nd {this, "OverlapRemoval_2nd", "HTTOverlapRemovalTool/HTTOverlapRemovalTool_2nd", "2nd stage overlap removal tool"};
        ToolHandle<HTTDataFlowTool>             m_dataFlowTool {this, "DataFlowTool", "HTTDataFlowTool/HTTDataFlowTool", "Data Flow Tool"};
        ToolHandle<IHTTEventOutputHeaderTool>   m_writeOutputTool {this, "OutputTool", "HTTOutputHeaderTool/HTTOutputHeaderTool", "Output tool"};
        ServiceHandle<ITrigHTTMappingSvc>       m_HTTMapping {this, "HTTMapping", "TrigHTTMappingSvc", "TrigHTTMappingSvc"};
        ServiceHandle<IHTTEventSelectionSvc>    m_evtSel {this, "eventSelector", "HTTEventSelectionSvc", "Event selection Svc"};
        
        // Flags
	Gaudi::Property<int> m_firstInputToolN {this, "FirstInputToolN", 1, "number of times to use event from first input tool"};
	Gaudi::Property<int> m_secondInputToolN {this, "SecondInputToolN", 0, "number of times to use event from second input tool"};
        Gaudi::Property<bool> m_doHitFiltering {this, "HitFiltering", false, "flag to enable hit/cluster filtering"};
	Gaudi::Property<bool> m_clustering {this, "Clustering", false, "flag to enable the clustering"};
	Gaudi::Property<bool> m_doSpacepoints {this, "Spacepoints", false, "flag to enable the spacepoint formation"};
	Gaudi::Property<bool> m_doTracking {this, "tracking", false, "flag to enable the tracking"};
	Gaudi::Property<bool> m_doMissingHitsChecks {this, "DoMissingHitsChecks", false};
	Gaudi::Property<bool> m_filterRoads  {this, "FilterRoads", false, "enable first road filter"};
	Gaudi::Property<bool> m_filterRoads2  {this, "FilterRoads2", false,  "enable second road filter"};
	Gaudi::Property<bool> m_runSecondStage {this, "RunSecondStage", false,  "flag to enable running the second stage fitting"};
	Gaudi::Property<bool> m_doHoughRootOutput {this, "DoHoughRootOutput", false, "Dump output from the Hough Transform to flat ntuples"};
	Gaudi::Property<bool> m_doNNTrack  {this, "DoNNTrack", false, "Run NN track filtering"};
	Gaudi::Property<bool> m_doLRT {this, "doLRT", false, "Enable Large Radious Tracking"};
	Gaudi::Property<bool> m_doLRTHitFiltering {this, "LRTHitFiltering", false, "flag to enable hit/cluster filtering for LRT"};
        Gaudi::Property<bool> m_writeOutputData  {this, "writeOutputData", true,"write the output TTree"};
	Gaudi::Property<bool> m_outputHitTxt  {this, "outputHitTxt", false, "write out road hits to text file"};
	
        std::string m_outputHitTxtName = "outputRoadHits.txt";
        std::ofstream m_outputHitTxtStream;

        // ROOT pointers 
        HTTEventInputHeader          m_eventHeader;
        HTTEventInputHeader          m_firstInputHeader;
        HTTLogicalEventInputHeader*  m_logicEventHeader_1st = nullptr;
        HTTLogicalEventInputHeader*  m_logicEventHeader_2nd = nullptr;
        HTTLogicalEventOutputHeader* m_logicEventOutputHeader = nullptr;

        // Event storage
        std::vector<HTTCluster> m_clusters_1st, m_clusters_1st_original, m_clusters_2nd;
        std::vector<HTTCluster> m_spacepoints_1st, m_spacepoints_2nd;
        std::vector<HTTHit>     m_hits_1st_miss, m_hits_2nd_miss;
        std::vector<HTTTrack>   m_tracks_1st_guessedcheck, m_tracks_1st_nomiss, m_tracks_2nd_guessedcheck, m_tracks_2nd_nomiss;


        StatusCode readInputs(bool & done);
        StatusCode processInputs();
        StatusCode secondStageProcessing(std::vector<HTTTrack> const & tracks_1st,
                                         std::vector<HTTRoad*> & roads_2nd, std::vector<HTTTrack> & tracks_2nd);


        StatusCode writeOutputData(std::vector<HTTRoad*> const & roads_1st, std::vector<HTTTrack> const & tracks_1st,
                                   std::vector<HTTRoad*> const & roads_2nd, std::vector<HTTTrack> const & tracks_2nd,
                                   HTTDataFlowInfo const * dataFlowInfo);

        void printHitSubregions(std::vector<HTTHit> const & hits);
};

#endif // HTTLOGICALHITSTOALGORITHMS_h
