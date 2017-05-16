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
namespace InDetDD{ // kazuki
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
  // vector of modulename and vector(barrel/endcap, layer, phi, eta)
  std::vector< std::pair< std::string, std::vector<int> > > m_pixelMapping;
  
  //std::vector<int> getPositionFromDCSID (std::string DCSID);
  std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);

  std::vector<std::string> &splitter(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  }
  
  std::vector<std::string> splitter(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splitter(s, delim, elems);
    return elems;
  }

  bool is_file_exist(const char *fileName){
    std::ifstream infile;
    infile.open(fileName);
    return infile.good();
  }
  
 private:
  ServiceHandle <ITHistSvc> m_tHistSvc;
  ServiceHandle <IInDetConditionsSvc> m_pixelConditionsSummarySvc;
  ServiceHandle <IPixelByteStreamErrorsSvc> m_BSErrorsSvc;
  ServiceHandle <ISpecialPixelMapSvc> m_specialPixelMapSvc; 
  
  std::string m_pixelRDOKey;
  
  bool m_isIBL; // kazuki
  std::vector<int> m_moduleHashList;
  
  double m_nEvents;
  //  double m_occupancyCut;
  
  TH1D* m_nEventsHist;
  TH1D* m_nEventsLBHist;
  std::vector<TH2D*> m_hitMaps;
  std::vector<TH2C*> m_noiseMaps;
  std::vector<TH1D*> m_LBdependence;
  std::vector<TH1D*> m_BCIDdependence;
  std::vector<TH1D*> m_TOTdistributions;
  
  TH1D* m_disabledModules;
  TH2D* m_overlayedPixelNoiseMap;
  TH2D* m_overlayedIBLDCNoiseMap; // Planar Double Chip
  TH2D* m_overlayedIBLSCNoiseMap; // 3D Single Chip
  //TH2D* m_overlayedDBMNoiseMap; // DBM

  const PixelID* m_pixelID;
  const InDetDD::PixelDetectorManager* m_pixman; // kazuki

  double m_disk1ACut;
  double m_disk2ACut;
  double m_disk3ACut;

  double m_disk1CCut;
  double m_disk2CCut;
  double m_disk3CCut;

  double m_iblCut; // kazuki
  double m_bLayerCut;
  double m_layer1Cut;
  double m_layer2Cut;
  double m_dbmCut; // kazuki

  double m_longPixelMultiplier;
  double m_gangedPixelMultiplier;

  //  int m_maxLVL1A;
  
  bool m_occupancyPerBC;

  unsigned int m_nBCReadout;

  int m_lbMin;
  int m_lbMax;

  bool m_calculateNoiseMaps;
};

#endif // PIXELCONDITIONSALGS_NOISEMAPBUILDER_H
