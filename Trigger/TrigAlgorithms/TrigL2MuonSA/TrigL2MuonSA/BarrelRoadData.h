/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_MU_BARRELROADDATA_H
#define TRIGL2MUONSA_MU_BARRELROADDATA_H

namespace TrigL2MuonSA {
  
  class BarrelRoadData {
    
  public:
    typedef float (*Roads)[12][8];
    
  public:
    BarrelRoadData();
    ~BarrelRoadData(void);
    
  public:
    Roads roads(void) const;
    
  private:
    float m_roads[2][12][8];
    
    friend class BarrelRoadDataSvc;
    
  };
  
  inline BarrelRoadData::Roads
    BarrelRoadData::roads(void) const
  {
    return (BarrelRoadData::Roads) m_roads;
  }
  
}

#endif // TRIGL2MUONSA_MU_BARRELROADDATA_H
  
