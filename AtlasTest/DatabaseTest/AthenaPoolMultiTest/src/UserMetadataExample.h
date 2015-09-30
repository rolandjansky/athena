/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_APMTUSEREXAMPLE_H
#define ATHENAPOOLMULTITEST_APMTUSEREXAMPLE_H
/**
 * @file UserMetadataExample.h
 * @brief class definition for UserMetadataExample
 */

/**
 * @class UserMetadataExample
 * @brief Simple Filter algorithm which simply sets pass=true for all events.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: UserMetadataExample.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
            
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
 
class string;
class ISvcLocator;
     
class UserMetadataExample : public AthAlgorithm 
{
public:
  UserMetadataExample(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~UserMetadataExample();

  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
private:
  bool m_done;
};
#endif
