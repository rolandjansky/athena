//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "LArRawConditions/LArHVScaleCorrComplete.h"

#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "LArRecConditions/LArHVCorr.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"

#include "LArHVScaleCorrTool.h"

// forward declaration
class CondAttrListCollection;
class AthenaAttributeList;
class CaloCell_ID;
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
  
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  virtual ~LArHVCondAlg() = default;

  virtual StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;


private:

  // Conditions keys read:
  SG::ReadCondHandleKey<LArHVPathology> m_pathologiesKey { this, "HVPathologies", "LArHVPathology", "Key for HV pathologies in Cond. store"};
  SG::ReadCondHandleKeyArray<CondAttrListCollection>  m_DCSFolderKeys { this, "DCSFolderNames", {"/LAR/DCS/HV/BARREl/I16", "/LAR/DCS/HV/BARREL/I8"}, 
									  "DCS folders with HV values"};
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey{this, "MissingFEBKey", "LArBadFeb", "SG key for missing FEB object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  SG::ReadCondHandleKey<LArHVIdMapping> m_hvMappingKey {this, "HVMappingKey", "LArHVIdMap", "Key for mapping object" };
  SG::ReadCondHandleKey<AthenaAttributeList>   m_hvRKey{this, "RvaluesKey", "/LAR/HVPathologiesOfl/Rvalues", "Cool folder with HV R values"};

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};

  // Conditions keys write:
  SG::WriteCondHandleKey<CaloAffectedRegionInfoVec> m_affectedKey{this, "OutputKey", "LArAffectedRegionInfo", "SG key for output"};

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineHVScaleCorrKey{this, "keyOnlineHVCorr", "LArHVScaleCorr",
                                                                "Input key for HVScaleCorr from conditions database (used online)"};

  SG::WriteCondHandleKey<LArHVCorr> m_outputHVScaleCorrKey{this, "keyOutputCorr", "LArHVScaleCorrRecomputed","Output key for LArHVScaleCorr"};

  //Other properties:
  Gaudi::Property<bool> m_doHV{this,"doHV",true,"create HV Scale Correction"};
  Gaudi::Property<bool> m_doRProp{this,"doR",true,"Use R values with current to improve HV"};
  Gaudi::Property<bool> m_undoOnlineHVCorr{this,"UndoOnlineHVCorr",true,"Undo the HVCorr done online"};
  Gaudi::Property<bool> m_useCurrentEMB{this,"UseCurrentsInHVEM",false,"Use currents in EMB as well"};
  Gaudi::Property<bool> m_useCurrentFCAL1{this,"UseCurrentsInHVFCAL1",false,"Use currents in FCAL1 as well"};
  Gaudi::Property<bool> m_useCurrentOthers{this,"UseCurrentsInHVOthers", "Use currents in other partitions as well"};
  bool m_doR = true; //will be set depending on the above properties

  Gaudi::Property<bool> m_doAffected{this,"doAffected",true,"create affected region info"};
  Gaudi::Property<bool> m_doAffectedHV{this,"doAffectedHV",true,"include HV non nominal regions info"};

  Gaudi::Property<std::vector<std::string> > m_fixHVStrings{this,"fixHVCorr"};

  // other members:
  const CaloCell_ID* m_calocellID=nullptr;
  const LArEM_ID* m_larem_id=nullptr;
  const LArHEC_ID* m_larhec_id=nullptr;
  const LArFCAL_ID* m_larfcal_id=nullptr;
  const LArElectrodeID* m_electrodeID=nullptr;
  const LArHVLineID* m_hvLineID=nullptr;
  const LArOnlineID* m_onlineID=nullptr;

  std::unique_ptr<const LArHVScaleCorrTool> m_scaleTool;

  //Internal representation of voltage & current per HV-Line (read for DCS)
  struct DCS_t {
    DCS_t(float ihv, float icurr) : hv(ihv),curr(icurr) {};
    DCS_t() = delete;
    float hv;
    float curr;
  };
  typedef std::unordered_map<unsigned,DCS_t> voltagePerLine_t;

  // Internal representation of voltage & current per cell,
  // definded in LArHVScaleCorrTool
  // struct HV_t {
  //   HV_t(float ihv, float iweight) : hv(ihv), weight(iweight) {};
  //   float hv; //voltage, potentially current*R corrected
  //   float weight;
  // };
  // typedef std::vector<HV_t> voltageCell_t;

  typedef LArHVScaleCorrTool::HV_t HV_t;
  typedef LArHVScaleCorrTool::voltageCell_t voltageCell_t;

  typedef std::vector<voltageCell_t> voltagePerCell_t;

  ///Internal strucutre for HV pathologies
  typedef std::vector<std::vector<unsigned short> > pathVec;

  /// Add voltage/weight for a sub-gap of a cell 
  void addHV(voltageCell_t& v, float hv, float weight) const;

  /// Read HV from DCS, store them in internal data structure per HV-line (Step 1)
  StatusCode  dcs2LineVoltage(voltagePerLine_t& result, const std::vector<const CondAttrListCollection* >& fldvec) const;

  /// Read the voltage per HV line and store it in structure per readout-cell (resolve the many-HV-lines-to-many-cells mapping). Simulanitously fill the pathologies
  StatusCode fillPathAndCellHV(voltagePerCell_t& hvdata
			       , const LArHVIdMapping* hvCabling
			       , const voltagePerLine_t& voltage
			       , const LArHVPathology& pathologies
			       , pathVec& hasPathologyEM
			       , pathVec& hasPathologyHEC
			       , pathVec& hasPathologyFCAL
			       , const float* rValues) const;


  std::vector<unsigned int> getElecList(const Identifier& id, const LArHVPathology& pathologies) const;





  // for the LArAffectedRegions construction
  void extendPhiRegion(float phi, float & phi_min, float & phi_max) const;

  StatusCode updateMethod(CaloAffectedRegionInfoVec *vAffected, const LArBadFebCont* bfCont, const LArOnOffIdMapping* cabling) const;

  StatusCode searchNonNominalHV_EMB(CaloAffectedRegionInfoVec *vAffected
				    , const LArHVIdMapping* hvCabling
				    , const voltagePerLine_t& voltage) const;

  StatusCode searchNonNominalHV_EMEC_OUTER(CaloAffectedRegionInfoVec *vAffected
					   , const LArHVIdMapping* hvCabling
					   , const voltagePerLine_t& voltage) const;

  StatusCode searchNonNominalHV_EMEC_INNER(CaloAffectedRegionInfoVec *vAffected
					   , const LArHVIdMapping* hvCabling
					   , const voltagePerLine_t& voltage) const;

  StatusCode searchNonNominalHV_HEC(CaloAffectedRegionInfoVec *vAffected
				    , const LArHVIdMapping* hvCabling
				    , const voltagePerLine_t& voltage) const;


  StatusCode searchNonNominalHV_FCAL(CaloAffectedRegionInfoVec *vAffected
				     , const LArHVIdMapping* hvCabling
				     , const voltagePerLine_t& voltage) const;

  float HV_nominal(const char *identification,const float eta) const;
  std::vector<int> returnProblem(const float eta, const float phi, const float delta_eta, const float delta_phi);

};


#endif
