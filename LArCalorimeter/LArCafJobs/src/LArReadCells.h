/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCAFJOBS_LArReadCells_H
#define LARCAFJOBS_LArReadCells_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "TTree.h"

class CaloCell_ID;


class LArReadCells: public ::AthAlgorithm { 
 public: 
  LArReadCells( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~LArReadCells(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

   double m_etcut;

   const DataHandle<CaloIdManager> m_caloIdMgr;
   const CaloCell_ID*       m_calo_id;


   TTree* m_tree;
   int m_runNumber;
   int m_lbNumber;
   int m_eventNumber;
   int m_bcid; 
   int m_error;
   int m_ncells;
   float m_ECell[250000];
   float m_TCell[250000];
   float m_EtaCell[250000];
   float m_PhiCell[250000];
   int   m_LayerCell[250000];
   int   m_ProvCell[250000];
   int   m_QuaCell[250000];
   int   m_GainCell[250000];
   int   m_HwidCell[250000];
   int   m_ADC[250000][32];

    ToolHandle<LArCablingService> m_larCablingSvc;
    const LArOnlineID*          m_lar_online_id;


}; 

#endif //> !MYSPLASHANALYSIS_LArReadCells_H
