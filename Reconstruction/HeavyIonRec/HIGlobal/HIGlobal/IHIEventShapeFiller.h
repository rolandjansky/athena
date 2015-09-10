/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __INTERFACE_HIEVENTSHAPEMODIFIER_H__
#define __INTERFACE_HIEVENTSHAPEMODIFIER_H__

#include "AsgTools/IAsgTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include <string>
//class INavigable4MomentumCollection;
//class CaloCellContainer;
#include <NavFourMom/INavigable4MomentumCollection.h>
#include <CaloEvent/CaloCellContainer.h>

class IHIEventShapeFiller : virtual public asg::IAsgTool 
{
   ASG_TOOL_INTERFACE(IHIEventShapeFiller)
public:
   virtual ~IHIEventShapeFiller() {};

   virtual StatusCode InitializeCollection            (xAOD::HIEventShapeContainer *evtShape_        )=0;
   virtual StatusCode FillCollectionFromTowers        (const std::string &m_tower_container_key      )=0;
   virtual StatusCode FillCollectionFromCells         (const std::string &m_cell_container_key       )=0;

   virtual const xAOD::HIEventShapeContainer* GetHIEventShapeContainer()=0;   
   virtual StatusCode  SetNumOrders(int NumOrders)=0;   

  inline std::string GetContainerName() const {return m_output_container_name;};
  inline void SetContainerName(std::string cname) {m_output_container_name=cname;};
private :

  std::string m_output_container_name;

};

#endif
