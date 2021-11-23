/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Bmumu_reco_mumu.h
///////////////////////////////////////////////////////////////////
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Original author (Reco_mumu):
//          Daniel Scheirich <daniel.scheirich@cern.ch>
// 
// Changes:
// Basic dimuon reconstruction for the derivation framework.
// This class inherits from CfAthAlgTool instead of AthAlgTool in order
// to have access to the CutFlowSvc instance.
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_Bmumu_reco_mumu_H
#define DERIVATIONFRAMEWORK_Bmumu_reco_mumu_H

#include <string>

#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

/** THE reconstruction tool
 */
namespace DerivationFramework {

  class Bmumu_reco_mumu : public CfAthAlgTool, public IAugmentationTool {
    public: 
      Bmumu_reco_mumu(const std::string& t, const std::string& n,
		      const IInterface* p);

      StatusCode initialize() override;
      
      virtual StatusCode addBranches() const override;
      
    private:
      /** tools
       */
      ToolHandle<Trk::V0Tools>                    m_v0Tools{this, "V0Tools", "Trk::V0Tools"};
      ToolHandle<Analysis::JpsiFinder>            m_jpsiFinder{this,"JpsiFinder", "Analysis::JpsiFinder"};
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter{this, "PVRefitter", "Analysis::PrimaryVertexRefitter"};
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      
      /** job options
       */
      
      SG::ReadHandleKey<xAOD::VertexContainer>  m_pvContainerKey{this, "PVContainerName", "PrimaryVertices"};
    
      SG::WriteHandleKey<xAOD::VertexContainer>  m_refContainerKey{this, "RefPVContainerName" , "RefittedPrimaryVertices"};
  
      SG::WriteHandleKey<xAOD::VertexContainer>  m_outVtxContainerKey{this, "OutputVtxContainerName" , "OniaCandidates"};
    
      Gaudi::Property<bool>        m_refitPV{this, "RefitPV", false};
      Gaudi::Property<int>         m_PV_max{this, "MaxPVrefit", 1};
      Gaudi::Property<int>         m_DoVertexType{this, "DoVertexType", 1};
      // minimum number of tracks for PV to be considered for PV association
      Gaudi::Property<unsigned int>m_PV_minNTracks{this, "MinNTracksInPV", 0};
      Gaudi::Property<bool>        m_do3d{this, "Do3d", false};
  
  }; 
}

#endif // DERIVATIONFRAMEWORK_Bmumu_reco_mumu_H
