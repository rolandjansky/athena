// -*- c++ -*-
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERMONITOR_H
#define CALOREC_CALOTOPOCLUSTERFROMTOWERMONITOR_H

#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthHistogramAlgorithm.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include "CaloRec/CaloTowerGeometrySvc.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"

#include <vector>
#include <map>

class TH1D;
class TH2D;

class CaloTopoClusterFromTowerMonitor : public AthHistogramAlgorithm
{
public:

  CaloTopoClusterFromTowerMonitor(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~CaloTopoClusterFromTowerMonitor();

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_towerContainerKey;
  ServiceHandle<CaloTowerGeometrySvc>           m_towerGeometrySvc;

  int    m_ncBins;
  double m_ncMin;
  double m_ncMax;

  int    m_nBins;
  double m_nMin;
  double m_nMax;

  int    m_ptBins;
  double m_ptMin;
  double m_ptMax;

  int    m_etaBins;
  double m_etaMin;
  double m_etaMax;

  int    m_phiBins;
  double m_phiMin;
  double m_phiMax;

  bool   m_doGeoAutoBins;

  TH1D* h_n;
  TH1D* h_pt;
  TH1D* h_eta;
  TH1D* h_phi;
  TH1D* h_nc;

  TH2D* d_n_eta_phi;
  TH2D* d_pt_eta;
  TH2D* d_nc_eta;

  typedef xAOD::CaloCluster                   cluster_t;
  typedef CaloCell                            cell_t;
  typedef CaloTowerGeometrySvc::index_t       index_t;

  typedef std::pair<index_t,const cluster_t*> key_t;
  typedef std::vector<const cell_t*>          data_t;
  typedef std::map<key_t,data_t >             table_t;

protected:

  virtual StatusCode book();

};
#endif


