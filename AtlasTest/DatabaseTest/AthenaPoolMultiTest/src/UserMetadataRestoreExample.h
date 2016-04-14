/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_USERMETADATARESTOREEXAMPLE_H
#define ATHENAPOOLMULTITEST_USERMETADATARESTOREEXAMPLE_H
/**
 * @file UserMetadataRestoreExample.h
 * @brief class definition for UserMetadataRestoreExample
 */

/**
 * @class UserMetadataRestoreExample
 * @brief Simple Filter algorithm which simply sets pass=true for all events.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: UserMetadataRestoreExample.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
            
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/Algorithm.h"
 
class string;
class ISvcLocator;
     
class UserMetadataRestoreExample : public AthAlgorithm 
{
public:
  UserMetadataRestoreExample(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~UserMetadataRestoreExample();

  virtual StatusCode initialize(); 
  virtual StatusCode start();
  virtual StatusCode execute();
  virtual StatusCode finalize();
private:
  bool m_done;
  bool m_detStoreWrite;
};
#endif

