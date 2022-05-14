///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "xAODEgamma/ElectronFwd.h"

//Includes for DataHandles
#include "StoreGate/DataHandle.h"

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
    ~METElectronAssociator() = default;

    // AsgTool Hooks
    virtual StatusCode initialize() override;

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

    private:
 
    /// Default constructor: 
    METElectronAssociator();

    SG::ReadHandleKey<xAOD::ElectronContainer> m_elContKey{this, "InputCollection", "Electrons", "electrons input key"};    

  }; 

}

#endif //> !METRECONSTRUCTION_METELECTRONASSOCIATOR_H
