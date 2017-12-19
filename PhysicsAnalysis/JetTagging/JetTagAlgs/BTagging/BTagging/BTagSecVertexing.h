/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGSECVERTEXING_H
#define BTAGGING_BTAGSECVERTEXING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "BTagging/IBTagSecVertexing.h"

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
#include "StoreGate/WriteHandleKey.h"

namespace Trk{

  class VxSecVKalVertexInfo;
  class VxJetFitterVertexInfo;

}


/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
  class IJetFitterVariablesFactory;
  class IMSVVariablesFactory;

  class BTagSecVertexing : public AthAlgTool, virtual public IBTagSecVertexing
  {
      public:
          /** Constructors and destructors */
          BTagSecVertexing(const std::string&,const std::string&,const IInterface*);
          virtual ~BTagSecVertexing();

          virtual StatusCode initialize() override;
          virtual StatusCode BTagSecVtx_exec(xAOD::Jet& myJet, xAOD::BTagging*, xAOD::VertexContainer*, xAOD::BTagVertexContainer*, const xAOD::Vertex* vtx=0) const override; //Kept for TrigBtagFex.cxx
          virtual StatusCode BTagSecVtx_exec(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) const override;
          virtual StatusCode finalize() override;


      private:
        
    StatusCode fillVkalVariables(xAOD::Jet&, xAOD::BTagging*, xAOD::VertexContainer*, const Trk::VxSecVKalVertexInfo*, const xAOD::TrackParticleContainer*,const xAOD::Vertex&,  std::string) const;
    StatusCode fillJFVariables(xAOD::Jet&, xAOD::BTagging*, xAOD::BTagVertexContainer*, const Trk::VxJetFitterVertexInfo*, const xAOD::TrackParticleContainer*, std::string) const;

         ToolHandleArray< InDet::ISecVertexInJetFinder > m_secVertexFinderToolsHandleArray;
         ToolHandle<IJetFitterVariablesFactory> m_JFvarFactory;
         ToolHandle<IMSVVariablesFactory> m_MSVvarFactory;

         std::vector<std::string> m_secVertexFinderTrackNameList;
         std::vector<std::string> m_secVertexFinderBaseNameList;

         std::string m_vxPrimaryName; //Input ReadHandle
         SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Primary vertex Collection"};
         SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingTrackSVCollectionName { this, "BTaggingTrackSVCollectionName", "", "BTagging container with tracks associations and sec vertexing"};
         SG::WriteHandleKey<xAOD::VertexContainer> m_BTagSVCollectionName {this, "BTagSVCollectionName", "", ""};
         SG::WriteHandleKey<xAOD::BTagVertexContainer> m_BTagJFVtxCollectionName {this, "BTagJFVtxCollectionName", "", ""};

  }; // End class

} // End namespace

#endif // BTAGSECVERTEXING_H
