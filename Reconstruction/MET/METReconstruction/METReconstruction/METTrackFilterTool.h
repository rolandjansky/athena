///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTrackFilterTool.h 
// Header file for class METTrackFilterTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTRACKFILTERTOOL_H
#define METRECONSTRUCTION_METTRACKFILTERTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METRefinerTool.h"

// Forward declaration
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/ElectronContainer.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

// Tool interfaces
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"

#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"


namespace met{

  class METTrackFilterTool
    : public METRefinerTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTrackFilterTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METTrackFilterTool(const std::string& name);
    ~METTrackFilterTool();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  protected: 
    StatusCode executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;
    // Accept functions
    // bool isPVTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* pv) const;
    bool isGoodEoverP(const xAOD::TrackParticle* trk,
		      const std::vector<const xAOD::TrackParticle*>& trkList,
		      const xAOD::CaloClusterContainer* clusters) const;

  private:
    // Default constructor: 
    METTrackFilterTool();

    StatusCode buildTrackMET(xAOD::MissingETComponentMap* const metMap,
			     xAOD::MissingET* const metTerm,
			     const xAOD::Vertex* const pv,
			     const std::vector<const xAOD::Electron*>& selElectrons,
			     const std::vector<const xAOD::Muon*>& selMuons,
			     const std::vector<const xAOD::TrackParticle*>& softTracks) const;

    bool isElTrack(const xAOD::TrackParticle &trk, const std::vector<const xAOD::Electron*>& electrons, size_t &el_index ) const;
    bool isMuTrack(const xAOD::TrackParticle &trk, const std::vector<const xAOD::Muon*>& muons) const;

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool{this,"TrackSelectorTool","",""};
    ToolHandle<CP::ITrackVertexAssociationTool> m_trkToVertexTool{this,"TrackVxAssocTool","",""};
    ToolHandle<xAOD::ITrackIsolationTool> m_trkIsolationTool{this,"TrackIsolationTool","",""};
    ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloIsolationTool{this,"CaloIsolationTool","",""};

    void selectElectrons(const xAOD::ElectronContainer &elCont, std::vector<const xAOD::Electron*>& electrons) const;
    void selectMuons(const xAOD::MuonContainer &muCont, std::vector<const xAOD::Muon*>& muons) const;

    bool m_trk_doPVsel;

    SG::ReadHandleKey<xAOD::ElectronContainer>      m_el_inputkey{this,"InputElectronKey","Electrons",""};
    SG::ReadHandleKey<xAOD::MuonContainer>          m_mu_inputkey{this,"InputMuonKey","Muons",""};
    SG::ReadHandleKey<xAOD::VertexContainer>  m_pv_inputkey{this,"InputPVKey","PrimaryVertices",""};
    SG::ReadHandleKey<xAOD::CaloClusterContainer>  m_cl_inputkey{this,"InputClusterKey","CaloCalTopoClusters",""};

    bool m_doVxSep;
    bool m_doLepRecovery;
    bool m_useIsolationTools;

    bool m_trk_doEoverPsel;

    double m_cenTrackPtThr;
    double m_forTrackPtThr;
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METTRACKFILTERTOOL_H
