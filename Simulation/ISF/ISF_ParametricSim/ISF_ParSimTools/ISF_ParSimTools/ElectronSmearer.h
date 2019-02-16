/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronSmearer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_ELECTRONSMEARER_H
#define ISF_PARSIMTOOLS_ELECTRONSMEARER_H

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
   @class ElectronSmearer

   Creates xAOD::TrackParticles from ISFParticles
   this is a smearer for electrons

   !!smearing parameters are needed!!

   @author Miha.Muskinja -at- cern.ch 
   */
      
  class ElectronSmearer : public extends<AthAlgTool, iParSim::IChargedSmearer>
  {
    public:

      /**Constructor */
      ElectronSmearer(const std::string&,const std::string&,const IInterface*);
      
      /**Destructor*/
      ~ElectronSmearer();
      
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

