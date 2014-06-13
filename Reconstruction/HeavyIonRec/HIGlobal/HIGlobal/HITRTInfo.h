/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HiTRTInfo.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HITRTINFO_H
#define HITRTINFO_H

// #include <string>
#include <vector>
// #include <map>

// using namespace std;

/** @class HITRTInfo

    Stores heavy ion TRT occupancy calculated from RDO container
*/

class Identifier;
class HITRTInfo 
{
public:
  
  //friend class HITRT;
  friend class HITRTInfoCnv_p1;
  
  //HITRTInfo(char *name);
  HITRTInfo(const std::vector<int>& OccupancyPerPart);
  HITRTInfo() {};
  ~HITRTInfo() {};

  //Functions getting Occupancy in raw
  //=====================================================================
  int GetAllStraws() { 
    return m_OccupancyPerPart.at(0); } ///< !!! Hits in event
  int GetDeadStraws() { 
    return m_OccupancyPerPart.at(1); } ///< !!! Noisy hits(straws) in event
  int GetBarrelStraws() { 
      return m_OccupancyPerPart.at(2); } ///< !!! Hits in event Barrel
  int GetEndCapStraws() { 
      return m_OccupancyPerPart.at(3); } ///< !!! Hits in event EndCap
  int GetEndCapAStraws() { 
      return m_OccupancyPerPart.at(4); } ///< !!! Hits in event EndCap A
  int GetEndCapCStraws() { 
      return m_OccupancyPerPart.at(5); } ///< !!! Hits in event EndCap C 

 
protected:
  

private:

  // char *m_name;
  std::vector<int> m_OccupancyPerPart;

};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(HITRTInfo, 225913892, 1)

#endif 
