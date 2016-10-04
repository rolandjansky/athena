/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_TRIGPREFLIGHTCHECK_H
#define TRIGSERVICES_TRIGPREFLIGHTCHECK_H
/**
 * @file   TrigPreFlightCheck.h
 * @brief  Does some generic checks to make sure we can run the HLT
 * @author Frank Winklmeier
 *
 * $Id: TrigPreFlightCheck.h 5 2013-05-14 10:33:04Z ricab $
 */

// STL includes
#include <string>
#include <vector>

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

/**
 * @brief Tool to do some basic checks ensuring the HLT is usable
 *
 * The tool has several checkXYZ() methods each returning a StatusCode for
 * the specific check it is doing. In addition, the check() method performs
 * all the checks and returns the logical AND of the individual checks.
 */
class TrigPreFlightCheck : public AthAlgTool {

public:

  TrigPreFlightCheck(const std::string &type, const std::string &name, const IInterface *parent);    
  virtual ~TrigPreFlightCheck();

  static const InterfaceID& interfaceID();  
  virtual StatusCode initialize();
  
  /**
   * @brief Check whether the relase is installed properly
   *
   * @param  errLvl   Message level used for failure messages
   * @return SUCCESS  Release properly installed
   *         FAILURE  Some parts of the release is missing
   */
  StatusCode checkRelease(const MSG::Level& errLvl);

  /**
   * @brief Perform all checks
   *
   * @param  errLvl   Message level used for failure messages
   * @return SUCCESS  All checks OK
   *         FAILURE  At least one check failed
   */
  StatusCode check(const MSG::Level& errLvl);

private:
  /** Directories to check for existence in checkRelease() */
  std::vector<std::string> m_releaseDirs;
};


inline const InterfaceID& TrigPreFlightCheck::interfaceID()
{
  static const InterfaceID _IID("TrigPreFlightCheck", 1, 0);
  return _IID;
}

#endif
