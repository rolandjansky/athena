/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file OutputStreamSequencerSvc.cxx
 *  @brief This file contains the implementation for the OutputStreamSequencerSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "OutputStreamSequencerSvc.h"
#include "MetaDataSvc.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include <sstream>

//________________________________________________________________________________
OutputStreamSequencerSvc::OutputStreamSequencerSvc(const std::string& name, ISvcLocator* pSvcLocator) : ::AthService(name, pSvcLocator),
	m_metaDataSvc("MetaDataSvc", name),
	m_fileSequenceNumber(0)
{
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
   if( !incidentName().empty() ) {
      incsvc->addListener(this, incidentName(), 100);
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
void OutputStreamSequencerSvc::handle(const Incident& inc)
{
   // process NextEventRange 
   ATH_MSG_INFO("handle " << name() << " incident type " << inc.type());
   m_currentRangeID.clear();
   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   if (fileInc != nullptr) {
      m_currentRangeID = fileInc->fileName();
      ATH_MSG_DEBUG("Requested (through incident) next event range filename extension: " << m_currentRangeID);
   }
   if( m_currentRangeID.empty() ) {
      std::ostringstream n;
      n << "_" << std::setw(4) << std::setfill('0') << m_fileSequenceNumber;
      m_currentRangeID = n.str();
      ATH_MSG_DEBUG("Default next event range filename extension: " << m_currentRangeID);
   }
   m_fileSequenceNumber++;
   if( m_fileSequenceNumber > 1 && m_metaTransOnNextRange ) {
      // End of the previous event range in AthenaMP
      ATH_MSG_DEBUG("MetaData transition");
      if (!m_metaDataSvc->transitionMetaDataFile( ignoringInputBoundary() ).isSuccess()) {
         ATH_MSG_FATAL("Cannot transition MetaDataSvc.");
      }
   }
}

//__________________________________________________________________________
std::string OutputStreamSequencerSvc::buildSequenceFileName(const std::string& orgFileName) const
{
   if( incidentName().empty() ) {
      // output sequences not configures, just return the original filename
      return orgFileName;
   }
   // build the full output file name for this event range
   std::string fileNameCore = orgFileName, fileNameExt;
   std::size_t sepPos = orgFileName.find("[");
   if (sepPos != std::string::npos) {
      fileNameCore = orgFileName.substr(0, sepPos);
      fileNameExt = orgFileName.substr(sepPos);
   }
   std::ostringstream n;
   n << fileNameCore << "." << m_currentRangeID << fileNameExt;
   return n.str();
}
