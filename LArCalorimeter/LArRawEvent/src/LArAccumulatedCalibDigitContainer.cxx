/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h" 
#include "CLHEP/Units/SystemOfUnits.h"

LArAccumulatedCalibDigitContainer::LArAccumulatedCalibDigitContainer(SG::OwnershipPolicy ownPolicy,
                                                                     double delayScale) : 
  DataVector<LArAccumulatedCalibDigit>(ownPolicy), 
  m_delayScale(delayScale)
{}

LArAccumulatedCalibDigitContainer::~LArAccumulatedCalibDigitContainer() 
{}
