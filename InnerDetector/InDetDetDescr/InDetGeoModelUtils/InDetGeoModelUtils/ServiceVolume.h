/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDD_ServiceVolume_H
#define InDetDD_ServiceVolume_H

#include <string>
#include <cmath>

class GeoMaterial;
class GeoShape;

namespace InDetDD {

  // class to hold GeoShape pointer. This takes care of the GeoShape ref counting and 
  // avoids needing to write a copy constructor for ServiceVolume
  class GeoShapeHolder {
  public:
    GeoShapeHolder(); 
    GeoShapeHolder(const GeoShape *); 
    GeoShapeHolder(const GeoShapeHolder &);
    GeoShapeHolder & operator=(const GeoShapeHolder &);
    GeoShapeHolder & operator=(GeoShapeHolder &&);
    ~GeoShapeHolder(); 
    const GeoShape * get() const {return m_geoShape;}
    void set(const GeoShape *);
    void reset();
  private:
    const GeoShape * m_geoShape;
  };
  
  class ServiceVolume {

  public: 
    ServiceVolume();
    void setRmin(double rmin) {m_rmin = rmin; resetGeoShape();} 
    void setRmax(double rmax) {m_rmax = rmax; resetGeoShape();} 
    void setRmin2(double rmin2) {m_rmin2 = rmin2; resetGeoShape();} 
    void setRmax2(double rmax2) {m_rmax2 = rmax2; resetGeoShape();} 
    void setZmin(double zmin) {m_zmin = zmin; resetGeoShape();} 
    void setZmax(double zmax) {m_zmax = zmax; resetGeoShape();} 
    //void setVolId(int num) {m_volId = num;}
    void setZsymm(bool zsymm) {m_zsymm = zsymm;}
    void setMaterial(const std::string & mat) {m_materialName = mat;} 
    void setMaterial(const GeoMaterial * mat) {m_material = mat;} 
    void setVolName(const std::string & name) {m_volName = name;}
    void setShapeType(const std::string & shapeType) {m_shapeType = shapeType; setSplittable();}
    void setPhiLoc(double phiLoc) {m_phiLoc = phiLoc; resetGeoShape();}
    void setPhiWidth(double phiWidth) {m_phiWidth = phiWidth; resetGeoShape();}
    void setNeedsRotation(bool flag) {m_needsRotation = flag;}
    void setSides(int sides) {m_sides = sides; resetGeoShape();}
    void setNCopies(int num) {m_nCopies = num;}
    void setRegion(const std::string & region) {m_region = region;}
    void setLabel(const std::string & name, int volId);
    void setLabel(const std::string & name) {m_label = name;}
    void addLabel(const std::string & name) {m_label += name;}
    //void setLength() {m_origLength = std::abs(m_zmax - m_zmin);}
    void setOrigVolume(double volume) {m_origVolume = volume;}
    void resetGeoShape() {if (!m_lockGeoShape) m_geoShape.reset();}
    void reduceSize(double safety);
    // Set a custom shape. A volume can be specified. 
    // This should be used when the shapes volume calculation is unreliable (eg boolean volumes)
    // NB. rmin,rmax,zmin,zmax should still be set to determine z position and to determine which zone it belongs.
    void setGeoShape(const GeoShape * geoShape, double volume=0);
    void setEnvelopeNum(int num) { m_envNum=num; }
    void setParentEnvelope(int num) { m_envParentNum=num; }
    void setZShift(double shift) { m_zShift=shift; }

    double rmin() const {return m_rmin;}
    double rmax() const {return m_rmax;}
    double rmin2() const {return m_rmin2;}
    double rmax2() const {return m_rmax2;}
    double zmin() const {return m_zmin+m_safety;}
    double zmax() const {return m_zmax-m_safety;}
    //int volId() const {return m_volId;}
    bool zsymm() const {return m_zsymm;}
    const std::string & materialName() const {return m_materialName;}
    const GeoMaterial * material() const {return m_material;}
    const std::string & volName() const {return m_volName;}
    const std::string & shapeType() const {return m_shapeType;}
    double phiLoc() const {return m_phiLoc;}
    double phiWidth() const {return m_phiWidth;}
    bool needsRotation() const {return m_needsRotation;}
    int sides() const {return m_sides;}
    int nCopies() const {return m_nCopies;}
    //double origLength() const {return m_origLength;}
    const std::string & region() const {return m_region;}
    std::string fullLabel() const;
    double length() const {return std::abs(m_zmax - m_zmin) - 2*m_safety;}
    double zposition() const {return 0.5 * (m_zmin + m_zmax);}
    double rposition() const {return 0.5 * (m_rmin + m_rmax);}
    //double fractionInRegion() const {return std::abs(m_zmax - m_zmin)/m_origLength;}
    double volume() const;
    double origVolume() const;
    const GeoShape * getShape() const;
    bool splittableInR() const {return m_splittableR;}
    bool splittableInZ() const {return m_splittableZ;}
    int envelopeNum() const {return m_envNum; }
    int envelopeParent() const {return m_envParentNum; }
    double zShift() const {return m_zShift; }

    // For debugging
    void print() const;
    
  private:
    void setSplittable();

    double m_rmin;
    double m_rmax;
    double m_rmin2;
    double m_rmax2;
    double m_zmin;
    double m_zmax;
    //int m_volId;
    bool m_zsymm;
    mutable GeoShapeHolder m_geoShape;
    const GeoMaterial * m_material;
    std::string m_materialName;
    std::string m_volName;
    std::string m_shapeType;
    double m_phiLoc;
    double m_phiWidth;
    bool m_needsRotation;
    int m_sides;
    int m_nCopies;
    //double m_origLength;
    double m_origVolume;
    mutable double m_volume;
    double m_safety;
    std::string m_region;
    std::string m_label;
    bool m_lockGeoShape;
    bool m_splittableR;
    bool m_splittableZ;

    int m_envNum;
    int m_envParentNum; 
    double m_zShift;
  };
  
} // End namespace 
    

#endif
