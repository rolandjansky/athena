/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHECNoise_H
#define LArHECNoise_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/AlgTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AnalysisTools/AnalysisTools.h"

//LAr services:
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArHVCablingTool.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "LArElecCalib/ILArPedestal.h"

// Trigger
#include "TrigDecisionTool/TrigDecisionTool.h"

//STL:
#include <string>
#include <bitset>


class LArOnlineID;
class LArElectrodeID;
class HWIdentifier;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class TTree;

class LArHECNoise : public AthAlgorithm  {

 public:

   LArHECNoise(const std::string& name, ISvcLocator* pSvcLocator);
   ~LArHECNoise();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();

 private:

   ITHistSvc * m_thistSvc;
    
   TTree* m_tree;

   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
   /*Tools*/
   ToolHandle<ICaloNoiseTool> m_calo_noise_tool;
   ToolHandle<Trig::IBunchCrossingTool> m_bc_tool;

   ToolHandle< Trig::TrigDecisionTool > m_trigDec;

   /*services*/
   const LArOnlineID* m_LArOnlineIDHelper;
   const CaloDetDescrManager* m_calodetdescrmgr;
   const CaloCell_ID*   m_calocell_id;

   /*declaration of branches*/
   int  m_nt_run ;
   int  m_nt_evtId;
   int  m_nt_evtCount;
   int  m_nt_evtTime;
   int  m_nt_evtTime_ns;
   int  m_nt_lb;
   int  m_nt_bcid;
   int  m_nt_gain;
   int  m_nt_side;
   int  m_nt_samp;
   int  m_nt_reg;
   int  m_nt_ieta;
   int  m_nt_iphi;
   int  m_nt_quality;
   short m_nt_digi[32];
   int m_nt_max;
   int m_nt_min;
   long m_nt_OID;
   float m_nt_avgMu;
   float m_nt_actMu;
   float m_nt_e;
   float m_nt_t;
   float m_nt_eta;
   float m_nt_phi;
   float m_nt_z;
   float m_nt_r;
   float m_nt_ped;
   float m_nt_pedRMS;
   float *m_nt_prescale;
   bool  *m_nt_trigger;

   // other members
   std::vector<std::string> m_TriggerLines;
   int m_MinDigitADC;
   int m_MaxDeltaT;
   const ILArPedestal* m_ped;

};

#endif // LArHECNoise_H

