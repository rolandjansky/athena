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
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
//#include "InDetTruthInterfaces/IPRD_MultiTruthBuilder.h"

class MuonPRD_MultiTruthMaker : public AthAlgorithm  {
  
public:
  MuonPRD_MultiTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:

  std::string m_MDT_ContainerName;
  std::string m_CSC_ContainerName;
  std::string m_RPC_ContainerName;
  std::string m_TGC_ContainerName;
  std::string m_STGC_ContainerName;
  std::string m_MM_ContainerName;

  std::string m_MDT_SimDataMapName;
  std::string m_CSC_SimDataMapName;
  std::string m_RPC_SimDataMapName;
  std::string m_TGC_SimDataMapName;
  std::string m_STGC_SimDataMapName;
  std::string m_MM_SimDataMapName;

  std::string m_MDT_PRD_TruthName;
  std::string m_CSC_PRD_TruthName;
  std::string m_RPC_PRD_TruthName;
  std::string m_TGC_PRD_TruthName;
  std::string m_STGC_PRD_TruthName;
  std::string m_MM_PRD_TruthName;

  //ToolHandle<InDet::IPRD_MultiTruthBuilder> m_PRDTruthTool;

  //----------------------------------------------------------------
  template < class PrepDataContainer, class SIMDATACOLLECTION > 
  StatusCode buildPRD_Truth(const std::string& prepDataKey, const std::string& sdoKey, const std::string& outputKey);

  template<class PRD_Container_Iterator, class SIMDATACOLLECTION >
  void addPRDCollections(PRD_MultiTruthCollection* prdTruth,
       PRD_Container_Iterator collections_begin,
       PRD_Container_Iterator collections_end,
       const SIMDATACOLLECTION* simDataMap);
    
  template<class PRD_Collection_Iterator, class SIMDATACOLLECTION >
  void addPRDRange (PRD_MultiTruthCollection* prdTruth,
        PRD_Collection_Iterator range_begin,
        PRD_Collection_Iterator range_end,
        const SIMDATACOLLECTION* simDataMap);


  // An analogue of this comes from a tool in InDet
  template < class SIMDATACOLLECTION >
  void addPrepRawDatum(PRD_MultiTruthCollection *prdTruth,
             const Trk::PrepRawData* prd,
             const SIMDATACOLLECTION* simDataMap);

};

#endif //PRD_MULTITRUTHMAKER_H
