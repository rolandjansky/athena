/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/GenericFunctions/AbsFunction.hh"

// Real magnetic field svc
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ServiceHandle.h"
 
// This class is an adaptor:
namespace Genfun {
  
  class AtlasBComponent: public AbsFunction {
    
    FUNCTION_OBJECT_DEF(AtlasBComponent)
      
      public:
    
    // Constructor:
    AtlasBComponent(unsigned int index);
   
    // Destructor:
    ~AtlasBComponent();
    
    // Copy Constructor:
    AtlasBComponent(const AtlasBComponent &right);

    // Dimensionality:
    virtual unsigned int dimensionality() const;
    
    virtual double operator () (double argument) const;   
    virtual double operator () (const Argument & a) const;
    
  private:
    
    unsigned int           m_index;

    // Don't allow assignment (avoid coverity warning).
    AtlasBComponent& operator= (const AtlasBComponent&);

    // Mag field accessor
    ServiceHandle<MagField::IMagFieldSvc>   m_magFieldSvc;

  };   

}
