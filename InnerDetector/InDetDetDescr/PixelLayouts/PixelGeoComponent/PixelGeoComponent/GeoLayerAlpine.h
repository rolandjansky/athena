/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoLayerAlpine_H
#define InDet_GeoLayerAlpine_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoComponentHelper.h"
/*#include "PixelGeoComponent/GeoLayerSector.h"*/
#include "PixelGeoComponent/GeoStaveAlpine.h"

class GeoFullPhysVol;
class GeoPhysVol;
class GeoBox;

namespace InDet {
  
  class GeoStaveAlpine;
  
  class GeoLayerAlpine : public GeoComponent {
    
    //  class GeoLayerAlpine : public GeoLayerSector {
  public:
    
    GeoLayerAlpine( GeoFullPhysVol* vol);
    GeoLayerAlpine( GeoPhysVol* vol);
    GeoLayerAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoLayerAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

    /// Interface extension: access to layer active components (staves, etc.
    virtual std::vector<GeoStaveAlpine*> sectors() const {return m_layerStaves;}
	
    /// Access to all components (layers and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;
      
    /// Polymorphic access to sectors
    virtual int numActiveComponents() const {return m_layerStaves.size();}

    /// Polymorphic access to sectors
    virtual const GeoStaveAlpine& activeComponent( int i) const {return *m_layerStaves.at(i);}
    //  virtual GeoComponent& activeComponent( int i) {return *m_layerStaves.at(i);}

    /// Radius of centers of sensors active volumes. In case not all sensors are at the same radius
    /// the result is layer-type dependent
    double radius(bool bBarrel, bool bEndcap) const;
    double radiusMin(bool bBarrel, bool bEndcap) const;
    double radiusMax(bool bBarrel, bool bEndcap) const;
    MinMaxHelper getBarrelModuleZMinMax(int iStave, int iModule) const;
    MinMaxHelper getEndcapModuleZMinMax(int iStave, int iModule, int iSide) const;
    int getNbBarrelModules(int iStave=0) const;
    int getNbEndcapModules(int iSide, int iStave=0) const;
    const GeoFullPhysVol* getGeoFullPhysVol(int iStave, int iModule) const;
    double getModuleHalfSizeInPhi(int iStave,int iModule,int iSide) const;
    double getModuleHalfSizeInEta(int iStave,int iModule,int iSide) const;
    const HepGeom::Transform3D& getBarrelModuleTransform3D(int iStave,int iModule) const;
    const HepGeom::Transform3D& getEndcapModuleTransform3D(int iStave,int iModule, int iSide) const;
    std::string getBarrelModuleName(int iStave,int iModule) const;
    std::string getEndcapModuleName(int iStave,int iModule, int iSide) const;
    
    // half length of the active part of the layer
    double activeHalfLength(bool bBarrel, bool bEndcap, int iStave, int iSide=0) const;
    MinMaxHelper activeLength_minmax(bool bBarrel, bool bEndcap, int iStave, int iSide=0) const;
    
    InDet::GeoLayerAlpine*  place(const HepGeom::Transform3D& transform, int number) const;

    virtual void placeStave( GeoStaveAlpine& stave, const HepGeom::Transform3D& transform, int number);
    virtual void removeAllStaveSupports();
    virtual void removeAllFoamModules();
    //    virtual void switchAllFoamModulesGeoPhysVol(GeoPhysVol *vol, std::string type);

    /// barrel module length 
    double barrelModuleLength(int iSector=0) const;
    
    /// stave length 
    double staveLength(int iSector=0) const;
    
    /// stave basis outline
    unsigned int getStaveNumber() const { return m_layerStaves.size(); }

    const GeoMaterial* getBarrelModuleMaterial() const { return m_layerStaves[0]->getBarrelModuleMaterial(); }
    const GeoMaterial* getBarrelTransModuleMaterial() const { return m_layerStaves[0]->getBarrelTransModuleMaterial(); }
    const GeoMaterial* getEndcapModuleMaterial() const { return m_layerStaves[0]->getEndcapModuleMaterial(); }
    const GeoMaterial* getEndcapTransModuleMaterial() const {  return m_layerStaves[0]->getEndcapTransModuleMaterial(); }

    const GeoMaterial* getBarrelStaveMaterial() const { return m_layerStaves[0]->getBarrelStaveMaterial(); }
    const GeoMaterial* getEndcapStaveMaterial(int iSide) const { return  m_layerStaves[0]->getEndcapStaveMaterial(iSide); }
    const GeoMaterial* getFoamModuleMaterial() const { return  m_layerStaves[0]->getFoamModuleMaterial(); }
    const GeoMaterial* getTransFoamModuleMaterial() const { return  m_layerStaves[0]->getTransFoamModuleMaterial(); }

    const GeoMaterial* getBarrelModuleAndStaveMaterial() const { return m_layerStaves[0]->getBarrelModuleAndStaveMaterial(); }
    const GeoMaterial* getBarrelTransModuleAndStaveMaterial() const { return m_layerStaves[0]->getBarrelTransModuleAndStaveMaterial(); }
    const GeoMaterial* getEcModuleAndFoamModuleMaterial() const { return m_layerStaves[0]->getEcModuleAndFoamModuleMaterial(); }
    const GeoMaterial* getEcTransModuleAndFoamModuleMaterial() const { return m_layerStaves[0]->getEcTransModuleAndFoamModuleMaterial(); }

    
    int getModuleType(int iStave, int iSide, int iModule) const;
    int getModuleType(int iStave, int iModule) const;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iStave, int iSide, int iModule, int iSensor=0) const;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iStave, int iModule, int iSensor=0) const;

    double getBarrelStaveWidth(int iStave) const;
    double getBarrelStaveLength(int iStave) const;
    double getEndcapStaveWidth(int iStave, int iSide) const;
    double getEndcapStaveLength(int iStave, int iSide) const;
    const HepGeom::Transform3D& getBarrelStaveTransform3D(int iStave) const;
    const HepGeom::Transform3D& getEndcapStaveTransform3D(int iStave, int iSide) const;

  private:

    std::vector<GeoStaveAlpine*> m_layerStaves;
  };

}

#endif
