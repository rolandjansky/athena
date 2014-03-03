// AsgExampleAlgorithm.h 

#ifndef AsgExampleAlgorithm_H
#define AsgExampleAlgorithm_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IAsgHelloTool;
class IExecuteTool;

class AsgExampleAlgorithm : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  AsgExampleAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~AsgExampleAlgorithm(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  AsgExampleAlgorithm();

private:

  /// Athena configured tools
  ToolHandleArray<IAsgHelloTool> m_asgtools;

}; 

#endif
