/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
 *  @brief This class provides the interface to the LCG POOL persistency software.
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

private: // data
   ServiceHandle<MetaDataSvc> m_metaDataSvc;
   unsigned int m_fileSequenceNumber;
   std::string  m_fileSequenceLabel;

private: // properties
   /// SequenceIncidentName, incident name for triggering file sequencing.
   StringProperty             m_incidentName;
   /// IgnoreInputFileBoundary, boolean whether to ignore the input file boundary requirement for file sequencing.
   BooleanProperty            m_ignoreInputFile;
};

#endif
