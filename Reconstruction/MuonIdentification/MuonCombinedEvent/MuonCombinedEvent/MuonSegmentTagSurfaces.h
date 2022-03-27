/*
  Copyright (C) 2002-2022  CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONCOMBINEDEVENT_MUONSEGMENTTAGSURFACES_H
#define MUONCOMBINEDEVENT_MUONSEGMENTTAGSURFACES_H

#include <memory>
#include <string>
#include <vector>
#include <array>
#include "TrkSurfaces/Surface.h"
#include "MuonStationIndex/MuonStationIndex.h"

/*
    Helper class defining a super simple model of the MS consisting of cylinders to represent the barrels
    & disks to represent the endcap stations. The simplification is used in the Segment Tagging alg
*/

namespace MuonCombined {
    class MuonSegmentTagSurfaces final {
    public:
        enum SurfDef {
            BI = 0,
            BM = 1,
            BO = 2,           
            EIA = 3,
            EMA = 4,
            EOA = 5,
            EEA = 6, 
            EIC = 7,
            EMC = 8,
            EOC = 9,
            EEC = 10,
            NumSurf = 11,
        };
        /// Transforms the Muon Station index to a SurfDef index. For the endcap
        /// external information is required whether the object is on positive or negative eta site
        static int stIdxToSurfDef(Muon::MuonStationIndex::StIndex st_idx, bool pos_eta);
        
        using SurfaceVec = std::array<std::unique_ptr<Trk::Surface>, SurfDef::NumSurf> ;
        using SurfaceStations = std::array<std::string, SurfDef::NumSurf> ;
        
        MuonSegmentTagSurfaces();
        ~MuonSegmentTagSurfaces() = default;

        const SurfaceVec& getSurfaces() const { return m_vec; };
        void setSurface(const Trk::Surface* surface, SurfDef def) { m_vec[def].reset(surface->clone()); };

        const std::string stationType(unsigned int count) const {
            if (count >= m_station.size())
                return "";
            else
                return m_station[count];
        };

        const Trk::Surface* getSurface(SurfDef surf) const { return m_vec[surf].get(); };

        const Trk::Surface* getSurface(unsigned int surf_count) const {
            if (surf_count >= m_vec.size())
                return nullptr;
            else
                return m_vec[surf_count].get();
        };

        const Trk::Surface* operator[](SurfDef surf) const { return m_vec[surf].get(); };

        SurfaceVec::const_iterator begin() const { return m_vec.begin(); };
        SurfaceVec::const_iterator end() const { return m_vec.end(); };

    private:
        SurfaceVec m_vec{};
        SurfaceStations m_station{};
    };

}


#endif  //
