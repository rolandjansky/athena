/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef ServiceDynVolume_H
#define ServiceDynVolume_H

#define _USE_MATH_DEFINES
//#include "InDetServMatGeoModel/LinearService.h"
#include "PixelServicesTool/ServiceDynMaterial.h"

#include <vector>
#include <string>
#include <cmath>

class ServicesDynLayer;

class ServiceDynVolume {
public:

  enum Shape {Cylinder, Disk};
  enum Routing {Z, R, Phi};

  typedef std::vector<const ServicesDynLayer*>   LayerContainer;

 ServiceDynVolume( Shape sh, Routing routing, double rmin, double rmax, double zmin, double zmax, const std::string& name) :
  m_shape(sh), m_routing(routing), m_rMin( rmin), m_rMax(rmax), m_zMin(zmin), m_zMax(zmax), m_name(name),
    m_layers(), m_endingLayer(0), m_previous(0), m_next(0),m_nSectors(1),m_refPhiSector(0.),m_sectorWidth(2*M_PI),m_splitLayersInPhi(false) {}

  /// Add a layer the services of which are routed through this volume
  void addLayer( const ServicesDynLayer* l) {
    m_layers.push_back(l);
  }

  /* don't remember why I wrote this one
  void addLayer( const ServicesDynLayer* l, const ServicesDynLayer* lnext) {
    m_layers.push_back(l);
    m_endingLayer = lnext;
  }
  */

  void addLayers( const LayerContainer& lc) {
    m_layers.insert( m_layers.end(), lc.begin(), lc.end());
  }

  void addEosServices( const ServicesDynLayer* l);
  
  Shape shape() const {return m_shape;}
  Routing routing() const {return m_routing;}
  double zPos() const {return 0.5*(m_zMin+m_zMax);}
  double rMin() const {return m_rMin;}
  double rMax() const {return m_rMax;}
  double zMin() const {return m_zMin;}
  double zMax() const {return m_zMax;}
  double radius() const {return 0.5*(rMin() + rMax());}

  double position() const {
    if (shape() == Cylinder) return zPos();
    else                     return radius();
  }

  double length() const {
    if (routing() == Routing::Z) return zMax()-zMin();
    else if (routing() == Routing::R) return rMax()-rMin();
    else if (routing() == Routing::Phi) return M_PI*(rMax()+rMin())/2.;
    else return 0.;
  }

  double volume() const{
    return M_PI*std::fabs( (zMax()-zMin()) * (rMax()*rMax()-rMin()*rMin()) );
  }

  const std::string& name() const {return m_name;}

  bool contains( double x) const {
    // one-dimensional check, radial for disks and along Z for cylinders)
    double eps = 0.0001; // hard-wired tolerance
    if (shape()==Disk) return (x > rMin()-eps && x < rMax()+eps);
    else               return (x > zMin()-eps && x < zMax()+eps);
  }

  LayerContainer layers() const {return m_layers;}

  ServiceDynVolume* next() const {return m_next;}

  void addPrevious( ServiceDynVolume* prev) { m_previous.push_back(prev);}

  void setNext( ServiceDynVolume* next) { m_next = next;}

  void dump( bool dumpMaterial = true) const;

  const std::vector<ServiceDynMaterial>& materials() const {return m_materials;}

  void setMaterials( const std::vector<ServiceDynMaterial>& mat) {m_materials = mat;}

  void addMaterial( const ServiceDynMaterial& mat) {m_materials.push_back( mat);}

  bool isEOS() const {
    if (m_name.size() < 4) return false;
    if (m_name.substr(m_name.size()-3) == "EOS") return true;
    return false;
  }

  void splitIntoSectors( int nSect, double phiRef, double sectorWidth, bool splitLayersInPhi, int phiStepInSectors) {
    m_nSectors = nSect;
    m_refPhiSector = phiRef;
    m_sectorWidth = sectorWidth;
    m_splitLayersInPhi = splitLayersInPhi;
    m_phiStepInSectors = phiStepInSectors;
  }

  int getNumSectors() const {return m_nSectors;}
  float getRefPhiSector() const {return m_refPhiSector; }
  float getSectorWidth() const {return m_sectorWidth; }
  bool splitLayersInPhi() const {return m_splitLayersInPhi; }
  int getPhiStep() const {return m_phiStepInSectors;}

private:

  Shape m_shape;
  Routing m_routing;
  double m_rMin;
  double m_rMax;
  double m_zMin;
  double m_zMax;
  std::string m_name;

  LayerContainer m_layers;  // All layers the services of which are routed through this volume

  const ServicesDynLayer* m_endingLayer;
  std::vector<ServiceDynVolume*> m_previous;
  ServiceDynVolume* m_next;

  std::vector<ServiceDynMaterial> m_materials;

  int m_nSectors;
  float  m_refPhiSector;
  float  m_sectorWidth;
  bool m_splitLayersInPhi; 
  int m_phiStepInSectors;

};

#endif
