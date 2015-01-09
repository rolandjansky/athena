/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RequireUniqueEvent.h
  
#ifndef ATHENAPOOLMULTITEST_REQUIREUNIQUEEVENT_H
#define ATHENAPOOLMULTITEST_REQUIREUNIQUEEVENT_H

/**
 * @file RequireUniqueEvent.h
 * @brief class definition for RequireUniqueEvent
 */
   
/**
 * @class RequireUniqueEvent
 * @brief Track event uniqueness by inserting into STL set. 
 * Issues a WARNING if an event repeats <i>within the job</i>
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * <i>No settable properties</i>
 * $Id: RequireUniqueEvent.h,v 1.3 2007-06-20 15:38:49 cranshaw Exp $
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventID.h"
#include <set>

class string;
class ISvcLocator;
     
class RequireUniqueEvent : public AthAlgorithm 
{
public:
  RequireUniqueEvent(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RequireUniqueEvent();
  
  virtual StatusCode initialize(); 
  virtual StatusCode execute();
  virtual StatusCode finalize();
                                                                
private:
  std::set<EventID> m_evList;
  int m_cnt;
};
#endif
