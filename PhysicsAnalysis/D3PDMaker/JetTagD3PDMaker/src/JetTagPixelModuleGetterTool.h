/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagPixelModuleGetterTool.h
 * @author Georges Aad
 * @date Jun, 2011
 * @brief pixel module getter to dump pixel conditions
 * 
 */


#ifndef JetTagD3PDMaker_JetTagPixelModuleGetterTool_H
#define JetTagD3PDMaker_JetTagPixelModuleGetterTool_H

#include "D3PDMakerUtils/ObjGetterTool.h" 
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"


namespace InDetDD{
  class PixelDetectorManager;
}

namespace D3PD { 

class  JetTagPixelModuleGetterTool
  : public ObjGetterTool<InDetDD::SiDetectorElementCollection>
{ 
 public: 
  typedef ObjGetterTool<InDetDD::SiDetectorElementCollection> Base; 
   JetTagPixelModuleGetterTool(const std::string& type, 
				    const std::string& name, 
				    const IInterface* parent); 

  virtual StatusCode initialize();
  virtual const InDetDD::SiDetectorElementCollection* get (bool allowMissing=false);
 
  
 private: 

  const InDetDD::PixelDetectorManager* m_pixMan; 
 
}; 

} 

#endif // JetTagD3PDMaker_JetTagPixelModuleGetterTool_H
