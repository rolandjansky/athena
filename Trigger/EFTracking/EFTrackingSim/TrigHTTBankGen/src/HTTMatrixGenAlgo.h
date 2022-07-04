// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMatrixGenAlgo_h
#define HTTMatrixGenAlgo_h

/**
 * @file HTTMatrixGenAlgo.h
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 8th, 2020
 * @brief Algorithm to generate matrix files, to be used for sector and constant generation.
 *
 * This algorithm uses muon truth tracks to generate matrix files. The matrix files
 * contain the sector definitions (list of modules) and an accumulation of the tracks'
 * hits and track parameters. These are later used by ConstGenAlgo to generate fit constants.
 *
 * For each sector, we store the information in the MatrixAccumulator struct. Each track
 * found in the sector has its parameters and hit coordinates added/appended to the struct.
 */


#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTMaps/HTTRegionMap.h"
#include "TrigHTTSGInput/ITrigHTTInputTool.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"
#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTHough/IHTTRoadFinderTool.h"
#include "HTTMatrixIO.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1I.h"

#include <string>
#include <vector>


class ITHistSvc;
class TH1F;
class TH2F;
class ITrigHTTMappingSvc;
class IHTTRoadFinderTool;

class HTTMatrixGenAlgo : public AthAlgorithm
{
    public:
        HTTMatrixGenAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTMatrixGenAlgo() = default;

        StatusCode initialize() override;
        StatusCode execute() override;
        StatusCode finalize() override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Objects

        // Main logic. For each sector, store a struct that accumulates the track parameters, hit coordinates, etc.
        std::vector<AccumulateMap> m_sector_cum; // Index by region

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<ITrigHTTMappingSvc>    m_HTTMapping{this,"TrigHTTMappingSvc","TrigHTTMappingSvc"};
        ServiceHandle<IHTTEventSelectionSvc> m_EvtSel{this,"HTTEventSelectionSvc","HTTEventSelectionSvc"};
        ServiceHandle<ITHistSvc>             m_tHistSvc{this,"THistSvc","THistSvc"};

        ToolHandle<ITrigHTTInputTool>       m_hitInputTool {this, "HTTSGToRawHitsTool", "HTTSGToRawHitsTool/HTTSGToRawHits", "input handler"};
	ToolHandle<HTTRawToLogicalHitsTool> m_hitMapTool {this, "HTTRawToLogicalHitsTool", "HTTRawToLogicalHitsTool/HTT_RawToLogicalHitsTool", "HTT_RawToLogicalHitsTool"};
        ToolHandle<HTTClusteringToolI>       m_clusteringTool { this, "HTTClusteringFTKTool", "HTTClusteringFTKTool/HTTClusteringFTKTool", "HTTClusteringFTKTool" };
	ToolHandle<IHTTRoadFinderTool>       m_roadFinderTool {this, "RoadFinder", "RoadFinder"};
	const HTTPlaneMap* m_pmap = nullptr; // alias to m_HTTMapping->PlaneMap();


        ///////////////////////////////////////////////////////////////////////
        // Configuration
	Gaudi::Property<int> m_nRegions {this, "NBanks", 0, "Number of banks to make"};
	Gaudi::Property<bool> m_doClustering {this, "Clustering", true, "Do cluster?"};
	Gaudi::Property<int> m_ideal_geom {this, "IdealiseGeometry", 0, "Ideal geo flag, 0 is non, 1 is 1st order, 2 is 2nd order"};
	Gaudi::Property<bool> m_single {this, "SingleSector", false, "Run single sector"};
	Gaudi::Property<bool> m_doHoughConstants {this, "HoughConstants", false, "If true will do the matrix for the delta global phis method"};
	Gaudi::Property<int> m_MaxWC {this, "WCmax", 0, "Max number of WCs"};
	Gaudi::Property<float> m_PT_THRESHOLD {this, "PT_THRESHOLD", 0., "Min pt"};
	Gaudi::Property<float> m_D0_THRESHOLD {this, "D0_THRESHOLD", 1., "Max d0"};
	Gaudi::Property<int> m_TRAIN_PDG {this, "TRAIN_PDG", 0, "PDG of particles to train on"};
	Gaudi::Property<float> m_temp_c_min {this, "par_c_min", -1, "Min curvature"};
	Gaudi::Property<float> m_temp_c_max {this, "par_c_max", 1, "Max curvature"};
	Gaudi::Property<float> m_temp_phi_min {this, "par_phi_min", -TMath::Pi(), "Min phi"};
	Gaudi::Property<float> m_temp_phi_max {this, "par_phi_max", TMath::Pi(), "Max phi"};
	Gaudi::Property<float> m_temp_d0_min {this, "par_d0_min", -2, "Min d0"};
	Gaudi::Property<float> m_temp_d0_max {this, "par_d0_max", 2, "Max d0"};
	Gaudi::Property<float> m_temp_z0_min {this, "par_z0_min", -200, "Min z0"};
	Gaudi::Property<float> m_temp_z0_max {this, "par_z0_max", 200, "Max z0"};
	Gaudi::Property<float> m_temp_eta_min {this, "par_eta_min", -5, "Min eta"};
	Gaudi::Property<float> m_temp_eta_max {this, "par_eta_max", 5, "Max eta"};
	Gaudi::Property<int> m_temp_c_slices {this, "par_c_slices", 100, "Number of c slides"};
	Gaudi::Property<int> m_temp_phi_slices {this, "par_phi_slices", 100, "Number of phi slices"};
	Gaudi::Property<int> m_temp_d0_slices {this, "par_d0_slices", 100, "Number of d0 slices"};
	Gaudi::Property<int> m_temp_z0_slices {this, "par_z0_slices", 100, "Number of z0 slices"};
	Gaudi::Property<int> m_temp_eta_slices {this, "par_eta_slices", 100, "Number of eta slices"};
	
	int m_nLayers = 0;
	int m_nDim = 0;
	int m_nDim2 = 0; // m_nDim ^ 2
	
        HTTTrackPars m_sliceMin = 0;
        HTTTrackPars m_sliceMax = 0;
        HTTTrackParsI m_nBins;

        HTTEventInputHeader*         m_eventHeader = nullptr;

        ///////////////////////////////////////////////////////////////////////
        // Meta Data

        size_t m_nTracks = 0; // Total number of truth tracks encountered
        size_t m_nTracksUsed = 0; // Total number of tracks after filtering, i.e. used to fit constants

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        enum class selectHit_returnCode { SH_FAILURE, SH_KEEP_OLD, SH_KEEP_NEW };

        StatusCode bookHistograms();
        std::vector<HTTHit> getLogicalHits() ;
        std::vector<HTTTruthTrack> filterTrainingTracks(std::vector<HTTTruthTrack> const & truth_tracks) const;
        std::map<int, std::vector<HTTHit>> makeBarcodeMap(std::vector<HTTHit> const & hits, std::vector<HTTTruthTrack> const & tracks) const;
        selectHit_returnCode selectHit(HTTHit const & old_hit, HTTHit const & new_hit) const;
        bool filterSectorHits(std::vector<HTTHit> const & all_hits, std::vector<HTTHit> & sector_hits, HTTTruthTrack const & t) const;
        int getRegion(std::vector<HTTHit> const & hits) const;
	StatusCode makeAccumulator(std::vector<HTTHit> const & sector_hits, HTTTruthTrack const & track, std::pair<std::vector<module_t>, HTTMatrixAccumulator> & accumulator) const;
        std::vector<TTree*> createMatrixTrees();
        void fillMatrixTrees(std::vector<TTree*> const & matrixTrees);
        void writeSliceTree();

        ///////////////////////////////////////////////////////////////////////
        // Histograms

        // These are ordered as in HTTTrackPars, phi, curvature, d0, z0, eta
        TH1I* m_h_trainingTrack[HTTTrackPars::NPARS];
        TH1I* m_h_sectorPars[HTTTrackPars::NPARS];
        TH1I* m_h_SHfailure[HTTTrackPars::NPARS];
        TH1I* m_h_3hitsInLayer[HTTTrackPars::NPARS];
        TH1I* m_h_notEnoughHits[HTTTrackPars::NPARS];

        TH1I* m_h_trackQoP_okHits = nullptr;
        TH1I* m_h_trackQoP_okRegion = nullptr;
        TH1I* m_h_nHit = nullptr;
};

#endif // HTTMatrixGenAlgo_h
