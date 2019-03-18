/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2L1CALOJETFULLSCANBASETOOL_H
#define TRIGT2CALOJET_T2L1CALOJETFULLSCANBASETOOL_H

/********************************************************************

NAME:     T2L1CaloJetFullScanBaseTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett
CREATED:  July, 2011

PURPOSE:  Base Tool for all T2L1CaloJet tools that require
          a vector of T2CaloJet objects as its input.  The derived
          tool must implement an execute(vector<T2CaloJet*>) method.


********************************************************************/

class TrigT2Jet;


#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

static const InterfaceID IID_T2L1CaloJetFullScanBaseTool("T2L1CaloJetFullScanBaseTool", 1, 0);

class T2L1CaloJetFullScanBaseTool : public IAlgToolCalo
{
 public:

  T2L1CaloJetFullScanBaseTool(const std::string& type, const std::string& name,
		    const IInterface* parent);

  ~T2L1CaloJetFullScanBaseTool();

  virtual StatusCode initialize();
  using IAlgToolCalo::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual StatusCode execute(DataVector<TrigT2Jet>*, int&, float&, float&, float&, float&);

  virtual TrigTimer* getTimer(const std::string& /*name*/){ return 0;};

  static const InterfaceID& interfaceID() { return  IID_T2L1CaloJetFullScanBaseTool; }


};

#endif // TRIGT2CALOJET_T2L1CaloJetFullScanBaseTool
