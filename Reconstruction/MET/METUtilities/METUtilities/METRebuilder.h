///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    virtual ~METRebuilder();

    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    StatusCode  execute();

    StatusCode copyMET(const std::string& metKey,
                       xAOD::MissingETContainer* metCont,
                       const xAOD::MissingETComponentMap* metMap);

    StatusCode rebuildMET(const std::string& metKey,
                          xAOD::MissingETContainer* metCont,
                          const xAOD::IParticleContainer* collection,
                          const xAOD::MissingETComponentMap* metMap,
                          bool doTracks=true);

    StatusCode rebuildMET(xAOD::MissingET* met,
                          const xAOD::IParticleContainer* collection,
                          const xAOD::MissingETComponent* component,
                          bool doTracks=true);

    StatusCode rebuildJetMET(const std::string& jetKey,
                             const std::string& softKey,
                             xAOD::MissingETContainer* metCont,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETComponentMap* metMap,
                             bool doTracks=true) {
      return rebuildJetMET(jetKey,softKey,metCont,jets,metMap,doTracks,
                           m_jetDoJvf,m_pureTrkSoft,m_softJetScale);
    }

    StatusCode rebuildJetMET(const std::string& jetKey,
                             const std::string& softKey,
                             xAOD::MissingETContainer* metCont,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETComponentMap* metMap,
                             bool doTracks,
                             bool doJvfCut,
                             bool pureTrkSoft,
                             const std::string& softJetScale);

    StatusCode rebuildJetMET(const std::string& jetKey,
                             const std::string& softKey,
                             xAOD::MissingETContainer* metCont,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETComponentMap* metMap,
                             bool doTracks,
                             bool doJvfCut,
                             bool pureTrkSoft,
                             const std::string& softJetScale,
			     float& stvf);

    StatusCode rebuildJetMET(xAOD::MissingET* metJet,
                             xAOD::MissingET* metSoft,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETComponent* component,
                             bool doTracks,
                             bool doJvfCut,
                             bool pureTrkSoft,
                             const std::string& softJetScale,
			     float& stvf,
			     const xAOD::MissingETComponent* comp_softtrk=0);

    StatusCode buildMETSum(const std::string& totalName,
                           xAOD::MissingETContainer* metCont);

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
    StatusCode fillMET(xAOD::MissingET *& met,
		       xAOD::MissingETContainer * metCont,
		       const std::string& metKey,
		       const MissingETBase::Types::bitmask_t metSource);



    /// Default constructor:
    METRebuilder();

    // ReadHandleKey(s)
    SG::ReadHandleKey<xAOD::MissingETComponentMap>  m_METMapKey;
    SG::ReadHandleKey<xAOD::MissingETContainer>     m_METContainerKey;
    SG::ReadHandleKey<xAOD::MissingETAuxContainer>  m_METAuxContainerKey;
    SG::ReadHandleKey<xAOD::ElectronContainer>      m_ElectronContainerKey;
    SG::ReadHandleKey<xAOD::PhotonContainer>        m_PhotonContainerKey;
    SG::ReadHandleKey<xAOD::TauJetContainer>        m_TauJetContainerKey;
    SG::ReadHandleKey<xAOD::MuonContainer>          m_MuonContainerKey;
    SG::ReadHandleKey<xAOD::JetContainer>           m_JetContainerKey;
    SG::ReadHandleKey<xAOD::VertexContainer>        m_PVKey;
    // WriteHandleKey(s)
    SG::WriteHandleKey<xAOD::MissingETContainer>    m_OutMETKey;

    std::string m_eleColl;
    std::string m_gammaColl;
    std::string m_tauColl;
    std::string m_jetColl;
    std::string m_muonColl;
    //
    std::string m_eleTerm;
    std::string m_gammaTerm;
    std::string m_tauTerm;
    std::string m_jetTerm;
    std::string m_muonTerm;
    std::string m_softTerm;
    std::string m_softTermType;
    //
    std::string m_inputMap;
    std::string m_outMETCont;
    std::string m_outMETTerm;




    bool m_warnOfDupes;

    bool m_doEle;
    bool m_doGamma;
    bool m_doTau;
    bool m_doMuon;

    bool m_rebuildEle;
    bool m_rebuildGamma;
    bool m_rebuildTau;
    bool m_rebuildMuon;

    // For jet/soft term -- eventually break off into a separate tool
    double m_jetPtCut;
    bool m_jetDoJvf;
    double m_jetJvfCut;
    std::string m_softJetScale;
    bool m_doTracks;
    bool m_pureTrkSoft;
    bool m_doSTVF;

    // Decorate tracks to state that they have been used for a MET calc
    SG::AuxElement::Decorator<char>  m_trkUsedDec;

    bool m_trk_doPVsel;
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;
    std::string m_vtxColl;
    std::string m_clusColl;
  };

} //> end namespace met
#endif //> !METUTILITIES_MET_METREBUILDER_H
