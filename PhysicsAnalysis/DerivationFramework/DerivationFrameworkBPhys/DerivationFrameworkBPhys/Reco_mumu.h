/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DERIVATIONFRAMEWORK_Reco_mumu_H
#define DERIVATIONFRAMEWORK_Reco_mumu_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
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

  class Reco_mumu : public AthAlgTool, public IAugmentationTool {
    public: 
      Reco_mumu(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize() override;
     
      StatusCode addBranches() const override;
      
    private:
      /** tools
       */
      ToolHandle<Trk::V0Tools>                    m_v0Tools{this, "V0Tools", "Trk::V0Tools"};
      ToolHandle<Analysis::JpsiFinder>            m_jpsiFinder{this,"JpsiFinder", "Analysis::JpsiFinder"};
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter{this, "PVRefitter", "Analysis::PrimaryVertexRefitter"};
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      
      
    // minimum number of tracks for PV to be considered for PV association
   
      Gaudi::Property<bool>         m_refitPV{this, "RefitPV", false};
      Gaudi::Property<int>          m_PV_max{this, "MaxPVrefit", 1};
      Gaudi::Property<int>          m_DoVertexType{this, "DoVertexType", 1};
      Gaudi::Property<unsigned int> m_PV_minNTracks{this, "MinNTracksInPV", 0};
      Gaudi::Property<bool>        m_do3d{this, "Do3d" , false};
      Gaudi::Property<bool>        m_checkCollections{this, "CheckCollections", false};
      
      SG::ReadHandleKeyArray<xAOD::VertexContainer> m_CollectionsToCheck{this, "CheckVertexContainers", {}};
      SG::ReadHandleKey<xAOD::VertexContainer> m_pvContainerKey{this,"PVContainerName", "PrimaryVertices"};
     
      SG::WriteHandleKey<xAOD::VertexContainer> m_refContainerKey{this, "RefPVContainerName","RefittedPrimaryVertices" };
      SG::WriteHandleKey<xAOD::VertexContainer> m_outContainerKey{this, "OutputVtxContainerName", "OniaCandidates"};
  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_mumu_H
