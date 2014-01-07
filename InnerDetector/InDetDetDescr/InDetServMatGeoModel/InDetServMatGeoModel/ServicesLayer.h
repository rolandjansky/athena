/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServicesLayer_H
#define ServicesLayer_H

#include "DetType.h"
#include <string>

class ServiceVolume;

class ServicesLayer {
public:

  /// constructor for barrel layers
  ServicesLayer( double radius, double halfLength, int nStaves,
		 DetType::Type type, DetType::Part part, int num,
		 const std::string& suffix,
		 int nModulesPerStave, int nChipsPerModule);

  /// constructor for endcap layers
  ServicesLayer( double zpos, double rmin, double rmax, int nStaves,
		 DetType::Type type, DetType::Part part, int num,
		 const std::string& suffix,
		 int nModulesPerStave, int nChipsPerModule);
  /*
  ServicesLayer( double zpos, double rmin, double rmax, int nStaves,
		 DetType::Type type, DetType::Part part, int num,
		 const ServicesStave* stave, const std::string& suffix);
  */
  double radius() const {return m_radius;}
  double zPos() const {return m_zPos;}

  double rMin() const {return m_rMin;}
  double rMax() const {return m_rMax;}

  double halfLength() const {return m_halfLength;}
  int nStaves() const { return m_nStaves;}
  DetType::Type type() const {return m_type;}
  DetType::Part part() const {return m_part;}

  int number() const {return m_number;}

  int modulesPerStave() const {return m_nModulesPerStave;}
  int chipsPerModule() const {return m_nChipsPerModule;}

  ServiceVolume* lastVolume() const {return m_lastVolume;}
  void setLastVolume( ServiceVolume* vp) {m_lastVolume = vp;}

  std::string name() const;

  const std::string& layerSuffix() const {return m_suffix;}

private:

  double               m_radius;
  double               m_zPos;
  double               m_rMin;
  double               m_rMax;
  double               m_halfLength;
  int                  m_nStaves;
  DetType::Type        m_type;
  DetType::Part        m_part;
  int                  m_number;
  ServiceVolume*       m_lastVolume;
  std::string          m_suffix;
  int                  m_nModulesPerStave;
  int                  m_nChipsPerModule;
};

#endif
