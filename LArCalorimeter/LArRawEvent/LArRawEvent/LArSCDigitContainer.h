/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSCDIGITCONTAINER_H
#define LARSCDIGITCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawEvent/LArSCDigit.h"

/**
@class LArSCDigitContainer
@brief Container class for LArSCDigit
@author Pavol Strizenec
*/

class LArSCDigitContainer : public DataVector<LArSCDigit> {

 public :
  /**
   @brief constructor
   */
  LArSCDigitContainer() : DataVector<LArSCDigit>() { }
  
/**
   @brief destructor 
  */
  virtual ~LArSCDigitContainer();
} ;


CLASS_DEF(LArSCDigitContainer,1153794060,0)

#endif
