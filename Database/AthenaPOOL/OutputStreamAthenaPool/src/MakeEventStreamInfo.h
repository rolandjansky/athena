/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <string>

class StoreGateSvc;
namespace xAODMaker {  class IEventInfoCnvTool;  }

/** @class MakeEventStreamInfo 
 *  @brief This class provides an algorithm to make the EventStreamInfo object and update it.
 **/
class MakeEventStreamInfo : public ::AthAlgTool, virtual public IAthenaOutputTool {
public:
   /// Standard AlgTool Constructor
   MakeEventStreamInfo(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~MakeEventStreamInfo();
   /// Required of all IAthenaOutputTools:
   /// Called by AthenaOutputStream::initialize() (via ToolSvc retrieve()).
   StatusCode initialize();
   /// Called at the end of AthenaOutputStream::initialize().
   StatusCode postInitialize();
   /// Called at the beginning of AthenaOutputStream::execute().
   StatusCode preExecute();
   /// Called at the end of AthenaOutputStream::execute().
   StatusCode postExecute();
   /// Called at the beginning of AthenaOutputStream::finalize().
   StatusCode preFinalize();
   /// Called at the end of AthenaOutputStream::finalize() (via release()).
   StatusCode finalize();

private:
   /// Key, the StoreGate key for the EventStreamInfo object.
   StringProperty m_key;

   /// Pointer to the data stores
   ServiceHandle<StoreGateSvc> m_metaDataStore;
   ServiceHandle<StoreGateSvc> m_eventStore;
};
#endif
