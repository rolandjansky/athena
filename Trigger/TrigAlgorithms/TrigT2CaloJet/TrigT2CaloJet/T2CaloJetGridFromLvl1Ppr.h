/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2CALOJETGRIDFROMLVL1PPR_H
#define TRIGT2CALOJET_T2CALOJETGRIDFROMLVL1PPR_H

/********************************************************************

NAME:     T2CaloJetGridFromLvl1Ppr.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE: Data preparation tool from Lvl1 Trigger Towers (accessed via Lvl1 PPr Bytestream)

********************************************************************/

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "GaudiKernel/AlgTool.h"

class TrigT2Jet;
class Trig3Momentum;

class T2CaloJetGridFromLvl1Ppr : public T2CaloJetBaseTool
{
 public:

  T2CaloJetGridFromLvl1Ppr(const std::string& type, const std::string& name, 
		    const IInterface* parent);
  
  ~T2CaloJetGridFromLvl1Ppr();
  
  virtual StatusCode initialize();
  using T2CaloJetBaseTool::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual StatusCode execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax);
  private:
    Trig3Momentum* m_gridElement;
  
};

#endif // TRIGT2CALOJET_T2CALOJETGRIDFROMLVL1PPR
