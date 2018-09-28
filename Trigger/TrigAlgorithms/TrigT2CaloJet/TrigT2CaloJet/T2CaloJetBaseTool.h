/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2CALOJETBASETOOL_H
#define TRIGT2CALOJET_T2CALOJETBASETOOL_H

/********************************************************************

NAME:     T2CaloJetBaseTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE:  Base Tool for all T2CaloJet tools require
          an T2CaloJet object as its input.  The derived
          tool must implement an execute(TrigT2Jet*) method.

********************************************************************/

class TrigT2Jet;


#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "IRegionSelector/IRoiDescriptor.h" 

static const InterfaceID IID_T2CaloJetBaseTool("T2CaloJetBaseTool", 1, 0);

class T2CaloJetBaseTool : public IAlgToolCalo
{
 public:

  T2CaloJetBaseTool(const std::string& type, const std::string& name,
		    const IInterface* parent);

  ~T2CaloJetBaseTool();

  virtual StatusCode initialize();
  using IAlgToolCalo::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  //  virtual StatusCode execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax);

  virtual StatusCode execute(TrigT2Jet* jet, const IRoiDescriptor& roi,
				const CaloDetDescrElement*& /*caloDDE*/);

  virtual TrigTimer* getTimer(const std::string& /*name*/){ return 0;};

  static const InterfaceID& interfaceID() { return  IID_T2CaloJetBaseTool; }


};

#endif // TRIGT2CALOJET_T2CALOJETBASETOOL
