/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_TRIGBSEXTRACTION_H
#define TRIGBSEXTRACTION_TRIGBSEXTRACTION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

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
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  /**
   @brief method which does loop over objects
   @param  navTool   navigation tool
   @param  key       does the job for EF or L2
   @param  equalize  flattens the EDM if true
   */
  StatusCode repackFeaturesToSG(HLT::Navigation& navTool, const std::string& key, bool equalize);

  ToolHandle<HLT::Navigation> m_navToolL2{this, "NavigationForL2", "HLT::Navigation/NavigationForL2",
                                          "Navigation tool for Run-1 L2 result"};
  ToolHandle<HLT::Navigation> m_navTool{this, "Navigation", "HLT::Navigation/Navigation",
                                        "Navigation tool for EF/HLT result"};

  Gaudi::Property<std::string> m_l2ResultKey{this, "L2ResultKey", "", "key for L2 result (Run-1)"};
  Gaudi::Property<std::string> m_hltResultKey{this, "HLTResultKey", "HLTResult_HLT", "key for EF/HLT result"};
  StringArrayProperty m_dataScoutingKeys{this, "DSResultKeys", {}, "keys for DataScouting HLT results"};
};


#endif // TRIGBSEXTRACTION_TRIGBSEXTRACTION_H
