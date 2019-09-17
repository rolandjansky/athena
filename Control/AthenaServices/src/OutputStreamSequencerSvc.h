/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OUTPUTSTREAMSEQUENCERSVC_H
#define OUTPUTSTREAMSEQUENCERSVC_H

/** @file OutputStreamSequencerSvc.h
 *  @brief This file contains the class definition for the OutputStreamSequencerSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"  // no forward decl: typedef
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"

// Forward declarations
class MetaDataSvc;

static const InterfaceID IID_OutputStreamSequencerSvc("OutputStreamSequencerSvc", 1, 0);

template <class TYPE> class SvcFactory;

/** @class OutputStreamSequencerSvc
 *  @brief This class provides configuration properties to enable OutputStream file sequences
 **/
class OutputStreamSequencerSvc : public ::AthService,
	virtual public IIncidentListener {

public: // Constructor and Destructor
   /// Standard Service Constructor
   OutputStreamSequencerSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~OutputStreamSequencerSvc();

   /// Retrieve interface ID
   static const InterfaceID& interfaceID() { return IID_OutputStreamSequencerSvc; }

public: // Non-static members
   /// Required of all Gaudi services:
   StatusCode initialize();
   /// Required of all Gaudi services:
   StatusCode finalize();
   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Incident service handle
   void handle(const Incident& /*inc*/);

   /// Returns sequenced file name for output stream
   std::string buildSequenceFileName(const std::string&) const;

   /// The name of the incident that starts a new event sequence
   std::string  incidentName() const            { return m_incidentName.value(); }
  
   bool         ignoringInputBoundary() const   { return m_ignoreInputFile.value(); }

   /// Is the service in active use? (true after the first range incident is handled)
   bool         inUse() const;
  
   /// Are there concurrent events? (threads>1)
   bool         inConcurrentEventsMode() const;

private: // data
   ServiceHandle<MetaDataSvc> m_metaDataSvc;

   /// The event sequence number
   int m_fileSequenceNumber;

   /// Current EventRange ID constructed on the NextRange incident
   std::string  m_currentRangeID;

private: // properties
   /// SequenceIncidentName, incident name for triggering file sequencing.
   StringProperty             m_incidentName;
   /// IgnoreInputFileBoundary, boolean whether to ignore the input file boundary requirement for file sequencing.
   BooleanProperty            m_ignoreInputFile;
};

#endif
