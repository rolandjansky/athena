/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_STOREDUMP_H
#define ATHENAPOOLMULTITEST_STOREDUMP_H
/**
 * @file StoreDump.h
 * @brief class definition for StoreDump
 */

/**
 * @class StoreDump
 * @brief Simple Filter algorithm which simply sets pass=true for all events.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: StoreDump.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
            
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
 
class string;
class ISvcLocator;
     
class StoreDump : public AthAlgorithm 
{
public:
  StoreDump(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~StoreDump();

  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
private:
  std::string m_storename;
  std::string m_key;
};
#endif
