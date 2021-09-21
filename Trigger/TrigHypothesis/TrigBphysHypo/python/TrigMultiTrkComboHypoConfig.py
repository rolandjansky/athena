# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigBphysHypo.TrigMultiTrkComboHypoMonitoringConfig import TrigMultiTrkComboHypoMonitoring, TrigMultiTrkComboHypoToolMonitoring

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMultiTrkComboHypoConfig')

trigMultiTrkComboHypoToolDict = {
    'bJpsimumu'  : { 'massRange' : (2500.,  4300.), 'chi2' : 20. },
    'bJpsi'      : { 'massRange' : (2500.,  4300.), 'chi2' : 20. },
    'bJpsimutrk' : { 'massRange' : (2600.,  3600.), 'chi2' : 20. },
    'bUpsimumu'  : { 'massRange' : (8000., 12000.), 'chi2' : 20. },
    'bUpsi'      : { 'massRange' : (8000., 12000.), 'chi2' : 20. },
    'bDimu'      : { 'massRange' : (1500., 14000.), 'chi2' : 20. },
    'bDimu2700'  : { 'massRange' : ( 100.,  2700.), 'chi2' : 20. },
    'bDimu6000'  : { 'massRange' : ( 100.,  6000.), 'chi2' : 20. },
    'bBmumu'     : { 'massRange' : (4000.,  8500.), 'chi2' : 20. },
    'bPhi'       : { 'massRange' : ( 940.,  1100.), 'chi2' : 10. },
    'bTau'       : { 'massRange' : (   0.,  2700.), 'chi2' : 50. },
    'bBeeM6000'  : { 'massRange' : ( 100.,  6000.), 'chi2' : 20. },
}


def StreamerDimuL2ComboHypoCfg(name):
    log.debug('DimuL2ComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = True,
        trigSequenceName = 'Dimu',
        trigLevel = 'L2',
        trackCollection='HLT_IDTrack_Muon_FTF')
    return hypo

def StreamerDimuL2IOComboHypoCfg(name):
    log.debug('DimuL2IOComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = True,
        trigSequenceName = 'Dimu',
        trigLevel = 'L2IO')
    return hypo

def DimuEFComboHypoCfg(name):
    log.debug('DimuEFComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = False,
        trigSequenceName = 'Dimu',
        trigLevel = 'EF',
        outputTrigBphysCollection = 'HLT_DimuEF')
    return hypo

def StreamerDimuEFComboHypoCfg(name):
    log.debug('StreamerDimuEFComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = True,
        trigSequenceName = 'StreamerDimu',
        trigLevel = 'EF')
    hypo.chi2 = 20.
    hypo.massRange = [ (100., 6000.) ]
    return hypo

def StreamerDiElecFastComboHypoCfg(name):
    log.debug('StreamerDiElecFastComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = True,
        trigSequenceName = 'DiElecFast',
        trigLevel = 'L2',
        doElectrons = True,
        trackCollection='HLT_IDTrack_Electron_FTF')
    return hypo

def StreamerNoMuonDiElecFastComboHypoCfg(name):
    log.debug('StreamerNoMuonDiElecFastComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = True,
        trigSequenceName = 'NoMuonDiElecFast',
        trigLevel = 'L2',
        doElectrons = True,
        trackCollection='HLT_IDTrack_Electron_FTF')
    return hypo

def DiElecPrecisionComboHypoCfg(name):
    log.debug('DiElecPrecisionComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = False,
        trigSequenceName = 'DiElecPrecision',
        trigLevel = 'EF',
        doElectrons = True,
        outputTrigBphysCollection = 'HLT_DiElecPrecision')
    return hypo

def NoMuonDiElecPrecisionComboHypoCfg(name):
    log.debug('NoMuonDiElecPrecisionComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = False,
        trigSequenceName = 'NoMuonDiElecPrecision',
        trigLevel = 'EF',
        doElectrons = True,
        outputTrigBphysCollection = 'HLT_NoMuonDiElecPrecision')
    return hypo

def BmutrkComboHypoCfg(name):
    log.debug('BmutrkComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = False,
        trigSequenceName = 'Bmutrk',
        trigLevel = 'EF',
        trackCollection='HLT_IDTrack_Bmumux_IDTrig',
        outputTrigBphysCollection = 'HLT_Bmutrk')
    hypo.isMuTrkMode = True
    hypo.chi2 = 20.
    hypo.massRange = [ (2500., 4400.) ]
    hypo.trackPtThresholds = [ [ 10000., 3000. ] ]
    return hypo

def TrigMultiTrkComboHypoToolFromDict(chainDict):
    config = TrigMultiTrkComboHypoConfig()
    tool = config.ConfigurationComboHypoTool(chainDict)
    return tool

class TrigMultiTrkComboHypoConfig(object):

    def ConfigurationComboHypo(self, isStreamer='False', trigSequenceName='Dimu', trigLevel='L2', trackCollection='', outputTrigBphysCollection='TrigBphysContainer', doElectrons = False):

        trigLevelDict = {'L2':0, 'L2IO':1, 'EF':2}

        try:
            value = trigLevelDict[trigLevel]
            log.debug('TrigMultiTrkComboHypo.trigLevel = %s ', value)
        except KeyError:
            raise Exception('TrigMultiTrkComboHypo.trigLevel should be L2, L2IO or EF, but %s provided.', trigLevel)

        baseName = 'Streamer'+trigSequenceName+trigLevel if isStreamer else trigSequenceName+trigLevel

        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        VertexFitter = CompFactory.Trk__TrkVKalVrtFitter(
            name = 'TrigBphysFitter_'+baseName,
            FirstMeasuredPoint = False,
            MakeExtendedVertex = False,
            Extrapolator = AtlasExtrapolator())

        VertexPointEstimator = CompFactory.InDet__VertexPointEstimator(
            name = 'VertexPointEstimator_'+baseName,
            MinDeltaR = [-10000., -10000., -10000.],
            MaxDeltaR = [ 10000.,  10000.,  10000.],
            MaxPhi    = [ 10000.,  10000.,  10000.],
            MaxChi2OfVtxEstimation = 2000.)

        if doElectrons:
            tool = CompFactory.TrigMultiTrkComboHypo(
              name = baseName+'ComboHypo',
              isStreamer = isStreamer,
              doElectrons = True,
              trigLevel = trigLevel,
              nTracks = [ 2 ],
              massRange = [ (100., 20000.) ],
              trackPtThresholds = [ [ 4000., 4000. ] ],
              TrackCollectionKey = trackCollection,
              TrigBphysCollectionKey = outputTrigBphysCollection,
              VertexFitter = VertexFitter,
              VertexPointEstimator = VertexPointEstimator,
              CheckMultiplicityMap = False,
              MonTool = TrigMultiTrkComboHypoMonitoring('TrigMultiTrkComboHypoMonitoring_'+baseName))
            return tool

        tool = CompFactory.TrigMultiTrkComboHypo(
            name = baseName+'ComboHypo',
            isStreamer = isStreamer,
            trigLevel = trigLevel,
            nTracks = [ 2 ],
            massRange = [ (100., 20000.) ],
            TrackCollectionKey = trackCollection,
            TrigBphysCollectionKey = outputTrigBphysCollection,
            VertexFitter = VertexFitter,
            VertexPointEstimator = VertexPointEstimator,
            CheckMultiplicityMap = False,
            MonTool = TrigMultiTrkComboHypoMonitoring('TrigMultiTrkComboHypoMonitoring_'+baseName))

        return tool

    def ConfigurationComboHypoTool(self, chainDict):

        tool = CompFactory.TrigMultiTrkComboHypoTool(chainDict['chainName'])

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

        if 'Lxy0' in chainDict['topo']:
            tool.LxyCut = 0.0

        if 'BPH-0DR3-EM7J15' in chainDict['L1item']:
            tool.isMergedElectronChain = True

        if 'bJpsimutrk' in chainDict['topo']:
            tool.isMuonTrkPEB = True
            tool.totalCharge = -100

        signatures = chainDict['signatures']
        tool.isCombinedChain = (signatures.count(signatures[0]) != len(signatures))
        tool.legMultiplicities = chainDict['chainMultiplicities']

        tool.MonTool = TrigMultiTrkComboHypoToolMonitoring('MonTool')
        return tool
