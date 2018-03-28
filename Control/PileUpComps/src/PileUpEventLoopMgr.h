/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPEVENTLOOPMGR_H
#define PILEUPEVENTLOOPMGR_H
/** @file PileUpEventLoopMgr.h
    @brief The ATLAS event loop for pile-up applications.
    @author Paolo Calafiura
    $Id: PileUpEventLoopMgr.h,v 1.9 2008-04-23 22:48:29 calaf Exp $
*/

// Base class headers
#include "AthenaKernel/IEventSeek.h"
#include "GaudiKernel/MinimalEventLoopMgr.h"

// Athena headers
#include "AthenaKernel/MsgStreamMember.h"
#include "EventInfo/EventID.h"  /* for EventID::number_type */
#include "PileUpTools/PileUpStream.h"

// Gaudi headers
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#ifndef _CPP_STRING
#include <string>
#endif

// Forward declarations
class IBeamIntensity;
class IBeamLuminosity;
class IBkgStreamsCache;
namespace xAODMaker
{
  class IEventInfoCnvTool;
}
class IEvtSelector;
class IIncidentSvc;
class ITimeKeeper;
class PileUpMergeSvc;
class StoreGateSvc;

/** @class PileUpEventLoopMgr
    @brief The ATLAS event loop for pile-up applications.
*/

class PileUpEventLoopMgr : virtual public IEventSeek,
                           public MinimalEventLoopMgr   {
public:

  /// Standard Constructor
  PileUpEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~PileUpEventLoopMgr();

public:
  /// implementation of IAppMgrUI::initialize
  virtual StatusCode initialize();
  /// implementation of IAppMgrUI::finalize
  virtual StatusCode finalize();
  /// implementation of IAppMgreUI::terminate
  //  virtual StatusCode terminate();
  /// implementation of IAppMgrUI::nextEvent
  virtual StatusCode nextEvent(int maxevt);
  /// implementation of IEventProcessor::executeEvent(void* par)
  virtual StatusCode executeEvent(void* par);

  /// Seek to a given event
  virtual StatusCode seek(int evt);
  /// Return the current event count
  virtual int curEvent() const;

  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvInterface);

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }

private:
  /// setup input and overlay selectors and iters
  StatusCode setupStreams();

  /// Run the algorithms for the current event
  virtual StatusCode executeAlgorithms();

  /// Run the algorithms beginRun hook
  StatusCode beginRunAlgorithms();

  /// Run the algorithms endRun hook
  StatusCode endRunAlgorithms();

  ///return the 'fake BCID' corresponding to bunchXing
  inline unsigned int getBCID(int bunchXing, unsigned int centralBCID) const {
    //FIXME to be completely safe this should should probably depend on the bunch spacing too. Perhaps that concept should be deprecated though?
    return static_cast<unsigned int>((((bunchXing + static_cast<int>(centralBCID)) % static_cast<int>(m_maxBunchCrossingPerOrbit)) + static_cast<int>(m_maxBunchCrossingPerOrbit) )  % static_cast<int>(m_maxBunchCrossingPerOrbit));
  }

  /// Incident Service
  ServiceHandle<IIncidentSvc> m_incidentSvc;

  /// PileUp Merge Service
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;

  /// Input Streams
  PileUpStream m_origStream;
  PileUpStream m_signalStream;

  /// output store
  ServiceHandle<StoreGateSvc> m_evtStore;              // overlaid (output) event store

  //unsigned int m_nInputs;
  //unsigned int m_nStores;

  /// @name Properties
  //@{
  /// Original (Physics) Event selector (background for overlay).
  ServiceHandle<IEvtSelector> m_origSel;
  /// Signal Event selector (for overlay).
  ServiceHandle<IEvtSelector> m_signalSel;
  /// BkgStreamsCaches managing background events
  ToolHandleArray<IBkgStreamsCache> m_caches;
  /// (max) minBias interactions per Xing, for setting MC luminosity
  FloatProperty m_maxCollPerXing;

  /// Xing frequency(ns);
  FloatProperty m_xingFreq;
  /// first xing to be simulated (0th xing is 1st after trigger)
  IntegerProperty m_firstXing;
  /// last xing to be simulated (0th xing is 1st after trigger)
  IntegerProperty m_lastXing;

  /// property: TimeKeeper service instance
  ServiceHandle<ITimeKeeper> m_timeKeeper;

  /// property: allow sub evts EOF condition when maxevt==-1
  BooleanProperty m_allowSubEvtsEOF;

  /// property: process bkg events xing by xing without caching them
  BooleanProperty m_xingByXing;

  /// property: is this job running RDO+RDO overlay.
  BooleanProperty m_isEventOverlayJob;

  /// property: is this job running MC RDO+RDO overlay.
  BooleanProperty m_isEventOverlayJobMC;

  /// property: control behaviour of event loop on algorithm failure
  IntegerProperty m_failureMode;

  /// property: beam intensity service handle for beam profile in local time
  ServiceHandle<IBeamIntensity> m_beamInt;
  /// property: beam intensity service handle for luminosity profile in iovtime
  ServiceHandle<IBeamLuminosity> m_beamLumi;
  /// property: Handle to the EventInfo -> xAOD::EventInfo converter tool
  ToolHandle< xAODMaker::IEventInfoCnvTool > m_xAODCnvTool;
  //@}

  /// current run number
  EventID::number_type m_currentRun;
  bool m_firstRun;

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  /// max bunch crossings per orbit
  unsigned int m_maxBunchCrossingPerOrbit;

  int m_nevt;

  int m_ncurevt;
  bool m_skipExecAlgs;
  bool m_loadProxies;

  /// property: flag to control extra checks for embedding jobs.
  BooleanProperty m_isEmbedding;
  /// property: Default true. When set to false, this will allow the
  /// code to reproduce serial output in an AthenaMP job, albeit with
  /// a significant performance penalty.
  BooleanProperty m_allowSerialAndMPToDiffer;
};
#endif // PILEUPTOOLS_PILEUPEVENTLOOPMGR_H
