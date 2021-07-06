/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONCOMBINEDBASETOOLS_MSSURFACES_H
#define MUONCOMBINEDBASETOOLS_MSSURFACES_H

#include <memory>
#include <string>
#include <vector>

#include "SurfDef.h"
#include "TrkSurfaces/Surface.h"

class MSSurfaces final {
public:
    typedef std::vector<std::unique_ptr<const Trk::Surface>> SurfaceVec;
    typedef std::vector<std::string> SurfaceStations;

    MSSurfaces();
    ~MSSurfaces() = default;

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
    SurfaceVec m_vec;
    SurfaceStations m_station;
};

#endif  //
