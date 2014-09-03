/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOOBJECT_H
#define CALOEVENT_CALOOBJECT_H
//

/** @class CaloObject
 *  @brief obsolete BaseClass for Calorimeter objects, such as CaloCell and CaloCluster.
 *  @author Hong Ma 
 *   Update: Jan 10 2001 HMA
      ordor of static/virtual/const 
*/

#include <math.h>

class CaloObject
{
public:
  // constructors 
  CaloObject() { };

  virtual ~CaloObject() { };

  /** @brief get energy in MeV */ 
  virtual double energy()  const = 0;
  /** @brief get transverse energy in MeV */ 
  virtual double et()      const = 0;
  /** @brief get phi */ 
  virtual double phi ()    const = 0;
  /** @brief get eta */ 
  virtual double eta ()    const = 0;

};

#endif 
