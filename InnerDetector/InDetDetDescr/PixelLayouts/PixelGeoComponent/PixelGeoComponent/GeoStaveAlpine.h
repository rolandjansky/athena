/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoStaveAlpine_H
#define InDet_GeoStaveAlpine_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoDetModule.h"

#include "GeoModelKernel/GeoMaterial.h"

#include "CLHEP/Geometry/Transform3D.h"

namespace InDet {

  class GeoDetModules;

  class GeoStaveAlpine : public GeoComponent {
  public:

    GeoStaveAlpine( GeoFullPhysVol* vol);
    GeoStaveAlpine( GeoPhysVol* vol);
    //    GeoStaveAlpine(const GeoStaveAlpine& comp,  int seqNum, const HepGeom::Transform3D& trf);
    //    GeoStaveAlpine(const GeoStaveAlpine& comp);

    GeoStaveAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoStaveAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
      
    /// From GeoLayerSector interface
    virtual std::vector<GeoDetModule*> modules() const;

    /// Extension of the interface. Note the return by const reference instead of return by value
    virtual const std::vector<GeoDetModule*>& barrelModules() const {return m_barrelModules;}
    /// the side argument is -1 for negative Z endcap, 1 for positive Z endcap
    virtual const std::vector<GeoDetModule*>& endcapModules( int side) const {
      return (side<0 ? m_negEcModules : m_posEcModules);
    }

    /// Access to all components (modules and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    virtual int numActiveComponents() const {return m_barrelModules.size()+m_negEcModules.size()+m_posEcModules.size();}

    virtual const GeoDetModule& activeComponent( int i) const {
      return *modules().at(i); // FIXME: very inefficient implementation. Check if used!
    }


    /* These methods take as input a GeoDetModule, a HepGeom::Transform3D of the module w.r.t. the stave frame, and the 
     * sequential number of the module in the (barrel or pos/neg endcap) part of the stave.
     * The GeoDetModule is clone()'d, added to the physVolume of the stave with appropriately generated GeoNameTag
     * and GeoIdentifierTag, and the transform. This new clone is added to the corresponding container of modules
     * (m_barrelModules/m_negEcModules/m_posEcModules).
     * The "transition" barrel and endcap modules are treated as normal barrel and endcap modules.
     */
    virtual void placeBarrelModule( GeoDetModule& module, const HepGeom::Transform3D& transform, int number,int index, int eta);
    virtual void placeEndcapModule( GeoDetModule& module, const HepGeom::Transform3D& transform, int number, int side, int index, int eta);

    virtual void placeBarrelStave( GeoComponent& stave, const HepGeom::Transform3D& transform, int number);
    virtual void placeEndcapStave( GeoComponent& stave, const HepGeom::Transform3D& transform, int number, int side);
    virtual void placeFoamModule( GeoComponent& foam, const HepGeom::Transform3D& transform, int number, int side, int index);

    InDet::GeoStaveAlpine* place(const HepGeom::Transform3D& transform, int number) const;
    //    InDet::GeoStaveAlpine*  copyandclone(const HepGeom::Transform3D& transform, int number) const;
    //    InDet::GeoStaveAlpine*  copyandclone() const;
    //    void buildComponentTree();

    /// barrel module length 
    double barrelModuleLength() const;
    int getNbBarrelModules() const { return m_barrelModules.size(); };
    int getNbEndcapModules(int iSide) const { return ((iSide>0)?m_posEcModules.size():m_negEcModules.size()); };
    
    // set stave support sequential number
    void setStaveSupportSequentialNumber(int number);
    //    std::vector<HepGeom::Point3D<double> > staveExternalShape(int iSide) const;

    // Remove stave supports or foam modules
    void removeStaveSupports();
    void removeFoamModules();
    //    void switchFoamModulesGeoPhysVol(GeoPhysVol *vol, std::string type);

    void computeStaveEquivalentCompositeMaterials_all(InDetMaterialManager* matMgr, std::string suffix);
    void computeStaveEquivalentCompositeMaterials(InDetMaterialManager* matMgr, GeoComponent* comp, std::string matName);
    void computeStaveEquivalentCompositeMaterials_combination(InDetMaterialManager* matMgr, GeoComponent *comp, std::string matName,
							      std::string name1, std::string name2, double prop1, double prop2);

    const GeoMaterial* getBarrelModuleMaterial() const;
    const GeoMaterial* getBarrelTransModuleMaterial() const;
    const GeoMaterial* getEndcapModuleMaterial() const;
    const GeoMaterial* getEndcapTransModuleMaterial() const;

    const GeoMaterial* getBarrelStaveMaterial() const;
    const GeoMaterial* getEndcapStaveMaterial(int iSide) const;
    const GeoMaterial* getFoamModuleMaterial() const;
    const GeoMaterial* getTransFoamModuleMaterial() const;

    const GeoMaterial* getBarrelModuleAndStaveMaterial() const;
    const GeoMaterial* getBarrelTransModuleAndStaveMaterial() const;

    const GeoMaterial* getEcModuleAndFoamModuleMaterial() const;
    const GeoMaterial* getEcTransModuleAndFoamModuleMaterial() const;

    const GeoMaterial* findCompositeMaterial(std::string matName) const;

    void addCompositeMat( std::pair<std::string,const GeoMaterial *>v){ m_compositeMat.insert(v); };
    void addCompositeMatName( std::pair<std::string,std::string> v){ m_compositeMatNames.insert(v); };

    int getModuleType(int iSide,int iModule) const;
    int getModuleType(int iModule) const;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iSide, int iModule, int iSensor) const;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iModule, int iSensor) const;

    double getBarrelStaveWidth() const;
    double getBarrelStaveLength() const;
    double getEndcapStaveWidth(int iSide) const;
    double getEndcapStaveLength(int iSide) const;
    const HepGeom::Transform3D& getBarrelStaveTransform3D() const;
    const HepGeom::Transform3D& getEndcapStaveTransform3D(int iSide) const;

  private:
    std::vector<GeoDetModule*> m_barrelModules;
    std::vector<GeoDetModule*> m_negEcModules;
    std::vector<GeoDetModule*> m_posEcModules;
    InDet::GeoComponent* m_barrelStave;
    InDet::GeoComponent* m_negEcStave;
    InDet::GeoComponent* m_posEcStave;

    std::vector<GeoComponent*> m_posFoamModule;
    std::vector<GeoComponent*> m_negFoamModule;
    
    std::map <std::string, const GeoMaterial* > m_compositeMat;
    std::map <std::string,std::string > m_compositeMatNames;

  };

  inline std::vector<GeoDetModule*> GeoStaveAlpine::modules() const {
    std::vector<GeoDetModule*> revert_negEcModules( m_negEcModules.rbegin(), m_negEcModules.rend());
    std::vector<GeoDetModule*> allModules(  revert_negEcModules); // FIXME revert the container achieve increasing Z order?
    allModules.insert( allModules.end(), m_barrelModules.begin(), m_barrelModules.end());
    allModules.insert( allModules.end(), m_posEcModules.begin(), m_posEcModules.end());
    return allModules;
  }

}

#endif
