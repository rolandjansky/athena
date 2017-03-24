#ifndef PFLOWUTILS_PFLOWMERGER_H
#define PFLOWUTILS_PFLOWMERGER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"


class PFlowMerger: public ::AthAlgorithm { 
 public: 
  PFlowMerger( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PFlowMerger(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

  std::vector<std::string> m_inputContainerNames;
  std::string m_outputContainerName;
}; 

#endif //> !PFLOWUTILS_PFLOWMERGER_H
