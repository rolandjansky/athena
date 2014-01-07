/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGS_NOISEMAPBUILDER_H
#define PIXELCONDITIONSALGS_NOISEMAPBUILDER_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include<string>

class IInDetConditionsSvc;
class IPixelByteStreamErrorsSvc;
class ITHistSvc;
class PixelID;
class TH2D;
class TH2C;
class TH1D;


/**
 *
 * NoiseMapBuilder.h
 *
 * Creates hit maps and maps of noisy pixels for every module 
 * of the pixel detector. The maps are stored in a root file. 
 * They can be written to the conditions database using the 
 * algorithm NoiseMapDBWriter.
 *
 * ruwiedel@physik.uni-bonn.de
 *
 */


class NoiseMapBuilder: public AthAlgorithm{

 public:
  NoiseMapBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  ~NoiseMapBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  ServiceHandle< ITHistSvc > m_tHistSvc;
  ServiceHandle< IInDetConditionsSvc > m_pixelConditionsSummarySvc;
  ServiceHandle< IPixelByteStreamErrorsSvc > m_BSErrorsSvc;

  std::string m_pixelRDOKey;
  
  double m_nEvents;
  double m_occupancyCut;

  TH1D* m_nEventsHist;
  std::vector<TH2D*> m_hitMaps;
  std::vector<TH2C*> m_noiseMaps;
  std::vector<TH1D*> m_LBdependence;
  
  TH1D* m_disabledModules;

  const PixelID* m_pixelID;

  double m_disk1ACut;
  double m_disk2ACut;
  double m_disk3ACut;
  
  double m_disk1CCut;
  double m_disk2CCut;
  double m_disk3CCut;
  
  double m_bLayerCut;
  double m_layer1Cut;
  double m_layer2Cut;

  double m_longPixelMultiplier;
  double m_gangedPixelMultiplier;

  int m_maxLVL1A;

  bool m_occupancyPerBC;

  unsigned int m_nBCReadout;
  
  int m_lbMin;
  int m_lbMax;

  bool m_calculateNoiseMaps;
};
 
#endif 
