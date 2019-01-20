/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ForDetEnvelopeFactory.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoShapeShift.h"  
#include "GeoModelKernel/GeoShapeUnion.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"

#include <vector>


ForDetEnvelopeFactory::ForDetEnvelopeFactory(StoreGateSvc *detStore)
  : m_detectorManager(0),
    m_materialManager(0),
    m_detectorStore(detStore)
{
}

ForDetEnvelopeFactory::~ForDetEnvelopeFactory()
{
}

void ForDetEnvelopeFactory::create(GeoPhysVol *world)
{
  const StoredMaterialManager * materialManagerTmp = 0;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManagerTmp, std::string("MATERIALS"))) {
    return; 
  } 
  m_materialManager = materialManagerTmp;
  
  // Get the materials  
  const GeoMaterial* air = m_materialManager->getMaterial("std::Air");

  // Build boolean shape: union of two tubes
  const GeoTube* tube1 = new GeoTube(0.,2000.*Gaudi::Units::mm,188750.*Gaudi::Units::mm);
  GeoTube* tube2 = new GeoTube(0.,2000.*Gaudi::Units::mm,188750.*Gaudi::Units::mm);
  GeoTrf::TranslateZ3D xfRelativeA(211250.*Gaudi::Units::mm);
  GeoTrf::TranslateZ3D xfRelativeC(-211250.*Gaudi::Units::mm);

  const GeoShapeShift& envShape1 = (*tube1)<<xfRelativeA;
  const GeoShapeUnion& envShape = envShape1.add((*tube2)<<xfRelativeC);
  
  GeoLogVol* lvEnv = new GeoLogVol("ForDetEnvelope",&envShape,air);
  GeoPhysVol* pvEnv = new GeoPhysVol(lvEnv);

  GeoNameTag *tag = new GeoNameTag("ForDetEnvelope");
  world->add(tag);
  world->add(pvEnv);

  m_detectorManager=new ForDetEnvelopeManager();
  m_detectorManager->addTreeTop(pvEnv);
}


const ForDetEnvelopeManager * ForDetEnvelopeFactory::getDetectorManager() const
{
  return m_detectorManager;
}

