/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#ifndef PRD_MULTITRUTHMAKER_H
#define PRD_MULTITRUTHMAKER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/CscSimDataCollection.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
//#include "InDetTruthInterfaces/IPRD_MultiTruthBuilder.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class MuonPRD_MultiTruthMaker : public AthAlgorithm  {
  
public:
  MuonPRD_MultiTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:

  SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_MDT_ContainerName;
  SG::ReadHandleKey<Muon::CscPrepDataContainer> m_CSC_ContainerName;
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_RPC_ContainerName;
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_TGC_ContainerName;
  SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_STGC_ContainerName;
  SG::ReadHandleKey<Muon::MMPrepDataContainer> m_MM_ContainerName;

  SG::ReadHandleKey<MuonSimDataCollection> m_MDT_SimDataMapName;
  SG::ReadHandleKey<CscSimDataCollection> m_CSC_SimDataMapName;
  SG::ReadHandleKey<MuonSimDataCollection> m_RPC_SimDataMapName;
  SG::ReadHandleKey<MuonSimDataCollection> m_TGC_SimDataMapName;
  SG::ReadHandleKey<MuonSimDataCollection> m_STGC_SimDataMapName;
  SG::ReadHandleKey<MuonSimDataCollection> m_MM_SimDataMapName;

  SG::WriteHandleKey<PRD_MultiTruthCollection> m_MDT_PRD_TruthName;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_CSC_PRD_TruthName;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_RPC_PRD_TruthName;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_TGC_PRD_TruthName;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_STGC_PRD_TruthName;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_MM_PRD_TruthName;

  //ToolHandle<InDet::IPRD_MultiTruthBuilder> m_PRDTruthTool;

  //----------------------------------------------------------------
  template < class PrepDataContainer, class SIMDATACOLLECTION > 
    StatusCode buildPRD_Truth(SG::ReadHandleKey<PrepDataContainer> prepDataKey, 
			      SG::ReadHandleKey<SIMDATACOLLECTION> sdoKey, 
			      SG::WriteHandleKey<PRD_MultiTruthCollection> outputKey);

  template<class PRD_Container_Iterator, class SIMDATACOLLECTION >
    void addPRDCollections(SG::WriteHandle<PRD_MultiTruthCollection> prdTruth,
			   PRD_Container_Iterator collections_begin,
			   PRD_Container_Iterator collections_end,
			   SG::ReadHandle<SIMDATACOLLECTION> simDataMap);
    
  template<class PRD_Collection_Iterator, class SIMDATACOLLECTION >
    void addPRDRange (SG::WriteHandle<PRD_MultiTruthCollection> prdTruth,
		      PRD_Collection_Iterator range_begin,
		      PRD_Collection_Iterator range_end,
		      SG::ReadHandle<SIMDATACOLLECTION> simDataMap);


  // An analogue of this comes from a tool in InDet
  template < class SIMDATACOLLECTION >
    void addPrepRawDatum(SG::WriteHandle<PRD_MultiTruthCollection> prdTruth,
			 const Trk::PrepRawData* prd,
			 SG::ReadHandle<SIMDATACOLLECTION> simDataMap);

};

#endif //PRD_MULTITRUTHMAKER_H
