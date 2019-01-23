/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"
#include "InDetGeoModelUtils/DistortedMaterialManager.h"
//#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"

#include <string>
#include <sstream>
#include <iostream>

namespace InDetDD {
  ExtraMaterial::ExtraMaterial(IRDBRecordset_ptr xMatTable, const AbsMaterialManager* matManager)
    : m_xMatTable(xMatTable),
    m_matManager(matManager)
  {}

  ExtraMaterial::ExtraMaterial(const DistortedMaterialManager* manager)
    : m_xMatTable(manager->extraMaterialTable()),
    m_matManager(manager->materialManager())
  {}

  void
  ExtraMaterial::add(GeoPhysVol* parent, const std::string& region, double zParent) {
    add(parent, 0, region, zParent);
  }

  void
  ExtraMaterial::add(GeoFullPhysVol* parent, const std::string& region, double zParent) {
    add(0, parent, region, zParent);
  }

  void
  ExtraMaterial::add(GeoPhysVol* parent, GeoFullPhysVol* fullparent, const std::string& region, double zParent) {
    //std::cout << "Adding Extra material for region: " << region << ", zParent = " << zParent << std::endl;

    for (unsigned int i = 0; i < m_xMatTable->size(); i++) {
      std::ostringstream volnamestr;
      volnamestr << "ExtraMaterial" << i;

      //std::cout << "In Extra material " << i << std::endl;

      if ((*m_xMatTable)[i]->getString("REGION") == region) {
        //std::cout << "Extra material Match " << i << std::endl;

        GenericTubeMaker tubeHelper((*m_xMatTable)[i]);
        const GeoMaterial* material = m_matManager->getMaterial(tubeHelper.volData().material());
        const GeoShape* shape = tubeHelper.buildShape();
        GeoLogVol* logVol = new GeoLogVol(volnamestr.str(), shape, material);
        GeoPhysVol* physVol = new GeoPhysVol(logVol);

        if (parent) {
          tubeHelper.placeVolume(parent, physVol, zParent);
        } else {
          tubeHelper.placeVolume(fullparent, physVol, zParent);
        }
      }
    }
  }
} // end namespace
