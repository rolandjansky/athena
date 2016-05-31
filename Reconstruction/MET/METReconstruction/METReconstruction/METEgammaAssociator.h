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
#include "METReconstruction/METRecoCommon.h"

#include "AsgTools/ToolHandle.h"

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
				   const xAOD::IParticleContainer* tcCont) const;

    StatusCode selectEgammaClusters(const xAOD::CaloCluster *swclus,
				    const std::vector<const xAOD::IParticle*>& inputTC,
				    std::vector<const xAOD::IParticle*>& tclist) const;

    double m_tcMatch_dR;
    double m_tcMatch_maxRat;
    unsigned short m_tcMatch_method;

    private:
 
    /// Default constructor: 
    METEgammaAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METEGAMMAASSOCIATOR_H
