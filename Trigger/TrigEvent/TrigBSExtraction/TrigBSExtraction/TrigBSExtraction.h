/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_TRIGBSEXTRACTION_H
#define TRIGBSEXTRACTION_TRIGBSEXTRACTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigNavigation/Navigation.h"

namespace HLT {  
  class HLTResult;
}

/**
 * @brief Top algorithms which unpacks objects from BS and places them in SG.
 */

class TrigBSExtraction : public AthAlgorithm {
public:
  TrigBSExtraction(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigBSExtraction();   
  StatusCode initialize(); 
  StatusCode finalize();
  StatusCode execute();

private:
  template<class T> bool repackFeature();    //!< helper which actually does the work of repacking (templated for each TriggerType)
  StatusCode repackFeaturesToSG(const std::string& key, bool equalize); //!< a method which does loop over objects (depending on @param key does the job for EF or L2) @param equalize flattens the EDM if true

  ToolHandle<HLT::Navigation> m_navigationForEF; //!< handle to Navigation tools
  ToolHandle<HLT::Navigation> m_navigationForL2; 
  HLT::Navigation* m_nav;

  std::string m_l2ResultKey;  //!< key of HLTResult for L2
  std::string m_efResultKey;  //!< key of HLTResult for EF
  std::string m_hltResultKey;  //!< key of HLTResult for HLT P
  StringArrayProperty m_dataScoutingKeys; //!< keys for DataScouting HLT Results
};


#endif // TRIGBSEXTRACTION_TRIGBSEXTRACTION_H
