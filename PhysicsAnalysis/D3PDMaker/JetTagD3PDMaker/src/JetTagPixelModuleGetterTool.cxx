/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagPixelModuleGetterTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

JetTagPixelModuleGetterTool::JetTagPixelModuleGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode JetTagPixelModuleGetterTool::initialize()
{
  ATH_CHECK( Base::initialize() );

  ATH_CHECK(m_pixelDetEleCollKey.initialize());

  return StatusCode::SUCCESS;
}


const InDetDD::SiDetectorElementCollection* JetTagPixelModuleGetterTool::get (bool ){
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements{*pixelDetEleHandle};
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_ERROR(m_pixelDetEleCollKey.fullKey() << " is not available.");
  }
  return elements;
}


}
