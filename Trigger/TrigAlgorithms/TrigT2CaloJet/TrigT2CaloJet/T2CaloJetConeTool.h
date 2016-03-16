/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2CALOJETCONETOOL_H
#define TRIGT2CALOJET_T2CALOJETCONETOOL_H

/********************************************************************

NAME:     T2CaloJetConeTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE: Cone Tool for all T2CaloJet.  Does simple cone alg after data
         preparation.

********************************************************************/

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "GaudiKernel/AlgTool.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class TrigT2Jet;

class T2CaloJetConeTool : public T2CaloJetBaseTool
{
 public:

  T2CaloJetConeTool(const std::string& type, const std::string& name, 
		    const IInterface* parent);
  
  ~T2CaloJetConeTool();
  
  virtual StatusCode initialize();
  using T2CaloJetBaseTool::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  //virtual StatusCode execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax);
  virtual StatusCode execute(TrigT2Jet* jet, const IRoiDescriptor& roi);

  void coneIteration(TrigT2Jet* jet, int coneIter);
  
 private:
  int m_nIterations;  
  double m_coneRadius;
  //int niterations;
  //double etot_jet;
  //Cell check
  int m_inCone, m_outCone, m_totalCell;
  // cleaning:
  int m_cellQualityThresholdLAr;
  float m_leadingCellFraction;

  std::vector<double> m_datav;
  std::vector<double> m_range1;
  std::vector<double> m_range2;
  std::vector<int> m_ndiv;
  std::vector<std::string> m_dataObject;
  std::vector<std::string> m_dataaxis;

  //New timers
  ITrigTimerSvc *m_pTimerService;   // pointer to Timer Service
  TrigTimer *m_fcal_timer;
  TrigTimer *m_other_timer;

  //FCal cone sizes
  std::vector<double> m_fj_cone_sizes;
  
  // cleaning:
  std::vector<double> m_cellenergies;

};

#endif // TRIGT2CALOJET_T2CALOJETCONETOOL
