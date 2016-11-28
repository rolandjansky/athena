/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWOBJECTBUILDER_TOOLS_H
#define EFLOWREC_EFLOWOBJECTBUILDER_TOOLS_H

/********************************************************************

NAME:     eflowCaloObjectBuilder.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  24th January, 2005

********************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class eflowISubtractionAlgTool;
class StoreGateSvc;

class eflowCaloObjectBuilder : public AthAlgorithm {

 public:

  eflowCaloObjectBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  ~eflowCaloObjectBuilder();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  private:
  void printTools();

  //Name of eflow cluster Container to write out:
  std::string m_eflowClustersOutputName;

  // Name of eflow calo object Container to read in:  
  std::string m_eflowCaloObjectsName;
  // Name of rec track object Container to read in:
  std::string m_eflowRecTracksName;
  // Name of rec cluster object Container to read in:
  std::string m_eflowRecClustersName;

  StoreGateSvc* m_storeGate;

  ToolHandleArray< eflowISubtractionAlgTool > m_tools;

};
#endif
