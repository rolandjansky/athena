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
    : public METAssociator
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

    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const final;
    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
				   std::vector<const xAOD::IParticle*>& tclist,
				   const met::METAssociator::ConstitHolder& constits) const final;

    StatusCode extractPFO(const xAOD::IParticle* obj,
			  std::vector<const xAOD::IParticle*>& pfolist,
			  const met::METAssociator::ConstitHolder& constits,
			  std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final;

    StatusCode extractPFOHR(const xAOD::IParticle* obj,
                            std::vector<const xAOD::IParticle*> /*hardObjs*/,
                            std::vector<const xAOD::IParticle*>& pfolist,
                            const met::METAssociator::ConstitHolder& constits,
                            std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta,
                            float& /*UEcorr*/) const final {
      // use the standard extractPFO method
      return this->METTauAssociator::extractPFO(obj, pfolist, constits, momenta);
    }
    
    StatusCode extractTracks(const xAOD::IParticle* obj,
			     std::vector<const xAOD::IParticle*>& constlist,
			     const met::METAssociator::ConstitHolder& constits) const final;

    private:
 
    /// Default constructor: 
    METTauAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METTAUASSOCIATOR_H
