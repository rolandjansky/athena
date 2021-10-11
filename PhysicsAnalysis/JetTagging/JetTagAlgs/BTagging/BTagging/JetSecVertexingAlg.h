/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "VxSecVertex/VxSecVertexInfo.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

namespace Trk{

  class VxSecVKalVertexInfo;
  class VxJetFitterVertexInfo;

}

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
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
        
        StatusCode createSecVkalContainer(xAOD::VertexContainer*, std::vector< ElementLink< xAOD::VertexContainer > >*, const Trk::VxSecVKalVertexInfo*) const;
        StatusCode createJFContainer(xAOD::BTagVertexContainer*, std::vector< ElementLink< xAOD::BTagVertexContainer > >*, const Trk::VxJetFitterVertexInfo*, const xAOD::TrackParticleContainer*) const;

        ToolHandle<IMSVVariablesFactory> m_MSVvarFactory;

        std::string m_secVertexFinderBaseName;
        std::string m_vxPrimaryName; //Input ReadHandle

        SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
        SG::ReadHandleKey<xAOD::TrackParticleContainer > m_TrackCollectionName {this, "TrackCollectionName", "", "Input track container"};
        SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Input primary vertex container"};
        SG::ReadHandleKey<Trk::VxSecVertexInfoContainer> m_VxSecVertexInfoName {this, "BTagVxSecVertexInfoName", "", "Input VxSecVertexInfo container"};
        SG::WriteHandleKey<xAOD::VertexContainer> m_BTagSVCollectionName {this, "BTagSVCollectionName", "", "Output BTagging secondary vertex container"};
        Gaudi::Property<SG::WriteDecorHandleKey<xAOD::JetContainer>> m_jetSVLinkName{ this, "JetSecVtxLinkName", "", "Element Link vector from jet to Vertex container"};
        SG::WriteHandleKey<xAOD::BTagVertexContainer> m_BTagJFVtxCollectionName {this, "BTagJFVtxCollectionName", "", "Output BTagging Jet Fitter container"};

  }; // End class

} // End namespace

#endif // JETSECVERTEXINGALG_H
