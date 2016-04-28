/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBHVDATACONTAINER_H
#define LARTBHVDATACONTAINER_H

#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "LArTBEvent/LArTBHVData.h"

/** @class LArTBHVDataContainer LArTBHVDataContainer.h 
    "LArTBEvent/LArTBHVDataContainer.h"
    
    @author A. Abdesselam amd P. Ghez
*/
class LArTBHVDataContainer : public DataVector<LArTBHVData> {
 public:
  
  /** Constructor */
  LArTBHVDataContainer() : DataVector<LArTBHVData>() { }
  
  virtual operator std::string();
  
  /**  destructor  */
  virtual ~LArTBHVDataContainer() { }

};

CLASS_DEF(LArTBHVDataContainer,2783,0)
#endif // LARTBHVDATACONTAINER_H



