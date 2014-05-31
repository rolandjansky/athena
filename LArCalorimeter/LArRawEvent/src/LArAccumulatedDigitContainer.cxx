/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArAccumulatedDigitContainer.h" 


LArAccumulatedDigitContainer::LArAccumulatedDigitContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<LArAccumulatedDigit>(ownPolicy),
  m_ownPolicy(ownPolicy),m_NStep(0),m_StepIndex(0)
{}

LArAccumulatedDigitContainer::~LArAccumulatedDigitContainer() 
{}
