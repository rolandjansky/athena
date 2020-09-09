/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4Builder.h"
#include "Geo2G4SvcAccessor.h"
#include "G4AtlasInterfaces/Geo2G4SvcBase.h"
#include "Geo2G4OpticalSurfaceFactory.h"

#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"

#include "G4ReflectionFactory.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

#include <map>
#include <iostream>
#include <utility>

Geo2G4Builder::Geo2G4Builder(const std::string& detectorName)
  : m_detectorName(detectorName)
  , m_motherTransform(GeoTrf::Transform3D::Identity())
  , m_matAir(nullptr)
  , m_pDetStore(nullptr)
  , m_msg("Geo2G4Builder")
{
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode sc=svcLocator->service("DetectorStore",m_pDetStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Geo2G4Builder for detector "<<detectorName<<"could not access the detector store - PANIC!!!!");
    abort();
  }

  const GeoModelExperiment* theExpt = nullptr;
  sc = m_pDetStore->retrieve( theExpt, "ATLAS" );
  if(sc.isFailure()){
    ATH_MSG_ERROR("Detector "<< detectorName << "could not get GeoModelExperiment!");
  } else {
    const GeoVDetectorManager *theManager = theExpt->getManager(detectorName);

    for(unsigned int i=0; i<theManager->getNumTreeTops(); i++)
      m_treeTops.push_back(theManager->getTreeTop(i));

    ATH_MSG_INFO("Found detector: top volume(s)");
    for(unsigned int i=0; i<m_treeTops.size();i++)
      ATH_MSG_INFO( "   Tree Top " << i << " " << m_treeTops[i]->getLogVol()->getName() );

    if(m_treeTops.size()>1)
      {
        // -------- -------- MATERIAL MANAGER -------- ----------
        const StoredMaterialManager* theMaterialManager = nullptr;
        sc = m_pDetStore->retrieve(theMaterialManager, "MATERIALS");
        if(sc.isFailure())
          ATH_MSG_ERROR("Detector "<< detectorName << "could not retrieve Material Manager when number of tree tops > 1");
        else
          m_matAir = theMaterialManager->getMaterial("std::Air");
      }

    Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    m_theBuilder=g->GetDefaultBuilder();
    if(m_theBuilder)
      ATH_MSG_INFO("Set volume builder ---> "<< m_theBuilder->GetKey());
    else
      ATH_MSG_WARNING("0 pointer to volume builder."
                      <<"\n Use 'DefaultBuilder' property of Geo2G4Svc or"
                      <<"\n 'GetVolumeBuilder' method of Geo2G4Builder");
  }
}

G4LogicalVolume* Geo2G4Builder::BuildTree()
{
  G4LogicalVolume* result = 0;
  OpticalVolumesMap* optical_volumes = 0;
  const GeoBorderSurfaceContainer* surface_container = 0;

  // Check whether we have to deal with optical surfaces
  if(m_pDetStore->contains<GeoBorderSurfaceContainer>(m_detectorName))
    {
      StatusCode sc = m_pDetStore->retrieve(surface_container,m_detectorName);
      if(sc.isSuccess() && surface_container!=0 && surface_container->size()>0)
        optical_volumes = new OpticalVolumesMap();
    }

  if(m_theBuilder) {
    if(m_treeTops.size()==1) {
      m_motherTransform = m_treeTops[0]->getX();
      result = m_theBuilder->Build(m_treeTops[0],optical_volumes);
    } else {
      // Create temporary GeoModel physical volume
      // The shape is composed by TreeTop shapes + their transforms
      const GeoShape& shFirst = (*(m_treeTops[0]->getLogVol()->getShape()))<<(m_treeTops[0]->getX());
      const GeoShape* shResult = &shFirst;

      for(unsigned int i=1; i<m_treeTops.size(); i++){
        const GeoShape& shNext = (*shResult).add((*(m_treeTops[i]->getLogVol()->getShape()))<<(m_treeTops[i]->getX()));
        shResult = &shNext;
      }

      GeoLogVol* lvEnvelope = new GeoLogVol(m_detectorName,shResult,m_matAir);
      GeoPhysVol* pvEnvelope = new GeoPhysVol(lvEnvelope);

      result = m_theBuilder->Build(pvEnvelope);

      // Get pointer to the World
      PVConstLink world = m_treeTops[0]->getParent();

      // Add all tree tops to the result
      for(unsigned int i=0; i<m_treeTops.size(); i++) {
        // Current Tree Top and its index
        PVConstLink pv = m_treeTops[i];
        Query<unsigned int> childIndx = world->indexOf(pv);

        // Tree Top transformation
        G4Transform3D theG4Position(Amg::EigenTransformToCLHEP(world->getXToChildVol(childIndx)));

        // Copy number
        int id = 16969;
        Query<int> Qint = world->getIdOfChildVol(childIndx);
        if(Qint.isValid()) id = Qint;

        // PV Tree Top name
        std::string nameTT =  world->getNameOfChildVol(childIndx);
        if (nameTT == "ANON") nameTT = pv->getLogVol()->getName();


        G4LogicalVolume* g4LV = m_theBuilder->Build(pv,optical_volumes);
        G4ReflectionFactory::Instance()->Place(theG4Position,
                                               nameTT,
                                               g4LV,
                                               result,
                                               false,
                                               id);
      }

      // Add the temporary physical volume to the GeoModelExperiment
      GeoModelExperiment * theExpt;
      StatusCode sc = m_pDetStore->retrieve(theExpt,"ATLAS");
      if(sc.isFailure())
        ATH_MSG_WARNING("Unable to retrieve GeoModelExperiment. Temporary volume cannot be released");
      else
        theExpt->addTmpVolume(pvEnvelope);
    }
  }

  // build optical surfaces if necessary
  if(optical_volumes!=0 && optical_volumes->size()>0){
    BuildOpticalSurfaces(surface_container,optical_volumes);
  } else if (optical_volumes!=0){
    ATH_MSG_WARNING("Optical volumes apparently requested, but none found!  Deleting temps");
  }
  if (optical_volumes!=0) delete optical_volumes;

  return result;
}


VolumeBuilder*  Geo2G4Builder::GetVolumeBuilder(std::string bname)
{
  Geo2G4SvcAccessor accessor;
  Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();

  m_theBuilder=g->GetVolumeBuilder(std::move(bname));
  return m_theBuilder;
}

void Geo2G4Builder::BuildOpticalSurfaces(const GeoBorderSurfaceContainer* surface_container,
                                         const OpticalVolumesMap* optical_volumes)
{
  Geo2G4OpticalSurfaceFactory surfaceFactory;

  // Iterate over all Border Surfaces in the container
  GeoBorderSurfaceContainer::const_iterator first = surface_container->begin();
  GeoBorderSurfaceContainer::const_iterator last  = surface_container->end();

  for(;first!=last;first++)
    {
      // Build Optical Surface
      const GeoBorderSurface& border_surface = *first;
      G4OpticalSurface* g4OptSurface = surfaceFactory.Build(border_surface.getOptSurface());

      G4VPhysicalVolume* g4PV1 = 0;
      G4VPhysicalVolume* g4PV2 = 0;
      OpticalVolumesMap::const_iterator volIt;

      // First physical volume
      volIt = optical_volumes->find(border_surface.getPV1());
      if(volIt == optical_volumes->end())
        {
          ATH_MSG_WARNING("Unable to find " << border_surface.getPV1()->getLogVol()->getName() << " in Optical Volumes map");
          continue;
        }
      g4PV1 = volIt.operator->()->second;

      // Second physical volume
      volIt = optical_volumes->find(border_surface.getPV2());
      if(volIt == optical_volumes->end())
        {
          ATH_MSG_WARNING("Unable to find " << border_surface.getPV1()->getLogVol()->getName() << " in Optical Volumes map");
          continue;
        }
      g4PV2 = volIt.operator->()->second;

      // G4LogicalBorderSurface
      G4LogicalBorderSurface* g4BorderSurface __attribute__((unused)) = new G4LogicalBorderSurface(border_surface.getName(),g4PV1,g4PV2,g4OptSurface);
    }
}
