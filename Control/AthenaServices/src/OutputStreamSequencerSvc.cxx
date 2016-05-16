/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file OutputStreamSequencerSvc.cxx
 *  @brief This file contains the implementation for the OutputStreamSequencerSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: OutputStreamSequencerSvc.cxx,v 1.46 2008-11-19 23:21:10 gemmeren Exp $
 **/

#include "OutputStreamSequencerSvc.h"
#include "MetaDataSvc.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include <sstream>

//________________________________________________________________________________
OutputStreamSequencerSvc::OutputStreamSequencerSvc(const std::string& name, ISvcLocator* pSvcLocator) : ::AthService(name, pSvcLocator),
	m_metaDataSvc("MetaDataSvc", name),
	m_fileSequenceNumber(0),
	m_fileSequenceLabel() {
   // declare properties
   declareProperty("SequenceIncidentName", m_incidentName = "");
   declareProperty("IgnoreInputFileBoundary", m_ignoreInputFile = false);
}
//__________________________________________________________________________
OutputStreamSequencerSvc::~OutputStreamSequencerSvc() {
}
//__________________________________________________________________________
StatusCode OutputStreamSequencerSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve MetaDataSvc
   if (!m_metaDataSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get MetaDataSvc.");
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incsvc("IncidentSvc", this->name());
   if (!incsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   if (!m_incidentName.value().empty()) {
      incsvc->addListener(this, m_incidentName.value(), 100);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode OutputStreamSequencerSvc::finalize() {
   // Release MetaDataSvc 
   if (!m_metaDataSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release MetaDataSvc.");
   }
   return(::AthService::finalize());
}
//_______________________________________________________________________
StatusCode OutputStreamSequencerSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (riid == this->interfaceID()) {
      *ppvInterface = this;
   } else {
      // Interface is not directly available: try out a base class
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
void OutputStreamSequencerSvc::handle(const Incident& inc) {
   ATH_MSG_INFO("handle " << name() << " incident type " << inc.type());
   if (m_fileSequenceNumber > 0 || !m_fileSequenceLabel.empty()) { // Do nothing for first call
      if (!m_metaDataSvc->transitionMetaDataFile(m_ignoreInputFile.value()).isSuccess()) {
         ATH_MSG_FATAL("Cannot transition MetaDataSvc.");
      }
   }
   m_fileSequenceNumber++;
   m_fileSequenceLabel.clear();
   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   if (fileInc != 0) {
      m_fileSequenceLabel = fileInc->fileName();
   }
}
//__________________________________________________________________________
std::string OutputStreamSequencerSvc::buildSequenceFileName(const std::string& orgFileName) const {
   if (!m_incidentName.value().empty()) {
      std::string fileNameCore = orgFileName, fileNameExt;
      std::size_t sepPos = orgFileName.find("[");
      if (sepPos != std::string::npos) {
         fileNameCore = orgFileName.substr(0, sepPos);
         fileNameExt = orgFileName.substr(sepPos);
      }
      std::ostringstream n;
      if (m_fileSequenceLabel.empty()) {
         n << fileNameCore << "._" << std::setw(4) << std::setfill('0') << m_fileSequenceNumber << fileNameExt;
      } else {
         n << fileNameCore << "." << m_fileSequenceLabel << fileNameExt;
      }
      return(n.str());
   }
   return(orgFileName);
}
