// AsgTool_Athena.cxx

#include "AsgTools/AsgToolsConf.h"

#ifndef ASGTOOL_ATHENA
#error("Only for use with Athena")
#endif

// Make the private members of AlgTool public so
// we can set the type.
#define private public
#include "GaudiKernel/AlgTool.h"
#undef private

#include "AsgTools/AsgTool.h"

void asg::AsgTool::setAlgToolType(const std::string& type) {
  m_type = type;
}
