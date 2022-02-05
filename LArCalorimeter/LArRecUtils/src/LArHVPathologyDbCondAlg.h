/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVPATHOLOGYDBCONDALG
#define LARHVPATHOLOGYDBCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecConditions/LArHVPathology.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include<mutex>

class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnlineID;
class CaloDetDescrManager;
class LArHVLineID;
class Identifier;
class TClass;

class LArHVPathologyDbCondAlg: public AthReentrantAlgorithm
{
 public: 
  LArHVPathologyDbCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~LArHVPathologyDbCondAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

 private:

  void fillElectMap(const CaloDetDescrManager* calodetdescrmgr,
                    LArHVPathology* hvpath) const;

  SG::ReadCondHandleKey<AthenaAttributeList> m_pathologyFolderKey {this, "PathologyFolder", "/LAR/HVPathologiesOfl/Pathologies", "Cool folder for HV pathologies" };
  SG::ReadCondHandleKey<LArHVIdMapping> m_hvMappingKey {this, "HVMappingKey", "LArHVIdMap", "Key for mapping object" };
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};

  SG::WriteCondHandleKey<LArHVPathology> m_hvPAthologyKey {this, "HVPAthologyKey", "LArHVPathology", "Key for HV pathologies in Cond. store"};

  const LArEM_ID*       m_larem_id=nullptr;
  const LArHEC_ID*       m_larhec_id=nullptr;
  const LArFCAL_ID*       m_larfcal_id=nullptr;
  const LArOnlineID*      m_laronline_id=nullptr;
  const LArHVLineID*      m_hvlineHelper=nullptr;

  mutable std::mutex m_mut;

  TClass* m_klass=nullptr;

};

#endif
