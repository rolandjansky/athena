/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOVARIABLETYPE_H
#define CALOEVENT_CALOVARIABLETYPE_H

/** @class CaloVariableType
    @brief definition of Calorimeter Variable types in CaloCluster.
*/ 

struct CaloVariableType
{
  public:
  enum VariableType { ENERGY      = 0,
		      ETA         = 1,
		      PHI         = 2,
		      DELTA_ETA   = 3,
		      DELTA_PHI   = 4,
		      MAX_ENERGY  = 5,
		      MAX_ETA     = 6,
		      MAX_PHI     = 7,
		      NO_OF_TYPES = 8};

  static unsigned int getVariableBit(const VariableType& varType)
  {
    unsigned int bitPattern;
    switch ( varType )
      {
      case ENERGY:     { bitPattern = 0x00000001; break; }
      case ETA:        { bitPattern = 0x00000002; break; }
      case PHI:        { bitPattern = 0x00000004; break; }
      case DELTA_ETA:  { bitPattern = 0x00000008; break; }
      case DELTA_PHI:  { bitPattern = 0x00000010; break; }
      case MAX_ENERGY: { bitPattern = 0x00000020; break; }
      case MAX_ETA:    { bitPattern = 0x00000040; break; }
      case MAX_PHI:    { bitPattern = 0x00000080; break; }
      default:         { bitPattern = 0x00000000; break; }
      };
    return bitPattern;
  }
		      
  static unsigned int getConstMarker()
  {
    return constMarker;
  }

  static bool checkConst(const unsigned int& bitPattern)
  {
    return (constMarker & bitPattern) == constMarker;
  }

  static unsigned int setConstMarker(const unsigned int& bitPattern)
  {
    return bitPattern | constMarker;
  }

  static unsigned int getNumberOfVariables()
  {
    return (unsigned int)NO_OF_TYPES;
  }
 
  static unsigned int getAllVariableBits()
  {
    return allBits;
  }

  private:

  static const unsigned int constMarker = 0x80000000;
  static const unsigned int allBits     = 0x000000ff;

};
#endif
