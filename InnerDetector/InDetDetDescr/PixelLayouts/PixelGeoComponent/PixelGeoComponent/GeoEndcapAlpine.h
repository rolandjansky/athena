/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoEndcapAlpine_H
#define InDet_GeoEndcapAlpine_H

#include "PixelGeoComponent/GeoComponent.h"

namespace InDet {

  class GeoRingAlpine;
  //  class GeoRingSupportAlpine;

  class GeoEndcapAlpine : public GeoComponent {
  public:

    GeoEndcapAlpine( GeoFullPhysVol* vol);
    GeoEndcapAlpine( GeoPhysVol* vol);
    GeoEndcapAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

    //Interface extension: access to rings
    virtual std::vector<GeoRingAlpine*> rings() const {return m_rings;}

    /// Access to all components (rings and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    // Polimorphic access to rings
    virtual int numActiveComponents() const {return m_rings.size();}

    // Polimorphic access to rings
    virtual const GeoRingAlpine& activeComponent( int i) const {return *m_rings.at(i);}
    //    virtual GeoRingAlpine* activeComponent_ptr( int i) const {return m_rings.at(i);}

    // InDet::GeoEndcapAlpine*  clone() const;

    void placeRing( GeoRingAlpine& ring, const HepGeom::Transform3D& transform, int number);

    // Radius of centers of sensors active volumes. In case not all sensors are at the same radius
    // the result is layer-type dependent
    double radius() const;
    double radiusMin() const;
    double radiusMax() const;
    std::vector<double> getZposFront() const;
    std::vector<double> getZposBack() const;
   
    int getNbRingModules(int iRing) const;
    int getNbRingModulesInPhi(int iRing) const;
    int getModuleType(int iRing, int iModule) const ;
    double getModuleHalfSizeInPhi(int iRing, int iModule) const ;
    double getModuleHalfSizeInEta(int iRing, int iModule) const ;

    const HepGeom::Transform3D& getRingModuleTransform3D(int iRing, int iModule) const ;
    std::string getRingModuleName(int iRing, int iModule) const ;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iRing,int iModule) const;

    const GeoMaterial* getRingModuleAndSupportMaterial(int) const; 
    const GeoMaterial* getRingSupportMaterial(int) const; 

    InDet::GeoEndcapAlpine*  place(const HepGeom::Transform3D& transform, int number) const;

  private:

     std::vector<GeoRingAlpine*> m_rings;

  };

}

#endif
