/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * main program for the Validation
 */
#include "MdtCalibValidation/MdtT0ValidationHandler.h"

int main()
{

// construct the default CalibrationManager
  MdtT0ValidationHandler* ValHandl = new MdtT0ValidationHandler::MdtT0ValidationHandler;
  ValHandl->initialize();
  ValHandl->execute();
  ValHandl->finalize();
  }
