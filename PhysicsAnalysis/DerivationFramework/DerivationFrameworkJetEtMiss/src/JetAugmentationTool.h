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
#include "StoreGate/WriteDecorHandleKeyArray.h"

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
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_calibpt_key {this, "CalibPtKey", "", "Decoration for calibrated jet pt"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_calibeta_key {this, "CalibEtaKey", "", "Decoration for calibrated jet eta"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_calibphi_key {this, "CalibPhiKey", "", "Decoration for calibrated jet phi"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_calibm_key {this, "CalibMKey", "", "Decoration for calibrated jet mass"};
    ToolHandle<IJetModifier> m_jetCalibTool;
    std::string m_calibMomentKey;
    bool m_docalib;

    // JVT
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_jvt_key {this, "JVTKey", "", "Decoration for JVT"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_passJvt_key {this, "passJVTKey", "", "Decoration for boolean containing JVT decision"};
    ToolHandle<IJetUpdateJvt> m_jvtTool;
    ToolHandle<CP::IJetJvtEfficiency> m_jetJvtEfficiencyTool;
    std::string m_jvtMomentKey;
    bool m_dojvt;

    //PFlow fJVT
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_fjvt;
    std::string m_fjvtMomentKey;

    // b-tagging       @author tripiana@cern.ch
    std::vector<std::string> m_btagWP;
    bool m_dobtag;
    /// Athena configured tools
    ToolHandleArray<IBTaggingSelectionTool> m_btagSelTools;
    SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_dec_btag_keys{this, "BtagDecKeys", {}, "SG keys for b-tagging WP decisions"};
    
    //TrackSumMass and TrackSumPt for calo-jets built in Tier-0
    //@author: nurfikri.bin.norjoharuddeen@cern.ch
    ToolHandle<IJetModifier> m_jetTrackSumMomentsTool;
    bool m_decoratetracksum;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_tracksummass;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_tracksumpt;
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_tracksummass_key {this,"TrackSumMassKey", "", "Decoration for mass calculated from associated tracks"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_tracksumpt_key {this,"TrackSumPtKey", "","Decoration for pt calculated from associated tracks"};

    // GhostTruthAssociation for derivations, @author jeff.dandoy@cern.ch
    ToolHandle<IJetModifier> m_jetPtAssociationTool;
    bool m_decorateptassociation;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_GhostTruthAssociationFraction;
    std::unique_ptr< SG::AuxElement::ConstAccessor< ElementLink<xAOD::JetContainer> > > m_acc_GhostTruthAssociationLink;
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_GhostTruthAssociationFraction_key {this,"GhostTruthAssocFracKey", "", "Decoration for fraction of ghost-associated truth pt to reco jet"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_GhostTruthAssociationLink_key {this, "GhostTruthAssocLinkKey", "", "Decoration for links of ghost-associated truth particles"};

    // Ntracks for QGTaggerTool --- 
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;
    ToolHandle<CP::ITrackVertexAssociationTool> m_trkVtxAssociationTool;
    ToolHandle<IJetDecorator> m_qgTool;
    bool m_decorateQGVariables;
    std::unique_ptr< SG::AuxElement::ConstAccessor<int> > m_acc_AssociatedNTracks;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_AssociatedTracksWidth;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float>  >m_acc_AssociatedTracksC1;
    std::unique_ptr< SG::AuxElement::ConstAccessor<int> > m_acc_Associated_truthjet_nCharged;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_Associated_truthjet_pt;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_Associated_truthjet_eta;
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_associatedNTracks_key {this, "associatedNTracksKey", "", "Decoration for number of ghost-associated tracks"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_associatedTracksWidth_key {this, "associatedTracksWidthKey", "", "Decoration for track width"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_associatedTracksC1_key {this, "associatedTracksC1Key", "", "Decoration for track C1"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_associated_truthjet_nCharged_key {this, "associatedTruthNChargedKey", "", "Decoration for charged truth particles"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_associated_truthjet_pt_key {this, "associatedTruthJetPtKey", "", "Decoration for associated truth jet pT"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_associated_truthjet_eta_key {this, "associatedTruthJetEtaKey", "", "Decoration for associated truth jet eta "};

    //// Large-R jet truth labeling @author jveatch@cern.ch and tnobe@cern.ch
    ToolHandle<JetTruthLabelingTool> m_jetTruthLabelingTool;
    bool m_decoratetruthlabel;
    std::unique_ptr< SG::AuxElement::ConstAccessor<int> > m_acc_label;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_dRW;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_dRZ;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_dRH;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_dRTop;
    std::unique_ptr< SG::AuxElement::ConstAccessor<int> > m_acc_NB;
    std::unique_ptr< SG::AuxElement::ConstAccessor<float> > m_acc_truthJetMass;
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_key {this,"truthLabelKey", "", "Decoration for large-R truth label"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_dRW_key {this, "truthLabeldRWKey", "", "Decoration for dR between large-R jet and W boson"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_dRZ_key {this, "truthLabeldRZKey", "", "Decoration for dR between large-R jet and Z boson"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_dRH_key {this, "truthLabeldRHKey", "", "Decoration for dR between large-R jet and H boson"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_dRTop_key {this, "truthLabeldRTopKey", "", "Decoration for dR between large-R jet and top quark"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_NB_key {this, "truthLabeldRNBKey", "", "Decoration for number of ghost-associated b-hadrons"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_truthLabel_truthJetMass_key {this, "truthLabelJetMassKey", "", "Decoration for matched truth-jet mass"};

    std::string m_truthLabelName;

  }; 
}

#endif // DERIVATIONFRAMEWORK_JETAUGMENTATIONTOOL_H
