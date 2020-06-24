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

Description: This class can take via python a set of EOverP values and cell subtraction parameters needed for energy flow

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"

class eflowEEtaBinnedParameters;

/** Must declare this, with name of interface*/
static const InterfaceID IID_IEFlowCellEOverPTool("IEFlowCellEOverPTool", 1, 0);

/**
Pure virtual base class, from which concrete classes inherit and define  reference e/p mean and widths, as well as reference energy density radial profile fit parameters. Inherits from AthAlgTool.
*/
class IEFlowCellEOverPTool : public AthAlgTool {

 public:

  IEFlowCellEOverPTool(const std::string& type,const std::string& name,const IInterface* parent) : AthAlgTool(type,name,parent) {};

  virtual ~IEFlowCellEOverPTool() {};

  static const InterfaceID& interfaceID();

  virtual StatusCode intialize() {return StatusCode::SUCCESS;};
  virtual StatusCode fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;};

protected:
  enum E_BINS        { ENERGY_BIN_START = 0, E001bin = ENERGY_BIN_START, E003point5bin = 1, E010bin = 2, E020bin = 3, E032point5bin = 4, E040bin = 5, ENERGY_BIN_END = 6 };

  enum ETA_BINS      { ETA_BIN_START = 0, eta050bin = ETA_BIN_START, eta100bin = 1, eta150bin = 2, eta250bin = 3, eta350bin = 4, eta450bin = 5, ETA_BIN_END = 6};

  enum SHAPE_PARAMS  { SHAPE_START = 0, NORM1 = SHAPE_START, WIDTH1 = 1, NORM2 = 2, WIDTH2 = 3, SHAPE_END = 4 };

};

inline const InterfaceID& IEFlowCellEOverPTool::interfaceID()
{
  return IID_IEFlowCellEOverPTool;
}


#endif
