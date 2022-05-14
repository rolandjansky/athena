///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METRebuilder.h
// Header file for class METRebuilder
// Author: T.J.Khoo<khoo@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef METUTILITIES_MET_METREBUILDER_H
#define METUTILITIES_MET_METREBUILDER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteHandleKey.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgTools/ToolHandle.h"


// METInterface includes
#include "METInterface/IMETRebuilder.h"

// Tracking Tool
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComponentMap.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetAttributes.h"

#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

// Forward declaration

namespace met {

  // typedefs
  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  class METRebuilder
  : public asg::AsgTool,
  virtual public IMETRebuilder

  {
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METRebuilder, IMETRebuilder)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    METRebuilder(const std::string& name);

    /// Destructor:
    virtual ~METRebuilder() = default;

    // Athena algtool's Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

    virtual StatusCode copyMET(const std::string& metKey,
                               xAOD::MissingETContainer* metCont,
                               const xAOD::MissingETComponentMap* metMap) override;

    virtual StatusCode rebuildMET(const std::string& metKey,
                                  xAOD::MissingETContainer* metCont,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETComponentMap* metMap,
                                  bool doTracks=true) override;

    virtual StatusCode rebuildMET(xAOD::MissingET* met,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETComponent* component,
                                  bool doTracks=true) override;

    virtual StatusCode rebuildJetMET(const std::string& jetKey,
                                     const std::string& softKey,
                                     xAOD::MissingETContainer* metCont,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETComponentMap* metMap,
                                     bool doTracks=true) override
    {
      return rebuildJetMET(jetKey,softKey,metCont,jets,metMap,doTracks,
                           m_jetDoJvf,m_pureTrkSoft,m_softJetScale);
    }

    virtual StatusCode rebuildJetMET(const std::string& jetKey,
                                     const std::string& softKey,
                                     xAOD::MissingETContainer* metCont,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETComponentMap* metMap,
                                     bool doTracks,
                                     bool doJvfCut,
                                     bool pureTrkSoft,
                                     const std::string& softJetScale) override;

    virtual StatusCode rebuildJetMET(const std::string& jetKey,
                                     const std::string& softKey,
                                     xAOD::MissingETContainer* metCont,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETComponentMap* metMap,
                                     bool doTracks,
                                     bool doJvfCut,
                                     bool pureTrkSoft,
                                     const std::string& softJetScale,
                                     float& stvf) override;

    virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
                                     xAOD::MissingET* metSoft,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETComponent* component,
                                     bool doTracks,
                                     bool doJvfCut,
                                     bool pureTrkSoft,
                                     const std::string& softJetScale,
			                               float& stvf,
			                               const xAOD::MissingETComponent* comp_softtrk=0) override;

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:
    bool acceptTrack(const xAOD::TrackParticle* trk,
                     const xAOD::Vertex* pv) const;
    void associateTracks(const xAOD::IParticle* obj);

    /// Default constructor:
    METRebuilder();

    // ReadHandleKey(s)
    SG::ReadHandleKey<xAOD::MissingETComponentMap>  m_METMapKey{this, "InputMap", "METMap_RefFinal", ""};
    SG::ReadHandleKey<xAOD::ElectronContainer>      m_ElectronContainerKey{this, "EleColl", "ElectronCollection", ""};
    SG::ReadHandleKey<xAOD::PhotonContainer>        m_PhotonContainerKey{this, "GammaColl", "PhotonCollection", ""};
    SG::ReadHandleKey<xAOD::TauJetContainer>        m_TauJetContainerKey{this, "TauColl", "TauRecContainer", ""};
    SG::ReadHandleKey<xAOD::MuonContainer>          m_MuonContainerKey{this, "MuonColl", "Muons", ""};
    SG::ReadHandleKey<xAOD::JetContainer>           m_JetContainerKey{this, "JetColl", "AntiKt4LCTopoJets", ""};
    SG::ReadHandleKey<xAOD::VertexContainer>        m_PVKey{this, "VertexColl", "PrimaryVertices", ""};
    // WriteHandleKey(s)
    SG::WriteHandleKey<xAOD::MissingETContainer>    m_OutMETKey{this, "OutputContainer", "MET_MyRefFinal", ""};

    //
    Gaudi::Property<std::string> m_eleTerm{this, "EleTerm", "RefEle", ""};
    Gaudi::Property<std::string> m_gammaTerm{this, "GammaTerm", "RefGamma", ""};
    Gaudi::Property<std::string> m_tauTerm{this, "TauTerm", "RefTau", ""};
    Gaudi::Property<std::string> m_jetTerm{this, "JetTerm", "RefJet", ""};
    Gaudi::Property<std::string> m_muonTerm{this, "MuonTerm", "Muons", ""};
    Gaudi::Property<std::string> m_softTerm{this, "SoftTerm", "", ""};
    Gaudi::Property<std::string> m_softTermType{this, "SoftTermType", "TrackSoftTerm", ""};
    //
    Gaudi::Property<std::string> m_outMETTerm{this, "OutputTotal", "Final", ""};


    bool m_doEle{};
    bool m_doGamma{};
    bool m_doTau{};
    bool m_doMuon{};

    bool m_rebuildEle{};
    bool m_rebuildGamma{};
    bool m_rebuildTau{};
    bool m_rebuildMuon{};

    // For jet/soft term -- eventually break off into a separate tool
    Gaudi::Property<double> m_jetPtCut{this, "CalibJetPtCut", 20e3, ""};
    Gaudi::Property<bool> m_jetDoJvf{this, "DoJetJVFCut", true, ""};
    Gaudi::Property<double> m_jetJvfCut{this, "CalibJetJvfCut", 0.25, ""};
    Gaudi::Property<std::string> m_softJetScale{this, "SoftJetScale", "", ""};
    bool m_doTracks{true};
    bool m_pureTrkSoft{true};
    Gaudi::Property<bool> m_doSTVF{this, "ComputeSTVF", false, ""};

    // Decorate tracks to state that they have been used for a MET calc
    SG::AuxElement::Decorator<char> m_trkUsedDec{"usedByMET"};

    Gaudi::Property<bool> m_trk_doPVsel{this, "DoTrackPVSel", true, ""};
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;
  };

} //> end namespace met
#endif //> !METUTILITIES_MET_METREBUILDER_H
