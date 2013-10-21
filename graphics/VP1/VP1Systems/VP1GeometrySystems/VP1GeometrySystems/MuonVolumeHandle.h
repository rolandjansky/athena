/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONVOLUMEHANDLE_H
#define MUONVOLUMEHANDLE_H

#include "MuonReadoutGeometry/MuonStation.h"


#include "VP1GeometrySystems/VolumeHandle.h"
#include <QString> 

/*
 *  MuonVolumeHandle.h
 * 
 *
 *  Created by Edward Moyse on 6/11/10.
 *
 */


class MuonVolumeHandle : public VolumeHandle {
public:
  MuonVolumeHandle(VolumeHandleSharedData * shared,VolumeHandle * parent, const GeoPVConstLink& pv,int childNumber,
		   const MuonChamberState& mcs, const SbMatrix& accumTrans,const MuonGM::MuonStation * station, 
		   const QList<const std::map<GeoPVConstLink, float>*>& t0s )
  : VolumeHandle(shared, parent, pv, childNumber, mcs, accumTrans), m_station(station), m_chamberT0s(t0s)
  {}
  
  virtual ~MuonVolumeHandle(){};
  virtual const MuonGM::MuonStation * station() const {return m_station;};
  virtual QString getDescriptiveName() const { 
    if (!m_station) return QString("Unknown");
    QString name = QString(m_station->getStationName().c_str());
    name += "("+QString::number(m_station->getEtaIndex())+","+QString::number(m_station->getPhiIndex())+")";
    
    return name;
  }
  
  virtual QString muonChamberT0(unsigned int i) const {
    if (static_cast<int>(i)>=m_chamberT0s.size())
      return "ERROR bad T0 source";
    const std::map<GeoPVConstLink, float>* t0map = m_chamberT0s.at(i);
    const QString no_info("?");
    if (!t0map)
      return no_info;
    const std::map<GeoPVConstLink, float>::const_iterator it = t0map->find(geoPVConstLink());
    return it==t0map->end() ? no_info : QString::number(it->second);
  }

private:
  const MuonGM::MuonStation * m_station;
  const QList<const std::map<GeoPVConstLink, float>*>& m_chamberT0s;
};

#endif
