/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyParticleOrderingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_ENERGYPARTICLEORDERINGTOOL_H
#define ISF_TOOLS_ENERGYPARTICLEORDERINGTOOL_H 1

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_Interfaces/IParticleOrderingTool.h"
#include "ISF_Event/SimSvcID.h"


namespace ISF {

  /** @class EnergyParticleOrderingTool

      This AthenaAlgTool orders the particles according to their kinetic energy
  
      @author bwynne -at- cern.ch
     */

  class EnergyParticleOrderingTool : public extends<AthAlgTool, IParticleOrderingTool> {

    public: 
      /** Constructor with parameters */
      EnergyParticleOrderingTool( const std::string& t, const std::string& n, const IInterface* p );

      /** Virtual destructor */
      virtual ~EnergyParticleOrderingTool();
       
      /** Compute particle order */
      virtual ISF::ParticleOrder computeOrder(const ISFParticle& p) const;
  }; 
  
}

#endif //> !ISF_TOOLS_ENERGYPARTICLEORDERINGTOOL_H

