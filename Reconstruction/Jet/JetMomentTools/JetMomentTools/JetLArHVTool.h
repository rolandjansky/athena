// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**  
     @class JetLArHVTool
     Calculates calorimeter based variables for jet quality 
     @author Nikola Makovec
     @author P-A Delsart 
     @date (first implementation) October , 2009
     @date (run 2 implementation) February , 2014
*/

#ifndef JETMOMENTTOOLS_JETLARHVTOOL_H
#define JETMOMENTTOOLS_JETLARHVTOOL_H

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"


class JetLArHVTool: public asg::AsgTool,
                    virtual public IJetDecorator{
  ASG_TOOL_CLASS0(JetLArHVTool)

public:
  JetLArHVTool(const std::string & name);

  virtual StatusCode decorate(const xAOD::JetContainer& jets) const;

  virtual StatusCode initialize();

 private:

  std::string m_jetContainerName;
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_fracKey;
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_nCellKey;
};

#endif
// DoxygenDocumentation 
/*! @class JetLArHVMoment
 @brief JetLArHVMoment


Calculates the proportion of cells in a bad LAr HV area, and gives an information about the energy affected.
(the second moment should change later).

</table>
*/
