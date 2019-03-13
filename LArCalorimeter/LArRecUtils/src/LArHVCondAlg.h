/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
  @brief This conditions algo produces both LArHVData and LArAffectedRegions

*/


#ifndef LARHVCONDALG_H
#define LARHVCONDALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/DataHandle.h" 
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "LArRecConditions/LArHVPathology.h"
#include "LArRecConditions/LArHVData.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArHVIdMapping.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "GaudiKernel/ICondSvc.h"


// forward declaration
class StoreGateSvc; 
class CondAttrListCollection;
class AthenaAttributeList;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloDetDescrManager;
class Identifier;
class LArElectrodeID;
class LArHVLineID;
class HWIdentifier;
class LArOnlineID;

class LArHVCondAlg: public AthReentrantAlgorithm
{
 
  public: 

  LArHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~LArHVCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  StatusCode execute(const EventContext& ctx) const override;


  private:

  // Conditions keys read:

  SG::ReadCondHandleKey<LArHVPathology> m_pathologiesKey { this, "HVPathologies", "LArHVPathology", "Key for HV pathologies in Cond. store"};

  SG::ReadCondHandleKeyArray<CondAttrListCollection>  m_DCSFolderKeys { this, "DCSFolderNames", {"/LAR/DCS/HV/BARREl/I16", "/LAR/DCS/HV/BARREL/I8"}, "DCS folders with HV values"};

  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey{this, "MissingFEBKey", "LArBadFeb", "SG key for missing FEB object"};

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};

  // Conditions keys write:
  SG::WriteCondHandleKey<LArHVData> m_hvDataKey {this, "OutputHVData", "LArHVData", "Key for output HV data object"};
  
  SG::WriteCondHandleKey<CaloAffectedRegionInfoVec> m_outKey{this, "OutputKey", "LArAffectedRegionInfo", "SG key for output"};

  // other members:
  
  const CaloDetDescrManager* m_calodetdescrmgr;
  const LArEM_ID* m_larem_id;
  const LArHEC_ID* m_larhec_id;
  const LArFCAL_ID* m_larfcal_id;
  const LArElectrodeID* m_electrodeID;
  const LArHVLineID* m_hvLineID;
  const LArOnlineID* m_onlineID;
  ServiceHandle<ICondSvc> m_condSvc;

  bool m_doHV;
  bool m_doAffected;
  bool m_doAffectedHV;

  // private methods:
  
  // for the LArHVData construction:

  typedef std::vector<std::vector<unsigned short> > pathVec;

  StatusCode fillPayload(LArHVData *hvdata, const LArHVData* hvdataOld, std::vector<float> &voltage, std::vector<float> &current, std::vector<unsigned int> &hvlineidx, const LArHVPathology& pathologies, pathVec& hasPathologyEM, pathVec& hasPathologyHEC, pathVec& hasPathologyFCAL) const;
  void addHV(std::vector< LArHVData::HV_t > & v, double hv, double wt) const;
  void addCurr(std::vector< LArHVData::CURRENT_t > & ihv, double curr, double wt) const;
  std::vector<unsigned int> getElecList(const Identifier& id, const LArHVPathology& pathologies) const;
  StatusCode fillUpdatedHVChannelsVec(std::vector<float> &voltage, std::vector<float> &current, std::vector<unsigned int> &hvlineidx, std::vector<const CondAttrListCollection* > fldvec) const;

  // for the LArAffectedRegions construction

  void extendPhiRegion(float phi, float & phi_min, float & phi_max) const;

  StatusCode updateMethod(CaloAffectedRegionInfoVec *vAffected, const LArBadFebCont* bfCont, const LArOnOffIdMapping* cabling) const;

  StatusCode searchNonNominalHV_EMB(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const;
  StatusCode searchNonNominalHV_EMEC_OUTER(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const;
  StatusCode searchNonNominalHV_EMEC_INNER(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const;
  StatusCode searchNonNominalHV_HEC(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const;
  StatusCode searchNonNominalHV_FCAL(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const;
  float HV_nominal(const char *identification,const float eta) const;
  std::vector<int> returnProblem(const float eta, const float phi, const float delta_eta, const float delta_phi);

};

//inline 
//const std::vector<HWIdentifier>&  LArHVCondAlg::getUpdatedElectrodes() { 
//  return m_updatedElectrodes;
//}

#endif
