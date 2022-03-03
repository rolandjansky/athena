/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONLAYERHOUGH_H
#define MUONLAYERHOUGH_H

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "MuonLayerHough/Hit.h"
#include "MuonLayerHough/MuonLayerHoughSelector.h"
#include "MuonStationIndex/MuonStationIndex.h"

class TH1;

namespace MuonHough {
    using HitVec = std::vector<std::shared_ptr<MuonHough::Hit>>;
    

    /** struct containing all information to build a Hough transform for a given chamber index */
    struct RegionDescriptor {
        RegionDescriptor(int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_, Muon::MuonStationIndex::ChIndex chIndex_,
                         float referencePosition_, float yMinRange_, float yMaxRange_, float yBinSize_, float thetaStep_,
                         unsigned int nthetaSamples_) :
            sector(sector_),
            region(region_),
            chIndex(chIndex_),
            referencePosition(referencePosition_),
            yMinRange(yMinRange_),
            yMaxRange(yMaxRange_),
            yBinSize(yBinSize_),
            thetaStep(thetaStep_),
            nthetaSamples(nthetaSamples_) {}
        RegionDescriptor() = default;

        int sector{0};
        Muon::MuonStationIndex::DetectorRegionIndex region{Muon::MuonStationIndex::DetectorRegionUnknown};
        Muon::MuonStationIndex::ChIndex chIndex{Muon::MuonStationIndex::ChUnknown};
        float referencePosition{0};
        float yMinRange{0};
        float yMaxRange{0};
        float yBinSize{0};
        float thetaStep{0};
        unsigned int nthetaSamples{1};
    };
    
    using RegionDescriptionVec =  std::vector<RegionDescriptor>;

    
    struct MuonLayerHough {
        /// struct representing the maximum in the hough space
        struct Maximum {
            Maximum() = default;

            float max{0.};    // value of the maximum
            float pos{0.};    // spacial position
            float theta{0.};  // angle

            float refpos{0.};   // reference position
            int refregion{0};   // reference region
            int refchIndex{0};  // reference chamber index

            int binpos{-1};           // position bin
            int binposmin{-1};        // lower edge of the maximum
            int binposmax{-1};        // upper edge of the maximu
            float binsize{0.};        // size of bin
            int bintheta{-1};         // theta bin
            int triggerConfirmed{0};  // number of in time trigger hits associated with the maximum
            HitVec hits;   // vector of associated hits

            const MuonLayerHough* hough{nullptr};  // pointer to the corresponding hough

            bool isEndcap() const {
                // refers to the chamber orientation!!!! so BEE is a barell in this def
                Muon::MuonStationIndex::DetectorRegionIndex region = hough->m_descriptor.region;
                Muon::MuonStationIndex::ChIndex chIndex = hough->m_descriptor.chIndex;
                // need to make sure BEE's reference plane is the same as barrel
                if (region != Muon::MuonStationIndex::Barrel && chIndex != Muon::MuonStationIndex::BEE) { return true; }
                return false;
            };
            float getGlobalR() const {
                if (isEndcap()) { return pos; }
                return refpos;
            };
            float getGlobalZ() const {
                if (isEndcap()) { return refpos; }
                return pos;
            };
            float getGlobalTheta() const {
                if (isEndcap()) {
                    // return M_PI/2.0 - theta;
                    if (theta > 0) {
                        return M_PI / 2.0 - theta;
                    } else {
                        return -M_PI / 2.0 - theta;
                    }
                }
                return theta;
            };
        };

        /// constructor
        MuonLayerHough(const RegionDescriptor& descriptor);

        /// destructor
        ~MuonLayerHough() = default;

        /// reset the transform
        void reset();

        /// enable debug output
        void setDebug(bool d) { m_debug = d; }

        /// calculate the position bin the hit will endup in
        int bin(const Hit& hit) const;

        /// calculate the bin corresponing to the given x,y position
        int bin(float x, float y) const;

        /// access to y coordinate of a given bin
        float yval(int posBin) const;

        /// calculate x,y for the given position bin
        void pars(int posBin, int /*thetaBin*/, float& x, float& y) const;

        /// calculate the highest value of the hough transform within the specified range for the given hit
        float layerConfirmation(const Hit& hit, float range = 1000.) const;

        /// calculate the highest value of the hough transform within the specified range for the given hit position
        float layerConfirmation(float x, float y, float range = 1000.) const;

        /// calculate the highest value of the hough transform within the specified range for the given hit position
        std::pair<float, float> layerConfirmation(unsigned int thetaBin, float x, float y, float range = 1000.) const;

        /// find the highest maximum that is above maxval
        bool findMaximum(Maximum& maximum, const MuonLayerHoughSelector& selector) const;

        /// associates the list of input hits to the provided maximum
        void associateHitsToMaximum(Maximum& maximum, const HitVec& hits) const;

        /// calculates the first and last bin the hit should be filled in for a given theta bin
        std::pair<int, int> range(const float x, const float y1, const float y2, const int bintheta) const;

        /// returns a pair with the position and angle corresponing to the input x,y values
        std::pair<float, float> maximum(float x, float y, int& posbin, int& thetabin) const;

        /// fill the hough space with a single position
        void fill(const Hit& hit);

        /// fill the hough space with a single position
        void fill(float x, float y, float weight);

        /// fill the hough space with a vector of hits using the layer mode
        void fillLayer(const HitVec& hits, bool substract = false);

        //  fill the hough space with a vector of hits using the layer mode
        void fillLayer2(const HitVec& hits, bool subtract = false);

        /// returns a vector with all the histograms of the hough as TH1*
        std::vector<TH1*> rootHistos(const std::string& prefix, const float* rmin = 0, const float* rmax = 0) const;

        // members

        float m_binsize{0};     /// binsize
        float m_invbinsize{0};  /// inverse binsize

        // unsigned int m_nthetasamples;
        int m_nbins{-1};

        unsigned int max{0};
        int maxhist{-1};
        int maxbin{-1};
        bool m_debug{false};
        std::vector<std::unique_ptr<unsigned int[]> > m_histos;  // the maximum contents of all the histograms, overlayed
        RegionDescriptor m_descriptor;
    };

    inline int MuonLayerHough::bin(const Hit& hit) const { return bin(hit.x, hit.ymin); }

    inline int MuonLayerHough::bin(float x, float y) const {
        float yref = y * (m_descriptor.referencePosition - x) / x + y;
        int in = (yref - m_descriptor.yMinRange) / m_binsize;
        if (in < 0 || in >= m_nbins) in = -1;
        return in;
    }

    inline void MuonLayerHough::pars(int posBin, int /*thetaBin*/, float& x, float& y) const {
        x = m_descriptor.referencePosition;
        y = yval(posBin);
    }

    inline float MuonLayerHough::yval(int posBin) const { return m_descriptor.yMinRange + posBin * m_binsize; }

    inline float MuonLayerHough::layerConfirmation(const Hit& hit, float range) const { return layerConfirmation(hit.x, hit.ymin, range); }

    inline void MuonLayerHough::fill(const Hit& hit) { fill(hit.x, hit.ymin, hit.w); }

    // function for linear/parabolic extrapolation from maxima to maxima in a different station
    float extrapolate(const MuonLayerHough::Maximum& ref, const MuonLayerHough::Maximum& ex, bool doparabolic = false);
}  // namespace MuonHough
#endif
