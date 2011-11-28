/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcCalibEvent_H
#define ZdcCalibEvent_H

// STL include
#include <vector>

// Lang include
#include <inttypes.h>

#include "DataModel/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"

#include "ZdcEvent/ZdcCell.h"

class ZdcCalibEvent
{

public:

  ZdcCalibEvent ();

  virtual ~ZdcCalibEvent() { }

  /**
   * Clear everything for next event
   */
  //virtual void clear ();

  void print() const;

 private:

  std::vector<double> m_towerE_A;
  std::vector<double> m_towerT_A;
  std::vector<double> m_towerE_C;
  std::vector<double> m_towerT_C;

  ZdcCellVec m_EMCellVec_A;
  ZdcCellVec m_HADCellVec_A;
  ZdcCellVec m_HADCellVec_C;

 public:

  double getTowerEnergy_A(int t) {return m_towerE_A[t];}
  void setTowerEnergy_A(int t, double energy) {m_towerE_A[t] = energy;}
  double getTowerEnergy_C(int t)  {return m_towerE_C[t];}
  void setTowerEnergy_C(int t,double energy)  {m_towerE_C[t] = energy;}

  double getTowerTime_A(int t) {return m_towerT_A[t];}
  void setTowerTime_A(int t, double energy) {m_towerT_A[t] = energy;}
  double getTowerTime_C(int t)  {return m_towerT_C[t];}
  void setTowerTime_C(int t,double energy)  {m_towerT_C[t] = energy;}

  double getTotalEnergy_A();
  double getTotalEnergy_C();

  double getEMEnergy_A();
  double getHADEnergy_A();
  double getHADEnergy_C();

  ZdcCellVec& getEMCell_A() {return m_EMCellVec_A;}
  ZdcCellVec& getHADCell_A() {return m_HADCellVec_A;}
  ZdcCellVec& getHADCell_C() {return m_HADCellVec_C;}

};

CLASS_DEF(ZdcCalibEvent,9292611,0)

#endif

