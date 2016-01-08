/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoEnvelopeAlpine_H
#define GeoEnvelopeAlpine_H

#include "PixelGeoComponent/GeoComponent.h"

class GeoFullPhysVol;
class GeoPhysVol;
class InDetMaterialManager;

namespace InDet {

/*   class GeoBarrel; */
/*   class GeoEndcap; */
  class GeoBarrelAlpine;
  class GeoEndcapAlpine;

  class GeoEnvelopeAlpine : public GeoComponent {
  public:

    GeoEnvelopeAlpine( GeoFullPhysVol* vol);
    GeoEnvelopeAlpine( GeoPhysVol* vol);
    GeoEnvelopeAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoEnvelopeAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

    /// Interface extension: access to layers
/*     virtual GeoBarrel* barrel() const {return m_barrel;} */
    virtual GeoBarrelAlpine* barrelAlpine() const {return m_barrelAlpine;}

/*     virtual std::vector<GeoEndcap*> endcaps() const {return m_endcaps;} */
    virtual std::vector<GeoEndcapAlpine*> endcapsAlpine() const;

    /// Access to all components (barrel, endcaps and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    /// Polimorphic access to barrel an endcaps
      virtual int numActiveComponents() const {return (m_posEndcapAlpine!=0)+(m_negEndcapAlpine!=0) + (m_barrelAlpine == 0 ? 0 : 1);}

    /// Polimorphic access to layers
    virtual const GeoComponent& activeComponent( int i) const;

/*     void placeBarrel( GeoBarrel& barrel, const HepGeom::Transform3D& transform, int number); */
    void placeBarrelAlpine( GeoBarrelAlpine& barrel, const HepGeom::Transform3D& transform, int number);
    void placeEndcapAlpine( GeoEndcapAlpine& endcap, const HepGeom::Transform3D& transform, int number, int iSide);
    //    virtual void buildComponentTree();

    GeoEnvelopeAlpine*  copyandclone(const HepGeom::Transform3D& transform, int number) const;
    GeoEnvelopeAlpine* copyandclone() const;

    //    // Extract sensor geometry from complete geometry
    //    InDet::GeoEnvelopeAlpine* extractSensorGeometry_Envelope() const;

    //    // Replace stave set by cylinders of composite material
    //    InDet::GeoEnvelopeAlpine* simplifyStaveGeometry(const std::string& compositeMatName, InDetMaterialManager* matMgr) const;

  private:

/*     GeoBarrel*                 m_barrel; */
/*     std::vector<GeoEndcap*>    m_endcaps; */

    GeoBarrelAlpine*                 m_barrelAlpine;
    GeoEndcapAlpine*                 m_negEndcapAlpine;
    GeoEndcapAlpine*                 m_posEndcapAlpine;

  };

}

#endif
