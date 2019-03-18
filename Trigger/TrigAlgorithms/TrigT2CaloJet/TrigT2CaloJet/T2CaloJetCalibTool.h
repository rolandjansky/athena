/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2CALOJETCALIBTOOL_H
#define TRIGT2CALOJET_T2CALOJETCALIBTOOL_H

/********************************************************************

NAME:     T2CaloJetCalibTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Patricia Conde Muino <Patricia.Conde.Muino@cern.ch>
          Nuno Anjos <Nuno.Anjos@cern.ch>
CREATED:  May 2006

PURPOSE: Calibration tool for LVL2 jets. 
         It uses T2JESCalibTool / T2GSCalibTool / T2SamplingCalibTool 
         from TrigTools/TrigT2CaloCalibration.

********************************************************************/

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT2CaloCalibration/IT2HadCalibTool.h"
//#include "TrigCaloEvent/TrigCaloCluster.h"

class TrigT2Jet;

class T2CaloJetCalibTool : public T2CaloJetBaseTool
{
 public:

  T2CaloJetCalibTool(const std::string& type, const std::string& name, 
		    const IInterface* parent);
  
  ~T2CaloJetCalibTool();
  
  virtual StatusCode initialize();
  using T2CaloJetBaseTool::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual StatusCode execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax);

 private:
  
  MsgStream * m_log;
  int m_outputLevel;

  ToolHandle < IT2HadCalibTool >  m_t2JetCalibTool;
  ToolHandle < IT2HadCalibTool >  m_t2JetCorrToolTile1;
  ToolHandle < IT2HadCalibTool >  m_t2JetCorrToolFcal1;
  ToolHandle < IT2HadCalibTool >  m_t2JetCorrToolEm3Bar;
  ToolHandle < IT2HadCalibTool >  m_t2JetCorrToolHec0;
  ToolHandle < IT2HadCalibTool >  m_t2JetCorrToolEm3Fwd;
  ToolHandle < IT2HadCalibTool >  m_t2JetCorrToolPsBar;

  bool m_useFEBJets;
//bool m_fillLayerInfo;
  bool m_doJetCalib;
  bool m_doJetCorrTile1;
  bool m_doJetCorrFcal1;
  bool m_doJetCorrEm3Bar;
  bool m_doJetCorrHec0;
  bool m_doJetCorrEm3Fwd;
  bool m_doJetCorrPsBar;

//TrigCaloCluster *m_layer_info;

// StatusCode calculateHadEMEnergy(TrigT2Jet* Jet, double *HadEnergy, double *EMEnergy);
 StatusCode calculateLayerFracs(TrigT2Jet* Jet, double *fTile1, double *fFcal1, double *fEm3, double *fHec0, double *fPs);

};

#endif // TRIGT2CALOJET_T2CALOJETCALIBTOOL
