// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_VERSIONS_TAUJETAUXCONTAINER_V3_H
#define XAODTAU_VERSIONS_TAUJETAUXCONTAINER_V3_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

//local includes
#include "xAODTau/TauDefs.h"

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/PFOContainer.h"

namespace xAOD {


  /// Temporary container used until we have I/O for AuxStoreInternal
  ///
  /// This class is meant to serve as a temporary way to provide an auxiliary
  /// store with Athena I/O capabilities for the tau EDM. Will be exchanged for
  /// a generic auxiliary container type (AuxStoreInternal) later on.
  ///
  /// @author Michel Janus <janus@cern.ch>
  ///
  ///
  class TauJetAuxContainer_v3 : public AuxContainerBase {

  public:
    /// Default constructor
    TauJetAuxContainer_v3();

  private:
    std::vector< float > pt;
    std::vector< float > eta;
    std::vector< float > phi;
    std::vector< float > m;

    std::vector< float > ptJetSeed;
    std::vector< float > etaJetSeed;
    std::vector< float > phiJetSeed;
    std::vector< float > mJetSeed;

    std::vector< float > ptDetectorAxis;
    std::vector< float > etaDetectorAxis;
    std::vector< float > phiDetectorAxis;
    std::vector< float > mDetectorAxis;

    std::vector< float > ptIntermediateAxis;
    std::vector< float > etaIntermediateAxis;
    std::vector< float > phiIntermediateAxis;
    std::vector< float > mIntermediateAxis;

    std::vector< float > ptTauEnergyScale;
    std::vector< float > etaTauEnergyScale; // same as IntermediateAxis
    std::vector< float > phiTauEnergyScale; // as IntermediateAxis
    std::vector< float > mTauEnergyScale; // is 0

    std::vector< float > ptTauEtaCalib;
    std::vector< float > etaTauEtaCalib;
    std::vector< float > phiTauEtaCalib; // same as IntermediateAxis
    std::vector< float > mTauEtaCalib; // is 0
   
    std::vector< float > ptPanTauCellBasedProto;
    std::vector< float > etaPanTauCellBasedProto;
    std::vector< float > phiPanTauCellBasedProto;
    std::vector< float > mPanTauCellBasedProto;
   
    std::vector< float > ptPanTauCellBased;
    std::vector< float > etaPanTauCellBased;
    std::vector< float > phiPanTauCellBased;
    std::vector< float > mPanTauCellBased;

    std::vector< float > ptTrigCaloOnly;
    std::vector< float > etaTrigCaloOnly;
    std::vector< float > phiTrigCaloOnly;
    std::vector< float > mTrigCaloOnly;

    std::vector< float > ptFinalCalib;
    std::vector< float > etaFinalCalib;
    std::vector< float > phiFinalCalib;
    std::vector< float > mFinalCalib;


    std::vector< unsigned int > ROIWord;
    std::vector< float > charge;

    //    std::vector< uint32_t > vetoFlags;//r21 cleanup
    std::vector< uint32_t > isTauFlags;
    
    // std::vector< float > Likelihood;
    // std::vector< float > SafeLikelihood;
    std::vector< float > BDTJetScore;
    std::vector< float > BDTEleScore;
    std::vector< float > EleMatchLikelihoodScore;//from TauEleOLRDecorator

    //additional discriminant output
    std::vector< float > BDTJetScoreSigTrans;
    // std::vector< float > BDTJetScoreBkgTrans;

    typedef std::vector< ElementLink< TauTrackContainer > > TauTrackLink_t;
    std::vector< TauTrackLink_t > tauTrackLinks;

    //simply nTracks(TauJetParameters::TauTrackFlag::classifiedCharged)
    //after the first pass of TauTrackFinder
    std::vector< int > nChargedTracks;
    std::vector< int > nIsolatedTracks;

    typedef std::vector< ElementLink< IParticleContainer > > IParticleLink_t;
    std::vector< IParticleLink_t > clusterLinks;//actually xAOD::CaloClusters
    std::vector< IParticleLink_t > pi0Links;//acutally xAOD::Particles


    std::vector< int > trackFilterProngs;
    std::vector< int > trackFilterQuality;             
    std::vector< float > pi0ConeDR;             

    typedef ElementLink< xAOD::JetContainer > JetLink_t;
    std::vector< JetLink_t > jetLink;

    typedef ElementLink< xAOD::VertexContainer > VertexLink_t;
    std::vector< VertexLink_t > vertexLink;
    std::vector< VertexLink_t > secondaryVertexLink;


    // //pfo links for substructure
    typedef std::vector< ElementLink< PFOContainer > > PFOLink_t;

    std::vector< PFOLink_t > chargedPFOLinks;// (Links to default charged PFOs)
    std::vector< PFOLink_t > neutralPFOLinks;// (Links to default all neutral PFOs)
    std::vector< PFOLink_t > pi0PFOLinks;// (Links to default pi0 neutral PFOs)
    std::vector< PFOLink_t > protoChargedPFOLinks;// (Links to cell-based charged PFOs)
    std::vector< PFOLink_t > protoNeutralPFOLinks;// (Links to cell-based all neutral PFOs)
    std::vector< PFOLink_t > protoPi0PFOLinks;// (Links to cell-based pi0 neutral PFOs)
    std::vector< PFOLink_t > shotPFOLinks;// (Links to shots)
    std::vector< PFOLink_t > hadronicPFOLinks;// (Links to hadronic pfos)
    
    //set of details variables
    std::vector< float > ipZ0SinThetaSigLeadTrk;
    std::vector< float > etOverPtLeadTrk;
    std::vector< float > leadTrkPt;
    std::vector< float > ipSigLeadTrk;
    std::vector< float > massTrkSys;
    std::vector< float > trkWidth2;
    std::vector< float > trFlightPathSig;
    ///electron LLH variables
    // std::vector< float > ele_E237E277; //r21 cleanup
    // std::vector< float > ele_PresamplerFraction; //r21 cleanup
    // std::vector< float > ele_ECALFirstFraction; //r21 cleanup
    //for topocluster ID variables
    std::vector< int > numCells;
    ///number of topocluster constituents of jet associated to tau candidate
    std::vector< int > numTopoClusters;
    ///number of effective topocluster constituents of jet associated to tau candidate !!! is a std::vector< float >
    std::vector< float > numEffTopoClusters;
    ///invariant mass from topoclusters of jet associated to tau candidate 
    std::vector< float > topoInvMass;
    ///invariant mass from effective topoclusters of jet associated to tau candidate 
    std::vector< float > effTopoInvMass;
    ///mean distance to leading topocluster for topoclusters of jet associated to tau candidate
    std::vector< float > topoMeanDeltaR;
    ///mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
    std::vector< float > effTopoMeanDeltaR;
    /// E_T radius
    std::vector< float > EMRadius;
    /// hadron calorimeter radius
    std::vector< float > hadRadius;
    /// EM energy at EM scale
    std::vector< float > etEMAtEMScale;
    /// Hadronic energy at EM scale
    std::vector< float > etHadAtEMScale;
    /// isolation fraction
    std::vector< float > isolFrac;
    /// centrality fraction
    std::vector< float > centFrac;
    /// strip width ^2
    std::vector< float > stripWidth2;
    /// number of strips
    std::vector< int > nStrip;
    /// calibrated EM transverse energy
    //    std::vector< float > etEMCalib;//r21 cleanup
    /// calibrated hadronic transverse energy
    //    std::vector< float > etHadCalib;//r21 cleanup
    /// eta of TauJet calculated from calorimeter
    //std::vector< float > seedCalo_eta; //r21 cleanup DetectorAxis eta
    /// phi of TauJet calculated from calorimeter
    //    std::vector< float > seedCalo_phi;//r21 cleanup DetectorAxis phi
    /// the average track distance to calorimeter seed
    std::vector< float > trkAvgDist;
    /// the RMS of track distance to calorimeter seed
    std::vector< float > trkRmsDist;
    ///  sum of 2 leading cluster energy over all cluster energy
    std::vector< float > lead2ClusterEOverAllClusterE;
    ///  sum of 3 leading cluster energy over all cluster energy
    std::vector< float > lead3ClusterEOverAllClusterE;
    ///  sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
    std::vector< float > caloIso;
    ///  sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4 (pile-up corrected)  
    std::vector< float > caloIsoCorrected;
    ///  maximal dR of tracks associated to calo-seeded tau
    std::vector< float > dRmax;
    
    ///migrate only seedTrk_ variables which are used in reco and ID and without prefix
    std::vector< float > secMaxStripEt;
    std::vector< float > sumEMCellEtOverLeadTrkPt;
    std::vector< float > hadLeakEt;

    /// EM+TES final scale
    //std::vector< float > EM_TES_scale;//r21 cleanup
    /// LC+TES precalibration
    //std::vector< float > LC_TES_precalib; //r21 cleanup
 
    std::vector< float > TESOffset;
    std::vector< float > TESCalibConstant;

    //////////////////////////////////////////////////////////////////////
    //
    // Cell-based energies in rings getters/setters
    //
    //////////////////////////////////////////////////////////////////////
    /// Ring 1: 0.00  < R < 0.05
    std::vector< float > cellBasedEnergyRing1;
    /// Ring 2: 0.05  < R < 0.075
    std::vector< float > cellBasedEnergyRing2;
    /// Ring 3: 0.075 < R < 0.10
    std::vector< float > cellBasedEnergyRing3;
    /// Ring 4: 0.10  < R < 0.125
    std::vector< float > cellBasedEnergyRing4;
    /// Ring 5: 0.125 < R < 0.15
    std::vector< float > cellBasedEnergyRing5;
    /// Ring 6: 0.15  < R < 0.2
    std::vector< float > cellBasedEnergyRing6;
    /// Ring 7: 0.2   < R < 0.4
    std::vector< float > cellBasedEnergyRing7;
    /// TRT hits high threshold over low threshold
    std::vector< float > TRT_NHT_OVER_NLT;
    /// @Tau Jet Vertex Fraction
    std::vector< float > TauJetVtxFraction;


    //generic substructure details
    //    std::vector< float > etEflow;//r21 cleanup
    //    std::vector< float > mEflow;//r21 cleanup
    //    std::vector< float > ptRatioEflow;//r21 cleanup
    //    std::vector< int > nPi0;//r21 cleanup
    std::vector< int > nCharged; //used in PanTau/PanTauAlgs/src/Tool_DetailsArranger.cxx

    //    std::vector< float > etEflowTopo;//r21 cleanup
    //    std::vector< float > mEflowTopo;//r21 cleanup
    //    std::vector< float > ptRatioEflowTopo;//r21 cleanup
    //    std::vector< int > nPi0Topo;//r21 cleanup
    //    std::vector< int > nChargedTopo;//r21 cleanup

    std::vector< float > mEflowApprox;
    std::vector< float > ptRatioEflowApprox;

    std::vector< float > innerTrkAvgDist;

    std::vector< float > SumPtTrkFrac;

    /// pileup-corrected ID variables
    std::vector< float > etOverPtLeadTrkCorrected;
    std::vector< float > ipSigLeadTrkCorrected;
    std::vector< float > trFlightPathSigCorrected;
    std::vector< float > massTrkSysCorrected;
    std::vector< float > dRmaxCorrected;
    std::vector< float > ChPiEMEOverCaloEMECorrected;
    std::vector< float > EMPOverTrkSysPCorrected;
    std::vector< float > ptRatioEflowApproxCorrected;
    std::vector< float > mEflowApproxCorrected;
    std::vector< float > centFracCorrected;
    std::vector< float > innerTrkAvgDistCorrected;
    std::vector< float > SumPtTrkFracCorrected;



    //trigger details
    std::vector< float > PSSFraction;
    std::vector< float > ChPiEMEOverCaloEME;
    std::vector< float > EMPOverTrkSysP;

    //panTau details
    //!PanTau variables when using CellBased pfos
    //Flag whether this seed has pantau info
    std::vector< int > PanTau_isPanTauCandidate;
    //decay modes (input mode [proto] and output mode )
    std::vector< int > PanTau_DecayModeProto;
    std::vector< int > PanTau_DecayMode;
    //BDT output distributions
    std::vector< float > PanTau_BDTValue_1p0n_vs_1p1n;
    std::vector< float > PanTau_BDTValue_1p1n_vs_1pXn;
    std::vector< float > PanTau_BDTValue_3p0n_vs_3pXn;
    //Variables used in BDTs
    std::vector< int > PanTau_BDTVar_Basic_NNeutralConsts;
    std::vector< float > PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt;
    std::vector< float > PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts;
    std::vector< float > PanTau_BDTVar_Neutral_HLV_SumM;
    std::vector< float > PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1;
    std::vector< float > PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2;
    std::vector< float > PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    std::vector< float > PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts;
    std::vector< float > PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed;
    std::vector< float > PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged;
    std::vector< float > PanTau_BDTVar_Charged_HLV_SumM;

    // RNN tau ID
    std::vector< float > RNNJetScore;
    std::vector< float > RNNJetScoreSigTrans;

    // RNN tau eVeto
    std::vector< float > RNNEleScore;
    std::vector< float > RNNEleScoreSigTrans;
    
  }; // class TauJetAuxContainer_v3

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TauJetAuxContainer_v3, xAOD::AuxContainerBase );

#endif // XAODTAU_VERSIONS_TAUJETAUXCONTAINER_V3_H
