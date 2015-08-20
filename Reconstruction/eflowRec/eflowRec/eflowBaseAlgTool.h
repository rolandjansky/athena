/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWBASEALGTOOL_H
#define EFLOWBASEALGTOOL_H

/********************************************************************

NAME:     eflowBaseAlgTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  24th January, 2005

********************************************************************/

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;
#include "GaudiKernel/IAlgTool.h"

class eflowBaseAlgTool : virtual public IAlgTool {

 public:

  virtual StatusCode intialize() {return StatusCode::SUCCESS;}
  virtual void execute(eflowCaloObjectContainer*) = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;}

};

#endif
