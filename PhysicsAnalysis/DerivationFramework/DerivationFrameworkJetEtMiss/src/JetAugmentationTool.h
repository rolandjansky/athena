////////////////////-*- C++ -*-////////////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetAugmentationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_JETAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_JETAUGMENTATIONTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetDecorator.h"
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "xAODJet/JetContainer.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "ParticleJetTools/JetTruthLabelingTool.h"

namespace DerivationFramework {

  class JetAugmentationTool : public AthAlgTool, public IAugmentationTool {
  public: 
    JetAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:
    std::string m_momentPrefix;
    std::string m_containerName;
    //
    // implement augmentations explicitly to avoid need to parse lists of moments to copy
    //
    // calibration

    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_calibpt;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_calibeta;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_calibphi;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_calibm;
    ToolHandle<IJetModifier> m_jetCalibTool;
    std::string m_calibMomentKey;
    bool m_docalib;

    // JVT
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_jvt;
    std::unique_ptr< SG::AuxElement::Decorator<char> > m_dec_passJvt;
    ToolHandle<IJetUpdateJvt> m_jvtTool;
    ToolHandle<CP::IJetJvtEfficiency> m_jetJvtEfficiencyTool;
    std::string m_jvtMomentKey;
    bool m_dojvt;

    //PFlow fJVT
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_fjvt;
    std::string m_fjvtMomentKey;

    // b-tagging       @author tripiana@cern.ch
    std::vector<SG::AuxElement::Decorator<float>*> m_dec_btag;
    std::vector<std::string> m_btagWP;
    bool m_dobtag;
    /// Athena configured tools
    ToolHandleArray<IBTaggingSelectionTool> m_btagSelTools;
    
    //TrackSumMass and TrackSumPt for calo-jets built in Tier-0
    //@author: nurfikri.bin.norjoharuddeen@cern.ch
    ToolHandle<IJetModifier> m_jetTrackSumMomentsTool;
    bool m_decoratetracksum;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_tracksummass;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_tracksumpt;

    // GhostTruthAssociation for derivations, @author jeff.dandoy@cern.ch
    ToolHandle<IJetModifier> m_jetPtAssociationTool;
    bool m_decorateptassociation;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_GhostTruthAssociationFraction;
    std::unique_ptr< SG::AuxElement::Decorator< ElementLink<xAOD::JetContainer> > > m_dec_GhostTruthAssociationLink;

    // Ntracks for QGTaggerTool --- 
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;
    ToolHandle<CP::ITrackVertexAssociationTool> m_trkVtxAssociationTool;
    ToolHandle<IJetDecorator> m_qgTool;
    bool m_decorateQGVariables;
    std::unique_ptr< SG::AuxElement::Decorator<int> > m_dec_AssociatedNTracks;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_AssociatedTracksWidth;
    std::unique_ptr< SG::AuxElement::Decorator<float>  >m_dec_AssociatedTracksC1;
    std::unique_ptr< SG::AuxElement::Decorator<int> > m_dec_Associated_truthjet_nCharged;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_Associated_truthjet_pt;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_Associated_truthjet_eta;

    //// Large-R jet truth labeling @author jveatch@cern.ch and tnobe@cern.ch
    ToolHandle<JetTruthLabelingTool> m_jetTruthLabelingTool;
    bool m_decoratetruthlabel;
    std::unique_ptr< SG::AuxElement::Decorator<int> > m_dec_Label;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_dRW;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_dRZ;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_dRH;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_dRTop;
    std::unique_ptr< SG::AuxElement::Decorator<int> > m_dec_NB;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_TruthJetMass;
    std::string m_truthLabelName;

  }; 
}

#endif // DERIVATIONFRAMEWORK_JETAUGMENTATIONTOOL_H
