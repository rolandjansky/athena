/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArAffectedRegionAlg_H
#define LArAffectedRegionAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/DataHandle.h"
#include "LArIdentifier/LArOnlineID.h"

#include <vector>

#include "CaloConditions/CaloAffectedRegionInfo.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"

#include "AthenaKernel/IOVSvcDefs.h" // for the definition of IOVSVC_CALLBACK_ARGS

#include "CaloIdentifier/CaloIdManager.h"

//#include ToolHandle<ILArHVTool> m_hvtool;
#include "AthenaPoolUtilities/CondAttrListCollection.h"

class LArCablingService;
class LArOnlineID;
class CaloDetDescrManager;
class ILArBadChanTool;

class CaloIdManager;
class ILArHVTool;
class IIOVDbMetaDataTool;

class LArAffectedRegionAlg : public AthAlgorithm,virtual public IIncidentListener  {
  //---
 public:
  LArAffectedRegionAlg(const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident& inc);
  StatusCode updateMethod(IOVSVC_CALLBACK_ARGS);
  StatusCode updateAffectedRegionsFromDB(IOVSVC_CALLBACK_ARGS);

  void searchNonNominalHV_EMB();
  void searchNonNominalHV_EMEC_OUTER();
  void searchNonNominalHV_EMEC_INNER();
  void searchNonNominalHV_HEC();
  void searchNonNominalHV_FCAL();

  float HV_nominal(const char *identification,const float eta);
  std::vector<int> returnProblem(const float eta, const float phi, const float delta_eta, const float delta_phi);  
  int getOccurenceProblem(const std::vector<CaloAffectedRegionInfo>& ArrayLArAffectedRegionInfo_global,float eta_min,float eta_max,float phi_min,float phi_max,int layer_min,int layer_max,int problem,float rate);
  void debuggingSearchDoublons(const std::vector<CaloAffectedRegionInfo>& ArrayLArAffectedRegionInfo);
  //---
 private:

  std::vector<CaloAffectedRegionInfo> m_ArrayLArAffectedRegionInfo_global;
  std::vector<CaloAffectedRegionInfo> m_ArrayLArAffectedRegionInfo_current_run;
  ToolHandle<LArCablingService> m_larCablingSvc;
  ToolHandle<ILArBadChanTool> m_BadChanTool;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  ToolHandle<ILArHVTool> m_hvtool;

  const LArOnlineID* m_onlineID;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  
  bool m_readingFromBytestream;

  CondAttrListCollection* m_attrListColl;  // info to give to metadata  
  ToolHandle<IIOVDbMetaDataTool> m_metaDataTool;          // Access to iov meta data tool  
  bool m_doHV;
  
  void extendPhiRegion(float phi, float & phi_min, float & phi_max);
};

#endif
