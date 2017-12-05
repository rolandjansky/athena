/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPEMAKER_H__
#define __HIEVENTSHAPEMAKER_H__

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODHIEvent/HIEventShapeContainer.h"
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "IHIEventShapeFiller.h"
#include "HIEventUtils/HIEventShapeSummaryTool.h"

class HIEventShapeMaker : public AthAlgorithm
{
public:

  /** Standard Athena-Algorithm Constructor */
  HIEventShapeMaker(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default Destructor */
  ~HIEventShapeMaker() {};


  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  std::string m_tower_container_key;//TowerContainer key
  std::string m_cell_container_key ;//CaloCellContainer key
  std::string m_output_key         ;//Name of output container key
  bool        m_use_calo_cell      ;//use cells instead of towers to fill the EventShape
  int         m_NumOrders          ;//The number of Orders of harmonic flow to store in the EventShape
  std::string m_summary_key;
  bool m_summary_only;

  ToolHandle<IHIEventShapeFiller> m_HIEventShapeFillerTool;
  ToolHandle<IHIEventShapeSummaryTool> m_summary_tool;
  void PrintHIEventShapeContainer(const xAOD::HIEventShapeContainer *Container);
};

#endif
