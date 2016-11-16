/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERMONITOR_H
#define CALOREC_CALOTOWERMONITOR_H
///////////////////////////////////////////////////////////////////////////////
/// \brief simple monitor for CaloTower objects
///
/// The CaloTowerMonitor is an algorithm to produce a few histograms from 
/// CaloTowers to check the performance of the CaloTower algorithm.
///
/// \author Peter Loch <loch@physics.arizona.edu>
/// \date May 05, 2004 - first implementation
///
///////////////////////////////////////////////////////////////////////////////

#include "CaloGeoHelpers/CaloSampling.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;

namespace AIDA {
class IHistogram1D;
class IHistogram2D;
}

class CaloTowerMonitor : public AthAlgorithm
{

 public:
  /// Algorithm constructor
  CaloTowerMonitor(const std::string& name, ISvcLocator* pService );
  
  virtual ~CaloTowerMonitor();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:

  std::vector<std::string> m_collectionNames;

  ////////////////
  // Histograms //
  ////////////////

  // number of towers
  IHistogram1D* m_nTowers;
  IHistogram1D* m_nTowersVsEta;
  IHistogram1D* m_nTowersVsPhi;

  // tower shape
  IHistogram2D* m_cellsInEtaVsPhi;
  IHistogram1D* m_nCellsInTower;
  IHistogram2D* m_nCellsInTowerVsEta;
  IHistogram2D* m_nCellsInTowerVsPhi;

  // tower energies
  IHistogram1D* m_eTowers;
  IHistogram2D* m_eTowersVsEta;
  IHistogram2D* m_eTowersVsPhi;
  IHistogram1D* m_eLogTowers;

  // tower transversal energies
  IHistogram1D* m_etTowers;
  IHistogram2D* m_etTowersVsEta;
  IHistogram2D* m_etTowersVsPhi;
  IHistogram1D* m_etLogTowers;

  // eta/phi matches
  IHistogram2D* m_etaTowerVsCell;
  IHistogram2D* m_phiTowerVsCell;
  std::map<CaloSampling::CaloSample,IHistogram2D*> m_etaTowerVsCellCalos;
  std::map<CaloSampling::CaloSample,IHistogram2D*> m_phiTowerVsCellCalos;
};
#endif
