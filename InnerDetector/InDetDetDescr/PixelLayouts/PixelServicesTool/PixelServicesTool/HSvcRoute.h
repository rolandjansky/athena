/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HSvcRoute_H
#define HSvcRoute_H

#include "SvcRoute.h"
#include <vector>

class ServiceDynVolume;

class HSvcRoute : public SvcRoute {
public:

  typedef std::vector<ServiceDynVolume*>   VolumeContainer;

  HSvcRoute() : SvcRoute()  {}
  HSvcRoute( double r, double zmi, double zma, double zexit, const std::string& name) : 
    SvcRoute(name),
    m_radius(r), m_zMin(zmi), m_zMax(zma), m_zExit(zexit), m_next(0) {}

  double zMin() const {return m_zMin;}
  double zMax() const {return m_zMax;}
  double radius() const {return m_radius;}
  double zExit() const {return m_zExit;}

  virtual double position() const {return radius();}

  virtual double exit() const {return zExit();}

  virtual SvcRoute* nextRoute() const {return m_next;}

  const VolumeContainer& volumes() const {return m_volumes;}

  virtual void addVolume( ServiceDynVolume* vol) {m_volumes.push_back(vol);}

  virtual void setNextRoute( SvcRoute* nr) {m_next = nr;}

private:

  double m_radius;
  double m_zMin;
  double m_zMax;
  double m_zExit;
  SvcRoute* m_next;

  VolumeContainer m_volumes;

};

#endif
