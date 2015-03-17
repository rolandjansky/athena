/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHIHypo_CreateHIUEEstimate_h
#define TrigHIHypo_CreateHIUEEstimate_h




#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "HIGlobal/IHIEventShapeFiller.h"


class CreateHIUEEstimate : public HLT::AllTEAlgo {
public:
  CreateHIUEEstimate(const std::string& name, ISvcLocator* pSvcLocator);
  virtual  HLT::ErrorCode hltInitialize();
  virtual  HLT::ErrorCode hltFinalize() { return HLT::OK; }
  virtual HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
			    unsigned int type_out);
  virtual HLT::ErrorCode hltEndEvent(); 
  
private:
  std::string m_HIEventShapeContainerKey;
  std::string m_CaloCellContainerKey;
  ToolHandle<IHIEventShapeFiller> m_fillerTool;
  bool m_hasRun;
};


#endif // TrigHIHypo_CreateHIUEEstimate_h
