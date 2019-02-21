/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DefaultSmearer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_DEFAULTSMEARER_H
#define ISF_PARSIMTOOLS_DEFAULTSMEARER_H

// Fatras
#include "ISF_Interfaces/IParticleProcessor.h"
#include "ISF_ParSimInterfaces/IChargedSmearer.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// CLHEP
//#include "CLHEP/Random/RandomEngine.h"

namespace ISF {
    class ISFParticle;
}

namespace iParSim {
      
  /** 
   @class DefaultSmearer

   Creates xAOD::TrackParticles from ISFParticles
   this is a "no smearer"; it does not smear input particles

   @author Miha.Muskinja -at- cern.ch 
   */
      
  class DefaultSmearer : public extends<AthAlgTool, iParSim::IChargedSmearer>
  {
    public:

      /**Constructor */
      DefaultSmearer(const std::string&,const std::string&,const IInterface*);
      
      /**Destructor*/
      ~DefaultSmearer();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
        
      /** Creates a new ISFParticle from a given ParticleState, universal transport tool */
      //const xAOD::TrackParticle* smear(const ISF::ISFParticle& isp) const override; 

      bool smear(xAOD::TrackParticle* xaodTP, CLHEP::HepRandomEngine*) const override; 

      unsigned int pdg() const override;

    protected:
      

  };

} // end of namespace

#endif 

