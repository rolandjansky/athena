/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     ITrigTauClusterTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************

#ifndef TRIGT2CALO_ITRIGTAUCLUSTERTOOL_H 
#define TRIGT2CALO_ITRIGTAUCLUSTERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

class TrigTauCluster;

static const InterfaceID IID_ITrigTauClusterTool("ITrigTauClusterTool",1,0);

class ITrigTauClusterTool: public virtual IAlgTool, 
			   public virtual IProperty {
public:
/** Destructor */
  virtual ~ITrigTauClusterTool() { }
/** TrigTauClusterTool execute */
  virtual bool execute(const TrigTauCluster &t2Cluster) = 0;
/** get cut counter */
  virtual int getCutCounter() = 0;
  
  static const InterfaceID& interfaceID(){ return IID_ITrigTauClusterTool;} ;
private:
};

#endif
