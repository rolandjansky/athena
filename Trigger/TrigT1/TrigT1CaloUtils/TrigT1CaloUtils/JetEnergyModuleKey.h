/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetEnergyModuleKey.h  -  description
                             -------------------
    begin                : Monday August 20 2001
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#ifndef JETENERGYMODULEKEY_H
#define JETENERGYMODULEKEY_H

#include <iostream>
#include <vector>

// running in Athena
#include "xAODTrigL1Calo/JetElement.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloUtils/KeyUtilities.h"
#include "TrigT1CaloUtils/BinAndCoord.h"
#include "TrigT1Interfaces/Coordinate.h"

namespace LVL1 {

/**
  The JetEnergyModuleKey object provides the key for each trigger tower
  depending on its eta-phi coords <p>
  The key is an integer number that uniquely
  identifies each tower, and is needed by the STL map container: the map holds
  trigger towers and we retrieve them using their keys.
  *@author Edward Moyse
  */
class JetEnergyModuleKey : public KeyUtilities {

public:
  enum JEMRegion {
    MidJEM=0,
    LeftEndJEM=1,
    RightEndJEM=2,
    JEMRegionERROR=999
  };
  
  JetEnergyModuleKey();
  virtual ~JetEnergyModuleKey();
  

  /** returns the key of the passed Coordinate */
  unsigned int jemKey(const xAOD::JetElement* jetElement);
  /** calculates key from crate and module index */
  unsigned int jemKey(unsigned int crate, unsigned int module);
  /** calculates keys of all JetElements in given crate and module */
  std::vector<unsigned int> jeKeys(unsigned int crate, unsigned int module);
  /** returns coordinates of all JetElements in given crate and module */
  std::vector<Coordinate> jeCoords(unsigned int crate, unsigned int module);
  /** returns the phi row of a coord within the JEM that contains it.
  Used in energy trigger (JEM) */
  unsigned int row(const Coordinate& coord) const;
  /** returns the phi row of a coord within the JEM that contains it.
  Used in energy trigger (JEM) */
  unsigned int row(const xAOD::JetElement* jetElement) const;
  /** returns the phi coord of the row of the JEM at the coordinates passed*/
  double rowPhiCoord(unsigned int row, const Coordinate& jemCoord);
  /** height */
  double dPhi(const Coordinate& coord) const;
  /** width */
  double dEta(const Coordinate& coord) const;
  /** region */
  JEMRegion region(const Coordinate& coord) const;
  /** return row of passed coordinate */
  unsigned int col(const Coordinate& coord) ;
  /** returns ID of JEP module (i.e. JEM) There are 16 JEMS per crate, and 8 JEMS per quadrant.
  The central 6 JEMS cover the barrel, JEMs 0,7,8 & 15 extend over the FCAL as well.
  (each crate contains JEMS for opposite quadrants, so one crate might have JEMs 0-7 in quadrant 0
  and JEMs 8-15 in quadrant 2)
  */
  unsigned int jem(const Coordinate & coord) const;
  
private: // Private methods
  /** converts integer phi, eta coordinates to phi, eta trigger bins,
  and central coords*/
  BinAndCoord* calculateTriggerBin(ICoordinate* iCoord);
  /** sets the eta and phi bins*/
  void setBins(ICoordinate* iCoord,unsigned int& phiBin, int& etaBin);
  /** No descriptions */
  unsigned int midJEMEtaCol(const Coordinate& coord) const;
  /** returns eta row of JEMs 0, or 8*/
  unsigned int leftEndJEMEtaCol(const Coordinate& coord) const;
   /** returns eta row of JEMs 7 or 15 */
  unsigned int rightEndJEMEtaCol(const Coordinate& coord) const;
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
