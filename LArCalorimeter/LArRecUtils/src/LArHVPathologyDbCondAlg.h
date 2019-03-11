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

class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnlineID;
class CaloDetDescrManager;
class LArHVLineID;
class Identifier;

class LArHVPathologyDbCondAlg: public AthReentrantAlgorithm
{
 public: 
  LArHVPathologyDbCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~LArHVPathologyDbCondAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

 private:

  void fillElectMap(LArHVPathology* hvpath) const;

  SG::ReadCondHandleKey<AthenaAttributeList> m_pathologyFolderKey {this, "PathologyFolder", "/LAR/HVPathologiesOfl/Pathologies", "Cool folder for HV pathologies" };
  SG::ReadCondHandleKey<LArHVIdMapping> m_hvMappingKey {this, "HVMappingKey", "LArHVIdMap", "Key for mapping object" };
  SG::WriteCondHandleKey<LArHVPathology> m_hvPAthologyKey {this, "HVPAthologyKey", "LArHVPathology", "Key for HV pathologies in Cond. store"};

  const CaloDetDescrManager* m_calodetdescrmgr;
  const LArEM_ID*       m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*       m_larfcal_id;
  const LArOnlineID*      m_laronline_id;
  const LArHVLineID*      m_hvlineHelper;

};

#endif
