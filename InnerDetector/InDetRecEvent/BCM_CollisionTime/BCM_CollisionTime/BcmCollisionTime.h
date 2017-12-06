/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_CollisionTime_H
#define BCM_CollisionTime_H

/**
   @class BcmCollisionTime
   @brief Holds information about bkg and collisions timing and multiplicity from BCM stations

   @author Hendrik Jansen
   
*/

#include "AthenaKernel/CLASS_DEF.h"
#include <vector>

class BcmCollisionTime
{
  
 public:
  
  /** @brief main constructor with all data members set to zero */
  BcmCollisionTime();
    
  /** @brief constructor; initialize all data members in constructor */
  //BcmCollisionTime( int multiLG, int multiHG, bool IsCol, bool IsBkg);

  /** @brief constructor; initialize all data members in constructor and include vector*/
  //BcmCollisionTime( int multiLG, int multiHG, bool IsCol, bool IsBkg, std::vector<float> deltaT);

  /** @brief constructor; initialize all data members in constructor and include vector*/
  BcmCollisionTime( int multiLG, int multiHG, std::vector<float> deltaT);

  /** @brief destructor; initialize all data members in constructor and include vector*/
  //~BcmCollisionTime();

  /** @brief get vector of deltaTs */
  //float* getDeltaT() const{ return m_deltaT; }

  /** @brief get LG multiplicity */
  int getMultiLG() const{ return m_multiLG; };

  /** @brief get HG multiplicity */
  int getMultiHG() const{ return m_multiHG; };

  /** @brief get if event is col like */
  //bool getIsCol() const{ return m_IsCol; };
  
  /** @brief get if event is bkg like */
  //bool getIsBkg() const{ return m_IsBkg; };

  /** @brief get complete list of deltaTs */
  std::vector<float> getDeltaT() const{ return m_deltaT; };
  
 private:

  int m_multiLG;
  int m_multiHG;
  //bool m_IsCol;
  //bool m_IsBkg;
  std::vector<float> m_deltaT;

};

CLASS_DEF(BcmCollisionTime,79775858,0)

/*inline BcmCollisionTime::BcmCollisionTime()
  : m_multiLG(0), m_multiHG(0), m_IsCol(false), m_IsBkg(false) {} */

/*inline BcmCollisionTime::BcmCollisionTime( int multiLG, int multiHG, bool IsCol, bool IsBkg)
  : m_multiLG(multiLG), m_multiHG(multiHG), m_IsCol(IsCol), m_IsBkg(IsBkg){}*/

inline BcmCollisionTime::BcmCollisionTime( int multiLG, int multiHG, std::vector<float> deltaT)
  : m_multiLG(multiLG), m_multiHG(multiHG), m_deltaT(deltaT){}

/*inline BcmCollisionTime::BcmCollisionTime( int multiLG, int multiHG, bool IsCol, bool IsBkg, std::vector<float> deltaT)
  : m_multiLG(multiLG), m_multiHG(multiHG), m_IsCol(IsCol), m_IsBkg(IsBkg), m_deltaT(deltaT){}*/


#endif //BCM_CollisionTime_H

