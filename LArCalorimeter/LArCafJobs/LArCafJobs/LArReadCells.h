/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCAFJOBS_LArReadCells_H
#define LARCAFJOBS_LArReadCells_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/ILArPedestal.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
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

   Gaudi::Property<double> m_etcut{this,"etCut",7500.,"Et cut to dump cells"};
   Gaudi::Property<double> m_etcut2{this,"etCut2",7500.,"Et cut to dump cells from second layer"};
   Gaudi::Property< std::string > m_outStream{this, "output","SPLASH", "to which stream write the ntuple"};

   const DataHandle<CaloIdManager> m_caloIdMgr;
   const CaloCell_ID*       m_calo_id = nullptr;


   TTree* m_tree = nullptr;
   int m_runNumber = 0;
   int m_lbNumber = 0;
   int m_eventNumber = 0;
   int m_bcid = 0; 
   int m_error = 0;
   int m_ncells = 0;
   std::vector<float> m_ECell    ;
   std::vector<float> m_TCell    ;
   std::vector<float> m_EtaCell  ;
   std::vector<float> m_PhiCell  ;
   std::vector<int>   m_LayerCell;
   std::vector<int>   m_ProvCell ;
   std::vector<int>   m_QuaCell  ;
   std::vector<int>   m_GainCell ;
   std::vector<int>   m_HwidCell ;
   std::vector<std::array<float ,32> > m_ADC    ;

    SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
    SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of Pedestal conditions object"};
    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};
    
    const LArOnlineID*          m_lar_online_id = nullptr;


}; 

#endif //> !MYSPLASHANALYSIS_LArReadCells_H

