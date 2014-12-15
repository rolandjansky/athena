///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaAssociator.h 
// Header file for class METEgammaAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METEGAMMAASSOCIATOR_H
#define METRECONSTRUCTION_METEGAMMAASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

namespace met{
  class METEgammaAssociator
    : virtual public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METEgammaAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METEgammaAssociator(const std::string& name);
    virtual ~METEgammaAssociator();

    // AsgTool Hooks
    StatusCode initialize();
    StatusCode finalize();

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

    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
				   std::vector<const xAOD::IParticle*>& tclist,
				   MissingETBase::Types::constvec_t& tcvec,
				   const xAOD::CaloClusterContainer* tcCont);
    double m_tcMatch_dR;
    double m_tcMatch_maxRat;
    double m_tcMatch_tolerance;
    unsigned short m_tcMatch_method;

    private:
 
    /// Default constructor: 
    METEgammaAssociator();

    // track overlap removal
    bool m_eg_doTracks;

  }; 

}

#endif //> !METRECONSTRUCTION_METEGAMMAASSOCIATOR_H
