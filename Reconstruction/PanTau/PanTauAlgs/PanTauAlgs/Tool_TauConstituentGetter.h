/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_TAUCONSTITUENTGETTER
#define PANTAUALGS_TOOL_TAUCONSTITUENTGETTER

#include <map>
#include <vector>
#include <string>

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/ITool_InformationStore.h"
#include "PanTauAlgs/ITool_TauConstituentGetter.h"
#include "PanTauAlgs/ITool_InputConverter.h"

#include "xAODTau/TauJet.h"

namespace PanTau {
  class TauConstituent;
}


namespace PanTau {

  class Tool_TauConstituentGetter : public asg::AsgTool, virtual public PanTau::ITool_TauConstituentGetter  {
        
    ASG_TOOL_CLASS1(Tool_TauConstituentGetter, PanTau::ITool_TauConstituentGetter)

      public:
        
    Tool_TauConstituentGetter(const std::string &name);
    virtual ~Tool_TauConstituentGetter ();
        
    virtual StatusCode initialize();
        
    virtual StatusCode GetTauConstituents(const xAOD::TauJet* tauJet,
					  std::vector<TauConstituent*>& outputList,
					  const std::string& algName) const;
        
        
  protected:
        
    //member variables 
    PanTau::HelperFunctions   m_HelperFunctions;
    ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
    ToolHandle<PanTau::ITool_InputConverter>    m_Tool_InputConverter;

    std::string m_Tool_InformationStoreName;
    std::string m_Tool_InputConverterName;

    bool m_init=false;

  public:
    inline bool isInitialized(){return m_init;}
        
  }; //end class ConstituentGetter

}//end namespace PanTau


#endif // PANTAUALGS_TOOL_TAUCONSTITUENTGETTER
