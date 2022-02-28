# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigBphysHypo.TrigBmuxComboHypoMonitoringConfig import TrigBmuxComboHypoMonitoring

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigBmuxComboHypoConfig')

def BmuxComboHypoCfg(name):
    log.debug('BmuxComboHypoCfg.name = %s ', name)
    suffix = 'Bmux'

    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    vertexFitter = CompFactory.Trk__TrkVKalVrtFitter(
        name = 'TrigBphysFitter_'+suffix,
        FirstMeasuredPoint = False,
        MakeExtendedVertex = False,
        Extrapolator = AtlasExtrapolator())

    vertexPointEstimator = CompFactory.InDet__VertexPointEstimator(
        name = 'VertexPointEstimator_'+suffix,
        MinDeltaR = [-10000., -10000., -10000.],
        MaxDeltaR = [ 10000.,  10000.,  10000.],
        MaxPhi    = [ 10000.,  10000.,  10000.],
        MaxChi2OfVtxEstimation = 2000.)

    trackToVertexTool = CompFactory.Reco__TrackToVertex(
        name = 'TrackToVertexTool_'+suffix,
        Extrapolator = AtlasExtrapolator())

    hypo = CompFactory.TrigBmuxComboHypo(
        name = 'BmuxComboHypo',
        VertexFitter = vertexFitter,
        VertexPointEstimator = vertexPointEstimator,
        TrackToVertexTool = trackToVertexTool,
        CheckMultiplicityMap = False,
        TrigBphysCollectionKey = 'HLT_Bmux',
        MuonCollectionKey = 'HLT_Muons_Bmumux',
        TrackCollectionKey = 'HLT_IDTrack_Bmumux_IDTrig',
        DeltaR = 0.01,
        FitAttemptsWarningThreshold = 200,
        FitAttemptsBreakThreshold = 1000,
        # B+ -> mu+ nu_mu anti-D0(-> K+ pi-) and B0 -> mu+ nu_mu D*-(-> anti-D0(-> K+ pi-) pi-)
        BToD0 = True,
        BToD0_makeDstar = True,
        BToD0_minD0KaonPt = 1000.,
        BToD0_minD0PionPt = 1000.,
        BToD0_minDstarPionPt = 1000.,
        BToD0_maxDstarPionZ0 = 5.,
        BToD0_massRange = (-1., 10000.),
        BToD0_D0MassRange = (1750., 2000.),
        BToD0_DstarMassRange = (-1., 2110.),
        BToD0_chi2 = 20.,
        # B0 -> mu+ nu_mu D-(-> K+ pi- pi-)
        BdToD = True,
        BdToD_minKaonPt = 1000.,
        BdToD_minPionPt = 1000.,
        BdToD_massRange = (-1., 10000.),
        BdToD_DMassRange = (1750., 2000.),
        BdToD_chi2 = 27.,
        # B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-)
        BsToDs = True,
        BsToDs_minKaonPt = 1000.,
        BsToDs_minPionPt = 1000.,
        BsToDs_massRange = (-1., 10000.),
        BsToDs_phiMassRange = (940., 1100.),
        BsToDs_DsMassRange = (1850., 2100.),
        BsToDs_chi2 = 27.,
        # Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-)
        LambdaBToLambdaC = True,
        LambdaBToLambdaC_minProtonPt = 2000.,
        LambdaBToLambdaC_minKaonPt = 1000.,
        LambdaBToLambdaC_minPionPt = 1000.,
        LambdaBToLambdaC_massRange = (-1., 10000.),
        LambdaBToLambdaC_LambdaCMassRange = (2190., 2390.),
        LambdaBToLambdaC_chi2 = 27.,
        MonTool = TrigBmuxComboHypoMonitoring('TrigBmuxComboHypoMonitoring'))

    return hypo
