/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          TriggerTowerKey.h  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#ifndef TRIGGERTOWERKEY_H

#define TRIGGERTOWERKEY_H

#include <iostream>
#include <stdlib.h>

#ifndef  TRIGGERSPACE
// running in Athena

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/KeyUtilities.h"
#include "TrigT1CaloUtils/ICoordinate.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1CaloUtils/BinAndCoord.h"
#include "CaloEvent/CaloCell.h"


#else
// running in TRIGGERSPACE
#include "KeyUtilities.h"
#include "ICoordinate.h"
#include "BinAndCoord.h"
#include "Coordinate.h"

#endif



namespace LVL1 {

/**
  The TriggerTowerKey object provides the key for each trigger tower
  depending on its eta-phi coords <p>
  The key is an integer number that uniquely
  identifies each tower, and is needed by the STL map container: the map holds
  trigger towers and we retrieve them using their keys.
  *@author Edward Moyse
  */
class TriggerTowerKey : public KeyUtilities {
public:

enum TriggerTowerRegion {
  TTBarrel=0,
  TTEndCap1=1,
  TTEndCap2=2,
  TTFCAL=3,
  TTRegionError=999
};

  TriggerTowerKey(double m_phi, double m_eta);
  TriggerTowerKey();
  virtual ~TriggerTowerKey();
  
#ifndef  TRIGGERSPACE
  // these methods are only applicable if running in Athena
  /** returns the key of the passed tower */
  virtual unsigned int ttKey( const TriggerTower & tower);
  /** returns the key of the passed calo cell */
  unsigned int ttKey( const CaloCell & calocell) ;
#endif

  /** returns trigger tower key of passed coords */
  unsigned int ttKey(const double phi, const double eta);
  /** returns the TT region that the coordinate corresponds to. */
  TriggerTowerRegion region(const Coordinate&  coord) const;
  /** returns eta width of trigger tower at coordinate coord */
  double dEta(const Coordinate& coord) const;
  /** returns phi height of trigger tower at coordinate coord */
  double dPhi(const Coordinate& coord) const;




private: // Private methods
  /** converts integer phi, eta 
coordinates to phi, eta trigger bins. */
  BinAndCoord* calculateTriggerBin(ICoordinate* iCoord);
private:
//  bool m_debug;
};
}
#endif
