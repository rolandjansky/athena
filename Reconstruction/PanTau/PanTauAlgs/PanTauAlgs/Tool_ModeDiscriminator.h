/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TOOL_MODEDISCRIMINATOR_H
#define PANTAUALGS_TOOL_MODEDISCRIMINATOR_H

//! ASG
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

//! PanTau
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/ITool_ModeDiscriminator.h"
#include "PanTauAlgs/ITool_InformationStore.h"

//! C++
#include <vector>
#include <string>

#include "MVAUtils/BDT.h"

namespace PanTau {
  class PanTauSeed;
}



namespace PanTau {

  /** @class Tool_ModeDiscriminator
      Tool for PID of Tau Seeds
      @author Sebastian Fleischmann
      @author Christian Limbach <limbach@physik.uni-bonn.de>
  */
  class Tool_ModeDiscriminator : public asg::AsgTool, virtual public PanTau::ITool_ModeDiscriminator {

    ASG_TOOL_CLASS1(Tool_ModeDiscriminator, PanTau::ITool_ModeDiscriminator)

      public:
        
    Tool_ModeDiscriminator(const std::string& name);
    virtual ~Tool_ModeDiscriminator ();
    
    virtual StatusCode initialize();

    virtual double getResponse(PanTau::PanTauSeed* inSeed, bool& isOK) const;
        
  private:
        
    void                                        updateReaderVariables(PanTau::PanTauSeed* inSeed, std::vector<float>& list_BDTVariableValues) const;
        
    std::string                                 m_calib_path; //<! cvmfs folder for data files
    std::string                                 m_Name_InputAlg;
    std::string                                 m_Name_ModeCase;
    ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
    std::string                                 m_Tool_InformationStoreName;
    PanTau::HelperFunctions   m_HelperFunctions;
    std::vector<std::unique_ptr<MVAUtils::BDT> > m_MVABDT_List;
        
    std::vector<double>                         m_BinEdges_Pt;
    std::string                                 m_ReaderOption;
    std::string                                 m_MethodName;
    std::vector<std::string>                    m_List_BDTVariableNames;
    std::vector<double>                         m_List_BDTVariableDefaultValues;

    bool m_init=false;

  public:
    inline bool isInitialized(){return m_init;}
        
  };
} // end of namespace PanTau
#endif // PANTAUALGS_TOOL_MODEDISCRIMINATOR_H
