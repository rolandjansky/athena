// IAsgTool.h

#ifndef asg_IAsgTool_H
#define asg_IAsgTool_H

// David Adams
// January 2014
//
// Base class for ASG tool interfaces.

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAlgTool.h"
#else
#include "AsgTools/StatusCode.h"
#include "AsgTools/INamed.h"
#endif
#include "AsgTools/AsgToolMacros.h"

namespace asg {

class IAsgTool
#ifdef ASGTOOL_ATHENA
: virtual public IAlgTool
#else
: virtual public INamed
#endif
{

public:

  IAsgTool();

  virtual ~IAsgTool();

#ifndef ASGTOOL_ATHENA
  // Opportunity to retrieve tool handles.
  virtual StatusCode initialize() =0;
#endif

  // Print state.
  virtual void print() const =0;

};

}

#endif
