/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagPixelModuleGetterTool.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

JetTagPixelModuleGetterTool::JetTagPixelModuleGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_pixMan(0)
{
}

StatusCode JetTagPixelModuleGetterTool::initialize()
{
  
CHECK( Base::initialize() );

 if(StatusCode::SUCCESS !=detStore()->retrieve(m_pixMan, "Pixel") || m_pixMan==0){
    msg(MSG::FATAL) << "Could not find Pixel manager "<<endreq; 
    return StatusCode::FAILURE; 
  }
  msg(MSG::INFO)  << "Pixel manager  retrieved" << endreq;

 
  return StatusCode::SUCCESS;

}


const InDetDD::SiDetectorElementCollection* JetTagPixelModuleGetterTool::get (bool ){

  return m_pixMan->getDetectorElementCollection();

}


}
