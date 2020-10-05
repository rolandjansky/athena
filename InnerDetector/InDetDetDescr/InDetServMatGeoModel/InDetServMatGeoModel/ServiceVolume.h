/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServiceVolume_H
#define ServiceVolume_H

//#include "InDetServMatGeoModel/LinearService.h"
#include "InDetServMatGeoModel/ServiceMaterial.h"

#include <vector>
#include <string>

class ServicesLayer;

class ServiceVolume {
public:

  enum Shape {Cylinder, Disk};

  typedef std::vector<const ServicesLayer*>   LayerContainer;

  ServiceVolume( Shape sh, double rmin, double rmax, double zmin, double zmax, const std::string& name) :
		 m_shape(sh), m_rMin( rmin), m_rMax(rmax), m_zMin(zmin), m_zMax(zmax), m_name(name),
		 m_layers(), m_previous(0), m_next(0) {}

  /// Add a layer the services of which are routed through this volume
  void addLayer( const ServicesLayer* l) {
    m_layers.push_back(l);
  }

  /* don't remember why I wrote this one
  void addLayer( const ServicesLayer* l, const ServicesLayer* lnext) {
    m_layers.push_back(l);
    m_endingLayer = lnext;
  }
  */

  void addLayers( const LayerContainer& lc) {
    m_layers.insert( m_layers.end(), lc.begin(), lc.end());
  }

  void addEosServices( const ServicesLayer* l);

  Shape shape() const {return m_shape;}

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
    if (shape() == Cylinder) return zMax()-zMin();
    else                     return rMax() - rMin();
  }

  const std::string& name() const {return m_name;}

  bool contains( double x) const {
    // one-dimensional check, radial for disks and along Z for cylinders)
    double eps = 0.0001; // hard-wired tolerance
    if (shape()==Disk) return (x > rMin()-eps && x < rMax()+eps);
    else               return (x > zMin()-eps && x < zMax()+eps);
  }

  LayerContainer layers() const {return m_layers;}

  ServiceVolume* next() {return m_next;}

  void addPrevious( ServiceVolume* prev) { m_previous.push_back(prev);}

  void setNext( ServiceVolume* next) { m_next = next;}

  void dump( bool dumpMaterial = true) const;

  const std::vector<ServiceMaterial>& materials() const {return m_materials;}

  void setMaterials( const std::vector<ServiceMaterial>& mat) {m_materials = mat;}

  void addMaterial( const ServiceMaterial& mat) {m_materials.push_back( mat);}

  bool isEOS() const {
    if (m_name.size() < 4) return false;
    if (m_name.substr(m_name.size()-3) == "EOS") return true;
    return false;
  }

private:

  Shape m_shape;
  double m_rMin;
  double m_rMax;
  double m_zMin;
  double m_zMax;
  std::string m_name;

  LayerContainer m_layers;  // All layers the services of which are routed through this volume

  std::vector<ServiceVolume*> m_previous;
  ServiceVolume* m_next;

  std::vector<ServiceMaterial> m_materials;

};

#endif
