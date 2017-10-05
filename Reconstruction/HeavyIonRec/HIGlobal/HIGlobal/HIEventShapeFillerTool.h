/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIGLOBAL_HIEVENTSHAPEFILLERTOOL_H
#define HIGLOBAL_HIEVENTSHAPEFILLERTOOL_H

#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "HIGlobal/IHIEventShapeFiller.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HITowerWeightTool.h"
#include <NavFourMom/INavigable4MomentumCollection.h>

class CaloCellContainer;


class HIEventShapeFillerTool : public asg::AsgTool, virtual public IHIEventShapeFiller
{
   ASG_TOOL_CLASS(HIEventShapeFillerTool,IHIEventShapeFiller)
  
   public:
   HIEventShapeFillerTool(const std::string& myname);
  
   virtual StatusCode InitializeCollection            (xAOD::HIEventShapeContainer *evtShape_        );
   virtual StatusCode FillCollectionFromTowers        (const std::string &tower_container_key      );
   virtual StatusCode FillCollectionFromCells         (const std::string &cell_container_key       );

  virtual StatusCode FillCollectionFromTowerContainer(const INavigable4MomentumCollection* navInColl);
  virtual StatusCode FillCollectionFromCellContainer(const CaloCellContainer* CellContainer);

   virtual const xAOD::HIEventShapeContainer* GetHIEventShapeContainer()  {return m_evtShape;} 
   virtual StatusCode  SetNumOrders(int Norders);   

  virtual StatusCode FillCollectionFromClusterContainer(const xAOD::CaloClusterContainer* theClusters);
   private:
   xAOD::HIEventShapeContainer *m_evtShape      ;
   int m_NumOrders;// The number of flow harmonics to store in the Q-vectors
   const HIEventShapeIndex* m_index;
  void UpdateShape(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg=false) const;

  ToolHandle<IHITowerWeightTool> m_towerWeightTool;
  bool m_useClusters;
};

#endif

