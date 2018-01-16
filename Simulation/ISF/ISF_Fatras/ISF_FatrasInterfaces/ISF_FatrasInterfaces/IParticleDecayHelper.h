/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleDecayHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IPARTICLEDECAYHELPER_H
#define ISF_FATRASINTERFACES_IPARTICLEDECAYHELPER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace ISF
{
  class ISFParticle;
}

namespace iFatras
{

  /** 
      @class IParticleDecayHelper
      
      Interface definition for the Particle decay helper in Fatras,
      the particle transport of the particle is also matter of
      the particle decayer.
      
      It returns a track (if there's a charged particle) up to the decay
      point or 0 if no trace is left.
      
      The decay products are registered in decay collection through the ICollectionManager.

      @author Joerg.Mechning -at- cern.ch
      @author Andreas.Salzburger - at- cern.ch
  */
  
  class IParticleDecayHelper : virtual public IAlgTool
  {
  public:
    
    /** Virtual destructor */
    virtual ~IParticleDecayHelper(){}
    
    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(IParticleDecayHelper, 1, 0);

    /** free path estimator (-1 for stable particle) */
    virtual double freePath(const ISF::ISFParticle& isp) const = 0;

    /** decay (includes handling of secondaries) */
    virtual void decay(const ISF::ISFParticle& isp,
		       const Amg::Vector3D& vertex,
		       const Amg::Vector3D& mom,
		       double timeStamp = 0.) const = 0;

    /** decay */
    virtual std::vector<ISF::ISFParticle*> decayParticle( const ISF::ISFParticle& parent,
                                                          const Amg::Vector3D& vertex,
                                                          const Amg::Vector3D& mom,
                                                          double timeStamp = 0.) const = 0;
  };
  
}

#endif // ISF_FATRASINTERFACES_IPARTICLEDECAYHELPER_H

