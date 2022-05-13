// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTHOUGHTRANSFORM_D0PHI0_TOOL_H
#define HTTHOUGHTRANSFORMTOOL_D0PHI0_H

/**
 * @file HTTHoughTransform_d0phi0_Tool.h
 * @brief Implements road finding using a Hough transform in d0 vs phi0, assuming q/pt = 0 
 *        (i.e. a straight track).
 *
 * Declarations in this file:
 *      class HTTHoughTransform_d0phi0_Tool : public AthAlgTool, virtual public IHTTRoadFinderTool
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTHough/HTTRoadFinderToolI.h"

#include "TFile.h"

#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;


class HTTHoughTransform_d0phi0_Tool : public AthAlgTool, virtual public IHTTRoadFinderTool
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

        ServiceHandle<ITrigHTTBankSvc> m_HTTBankSvc;
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;

        ///////////////////////////////////////////////////////////////////////
        // Properties

        // ==== Hits ===
        int m_subRegion = 0; // -1 for entire region (no slicing)

        BooleanProperty m_traceHits = true; // Trace each hit that goes in a bin
            // Disabling this will save memory/time since each bin doesn't have to store all its hits
            // but the roads created won't have hits from convolution, etc.
        BooleanProperty m_stereo = false;

        // === Image ===
        HTTTrackPars::pars_index m_par_x = HTTTrackPars::IPHI; // sets phi as the x variable
        HTTTrackPars::pars_index m_par_y = HTTTrackPars::ID0; // sets d0 as the y variable

        HTTTrackPars m_parMin; // These are the bounds of the image, i.e. the region of interest
        HTTTrackPars m_parMax; // Only the two parameters chosen above are used

        unsigned m_imageSize_x = 0; // i.e. number of bins in phi_track
        unsigned m_imageSize_y = 0; // i.e. number of bins in q/pT

        // === Smearing ===
        unsigned m_convSize_x = 0; // Size of convolution
        unsigned m_convSize_y = 0; //
        std::vector<int> m_conv; // Convolution filter, with size m_convSize_y * m_convSize_x

        std::vector<unsigned> m_hitExtend_x; // Hit lines will fill extra bins in x by this amount on each side, size == nLayers

        unsigned m_nCombineLayers; // number of layers after combined
        std::vector<unsigned> m_combineLayers; // Combine layers i.e. [0,1,2,0,1,2,0,1] will combine (L0, L3, L6) (L1, L4, L7) (L2, L5)
        std::vector< std::vector<unsigned> > m_combineLayer2D; // 2d array of combined layers i.e. [[1,2,3],[0,4,5],[6,7]] will combine (L1, L2, L3), (L0, L4, L5), (L6, L7)
        std::vector<unsigned> m_binScale;

        // === Roads ===
        std::vector<int> m_threshold; // Minimum point value post-convolution to accept as a road (inclusive)
        IntegerProperty m_localMaxWindowSize = 0; // Only create roads from a local maximum, requires traceHits
        BooleanProperty m_useSectors = false; // Will reverse calculate the sector for track-fitting purposes
        BooleanProperty m_idealGeoRoads = false; // will set sectors for the road based on the pt of the track

        ///////////////////////////////////////////////////////////////////////
        // Convenience

        unsigned m_nLayers; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();

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
