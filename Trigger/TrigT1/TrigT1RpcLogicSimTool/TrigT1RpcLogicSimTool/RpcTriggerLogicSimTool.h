/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCTRIGGERLOGICSIMTOOL_H
#define RPCTRIGGERLOGICSIMTOOL_H 

#include "TrigT1RpcLogicSimTool/IRpcTriggerLogicSimTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>

class InefficientTrigger;

class RpcTriggerLogicSimTool :  virtual public IRpcTriggerLogicSimTool, 
				public AlgTool   {
 public:
  RpcTriggerLogicSimTool(const std::string&,const std::string&,
			 const IInterface*);
  
  virtual ~RpcTriggerLogicSimTool ();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
        
  virtual bool doTrigger(std::vector<unsigned int>, int);

 private:
  MsgStream          *m_log;
  InefficientTrigger *m_trigger;

};

#endif // RPCTRIGGERLOGICSIMTOOL_H 
