/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWOBJECTBUILDER_TOOLS_H
#define EFLOWREC_EFLOWOBJECTBUILDER_TOOLS_H

/********************************************************************

NAME:     eflowObjectBuilder_Tools.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  24th January, 2005

********************************************************************/

#include "eflowRec/eflowBaseAlg.h"
#include "GaudiKernel/ToolHandle.h"

class eflowBaseAlgTool;
class StoreGateSvc;

class eflowObjectBuilder_Tools : public eflowBaseAlg {

 public:

  eflowObjectBuilder_Tools(const std::string& name, ISvcLocator* pSvcLocator);
  ~eflowObjectBuilder_Tools();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  private:
  void printTools();

  //Name of eflow cluster Container to write out:
  std::string m_eflowClustersOutputName;

  // Name of eflow calo object Container to read in:  
  std::string m_eflowCaloObjectsName;

  StoreGateSvc* m_storeGate;

  ToolHandleArray< eflowBaseAlgTool > m_tools;

};
#endif
