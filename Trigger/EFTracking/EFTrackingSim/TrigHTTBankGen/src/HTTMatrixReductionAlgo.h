// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMatrixReductionAlgo_h
#define HTTMatrixReductionAlgo_h

/**
 * @file HTTMatrixReductionAlgo.h
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 21st, 2020
 * @brief Algorithm to reduce matrix files from 2nd stage to 1st stage.
 *
 * This algorithm simply copies the sector matrices from the input file,
 * but only keeping 1st stage layers/coordinates. In python pseudocode,
 *          layer_filter = [ is1stStage(layer_2nd) for layer_2nd in range(# 2nd stage layers) ]
 *          modules_1st = modules_2nd[layer_filter]
 * and similarly for the coordinate-length members.
 *
 * It is questionable if this step is even needed. I imagine the constant generation can just
 * read in the 2nd stage matrices and do this step by itself. (Once we combine the plane maps
 * the filtering should be just a built-in function for the pmap class).
 */


#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "HTTMatrixAccumulator.h"

class TTree;

#include <string>
#include <vector>


class HTTMatrixReductionAlgo: public AthAlgorithm
{
    public:

        HTTMatrixReductionAlgo (const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~HTTMatrixReductionAlgo() = default;
        StatusCode initialize() override;
        StatusCode finalize() override;
	// Execute does not do anything for this alg. This class does not process events, everything is done in init and finalize
        StatusCode execute() override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping {this, "TrigHTTMappingSvc","TrigHTTMappingSvc"};
        ServiceHandle<ITHistSvc> m_tHistSvc {this, "THistSvc","THistSvc"};
        HTTPlaneMap const * m_pmap_1st = nullptr;
        HTTPlaneMap const * m_pmap_2nd = nullptr;

        ///////////////////////////////////////////////////////////////////////
        // Configuration
	Gaudi::Property<bool> m_allregion {this, "allregion", false,"Run all regions?"};
	Gaudi::Property<std::string> m_filePath {this, "LayerChange_path", "default filepath"};
	Gaudi::Property<int> m_region {this, "region", 0, "Region to run"};
	Gaudi::Property<int> m_nRegions {this, "nbank", 96, "Number of regions and thus banks to create"};

        int m_region_start = 0;
        int m_region_end = 0;

        ///////////////////////////////////////////////////////////////////////
        // Data

        std::vector<AccumulateMap> m_sector_cum; // 1st stage accumulate maps, size nRegion.

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        StatusCode copySliceTree(TFile *file);
        std::pair<std::vector<size_t>, std::vector<size_t>> matchStages();
        std::pair<std::vector<module_t>, HTTMatrixAccumulator> reduce(
                std::vector<module_t> const & modules_2nd, HTTMatrixAccumulator const & acc_2nd,
                std::vector<size_t> const & layers_1st_to_2nd, std::vector<size_t> const & coords_1st_to_2nd);
        void extract_1stStage(TFile *file);
};




#endif // HTTMatrixReductionAlgo_h

