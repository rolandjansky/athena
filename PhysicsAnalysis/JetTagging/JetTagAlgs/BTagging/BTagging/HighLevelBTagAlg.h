/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIGHLEVEL_BTAG_ALG_H
#define HIGHLEVEL_BTAG_ALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

#include "xAODBTagging/BTaggingContainer.h"
#include "JetInterface/IJetModifier.h"
#include "FlavorTagDiscriminants/ISingleJetDecorator.h"
#include "VxSecVertex/VxSecVertexInfo.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"

namespace InDet {
  class ISecVertexInJetFinder;
}

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{

  class HighLevelBTagAlg : public AthAlgorithm
  {
      public:
        /** Constructors and destructors */
        HighLevelBTagAlg(const std::string& name, ISvcLocator *pSvcLocator);
        virtual ~HighLevelBTagAlg();
    
        /** Main routines specific to an ATHENA algorithm */
        virtual StatusCode initialize() override;
        virtual StatusCode execute() override;

      private:
        
        ToolHandle<ISingleJetDecorator> m_jetDecorator;

        SG::ReadHandleKey<xAOD::BTaggingContainer> m_BTagCollectionName {this, "BTaggingCollectionName", "", "Input BTagging container"};
        Gaudi::Property<SG::ReadDecorHandleKey<xAOD::BTaggingContainer> >m_btagJetLinkName{this,"JetContainerLink","","Element link form BTagging to Jet container"};
        SG::ReadHandleKey< xAOD::TrackParticleContainer > m_TrackContainerKey {this,"TrackContainer","InDetTrackParticles","Key for the input track collection"};

        Gaudi::Property< std::string > m_prefix{this,"prefix","btagIp_",""};

        SG::ReadDecorHandleKey< xAOD::TrackParticleContainer > m_dec_d0 {this, "d0", "d0", "d0 of tracks"};
        SG::ReadDecorHandleKey< xAOD::TrackParticleContainer > m_dec_z0 {this, "z0SinTheta", "z0SinTheta", "z0SinTheta of tracks"};
        SG::ReadDecorHandleKey< xAOD::TrackParticleContainer > m_dec_d0_sigma {this, "d0Uncertainty", "d0Uncertainty", "d0Uncertainty of tracks"};
        SG::ReadDecorHandleKey< xAOD::TrackParticleContainer > m_dec_z0_sigma {this, "z0SinThetaUncertainty", "z0SinThetaUncertainty", "z0SinThetaUncertainty of tracks"};

        SG::ReadDecorHandleKey< xAOD::TrackParticleContainer > m_dec_track_pos {this, "trackDisplacement","trackDisplacement","trackDisplacement of tracks" };
        SG::ReadDecorHandleKey< xAOD::TrackParticleContainer > m_dec_track_mom {this, "trackMomentum","trackMomentum","trackMomentum of tracks" };

  }; // End class

} // End namespace

#endif // PREBTAGMODIFIERS_H
