/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDUTILS_LARHVPATHOLOGYDBALG_H
#define LARCONDUTILS_LARHVPATHOLOGYDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

class IIOVRegistrationSvc;
class ILArHVPathologyDbTool;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnlineID;
class CaloIdManager;
class Identifier;

class LArHVPathologyDbAlg : public AthAlgorithm 
{
 public:
  LArHVPathologyDbAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArHVPathologyDbAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode stop() override;

 private:
  StatusCode createCondObjects (const EventContext& ctx, const CaloDetDescrManager* calodetdescrmgr);
  StatusCode printCondObjects (const EventContext& ctx, const CaloDetDescrManager* calodetdescrmgr);
  StatusCode registerCondObjects();
  std::vector<unsigned int> getElectInd(const LArHVIdMapping& hvIdMapping,
                                        const Identifier& id, unsigned int module, unsigned int line,
					const CaloDetDescrManager* calodetdescrmgr);
  int getHVline(const LArHVIdMapping& hvIdMapping,
                const Identifier& id, short unsigned int ElectInd,
		const CaloDetDescrManager* calodetdescrmgr);
 
  BooleanProperty           m_writeCondObjs;
  StringProperty            m_inpFile;
  StringProperty            m_outFile;
 
  StringProperty            m_folder;
  StringProperty            m_outpTag;

  ServiceHandle<IIOVRegistrationSvc>   m_regSvc;
  ToolHandle<ILArHVPathologyDbTool>    m_pathologyTool;

  int m_mode;

  const CaloIdManager* m_caloIdMgr{nullptr};
  const LArEM_ID*      m_larem_id{nullptr};
  const LArHEC_ID*     m_larhec_id{nullptr};
  const LArFCAL_ID*    m_larfcal_id{nullptr};
  const LArOnlineID*   m_laronline_id{nullptr};

  SG::ReadCondHandleKey<LArHVIdMapping> m_hvCablingKey
    {this, "LArHVIdMapping", "LArHVIdMap", "SG key for HV ID mapping"};
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey
    {this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey 
    {this, "CaloDetDescrManager", "CaloDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store" };
};

#endif
