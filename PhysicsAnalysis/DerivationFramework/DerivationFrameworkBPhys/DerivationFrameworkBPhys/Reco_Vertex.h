/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Reco_Vertex.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Reco_Vertex_H
#define DERIVATIONFRAMEWORK_Reco_Vertex_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/ICandidateSearch.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace DerivationFramework {

  class Reco_Vertex : public AthAlgTool, public IAugmentationTool {
    public: 
      Reco_Vertex(const std::string& t, const std::string& n, const IInterface* p);

      virtual StatusCode initialize();
      
      virtual StatusCode addBranches() const;
      
    private:
      /** tools
       */
      ToolHandle<Trk::V0Tools>                    m_v0Tools;
      ToolHandle<Analysis::ICandidateSearch>      m_SearchTool;
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      
      /** job options
       */
      SG::WriteHandleKey<xAOD::VertexContainer> m_outputVtxContainerName;
      SG::ReadHandleKey<xAOD::VertexContainer> m_pvContainerName;
      SG::WriteHandleKey<xAOD::VertexContainer> m_refPVContainerName;
      bool        m_refitPV;
      int         m_PV_max;
      int         m_DoVertexType;
      size_t      m_PV_minNTracks;
      bool        m_do3d;
      bool        m_checkCollections;
      SG::ReadHandleKeyArray<xAOD::VertexContainer> m_CollectionsToCheck;
  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_Vertex_H
