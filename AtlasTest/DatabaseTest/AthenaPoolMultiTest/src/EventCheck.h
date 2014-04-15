/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventCheck.h

#ifndef ATHENAPOOLMULTITEST_EVENTCHECK_H
#define ATHENAPOOLMULTITEST_EVENTCHECK_H
/**
 * @file EventCheck.h
 * @brief class definition for EventCheck
 */
 
/**
 * @class EventCheck
 * @brief Counts events in a pool file and tries to read in all objects 
 * listed in the DataHeader.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created October 2004
 * $Id: EventCheck.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
             

#include <string>
#include <set>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

class StoreGateSvc;
class IClassIDSvc;

class EventCheck : public Algorithm {
public:
	  
  EventCheck(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EventCheck();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  StoreGateSvc* m_sGevent; 
  IClassIDSvc* m_pCLIDSvc;

  bool m_chkData;                 /// PROP If this is true the objects are read in.
  IntegerArrayProperty m_exclude; /// PROP List of clid's to exclude as known problems
  StringArrayProperty  m_toCheck; /// PROP List of class names to select objects read in
  int m_maxCheck;                 /// PROP Maximum number of events to check

  int m_nev;                      /// Event count
  std::set<int> m_success;        /// List of clid's successfully read
  std::set<int> m_failures;       /// List of clid's that failed reading
  std::set<int> m_badclids;       /// Redundant with m_exclude
  std::set<int> m_selectedclids;  /// Selected with m_toCheck
};
#endif
