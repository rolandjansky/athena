// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**  
     @class HIJetSignificanceTool
*/

#ifndef HIJETREC_HIJETSIGNIFICANCETOOL_H
#define HIJETREC_HIJETSIGNIFICANCETOOL_H

#include "JetRec/JetModifierBase.h"
#include "AsgTools/ToolHandle.h"


class HIJetSignificanceTool: public JetModifierBase {
  ASG_TOOL_CLASS0(HIJetSignificanceTool)

public:
  HIJetSignificanceTool(const std::string & name);

  virtual int modifyJet(xAOD::Jet& ) const ;
  
  virtual StatusCode initialize();

 private:


};

#endif
// DoxygenDocumentation 
/*! @class JetLArHVMoment
 @brief JetLArHVMoment


Calculates the proportion of cells in a bad LAr HV area, and gives an information about the energy affected.
(the second moment should change later).

</table>
*/
