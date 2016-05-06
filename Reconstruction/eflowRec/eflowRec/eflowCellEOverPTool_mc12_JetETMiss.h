/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLEOVERPTOOL_MC12_JETETMISS_H
#define EFLOWCELLEOVERPTOOL_MC12_JETETMISS_H

/********************************************************************

NAME:     eflowCellEOverPTool_mc12_JetETMiss.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  March 2014

Description: New EM e/p for DC14 - JetMET ONLY - this is because taus already tuned PanTau on eflowRec, and hence prefer no updates for DC14

********************************************************************/

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBaseParameters;

class eflowCellEOverPTool_mc12_JetETMiss : public IEFlowCellEOverPTool {

 public:

  eflowCellEOverPTool_mc12_JetETMiss(const std::string& type,const std::string& name,const IInterface* parent);
  
  ~eflowCellEOverPTool_mc12_JetETMiss() {};

  StatusCode initialize();
  StatusCode execute(eflowEEtaBinnedParameters *binnedParameters) ;
  StatusCode finalize() ;

 private:

  std::vector<std::vector<std::vector<std::vector<std::vector<double> > > > > m_theEnergyEtaFirstIntLayerShapeParams;
  std::vector<std::vector<double> > m_theLayerShapeParams;
  std::vector<std::vector<std::vector<double> > > m_theEnergyEtaRingThicknesses;
  std::vector<std::vector<double> > m_theRingThicknesses;
  std::vector<double> m_theEOverPMeans;
  std::vector<double> m_theEOverPStdDevs;
  std::vector<std::vector<std::vector<double> > >  m_theEnergyEtaFirstIntLayerEOverPMeans;
  std::vector<std::vector<std::vector<double> > >  m_theEnergyEtaFirstIntLayerEOverPStandardDeviations;
  std::vector<std::vector<double> > m_test2;

  //const int m_nEBins;
  //const int m_nEtaBins;
  //const int m_nFirstIntLayerBins;
  //const int m_nCaloRegionBins;

  enum E_BINS        { E001bin = 0, E003point5bin, E010bin, E020bin, E032point5bin, E040bin };
    
  enum ETA_BINS      { eta050bin = 0, eta100bin, eta150bin, eta250bin};

  enum SHAPE_PARAMS  { NORM1 = 0, WIDTH1, NORM2, WIDTH2 };

  std::vector<double>  m_eBinValues;
  std::vector<double> m_etaBinBounds;
  std::vector<std::string> m_eBinLabels;
  std::vector<std::string> m_etaBinLabels;

};
#endif
