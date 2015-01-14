/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#define MSG(__sev, __msg) { std::cout << std::setw(10) << std::left << __sev <<" LINE:" << std::setw(4) << __LINE__ << " " << __msg << std::endl;}
#define REPORT_AND_STOP(__msg) { MSG("ERROR", __msg) return false; }
#define ABORT(__msg) { MSG("ERROR", __msg) return -1; }
#define BEGIN_TEST MSG("INFO", "") MSG("INFO", "Test " << __FUNCTION__)
#define PROGRESS MSG("DEBUG", "Test progressing")
