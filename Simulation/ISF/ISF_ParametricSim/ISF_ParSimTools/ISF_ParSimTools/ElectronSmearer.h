/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
      virtual ~ElectronSmearer();
      
      /** AlgTool initialize method.*/
      virtual StatusCode initialize() override;
      
      /** AlgTool finalize method */
      virtual StatusCode finalize() override;
        
      /** Creates a new ISFParticle from a given ParticleState, universal transport tool */
      //const xAOD::TrackParticle* smear(const ISF::ISFParticle& isp) const override; 

      virtual
      bool smear(xAOD::TrackParticle* xaodTP, CLHEP::HepRandomEngine*) const override; 

      virtual
      unsigned int pdg() const override;

    protected:
      

  };

} // end of namespace

#endif 

