/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __INTERFACE_HIEVENTSHAPEMODIFIER_H__
#define __INTERFACE_HIEVENTSHAPEMODIFIER_H__

#include "AsgTools/IAsgTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include <string>
#include <NavFourMom/INavigable4MomentumCollection.h>
#include <CaloEvent/CaloCellContainer.h>

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include <iostream>
#include <iomanip>

class CaloCellContainer;

class IHIEventShapeFiller : virtual public asg::IAsgTool
{
   ASG_TOOL_INTERFACE(IHIEventShapeFiller)
public:
   virtual ~IHIEventShapeFiller() {};

   virtual StatusCode initializeCollection            (xAOD::HIEventShapeContainer *evtShape_        )=0;
   virtual StatusCode fillCollectionFromTowers        (const SG::ReadHandleKey<xAOD::CaloClusterContainer> &m_tower_container_key      )=0;
   virtual StatusCode fillCollectionFromCells         (const SG::ReadHandleKey<CaloCellContainer> &m_cell_container_key                )=0;

   virtual const xAOD::HIEventShapeContainer* getHIEventShapeContainer() const =0;
   virtual StatusCode  setNumOrders(int NumOrders)=0;

   inline std::string getContainerName() const { return m_outputContainerName; };
   inline void setContainerName(std::string cname) { m_outputContainerName=cname; };

private :

  std::string m_outputContainerName;

};

#endif
