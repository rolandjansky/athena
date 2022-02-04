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
    'b3mu'       : { 'massRange' : ( 100., 10000.), 'chi2' : 30., 'nTrk' : 3, 'charge' : 1 },
    'bBeeM6000'  : { 'massRange' : ( 100.,  6000.), 'chi2' : 20. },
    'b0dRAB12vtx20'         : { 'massRange' : ( 0., 999999999.), 'chi2' : 20., 'deltaRMax' : 1.2 },
    'b0dRAB207invmAB22vtx20': { 'massRange' : ( 7000., 22000.), 'chi2' : 20., 'deltaRMax' : 2.0 },
    'b0dRAB127invmAB22vtx20': { 'massRange' : ( 7000., 22000.), 'chi2' : 20., 'deltaRMax' : 1.2 },
    'b7invmAB22vtx20'  : { 'massRange' : (  7000., 22000.), 'chi2' : 20., 'charge' : -1 },
    'b7invmAB9vtx20'   : { 'massRange' : (  7000.,  9000.), 'chi2' : 20., 'charge' : -1 },
    'b11invmAB60vtx20' : { 'massRange' : ( 11000., 60000.), 'chi2' : 20., 'charge' : -1 },
    'b11invmAB24vtx20' : { 'massRange' : ( 11000., 24000.), 'chi2' : 20., 'charge' : -1 },
    'b24invmAB60vtx20' : { 'massRange' : ( 24000., 60000.), 'chi2' : 20., 'charge' : -1 }
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

def StreamerDimuL2MTComboHypoCfg(name):
    log.debug('DimuL2MTComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = True,
        trigSequenceName = 'Dimu',
        trigLevel = 'L2MT')
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
    hypo.nTracks = [ 2 ]
    hypo.massRange = [ (100., 6000.) ]
    hypo.trackPtThresholds = [ [ 100., 100. ] ]
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
    hypo.mergedElectronChains = ['BPH-0DR3-EM7J15','HLT_e5_lhvloose_bBeeM6000_L1EM22VHI','HLT_e5_lhvloose_bBeeM6000_L14J15','HLT_e5_lhvloose_bBeeM6000_L1BKeePrimary','HLT_e5_lhvloose_bBeeM6000_L1BKeePrescaled','HLT_e5_lhvloose_bBeeM6000_L1All']
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
    hypo.mergedElectronChains = ['BPH-0DR3-EM7J15','HLT_e5_lhvloose_bBeeM6000_L1EM22VHI','HLT_e5_lhvloose_bBeeM6000_L14J15']
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
    hypo.nTracks = [ 2 ]
    hypo.totalCharge = [ 0 ]
    hypo.massRange = [ (2500., 4400.) ]
    hypo.trackPtThresholds = [ [ 10000., 3000. ] ]
    return hypo

def DrellYanComboHypoCfg(name):
    log.debug('DrellYanComboHypoCfg.name = %s ', name)

    config = TrigMultiTrkComboHypoConfig()
    hypo = config.ConfigurationComboHypo(
        isStreamer = False,
        trigSequenceName = 'DrellYan',
        trigLevel = 'EF',
        outputTrigBphysCollection = 'HLT_DrellYan')
    hypo.nTracks = [ 2 ]
    hypo.totalCharge = [ -1 ]
    hypo.massRange = [ (7000., 60000.) ]
    hypo.trackPtThresholds = [ [ 100., 100. ] ]
    hypo.chi2 = 20.
    hypo.combineInputDecisionCollections = True
    hypo.applyOverlapRemoval = False
    hypo.useLeptonMomentum = True
    return hypo

def TrigMultiTrkComboHypoToolFromDict(chainDict):
    config = TrigMultiTrkComboHypoConfig()
    tool = config.ConfigurationComboHypoTool(chainDict)
    return tool

class TrigMultiTrkComboHypoConfig(object):

    def ConfigurationComboHypo(self, isStreamer='False', trigSequenceName='Dimu', trigLevel='L2', trackCollection='', outputTrigBphysCollection='TrigBphysContainer', doElectrons = False):

        trigLevelDict = {'L2':0, 'L2IO':1, 'L2MT':2, 'EF':3}

        try:
            value = trigLevelDict[trigLevel]
            log.debug('TrigMultiTrkComboHypo.trigLevel = %s ', value)
        except KeyError:
            raise Exception('TrigMultiTrkComboHypo.trigLevel should be L2, L2IO, L2MT or EF, but %s provided.', trigLevel)

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
            nTracks = [ 2, 3 ],
            massRange = [ (100., 20000.), (0., 11000.) ],
            trackPtThresholds = [ [ 3650., 3650. ], [ 3650., 3650., 3650. ] ],
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
            tool.nTrk = value['nTrk'] if 'nTrk' in value else 2
            tool.totalCharge = value['charge'] if 'charge' in value else 0
            if 'deltaRMin' in value:
               tool.deltaRMin = value['deltaRMin']
            if 'deltaRMax' in value:
               tool.deltaRMax = value['deltaRMax']
        except LookupError:
            raise Exception('TrigMultiTrkComboHypo misconfigured for \'%s\': topo \'%s\' is not supported.', chainDict['chainName'], topo)

        if 'nocut' in chainDict['topo']:
            tool.AcceptAll = True

        if 'noos' in chainDict['topo']:
            tool.totalCharge = -100 # negative number to indicate no charge cut

        if 'Lxy0' in chainDict['topo']:
            tool.LxyCut = 0.0

        electronMultiplicity = [int(chainPart['multiplicity']) for chainPart in chainDict['chainParts'] if chainPart['signature']=='Electron']
        if len(electronMultiplicity) == 1 and electronMultiplicity[0] == 1:
            tool.isMergedElectronChain = True

        if 'bJpsimutrk' in chainDict['topo']:
            tool.isMuonTrkPEB = True
            tool.totalCharge = 0

        if 'bTau' in chainDict['topo']:
            tool.nTrk = sum(int(chainPart['multiplicity']) for chainPart in chainDict['chainParts'])
            tool.totalCharge = 1 if tool.nTrk == 3 else -1

        signatures = chainDict['signatures']
        tool.isCombinedChain = (signatures.count(signatures[0]) != len(signatures))
        tool.legMultiplicities = chainDict['chainMultiplicities']

        tool.MonTool = TrigMultiTrkComboHypoToolMonitoring('MonTool')
        return tool
