/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef VSvcRoute_H
#define VSvcRoute_H

#include "SvcRoute.h"
#include <vector>

class VSvcRoute : public SvcRoute {
 public:

  typedef std::vector<ServiceDynVolume*>   VolumeContainer;
  
 VSvcRoute() : SvcRoute() {}
 VSvcRoute( double z, double rmi, double rma, double rexit, ServiceDynVolume::Routing routing, const std::string& name):
  SvcRoute(routing, name), m_zPos(z), m_rMin(rmi), m_rMax(rma), m_rExit(rexit), m_next(0){}
      
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

private:

  double m_zPos;
  double m_rMin;
  double m_rMax;
  double m_rExit;
  SvcRoute* m_next;

  VolumeContainer m_volumes;
};

#endif
