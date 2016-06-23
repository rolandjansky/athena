// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetAuxContainer_v2.h 750177 2016-05-27 08:12:45Z krasznaa $
#ifndef XAODTAU_VERSIONS_TAUJETAUXCONTAINER_V2_H
#define XAODTAU_VERSIONS_TAUJETAUXCONTAINER_V2_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

//local includes
#include "xAODTau/TauDefs.h"

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
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
  class TauJetAuxContainer_v2 : public AuxContainerBase {

  public:
    /// Default constructor
    TauJetAuxContainer_v2();

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
    std::vector< float > etaTauEnergyScale;
    std::vector< float > phiTauEnergyScale;
    std::vector< float > mTauEnergyScale;

    std::vector< float > ptTauEtaCalib;
    std::vector< float > etaTauEtaCalib;
    std::vector< float > phiTauEtaCalib;
    std::vector< float > mTauEtaCalib;
   

    // std::vector< float > ptPanTauEFlowRecProto;
    // std::vector< float > etaPanTauEFlowRecProto;
    // std::vector< float > phiPanTauEFlowRecProto;
    // std::vector< float > mPanTauEFlowRecProto;
   
    // std::vector< float > ptPanTauEFlowRec;
    // std::vector< float > etaPanTauEFlowRec;
    // std::vector< float > phiPanTauEFlowRec;
    // std::vector< float > mPanTauEFlowRec;
   
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
   
        
             


    std::vector< unsigned int > ROIWord;
    std::vector< float > charge;

    std::vector< uint32_t > vetoFlags;
    std::vector< uint32_t > isTauFlags;
    
    std::vector< float > Likelihood;
    std::vector< float > SafeLikelihood;
    std::vector< float > BDTJetScore;
    std::vector< float > BDTEleScore;

    //additional discriminant output
    std::vector< float > BDTJetScoreSigTrans;
    std::vector< float > BDTJetScoreBkgTrans;

    typedef std::vector< ElementLink< TrackParticleContainer > > TrackLink_t;
    std::vector< TrackLink_t > trackLinks;
    std::vector< TrackLink_t > wideTrackLinks;
    std::vector< TrackLink_t > otherTrackLinks;


    std::vector< std::vector < uint8_t > > trackFlags;
    std::vector< int > trackFilterProngs;
    std::vector< int > trackFilterQuality;             
    std::vector< float > pi0ConeDR;             

    std::vector< std::vector < float > > trackEtaStrip;
    std::vector< std::vector < float > > trackPhiStrip;


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
    std::vector< float > ele_E237E277;
    std::vector< float > ele_PresamplerFraction;
    std::vector< float > ele_ECALFirstFraction;
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
    std::vector< float > etEMCalib;
    /// calibrated hadronic transverse energy
    std::vector< float > etHadCalib;
    /// eta of TauJet calculated from calorimeter
    std::vector< float > seedCalo_eta;
    /// phi of TauJet calculated from calorimeter
    std::vector< float > seedCalo_phi;
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
    std::vector< float > EM_TES_scale;
    /// LC+TES precalibration
    std::vector< float > LC_TES_precalib;
 
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
    std::vector< float > etEflow;
    std::vector< float > mEflow;
    std::vector< float > ptRatioEflow;
    std::vector< int > nPi0;
    std::vector< int > nCharged;

    std::vector< float > etEflowTopo;
    std::vector< float > mEflowTopo;
    std::vector< float > ptRatioEflowTopo;
    std::vector< int > nPi0Topo;
    std::vector< int > nChargedTopo;

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
    std::vector< int > pantau_CellBasedInput_isPanTauCandidate;
    //decay modes (input mode [proto] and output mode )
    std::vector< int > pantau_CellBasedInput_DecayModeProto;
    std::vector< int > pantau_CellBasedInput_DecayMode;
    //BDT output distributions
    std::vector< float > pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n;
    std::vector< float > pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn;
    std::vector< float > pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn;
    //Variables used in BDTs
    std::vector< int > pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts;
    std::vector< float > pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt;
    std::vector< float > pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts;
    std::vector< float > pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM;
    std::vector< float > pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1;
    std::vector< float > pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2;
    std::vector< float > pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    std::vector< float > pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts;
    std::vector< float > pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed;
    std::vector< float > pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged;
    std::vector< float > pantau_CellBasedInput_BDTVar_Charged_HLV_SumM;

    
    
    
  }; // class TauJetAuxContainer_v2

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TauJetAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTAU_VERSIONS_TAUJETAUXCONTAINER_V2_H
