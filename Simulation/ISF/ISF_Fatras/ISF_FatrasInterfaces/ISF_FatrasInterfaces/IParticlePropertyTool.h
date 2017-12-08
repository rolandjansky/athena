/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticlePropertyTool.h, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#ifndef FATRASINTERFACES_IPARTICLEPROPERTYTOOL_H
#define FATRASINTERFACES_IPARTICLEPROPERTYTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// STL
#include <vector>
#include <string>

namespace iFatras
{
    
  static const InterfaceID  IID_IParticlePropertyTool("IParticlePropertyTool", 1, 0);
  /** 
      @class IParticlePropertyTool
      
      Interface definition for a tool responsible for retrieving
      particle properties.
      
      @author Joerg.Mechnich@cern.ch
  */
  
  class IParticlePropertyTool : virtual public IAlgTool
  {
  public:
    
    /** Virtual destructor */
    virtual ~IParticlePropertyTool(){}
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID()
          { return IID_IParticlePropertyTool; }

    virtual std::vector<std::pair<int,std::string> >
    listOfParticles() const=0;
    
    /** retrieve basic information of the particle (charge sign and
        mass in MeV) */
    virtual StatusCode
    basicInfo( int pdgCode, int* charge=0, double* mass=0) const = 0;
  };
  
}
#endif // FATRASINTERFACES_IPARTICLEPROPERTYTOOL_H
