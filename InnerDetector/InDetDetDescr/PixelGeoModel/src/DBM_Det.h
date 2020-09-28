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

class DBM_Det : public GeoVPixelFactory {
 public:
  DBM_Det(InDetDD::PixelDetectorManager* ddmgr, PixelGeometryManager* mgr);
  virtual  GeoVPhysVol* Build() override;
  
 private:
  
  /** member variables for algorithm properties: */
  // int/double/bool  m_propertyName;
  std::vector<double> m_module[4];
};

#endif
