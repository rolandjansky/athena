/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TheCaloCellNoiseAlg.h
//

#ifndef _CaloCondBlobAlgs_CaloCellNoiseAlg_H
#define _CaloCondBlobAlgs_CaloCellNoiseAlg_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
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

  class CaloCellNoiseAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloCellNoiseAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CaloCellNoiseAlg();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    /** standard Athena-Algorithm method */
    StatusCode          stop();
    
  private:

    StatusCode         fillNtuple();
    StatusCode         fitNoise();
    StatusCode         readNtuple();
    float              getLuminosity();

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  ITHistSvc* m_thistSvc;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const CaloCell_ID*       m_calo_id;
  const DataHandle<ILArNoise> m_dd_noise;
  const DataHandle<ILArPedestal> m_dd_pedestal;
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

   int  m_ncell;

   unsigned int m_lumiblock;
   unsigned int m_lumiblockOld;
   bool m_first;

   float m_luminosity;
   int m_nevt[200000];
   int m_nevt_good[200000];
   int m_layer[200000];
   int m_identifier[200000];
   float m_eta[200000];
   float m_phi[200000];
   float m_average[200000];
   float m_rms[200000];
   float m_reference[200000];
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
   std::string m_lumiFolderName;
   int m_addlumiblock;
   float m_deltaLumi;

  };
#endif
