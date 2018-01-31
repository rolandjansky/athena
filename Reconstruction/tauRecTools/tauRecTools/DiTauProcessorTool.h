#ifndef TAUREC_DITAUPROCESSORTOOL_H
#define TAUREC_DITAUPROCESSORTOOL_H

#include "tauRecTools/ITauToolExecBase.h"
#include "AsgTools/ToolHandleArray.h"
#include "tauRecTools/IDiTauToolBase.h"


class DiTauProcessorTool : public asg::AsgTool, virtual public ITauToolExecBase {
 public:

  ASG_TOOL_CLASS1( DiTauProcessorTool, ITauToolExecBase )
    
  DiTauProcessorTool(const std::string& type);
  ~DiTauProcessorTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  std::string find_file(const std::string& fname) const;
  std :: string                 m_sDiTauContainerNameHadHad;
  std :: string                 m_sDiTauContainerNameHadEl;
  std :: string                 m_sDiTauContainerNameHadMu;

  ToolHandleArray<tauRecTools::IDiTauToolBase>  m_aVarCalculatorTools;
  
  ToolHandleArray<tauRecTools::IDiTauToolBase>  m_aVarCalculatorToolsHadHad;

  ToolHandleArray<tauRecTools::IDiTauToolBase>  m_aVarCalculatorToolsHadEl;
  
  ToolHandleArray<tauRecTools::IDiTauToolBase>  m_aVarCalculatorToolsHadMu;

  StatusCode processContainer(std::string& diTauContainerName, 
                              ToolHandleArray<tauRecTools::IDiTauToolBase>& varCalculatorTools);
  
  enum DecayChannel{ HadHad, HadEl, HadMu, Default };
};
#endif //TAUREC_DITAUPROCESSORTOOL_H
