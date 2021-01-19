# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigMultiTrkComboHypo, TrigMultiTrkComboHypoTool
from TrigBphysHypo.TrigMultiTrkComboHypoMonitoringConfig import TrigMultiTrkComboHypoMonitoring, TrigMultiTrkComboHypoToolMonitoring

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMultiTrkComboHypoConfig')

trigMultiTrkComboHypoToolDict = {
    'bJpsimumu'     : { 'massRange' : (2500.,  4300.), 'chi2' : 20. },
    'bJpsi'         : { 'massRange' : (2500.,  4300.), 'chi2' : 20. },
    'bJpsimumul2io' : { 'massRange' : (2500.,  4300.), 'chi2' : 20. },
    'bUpsimumu'     : { 'massRange' : (8000., 12000.), 'chi2' : 20. },
    'bUpsi'         : { 'massRange' : (8000., 12000.), 'chi2' : 20. },
    'bDimu'         : { 'massRange' : (1500., 14000.), 'chi2' : 20. },
    'bDimu2700'     : { 'massRange' : ( 100.,  2700.), 'chi2' : 20. },
    'bDimu6000'     : { 'massRange' : ( 100.,  6000.), 'chi2' : 20. },
    'bBmumu'        : { 'massRange' : (4000.,  8500.), 'chi2' : 20. },
    'bPhi'          : { 'massRange' : ( 940.,  1100.), 'chi2' : 10. },
    'bTau'          : { 'massRange' : (   0.,  2700.), 'chi2' : 50. },
}


def DimuL2ComboHypoCfg(name):
    log.debug('DimuL2ComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        trigSequenceName = 'Dimu',
        trigLevel = 'L2',
        trackCollection='HLT_IDTrack_Muon_FTF')
    return hypo

def DimuEFComboHypoCfg(name):
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muonNames
    log.debug('DimuEFComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        trigSequenceName = 'Dimu',
        trigLevel = 'EF',
        muonCollection = muonNames().getNames('RoI').EFCBName)
    return hypo

def TrigMultiTrkComboHypoToolFromDict(chainDict):
    config = TrigMultiTrkComboHypoConfig()
    tool = config.ConfigurationComboHypoTool(chainDict)
    return tool

class TrigMultiTrkComboHypoConfig(object):

    def ConfigurationComboHypo(self, trigSequenceName='Dimu', trigLevel='L2', trackCollection='', muonCollection=''):

        trigLevelDict = {'L2':0, 'EF':1}

        try:
            value = trigLevelDict[trigLevel]
            log.debug('TrigMultiTrkComboHypo.trigLevel = %s ', value)
        except KeyError:
            raise Exception('TrigMultiTrkComboHypo.trigLevel should be L2 or EF, but %s provided.', trigLevel)

        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        VertexFitter = Trk__TrkVKalVrtFitter(
            name = 'TrigBphysFitter_'+trigSequenceName+trigLevel,
            FirstMeasuredPoint = False,
            MakeExtendedVertex = False,
            Extrapolator = AtlasExtrapolator())

        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
        VertexPointEstimator = InDet__VertexPointEstimator(
            name = 'VertexPointEstimator_'+trigSequenceName+trigLevel,
            MinDeltaR = [-10000., -10000., -10000.],
            MaxDeltaR = [ 10000.,  10000.,  10000.],
            MaxPhi    = [ 10000.,  10000.,  10000.],
            MaxChi2OfVtxEstimation = 2000.)

        tool = TrigMultiTrkComboHypo(
            name = trigSequenceName+trigLevel+'ComboHypo',
            trigLevel = trigLevel,
            nTracks = 2,
            massRanges = [ (100., 20000.) ],
            TrackCollectionKey = trackCollection,
            MuonCollectionKey = muonCollection,
            VertexFitter = VertexFitter,
            VertexPointEstimator = VertexPointEstimator,
            CheckMultiplicityMap = False,
            MonTool = TrigMultiTrkComboHypoMonitoring('TrigMultiTrkComboHypoMonitoring_'+trigSequenceName+trigLevel))

        if trigLevel == 'EF':
            tool.TrigBphysCollectionKey = 'HLT_'+trigSequenceName+trigLevel

        return tool

    def ConfigurationComboHypoTool(self, chainDict):

        tool = TrigMultiTrkComboHypoTool(chainDict['chainName'])

        try:
            topo = chainDict['topo'][0]
            value = trigMultiTrkComboHypoToolDict[topo]
            tool.massRange = value['massRange']
            tool.chi2 = value['chi2']
            tool.totalCharge = 0
        except LookupError:
            raise Exception('TrigMultiTrkComboHypo misconfigured for \'%s\': topo \'%s\' is not supported.', chainDict['chainName'], topo)

        if 'nocut' in chainDict['topo']:
            tool.AcceptAll = True

        if 'noos' in chainDict['topo']:
            tool.totalCharge = -100 # negative number to indicate no charge cut

        tool.MonTool = TrigMultiTrkComboHypoToolMonitoring('MonTool')
        return tool
