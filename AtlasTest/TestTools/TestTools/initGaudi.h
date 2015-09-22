/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TEST_INITGAUDI_H
# define TEST_INITGAUDI_H
/** @file initGaudi.h
 * @brief  minimal gaudi initialization for AthenaServices unit testing
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: initGaudi.h,v 1.4 2005-11-29 00:51:33 calaf Exp $
 **/

#include <string>

#undef NDEBUG

class ISvcLocator;


namespace Athena_test {
  /** @fn bool initGaudi(ISvcLocator*& pSvcLoc)
   *  @brief  minimal gaudi initialization for AthenaServices unit testing
   *  @param pSvcLoc returns a pointer to the Gaudi ServiceLocator
   */
  bool initGaudi(ISvcLocator*& pSvcLoc);
  /** @fn initGaudi(const std::string& jobOptsFile, ISvcLocator*& pSvcLoc);
   *  @brief  minimal gaudi initialization for AthenaServices unit testing
   *  @param jobOptsFile job opts file name (located at ../share/jobOptFiles)
   *  @param pSvcLoc returns a pointer to the Gaudi ServiceLocator
   */
  bool initGaudi(const std::string& jobOptsFile, ISvcLocator*& pSvcLoc);
}
#endif // TEST_INITGAUDI_H
