/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinematicParticleFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_KINEMATICPARTICLEFILTER_H
#define ISF_TOOLS_KINEMATICPARTICLEFILTER_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_Event/KinematicParticleCuts.h"
#include "ISF_Interfaces/IParticleFilter.h"

namespace ISF {

  /** @class KinematicParticleFilter
  
      A generic kinematic ISFParticle filter using the implementation
      present in ISF::KinematicParticleCuts.

      @author Elmar.Ritsch -at- cern.ch
     */
  class KinematicParticleFilter : public AthAlgTool,
                                  public IParticleFilter,
                                  public KinematicParticleCuts { 
      
    public: 
      /** Constructor with parameters */
      KinematicParticleFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Virtual destructor */
      virtual ~KinematicParticleFilter();

      /** Athena AlgTool initialize */
      StatusCode  initialize();

      /** Athena AlgTool finalize */
      StatusCode  finalize();
       
      /** Returns a pass boolean on the particle  */
      inline virtual bool passFilter(const ISFParticle& isp) const;
     
    private:
      float                        m_maxEtaSym;        //!< maximum pseudorapidity cut (-eta/+eta)
  }; 
  
}

#endif //> !ISF_TOOLS_KINEMATICPARTICLEFILTER_H

