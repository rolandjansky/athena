/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    
@class LArAccumulatedCalibDigitContainer
@brief Container class for LArAccumulatedCalibDigit

@author Sandrine Laplace
@author Isabelle Wingerter-Seez

*/

#ifndef LARACCUMULATEDCDIGITCONTAINER_H
#define LARACCUMULATEDCDIGITCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawEvent/LArAccumulatedCalibDigit.h"
#include "AthContainers/ConstDataVector.h"
#include "CLHEP/Units/SystemOfUnits.h"


class LArAccumulatedCalibDigitContainer : public DataVector<LArAccumulatedCalibDigit> {
  
 public :
  /** @brief Default Constructor */
  LArAccumulatedCalibDigitContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;
  /** @brief destructor */
  virtual ~LArAccumulatedCalibDigitContainer();
  
  /** @brief set the delay Scale   */
  void setDelayScale(const double scale) {m_delayScale=scale;}

  /** @brief get the delay Scale   */
  inline double getDelayScale() const {return m_delayScale;}
  
 private:    

  /** @brief delayScale */
  double m_delayScale;

  /** @brief SG ownership policy  */
  //SG::OwnershipPolicy  m_ownPolicy ;
};


CLASS_DEF(LArAccumulatedCalibDigitContainer,1157958831,0)

#endif
