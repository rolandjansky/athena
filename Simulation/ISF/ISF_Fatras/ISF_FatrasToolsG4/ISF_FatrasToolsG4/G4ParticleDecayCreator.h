/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4ParticleDecayCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLSG4_G4PARTICLEDECAYCREATOR_H
#define ISF_FATRASTOOLSG4_G4PARTICLEDECAYCREATOR_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// iFatras
#include "ISF_FatrasInterfaces/IParticleDecayer.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

class StoreGateSvc;
class G4RunManager;

namespace ISF
{
  class ISFParticle;
  class IParticleBroker;
  class IG4RunManagerHelper;
}

namespace iFatras
{
        
  class PDGToG4Particle;
    
  /** @class G4ParticleDecayCreator

     G4 implementationof the IParticleDecayer interface from Fatras

      @author Joerg.Mechnich -at- cern.ch, Andreas.Salzburger -at- cern.ch
  */

  class G4ParticleDecayCreator: public AthAlgTool,
                                virtual public IParticleDecayer
  {
  public:
    /**AlgTool constructor for G4ParticleDecayCreator*/
    G4ParticleDecayCreator( const std::string&,
                            const std::string&,
                            const IInterface*);
    
    /** AlgTool initailize method.*/
    StatusCode initialize();
    /** AlgTool finalize method */
    StatusCode finalize();
    
    /** implementation of IParticleDecayer method for particle decay */
    std::vector<ISF::ISFParticle*> decayParticle(const ISF::ISFParticle& parent,
                                                 const Amg::Vector3D& vertex,
                                                 const Amg::Vector3D& mom,
                                                 int pdgCode,
                                                 double timeStamp = 0) const;

  private:
    /** initialize G4RunManager on first call if not done by then */
    bool initG4RunManager() const;

    /*---------------------------------------------------------------------
     *  Service & ToolHandles
     *---------------------------------------------------------------------*/
    ToolHandle<ISF::IG4RunManagerHelper> m_g4RunManagerHelper;   //!< G4RunManager needs to be initialized before G4 tables are accessed
    /** Handle on the PDGToG4Particle converter */
    ToolHandle<PDGToG4Particle>         m_pdgToG4Conv;

    mutable G4RunManager*                m_g4runManager;         //!< for dummy G4 initialization 
  };
}

#endif
