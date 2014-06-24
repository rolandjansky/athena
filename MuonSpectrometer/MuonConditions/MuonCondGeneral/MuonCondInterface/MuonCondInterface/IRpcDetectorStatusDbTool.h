/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IRPCDETECTORSTATUSDBTOOL_H 
#define MUONCONDINTERFACE_IRPCDETECTORSTATUSDBTOOL_H 
 
 // Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"
//#include "Identifier/Identifier.h"
 
 
static const InterfaceID IID_IRpcDetectorStatusDbTool("IRpcDetectorStatusDbTool", 1, 0);
 
class IRpcDetectorStatusDbTool: virtual public IAlgTool, 
                        virtual public IAddressProvider {
   
 public:
 

  virtual std::string FolderName() const = 0; 
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode loadRpcDqStatus(IOVSVC_CALLBACK_ARGS) = 0;
  static const InterfaceID& interfaceID() {return IID_IRpcDetectorStatusDbTool;};
  
 
  virtual const std::vector<Identifier>&  EffPanelId()=0 ;
  virtual const std::vector<Identifier>&  EffStripId()=0 ;
  
  virtual const std::map<Identifier,     double>& RPC_EfficiencyMap()=0              ; 
  virtual const std::map<Identifier,     double>& RPC_EfficiencyGapMap()=0           ; 
  virtual const std::map<Identifier,     double>& RPC_MeanClusterSizeMap()=0         ; 
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize1Map()=0        ; 
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize2Map()=0        ; 
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize3Map()=0        ; 
  virtual const std::map<Identifier,std::string>& RPC_DeadStripListMap()=0           ; 
  virtual const std::map<Identifier,      float>& RPC_FracDeadStripMap()=0           ; 
  virtual const std::map<Identifier,        int>& RPC_ProjectedTracksMap()=0         ;  
  virtual const std::map<Identifier,        int>& RPC_DeadStripList()=0              ; 
  virtual const std::map<Identifier,     std::vector<double> >& RPC_TimeMapforStrip()=0      ;	
};

 
#endif  
