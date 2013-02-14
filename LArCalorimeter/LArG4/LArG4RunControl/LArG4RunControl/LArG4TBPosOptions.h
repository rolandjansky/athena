/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LArG4TBPosOptions_h_
#define _LArG4TBPosOptions_h_
#include "CLHEP/Units/SystemOfUnits.h"
#include <string>
// ----------------------------------------------------------------------//
//                                                                       //
// This class defines options for the Positioning of Test Beam components//
// in test beam simulation.  It sets the nickname and nicknumber of      //
// a row in the LArTBPositions table in Oracle (or replica).  It is      //
// originally used in the LAr H6 2003 TB but is probably applicable and  //
// going to be used in other test beam situations.                       //
//                                                                       //
//-----------------------------------------------------------------------//

class LArG4TBPosOptions {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArG4TBPosOptions() : 
    m_PositionNickname("POSITION1"),
    m_PositionNicknumber(0) {}

  void saveMe();
  void printMe();

  // Set methods
  inline void PositionNickname (const std::string &  value)     { m_PositionNickname   = value; }
  inline void PositionNicknumber(int value)                     { m_PositionNicknumber = value; }

  // Get methods
  inline const std::string &  PositionNickname(void) const    { return m_PositionNickname; }
  inline int PositionNicknumber(void) const                   { return m_PositionNicknumber; }

 private:

  // Value of the substep length in the current simulation
  std::string m_PositionNickname;

  // Width of the time bins
  int m_PositionNicknumber;

};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArG4TBPosOptions, 321345482, 1)
#endif

#endif
