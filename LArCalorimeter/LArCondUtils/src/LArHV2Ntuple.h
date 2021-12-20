/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TheLArHV2Ntuple.h
//

#ifndef LARCONDUTILS_LARHV2NTUPLE_H
#define LARCONDUTILS_LARHV2NTUPLE_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/FCALHVManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "TTree.h"

class CaloCell_ID;
class LArOnlineID;
class Identifier;
class HWIdentifier;
class CondAttrListCollection;

class LArHV2Ntuple : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    LArHV2Ntuple(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    virtual ~LArHV2Ntuple();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute() override;
    
  private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  SG::ReadCondHandleKey<LArHVIdMapping> m_hvCablingKey
    {this, "LArHVIdMapping", "LArHVIdMap", "SG key for HV ID mapping"};
  SG::ReadCondHandleKeyArray<CondAttrListCollection>  m_DCSFolderKeys
    { this, "DCSFolderNames", {"/LAR/DCS/HV/BARREl/I16", "/LAR/DCS/HV/BARREL/I8"}, "DCS folders with HV values"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };


  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  ITHistSvc* m_thistSvc;
  TTree* m_tree;

  bool m_addcells;

  // tree variables
   int m_bec;
   int m_isPresampler;
   float m_eta;
   float m_phi;
   int m_electrode;
   int m_gap;
   int m_hvline;
   float m_hv;
   float m_current;

   int m_barrelec;
   int m_posneg;
   int m_FT;
   int m_slot;
   int m_channel;

  // ID helper
  const CaloCell_ID* m_caloId; 
  const LArOnlineID* m_onlId;
  std::map<int, std::vector<HWIdentifier> >m_hvonlId_map;

  std::vector<int> GetHVLines (const EMBHVManager::EMBHVData& hvdata_EMB,
                               const EMBPresamplerHVManager::EMBPresamplerHVData& hvdata_EMBPS,
                               const EMECHVManager::EMECHVData& hvdata_EMEC_OUT,
                               const EMECHVManager::EMECHVData& hvdata_EMEC_IN,
                               const EMECPresamplerHVManager::EMECPresamplerHVData& hvdata_EMECPS,
                               const HECHVManager::HECHVData& hvdata_HEC,
                               const FCALHVManager::FCALHVData& hvdata_FCAL,
                               const Identifier& id,
			       const CaloDetDescrManager* calodetdescrmgr) ;
};
#endif
