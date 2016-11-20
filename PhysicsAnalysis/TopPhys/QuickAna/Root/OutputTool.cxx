/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE

// Local includes
#include <QuickAna/OutputTool.h>

#include <QuickAna/MessageCheck.h>

namespace ana
{

  OutputCopyTool::OutputCopyTool(const std::string& name,
                                 const std::string& contName)
    : asg::AsgTool(name), m_contName(contName)
  {
    declareProperty("ContainerName", m_contName);
  }


  StatusCode OutputCopyTool::initialize()
  {
    ATH_MSG_WARNING("This tool is deprecated. Please move to the new " <<
                    "OutputToolXAOD and MasterOutputToolXAOD.");
    return StatusCode::SUCCESS;
  }


  StatusCode OutputCopyTool::write(xAOD::TEvent* outputStore,
                                   const std::string& /*suffix*/)
  {
    ANA_CHECK( outputStore->copy(m_contName) );
    return StatusCode::SUCCESS;
  }

}

#endif
