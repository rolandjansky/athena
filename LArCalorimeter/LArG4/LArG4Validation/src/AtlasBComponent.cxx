/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Units/SystemOfUnits.h"
#include "AtlasBComponent.h"


using CLHEP::kilogauss;

  
namespace Genfun {
  FUNCTION_OBJECT_IMP(AtlasBComponent)
    // This class is an adaptor:
    
  // Constructor:
  AtlasBComponent::AtlasBComponent(unsigned int index)
   : AbsFunction()
   , m_index(index) 
   , m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc", "G4AtlasFieldSvc") {}
  
  // Destructor:
  AtlasBComponent::~AtlasBComponent() {}
  
  // Copy Constructor:
  AtlasBComponent::AtlasBComponent(const AtlasBComponent &right)
   : AbsFunction()
   , m_index(right.m_index)
   , m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc", "G4AtlasFieldSvc") {}
  
  // Dimensionality:
  unsigned int AtlasBComponent::dimensionality() const { return 3;}
  
  double AtlasBComponent::operator () (double) const { throw "Invalid parameter!"; }   

  double AtlasBComponent::operator () (const Argument & a) const {
    static MagField::IMagFieldSvc * magFieldSvcQuick=0;
    if (!magFieldSvcQuick) {
      if ( m_magFieldSvc.retrieve().isFailure()) {
        std::runtime_error("Could not retrieve B-field svc");
        return -1.;
      }
      magFieldSvcQuick=&(*m_magFieldSvc);
    }
    // Changes from doubles to floats and back
    double XYZ_in_mm[3] , BXYZ_in_kgmm[3];
    for (int i=0;i<3;++i) *(XYZ_in_mm+i) = a[i];
    magFieldSvcQuick->getField( XYZ_in_mm , BXYZ_in_kgmm );
    return BXYZ_in_kgmm[m_index]*kilogauss;
  }

}

