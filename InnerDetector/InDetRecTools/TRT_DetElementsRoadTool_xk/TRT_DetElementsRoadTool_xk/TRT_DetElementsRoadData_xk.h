/*                                                                                                                                          
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                                                    
*/

#include "TrkSurfaces/CylinderBounds.h"
#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsLayerVectors_xk.h"

#ifndef TRT_DetElementsRoadData_xk_H
#define TRT_DetElementsRoadData_xk_H

namespace InDet{

 class TRT_DetElementsRoadData_xk{
   public:

   TRT_DetElementsRoadData_xk()=default;
   ~TRT_DetElementsRoadData_xk()=default;

   inline void setTRTLayerVectors(TRT_DetElementsLayerVectors_xk layers){m_TRTLayerVectors=std::move(layers);}
   inline void setBounds(Trk::CylinderBounds cbounds, double rmintrt){m_bounds=cbounds;m_rminTRT=rmintrt;}

   inline const TRT_DetElementsLayerVectors_xk* getLayers() const {return &m_TRTLayerVectors;}
   inline const Trk::CylinderBounds getBounds() const {return m_bounds;}
   inline double getTRTMinR() const {return m_rminTRT;}

   private: 

   TRT_DetElementsLayerVectors_xk m_TRTLayerVectors;
   double                         m_rminTRT{};
   Trk::CylinderBounds            m_bounds;
 };

}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::TRT_DetElementsRoadData_xk , 151597572 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDet::TRT_DetElementsRoadData_xk , 991597572 );

#endif
