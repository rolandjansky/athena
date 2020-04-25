/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "CxxUtils/checker_macros.h"

  class CaloCellNoiseAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloCellNoiseAlg(const std::string& name, ISvcLocator* pSvcLocator) 
      ATLAS_CTORDTOR_NOT_THREAD_SAFE;//Due to Data Handle not thread safe
    /** Default Destructor */
    ~CaloCellNoiseAlg() 
      ATLAS_CTORDTOR_NOT_THREAD_SAFE; //Due to DataHandle not thread safe
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize ATLAS_NOT_THREAD_SAFE(); //StoreGateSvc::regHandle(const DataHandle<H>& ... not safe
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    /** standard Athena-Algorithm method */
    StatusCode          stop ATLAS_NOT_THREAD_SAFE(); //Due tof fitNoise, so also due to DataHandle not thread safe
    
  private:

    StatusCode         fillNtuple();
    StatusCode         fitNoise ATLAS_NOT_THREAD_SAFE(); //Due to DataHandle not thread safe
    StatusCode         readNtuple();
    float              getLuminosity();

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  ITHistSvc* m_thistSvc;

  const CaloDetDescrManager* m_calodetdescrmgr;
  const CaloCell_ID*       m_calo_id;
  const DataHandle<ILArNoise> m_dd_noise; /* Data Handle is marked as not thread safe*/
  const DataHandle<ILArPedestal> m_dd_pedestal; /* Data Handle is marked as not thread safe*/ 
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;

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
   ToolHandle<ICaloNoiseTool> m_noiseToolDB;
   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
   std::string m_lumiFolderName;
   int m_addlumiblock;
   float m_deltaLumi;

  };
#endif
