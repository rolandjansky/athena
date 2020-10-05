/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAKEEVENTSTREAMINFO_H
#define MAKEEVENTSTREAMINFO_H
/** @file MakeEventStreamInfo.h
 *  @brief This file contains the class definition for the MakeEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: MakeEventStreamInfo.h,v 1.2 2008-05-27 21:22:07 gemmeren Exp $
 **/

#include "AthenaKernel/IAthenaOutputTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IMetaDataSvc.h"

#include <string>

class StoreGateSvc;
namespace xAODMaker {  class IEventInfoCnvTool;  }

/** @class MakeEventStreamInfo 
 *  @brief This class provides an algorithm to make the EventStreamInfo object and update it.
 **/
class MakeEventStreamInfo : public extends<::AthAlgTool, IAthenaOutputTool> {
public:
   /// Standard AlgTool Constructor
   MakeEventStreamInfo(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~MakeEventStreamInfo();
   /// Required of all IAthenaOutputTools:
   /// Called by AthenaOutputStream::initialize() (via ToolSvc retrieve()).
   virtual StatusCode initialize() override;
   /// Called at the end of AthenaOutputStream::initialize().
   virtual StatusCode postInitialize() override;
   /// Called at the beginning of AthenaOutputStream::execute().
   virtual StatusCode preExecute() override;
   /// Called before actually streaming objects.
   virtual StatusCode preStream() override;
   /// Called at the end of AthenaOutputStream::execute().
   virtual StatusCode postExecute() override;
   /// Called at the beginning of AthenaOutputStream::finalize().
   virtual StatusCode preFinalize() override;
   /// Called at the end of AthenaOutputStream::finalize() (via release()).
   virtual StatusCode finalize() override;

private:
   /// Key, the StoreGate key for the EventStreamInfo object.
   StringProperty m_key;

   /// Key, the StoreGate key for the xAOD::EventInfo object.
   StringProperty m_eventInfoKey;
   /// Key, the StoreGate key for the old EventInfo object, ix there is no xAOD::EventInfo.
   StringProperty m_oEventInfoKey;

   /// Pointer to the data stores
   ServiceHandle<IMetaDataSvc>  m_metaDataSvc;
   ServiceHandle<StoreGateSvc>  m_eventStore;
};
#endif
