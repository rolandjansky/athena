/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_ITILECALIBTOOL_H
#define TILECALIBALG_ITILECALIBTOOL_H

// GAUDI includes 

#include "GaudiKernel/IAlgTool.h"

class TFile;

// Declare the interfaceID

static const InterfaceID IID_ITileCalibTool("ITileCalibTool",1,0);

class ITileCalibTool: virtual public IAlgTool
{
 public:
  static const InterfaceID& interfaceID() { return IID_ITileCalibTool; }
  virtual ~ITileCalibTool() {};

  virtual StatusCode initialize() = 0;
  virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile) = 0;
  virtual StatusCode execute() = 0;
  virtual StatusCode finalizeCalculations() = 0;
  virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile) = 0;
  virtual StatusCode finalize() = 0;
};

#endif // #ifndef TILECALIBALG_ITILECALIBTOOL_H
