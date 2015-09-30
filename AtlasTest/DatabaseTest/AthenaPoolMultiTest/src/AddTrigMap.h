/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AddTrigMap.h
  
#ifndef ATHENAPOOLMULTITEST_ADDTRIGMAP_H
#define ATHENAPOOLMULTITEST_ADDTRIGMAP_H

/**
 * @file AddTrigMap.h
 * @brief class definition for AddTrigMap
 */
   
/**
 * @class AddTrigMap
 * @brief Adds a dummy trigger object to the event stream 
 * in order to do the event splitting tests in the package <i>AthenaPoolMultiTest</i>.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: AddTrigMap.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class string;
class ISvcLocator;
class StoreGateSvc;
     
class AddTrigMap : public AthAlgorithm 
{
public:
  AddTrigMap(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~AddTrigMap();
  
  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
                                                                
private:
  StoreGateSvc* m_sGevent;  /// cache for efficiency.
};
#endif
