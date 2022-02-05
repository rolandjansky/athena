/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TheCaloCellNoiseAlg.h
//

#ifndef CALOCONDPHYSALGS_CALOCELLNOISEALG_H
#define CALOCONDPHYSALGS_CALOCELLNOISEALG_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "CaloConditions/CaloNoise.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


  class CaloCellNoiseAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloCellNoiseAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    virtual ~CaloCellNoiseAlg();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          stop() override;
    
  private:

    StatusCode         fillNtuple();
    StatusCode         fitNoise();
    static StatusCode         readNtuple();
    float              getLuminosity();

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
    ITHistSvc* m_thistSvc{nullptr};

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  const CaloCell_ID*       m_calo_id{nullptr};
  SG::ReadCondHandleKey<ILArNoise>    m_noiseKey{this,"NoiseKey","LArNoiseSym","SG Key of ILArNoise object"};
  SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of LArPedestal object"};
  SG::ReadCondHandleKey<LArADC2MeV> m_adc2mevKey
    { this, "ADC2MeVKey", "LArADC2MeV", "SG Key of the LArADC2MeV CDO" };
  SG::ReadCondHandleKey<CaloNoise> m_totalNoiseKey
    { this, "TotalNoiseKey", "totalNoise", "SG conditions key for total noise" };
  SG::ReadCondHandleKey<CaloNoise> m_elecNoiseKey
    { this, "ElecNoiseKey", "electronicNoise", "SG conditions key for electronic noise" };

  // list of cell energies
  struct CellInfo {
       int nevt;
       int nevt_good;
       double average;
       double rms;
       int identifier;
       int sampling;
       float eta;
       float phi;
       float reference;
   };
   std::vector<CellInfo> m_CellList;
   int m_ncell;

   unsigned int m_lumiblock;
   unsigned int m_lumiblockOld;
   bool m_first;

   // Split this out into a separate, dynamically-allocated block.
   // Otherwise, the CaloCellNoiseAlg is so large that it violates
   // the ubsan sanity checks.
   struct TreeData {
     float m_luminosity {0};
     int  m_ncell {0};
     int m_nevt[200000] {0};
     int m_nevt_good[200000] {0};
     int m_layer[200000] {0};
     int m_identifier[200000] {0};
     float m_eta[200000] {0};
     float m_phi[200000] {0};
     float m_average[200000] {0};
     float m_rms[200000] {0};
     float m_reference[200000] {0};
   };
   std::unique_ptr<TreeData> m_treeData;
   TTree* m_tree;

   bool m_doMC;
   bool m_readNtuple;
   bool m_doFit;
   bool m_doLumiFit;
   int  m_nmin;
   ToolHandle<Trig::TrigDecisionTool> m_trigDecTool; //!< TDT handle
   std::string  m_triggerChainProp;
   FloatArrayProperty m_cuts;
   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
   std::string m_lumiFolderName;
   int m_addlumiblock;
   float m_deltaLumi;

  };
#endif
