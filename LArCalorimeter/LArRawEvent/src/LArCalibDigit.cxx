/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArCalibDigit.h"

/** Destructor */
LArCalibDigit::~LArCalibDigit()
{}

/** default constructor for persistency */
LArCalibDigit::LArCalibDigit()
  : LArDigit(),
    m_DAC(0),
    m_delayPulsed(0)
{}
