#ifndef HTT_READLOGICALHITS_H
#define HTT_READLOGICALHITS_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigHTTInput/IHTTEventOutputHeaderTool.h"
#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"


class HTTReadLogicalHitsAlg : public AthAlgorithm {
public:
  HTTReadLogicalHitsAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTReadLogicalHitsAlg () = default;
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute()    override;
  

private:

  ToolHandle<IHTTEventOutputHeaderTool> m_readOutputTool {this, "InputTool", "HTTOutputHeaderTool/HTTOutputHeaderTool"};
  
  unsigned int m_event = 0U;
  
};

#endif // HTTREADLOGICALHITS_h
