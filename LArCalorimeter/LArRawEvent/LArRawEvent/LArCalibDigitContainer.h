/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBDIGITCONTAINER_H
#define LARCALIBDIGITCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArRawEvent/LArCalibDigit.h"

/**
@class LArCalibDigitContainer
@brief Container class for LArCalibDigit
@author Walter Lampl
*/

class LArCalibDigitContainer : public DataVector<LArCalibDigit> {

 public :
  /**
   @brief constructor
   */
  LArCalibDigitContainer() : DataVector<LArCalibDigit>(),m_delayScale(1*CLHEP::ns) { }
  
  /** @brief set delay scale
      @param[in] scale  time in ns between two delay steps
  */
  void setDelayScale(const double scale) {m_delayScale=scale;}
  /** @brief get delay scale
      @return time in ns between two delay steps
  */
  inline double getDelayScale() const {return m_delayScale;}

/**
   @brief destructor 
  */
  virtual ~LArCalibDigitContainer();
private:    

  /** @brief time in ns between two delay step */
  double m_delayScale;

} ;


CLASS_DEF(LArCalibDigitContainer,1204837524,0)

#endif
