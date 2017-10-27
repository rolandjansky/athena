/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MBTSCollisionTime_H
#define MBTSCollisionTime_H

/**
   @class MBTSCollisionTime
   @brief Holds information about collisions timing from MBTS counters
   @brief Direct copy of LArCollisionTime class

   @author G.Unal
   
*/

#include "AthenaKernel/CLASS_DEF.h"

class MBTSCollisionTime 
{
  
 public:
  
  /** @brief main constructor with all data members set to zero */
  MBTSCollisionTime();
    
  /** @brief constructor; initialize all data members in constructor */
  MBTSCollisionTime( int ncellA, int ncellC, float energyA, float energyC, float timeA, float timeC);

  /** @brief get number of cells for time A side */
  int ncellA() const{ return m_ncellA; }

  /** @brief get number of cells for time C side */
  int ncellC() const{ return m_ncellC; };

  /** @brief get total number of cells */
  int ncell() const{ return (m_ncellA + m_ncellC); };

  /** @brief total energy A side */
  double energyA() const{ return m_energyA; };
  
  /** @brief total energy C side */
  double energyC() const{ return m_energyC; };
  
  /** @brief total energy from both sides */
  double energy() const{ return (m_energyA + m_energyC); };

  /** @brief time A side */
  double timeA() const{ return m_timeA; };
  
  /** @brief time C side */
  double timeC() const{ return m_timeC; };
  
  /** @brief time difference A - C */
  double time() const{ return (m_timeA - m_timeC); };

 private:

  int m_ncellA;
  int m_ncellC;
  float m_energyA;
  float m_energyC;
  float m_timeA;
  float m_timeC;

};

CLASS_DEF(MBTSCollisionTime,2953,0)

inline MBTSCollisionTime::MBTSCollisionTime()
  : m_ncellA(0), m_ncellC(0), m_energyA(0.), m_energyC(0.), m_timeA(0.), m_timeC(0.) {} 

inline MBTSCollisionTime::MBTSCollisionTime( int ncellA, int ncellC, float energyA, float energyC, float timeA, float timeC)
  : m_ncellA(ncellA), m_ncellC(ncellC), m_energyA(energyA), m_energyC(energyC), m_timeA(timeA), m_timeC(timeC){}


#endif //MBTSCollisionTime_H
