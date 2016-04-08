#ifndef TRIGGERMENUANALYSIS_JUSTINITTDTALGO_HXX
#define TRIGGERMENUANALYSIS_JUSTINITTDTALGO_HXX
/*
  JustInitTDTAlgo.hxx
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"

class JustInitTDTAlgo : public AthAlgorithm {
public:
  JustInitTDTAlgo(const std::string& name, ISvcLocator* svcloc);
  ~JustInitTDTAlgo();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle<TrigAccessTool> m_trigAccessTool;
};

#endif // TRIGGERMENUANALYSIS_JUSTINITTDTALGO_HXX
