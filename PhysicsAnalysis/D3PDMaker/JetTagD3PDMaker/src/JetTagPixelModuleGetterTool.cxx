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
  ATH_CHECK( Base::initialize() );
  ATH_CHECK( detStore()->retrieve(m_pixMan, "Pixel") );
  ATH_MSG_INFO( "Pixel manager  retrieved"  );
  return StatusCode::SUCCESS;
}


const InDetDD::SiDetectorElementCollection* JetTagPixelModuleGetterTool::get (bool ){

  return m_pixMan->getDetectorElementCollection();

}


}
