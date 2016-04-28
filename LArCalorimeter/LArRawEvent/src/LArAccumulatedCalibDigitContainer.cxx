/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h" 
#include "CLHEP/Units/SystemOfUnits.h"

LArAccumulatedCalibDigitContainer::LArAccumulatedCalibDigitContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<LArAccumulatedCalibDigit>(ownPolicy), 
  m_delayScale(1*CLHEP::ns)
  //m_ownPolicy(ownPolicy)
{}

LArAccumulatedCalibDigitContainer::~LArAccumulatedCalibDigitContainer() 
{}
