/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#ifndef PRD_MULTITRUTHMAKER_H
#define PRD_MULTITRUTHMAKER_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetTruthInterfaces/IPRD_MultiTruthBuilder.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"

namespace InDet {

class PRD_MultiTruthMaker : public AthReentrantAlgorithm  {

public:
  PRD_MultiTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute_r(const EventContext &ctx) const;
  virtual StatusCode finalize();

private:

  SG::ReadHandleKey<InDet::SiClusterContainer>       m_PixelClustersName;
  SG::ReadHandleKey<InDet::SiClusterContainer>       m_SCTClustersName;
  SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_TRTDriftCircleContainerName;
  SG::ReadHandleKey<InDetSimDataCollection>          m_simDataMapNamePixel;
  SG::ReadHandleKey<InDetSimDataCollection>          m_simDataMapNameSCT;
  SG::ReadHandleKey<InDetSimDataCollection>          m_simDataMapNameTRT;

  SG::WriteHandleKey<PRD_MultiTruthCollection> m_PRDTruthNamePixel;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_PRDTruthNameSCT;
  SG::WriteHandleKey<PRD_MultiTruthCollection> m_PRDTruthNameTRT;

  ToolHandle<InDet::IPRD_MultiTruthBuilder> m_PRDTruthTool;

  //----------------------------------------------------------------
  template<class PRD_Container_Iterator>
  void addPRDCollections(PRD_MultiTruthCollection* prdTruth,
			 PRD_Container_Iterator collections_begin,
			 PRD_Container_Iterator collections_end,
			 const InDetSimDataCollection* simDataMap,
			 bool pixels) const;

  template<class PRD_Collection_Iterator>
  void addPRDRange (PRD_MultiTruthCollection* prdTruth,
		    PRD_Collection_Iterator range_begin,
		    PRD_Collection_Iterator range_end,
		    const InDetSimDataCollection* simDataMap,
		    bool pixels) const;

};


} // namespace InDet


#endif //PRD_MULTITRUTHMAKER_H
