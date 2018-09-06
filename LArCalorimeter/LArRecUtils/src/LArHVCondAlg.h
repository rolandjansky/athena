/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVCONDALG_H
#define LARHVCONDALG_H


//#include "LArElecCalib/ILArHVTool.h" 
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/DataHandle.h" 
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "LArRecConditions/LArHVPathology.h"
#include "LArRecConditions/LArHVData.h"
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
class CaloIdManager;
class CaloDetDescrManager;
class Identifier;
class LArElectrodeID;
class LArHVLineID;
class HWIdentifier;

class LArHVCondAlg: public AthReentrantAlgorithm
{
 
  public: 

  LArHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~LArHVCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  StatusCode execute_r(const EventContext& ctx) const override;


  private:

  typedef std::vector<std::vector<unsigned short> > pathVec;

  StatusCode fillPayload(LArHVData *hvdata, const LArHVData* hvdataOld, std::vector<float> &voltage, std::vector<float> &current, std::vector<unsigned int> &hvlineidx, const LArHVPathology& pathologies, pathVec& hasPathologyEM, pathVec& hasPathologyHEC, pathVec& hasPathologyFCAL) const;
  void addHV(std::vector< LArHVData::HV_t > & v, double hv, double wt) const;
  void addCurr(std::vector< LArHVData::CURRENT_t > & ihv, double curr, double wt) const;
  std::vector<unsigned int> getElecList(const Identifier& id, const LArHVPathology& pathologies) const;
  StatusCode fillUpdatedHVChannelsVec(std::vector<float> &voltage, std::vector<float> &current, std::vector<unsigned int> &hvlineidx, std::vector<const CondAttrListCollection* > fldvec) const;


  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const LArEM_ID* m_larem_id;
  const LArHEC_ID* m_larhec_id;
  const LArFCAL_ID* m_larfcal_id;
  const LArElectrodeID* m_electrodeID;
  const LArHVLineID* m_hvLineID;

  SG::ReadCondHandleKey<LArHVPathology> m_pathologiesKey { this, "HVPathologies", "LArHVPathology", "Key for HV pathologies in Cond. store"};

  SG::ReadCondHandleKeyArray<CondAttrListCollection>  m_DCSFolderKeys { this, "DCSFolderNames", {"/LAR/DCS/HV/BARREl/I16", "/LAR/DCS/HV/BARREL/I8"}, "DCS folders with HV values"};

  SG::WriteCondHandleKey<LArHVData> m_hvDataKey {this, "OutputHVData", "LArHVData", "Key for output HV data object"};

  ServiceHandle<ICondSvc> m_condSvc;


};

//inline 
//const std::vector<HWIdentifier>&  LArHVCondAlg::getUpdatedElectrodes() { 
//  return m_updatedElectrodes;
//}

#endif
