/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoComponent_H
#define InDet_GeoComponent_H

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"

#include <vector>
#include <map>
#include <string>

class GeoFullPhysVol;
class GeoPhysVol;
class GeoAlignableTransform;
class GeoTransform;
class GeoIdentifierTag;
class GeoNameTag;
class GeoMaterial;
class InDetMaterialManager;
/** Base class for components of the detector geometry.
    Intended for logical structuring of the components relevant for the readout and tracking geometries, 
    like barrel containing layers containing staves containing modules etc.
 */

namespace InDet {

  const int NoNumber=999999;
  
  class GeoComponent {
  public:

    GeoComponent( GeoFullPhysVol* vol);
    GeoComponent( GeoPhysVol* vol);

    GeoComponent(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);
    GeoComponent(GeoPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag);

/*     GeoComponent(const GeoComponent& comp); */
/*     GeoComponent(const GeoComponent& comp,  int seqNum, const HepGeom::Transform3D& trf); */
/*     GeoComponent(const GeoComponent& comp,  int seqNum, const HepGeom::Transform3D& trf, bool insertFlag); */


    virtual ~GeoComponent();
    void UnRef();

    template <class T> T* physVolume() const; 

    virtual const std::vector<GeoComponent*> components() const {return m_components;}
    virtual void deleteAllChildComponents();

/*     virtual void addAlignableComponent( GeoComponent& child); */

    // create a copy of GeComponent with given trf and seq. number
    /*virtual*/ GeoComponent* place(const HepGeom::Transform3D& transform, int number, bool bG4insertFlag=false) const;
    GeoComponent* clone() const;

    // create a copy of child with given trf and seq. number + add it as a child to current GeoComponent
    //   + G4insertFlag = true
    virtual GeoComponent* placeAsChild_insert(const GeoComponent& child, const HepGeom::Transform3D& transform, int number);

    // create a copy of child with given trf and seq. number + add it as a child to current GeoComponent 
    //   + G4InsertFlag = false
    virtual GeoComponent* placeAsChild(const GeoComponent& child, const HepGeom::Transform3D& transform, int number);
  
    
    // FullPhysVol or PhysVol ?
    virtual bool isFullPhysVol() const { return (m_physVol==0); };
    virtual bool isPhysVol() const { return (m_fullPhysVol==0); };

    // Function used to set/get GeoComponent private parameters
    //    HepGeom::Transform3D* getTransform3D() const;

    const HepGeom::Transform3D& getTransform3D() const;
    const HepGeom::Transform3D& getTransform() const;

    virtual const std::string& name() const; 

    int getSeqNumber() const {return m_number;}
    virtual const std::string& getNameTag() const;
    virtual bool getG4InsertFlag() const { return m_bG4InsertFlag; }

    // Lock 
    virtual void lock() { m_bLock=true; }
    virtual bool isLocked() const { return m_bLock; }

    // Material mapping
    virtual void setMaterialAsMapped() { m_bMatMapped=true; }
    virtual bool isMaterialMapped() const { return m_bMatMapped; }

    // get GeoComponent shape/material ( defined in logvol)
    virtual double getShape() const;
    virtual std::string getMaterialName() const;
    virtual const GeoMaterial* getMaterial() const;

    //    // Extract readout geometry from complete geometry
    //    virtual std::vector<GeoComponent*> extractSensorGeometry(const HepGeom::Transform3D& trf,const std::string &sensorFlag) const;

    // Tools to dump/browse GeoComponent object
    virtual void dump(int iLevel=0) const;
    virtual void dumpNames(int iLevel=0) const;
    virtual void dumpAddresses(int iLevel=0) const;
    virtual void showTransform() const;

    // Extract GeoComponent tree
    GeoComponent* extractGeoComponentTree() const;

  protected:

    GeoFullPhysVol*        m_fullPhysVol;
    GeoPhysVol*            m_physVol;
    int                    m_number;
    //    HepGeom::Transform3D*  m_transform3D;
    HepGeom::Transform3D   m_transform3D;
    //    GeoNameTag*            m_nameTag;
    std::string           m_nameTag;
    bool                   m_bG4InsertFlag;

    bool                   m_bLock;
    bool                   m_bMatMapped;
    bool                   m_bOriginalObject;

    std::vector<GeoComponent*> m_components;

    // place l' objet dans la hierachie des (full )physvol et le sauve dans m_components
    void connectComponent(GeoComponent& child);
    void connectComponent(GeoComponent& child, const HepGeom::Transform3D &trf);
    void connectChildComponent( GeoComponent& child);

    void setTransform3D(const HepGeom::Transform3D &trf);

  };

  template<> inline GeoFullPhysVol* InDet::GeoComponent::physVolume<GeoFullPhysVol>() const
    {
      return m_fullPhysVol;
    } 
  
  template<> inline GeoPhysVol* InDet::GeoComponent::physVolume<GeoPhysVol>() const
    {
      return m_physVol;
    } 
  
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( InDet::GeoComponent , 7368888 , 1 )


#endif
