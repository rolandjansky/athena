/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLEOVERPTOOL_H
#define EFLOWCELLEOVERPTOOL_H

/********************************************************************

NAME:     eflowCellEOverPTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  14th Septemeber, 2006

Description: This class can take via python a set of EOverP values and cell subtraction parametyers needed for energy flow

********************************************************************/

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBinnedParameters;

class eflowCellEOverPTool : public IEFlowCellEOverPTool {

 public:

  eflowCellEOverPTool(const std::string& type,const std::string& name,const IInterface* parent);
  
  ~eflowCellEOverPTool() {};

  StatusCode initialize();
  StatusCode execute(eflowBinnedParameters *binnedParameters) ;
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

  const int m_nEBins;
  const int m_nEtaBins;
  const int m_nFirstIntLayerBins;
  const int m_nCaloRegionBins;

  enum E_BINS        { E006bin = 0, E010bin, E020bin, E050bin };

  enum ETA_BINS      { eta010bin = 0, eta020bin, eta030bin, eta040bin, eta050bin, eta060bin, eta070bin,
			 eta080bin, eta090bin, eta100bin, eta110bin, eta120bin, eta130bin, eta140bin,
			 eta150bin, eta160bin, eta170bin, eta180bin, eta190bin, eta200bin, eta210bin,
		       eta220bin, eta230bin, eta240bin, eta250bin };

  enum SHAPE_PARAMS  { NORM1 = 0, WIDTH1, NORM2, WIDTH2 };

  std::vector<double>  m_eBinValues;
  std::vector<double> m_etaBinBounds;
  std::vector<std::string> m_eBinLabels;
  std::vector<std::string> m_etaBinLabels;

};
#endif
