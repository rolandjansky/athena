/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGLIGHTSECVERTEXING_H
#define BTAGGING_BTAGLIGHTSECVERTEXING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "BTagging/IBTagLightSecVertexing.h"

namespace InDet {
  class ISecVertexInJetFinder;
}

#include <string>

// Since these are typedefs, forward declarations won't work?
// namespace xAOD{
//   class Jet;
//   class BTagging;
//   class VertexContainer;
// }
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "StoreGate/ReadHandleKey.h"

namespace Trk{

  class VxSecVKalVertexInfo;
  class VxJetFitterVertexInfo;

}


/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
  class IJetFitterVariablesFactory;
  class IMSVVariablesFactory;

  class BTagLightSecVertexing : public AthAlgTool, virtual public IBTagLightSecVertexing
  {
      public:
          /** Constructors and destructors */
          BTagLightSecVertexing(const std::string&,const std::string&,const IInterface*);
          virtual ~BTagLightSecVertexing();

          virtual StatusCode initialize() override;
          virtual StatusCode BTagSecVertexing_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) const override;
          virtual StatusCode finalize() override;


      private:
        
         StatusCode fillVkalVariables(const xAOD::Jet&, xAOD::BTagging*, const Trk::VxSecVKalVertexInfo*, const xAOD::TrackParticleContainer*,  std::string) const;
         StatusCode fillJFVariables(const xAOD::Jet&, xAOD::BTagging*, const Trk::VxJetFitterVertexInfo*, const xAOD::TrackParticleContainer*, std::string) const;

         ToolHandleArray< InDet::ISecVertexInJetFinder > m_secVertexFinderToolsHandleArray;
         ToolHandle<IJetFitterVariablesFactory> m_JFvarFactory;
         //ToolHandle<IMSVVariablesFactory> m_MSVvarFactory;

         std::vector<std::string> m_secVertexFinderTrackNameList;
         std::vector<std::string> m_secVertexFinderBaseNameList;

         std::string m_vxPrimaryName; //Input ReadHandle
         SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Input primary vertex container"};
         Gaudi::Property<SG::ReadDecorHandleKey<xAOD::JetContainer>> m_jetSVLinkName{ this, "JetSecVtxLinkName", "", "Element Link vector form jet to SV container"};
         Gaudi::Property<SG::ReadDecorHandleKey<xAOD::JetContainer>> m_jetJFVtxLinkName{ this, "JetJFVtxLinkName", "", "Element Link vector form jet to JF vertex"};

  }; // End class

} // End namespace

#endif // BTAGLIGHTSECVERTEXING_H
