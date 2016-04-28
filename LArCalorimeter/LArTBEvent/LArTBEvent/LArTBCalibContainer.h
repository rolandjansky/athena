/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBCALIBCONTAINER_H
#define LARTBCALIBCONTAINER_H

#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "LArTBEvent/LArTBCalib.h"


/** @class LArTBCalibContainer LArTBCalibContainer.h 
    "LArTBEvent/LArTBCalibContainer.h"
    
    @ author A. Abdesselam and P. Ghez
*/
class LArTBCalibContainer : public DataVector<LArTBCalib> {
 public:
  
  /** Container */
  LArTBCalibContainer() : DataVector<LArTBCalib>() { }
  
  virtual operator std::string();
  
  /**  destructor  */
  virtual ~LArTBCalibContainer() { }

};

CLASS_DEF(LArTBCalibContainer,2782,0)
#endif
