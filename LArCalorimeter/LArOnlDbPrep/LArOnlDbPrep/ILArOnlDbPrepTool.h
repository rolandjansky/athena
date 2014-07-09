/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLDBPREP_ILARONLDBPREPTOOL_H
#define LARONLDBPREP_ILARONLDBPREPTOOL_H

#include "GaudiKernel/IAlgTool.h"


/**
   @class ILArOnlDbPrepTool
   @brief Tool interface to prepare online calibration data. 

*/
class ILArOnlDbPrepTool: virtual public IAlgTool {

 public: 
  virtual ~ILArOnlDbPrepTool() {}  ; 
  
  /** 
  @brief method to prepare the data for online 
  */
  virtual StatusCode prepare() = 0  ; 

};

#endif 
