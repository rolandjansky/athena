// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTOBJECTS_HTTROAD_H
#define TRIGHTTOBJECTS_HTTROAD_H

/**
 * @file HTTRoad.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date Janurary 13th, 2020
 * @brief Defines a class for roads.
 *
 * Roads are triggered bins in Hough. They collect
 * the hits that fired the bin with some auxillary information.
 */


#include <vector>
#include <unordered_set>
#include <ostream>

#include "TObject.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTMultiTruth.h"


class HTTRoad : public TObject
{
public:

    ///////////////////////////////////////////////////////////////////////
    // Constructors

    HTTRoad() = default;

    HTTRoad(unsigned nLayers) : m_hits(nLayers) { }

    HTTRoad(int roadID, pid_t pid, sector_t sector, layer_bitmask_t hit_layers,
        layer_bitmask_t wildcard_layers, std::vector<std::vector<const HTTHit*>> const& hits)
        : m_roadID(roadID), m_pid(pid), m_sector(sector), m_hit_layers(hit_layers), m_wildcard_layers(wildcard_layers), m_hits(hits)
    { }


    ///////////////////////////////////////////////////////////////////////
    // Setters

    void setRoadID(int roadID) { m_roadID = roadID; }
    void setPID(pid_t pid) { m_pid = pid; }
    void setSector(sector_t sector) { m_sector = sector; }

    void setHitLayers(layer_bitmask_t hit_layers) { m_hit_layers = hit_layers; }
    void setWCLayers(layer_bitmask_t wc_layers) { m_wildcard_layers = wc_layers; }

    void setNLayers(unsigned layers) { m_hits.resize(layers); }
    void setHits(std::vector<std::vector<const HTTHit*>> const& hits) { m_hits = hits; }
    void setHits(unsigned layer, std::vector<const HTTHit*> const& hits) { m_hits.at(layer) = hits; } // ensure setNLayers is called first

    ///////////////////////////////////////////////////////////////////////
    // Getters

    int getRoadID() const { return m_roadID; }
    pid_t getPID() const { return m_pid; }
    sector_t getSector() const { return m_sector; }

    layer_bitmask_t getHitLayers() const { return m_hit_layers; }
    layer_bitmask_t getWCLayers() const { return m_wildcard_layers; }

    std::vector<const HTTHit*> const& getHits(size_t layer) const { return m_hits.at(layer); }
    std::unordered_set<const HTTHit*> getHits_flat() const
    {
        std::unordered_set<const HTTHit*> hits;
        for (auto& x : m_hits) hits.insert(x.begin(), x.end());
        return hits;
    }

    ///////////////////////////////////////////////////////////////////////
    // Utility

    size_t getNLayers() const { return m_hits.size(); }
    size_t getNHitLayers() const { return __builtin_popcount(m_hit_layers); }
    size_t getNWCLayers() const { return __builtin_popcount(m_wildcard_layers); }

    size_t getNHits() const;
    std::vector<size_t> getNHits_layer() const;
    size_t getNHitCombos() const;


    // Weight of each barcode is the fraction of layers with corresponding hits
    // where pixels are weighted twice as much
    HTTMultiTruth getTruth() const;

private:

    int m_roadID = 0;       // Currently just a count set by RoadFinder.
    pid_t m_pid = 0;        // the pattern ID that fired this road
    sector_t m_sector = 0;  // Sector this road belongs to / should be fitted with

    layer_bitmask_t m_hit_layers = 0;       // Layers that had hits match the pattern, not including wildcards
    layer_bitmask_t m_wildcard_layers = 0;  // Layers that matched because of a wildcard in the pattern

    std::vector<std::vector<const HTTHit*>> m_hits; // [layer, hit#]
        // A list of hits in the road for each layer.
        // These pointers are not owned by the road.


    ///////////////////////////////////////////////////////////////////////
    // Misc

    friend std::ostream& operator<<(std::ostream& os, const HTTRoad& road);
    ClassDef(HTTRoad, 2);
};



class HTTRoad_Hough : public HTTRoad
{
public:

    int getSubRegion() const { return m_subRegion; }
    unsigned getXBin() const { return m_xBin; }
    unsigned getYBin() const { return m_yBin; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }

    void setSubRegion(int v) { m_subRegion = v; }
    void setXBin(unsigned v) { m_xBin = v; }
    void setYBin(unsigned v) { m_yBin = v; }
    void setX(float v) { m_x = v; }
    void setY(float v) { m_y = v; }

private:

    int m_subRegion = 0; // slice that the road came from
    unsigned m_xBin = 0;
    unsigned m_yBin = 0;
    float m_x = 0; // x value of Hough bin
    float m_y = 0; // y value of Hough bin

};

#endif // HTTROAD_H

