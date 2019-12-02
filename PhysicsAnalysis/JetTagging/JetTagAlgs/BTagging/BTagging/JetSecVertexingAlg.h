/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_JETSECVERTEXINGALG_H
#define BTAGGING_JETSECVERTEXINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace InDet {
  class ISecVertexInJetFinder;
}

#include <string>

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

  class JetSecVertexingAlg : public AthAlgorithm
  {
      public:
        /** Constructors and destructors */
        JetSecVertexingAlg(const std::string& name, ISvcLocator *pSvcLocator);
        virtual ~JetSecVertexingAlg();
    
        /** Main routines specific to an ATHENA algorithm */
        virtual StatusCode initialize();
        virtual StatusCode execute();



      private:
        
        StatusCode createSecVkalContainer(xAOD::VertexContainer*, const Trk::VxSecVKalVertexInfo*) const;
        StatusCode createJFContainer(xAOD::BTagVertexContainer*, const Trk::VxJetFitterVertexInfo*, const xAOD::TrackParticleContainer*) const;

        //ToolHandleArray< InDet::ISecVertexInJetFinder > m_secVertexFinderToolsHandleArray;
        ToolHandle< InDet::ISecVertexInJetFinder > m_secVertexFinderToolsHandle;
        ToolHandle<IJetFitterVariablesFactory> m_JFvarFactory;
        ToolHandle<IMSVVariablesFactory> m_MSVvarFactory;

        //std::vector<std::string> m_secVertexFinderBaseNameList;
        std::string m_secVertexFinderBaseName;

        std::string m_vxPrimaryName; //Input ReadHandle
        SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
        Gaudi::Property<SG::ReadDecorHandleKey<xAOD::JetContainer>> m_jetParticleLinkName{ this, "TrackToJetAssociatorName", "", "Element Link vector form jet to particle container"};
        SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Input primary vertex container"};
        SG::WriteHandleKey<xAOD::VertexContainer> m_BTagSVCollectionName {this, "BTagSVCollectionName", "", "Output BTagging secondary vertex container"};
        SG::WriteHandleKey<xAOD::BTagVertexContainer> m_BTagJFVtxCollectionName {this, "BTagJFVtxCollectionName", "", "Output BTagging Jet Fitter container"};

  }; // End class

} // End namespace

#endif // JETSECVERTEXINGALG_H
