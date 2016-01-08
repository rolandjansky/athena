/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoRingAlpine_H
#define InDet_GeoRingAlpine_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoDetModule.h"

#include "GeoModelKernel/GeoMaterial.h"

#include "CLHEP/Geometry/Transform3D.h"

namespace InDet {

  class GeoDetModules;

  class GeoRingAlpine : public GeoComponent {
  public:

    GeoRingAlpine( GeoFullPhysVol* vol);
    GeoRingAlpine( GeoPhysVol* vol);
    GeoRingAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
      
    /// From GeoLayerSector interface
    virtual const std::vector<GeoDetModule*> modules() const;

    /// Extension of the interface. Note the return by const reference instead of return by value
    virtual const std::vector<GeoDetModule*>& ringModules() const {return m_ringModules;}

    /// Access to all components (modules and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    virtual int numActiveComponents() const {return m_ringModules.size();}

    virtual const GeoDetModule& activeComponent( int i) const {
      return *modules().at(i); // FIXME: very inefficient implementation. Check if used!
    }

    virtual void placeRingModule( GeoDetModule& module, const HepGeom::Transform3D& transform, int number,int index, int eta);
    virtual void placeRingSupport( GeoComponent& module, const HepGeom::Transform3D& transform);
    
    InDet::GeoRingAlpine* place(const HepGeom::Transform3D& transform, int number) const;
/*     InDet::GeoRingAlpine*  copyandclone(const HepGeom::Transform3D& transform, int number) const; */
/*     InDet::GeoRingAlpine* copyandclone() const; */
//    void buildComponentTree();

    void computeRingEquivalentCompositeMaterials_all(InDetMaterialManager* matMgr, std::string suffix);
    void computeRingEquivalentCompositeMaterials(InDetMaterialManager* matMgr, GeoComponent* comp, std::string matName);
    void computeStaveEquivalentCompositeMaterials_combination(InDetMaterialManager* matMgr, GeoComponent *comp, std::string matName,
							      std::string name1, std::string name2, double prop1, double prop2);

    const GeoMaterial* findCompositeMaterial(std::string matName) const;
    const GeoMaterial* getRingModuleAndSupportMaterial() const; 
    const GeoMaterial* getRingSupportMaterial() const; 
    void addCompositeMat( std::pair<std::string,const GeoMaterial *>v){ m_compositeMat.insert(v); };
    void addCompositeMatName( std::pair<std::string,std::string> v){ m_compositeMatNames.insert(v); };

    int getModuleType( int iModule) const ;
    double getModuleHalfSizeInPhi( int iModule) const ;
    double getModuleHalfSizeInEta( int iModule) const ;
    const HepGeom::Transform3D& getRingModuleTransform3D( int iModule) const ;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iModule, int iSensor) const;

  private:

    std::map <std::string, std::string > m_compositeMatNames;
    std::map <std::string, const GeoMaterial* > m_compositeMat;

    std::vector<GeoDetModule*> m_ringModules;
    InDet::GeoComponent* m_ringSupport;

  };

  inline const std::vector<GeoDetModule*> GeoRingAlpine::modules() const {
    std::vector<GeoDetModule*> allModules(m_ringModules);
    return allModules;
  }

}

#endif
