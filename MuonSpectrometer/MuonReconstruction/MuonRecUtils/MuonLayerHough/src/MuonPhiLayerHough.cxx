/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/MuonPhiLayerHough.h"

#include <TH1.h>
#include <memory.h>

#include <cmath>
#include <iostream>

namespace MuonHough {

    MuonPhiLayerHough::MuonPhiLayerHough(int nbins, float rangemin, float rangemax, Muon::MuonStationIndex::DetectorRegionIndex region) :
        m_rangemin(rangemin), m_rangemax(rangemax), m_region(region), m_nbins(nbins), m_histo{new unsigned int[m_nbins]} {
        // calculate the binsize
        m_binsize = (m_rangemax - m_rangemin) / m_nbins;
        m_invbinsize = 1. / m_binsize,

        // setup the histograms
        reset();
    }

    MuonPhiLayerHough::~MuonPhiLayerHough() = default;

    void MuonPhiLayerHough::reset() const { memset(m_histo.get(), 0, sizeof(unsigned int) * m_nbins); }

    void MuonPhiLayerHough::fillLayer2(const PhiHitVec& hits, bool subtract) const {
        if (hits.empty()) return;

        std::vector<int> layerCounts(m_nbins, 0);
        int sign = subtract ? -1000 : 1000;
        // outer loop over cycles

        // keep track of the previous layer
        int prevlayer = hits.front()->layer;

        // inner loop over hits
        PhiHitVec::const_iterator it = hits.begin();
        PhiHitVec::const_iterator it_end = hits.end();
        for (; it != it_end; ++it) {
            // if we get to the next layer process the current one and fill the Hough space
            if (prevlayer != (*it)->layer) {
                for (int i = 0; i < m_nbins; ++i) {
                    if (subtract && -layerCounts[i] >= static_cast<int>(m_histo[i]))
                        m_histo[i] = 0;
                    else
                        m_histo[i] += layerCounts[i];
                    // if( m_debug && layerCounts[i] != 0 ) std::cout << " filling layer " << prevlayer << " bin " << i << std::endl;
                    layerCounts[i] = 0;  // reset bin
                }
                prevlayer = (*it)->layer;
            }

            // get bin range
            std::pair<int, int> minMax = range((*it)->r, (*it)->phimin, (*it)->phimax);
            int binmin = minMax.first;
            int binmax = minMax.second;

            // check wether we are within the Hough space
            if (binmin >= m_nbins) continue;
            if (binmax < 0) continue;

            // adjust boundaries if needed
            if (binmin < 0) binmin = 0;
            if (binmax >= m_nbins) binmax = m_nbins - 1;

            // output hit for debug purposes
            if (m_debug) {
                std::cout << " filling hit " << (*it)->layer << " phimin " << (*it)->phimin << " phimax " << (*it)->phimax << " weight "
                          << (*it)->w << " binmin " << binmin << " max " << binmax;
                if ((*it)->debugInfo()) {
                    const HitDebugInfo* db1 = (*it)->debugInfo();
                    std::cout << " sec " << db1->sector << " r " << db1->region << " type " << db1->type << " lay " << db1->layer << " bc "
                              << db1->barcode << std::endl;
                } else
                    std::cout << std::endl;
            }
            int weight = sign * (*it)->w;
            // set bits to true
            for (; binmin <= binmax; ++binmin) layerCounts[binmin] = weight;
        }
        // if the last set of hits was not filled, fill them now
        for (int i = 0; i < m_nbins; ++i) {
            if (subtract && -layerCounts[i] >= static_cast<int>(m_histo[i]))
                m_histo[i] = 0;
            else
                m_histo[i] += layerCounts[i];
            // if( m_debug && layerCounts[i] != 0 ) std::cout << " filling layer " << prevlayer << " bin " << i << std::endl;
        }
    }

    void MuonPhiLayerHough::fillLayer(const PhiHitVec& hits, bool subtract) const {
        if (hits.empty()) return;
        if (m_debug) std::cout << " filling layers, hits " << hits.size() << " subtract " << subtract << std::endl;
        int prevlayer = -1;
        int prevbinmin = 10000;
        int prevbinmax = -1;
        //  loop over hits
        PhiHitVec::const_iterator it = hits.begin();
        PhiHitVec::const_iterator it_end = hits.end();
        for (; it != it_end; ++it) {
            std::pair<int, int> minMax = range((*it)->r, (*it)->phimin, (*it)->phimax);
            // if( m_debug ) std::cout << " filling: min " << minMax.first << "  max " << minMax.second << std::endl;
            int binmin = minMax.first;
            int binmax = minMax.second;

            if (binmin >= m_nbins) continue;
            if (binmax < 0) continue;

            if (binmin < 0) binmin = 0;
            if (binmax >= m_nbins) binmax = m_nbins - 1;
            if (m_debug)
                std::cout << "  layer " << (*it)->layer << " r " << (*it)->r << " phimin " << (*it)->phimin << " phimax " << (*it)->phimax
                          << " new min " << binmin << "  " << binmax << std::endl;

            // first hit within range
            if (prevbinmax == -1) {
                if (m_debug)
                    std::cout << " first range " << (*it)->layer << " r " << (*it)->r << " range " << binmin << " " << binmax << " new min "
                              << binmin << "  " << binmax << std::endl;
                prevbinmin = binmin;
                prevbinmax = binmax;
                prevlayer = (*it)->layer;
                continue;
            }

            if (binmin < prevbinmin && prevlayer == (*it)->layer)
                std::cout << "Error hits are out of order: min " << binmin << " max " << binmax << std::endl;

            // if the max value of the previous hit is smaller than the current minvalue fill the histogram of the previous hit
            // do the same when reached last hit
            if (prevbinmax < binmin || prevlayer != (*it)->layer) {
                if (m_debug)
                    std::cout << " filling " << (*it)->layer << " r " << (*it)->r << " phimin " << (*it)->phimin << " phimax "
                              << (*it)->phimax << " range " << prevbinmin << " " << prevbinmax << " new min " << binmin << "  " << binmax
                              << std::endl;
                for (int n = prevbinmin; n <= prevbinmax; ++n) {
                    unsigned int& val = m_histo[n];
                    int w = 1000 * (*it)->w;
                    if (subtract) w *= -1;
                    if (w < 0 && (int)val < -w)
                        val = 0;
                    else
                        val += w;
                }
                prevbinmin = binmin;
                prevbinmax = binmax;
                prevlayer = (*it)->layer;

            } else {
                // update the maximum value of the window
                if (m_debug)
                    std::cout << " updating range " << (*it)->layer << " r " << (*it)->r << " phimin " << (*it)->phimin << " phimax "
                              << (*it)->phimax << " range " << prevbinmin << " " << prevbinmax << " new min " << binmin << "  " << binmax
                              << std::endl;
                prevbinmax = binmax;
            }
        }
        if (prevbinmax != -1) {
            if (m_debug)
                std::cout << " filling " << hits.back()->layer << " r " << hits.back()->r << " phimin " << hits.back()->phimin << " phimax "
                          << hits.back()->phimax << " range " << prevbinmin << " " << prevbinmax << std::endl;
            for (int n = prevbinmin; n <= prevbinmax; ++n) {
                unsigned int& val = m_histo[n];
                int w = 1000 * hits.back()->w;
                if (subtract) w *= -1;
                if (w < 0 && (int)val < -w)
                    val = 0;
                else
                    val += w;
            }
        }
    }

    std::vector<TH1*> MuonPhiLayerHough::rootHistos(const std::string& prefix, const float* phimi, const float* phima) const {
        std::vector<TH1*> hists;

        float phimin = phimi ? *phimi : m_rangemin;
        float phimax = phima ? *phima : m_rangemax;

        TString hname = prefix + "_hist";
        TH1F* h = new TH1F(hname, hname, m_nbins, phimin, phimax);
        for (int n = 0; n < m_nbins; ++n) h->SetBinContent(n + 1, m_histo[n] * 0.001);
        hists.push_back(h);
        return hists;
    }

    bool MuonPhiLayerHough::findMaximum(MuonPhiLayerHough::Maximum& maximum, float maxval) const {
        maximum.max = 0;
        maximum.pos = 0;

        maximum.binpos = -1;
        maximum.binposmin = -1;
        maximum.binposmax = -1;

        maximum.hits.clear();
        maximum.hough = this;

        if (maxval < 0) return false;

        unsigned int tmax = 0;
        int posb = -1;
        unsigned int imaxval = maxval * 1000;
        // loop over histograms and find maximum
        for (int n = 0; n < m_nbins; ++n) {
            if (m_histo[n] < tmax) continue;
            tmax = m_histo[n];
            posb = n;
        }
        if (posb == -1) return false;
        if (tmax < imaxval) return false;

        maximum.max = tmax / 1000.;
        maximum.pos = m_rangemin + m_binsize * posb;
        maximum.binpos = posb;
        maximum.binposmin = posb;
        maximum.binposmax = posb;
        if (maximum.max > 100) {
            std::cout << " too large maximum: " << maximum.max << " tmax " << tmax << std::endl;
            for (int n = 0; n < m_nbins; ++n) std::cout << "  " << m_histo[n] << std::endl;
        }

        // determin width of maximum
        unsigned int imax = m_histo[posb];
        unsigned int sidemax = 0.7 * imax;
        // loop down, catch case the maximum is in the first bin
        for (int n = posb != 0 ? posb - 1 : posb; n >= 0; --n) {
            if (m_histo[n] > sidemax) {
                maximum.binposmin = n;
            } else {
                break;
            }
        }
        for (int n = posb + 1; n < m_nbins; ++n) {
            if (m_histo[n] > sidemax) {
                maximum.binposmax = n;
            } else {
                break;
            }
        }
        return true;
    }

    void MuonPhiLayerHough::associateHitsToMaximum(MuonPhiLayerHough::Maximum& maximum, const PhiHitVec& hits) const {
        if (maximum.binposmax == -1 || maximum.binposmin == -1) return;
        // loop over hits and find those that are compatible with the maximum
        PhiHitVec::const_iterator it = hits.begin();
        PhiHitVec::const_iterator it_end = hits.end();
        for (; it != it_end; ++it) {
            // calculate the bins associated with the hit and check whether any of they are part of the maximum
            std::pair<int, int> minMax = range((*it)->r, (*it)->phimin, (*it)->phimax);
            if (m_debug)
                std::cout << " hit: r " << (*it)->r << " phimin " << (*it)->phimin << " phimax " << (*it)->phimax << " range "
                          << minMax.first << " " << minMax.second << "  maximum range " << maximum.binposmin << " " << maximum.binposmax
                          << std::endl;
            if (minMax.first > maximum.binposmax) continue;   // minimum bin large than the maximum, drop
            if (minMax.second < maximum.binposmin) continue;  // maximum bin smaller than the minimum, drop
            // keep everything else
            maximum.hits.push_back(*it);
        }
    }

}  // namespace MuonHough
