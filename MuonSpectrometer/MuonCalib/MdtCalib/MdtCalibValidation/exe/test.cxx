/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * main program for the Validation
 */
// #include "MdtCalibValidation/MDTT0CalibrationHandler.h"
#include "MdtCalibValidation/MdtT0ValidationHandler.h"

int main()
{

// construct the default CalibrationManager
  // MDTT0CalibrationHandler* ValHandl = new MDTT0CalibrationHandler::MDTT0CalibrationHandler;
  MdtT0ValidationHandler* ValHandl = new MdtT0ValidationHandler::MdtT0ValidationHandler;
  ValHandl->initialize();
  // ValHandl->TestRootDB();
  // ValHandl->FillTreeFromRootDB();
  ValHandl->execute();
  ValHandl->finalize();
  }
