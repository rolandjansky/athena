/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoBarrelAlpine_H
#define InDet_GeoBarrelAlpine_H

#include "PixelGeoComponent/GeoComponent.h"

namespace InDet {

  class GeoLayerAlpine;
  class GeoStaveRingSupportAlpine;

  class GeoBarrelAlpine : public GeoComponent {
  public:

    GeoBarrelAlpine( GeoFullPhysVol* vol);
    GeoBarrelAlpine( GeoPhysVol* vol);
    GeoBarrelAlpine(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoBarrelAlpine(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

    //Interface extension: access to layers
    virtual const std::vector<GeoLayerAlpine*> layers() const {return m_layers;}

    /// Access to all components (layers and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    // Polimorphic access to layers
    virtual int numActiveComponents() const {return m_layers.size();}

    // Polimorphic access to layers
    virtual const GeoLayerAlpine& activeComponent( int i) const {return *m_layers.at(i);}
    //    virtual GeoLayerAlpine* activeComponent_ptr( int i) {return m_layers.at(i);}

    // InDet::GeoBarrelAlpine*  clone() const;

    void placeLayer( GeoLayerAlpine& layer, const HepGeom::Transform3D& transform, int number);
    void placeSupport( GeoStaveRingSupportAlpine& ring, const HepGeom::Transform3D& transform, int number);

/*     // Get stave outline & radis minmax  (-> used to compute ring support geometry) */
/*     const std::vector<HepGeom::Point3D<double> > getStaveBasisOutline(const std::string& volumeName, int iLayer) const; */
/*     double getStaveRmax(const std::string& volumeName, int iLayer) const; */
/*     double getModuleRmax(const std::string& moduleName, int iLayer) const; */

    InDet::GeoBarrelAlpine* place(const HepGeom::Transform3D& transform, int number) const;

  private:

     std::vector<GeoLayerAlpine*> m_layers;
     std::vector<GeoStaveRingSupportAlpine*> m_supports;

  };

}

#endif
