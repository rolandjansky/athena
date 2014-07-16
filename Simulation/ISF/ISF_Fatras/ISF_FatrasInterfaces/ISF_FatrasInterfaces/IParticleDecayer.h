/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleDecayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IPARTICLEDECAYER_H
#define ISF_FATRASINTERFACES_IPARTICLEDECAYER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace ISF {
    class ISFParticle;
}

namespace iFatras 
{
  
  static const InterfaceID IID_IParticleDecayer("IParticleDecayer", 1, 0);
  /** 
      @class IParticleDecayer
      
      Interface definition for the Particle decay in Fatras,
      the particle transport of the particle is also matter of
      the particle decayer.
      
      Returning a vector of size 0 means: particle decay did not happen

      @author Joerg.Mechning -at- cern.ch
      @author Andreas.Salzburger@cern.ch
  */
  
  class IParticleDecayer : virtual public IAlgTool
  {
  public:
    
    /** Virtual destructor */
    virtual ~IParticleDecayer(){}
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IParticleDecayer; }
    
    /** interface for the transport and Decay of a particle */
    virtual std::vector<ISF::ISFParticle*> decayParticle( const ISF::ISFParticle& parent,
                                                          const Amg::Vector3D& vertex,
                                                          const Amg::Vector3D& mom,
                                                          int pdgCode,
                                                          double timeStamp = 0.) const = 0;
  };
  
}

#endif // ISF_FATRASINTERFACES_IPARTICLEDECAYER_H

