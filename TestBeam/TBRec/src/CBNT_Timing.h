/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_CBNT_TIMING_H
#define TBREC_CBNT_TIMING_H

#include "GaudiKernel/ToolHandle.h"
#include "CBNT_TBRecBase.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/LArEM_ID.h"
 #include "LArCabling/LArOnOffIdMapping.h"

class StoreGateSvc;
class LArOnlineID;

#include <fstream>
#include <string>

class StoreGateSvc;

class CBNT_Timing : public CBNT_TBRecBase
{
 public:
  CBNT_Timing(const std::string & name, ISvcLocator * pSvcLocator);

  ~CBNT_Timing();

  //standart algorithm methods
  virtual StatusCode CBNT_initialize();
  virtual StatusCode CBNT_execute();
  virtual StatusCode CBNT_finalize();
  virtual StatusCode CBNT_clear();

 private: 
  static const int NOTIME = -999;
  static const int NOENERGY = -999;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  const LArOnlineID* m_onlineHelper;
  const LArEM_ID* m_emId;

  double m_energy_cut;
  bool m_first_event;

  std::string m_tbphase;
  std::vector<std::string> m_sampling_names;
  std::string m_caloCellName;

 /*! \brief stores */
  //  const CaloCell_ID* m_caloCellHelper;
  std::vector<CaloSampling::CaloSample> m_samplingIndices;
  std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> > m_calosAndSamplings;
  std::vector<HWIdentifier> m_febIDs;

  /*! \brief internal helpers */
  StatusCode setupLookupTables();
  std::map<int, std::string>                               m_slotToFebNameLookup;
  std::map<CaloCell_ID::SUBCALO, std::string>              m_caloToNameLookup;
  std::map<std::string, CaloCell_ID::SUBCALO>              m_caloFromNameLookup;
  std::map<CaloSampling::CaloSample, std::string>          m_samplingToNameLookup;
  std::map<std::string, CaloSampling::CaloSample>          m_samplingFromNameLookup;
  std::map<CaloSampling::CaloSample, CaloCell_ID::SUBCALO> m_caloLookup;


  // per cell:
  std::vector<long>  *m_layer_cell, *m_eta_cell, *m_phi_cell, *m_febId_cell;
  std::vector<float> *m_energy_cell, *m_time_cell;
  // per layer:
  std::vector<float> *m_time_layer;
  std::vector<long>  *m_layer_layer;

  // per feb
  std::vector<long>  *m_febId_feb;
  std::vector<float> *m_time_feb;
  std::vector<long> *m_slot_feb;
  std::vector<long> *m_feedthrough_feb;

  // per event:
  float  m_energy, m_time, m_tdc_phase; // time = calculated cell time (cubic), clock = phase (TBRec)
};

#endif
