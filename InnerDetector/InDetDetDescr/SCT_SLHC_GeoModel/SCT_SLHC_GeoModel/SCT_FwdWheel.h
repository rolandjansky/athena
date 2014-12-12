/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_FWDWHEEL_H
#define SCT_SLHC_GeoModel_SCT_FWDWHEEL_H

#include "SCT_SLHC_GeoModel/SCT_ComponentFactory.h"

#include <vector>

namespace InDetDDSLHC {

class SCT_FwdRing;
class SCT_FwdDiscSupport;

class SCT_FwdWheel : public SCT_UniqueComponentFactory{

 public:
  SCT_FwdWheel(const std::string & name, int iWheel);
  ~SCT_FwdWheel();
  
  SCT_FwdWheel & operator=(const SCT_FwdWheel &) = delete;
  SCT_FwdWheel(const SCT_FwdWheel &) = delete;
  int wheelNum() const {return m_iWheel;}
  
  // Retrieve basic parameters
  //
  double zPosition() const {return m_zPosition;} 
  int    numRings() const {return m_numRings;}
  const  SCT_FwdRing* ring(int i) {return m_rings[i];}

  //
  // Retrieve derived parameters
  //
  double thickness() const {return m_thickness;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 

  virtual GeoVPhysVol * build(SCT_Identifier id) const;
  
  void setRings(const std::vector<const SCT_FwdRing *> & rings);

 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  int m_iWheel;

  // Basic parameters
  int      m_numRings;
  //int      m_numRingsMax;
  short    m_ringSide;
  double   m_zPosition;
  std::vector<int> m_ringTypes;
  std::vector<double > m_ringOffset;
  double m_safety;
  // Derived parameters
  double m_innerRadius;
  double m_outerRadius;
  double m_thickness;
  double m_discSupportZpos;
  double m_staggerGap;
  // Child detector elements
  std::vector<const SCT_FwdRing*> m_rings;
  const SCT_FwdDiscSupport *  m_discSupport;
};
}
#endif //SCT_SLHC_GeoModel_SCT_FWDRING_H

