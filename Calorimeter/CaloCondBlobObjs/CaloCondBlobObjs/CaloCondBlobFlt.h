/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_CALOCONDBLOBFLT_H
#define CALOCONDBLOBOBJS_CALOCONDBLOBFLT_H

/** 
    @brief Class for storing a number of floats (Flt) and functions on those.
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details This class implements the infrastructure to store a
    (variable) number of floats for each channel or ADC. 
    The stored floats can be used to define a function of one parameter. Different 
    functional forms can be implemented and differentiated by the objVersion header 
    variable (this allows for e.g. schema evolution). The functions are evaluated 
    by calling the getCalib() function.
*/

#include "CaloCondBlobObjs/CaloCondBlobDat.h"
#include "CaloCondBlobObjs/CaloCondType.h"
#include <stdint.h>
#include <vector>
#include <ostream>
#include <iostream>
#include "CoralBase/Blob.h"

class CaloCondBlobFlt : public CaloCondBlobDat<float>
{
 public:

  /** @brief Dtor */
  virtual ~CaloCondBlobFlt(){}
  
  /** @brief Returns CaloCondType::FLT. */
  virtual uint16_t getType() const {return CaloCondType::FLT;} 
  
  /** @brief Returns a pointer to a non-const CaloCondBlobFlt. */
  static CaloCondBlobFlt* getInstance(coral::Blob& blob);

  /** @brief Returns a pointer to a const CaloCondBlobFlt. */
  static const CaloCondBlobFlt* getInstance(const coral::Blob& blob);
  
  /** @brief Returns the calibrated energy for a given channel/gain and input energy.
      @param channel The channel number
      @param adc The gain index
      @param x Parameter of the (calibration) function
      @param invert If true, the calibration is undone */
  float getCalib(unsigned int channel, unsigned int adc, float x, bool invert=false) const;

 protected:
  /** @brief Ctor. */
  CaloCondBlobFlt(const coral::Blob& blob);
  /** @brief Ctor. */
  CaloCondBlobFlt(coral::Blob& blob);
  
};
#endif
