/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//vitrual interface for all the Muon RPC technologies 

#ifndef IRPCConditionsSvc_h
#define IRPCConditionsSvc_h

#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <list>
#include <string>
#include <vector>
#include <map>

class Identifier;
class StatusCode;

class IRPCConditionsSvc: virtual public IInterface{
public:
  virtual ~IRPCConditionsSvc(){}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
 
  ///Return whether this service can report on the hierarchy level 
  //   virtual bool canReportAbout(MuonConditions::Hierarchy h) = 0;

  //@todo introduce hash identifier method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS)=0;

  virtual bool isGoodPanel(const Identifier & Id) const = 0;
  virtual bool isGoodStrip(const Identifier & Id) const = 0;
  
  virtual const std::vector<Identifier>& deadPanelId() const = 0;
  virtual const std::vector<Identifier>& offPanelId() const = 0;
  
  
  virtual const std::vector<Identifier>&  EffPanelId() const =0 ;
  virtual const std::vector<Identifier>&  EffStripId() const =0 ;

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
  virtual const std::map<Identifier,std::vector<double> >& RPC_TimeMapforStrip()=0   ;
  
};

inline const InterfaceID & IRPCConditionsSvc::interfaceID(){
  static const InterfaceID IID_IRPCConditionsSvc("RPCConditionsSvc",1,0);
  return IID_IRPCConditionsSvc;
}

#endif
