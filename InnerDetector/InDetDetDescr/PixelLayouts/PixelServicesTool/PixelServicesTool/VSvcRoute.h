/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VSvcRoute_H
#define VSvcRoute_H

#include "SvcRoute.h"
#include <vector>

class ServiceDynVolume;

class VSvcRoute : public SvcRoute {
public:

  typedef std::vector<ServiceDynVolume*>   VolumeContainer;

  VSvcRoute() : SvcRoute() {}
    VSvcRoute( double z, double rmi, double rma, double rexit, const std::string& name):
      SvcRoute(name), 
      m_zPos(z), m_rMin(rmi), m_rMax(rma), m_rExit(rexit), m_next(0){}
      
  double rMin() const {return m_rMin;}
  double rMax() const {return m_rMax;}
  double zPos() const {return m_zPos;}
  double rExit() const {return m_rExit;}

  virtual double position() const {return zPos();}

  virtual double exit() const {return rExit();}

  virtual SvcRoute* nextRoute() const {return m_next;}

  virtual const VolumeContainer& volumes() const {return m_volumes;}

  virtual void addVolume( ServiceDynVolume* vol) {m_volumes.push_back(vol);}

  virtual void setNextRoute( SvcRoute* nr) {m_next = nr;}
  /*
  virtual ServiceDynVolume* entryVolume( double pos) const {
    // FIXME - too simple, for tests only
    if (volumes().empty()) return 0;
    if (volumes().front()->contains(pos)) return volumes().front();
    else if (volumes().back()->contains(pos)) return volumes().back();
    else {
      //should iterate to find exit volume
      return 0; // FIXME
    }
  }

  virtual ServiceDynVolume* exitVolume() const;
  */
private:

  double m_zPos;
  double m_rMin;
  double m_rMax;
  double m_rExit;
  SvcRoute* m_next;

  VolumeContainer m_volumes;
};

#endif
