#ifndef DERIVATIONFRAMEWORKCORE_GOODRUNSLISTFILTERALGORITHM_H
#define DERIVATIONFRAMEWORKCORE_GOODRUNSLISTFILTERALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

namespace DerivationFramework {

class GoodRunsListFilterAlgorithm: public ::AthAlgorithm { 
 public: 
  GoodRunsListFilterAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GoodRunsListFilterAlgorithm(); 

  ///uncomment and implement methods as required

                                        //IS EXECUTED:
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode  finalize();       //once, after all events processed
  

 private: 
  ToolHandle<IGoodRunsListSelectionTool> m_grlTool, m_brlTool;

  std::vector<std::string> m_goodRunsListVec;
  std::vector<std::string> m_blackRunsListVec;

  uint m_maxGRLRunNumber=0;

}; 

}

#endif //> !DERIVATIONFRAMEWORKCORE_GOODRUNSLISTFILTERALGORITHM_H
