// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTNNTRACKTOOL_H
#define HTTNNTRACKTOOL_H

/**
 * @file HTTNNTrackTool.h
 * @author Elliott Cheu
 * @date April 27 2021
 * @brief Utilize NN score to build track candidates
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTMultiTruth.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTBanks/HTTSectorBank.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"


class ITrigHTTMappingSvc;

class HTTNNTrackTool : public AthAlgTool
{
  public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTNNTrackTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
	StatusCode getTracks(std::vector<HTTRoad*> &roads, std::vector<HTTTrack> &tracks, 
			     const HTTNNMap *nnMap);

        static float getXScale() { return 1015;};
        static float getYScale() { return 1015;};
        static float getZScale() { return 3000;};

	// Flags

	Gaudi::Property <double> m_NNCut { this, "NNCut", 0.0, " NN output value to cut on when selecting good tracks"};
	Gaudi::Property <double> m_chi2_scalefactor { this, "Chi2ScaleFactor", 40/(1-0.1), "Scale factor to use in converting to a chi2, Nominal chi2ndof cut is 40 and we want to use NN>0.0075 (or NN<(1-0.0075)"};


    private:

        ServiceHandle<ITrigHTTMappingSvc>   m_HTTMapping{this, "TrigHTTMappingSvc","TrigHTTMappingSvc"};
        ServiceHandle<ITHistSvc> m_tHistSvc{this, "THistSvc","THistSvc"};

	std::vector<float> m_x; // x position of hit in road
        std::vector<float> m_y; // y pos
        std::vector<float> m_z; // z pos
	std::vector<float> m_barcodefrac; // truth barcode fraction for the hit
	std::vector<int> m_barcode; // truth barcode for the hit
	std::vector<int> m_eventindex; // event index for the hit
	std::vector<unsigned int> m_isPixel; // is hit pixel? if 0 it is strip
	std::vector<unsigned int> m_layer; // layer ID
	std::vector<unsigned int> m_isBarrel; // is hit in barrel? if 0 it is endcap
	std::vector<unsigned int> m_etawidth;
	std::vector<unsigned int> m_phiwidth;
	std::vector<unsigned int> m_etamodule;
	std::vector<unsigned int> m_phimodule;
	std::vector<unsigned int> m_ID; // ID hash for hit

	std::vector<float> m_truth_d0;
	std::vector<float> m_truth_z0;
	std::vector<float> m_truth_pt;
	std::vector<float> m_truth_eta;
	std::vector<float> m_truth_phi;
	std::vector<float> m_truth_pdg;
	std::vector<int> m_truth_q;
	std::vector<int> m_truth_barcode;
	std::vector<int> m_truth_eventindex;

	//////////////////////////////////////////////////////////////////
	// NN stuff
        std::vector<const char*> m_input_node_names;
        std::vector<int64_t> m_input_node_dims;
        std::vector<const char*> m_output_node_names;

    void compute_truth(HTTTrack & newtrk) const;

};


#endif // HTTNNTRACKTOOL_H
