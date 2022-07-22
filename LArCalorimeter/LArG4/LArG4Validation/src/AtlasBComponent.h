/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CxxUtils/checker_macros.h"

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

    /* Mag field accessor.
     *
     * For this to be thread-safe the caller needs to have a local instance
     * of AtlasFieldCache as the cache is being mutated by operator(). We have
     * no way of knowing if this is the case, but this class is used only in
     * one place.
     */
    MagField::AtlasFieldCache* m_fieldCache ATLAS_THREAD_SAFE{};

  };   

}
