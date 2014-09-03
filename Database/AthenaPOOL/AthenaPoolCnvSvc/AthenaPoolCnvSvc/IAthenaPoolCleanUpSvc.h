/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_IATHENAPOOLCLEANUPSVC_H
#define ATHENAPOOLCNVSVC_IATHENAPOOLCLEANUPSVC_H

/** @file IAthenaPoolCleanUpSvc.h
 *  @brief This file contains the class definition for the IAthenaPoolCleanUpSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/IAthenaPoolCleanUp.h"
#include "GaudiKernel/StatusCode.h"

#include <vector>

/** @class IAthenaPoolCleanUpSvc
 *  @brief This class provides the interface for the IAthenaPoolCleanUpSvc which is used to clean up
 *  AthenaPoolConverter
 **/
class IAthenaPoolCleanUpSvc {

public:
   /// Destructor
   virtual ~IAthenaPoolCleanUpSvc() {};

   virtual StatusCode registerCleanUp(IAthenaPoolCleanUp* cnv) = 0;
   virtual StatusCode cleanUp() = 0;

protected:
   /// Standard Constructor
   IAthenaPoolCleanUpSvc() : m_cnvs() {};

   std::vector<IAthenaPoolCleanUp*> m_cnvs;
};

#endif
