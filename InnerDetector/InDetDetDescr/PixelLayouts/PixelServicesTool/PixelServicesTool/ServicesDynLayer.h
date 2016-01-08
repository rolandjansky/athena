/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServicesDynLayer_H
#define ServicesDynLayer_H

#include "DetTypeDyn.h"
#include <string>
#include <vector>

class ServiceDynVolume;

class ServicesDynLayer {
public:

  /// constructor for barrel layers
  ServicesDynLayer( double radius, double halfLength, int nStaves,
		    DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
		    const std::string& suffix,
		    int nModulesPerStave, int nChipsPerModule);

  ServicesDynLayer( double radius, double halfLength, int nStaves,
		    DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
		    const std::string& suffix,
		    std::vector<int> nModulesPerStave, std::vector<int> nChipsPerModule);

  /// constructor for endcap layers
  ServicesDynLayer( double zpos, double rmin, double rmax, int nStaves,
		 DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
		 const std::string& suffix,
		 int nModulesPerStave, int nChipsPerModule);
  /*
  ServicesDynLayer( double zpos, double rmin, double rmax, int nStaves,
		 DetTypeDyn::Type type, DetTypeDyn::Part part, int num,
		 const ServicesStave* stave, const std::string& suffix);
  */
  double radius() const {return m_radius;}
  double zPos() const {return m_zPos;}

  double rMin() const {return m_rMin;}
  double rMax() const {return m_rMax;}
  double zMin() const {return m_zPos-m_halfLength; }
  double zMax() const {return m_zPos+m_halfLength; }

  double halfLength() const {return m_halfLength;}
  int nStaves() const { return m_nStaves;}
  DetTypeDyn::Type type() const {return m_type;}
  DetTypeDyn::Part part() const {return m_part;}

  int number() const {return m_number;}

  int modulesPerStaveFirst() const {return m_nModulesPerStave[0];}
  int chipsPerModuleFirst() const {return m_nChipsPerModule[0];}
  std::vector<int> modulesPerStave() const {return m_nModulesPerStave;}
  std::vector<int> chipsPerModule() const {return m_nChipsPerModule;}
  int modulesPerStaveTot() const;
  int moduleTypeNumber() const { return m_nModulesPerStave.size(); }

  ServiceDynVolume* lastVolume() const {return m_lastVolume;}
  void setLastVolume( ServiceDynVolume* vp) {m_lastVolume = vp;}

  std::string name() const;

  const std::string& layerSuffix() const {return m_suffix;}

private:

  double               m_radius;
  double               m_zPos;
  double               m_rMin;
  double               m_rMax;
  double               m_halfLength;
  int                  m_nStaves;
  DetTypeDyn::Type        m_type;
  DetTypeDyn::Part        m_part;
  int                  m_number;
  ServiceDynVolume*       m_lastVolume;
  std::string          m_suffix;
  std::vector<int>     m_nModulesPerStave;
  std::vector<int>     m_nChipsPerModule;
};

#endif
