#ifndef egammaValidation_ArtTest_H
#define egammaValidation_ArtTest_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include <string>


class ArtTest : public AthAlgorithm
{
 public:
 
  ArtTest (const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize ();
  virtual StatusCode beginInputFile();
  virtual StatusCode firstExecute();
  virtual StatusCode execute ();
  virtual StatusCode finalize ();

 private:
  
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  
};

#endif
