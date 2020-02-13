/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LarEMSamplingFraction_H
#define LarEMSamplingFraction_H 1

//#include "GaudiKernel/HistoDef.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArfSampl.h"
#include "TileConditions/TileInfo.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include <vector>
#include <string>
#include "TTree.h"

using namespace std;

class LarEMSamplingFraction  : public ::AthAlgorithm
{

 public:

  LarEMSamplingFraction(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LarEMSamplingFraction();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private:
  
  /// Default constructor: 
  LarEMSamplingFraction();
  
  bool m_docells;
  
  int mc_pdg;
	double mc_eta;
	double mc_phi;
	double mc_e;
	double mc_pt;
  
  std::vector<Long64_t>* m_cell_identifier;
  std::vector<float>*    m_cell_energy_reco;
  std::vector<float>*    m_cell_energy_active_total_corrected;
  std::vector<float>*    m_cell_energy_active_total;
  std::vector<float>*    m_cell_energy_inactive_total;
  std::vector<int>*      m_cell_sampling;
  std::vector<float>*    m_cell_eta;
  std::vector<float>*    m_cell_phi;

  vector<float> *energy_reco;
  vector<float> *energy_hit;

  vector<float> *energy_inactive_total;
  vector<float> *energy_inactive_em;
  vector<float> *energy_inactive_nonem;
  vector<float> *energy_inactive_inv;
  vector<float> *energy_inactive_esc;
  
  vector<float> *energy_active_total_corrected;
  vector<float> *energy_active_total;
  vector<float> *energy_active_em;
  vector<float> *energy_active_nonem;
  vector<float> *energy_active_inv;
  vector<float> *energy_active_esc;
  
  TTree* mytree =0;

  vector<string> m_CalibrationHitContainerNames;

  const CaloDetDescrManager* m_calo_dd_man; 
  
  const CaloCell_ID* m_calo_id;
  
  const TileInfo *m_tileInfo;
  const TileID * m_tileID;
  const DataHandle<ILArfSampl>   m_dd_fSampl;
};

#endif //> !LarEMSamplingFraction_H
