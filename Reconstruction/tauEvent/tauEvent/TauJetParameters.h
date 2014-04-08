/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TauJetParameters.h
 * Definition of parameters for TauJet object
 * 
 * Package: Reconstruction/tauEvent
 *
 *@author
 * Noel Dawe 
 * Thomas Burgess <thomas.burgess@cern.ch>
 * Lukasz Janyst
 * Kyle Cranmer
 *
 * $Id: TauJetParameters.h,v 1.9 2008-11-28 09:52:25 slai Exp $
 */

#ifndef TAU_PARAMETERS_H
#define TAU_PARAMETERS_H

namespace TauJetParameters
{
    //-------------------------------------------------------------------------
    // DO NOT CHANGE THE ORDER OF THE ENUMS!
    // You can add, but changing the order may lead to disaster!
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    //! Enum for author of TauJet.
    //-------------------------------------------------------------------------
    enum Author
    {
        unknown = 0,
        tauRec  = 1,
        tau1P3P = 2,
        PanTau  = 3
    };

    //-------------------------------------------------------------------------
    //! Enum for discriminants
    //-------------------------------------------------------------------------
    enum TauID
    {
        //! discriminant for NN (used by tau1p3p)
        //DiscNN                = 1, 
        //! discriminant for LL (used by tauRec)
        Likelihood            = 2,
        //! discriminant for PDRS (used by tau1p3p)
        //DiscPDERS             = 3,
        //! discriminant for jet-tau separation LL (used by tauRec) 
        //TauJetLikelihoodOld   = 4,
        //! old discriminant for electron-tau separation LL (used by tauRec)
        //TauElLikelihoodOld    = 5,
        //! discriminant for NN (used by tauRec)
        //TauJetNeuralNetwork   = 6,
        //! discriminant for NN electron-tau separation (used by tauRec)   
        //TauENeuralNetwork     = 7,
        //! discriminant for flat acceptance NN (used by tau1p3p)
        //EfficNN               = 8,
        //! discriminant for flat acceptance PDERS (not used currently)
        //EfficPDERS            = 9,
        //! new ie july 2008 discriminant for electron-tau separation LL (used by tauRec)
        //TauElTauLikelihood    = 10,
        //! acceptance flag for cut analysis (used by tau1p3p)
        //DiscCut               = 11,
        //! acceptance flag for cuts optimized with TMVA (used by tau1p3p)
        //DiscCutTMVA           = 12,
        //! discriminant for LL (used by tau1p3p)
        //DiscLL                = 13,
        //! discriminant for flat acceptance LL (not used currently)
        //EfficLL               = 14,
        //! Boosted Decision Tree socre for Jet rejection (not transformed)
        BDTJetScore           = 15,
        //! Boosted Decision Tree score for electron rejection
        BDTEleScore           = 16,
        //! Safe LLH
        SafeLikelihood        = 17,
        //! BDT score which is signal transformed/flattened
        BDTJetScoreSigTrans   = 18, // new
        //! BDT score which is background transformed/flattened
        BDTJetScoreBkgTrans   = 19,  // new
        //! PanTau's Score
        PanTauScore           = 20
    };

    //-------------------------------------------------------------------------
    //! Enum for Veto flags
    //-------------------------------------------------------------------------
    enum VetoFlags
    {
        //! electron veto flag based on cuts (used by tau1p3p)
        ElectronFlag  = 0,
        //! electron veto flag based on egamma information (used by tau1p3p)
        EgammaFlag    = 1,
        //! muon veto flag based on ETCalo cut (used by tau1p3p)
        MuonFlag      = 2
    };

    //-------------------------------------------------------------------------
    //! Enum for IsTau flags
    //-------------------------------------------------------------------------
    enum IsTauFlag
    {
        TauCutLoose          = 0,
        TauCutMedium         = 1,
        TauCutTight          = 2,
        ElectronVeto         = 3, //Obsolete, dont use
        MuonVeto             = 4,
        TauLlhTight          = 5,
        TauLlhMedium         = 6,
        TauLlhLoose          = 7,
        //TauRecOld            = 8, //Obsolete, dont use?
        //Tau1P3POld           = 9, //Obsolete, dont use?
        //TauCutSafeLoose      = 10,
        //TauCutSafeMedium     = 11,
        //TauCutSafeTight      = 12,
        //TauCutSafeCaloLoose  = 13,
        //TauCutSafeCaloMedium = 14,
        //TauCutSafeCaloTight  = 15,
        ElectronVetoLoose    = 16,
        ElectronVetoMedium   = 17,
        ElectronVetoTight    = 18,
        JetBDTSigLoose          = 19, // rename (add sig)
        JetBDTSigMedium         = 20, // rename (add sig)
        JetBDTSigTight          = 21, // rename (add sig)
        EleBDTLoose          = 22,
        EleBDTMedium         = 23,
        EleBDTTight          = 24,
        //SafeLlhLoose         = 25,
        //SafeLlhMedium        = 26,
        //SafeLlhTight         = 27
        JetBDTBkgLoose       = 25, // new
        JetBDTBkgMedium      = 26, // new
        JetBDTBkgTight       = 27,  // new
        PanTauScoreLoose     = 28,
        PanTauScoreMedium    = 29,
        PanTauScoreTight     = 30
    };

    //-------------------------------------------------------------------------
    //! Enum for tau parameters - used mainly for backward compatibility with
    //! the analysis code
    //-------------------------------------------------------------------------
    enum Detail
    {
        etHadCalib               = 0,
        etEMCalib                = 1,
        emRadius                 = 2,
        isolationFraction        = 3,
        centralityFraction       = 4,
        stripWidth2              = 5,
        nStripCells              = 6,

        logLikelihoodRatio       = 7,  //<! Log likelihood ratio between tau and jet
        lowPtTauJetDiscriminant  = 8,  //<! Discriminant (s/(s+b)) for tau vs. jet at low pt
        lowPtTauEleDiscriminant  = 9,  //<! Discriminant (s/(s+b)) for tau vs. ele at low pt
        tauJetNeuralnetwork      = 10, //<! Neural network for tau vs. jet
        tauENeuralNetwork        = 11, //<! Neural netowrk for tau vs. ele

        annularIsolationFraction = 8,  //<! Fraction of energy in radius .1<r<.2
        etCaloAtEMScale          = 9,  //<! Sum of ET cells inside cone RconeTau around the candidate, in EM scale
        etChargedHadCells        = 10, //<! Sum of ET cells in HAD layers within narrow window around track, at EM scale
        etOtherEMCells           = 11, //<! Sum of ET cells in EM layers, between in range between (RcoreTau - RconeTau ), at EM scale
        etOtherHadCells          = 12, //<! Sum of ET cells in HAD layers, between in range between (RcoreTau - RconeTau ), at EM scale
        discriminant             = 13, //<! Tau1P3P tau vs. jet discriminant variable - discCut
        discPDERS                = 14, //<! Tau1P3P tau vs. jet discriminant variable - PDERS
        discNN                   = 15, //<! Tau1P3P tau vs. jet discriminant variable - NN
        sumPTtracks              = 16, //<! Sum of momenta of associated tracks
        nAssocTracksCore         = 17, //<! Number of tracks in the core region
        nAssocTracksIsol         = 18, //<! Number of tracks in the isolation region
        rWidth2Trk3P             = 19, //<! Radius width for track system
        massTrk3P                = 20, //<! Mass of charged tracks system
        signDOTrk3P              = 21  //<! Signed impact parameter from tracks system
    };

    //-------------------------------------------------------------------------
    //! Enum for tau seed type
    //-------------------------------------------------------------------------
    enum SeedTypes
    {
        CaloObject = 0,
        Track      = 1
    };

    //-------------------------------------------------------------------------
    //! Enum for tau seed type
    //-------------------------------------------------------------------------
    enum TauCalibType
    {
        JetSeed=0,
        DetectorAxis=1,
        IntermediateAxis=2,
        TauEnergyScale=3,
        TauEtaCalib=4
    };

}

#endif // TAU_PARAMETERS_H
