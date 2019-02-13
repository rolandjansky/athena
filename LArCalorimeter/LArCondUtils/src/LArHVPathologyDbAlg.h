/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDUTILS_LARHVPATHOLOGYDBALG_H
#define LARCONDUTILS_LARHVPATHOLOGYDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IIOVRegistrationSvc;
class ILArHVPathologyDbTool;
class LArCablingLegacyService ;
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
  StatusCode createCondObjects();
  StatusCode printCondObjects();
  StatusCode registerCondObjects();
  std::vector<unsigned int> getElectInd(const Identifier& id, unsigned int module, unsigned int line);
  int getHVline(const Identifier& id, short unsigned int ElectInd);
 
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
  ToolHandle<LArCablingLegacyService> m_cablingService;
  const LArEM_ID*       m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*       m_larfcal_id;
  const LArOnlineID*      m_laronline_id;

};

#endif
