//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* author : Pierre-Antoine Delsart */

/* date of creation : 04/10/2004 */
/* Copy of LArCalibDigitContainer*/

#ifndef TBLARCALIBDIGITCONTAINER_H
#define TBLARCALIBDIGITCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArRawEvent/LArCalibDigit.h"

/**
  *  Liquid Argon Calibration Digit Container <br><br>
  * 
  * Publicly inherits from ATHENA ObjectVector<br>
  * @author Walter Lampl
  */
class TBLArCalibDigitContainer : public DataVector<LArCalibDigit> {

 public :
  /**
   * constructor
   */
  TBLArCalibDigitContainer() : DataVector<LArCalibDigit>(),m_delayScale(1*CLHEP::ns) { }
  
  /**
   * Setter & Getter for delayScale
   */
  void setDelayScale(const double scale) {m_delayScale=scale;}
  inline double getDelayScale() const {return m_delayScale;}

/**
  * destructor 
  */
  virtual ~TBLArCalibDigitContainer();
private:    
  double m_delayScale;

} ;

CLASS_DEF( TBLArCalibDigitContainer , 1147713396 , 1 )


#endif
