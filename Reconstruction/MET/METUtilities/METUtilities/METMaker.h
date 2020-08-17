///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMaker.h
// Header file for class METMaker
// Author: T.J.Khoo<khoo@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef METUTILITIES_MET_METMAKER_H
#define METUTILITIES_MET_METMAKER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METInterface includes
#include "METInterface/IMETMaker.h"

// EDM includes
#include "xAODJet/JetContainer.h"

// Tracking Tool
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// Forward declaration

namespace met {

  // typedefs
  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  class METMaker
  : public asg::AsgTool,
  virtual public IMETMaker

  {
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METMaker, IMETMaker)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    METMaker(const std::string& name);

    /// Destructor:
    virtual ~METMaker();

    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    StatusCode rebuildMET(const std::string& metKey,
                          xAOD::Type::ObjectType metType,
                          xAOD::MissingETContainer* metCont,
                          const xAOD::IParticleContainer* collection,
                          xAOD::MissingETAssociationHelper* helper,
                          MissingETBase::UsageHandler::Policy objScale);
    //
    StatusCode rebuildMET(xAOD::MissingET* met,
                          const xAOD::IParticleContainer* collection,
                          xAOD::MissingETAssociationHelper* helper,
                          MissingETBase::UsageHandler::Policy objScale);
    //
    StatusCode rebuildMET(xAOD::MissingET* met,
                          const xAOD::IParticleContainer* collection,
                          xAOD::MissingETAssociationHelper* helper,
                          MissingETBase::UsageHandler::Policy p,
                          bool removeOverlap,
                          MissingETBase::UsageHandler::Policy objScale);

    StatusCode rebuildJetMET(const std::string& metJetKey,
                             const std::string& softClusKey,
                             const std::string& softTrkKey,
                             xAOD::MissingETContainer* metCont,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETContainer* metCoreCont,
                             xAOD::MissingETAssociationHelper* helper,
                             bool doJetJVT);
    StatusCode rebuildJetMET(const std::string& metJetKey,
                             const std::string& metSoftKey,
                             xAOD::MissingETContainer* metCont,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETContainer* metCoreCont,
                             xAOD::MissingETAssociationHelper* helper,
                             bool doJetJVT);
    StatusCode rebuildJetMET(xAOD::MissingET* metJet,
                             const xAOD::JetContainer* jets,
                             xAOD::MissingETAssociationHelper* helper,
                             xAOD::MissingET* metSoftClus,
                             const xAOD::MissingET* coreSoftClus,
                             xAOD::MissingET* metSoftTrk,
                             const xAOD::MissingET* coreSoftTrk,
                             bool doJetJVT,
                             bool tracksForHardJets = false,
                             std::vector<const xAOD::IParticle*>* softConst=0);

    StatusCode rebuildTrackMET(const std::string& metJetKey,
                             const std::string& softTrkKey,
                             xAOD::MissingETContainer* metCont,
                             const xAOD::JetContainer* jets,
                             const xAOD::MissingETContainer* metCoreCont,
                             xAOD::MissingETAssociationHelper* helper,
                             bool doJetJVT);
    StatusCode rebuildTrackMET(xAOD::MissingET* metJet,
                             const xAOD::JetContainer* jets,
                             xAOD::MissingETAssociationHelper* helper,
                             xAOD::MissingET* metSoftTrk,
                             const xAOD::MissingET* coreSoftTrk,
                             bool doJetJVT);

    StatusCode markInvisible(const xAOD::IParticleContainer* collection,
			     xAOD::MissingETAssociationHelper* helper,
			     xAOD::MissingETContainer* metCont);

    StatusCode buildMETSum(const std::string& totalName,
                           xAOD::MissingETContainer* metCont,
                           MissingETBase::Types::bitmask_t softTermsSource);

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

    StatusCode fillMET(xAOD::MissingET *& met,
		       xAOD::MissingETContainer * metCont,
		       const std::string& metKey,
		       const MissingETBase::Types::bitmask_t metSource);
    bool acceptTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* vx) const;
    const xAOD::Vertex* getPV() const;


    SG::ReadHandleKey<xAOD::VertexContainer>  m_PVkey;
    // std::string m_pvcoll;

    bool m_jetCorrectPhi;
    double m_jetMinEfrac;
    double m_jetMinWeightedPt;
    std::string m_jetConstitScaleMom;
    std::string m_jetJvtMomentName;
    std::string m_jetRejectionDec;

    double m_CenJetPtCut, m_FwdJetPtCut ; // jet pt cut for central/forward jets(eta<2.4)
    double m_JvtCut, m_JvtPtMax; // JVT cut and pt region of jets to apply a JVT selection
    std::string m_jetSelection;
    // Extra configurables for custom WP
    double m_customCenJetPtCut,m_customFwdJetPtCut;
    double m_customJvtCut,m_customJvtPtMax;

    bool m_extraJetRejection;
    bool m_doPFlow;
    bool m_doSoftTruth;
    bool m_doConstJet;

    bool m_useGhostMuons;
    bool m_doRemoveMuonJets;
    bool m_doRemoveElecTrks;
    bool m_doRemoveElecTrksEM;
    bool m_doSetMuonJetEMScale;

    bool m_muEloss;
    bool m_orCaloTaggedMuon;

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;
    /// Default constructor:
    METMaker();

  };

} //> end namespace met
#endif //> !METUTILITIES_MET_METMAKER_H
