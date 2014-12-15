///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTauAssociator.h 
// Header file for class METTauAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTAUASSOCIATOR_H
#define METRECONSTRUCTION_METTAUASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

namespace met{
  class METTauAssociator final
    : virtual public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTauAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METTauAssociator(const std::string& name);
    ~METTauAssociator();

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
				   MissingETBase::Types::constvec_t& tcvec,
				   const xAOD::CaloClusterContainer* tcCont);
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
    METTauAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METTAUASSOCIATOR_H
