/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// InDetMaterialVeto.h - Description
//
/* 
   Basic class to produce 2D map of detector material to be used for rejection of secondary vertices, introduced for ITk

   Author: Thomas Strebler
   e-mail: thomas.strebler@cern.ch

-----------------------------------------------------------------------------*/

#ifndef _InDetMaterialVeto_H
#define _InDetMaterialVeto_H

#include "TH2D.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/MsgStream.h"

#include "BeamPipeGeoModel/BeamPipeDetectorManager.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

//------------------------------------------------------------------------
namespace InDet {

  class InDetMaterialVeto: public AthMessaging{

  public:
    InDetMaterialVeto(const BeamPipeDetectorManager* beamPipeMgr,
		      const InDetDD::PixelDetectorManager* pixelManager); 
    std::unique_ptr<TH2D> ITkPixMaterialMap(){ return std::move(m_ITkPixMaterialMap); }

  private:
    std::unique_ptr<TH2D> m_ITkPixMaterialMap;
    
  };

}

#endif
