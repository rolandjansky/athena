/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VRoute_H
#define VRoute_H

#include "Route.h"
#include <vector>

class ServiceVolume;

class VRoute : public Route {
public:

  typedef std::vector<ServiceVolume*>   VolumeContainer;

  VRoute() : Route() {}
    VRoute( double z, double rmi, double rma, double rexit, const std::string& name):
      Route(name), 
      m_zPos(z), m_rMin(rmi), m_rMax(rma), m_rExit(rexit), m_next(0){}
      
  double rMin() const {return m_rMin;}
  double rMax() const {return m_rMax;}
  double zPos() const {return m_zPos;}
  double rExit() const {return m_rExit;}

  virtual double position() const {return zPos();}

  virtual double exit() const {return rExit();}

  virtual Route* nextRoute() {return m_next;}

  virtual const VolumeContainer& volumes() const {return m_volumes;}

  virtual void addVolume( ServiceVolume* vol) {m_volumes.push_back(vol);}

  virtual void setNextRoute( Route* nr) {m_next = nr;}
  /*
  virtual ServiceVolume* entryVolume( double pos) const {
    // FIXME - too simple, for tests only
    if (volumes().empty()) return 0;
    if (volumes().front()->contains(pos)) return volumes().front();
    else if (volumes().back()->contains(pos)) return volumes().back();
    else {
      //should iterate to find exit volume
      return 0; // FIXME
    }
  }

  virtual ServiceVolume* exitVolume() const;
  */
private:

  double m_zPos{};
  double m_rMin{};
  double m_rMax{};
  double m_rExit{};
  Route* m_next{};

  VolumeContainer m_volumes;
};

#endif
