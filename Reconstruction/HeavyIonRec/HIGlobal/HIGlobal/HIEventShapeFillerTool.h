/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPEMODIFIERBASE_H__
#define __HIEVENTSHAPEMODIFIERBASE_H__

#include "AsgTools/AsgTool.h"
#include "HIGlobal/IHIEventShapeFiller.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include <CaloEvent/CaloCell.h>

class HIEventShapeFillerTool : virtual public asg::AsgTool, virtual public IHIEventShapeFiller
{
   ASG_TOOL_CLASS(HIEventShapeFillerTool,IHIEventShapeFiller)

   enum{
      N_ETA_SLICES  = 100,
      N_CALO_LAYERS =  24,
      N_ORDERS      =   4,
   };
  
   public:
   HIEventShapeFillerTool(const std::string& myname);
  
   virtual StatusCode InitializeCollection();
   virtual StatusCode RecordCollection(const std::string &event_shape_key, const std::string &event_shapeaux_key);
   virtual StatusCode UpdateWithCell(const CaloCell* theCell, float geoWeight);

   virtual const xAOD::HIEventShapeContainer* GetHIEventShapeContainer(){return evtShape;} 
 
   private:
   xAOD::HIEventShapeContainer    *evtShape    ;
   xAOD::HIEventShapeAuxContainer *evtShapeAux ;

};

#endif

