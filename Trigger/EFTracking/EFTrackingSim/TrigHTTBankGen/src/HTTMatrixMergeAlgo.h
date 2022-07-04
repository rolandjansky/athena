// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMatrixMergeAlgo_h
#define HTTMatrixMergeAlgo_h

/**
 * @file HTTMatrixMergeAlgo.h
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 8th, 2020
 * @brief Algorithm to merge matrix files, to be used for sector and constant generation.
 *
 * This algorithm merges multiple matrix files. See HTTMatrixGenAlgo.h for information
 * on the matrix file. This class uses the same map structure to accumulate the data.
 *
 * This algorithm doesn't run on events; the functinality takes place in initialize()
 * and finalize().
 */


#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "HTTMatrixIO.h"

#include "TTree.h"
#include "TH1I.h"
#include "TH1F.h"

#include <string>
#include <vector>

class ITHistSvc;


class HTTMatrixMergeAlgo : public AthAlgorithm
{
    public:
        HTTMatrixMergeAlgo(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTMatrixMergeAlgo() = default;
        StatusCode initialize() override;
	// Execute does not do anything for this alg. This class does not process events, everything is done in init and finalize
        StatusCode execute() override;
        StatusCode finalize() override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Objects

        // Main logic. For each sector, store a struct that accumulates the track parameters, hit coordinates, etc.
        std::vector<AccumulateMap> m_sector_cum; // Index by region

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<ITHistSvc> m_tHistSvc{this,"THistSvc","THistSvc"};

        ///////////////////////////////////////////////////////////////////////
        // Configuration

	Gaudi::Property<std::vector<std::string> > m_fpath {this, "file_path",{},"vector of files to merge"};
	Gaudi::Property<bool> m_allregion {this, "allregion",false, "Run all regions?"};
	Gaudi::Property<int> m_region {this, "region", 0, "Region to run"};
	Gaudi::Property<int> m_nRegions {this, "nbank", 96, "Number of regions"};
	Gaudi::Property<bool> m_Monitor {this, "Monitor", false, "flag to enable the monitor"};


        int m_region_start = 0;
        int m_region_end = 0;

        int m_nFiles = 0;
        int m_nLayers = 0;
        int m_nDim = 0;
        int m_nDim2 = 0;

        ///////////////////////////////////////////////////////////////////////
        // Meta Data

        // Histograms
        TH1I* m_h_nSector = nullptr;
        TH1I* m_h_nHit = nullptr;
        TH1F* m_h_c = nullptr;
        TH1F* m_h_d = nullptr;
        TH1F* m_h_phi = nullptr;
        TH1F* m_h_coto = nullptr;
        TH1F* m_h_z = nullptr;

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        StatusCode bookHistograms();
        StatusCode copySliceTree(TFile *file);
        void readFiles();
        void readTree(TTree *matrix_tree, int region);
        std::vector<TTree*> createMatrixTrees();
        void fillMatrixTrees(std::vector<TTree*> const & matrixTrees);
};


#endif // HTTMatrixMergeAlgo_h

