/*
     Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_VERTEXRECO_IAFP_TIMERECOTOOL_H
#define AFP_VERTEXRECO_IAFP_TIMERECOTOOL_H

/// @file   IAFP_TimeRecoTool.h
/// @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-12-14
/// 
/// @brief  Header file for interface IAFP_TimeRecoTool


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "xAODForward/AFPVertexContainer.h"

#include <memory>
#include <string>

/// @brief Interface for proton reconstruction tool.
///
// The tool reconstructs AFPVertex from AFPProtons and AFPToFTracks.
class IAFP_TimeRecoTool : virtual public ::IAlgTool
{
    public:
    DeclareInterfaceID(IAFP_TimeRecoTool, 1, 0);
  
    /// @brief run protons reconstruction
    /// 
    /// The method calls tools that reconstruct protons.
    virtual StatusCode doVertexReco(std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer, const EventContext& ctx) const = 0;
    
    /// @brief StoreGate name of the container where the reconstructed will be saved 
    virtual const std::string& outputContainerName () const = 0;
};



#endif
