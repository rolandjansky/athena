/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MSSURFACES_H
#define MUONCOMBINEDBASETOOLS_MSSURFACES_H

#include "TrkSurfaces/Surface.h"
#include "SurfDef.h"

#include <vector>
#include <string>

class MSSurfaces{
 public: 
  typedef std::vector<const Trk::Surface*> SurfaceVec;
  typedef std::vector<std::string> SurfaceStations;
  
  MSSurfaces();
  ~MSSurfaces();

  const SurfaceVec getSurfaces() const { return m_vec ; } ;
  void setSurface(const Trk::Surface* surface, SurfDef def){ m_vec[def] = surface; } ;

  const std::string stationType(unsigned int count) const { 
    if(count > m_station.size())  return ""; 
    else return m_station[count] ; 
  } ;
  
  const Trk::Surface* getSurface(SurfDef surf) const { return m_vec[surf]; };
  
  const Trk::Surface* getSurface(unsigned int surf_count) const {
    if( surf_count > m_vec.size() ) return 0; 
    else return m_vec[surf_count]; };
  
  const Trk::Surface* operator[](SurfDef surf) const { 
    return m_vec[surf] ; 
  };
  

  std::vector<const Trk::Surface*>::const_iterator begin() const { return m_vec.begin() ; };
  std::vector<const Trk::Surface*>::const_iterator end() const { return m_vec.end() ; };
  

 private:
  SurfaceVec       m_vec;
  SurfaceStations  m_station;
};

#endif //
