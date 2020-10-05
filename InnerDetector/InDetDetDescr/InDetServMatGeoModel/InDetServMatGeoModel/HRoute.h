/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HRoute_H
#define HRoute_H

#include "Route.h"
#include <vector>

class ServiceVolume;

class HRoute : public Route {
public:

  typedef std::vector<ServiceVolume*>   VolumeContainer;

  HRoute() : Route()  {}
    HRoute( double r, double zmi, double zma, double zexit, const std::string& name) : 
      Route(name),
      m_radius(r), m_zMin(zmi), m_zMax(zma), m_zExit(zexit), m_next(0) {}

  double zMin() const {return m_zMin;}
  double zMax() const {return m_zMax;}
  double radius() const {return m_radius;}
  double zExit() const {return m_zExit;}

  virtual double position() const {return radius();}

  virtual double exit() const {return zExit();}

  virtual Route* nextRoute() {return m_next;}

  const VolumeContainer& volumes() const {return m_volumes;}

  virtual void addVolume( ServiceVolume* vol) {m_volumes.push_back(vol);}

  virtual void setNextRoute( Route* nr) {m_next = nr;}

private:

  double m_radius{};
  double m_zMin{};
  double m_zMax{};
  double m_zExit{};
  Route* m_next{};

  VolumeContainer m_volumes;

};

#endif
