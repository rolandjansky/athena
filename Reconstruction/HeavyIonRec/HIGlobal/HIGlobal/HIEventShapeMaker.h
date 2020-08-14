/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPEMAKER_H__
#define __HIEVENTSHAPEMAKER_H__

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "IHIEventShapeFiller.h"
#include "HIEventUtils/HIEventShapeSummaryTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class CaloCellContainer;

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
  SG::ReadHandleKey<xAOD::CaloClusterContainer>   m_towerContainerKey { this, "InputTowerKey"     , "CombinedTower"   , "InputTowerKey"}; //TowerContainer key
  SG::ReadHandleKey<CaloCellContainer>            m_cellContainerKey  { this, "InputCellKey"      , "AllCalo"         , "InputCellKey" }; //CaloCellContainer key
  SG::ReadHandleKey<xAOD::HIEventShapeContainer>  m_readExistingKey   { this, "ReadExistingKey", "HIEventShapeContainer", "Read key to retrieve the shape if m_summary_only is true"};
  SG::WriteHandleKey<xAOD::HIEventShapeContainer> m_outputKey         { this, "OutputContainerKey"    , "HIEventShapeContainer", "Output Container Key"}; //Name of output container key
  SG::WriteHandleKey<xAOD::HIEventShapeContainer>  m_summaryKey       { this, "SummaryContainerKey"   , "", "SummaryContainerKey" };

  //use cells instead of towers to fill the EventShape
  Gaudi::Property< bool > m_useCaloCell { this, "UseCaloCell", false, "Use cells instead of towers to fill the EventShape" }  ;
  //The number of Orders of harmonic flow to store in the EventShape
  Gaudi::Property< int > m_numOrders { this, "OrderOfFlowHarmonics", 7, "The number of Orders of harmonic flow to store in the EventShape" }  ;
  Gaudi::Property< bool > m_summaryOnly { this, "SummaryOnly", false, "Summary Only boolean switch" }  ;

  ToolHandle<IHIEventShapeFiller> m_HIEventShapeFillerTool { this, "HIEventShapeFillerTool", "HIEventShapeFillerTool", "HIEventShapeFillerTool" };
  ToolHandle<IHIEventShapeSummaryTool> m_summaryTool      { this, "SummaryTool", "HIEventShapeSummaryTool", "Handle to IHIEventShapeSummaryTool" };

  void PrintHIEventShapeContainer(const xAOD::HIEventShapeContainer *Container);
};

#endif
