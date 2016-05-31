///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonAssociator.h 
// Header file for class METMuonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METMUONASSOCIATOR_H
#define METRECONSTRUCTION_METMUONASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

#include "AsgTools/ToolHandle.h"

class ICaloClusterMatchingTool;

namespace met{
  class METMuonAssociator final
    : public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METMuonAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METMuonAssociator(const std::string& name);
    ~METMuonAssociator();

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
    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
				   std::vector<const xAOD::IParticle*>& tclist,
				   const xAOD::IParticleContainer* tcCont) const;
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
    METMuonAssociator();    

  }; 

}

#endif //> !METRECONSTRUCTION_METMUONASSOCIATOR_H
