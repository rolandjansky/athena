/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHILAYERHOUGH_H
#define MUONPHILAYERHOUGH_H

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "MuonLayerHough/Hit.h"

class TH1;

namespace MuonHough {
    
    using PhiHitVec = std::vector<std::shared_ptr<MuonHough::PhiHit>>;
   
    struct MuonPhiLayerHough {
        struct Maximum {
            Maximum() = default;

            float max{0.};
            float pos{0.};

            int binpos{-1};
            int binposmin{-1};
            int binposmax{-1};

            int sector{-1};

            PhiHitVec hits;

            const MuonPhiLayerHough* hough{nullptr};
        };

        MuonPhiLayerHough(int nbins, float rangemin, float rangemax, Muon::MuonStationIndex::DetectorRegionIndex region_);
        ~MuonPhiLayerHough();

        void reset() const;
        void setDebug(bool d) { m_debug = d; }

        bool findMaximum(Maximum& maximum, float maxval) const;
        void associateHitsToMaximum(Maximum& maximum, const PhiHitVec& hits) const;

        std::pair<int, int> range(float /*r*/, float phi1, float phi2) const {
            float phimin = std::min(phi1, phi2);
            float phimax = std::max(phi1, phi2);
            int bphimin = (phimin - m_rangemin) * m_invbinsize;
            int bphimax = (phimax - m_rangemin) * m_invbinsize;
            // add neighbouring bin
            if ((m_rangemin + m_binsize * bphimin - phimin) * m_invbinsize > 0) bphimin -= 1;
            if ((m_rangemin + m_binsize * bphimax - phimax) * m_invbinsize < 0) bphimax += 1;
            return std::make_pair(bphimin, bphimax);
        }

        void fillLayer(const PhiHitVec& hits, bool substract = false) const;
        void fillLayer2(const PhiHitVec& hits, bool substract = false) const;

        float maximum(float r, float phimin, float phimax, int& posbin) const {
            unsigned int max = 0;
            posbin = -1;

            std::pair<int, int> minMax = range(r, phimin, phimax);
            for (int n = minMax.first; n <= minMax.second; ++n) {
                if (max < m_histo[n]) {
                    max = m_histo[n];
                    posbin = n;
                }
            }
            if (max > 100000) {
                std::cout << " maximum too large " << max * 0.001 << " min " << minMax.first << " max " << minMax.second << " nbins "
                          << m_nbins << " phimin " << phimin << " phimax " << phimax << std::endl;
                for (int n = minMax.first; n <= minMax.second; ++n) { std::cout << " bin " << n << " val " << m_histo[n] << std::endl; }
            }

            return 0.001 * max;
        }

        std::vector<TH1*> rootHistos(const std::string& prefix, const float* phimin = 0, const float* phimax = 0) const;

        float m_binsize{0.};
        float m_invbinsize{0.};
        float m_rangemin{0.};
        float m_rangemax{0.};

        Muon::MuonStationIndex::DetectorRegionIndex m_region;
        int m_nbins;
        bool m_debug{false};
        std::unique_ptr<unsigned int[]> m_histo;

    private:
        // fake copy constructor and assignment operator
        MuonPhiLayerHough(const MuonPhiLayerHough&);
        MuonPhiLayerHough& operator=(const MuonPhiLayerHough& right);
    };

}  // namespace MuonHough
#endif
