// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTROAD_H
#define HTTROAD_H

/**
 * @file HTTRoad.h
 * @author Riley Xu - rixu@cern.ch
 * @date Janurary 13th, 2020
 * @brief Defines a class for roads.
 */


#include <vector>
#include <ostream>

#include "TObject.h"

#include "TrigHTTUtils/HTTTypes.h"
#include "HTTHit.h"


class HTTRoad : public TObject
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // Constructors

        HTTRoad() = default;

        HTTRoad(unsigned nLayers) : m_hits(nLayers) { }

        HTTRoad(int roadID, pid_t pid, sector_t sector, layer_bitmask_t hit_layers,
                layer_bitmask_t wildcard_layers, std::vector<std::vector<const HTTHit*>> const & hits)
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
        void setHits(std::vector<std::vector<const HTTHit*>> const & hits) { m_hits = hits; }
        void setHits(unsigned layer, std::vector<const HTTHit*> const & hits) { m_hits.at(layer) = hits; } // ensure setNLayers is called first

        ///////////////////////////////////////////////////////////////////////
        // Getters

        int getRoadID() const { return m_roadID; }
        pid_t getPID() const { return m_pid; }
        sector_t getSector() const { return m_sector; }

        layer_bitmask_t getHitLayers() const { return m_hit_layers; }
        layer_bitmask_t getWCLayers() const { return m_wildcard_layers; }

        size_t getNLayers() const { return m_hits.size(); }
        size_t getNHits() const
        {
            size_t n = 0;
            for (auto & l : m_hits) n += l.size();
            return n;
        }
        std::vector<size_t> getNHits_layer() const
        {
            std::vector<size_t> out;
            for (auto & l : m_hits) out.push_back(l.size());
            return out;
        }
        std::vector<const HTTHit*> const & getHits(size_t layer) const { return m_hits.at(layer); }

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

        friend std::ostream& operator<<(std::ostream& os, const HTTRoad & road);
        ClassDef(HTTRoad, 1);
};


#endif // HTTROAD_H

