/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_IATHENAPOOLCLEANUP_H
#define ATHENAPOOLCNVSVC_IATHENAPOOLCLEANUP_H

/** @file IAthenaPoolCleanUp.h
 *  @brief This file contains the class definition for the IAthenaPoolCleanUp class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/StatusCode.h"

/** @class IAthenaPoolCleanUp
 *  @brief This class provides the interface for the AthenaPoolCleanUp which is used to clean up
 *  AthenaPoolConverter
 **/
class IAthenaPoolCleanUp {

public:
   /// Destructor
   virtual ~IAthenaPoolCleanUp() {};

   virtual StatusCode cleanUp() = 0;

protected:
   /// Standard Constructor
   IAthenaPoolCleanUp() {};
};

#endif
