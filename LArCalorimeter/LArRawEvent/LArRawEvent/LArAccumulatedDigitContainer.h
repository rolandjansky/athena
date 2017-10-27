/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    
@class LArAccumulatedDigitContainer
@brief Container class for LArAccumulatedDigit

@author Remi Lafaye
*/

#ifndef LARACCUMULATEDDIGITCONTAINER_H
#define LARACCUMULATEDDIGITCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArRawEvent/LArAccumulatedDigit.h"


class LArAccumulatedDigitContainer : public DataVector<LArAccumulatedDigit> {
  
 public :
  /** @brief Default Constructor */
  LArAccumulatedDigitContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;
  /** @brief destructor */
  virtual ~LArAccumulatedDigitContainer();
    
  inline void setNStep    (unsigned int NStep)     { m_NStep=NStep; }
  inline void setStepIndex(unsigned int StepIndex) { m_StepIndex=StepIndex; }
  inline unsigned int getNStep    () const { return m_NStep; }
  inline unsigned int getStepIndex() const { return m_StepIndex; }
  inline int isLastStep() const { return (m_StepIndex==m_NStep-1); }
  //private:    

  /** @brief SG ownership policy  */
  SG::OwnershipPolicy  m_ownPolicy ;
  unsigned int m_NStep, m_StepIndex;
};


CLASS_DEF(LArAccumulatedDigitContainer,1185131589,0)

#endif
