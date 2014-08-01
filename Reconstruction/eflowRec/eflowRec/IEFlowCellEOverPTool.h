/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEFLOWCELLEOVERPTOOL_H
#define IEFLOWCELLEOVERPTOOL_H

/********************************************************************

NAME:     eflowCellEOverPTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  14th Septemeber, 2006

Description: This class can take via python a set of EOverP values and cell subtraction parametyers needed for energy flow

********************************************************************/

#include "GaudiKernel/AlgTool.h"

class eflowBinnedParameters;

/** Must declare this, with name of interface*/
static const InterfaceID IID_IEFlowCellEOverPTool("IEFlowCellEOverPTool", 1, 0);

class IEFlowCellEOverPTool : public AlgTool {

 public:

  IEFlowCellEOverPTool(const std::string& type,const std::string& name,const IInterface* parent) : AlgTool(type,name,parent) {};
  
  virtual ~IEFlowCellEOverPTool() {};

  static const InterfaceID& interfaceID();

  virtual StatusCode intialize() {return StatusCode::SUCCESS;};
  virtual StatusCode execute(eflowBinnedParameters *binnedParameters) = 0 ;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;};

};

inline const InterfaceID& IEFlowCellEOverPTool::interfaceID()
{ 
  return IID_IEFlowCellEOverPTool; 
}


#endif
