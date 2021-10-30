/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSALGS_HITMAPBUILDER_H
#define PIXELCONDITIONSALGS_HITMAPBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility> // pair

//yosuke class IInDetConditionsSvc;
//yosuke class IPixelByteStreamErrorsSvc;
class ITHistSvc;
class PixelID;
class TH2F;
class TH1F;
class TH1;
//yosuke class ISpecialPixelMapSvc;

namespace InDetDD{
  class PixelDetectorManager;
}

/**
 *
 * HitMapBuilder.h
 *
 * Creates hit maps and maps of noisy pixels for every module
 * of the pixel detector. The maps are stored in a root file.
 * They can be written to the conditions database using the
 * algorithm NoiseMapDBWriter.
 *
 * ruwiedel@physik.uni-bonn.de
 *
 */

class HitMapBuilder: public AthAlgorithm {

 public:
  HitMapBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  ~HitMapBuilder();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  std::string getDCSIDFromPosition(int bec, int layer, int modPhi, int modEta);
  std::vector<std::string> &splitter(const std::string &str, char delim, std::vector<std::string> &elems);
  std::vector<std::string> splitter(const std::string &str, char delim);
  StatusCode registerHistograms();
  const std::string histoSuffix(const int bec, const int layer);

 private:
  ServiceHandle <ITHistSvc> m_tHistSvc;
  const InDetDD::PixelDetectorManager *m_pixman;
  const PixelID *m_pixelID;

  // vector of modulename and vector(barrel/endcap, layer, phi, eta)
  std::vector< std::pair< std::string, std::vector<int> > > m_pixelMapping;

  std::string m_pixelRDOKey;

  double m_nEvents;
  std::vector<double> m_nEventsLB; // Events per LB
  std::vector<double> m_nEventsLBCategory; // Events per certain LB for LB category

  std::unique_ptr<TH1F> m_nEventsHist;
  std::unique_ptr<TH1F> m_nEventsLBHist;
  std::vector<std::unique_ptr<TH2F>> m_occupancyMaps;
  std::vector<std::unique_ptr<TH2F>> m_occupancyMapsIBL2dLB;
  std::vector<std::unique_ptr<TH1F>> m_TOTdistributions;
  std::vector<std::unique_ptr<TH1F>> m_TOTdistributionsIBL2dLB;
  std::vector<std::unique_ptr<TH1F>> m_occupancyLB;

  int m_hist_lbMax;   // max number of LB

  int m_evt_lbMin; // lower limit for LB to be taken into account
  int m_evt_lbMax; // upper limit for LB to be taken into account

  int m_LBrange_max = -9999;

  const unsigned int m_nIblFes = 14 * (4 + 6*2) * 2; // 14 stave * (4 3Ds + 6 2Ds * 2 Fes) * 2 sides = 448
  const int m_perLB_min=0, m_perLB_max=3000, m_perLB_step=100; // For plots per certain LBs
  const int m_perLB_n = (m_perLB_max-m_perLB_min)/m_perLB_step; // For plots per certain LBs

  const int m_fei4bPixels = 26880; // 80 * 336
  const int m_pixModPixels = 46080; // 144 * 320;
};


#endif // PIXELCONDITIONSALGS_HITMAPBUILDER_H
