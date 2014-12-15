///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METPhotonAssociator.h 
// Header file for class METPhotonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METPHOTONASSOCIATOR_H
#define METRECONSTRUCTION_METPHOTONASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METEgammaAssociator.h"

namespace met{
  class METPhotonAssociator final
    : virtual public METEgammaAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METPhotonAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METPhotonAssociator(const std::string& name);
    ~METPhotonAssociator();

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
			  MissingETBase::Types::constvec_t& pfovec,
			  MissingETBase::Types::constvec_t& trkvec,
			  const xAOD::PFOContainer* pfoCont,
			  const xAOD::Vertex* pv) final;
    StatusCode extractTracks(const xAOD::IParticle* obj,
			     std::vector<const xAOD::IParticle*>& constlist,
			     MissingETBase::Types::constvec_t& trkvec,
			     const xAOD::CaloClusterContainer* tcCont,
			     const xAOD::Vertex* pv) final;

    private:
 
    /// Default constructor: 
    METPhotonAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METPHOTONASSOCIATOR_H
