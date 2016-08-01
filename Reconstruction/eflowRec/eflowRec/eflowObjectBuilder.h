/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowObjectBuilder.h
 *
 *  Created on: Mar 2, 2015
 *      Author: zhangrui
 */

#ifndef EFLOWOBJECTBUILDER_H_
#define EFLOWOBJECTBUILDER_H_


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <cassert>

class eflowBaseAlgTool;
class StoreGateSvc;

class eflowObjectBuilder : public AthAlgorithm {

 public:

  eflowObjectBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  ~eflowObjectBuilder();

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

#endif /* EFLOWOBJECTBUILDER_H_ */
