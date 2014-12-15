///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METJetAssocTool.h 
// Header file for class METJetAssocTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METJETASSOCTOOL_H
#define METRECONSTRUCTION_METJETASSOCTOOL_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

namespace met{
  class METJetAssocTool final
    : virtual public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METJetAssocTool, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METJetAssocTool(const std::string& name);
    ~METJetAssocTool();

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
    StatusCode extractPFO(const xAOD::IParticle*,
			  std::vector<const xAOD::IParticle*>&,
			  MissingETBase::Types::constvec_t&,
			  MissingETBase::Types::constvec_t&,
			  const xAOD::PFOContainer*,
			  const xAOD::Vertex*){return StatusCode::FAILURE;} // should not be called

    StatusCode extractTracks(const xAOD::IParticle*,
			     std::vector<const xAOD::IParticle*>&,
			     MissingETBase::Types::constvec_t&, 
			     const xAOD::CaloClusterContainer*,
			     const xAOD::Vertex*){return StatusCode::FAILURE;} // should not be called
    StatusCode extractTopoClusters(const xAOD::IParticle*,
				   std::vector<const xAOD::IParticle*>&,
				   MissingETBase::Types::constvec_t&, 
				   const xAOD::CaloClusterContainer*){return StatusCode::FAILURE;} // should not be called

    private:
 
    /// Default constructor: 
    METJetAssocTool();

  }; 

}

#endif //> !METRECONSTRUCTION_METJETASSOCTOOL_H
