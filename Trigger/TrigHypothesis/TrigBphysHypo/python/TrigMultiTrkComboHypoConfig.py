# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigMultiTrkComboHypo, TrigMultiTrkComboHypoTool
from TrigBphysHypo.TrigMultiTrkComboHypoMonitoringConfig import TrigMultiTrkComboHypoMonitoring, TrigMultiTrkComboHypoToolMonitoring

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMultiTrkComboHypoConfig')

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
            log.error('TrigMultiTrkComboHypo.trigLevel should be L2 or EF, but %s provided.', trigLevel)

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

        topoAlgs = chainDict['chainName']
        log.debug("Set for algorithm %s", topoAlgs)

        tool = TrigMultiTrkComboHypoTool(topoAlgs)

        if 'nocut' in topoAlgs:
            tool.AcceptAll = True

        if 'noos' in topoAlgs:
            tool.TotChargeCut = -100 #Negative number to indicate no charge cut

        tool.ApplyUpperMassCut = True
        tool.ApplyChi2Cut = True
        tool.Chi2VtxCut = 20
        tool.trkPtThresholds = getBphysThresholds(chainDict)

        if 'bJpsimumu' in topoAlgs:
            tool.LowerMassCut =  2500 #MeV
            tool.UpperMassCut =  4300 #MeV

        elif 'bUpsimumu' in topoAlgs:
            tool.LowerMassCut =  8000 #MeV
            tool.UpperMassCut = 12000 #MeV

        elif 'bBmumu' in topoAlgs:
            tool.LowerMassCut =  4000 #MeV
            tool.UpperMassCut =  8500 #MeV
            tool.Chi2VtxCut = 60

        elif 'bDimu' in topoAlgs:
            tool.LowerMassCut =   100 #MeV
            tool.UpperMassCut = 14000 #MeV

        tool.MonTool = TrigMultiTrkComboHypoToolMonitoring('MonTool')
        return tool


def getBphysThresholds(chainDict):
    mult = 0
    trkmuons = []

    for part in chainDict['chainParts']:
        mult = mult + int(part['multiplicity'])

    for dictpart in chainDict['chainParts']:
        if 'mu' in dictpart['trigType']:
            for x in range(0,int(dictpart['multiplicity'])):
                if dictpart['threshold']!='0':
                    dthr = float(dictpart['threshold'] )
                    thr= dthr * 1000.  # in MeV;

                    #lower to match EF muon threshols
                    if dthr < 9.5 :
                        thr = thr - 350.
                    elif dthr < 11.5 :
                        thr = thr - 550.
                    elif dthr < 21.5  :
                        thr = thr - 750.
                    else :
                        thr = thr - 1000.

                else :
                    thr = 900.
                trkmuons.append(thr)
    return trkmuons
