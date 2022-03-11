/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file HTTRoad.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date January 13th, 2020
 * @brief Defines a class for roads.
 */

#include <vector>

#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTConstants.h"

using namespace std;

size_t HTTRoad::getNHits() const
{
    size_t n = 0;
    for (auto& l : m_hits) n += l.size();
    return n;
}

std::vector<size_t> HTTRoad::getNHits_layer() const
{
    std::vector<size_t> out;
    for (auto& l : m_hits) out.push_back(l.size());
    return out;
}

size_t HTTRoad::getNHitCombos() const
{
    size_t combos = 1;
    size_t l = m_hits.size();
    std::vector<unsigned int> skipindices;

    // figure out indices of outer layers of stereo layers, where inner layer is also present to not
    // double count spacepoints
    for (size_t i = 1; i < l; ++i) {
        if (m_hits[i - 1].size() > 0 && m_hits[i].size() > 0) {
            if ((m_hits[i - 1][0]->isStrip() && m_hits[i][0]->isStrip()) &&
                (m_hits[i - 1][0]->getPhysLayer() % 2 == 0) &&
                (m_hits[i][0]->getPhysLayer() - m_hits[i - 1][0]->getPhysLayer() == 1)) {
                skipindices.push_back(i);
                ++i;
            }
        }
    }

    // calculate number of combinations
    for (size_t i = 0; i < l; ++i) {
        size_t n = 0;
        for (size_t j = 0; j < m_hits[i].size(); ++j) {
            // extra handling of spacepoints in combination calculation
            if (m_hits[i][j]->getHitType() == HitType::spacepoint) {
                bool skip = false;
                size_t skipsize = skipindices.size();
                for (size_t k = 0; k < skipsize; ++k)
                    if (i == skipindices[k]) skip = true;

                if (skip) continue;
            }
            ++n;
        }
        if (n > 0) combos *= n;
    }

    return combos;
}


HTTMultiTruth HTTRoad::getTruth() const
{
    // get number of pixel layers from hits. Assumes strips follow pixels,
    // and all hits in a layer share the same det type
    unsigned nPixel;
    for (nPixel = 0; nPixel < m_hits.size(); nPixel++)
        if (!m_hits[nPixel].empty() && m_hits[nPixel].front()->isStrip())
            break;

    std::map<HTTMultiTruth::Barcode, layer_bitmask_t> layer_map;

    for (auto const& hits : m_hits)
        for (HTTHit const* h : hits)
            for (auto const& x : h->getTruth())
                layer_map[x.first] |= (1 << h->getLayer());

    HTTMultiTruth mt;
    for (auto const& x : layer_map)
    {
        int w = 0, n = 0;
        for (unsigned i = 0; i < m_hits.size(); i++)
        {
            if (!m_hits[i].empty())
                n += (i < nPixel) ? 2 : 1; // double weight pixels
            if (x.second & (1 << i))
                w += (i < nPixel) ? 2 : 1; // double weight pixels
        }

        mt.add(x.first, static_cast<float>(w) / n);
    }

    return mt;
}

ostream& operator<<(ostream& os, const HTTRoad& road)
{
    os << "road " << road.m_roadID
        << ": PID " << road.m_pid
        << ", sector " << road.m_sector
        << " hitLayers " << std::showbase << std::hex << road.m_hit_layers
        << " wcLayers " << std::showbase << std::hex << road.m_wildcard_layers
        << ", nHits " << road.getNHits();
    /*
    for (size_t l=0; l < road.m_hits.size(); l++)
    {
        for (HTTHit const * hit : road.m_hits[l])
            os << "\n\t" << *hit;
    }
    */

    return os;
}

