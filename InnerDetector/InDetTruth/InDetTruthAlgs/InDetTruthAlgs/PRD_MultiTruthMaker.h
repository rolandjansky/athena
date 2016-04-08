/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
// A. Gaponenko, 2006

#ifndef PRD_MULTITRUTHMAKER_H
#define PRD_MULTITRUTHMAKER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetTruthInterfaces/IPRD_MultiTruthBuilder.h"

namespace InDet {

class PRD_MultiTruthMaker : public AthAlgorithm  {
  
public:
  PRD_MultiTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  
  std::string m_PixelClustersName;
  std::string m_SCTClustersName;
  std::string m_TRTDriftCircleContainerName; 
  std::string m_simDataMapNamePixel;
  std::string m_simDataMapNameSCT;
  std::string m_simDataMapNameTRT;

  std::string m_PRDTruthNamePixel;
  std::string m_PRDTruthNameSCT;
  std::string m_PRDTruthNameTRT;

  ToolHandle<InDet::IPRD_MultiTruthBuilder> m_PRDTruthTool;

  //----------------------------------------------------------------
  template<class PRD_Container_Iterator>
  void addPRDCollections(PRD_MultiTruthCollection* prdTruth,
			 PRD_Container_Iterator collections_begin,
			 PRD_Container_Iterator collections_end,
			 const InDetSimDataCollection* simDataMap,
			 bool pixels);
    
  template<class PRD_Collection_Iterator>
  void addPRDRange (PRD_MultiTruthCollection* prdTruth,
		    PRD_Collection_Iterator range_begin,
		    PRD_Collection_Iterator range_end,
		    const InDetSimDataCollection* simDataMap,
		    bool pixels);

};


} // namespace InDet


#endif //PRD_MULTITRUTHMAKER_H
