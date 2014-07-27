// emacs: this is -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     IAlgToolCalo.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
// 
// AUTHOR:   M.P. Casado
// 
// - Add T2Calo to execute function to allow access to T2Calo member 
//   variables for job option controlled region limits   - R. Soluk
// ********************************************************************

#ifndef TRIGT2CALOCOMMON_IALGTOOLCALOSAMP1EN_H 
#define TRIGT2CALOCOMMON_IALGTOOLCALOSAMP1EN_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

class T2CaloSamp1En;

class IAlgToolCaloSamp1En: public virtual IAlgTool, 
			   public virtual IProperty {
public:
  virtual ~IAlgToolCaloSamp1En() { }

private:  
};

#endif
