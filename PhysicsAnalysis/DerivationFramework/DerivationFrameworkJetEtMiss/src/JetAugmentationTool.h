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
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "xAODJet/JetContainer.h"
#include "ParticleJetTools/JetTruthLabelingTool.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h" // QGTaggerTool

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
    SG::AuxElement::Decorator<float>* dec_calibpt;
    SG::AuxElement::Decorator<float>* dec_calibeta;
    SG::AuxElement::Decorator<float>* dec_calibphi;
    SG::AuxElement::Decorator<float>* dec_calibm;
    ToolHandle<IJetModifier> m_jetCalibTool;
    std::string m_calibMomentKey;
    bool m_docalib;

    // JVT
    SG::AuxElement::Decorator<float>* dec_jvt;
    SG::AuxElement::Decorator<char>* dec_passJvt;
    ToolHandle<IJetUpdateJvt> m_jvtTool;
    ToolHandle<CP::IJetJvtEfficiency> m_jetJvtEfficiencyTool; //!
    std::string m_jvtMomentKey;
    bool m_dojvt;

    // PFlow fJVT
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_fjvt;
    ToolHandle<IJetModifier> m_fjvtTool;
    std::string m_fjvtMomentKey;
    bool m_dofjvt;

    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_customSumTrk;
    ToolHandle<IJetModifier> m_customSumTrkTool;
    std::string m_customSumTrkMomentKey;
    bool m_docustomSumTrk;

    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_customJvf;
    ToolHandle<IJetModifier> m_customJvfTool;
    std::string m_customJvfMomentKey;
    bool m_docustomJvf;

    // MV-fJVT     @author: louis.portales@cern.ch
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_MVfJvt;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_MVfJvt_Sumcle;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_MVfJvt_SumclIso;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_MVfJvt_SumclEMprob;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_MVfJvt_LeadclWidth;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_MVfJvt_LeadclSecondLambda;
    ToolHandle<IJetModifier> m_MVfJvtTool;
    std::string m_MVfJvtMomentKey;
    bool m_doMVfJvt;

    // b-tagging       @author tripiana@cern.ch
    std::vector<SG::AuxElement::Decorator<float>*> dec_btag;
    std::vector<std::string> m_btagWP;
    bool m_dobtag;
    /// Athena configured tools
    ToolHandleArray<IBTaggingSelectionTool> m_btagSelTools;

    //TrackSumMass and TrackSumPt for calo-jets built in Tier-0
    //@author: nurfikri.bin.norjoharuddeen@cern.ch
    ToolHandle<IJetModifier> m_jetTrackSumMomentsTool;
    bool m_decoratetracksum;
    SG::AuxElement::Decorator<float>* dec_tracksummass;
    SG::AuxElement::Decorator<float>* dec_tracksumpt;

    //OriginCorrection 
    ToolHandle<IJetModifier> m_jetOriginCorrectionTool;
    bool m_decorateorigincorrection;
    SG::AuxElement::Decorator<ElementLink<xAOD::VertexContainer>>* dec_origincorrection;
    SG::AuxElement::Decorator<float>* dec_originpt;
    SG::AuxElement::Decorator<float>* dec_origineta;
    SG::AuxElement::Decorator<float>* dec_originphi;
    SG::AuxElement::Decorator<float>* dec_originm;
    
    // GhostTruthAssociation for derivations, @author jeff.dandoy@cern.ch
    ToolHandle<IJetModifier> m_jetPtAssociationTool;
    bool m_decorateptassociation;
    SG::AuxElement::Decorator<float>* dec_GhostTruthAssociationFraction;
    SG::AuxElement::Decorator< ElementLink<xAOD::JetContainer> >* dec_GhostTruthAssociationLink;

    // Ntracks for QGTaggerTool ---
    bool m_decorateQGVariables;
    SG::AuxElement::Decorator<int>* dec_AssociatedNTracks;
    SG::AuxElement::Decorator<float>* dec_AssociatedTracksWidth;
    SG::AuxElement::Decorator<float>* dec_AssociatedTracksC1;
    SG::AuxElement::Decorator<int>*   dec_Associated_truthjet_nCharged;
    SG::AuxElement::Decorator<float>* dec_Associated_truthjet_pt;
    SG::AuxElement::Decorator<float>* dec_Associated_truthjet_eta;
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;

    //// Large-R jet truth labeling @author jveatch@cern.ch and tnobe@cern.ch
    ToolHandle<JetTruthLabelingTool> m_jetTruthLabelingTool;
    bool m_decoratetruthlabel;
    std::unique_ptr< SG::AuxElement::Decorator<int> > dec_Label;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dRW;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dRZ;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dRH;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_dRTop;
    std::unique_ptr< SG::AuxElement::Decorator<int> > dec_NB;
    std::unique_ptr< SG::AuxElement::Decorator<float> > dec_TruthJetMass;
    std::string m_truthLabelName;

  };
}

#endif // DERIVATIONFRAMEWORK_JETAUGMENTATIONTOOL_H
