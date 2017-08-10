/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFISUBTRACTIONALGTOOL_H
#define PFISUBTRACTIONALGTOOL_H

#include "GaudiKernel/IAlgTool.h"

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;

class PFISubtractionTool : virtual public IAlgTool {

 public:

  virtual StatusCode intialize() {return StatusCode::SUCCESS;}
  virtual void execute(eflowCaloObjectContainer*, eflowRecTrackContainer*, eflowRecClusterContainer*) = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;}

};

#endif
