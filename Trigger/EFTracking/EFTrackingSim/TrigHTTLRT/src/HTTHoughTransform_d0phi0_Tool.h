// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTHOUGHTRANSFORM_D0PHI0_TOOL_H
#define HTTHOUGHTRANSFORM_D0PHI0_TOOL_H

/**
 * @file HTTHoughTransform_d0phi0_Tool.h
 * @brief Implements road finding using a Hough transform in d0 vs phi0, assuming q/pt = 0 
 *        (i.e. a straight track).
 *
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTHough/IHTTRoadFinderTool.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
  
#include "TFile.h"

#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;


class HTTHoughTransform_d0phi0_Tool : public extends <AthAlgTool, IHTTRoadFinderTool>
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTHoughTransform_d0phi0_Tool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        ///////////////////////////////////////////////////////////////////////
        // IHTTRoadFinderTool

        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

	ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc {this, "TrigHTTBankSvc", "TrigHTTBankSvc"};
	ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping {this, "TrigHTTMappingSvc", "TrigHTTMappingSvc"};


        ///////////////////////////////////////////////////////////////////////
        // Properties

	Gaudi::Property <int> m_subRegion { this, "subRegion", 0," -1 for entire region (no slicing)"};
	Gaudi::Property <float> m_tempMin_phi { this, "phi_min", 0, "min phi"};
	Gaudi::Property <float> m_tempMax_phi { this, "phi_max", 1, "max phi"};
	Gaudi::Property <float> m_tempMin_d0 { this, "d0_min", -1, "min q/pt"};
	Gaudi::Property <float> m_tempMax_d0 { this, "d0_max", 1, "max q/pt"};
	Gaudi::Property<std::vector<int> > m_threshold  { this, "threshold", {1},"Minimum number of hit layers to fire a road"};
	Gaudi::Property <unsigned int> m_imageSize_x { this, "nBins_x", 1, ""};
	Gaudi::Property <unsigned int> m_imageSize_y { this, "nBins_y", 1, ""};
	Gaudi::Property<std::vector<int> > m_conv  { this, "convolution", {}, "Convolution filter, with size m_convSize_y * m_convSize_x"};
	Gaudi::Property<std::vector<unsigned int> > m_combineLayers  { this, "combine_layers", {0,1,2,3,4,5,6,7}, ""};
	Gaudi::Property<std::vector<unsigned int> > m_binScale  { this, "scale", {}, "Vector containing the scales for each layers"};
	Gaudi::Property <unsigned int> m_convSize_x { this, "convSize_x", 0, ""};
	Gaudi::Property <unsigned int> m_convSize_y { this, "convSize_y", 0, ""};
	Gaudi::Property<std::vector<unsigned int> > m_hitExtend_x  { this, "hitExtend_x", {}, "Hit lines will fill extra bins in x by this amount on each side, size == nLayers"};
	Gaudi::Property <bool> m_traceHits { this, "traceHits", true, "Trace each hit that goes in a bin. Disabling this will save memory/time since each bin doesn't have to store all its hits but the roads created won't have hits from convolution, etc."};
	Gaudi::Property <bool> m_stereo { this, "stereo", false, "Combine stereo layers"};
	Gaudi::Property <bool> m_localMaxWindowSize { this, "localMaxWindowSize", 0, "Only create roads that are a local maximum within this window size. Set this to 0 to turn off local max filtering"};
	Gaudi::Property <bool> m_useSectors { this, "useSectors", false, "Will reverse calculate the sector for track-fitting purposes"};
	Gaudi::Property <bool> m_idealGeoRoads { this, "IdealGeoRoads", false, "Set sectors to use ideal geometry fit constants"};

        HTTTrackPars::pars_index m_par_x = HTTTrackPars::IPHI; // sets phi as the x variable
        HTTTrackPars::pars_index m_par_y = HTTTrackPars::ID0; // sets d0 as the y variable

        HTTTrackPars m_parMin; // These are the bounds of the image, i.e. the region of interest
        HTTTrackPars m_parMax; // Only the two parameters chosen above are used

        unsigned m_nCombineLayers = 0U; // number of layers after combined
        std::vector< std::vector<unsigned> > m_combineLayer2D; // 2d array of combined layers i.e. [[1,2,3],[0,4,5],[6,7]] will combine (L1, L2, L3), (L0, L4, L5), (L6, L7)

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        unsigned m_nLayers = 0U; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();

        double m_step_x = 0; // step size of the bin boundaries in x
        double m_step_y = 0; // step size of the bin boundaries in y
        std::vector<double> m_bins_x; // size == m_imageSize_x + 1.
        std::vector<double> m_bins_y; // size == m_imageSize_y + 1
            // Bin boundaries, where m_bins_x[i] is the lower bound of bin i.
            // These are calculated from m_parMin/Max.
        std::unordered_map<int, std::vector<size_t>> m_yBins_scaled; // saved all scaled binnings

        typedef vector2D<std::pair<int, std::unordered_set<const HTTHit*>>> Image;
            // An image is a 2d array of points, where each point has a value.
            // This starts as the number of hits (or hit layers), but will
            // change after the convolution. Also stored are all hits that
            // contributed to each bin.
            // Size m_imageSize_y * m_imageSize_x. (NOTE y is row coordinate)

        ///////////////////////////////////////////////////////////////////////
        // Event Storage

        std::vector<HTTRoad_Hough> m_roads;

        ///////////////////////////////////////////////////////////////////////
        // Metadata and Monitoring

        int m_event = 0;
        std::string m_name; // Gets the instance name from the full gaudi name
        TFile m_monitorFile;

        ///////////////////////////////////////////////////////////////////////
        // Core

        // std::vector<HTTHit const *> filterHits(std::vector<HTTHit const *> const & hits) const;
        Image createLayerImage(std::vector<unsigned> const & combine_layers, std::vector<HTTHit const *> const & hits, unsigned const scale) const;
        Image createImage(std::vector<HTTHit const *> const & hits) const;
        Image convolute(Image const & image) const;

        ///////////////////////////////////////////////////////////////////////
        // Helpers

        double yToX(double y, HTTHit const * h) const;
        std::pair<unsigned, unsigned> yToXBins(size_t yBin_min, size_t yBin_max, HTTHit const * hit) const;
        bool passThreshold(Image const & image, unsigned x, unsigned y) const;
        void matchIdealGeoSector(HTTRoad_Hough & r) const;
	HTTRoad_Hough createRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y) const;
        HTTRoad_Hough createRoad(std::vector<std::vector<const HTTHit*>> const & hits, layer_bitmask_t hitLayers, unsigned x, unsigned y) const;
        void addRoad(std::unordered_set<const HTTHit*> const & hits, unsigned x, unsigned y);
        void addRoad(std::vector<const HTTHit*> const & hits, unsigned x, unsigned y);
        int conv(unsigned y, unsigned x) { return m_conv[y * m_convSize_x + x]; } // NOTE: y index is first
        void drawImage(Image const & image, std::string const & name);

};



#endif // HTTHOUGHTRANSFORM_D0PHI0_TOOL_H
