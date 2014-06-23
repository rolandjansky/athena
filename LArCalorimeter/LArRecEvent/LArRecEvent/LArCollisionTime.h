/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCollisionTime_H
#define LArCollisionTime_H

/**
   @class LArCollisionTime
   @brief Holds information about collisions timing from end-cap LAr calorimeters

   @author G.Unal
   
*/

#include "CLIDSvc/CLASS_DEF.h"

class LArCollisionTime 
{
  
 public:
  
  /** @brief main constructor with all data members set to zero */
  LArCollisionTime();
    
  /** @brief constructor; initialize all data members in constructor */
  LArCollisionTime( int ncellA, int ncellC, float energyA, float energyC, float timeA, float timeC);

  /** @brief get number of cells for time A side */
  int ncellA() const{ return m_ncellA; }

  /** @brief get number of cells for time C side */
  int ncellC() const{ return m_ncellC; };

  /** @brief total energy A side */
  double energyA() const{ return m_energyA; };
  
  /** @brief total energy C side */
  double energyC() const{ return m_energyC; };
  
  /** @brief time A side */
  double timeA() const{ return m_timeA; };
  
  /** @brief time C side */
  double timeC() const{ return m_timeC; };
  
 private:

  int m_ncellA;
  int m_ncellC;
  float m_energyA;
  float m_energyC;
  float m_timeA;
  float m_timeC;

};

CLASS_DEF(LArCollisionTime,79775856,0)

inline LArCollisionTime::LArCollisionTime()
  : m_ncellA(0), m_ncellC(0), m_energyA(0.), m_energyC(0.), m_timeA(0.), m_timeC(0.) {} 

inline LArCollisionTime::LArCollisionTime( int ncellA, int ncellC, float energyA, float energyC, float timeA, float timeC)
  : m_ncellA(ncellA), m_ncellC(ncellC), m_energyA(energyA), m_energyC(energyC), m_timeA(timeA), m_timeC(timeC){}


#endif //LArCollisionTime_H
