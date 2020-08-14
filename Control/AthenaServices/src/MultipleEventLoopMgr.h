/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_MULTIPLEEVENTLOOPMGR_H
#define ATHENASERVICES_MULTIPLEEVENTLOOPMGR_H
/** @file MultipleEventLoopMgr.h
    @brief A specialize AthenaEventLoopMgr that allows multiple passes over an 
    event collection.
    @author Paolo Calafiura - ATLAS Collaboration
    $Id: MultipleEventLoopMgr.h,v 1.3 2007-06-16 00:55:22 calaf Exp $
*/

#include <string>

#ifndef GAUDIKERNEL_PROPERTY_H
# include "Gaudi/Property.h"
#endif
#ifndef GAUDIKERNEL_SERVICEHANDLE_H
# include "GaudiKernel/ServiceHandle.h"
#endif
#ifndef GAUDIKERNEL_STATUSCODE_H
# include "GaudiKernel/StatusCode.h"
#endif
#ifndef ATHENASERVICES_ATHENAEVENTLOOPMGR_H
# include "AthenaEventLoopMgr.h"
#endif

// Forward declarations
class IAlgManager;
class INextPassFilter;
class IToolSvc;
class ISvcLocator;
template <class TYPE> class SvcFactory;

/** @class MultipleEventLoopMgr
    @brief A specialized AthenaEventLoopMgr that allows multiple passes over 
    an event collection.

    @details  The number of passes is controlled by a tool
    called at the end of each pass over the events that
    implements the INextPassFilter interface (specified using the 
    NextPassFilter property). At the end of each pass the event selector 
    is rewound and the services listed in the ToBeReinitialized property are 
    reinitialized. Typically MultipleEventLoopMgr will be used for
    calibration jobs, with the INextPassFilter object looking at the status 
    of the constants in the DetectorStore 
*/
class MultipleEventLoopMgr : public AthenaEventLoopMgr   {
public:
  /// Your creator is your friend
  friend class SvcFactory<MultipleEventLoopMgr>;

  /// Standard Constructor
  MultipleEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
 
  /// Standard Destructor
  virtual ~MultipleEventLoopMgr();

  /// Dump out histograms as needed
  //INH  virtual StatusCode writeHistograms(bool force=false);

  /// Run the algorithms for the current event
  //INH  virtual StatusCode executeAlgorithms();

public:
  /// implementation of IAppMgrUI::initalize
  //INH  virtual StatusCode initialize();
  /// implementation of IAppMgrUI::finalize
  //INH  virtual StatusCode finalize();
  /// implementation of IAppMgrUI::nextEvent
  virtual StatusCode nextEvent(int maxevt);
  /// implementation of IEventProcessor::executeEvent(void* par)
  //INH  virtual StatusCode executeEvent(void* par);
  // Seek to a given event.
  //INH  virtual StatusCode seek(int evt);
  // Return the current event count.
  //INH  virtual int curEvent();

private:
  MultipleEventLoopMgr(); ///< no implementation
  MultipleEventLoopMgr(const MultipleEventLoopMgr&); ///< no implementation

  /// @name Properties
  //@{
  /// the name of the INextPassFilter object queried at end of pass
  StringProperty m_nextPassFilterName;

  /// a list of services to be reinit at the end of the pass
  StringArrayProperty m_toBeReInitializedNames;
  //@}

  /// Locate filter. Parses m_nextPassFilterName allowing 
  ///for interactive changes.
  INextPassFilter* nextPassFilter();

  /// called at end of pass. Calls nextPassFilter
  bool doNextPass(); ///< if true start another pass

  ///called at each end of pass. Parses m_toBeReInitializedNames allowing 
  ///for interactive changes
  StatusCode reInitList();  

  /// handle to the ToolSvc
  ServiceHandle<IToolSvc> m_pToolSvc;

  IAlgManager* algMgr();
  IAlgManager* m_pAlgMgr;

  unsigned int m_passDone; ///< number of passes already completed
};

#endif
