/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/GenericFunctions/AbsFunction.hh"

// Real magnetic field svc
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
 
// This class is an adaptor:
namespace Genfun {
  
  class AtlasBComponent: public AbsFunction {
    
    FUNCTION_OBJECT_DEF(AtlasBComponent)
      
      public:
    
    // Constructor:
    AtlasBComponent(unsigned int index, MagField::AtlasFieldCache* fieldCache);
   
    // Destructor:
    virtual ~AtlasBComponent();
    
    // Copy Constructor:
    AtlasBComponent(const AtlasBComponent &right);

    // Dimensionality:
    virtual unsigned int dimensionality() const override;
    
    virtual double operator () (double argument) const override;
    virtual double operator () (const Argument & a) const override;
    
  private:
    
    unsigned int           m_index;

    // Don't allow assignment (avoid coverity warning).
    AtlasBComponent& operator= (const AtlasBComponent&);

    // Mag field accessor
    MagField::AtlasFieldCache* m_fieldCache{};

  };   

}
