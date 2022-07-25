/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_VERTEXRECO_AFP_TIMERECOTOOL_H
#define AFP_VERTEXRECO_AFP_TIMERECOTOOL_H

#include "AFP_VertexReco/IAFP_TimeRecoTool.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "StoreGate/ReadHandleKey.h"
#include "PathResolver/PathResolver.h"

// xAOD includes
#include "AFP_DBTools/ToFVtxParamData.h"
#include "AFP_DBTools/IToFVtxParamDBTool.h"
#include "xAODForward/AFPVertex.h"
#include "xAODForward/AFPVertexContainer.h"
#include "xAODForward/AFPVertexAuxContainer.h"
#include "xAODForward/AFPProton.h"
#include "xAODForward/AFPProtonContainer.h"
#include "xAODForward/AFPProtonAuxContainer.h"
#include "xAODForward/AFPToFTrack.h"
#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"

#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <memory>
#include <iterator>
#include <algorithm>


/// Tool for vertex reconstruction by basic algorithm.
class AFP_VertexRecoBasic : public extends<AthAlgTool, IAFP_TimeRecoTool> {

  public:
    /// Default constructor
    AFP_VertexRecoBasic(const std::string& type, const std::string& name, const IInterface* parent);
    
    virtual ~AFP_VertexRecoBasic() = default;
    
    /// Loads parameterization
    virtual StatusCode initialize() override;
  
    // return name of the output container; needed by AFP_VertexRecoTool to make write handle keys
    const std::string& outputContainerName () const override {return m_vertexContainerName;}

    // TODO: put doVertexReco() in a separate "base" class
    StatusCode doVertexReco(std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer, const EventContext& ctx) const override;
    

  private:
    
    /// @copydoc AFP::ITool::configInfo()
    StatusCode configInfo() const;
    
    
    /// @brief Reconstructs single vertex from pair of ToFTracks and a pair of protons
    ///
    /// * Sets up measurement and calculates distances and times
    /// * Reconstucts vertex using basic algorithm
    /// * Calculates initial matching quality
    /// * Adds vertex to outputContainer and sets it's properties
    /// 
    /// @return Poiner to reconstucted AFPVertex
    xAOD::AFPVertex * reco(const double distA, const double distC, const xAOD::AFPToFTrack* toFTrackA, const xAOD::AFPToFTrack* toFTrackC,const xAOD::AFPProton* protonA, const xAOD::AFPProton* protonC, const AFP::ToFVtxParamData& TVP_A, const AFP::ToFVtxParamData& TVP_C, std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer) const;

    
    /// @ brief Tool for accessing DB to get the vertex ToF parameters
  ToolHandle<AFP::IToFVtxParamDBTool> m_tofVtxParamDBTool {this, "tofVtxParamDBTool", "AFP__ToFLocParamDBTool", "Tool to access DB to get the vertex ToF parameters"};

    Gaudi::Property<double> m_trackDistance{this, "TrackDistance", 0.5, "Maximum distance between ToF train edge and proton"};

    SG::ReadHandleKey<xAOD::AFPToFTrackContainer> m_tofTrackContainerKey{this, "AFPToFTrackContainerKey", "AFPToFTrackContainer", "Name of the container with ToF tracks from which vertices are to be reconstructed"};
  
    SG::ReadHandleKey<xAOD::AFPProtonContainer> m_protonContainerKey{this, "AFPProtonContainerKey", "AFPProtonContainer", "Name of the container with protons from which vertices are to be reconstructed"};

    Gaudi::Property<std::string> m_vertexContainerName{this, "verticesContainerName", "AFPVertexContainer", "Name of the container in which vertices are saved"};
  
    /// Creates and sets up a vertex
    xAOD::AFPVertex * createVertex (const double position, const double distA, const double distC, std::unique_ptr<xAOD::AFPVertexContainer>& outputContainer) const;

    /// Links ToF track pair to reconstructed vertex
    void linkToFTracksToVertex (const xAOD::AFPToFTrack* toFTrack, SG::ReadHandle<xAOD::AFPToFTrackContainer>& tofTrackContainer, xAOD::AFPVertex * vertex) const;

    /// Links proton pair to reconstructed vertex
    void linkProtonsToVertex (const xAOD::AFPProton* proton, SG::ReadHandle<xAOD::AFPProtonContainer>& protonContainer, xAOD::AFPVertex * vertex) const;

};


#endif
