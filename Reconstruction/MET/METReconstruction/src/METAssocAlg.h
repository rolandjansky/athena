///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METAssocAlg.h

#ifndef METAssocAlg_H
#define METAssocAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODMissingET/MissingETAssociationMap.h"

#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"
#include "PFlowUtils/IRetrievePFOTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

class IMETRecoTool;

namespace met {
  class METAssocAlg : public AthAlgorithm { 

  public: 

    /// Constructor with parameters:
    METAssocAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METAssocAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METAssocAlg();

    StatusCode setupJetAssoc(xAOD::MissingETAssociationMap* metMap,
			     const xAOD::JetContainer* jets,
			     const xAOD::Vertex* pv);
    StatusCode setupMuons(xAOD::MissingETAssociationMap* metMap,
			  const xAOD::MuonContainer* muons,
			  const xAOD::Vertex* pv,
			  const xAOD::PFOContainer* pfoCont);
    //			  std::vector<const xAOD::IParticle*>& mutracks);
    StatusCode setupElectrons(xAOD::MissingETAssociationMap* metMap,
			      const xAOD::ElectronContainer* electrons,
			      const xAOD::CaloClusterContainer* clusters,
			      const xAOD::Vertex* pv,
			      const xAOD::PFOContainer* pfoCont);
			      // std::vector<const xAOD::IParticle*>& mutracks);
    StatusCode setupPhotons(xAOD::MissingETAssociationMap* metMap,
			    const xAOD::PhotonContainer* photons,
			    const xAOD::CaloClusterContainer* clusters,
			    const xAOD::Vertex* pv,
			    const xAOD::PFOContainer* pfoCont);
			    // std::vector<const xAOD::IParticle*>& mutracks);
    StatusCode setupTaus(xAOD::MissingETAssociationMap* metMap,
			 const xAOD::TauJetContainer* taus,
			 const xAOD::Vertex* pv,
			 const xAOD::PFOContainer* pfoCont);
			 // std::vector<const xAOD::IParticle*>& mutracks);

    void extractTopoClusters(const xAOD::Egamma* eg,
			     const xAOD::CaloClusterContainer* tcCont,
			     std::vector<const xAOD::IParticle*>& tclist,
			     MissingETBase::Types::constvec_t& tcvec);
    void extractTopoClusters(const xAOD::TauJet* tau,
			     std::vector<const xAOD::IParticle*>& tclist,
			     MissingETBase::Types::constvec_t& tcvec);
    void extractTracks(const xAOD::TauJet* tau,
		       const xAOD::Vertex* pv,
		       std::vector<const xAOD::IParticle*>& constlist,
		       MissingETBase::Types::constvec_t& trkvec);
    void extractTracks(const xAOD::Electron* el,
		       const xAOD::Vertex* pv,
		       std::vector<const xAOD::IParticle*>& constlist,
		       MissingETBase::Types::constvec_t& trkvec);
    void extractTracks(const xAOD::Photon* ph,
		       const xAOD::Vertex* pv,
		       std::vector<const xAOD::IParticle*>& constlist,
		       MissingETBase::Types::constvec_t& trkvec);
    void extractPFO(const xAOD::TauJet* tau,
		    const xAOD::PFOContainer* pfoCont,
		    std::vector<const xAOD::IParticle*>& pfolist,
		    MissingETBase::Types::constvec_t& pfovec,
		    MissingETBase::Types::constvec_t& trkvec,
		    const xAOD::Vertex* pv);
    void extractPFO(const xAOD::Egamma* eg,
		    const xAOD::PFOContainer* pfoCont,
		    std::vector<const xAOD::IParticle*>& pfolist,
		    MissingETBase::Types::constvec_t& pfovec,
		    MissingETBase::Types::constvec_t& trkvec,
		    const xAOD::Vertex* pv);

    bool acceptTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* pv);
    bool acceptChargedPFO(const xAOD::TrackParticle* trk, const xAOD::Vertex* pv);
    bool isGoodEoverP(const xAOD::TrackParticle* trk);
    void filterTracks(const xAOD::TrackParticleContainer* tracks,
		      const xAOD::Vertex* pv);
    std::vector<const xAOD::IParticle*> selectTracks(const std::vector<const xAOD::IParticle*>& tracksin,
						     const xAOD::Vertex* pv);

    // store list of accepted tracks for track isolation checks
    std::vector<const xAOD::TrackParticle*> m_goodtracks;

    // configurable data members
    std::string m_contname;
    std::string m_mapname;

    std::string m_jetcoll;
    std::string m_elecoll;
    std::string m_gammacoll;
    std::string m_taucoll;
    std::string m_muoncoll;
    std::string m_pvcoll;
    std::string m_trkcoll;
    std::string m_clcoll;

    bool m_pflow;    

    ToolHandle<CP::IRetrievePFOTool> m_pfotool;
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;

  };

}

#endif
