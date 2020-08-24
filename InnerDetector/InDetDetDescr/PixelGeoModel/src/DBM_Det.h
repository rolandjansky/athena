/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DBM_Det.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DBMDET_H
#define DBMDET_H


#include "GeoVPixelFactory.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>



  /** @class DBM_Det 

  Diamond Beam Monitor detector builder
  */  

class ATLAS_NOT_THREAD_SAFE DBM_Det : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  DBM_Det();
  virtual  GeoVPhysVol* Build();
  
 private:
  
  /** member variables for algorithm properties: */
  // int/double/bool  m_propertyName;
  std::vector<double> m_module[4];
};

#endif
