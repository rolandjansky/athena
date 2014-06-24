/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IRPCTRIGGERDBTOOL_H 
#define MUONCONDINTERFACE_IRPCTRIGGERDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"

 

class RpcIdHelper; 

static const InterfaceID IID_IRPCTriggerDbTool("IRPCTriggerDbTool", 1, 0);

class IRPCTriggerDbTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IRPCTriggerDbTool;};
   
   //getter methods
   virtual std::string EtaTableFolderName() const = 0; 
   virtual std::string PhiTableFolderName() const = 0; 

   virtual const std::vector<std::string> GetStringRPCEtaCM_File() const = 0;
   virtual const std::vector<std::string> GetStringRPCEtaTh0() const = 0;
   //   virtual const std::vector<std::string> GetStringRPCEtaTh1() const = 0;
   //   virtual const std::vector<std::string> GetStringRPCEtaTh2() const = 0;
   virtual const std::vector<std::string> GetStringRPCEtaSequence_Th() const = 0;
   virtual const std::vector<std::string> GetStringRPCEtaInfo() const = 0;
  
   virtual const std::vector<std::string> GetStringRPCPhiCM_File() const = 0;
   virtual const std::vector<std::string> GetStringRPCPhiTh0() const = 0;
   virtual const std::vector<std::string> GetStringRPCPhiInfo() const = 0;

   virtual const std::map<std::string, std::string>* GetRoads() const = 0;  

   //load methods
   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadRPCEtaTable(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadRPCPhiTable(IOVSVC_CALLBACK_ARGS) = 0;

};




#endif  
