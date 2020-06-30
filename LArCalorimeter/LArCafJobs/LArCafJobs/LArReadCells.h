/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCAFJOBS_LArReadCells_H
#define LARCAFJOBS_LArReadCells_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/ILArPedestal.h"
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

   Gaudi::Property<double> m_etcut{this,"etCut",1000.,"Et cut to dump cells"};

   const DataHandle<CaloIdManager> m_caloIdMgr;
   const CaloCell_ID*       m_calo_id;


   TTree* m_tree;
   int m_runNumber;
   int m_lbNumber;
   int m_eventNumber;
   int m_bcid; 
   int m_error;
   int m_ncells;
   std::vector<float> m_ECell     { 250000 };
   std::vector<float> m_TCell     { 250000 };
   std::vector<float> m_EtaCell   { 250000 };
   std::vector<float> m_PhiCell   { 250000 };
   std::vector<int>   m_LayerCell { 250000 };
   std::vector<int>   m_ProvCell  { 250000 };
   std::vector<int>   m_QuaCell   { 250000 };
   std::vector<int>   m_GainCell  { 250000 };
   std::vector<int>   m_HwidCell  { 250000 };
   std::vector<int[32]> m_ADC     { 250000 };

    SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
    SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of Pedestal conditions object"};
    const LArOnlineID*          m_lar_online_id;


}; 

#endif //> !MYSPLASHANALYSIS_LArReadCells_H

