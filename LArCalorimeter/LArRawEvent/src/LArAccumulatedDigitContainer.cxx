/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArAccumulatedDigitContainer.h" 


LArAccumulatedDigitContainer::LArAccumulatedDigitContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<LArAccumulatedDigit>(ownPolicy),
  m_NStep(0),m_StepIndex(0)
{}

LArAccumulatedDigitContainer::~LArAccumulatedDigitContainer() 
{}
