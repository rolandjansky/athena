/*
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECO_IAFP_PROTONRECOTOOL_H
#define AFP_GLOBRECO_IAFP_PROTONRECOTOOL_H

/// @file   IAFP_ProtonRecoTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date   2021-11-26
/// 
/// @brief  Header file for interface IAFP_ProtonRecoTool


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "xAODForward/AFPProtonContainer.h"

#include <memory>
#include <string>

/// @brief Interface for proton reconstruction tool.
///
// The tool reconstructs AFPProtons from AFPTracks.
class IAFP_ProtonRecoTool : virtual public ::IAlgTool
{
    public:
    DeclareInterfaceID(IAFP_ProtonRecoTool, 1, 0);
  
    /// @brief run protons reconstruction
    /// 
    /// The method calls tools that reconstruct protons.
    virtual StatusCode doProtonReco(std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer, const EventContext& ctx) const = 0;
    
    /// @brief StoreGate name of the container where the reconstructed will be saved 
    virtual const std::string& outputContainerName () const = 0;
};



#endif