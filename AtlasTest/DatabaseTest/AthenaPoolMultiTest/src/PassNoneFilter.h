/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PassNoneFilter.h
  
#ifndef ATHENAPOOLMULTITEST_PASSNONEFILTER_H
#define ATHENAPOOLMULTITEST_PASSNONEFILTER_H
/**
 * @file AthenaPoolMultiTest/src/PassNoneFilter.h
 * @brief class definition for PassNoneFilter
 */
 
/**
 * @class PassNoneFilter
 * @brief Simple Filter algorithm which simply sets pass=false for all events.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: PassNoneFilter.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
*/
             
#include "AthenaBaseComps/AthFilterAlgorithm.h"
    
class string;
class ISvcLocator;
     
class PassNoneFilter : public AthFilterAlgorithm 
{
public:
  PassNoneFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PassNoneFilter();

  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
};
#endif
