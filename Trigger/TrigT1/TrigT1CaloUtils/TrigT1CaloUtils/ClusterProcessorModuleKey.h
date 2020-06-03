/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ClusterProcessorModuleKey.h  -  description
                             -------------------
    begin                : Monday August 20 2001
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#ifndef ClusterProcessorModuleKey_H
#define ClusterProcessorModuleKey_H

#include <iostream>
#include <vector>

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/KeyUtilities.h"
#include "TrigT1CaloUtils/BinAndCoord.h"
#include "TrigT1Interfaces/Coordinate.h"

#else
// running in TRIGGERSPACE/
#include "TriggerTower.h"
#include "BinAndCoord.h"
#include "Coordinate.h"
#include "KeyUtilities.h"

#endif

namespace LVL1 {

/**
  The ClusterProcessorModuleKey object provides the key for each trigger tower
  depending on its eta-phi coords <p>
  The key is an integer number that uniquely
  identifies each tower, and is needed by the STL map container: the map holds
  trigger towers and we retrieve them using their keys.
  *@author Edward Moyse
  */
class ClusterProcessorModuleKey : public KeyUtilities {

public:
  
  ClusterProcessorModuleKey();
  virtual ~ClusterProcessorModuleKey();
  
  /** calculates key from crate and module index */
  unsigned int cpmKey(unsigned int crate, unsigned int module);
  /** height */
  double dPhi(const Coordinate& coord) const;
  /** width */
  double dEta(const Coordinate& coord) const;
  /** returns CPM number within a crate for a TriggerTower or RoI coordinate*/
  unsigned int cpm(const Coordinate & coord) const;
  
private: // Private methods
  /** converts integer phi, eta coordinates to phi, eta trigger bins,
  and central coords*/
  BinAndCoord* calculateTriggerBin(ICoordinate* iCoord);
  /** sets the eta and phi bins*/
  void setBins(ICoordinate* iCoord,unsigned int& phiBin, int& etaBin);
  /** returns the quadrant number associated with the phi coordinate,
  0   - 90   = 0
  90 - 180 = 1
  180-270 = 2
  270-360 = 3 */
  unsigned int phiQuadrant(const double phi) const ;
private:
  /**set to true to turn debugging info on*/
  bool m_debug;
};
}

#endif
