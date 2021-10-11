/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamOutputStreamCopyTool.cxx
 *  @brief This file contains the implementation for the ByteStreamOutputStreamCopyTool class.
 *  @author Hong Ma <hma@bnl.gov>
 **/

#include "ByteStreamOutputStreamCopyTool.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ISvcLocator.h"

/// Constructor
ByteStreamOutputStreamCopyTool::ByteStreamOutputStreamCopyTool(
		  const std::string& type,
		  const std::string& name,
		  const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_outputSvc("ByteStreamEventStorageOutputSvc", name),
    m_inputSvc("ByteStreamEventStorageInputSvc", name)
{
   // Declare IAthenaOutputStreamTool interface
   declareInterface<IAthenaOutputStreamTool>(this);
   declareProperty("ByteStreamOutputSvc", m_outputSvc);
   declareProperty("ByteStreamInputSvc",  m_inputSvc);
}

//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::initialize() {

   ATH_MSG_INFO("Initializing");

   // retrieve services
   ATH_CHECK( m_inputSvc.retrieve() );
   ATH_CHECK( m_outputSvc.retrieve() );

   return StatusCode::SUCCESS;
}

//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::connectServices(const std::string& /*dataStore*/,
	const std::string& /*cnvSvc*/,
	bool /*extendProvenenceRecord*/) {
   /// Do nothing for connectServices
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::connectOutput(const std::string& /*outputName*/) {
   /// Do nothing for connectOutput
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::commitOutput(bool/* doCommit*/) {

   ATH_MSG_DEBUG("In commitOutput");
   const RawEvent* re = m_inputSvc->currentEvent() ; 
   if(!re){
     ATH_MSG_ERROR(" failed to get the current event from ByteStreamInputSvc");
     return StatusCode::FAILURE ; 
   }
   if( ! m_outputSvc->putEvent(re) ) {
     ATH_MSG_ERROR(" failed to write event to ByteStreamOutputSvc");
     return StatusCode::FAILURE ; 
   }
   ATH_MSG_DEBUG(" done in commitOutput");
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::finalizeOutput() {
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::streamObjects(const TypeKeyPairs&  typeKeys, const std::string&
 /*outputName*/) {
   if (typeKeys.size() != 0){
     ATH_MSG_WARNING("Streaming objects is not supported. The whole input event is written out");
   }
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::streamObjects(const DataObjectVec& dataObjects, const std::string&
 /*outputName*/) {
   if (dataObjects.size() != 0){
      ATH_MSG_WARNING("Streaming objects is not supported. The whole input event is written out");
   }
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::getInputItemList(SG::IFolder* /*m_p2BWrittenFromTool*/) {
   return StatusCode::SUCCESS;
}
