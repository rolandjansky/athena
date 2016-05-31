///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METElectronAssociator.h 
// Header file for class METElectronAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METELECTRONASSOCIATOR_H
#define METRECONSTRUCTION_METELECTRONASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METEgammaAssociator.h"
#include "METReconstruction/METAssociator.h"

namespace met{
  class METElectronAssociator final
    : public METEgammaAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METElectronAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METElectronAssociator(const std::string& name);
    ~METElectronAssociator();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    protected: 

    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap);
    StatusCode extractPFO(const xAOD::IParticle* obj,
			  std::vector<const xAOD::IParticle*>& pfolist,
			  const xAOD::PFOContainer* pfoCont,
			  std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta,
			  const xAOD::Vertex* pv) const final;
    StatusCode extractTracks(const xAOD::IParticle* obj,
			     std::vector<const xAOD::IParticle*>& constlist,
			     const xAOD::IParticleContainer* tcCont,
			     const xAOD::Vertex* pv) const final;

    private:
 
    /// Default constructor: 
    METElectronAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METELECTRONASSOCIATOR_H
