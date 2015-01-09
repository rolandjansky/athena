/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventCount.h

#ifndef ATHENAPOOLTOOLS_EVENTCOUNT_H
#define ATHENAPOOLTOOLS_EVENTCOUNT_H
/**
 * @file EventCount.h
 * @brief class definition for EventCount
 */
 
/**
 * @class EventCount
 * @brief Counts events in a pool file, and can also count the objects 
 * and give a summary of their keys. Output is via the message logger.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created October 2004
 * $Id: EventCount.h,v 1.4 2007-12-20 21:54:12 cranshaw Exp $
 */
             
                                                                                
#include <string>
#include <vector>
#include <map>
#include <set>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "PersistentDataModel/Guid.h"
#include "EventInfo/EventID.h"

class IClassIDSvc;

class EventCount : public AthAlgorithm, virtual public IIncidentListener {
public:

  /**
   * @class ObjSum
   * @brief struct intended to keep mapping of clid to keys
   */
  struct ObjSum {
      int num;
      std::set<std::string> keys; /**< returns list of unique keys */
      ObjSum();
      ObjSum(int n);
  };
	  
  EventCount(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EventCount();
  
  StatusCode initialize(); /// Algorithm interface. Cannot re-initialize with this
  StatusCode execute();    /// Algorithm interface.
  StatusCode finalize();   /// Algorithm interface.

  void handle(const Incident& inc);
  
private:
  bool m_dump;                     /// PROP If dump is set to true then the 
				   ///  object summary will be printed 
  ServiceHandle<IClassIDSvc> m_pCLIDSvc;
  
  int m_nev;                        /// Event count for output
  int m_first;                      /// first event number in file
  int m_final;                      /// last event number in file
  int m_firstlb;                    /// first lumiblock number in file
  int m_finallb;                    /// last lumiblock number in file
  int m_currentRun;                 /// State marker
  std::string m_currentFile;        ///
  std::vector<int> m_runs;          /// list of unique runs in file
  std::map<std::string,int> m_streams; /// list of streams
  std::map<std::string,std::set<EventID>> m_fileEvents; 
  std::set<std::string> m_types;    /// list of unique event types in file
  std::map<int,int> m_clids;        /// map of clids to counts for that clid
  //std::map<int,int> m_clids_pr;     /// map of clids to counts for that clid for provenance records
  std::map<int,ObjSum> m_summaries; /// map of clids to key lists
  //std::set<std::string> m_prov_keys; /// map of clids to key lists for provenance records
  std::map<Guid,std::string> m_guids; /// map of clids to key lists for provenance records
};
#endif
