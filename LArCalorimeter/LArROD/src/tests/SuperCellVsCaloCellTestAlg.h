/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_SUPERCELLVSCALOCELLTESTALG_H
#define LARROD_SUPERCELLVSCALOCELLTESTALG_H 1

//Author: Will Buttinger <will@cern.ch>
//Purpose: Quick comparison of supercell to sum-of-calocell quantities

#include "AthenaBaseComps/AthAlgorithm.h"


#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ToolHandle.h"

#include "TString.h"

class TProfile;
class TTree;
class TH1F;
class TH2F;

class SuperCellVsCaloCellTestAlg: public ::AthAlgorithm { 
 public: 
  SuperCellVsCaloCellTestAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~SuperCellVsCaloCellTestAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

   

 private: 
   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"SCCablingKey","LArOnOffIdMapSC","SG Key of LArOnOffIdMapping object"};
   ToolHandle<ICaloSuperCellIDTool> m_sc2ccMappingTool;
   const CaloCell_ID* m_ccIdHelper;

   std::string m_scKey, m_ccKey, m_tscKey, m_digitKey, m_stream;

/*
   static const int nBinsE=11;
   static const int nBinsEta=34;
   static constexpr double eBins[nBinsE+1] {0.00001,0.2,0.4,0.6,0.8,1,2,3,4,5,10,50};
   static constexpr double etaBins[nBinsEta+1] {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,3.1,3.2,3.5,3.8,4.1,4.5,4.9};
*/
   
   std::vector<TProfile*> m_etReso;
   std::vector<TProfile*> m_etResoTruth;

   std::vector<TH1F*> m_Reso_et;
   std::vector<TH2F*> m_Reso_et_vs_eta;
   std::vector<TH2F*> m_Reso_et_vs_et;
   std::vector<TH2F*> m_Linear_SCet_vs_et;

   std::map<TString,std::vector<float> > m_graphsX, m_graphsY;


   int m_eventNumber=0;
   int m_treeChannel=0;
   int m_treeSampling=0;
   float m_treeEta=0;
   float m_treeSCET=0;
   float m_treeTruthET=0;
   std::vector<short int>* m_treeDigits;
   TTree* m_tree;


}; 

#endif //> !LARROD_SUPERCELLVSCALOCELLTESTALG_H
