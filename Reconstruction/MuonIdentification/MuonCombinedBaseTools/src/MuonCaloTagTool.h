/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCALOTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCALOTAGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTrigCaloTagExtensionTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/CaloTag.h"

//#include "AthenaBaseComps/AthAlgorithm.h"
//#include "StoreGate/DataHandle.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "TrkParameters/TrackParameters.h"
// #include "IsolationTool/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"

// - NEW
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
//
#include "muonEvent/DepositInCalo.h"

#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "ICaloTrkMuIdTools/ICaloMuonTag.h"
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

// - STL
#include <vector>


namespace MuonCombined {

  class MuonCaloTagTool: public AthAlgTool, virtual public IMuonCombinedInDetExtensionTool, virtual public IMuonCombinedTrigCaloTagExtensionTool
  {

  public:
    MuonCaloTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCaloTagTool(void); // destructor
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */    
    virtual
    void extend( const InDetCandidateCollection& inDetCandidates ) override;

    virtual
    void extend( const InDetCandidateCollection& inDetCandidates,
                 const CaloCellContainer* caloCellContainer,
                 const xAOD::CaloClusterContainer* caloClusterContainer) override;

    virtual void extendWithPRDs(const InDetCandidateCollection& inDetCandidates, const Muon::MdtPrepDataContainer* mdtPRDs, const Muon::CscPrepDataContainer* cscPRDs,
				const Muon::RpcPrepDataContainer* rpdPRDs, const Muon::TgcPrepDataContainer* tgcPRDs, const Muon::sTgcPrepDataContainer* sTGCPRDs,
                                const Muon::MMPrepDataContainer* mmPRDs) override;


  private:
    
    void createMuon(InDetCandidate & muonCandidate, const std::vector<DepositInCalo>& deposits, int tag, float likelihood) const;
    const Trk::Vertex* retrievePrimaryVertex() const;
    const Trk::TrackParameters* getTrackParameters(const Trk::Track* trk) const;
    //bool applyEnergyIsolation(const xAOD::IParticle* muon);
    bool selectTrack(const Trk::Track* trk, const Trk::Vertex* vertex) const;
    bool selectCosmic(const Trk::Track* ptcl) const;
    bool applyTrackIsolation(const xAOD::TrackParticle& tp);
    void showTrackInfo(const Trk::TrackParameters* par) const;
//    std::vector<DepositInCalo> getDeposits(const xAOD::TrackParticle* tp, const CaloCellContainer* caloCellCont) const;
    
    // --- StoreGate keys ---
    std::string m_TrackParticleName;                 //!< TrackParticle container name
    std::string m_VertexContainerName;               //!< name of the vertex container
    //std::string m_MuonContainerOutputName;           //!< Calo Muon Collection name
    //std::string m_muonCaloEnergyCollectionName;      //!< Muon CaloEnergy Collection
    
    // --- Internal cache ---
    mutable int m_nTrueMuons;          //!< Counts the number true muons
    mutable int m_nTracksTagged;       //!< Counts the number of tracks tagged
    mutable int m_nMuonsTagged;        //!< Counts the number of truth muons tagged
    
    // --- Set up what to do and what not to do ---
    bool m_doCaloMuonTag;               //!< run CaloMuonTag Tool
    bool m_doCaloLR;                    //!< run CaloMuonLikelihoodTool
    bool m_doTrkSelection;              //!< This variable should be set to false when there is no primary vertex reconstructed.
    bool m_doCosmicTrackSelection;      //!< Apply track selection for cosmics
    
    // --- Debugging options ---
    bool m_showCutFlow;                 //!< Verbose track selection and track isolation
    bool m_doTruth;                     //!< Display truth info for each analysed track
    bool m_debugMode;                   //!< Switch for extra printout
    bool m_doOldExtrapolation;          //!< In doubt ? >
    //bool m_doDressing;                  //!< This is to speed up tests where high fake rate is expected. Should be true by default
    bool m_ignoreSiAssocated;           //!< If true, ignore InDetCandidates which are SiAssociated
    
    // --- Isolation cuts ---
    double m_ptIsoPtRatioCut;           //!< Pt isolation for a .45 cone, normalized to track pt
    double m_eIsoBarrelCut;             //!< Energy isolation for a .45 cone in Barrel
    double m_eIsoTransitionCut;         //!< Energy isolation  for a .45 cone in Barrel-EndCap transition region
    double m_eIsoEndCapCut;             //!< Energy isolation for a .45 cone in Endcap
    double m_eIsoPtRatioBarrelCut;      //!< Energy isolation for a .45 cone in Barrel, normalized to track pt
    double m_eIsoPtRatioTransitionCut;  //!< Energy isolation  for a .45 cone in Barrel-EndCap transition region, normalized to track pt
    double m_eIsoPtRatioEndCapCut;      //!< Energy isolation for a .45 cone in Endcap, normalized to track pt
    double m_CaloLRlikelihoodCut;       //!< CaloLR likelihood cut
    //double m_coreDR;                    //!< deltaR of the core
    double m_trackIsoCone;              //!< Cone size for track isolation
    double m_energyIsoCone;             //!< Cone size for energy isolation
    
    // --- CaloTrkMuIdTools ---
    ToolHandle<ICaloMuonLikelihoodTool>  m_caloMuonLikelihood; //!< CaloTrkMuIdTools::CaloMuonLikelihoodTool
    ToolHandle<ICaloMuonTag>             m_caloMuonTagLoose;   //!< CaloTrkMuIdTools::CaloMuonTag for loose tagging
    ToolHandle<ICaloMuonTag>             m_caloMuonTagTight;   //!< CaloTrkMuIdTools::CaloMuonTag for tight tagging
    ToolHandle<ITrackDepositInCaloTool>  m_trkDepositInCalo;   //!< CaloTrkMuIdTools::TrackDepositInCaloTool
    
    // --- External tools ---
    ToolHandle<xAOD::ITrackIsolationTool>  m_trackIsolationTool; //!< track isolation
    ToolHandle<Trk::ITrackSelectorTool>    m_trkSelTool;         //!<  ITrackSelectorTool
  };

}	// end of namespace

#endif


