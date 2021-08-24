/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_JETSECVTXFINDINGALG_H
#define BTAGGING_JETSECVTXFINDINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

#include "xAODJet/JetContainer.h"
#include "VxSecVertex/VxSecVertexInfo.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTracking/VertexContainer.h"

namespace InDet {
  class ISecVertexInJetFinder;
}

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{

  class JetSecVtxFindingAlg : public AthAlgorithm
  {
      public:
        /** Constructors and destructors */
        JetSecVtxFindingAlg(const std::string& name, ISvcLocator *pSvcLocator);
        virtual ~JetSecVtxFindingAlg();
    
        /** Main routines specific to an ATHENA algorithm */
        virtual StatusCode initialize();
        virtual StatusCode execute();

      private:
        
        ToolHandle< InDet::ISecVertexInJetFinder > m_secVertexFinderToolHandle;

        std::string m_vxPrimaryName; //Input ReadHandle

        SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
        SG::ReadDecorHandleKey<xAOD::JetContainer> m_TracksToTag { this, "TracksToTag", "", "Element Link vector from jet to IParticleContainer"};
        SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Input primary vertex container"};
        SG::WriteHandleKey<Trk::VxSecVertexInfoContainer> m_VxSecVertexInfoName {this, "BTagVxSecVertexInfoName", "", "Output VxSecVertexInfo container"};

  }; // End class

} // End namespace

#endif // JETSECVTXFINDINGALG_H
