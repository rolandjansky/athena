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

class IIOVRegistrationSvc;
class ILArHVPathologyDbTool;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnlineID;
class CaloIdManager;
class CaloDetDescrManager;
class Identifier;

class LArHVPathologyDbAlg : public AthAlgorithm 
{
 public:
  LArHVPathologyDbAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArHVPathologyDbAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  StatusCode createCondObjects (const EventContext& ctx);
  StatusCode printCondObjects (const EventContext& ctx);
  StatusCode registerCondObjects();
  std::vector<unsigned int> getElectInd(const LArHVIdMapping& hvIdMapping,
                                        const Identifier& id, unsigned int module, unsigned int line);
  int getHVline(const LArHVIdMapping& hvIdMapping,
                const Identifier& id, short unsigned int ElectInd);
 
  BooleanProperty           m_writeCondObjs;
  StringProperty            m_inpFile;
  StringProperty            m_outFile;
 
  StringProperty            m_folder;
  StringProperty            m_outpTag;

  ServiceHandle<IIOVRegistrationSvc>   m_regSvc;
  ToolHandle<ILArHVPathologyDbTool>    m_pathologyTool;

  int m_mode;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const LArEM_ID*       m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*       m_larfcal_id;
  const LArOnlineID*      m_laronline_id;

  SG::ReadCondHandleKey<LArHVIdMapping> m_hvCablingKey
    {this, "LArHVIdMapping", "LArHVIdMap", "SG key for HV ID mapping"};
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey
     {this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
};

#endif
