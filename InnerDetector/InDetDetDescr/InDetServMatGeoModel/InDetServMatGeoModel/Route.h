/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Route_H
#define Route_H

#include "AthenaKernel/MsgStreamMember.h"

#include <vector>
#include <string>

class ServiceVolume;

class Route {
public:

  typedef std::vector<ServiceVolume*>   VolumeContainer;

  virtual ServiceVolume* entryVolume( double pos, bool ascending, Athena::MsgStreamMember& msg) const ;

  virtual ServiceVolume* exitVolume( bool ascending, Athena::MsgStreamMember& msg) const ;

  
  Route(): m_exitVolume(0) {}
  Route( const std::string& name): m_exitVolume(0), m_name(name) {}

  virtual ~Route() {}

  virtual double position() const = 0;

  virtual double exit() const = 0;

  //virtual int direction() = 0;

  virtual Route* nextRoute() = 0;

  virtual const VolumeContainer& volumes() const = 0;

  virtual void setNextRoute( Route* nr) = 0;

  const std::string& name() const {return m_name;}

  // virtual ServiceVolume* entryVolume( double pos) const = 0;

  // virtual ServiceVolume* exitVolume() const = 0;

  virtual void addVolume( ServiceVolume* vol) = 0;

  void setExitVolume( ServiceVolume* vp) { 
    m_exitVolume = vp;
    addVolume(vp);
  }

private:
  
  ServiceVolume* m_exitVolume;
  std::string    m_name;

};

#endif
