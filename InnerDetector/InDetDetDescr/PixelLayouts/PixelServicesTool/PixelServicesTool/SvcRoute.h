/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SvcRoute_H
#define SvcRoute_H

#include "AthenaKernel/MsgStreamMember.h"

#include <vector>
#include <string>

class ServiceDynVolume;

class SvcRoute {
public:

  typedef std::vector<ServiceDynVolume*>   VolumeContainer;

  virtual ServiceDynVolume* entryVolume( double pos, bool ascending, const Athena::MsgStreamMember& msg) const ;

  virtual ServiceDynVolume* exitVolume( bool ascending, const Athena::MsgStreamMember& msg) const ;

  
  SvcRoute(): m_exitVolume(0) {}
  SvcRoute( const std::string& name): m_exitVolume(0), m_name(name) {}

  virtual ~SvcRoute() {}

  virtual double position() const = 0;

  virtual double exit() const = 0;

  //virtual int direction() = 0;

  virtual SvcRoute* nextRoute() const = 0;

  virtual const VolumeContainer& volumes() const = 0;

  virtual void setNextRoute( SvcRoute* nr) = 0;

  const std::string& name() const {return m_name;}

  // virtual ServiceDynVolume* entryVolume( double pos) const = 0;

  // virtual ServiceDynVolume* exitVolume() const = 0;

  virtual void addVolume( ServiceDynVolume* vol) = 0;

  void setExitVolume( ServiceDynVolume* vp) { 
    m_exitVolume = vp;
    addVolume(vp);
  }

private:
  
  ServiceDynVolume* m_exitVolume;
  std::string    m_name;

};

#endif
