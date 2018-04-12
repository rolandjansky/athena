/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGS_NOISEMAPBUILDER_H
#define PIXELCONDITIONSALGS_NOISEMAPBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility> // pair

class IInDetConditionsSvc;
class IPixelByteStreamErrorsSvc;
class ITHistSvc;
class PixelID;
class TH2D;
class TH2C;
class TH1D;
class ISpecialPixelMapSvc;

namespace InDetDD{ 
  class PixelDetectorManager;
}

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

class NoiseMapBuilder: public AthAlgorithm {

 public:
  NoiseMapBuilder (const std::string& name, ISvcLocator* pSvcLocator);
  ~NoiseMapBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  std::string getDCSIDFromPosition(int bec, int layer, int modPhi, int modEta);
  
  std::vector<std::string> &splitter(const std::string &str, 
				     char delim, 
				     std::vector<std::string> &elems);
  
  std::vector<std::string> splitter(const std::string &str, 
				    char delim);
  
  StatusCode registerHistograms();

  const std::string histoSuffix(const int bec, const int layer);
  
 private:
  ServiceHandle <ITHistSvc> m_tHistSvc;
  ServiceHandle <IInDetConditionsSvc> m_pixelConditionsSummarySvc;
  ServiceHandle <IPixelByteStreamErrorsSvc> m_BSErrorsSvc;
  ServiceHandle <ISpecialPixelMapSvc> m_specialPixelMapSvc; 

  const InDetDD::PixelDetectorManager *m_pixman; 
  const PixelID *m_pixelID;
  
  // vector of modulename and vector(barrel/endcap, layer, phi, eta)
  std::vector< std::pair< std::string, std::vector<int> > > m_pixelMapping;

  std::string m_pixelRDOKey;  
  //std::vector<int> m_moduleHashList;
  
  double m_nEvents;
  //  double m_occupancyCut;
  
  TH1D *m_nEventsHist;
  TH1D *m_nEventsLBHist;
  std::vector<TH2D*> m_hitMaps;
  std::vector<TH1D*> m_LBdependence;
  std::vector<TH1D*> m_BCIDdependence;
  std::vector<TH1D*> m_TOTdistributions;
  std::vector<TH2C*> m_noiseMaps;  
  TH1D *m_disabledModules;
  TH2D *m_overlayedPixelNoiseMap;
  TH2D *m_overlayedIBLDCNoiseMap; // Planar Double Chip
  TH2D *m_overlayedIBLSCNoiseMap; // 3D Single Chip
  //TH2D* m_overlayedDBMNoiseMap; // DBM

  // cuts ....
  double m_disk1ACut; // disk-1, A-side
  double m_disk2ACut; // disk-2, A-side
  double m_disk3ACut; // disk-3, A-side
  double m_disk1CCut; // disk-1, C-side
  double m_disk2CCut; // disk-2, C-side
  double m_disk3CCut; // disk-3, C-side
  double m_iblCut;    // IBL
  double m_bLayerCut; // B-layer
  double m_layer1Cut; // Layer 1
  double m_layer2Cut; // Layer 2
  double m_dbmCut;    // DBM

  int m_hist_lbMax;   // max number of LB 

  double m_longPixelMultiplier;
  double m_gangedPixelMultiplier;

  //  int m_maxLVL1A;
  
  bool m_occupancyPerBC;

  unsigned int m_nBCReadout;

  int m_evt_lbMin; // lower limit for LB to be taken into account
  int m_evt_lbMax; // upper limit for LB to be taken into account

  bool m_calculateNoiseMaps;
};


#endif // PIXELCONDITIONSALGS_NOISEMAPBUILDER_H
