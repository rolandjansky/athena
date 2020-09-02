/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamOutputStreamCopyTool.cxx
 *  @brief This file contains the implementation for the ByteStreamOutputStreamCopyTool class.
 *  @author Hong Ma <hma@bnl.gov>
 **/

#include "ByteStreamOutputStreamCopyTool.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

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
ByteStreamOutputStreamCopyTool::~ByteStreamOutputStreamCopyTool() {
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endmsg;

   StatusCode status = ::AlgTool::initialize();
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Unable to initialize AlgTool base class." << endmsg;
      return(status);
   }

   // retrieve services 
   status = m_inputSvc.retrieve();
   if (status.isFailure()) {
      log << MSG::ERROR << "Unable to locate ByteStreamInputSvc " << endmsg;
      return(status);
   } else {
      log << MSG::DEBUG << "Found ByteStreamInputSvc." << endmsg;
   }

   status = m_outputSvc.retrieve();
   if (status.isFailure()) {
      log << MSG::ERROR << "Unable to locate ByteStreamOutputSvc " << endmsg;
      return(status);
   } else {
      log << MSG::DEBUG << "Found ByteStreamOutputSvc." << endmsg;
   }

   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::finalize() {
   StatusCode status = m_outputSvc.release();
   if (status.isFailure()) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Cannot release the ByteStreamOutputSvc" << endmsg;
   }
   return(::AlgTool::finalize());
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::connectServices(const std::string& /*dataStore*/,
	const std::string& /*cnvSvc*/,
	bool /*extendProvenenceRecord*/) {
   /// Do nothing for connectServices
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::connectOutput(const std::string& /*outputName*/) {
   /// Do nothing for connectOutput
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::commitOutput(bool/* doCommit*/) {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "In commitOutput" << endmsg;
   const RawEvent* re_c = m_inputSvc->currentEvent() ; 
   if(!re_c){
     log << MSG::ERROR << " failed to get the current event from ByteStreamInputSvc  " << endmsg; 
     return StatusCode::FAILURE ; 
   }
   RawEvent* re =  const_cast<RawEvent*>(re_c); 
   if( ! m_outputSvc->putEvent(re) ) {

     log << MSG::ERROR << " failed to write event to ByteStreamOutputSvc  " << endmsg; 
     return StatusCode::FAILURE ; 
   }
   log << MSG::DEBUG << " done in commitOutput  " << endmsg;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::finalizeOutput() {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::streamObjects(const TypeKeyPairs&  typeKeys, const std::string&
 /*outputName*/) {
   if (typeKeys.size() != 0){
     MsgStream log(msgSvc(), name());
     log << MSG::WARNING << " Streaming objects is not supported.  The whole input event is written out" << endmsg;
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::streamObjects(const DataObjectVec& dataObjects, const std::string&
 /*outputName*/) {
   if (dataObjects.size() != 0){
     MsgStream log(msgSvc(), name());
     log << MSG::WARNING << " Streaming objects is not supported.  The whole input event is written out" << endmsg;
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::fillObjectRefs(const DataObjectVec& dataObjects) {
   if (dataObjects.size() != 0){
     MsgStream log(msgSvc(), name());
     log << MSG::WARNING << " fillObjectRefs is not supported.  The whole input event is written out" << endmsg;
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamOutputStreamCopyTool::getInputItemList(SG::IFolder* /*m_p2BWrittenFromTool*/) {
   return(StatusCode::SUCCESS);
}
