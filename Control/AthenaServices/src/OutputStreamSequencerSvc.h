/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OUTPUTSTREAMSEQUENCERSVC_H
#define OUTPUTSTREAMSEQUENCERSVC_H

/** @file OutputStreamSequencerSvc.h
 *  @brief This file contains the class definition for the OutputStreamSequencerSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"  // no forward decl: typedef
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"

#include <memory>
#include <map>
#include <mutex>

// Forward declarations
class MetaDataSvc;

static const InterfaceID IID_OutputStreamSequencerSvc("OutputStreamSequencerSvc", 1, 0);

template <class TYPE> class SvcFactory;

/** @class OutputStreamSequencerSvc
 *  @brief This class provides configuration properties to enable OutputStream file sequences
 **/
class OutputStreamSequencerSvc : public ::AthService,
  virtual public IIncidentListener {

 public:
  typedef std::pair<std::string,std::string> RangeReport_t; // (RangeID,RangeFileName)
  typedef std::unique_ptr<RangeReport_t>     RangeReport_ptr;

public: // Constructor and Destructor
   /// Standard Service Constructor
   OutputStreamSequencerSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~OutputStreamSequencerSvc();

   /// Retrieve interface ID
   static const InterfaceID& interfaceID() { return IID_OutputStreamSequencerSvc; }

public: // Non-static members
   /// Required of all Gaudi services:
   virtual StatusCode initialize() override final;
   /// Required of all Gaudi services:
   virtual StatusCode finalize() override final;
   /// Required of all Gaudi services:  see Gaudi documentation for details
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override final;

   /// Incident service handle
   virtual void handle(const Incident& /*inc*/) override final;

   /// Returns sequenced file name for output stream
   std::string buildSequenceFileName(const std::string&);
   void publishRangeReport(const std::string& outputFile);
   RangeReport_ptr getRangeReport();

   /// The name of the incident that starts a new event sequence
   std::string  incidentName() const            { return m_incidentName.value(); }

   /// The current Event Range ID (only one range is 
   std::string  currentRangeID() const;

   /// Is the service in active use? (true after the first range incident is handled)
   bool         inUse() const;
  
   /// Are there concurrent events? (threads>1)
   bool         inConcurrentEventsMode() const;

private: // data
   ServiceHandle<MetaDataSvc> m_metaDataSvc;

   /// The event sequence number
   int m_fileSequenceNumber;

   /// Current EventRange ID constructed on the last NextRange incident
   std::string  m_currentRangeID;
   
   /// EventRange ID for all slots
   std::vector<std::string>   m_rangeIDinSlot;

private: // properties
   /// SequenceIncidentName, incident name for triggering file sequencing.
   StringProperty             m_incidentName;

   std::map<std::string,std::string> m_fnToRangeId;
   std::map<std::string,std::string>::iterator m_finishedRange;

   mutable std::mutex         m_mutex;
};

#endif
