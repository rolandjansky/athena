/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTAUBASEALGTOOL_H
#define EFLOWTAUBASEALGTOOL_H

/********************************************************************

NAME:     eflowTauBaseAlgTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  May 2013

********************************************************************/

class eflowTauObject;

#include "GaudiKernel/IAlgTool.h"

class eflowTauBaseAlgTool : virtual public IAlgTool {

 public:

  virtual StatusCode intialize() {return StatusCode::SUCCESS;};
  virtual StatusCode execute(const eflowTauObject& eflowTauObject) = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;};

};

#endif
