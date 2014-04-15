/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_PASSALLFILTER_H
#define ATHENAPOOLMULTITEST_PASSALLFILTER_H
/**
 * @file PassAllFilter.h
 * @brief class definition for PassAllFilter
 */

/**
 * @class PassAllFilter
 * @brief Simple Filter algorithm which simply sets pass=true for all events.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: PassAllFilter.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
            
#include "AthenaBaseComps/AthFilterAlgorithm.h"
 
class string;
class ISvcLocator;
     
class PassAllFilter : public AthFilterAlgorithm 
{
public:
  PassAllFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PassAllFilter();

  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
};
#endif
