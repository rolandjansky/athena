#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def DQTBackgroundMonAlgConfig(flags, isOld=False):
    if isOld: # replace with helper for the old configuration
        from AthenaMonitoring import AthMonitorCfgHelperOld as AthMonitorCfgHelper
        from .DataQualityToolsConf import DQTBackgroundMon
    else:
        from AthenaMonitoring import AthMonitorCfgHelper
        from AthenaConfiguration.ComponentFactory import CompFactory
        DQTBackgroundMon = CompFactory.DQTBackgroundMon
    helper = AthMonitorCfgHelper(flags, 'DQTBackgroundMonAlgCfg')
    

    monAlg = helper.addAlgorithm(DQTBackgroundMon,'DQTBackgroundMonAlg')
    monAlg.Muons = True
    group = helper.addGroup(monAlg,'default','GLOBAL/BackgroundMon','run')

    # Histogram titles. Some commonly used strings in the histograms defined below.
    bgT = 'Background Word '
    filledT = '(Filled BG) '
    emptyT = '(Empty BG) '
    unpairIsoT = '(UnpairIso BG) '
    unpairNonIsoT = '(UnpairNonIso BG) '
    pixT = 'Pixel SpacePoints '
    sctT = 'SCT SpacePoints '
    hugeMultT = 'Huge Multiplicity '
    lowMultT = 'Low Multiplicity '
    pVertexT = 'Number of Primary Vertices '

    # Background word histograms
    bitSetLabels = ["MBTSTimeDiffHalo","MBTSTimeDiffCol","LArECTimeDiffHalo",
        "LArECTimeDiffCol","PixMultiplicityHuge","PixSPNonEmpty","SCTMultiplicityHuge",
        "SCTSPNonEmpty","CSCTimeDiffHalo","CSCTimeDiffCol","BCMTimeDiffHalo",
        "BCMTimeDiffCol","MuonTimingCol","MuonTimingCosmic","MBTSBeamVeto",
        "BCMBeamVeto","LUCIDBeamVeto","HaloMuonSegment","HaloClusterShape",
        "HaloMuonOneSided","HaloMuonTwoSided","HaloTileClusterPattern","BeamGasPixel",
        "CosmicStandAlone","CosmicStandAloneTight","CosmicCombined",
        "CosmicCombinedTight","BkgdResvBit1","BkgdResvBit2","BkgdResvBit3",
        "BkgdResvBit4","BkgdResvBit5"]
    group.defineHistogram('bitSet', title=bgT, xlabels=bitSetLabels,
                          xbins=len(bitSetLabels),xmin=0,xmax=len(bitSetLabels))
    group.defineHistogram('bitSetFilled', weight='filled',
                          title=bgT+filledT, xlabels=bitSetLabels,
                          xbins=len(bitSetLabels),xmin=0,xmax=len(bitSetLabels))
    group.defineHistogram('bitSetEmpty', weight='empty',
                          title=bgT+emptyT, xlabels=bitSetLabels,
                          xbins=len(bitSetLabels),xmin=0,xmax=len(bitSetLabels))
    group.defineHistogram('bitSetUnpairIso', weight='unpairIso',
                          title=bgT+unpairIsoT, xlabels=bitSetLabels,
                          xbins=len(bitSetLabels),xmin=0,xmax=len(bitSetLabels))
    group.defineHistogram('bitSetUnpairNonIso', weight='unpairNonIso',
                          title=bgT+unpairNonIsoT, xlabels=bitSetLabels,
                          xbins=len(bitSetLabels),xmin=0,xmax=len(bitSetLabels))

    # Pixel spacepoint histograms
    group.defineHistogram('nPixSPs;nPixSPs_high',
                          title=pixT+hugeMultT,
                          xbins=200, xmin=0, xmax=1e6)
    group.defineHistogram('nPixSPs;nPixSPs_high_unpairIso', weight='unpairIso',
                          title=pixT+hugeMultT+unpairIsoT,
                          xbins=200, xmin=0, xmax=1e6)
    group.defineHistogram('nPixSPs;nPixSPs_high_unpairNonIso', weight='unpairNonIso',
                          title=pixT+hugeMultT+unpairNonIsoT,
                          xbins=200, xmin=0, xmax=1e6)

    lowMultiplicityPixGroup = helper.addGroup(monAlg,'lowMultiplicityPixGroup',
                                              'GLOBAL/BackgroundMon','run')
    lowMultiplicityPixGroup.defineHistogram('nPixSPs;nPixSPs_low',
                                            title=pixT+lowMultT,
                                            xbins=200, xmin=0, xmax=5e3)
    lowMultiplicityPixGroup.defineHistogram('nPixSPs;nPixSPs_low_unpairIso',
                                            weight='unpairIso',
                                            title=pixT+lowMultT+unpairIsoT,
                                            xbins=200, xmin=0, xmax=5e3)
    lowMultiplicityPixGroup.defineHistogram('nPixSPs;nPixSPs_low_unpairNonIso',
                                            weight='unpairNonIso',
                                            title=pixT+lowMultT+unpairNonIsoT,
                                            xbins=200, xmin=0, xmax=5e3)

    # SCT spacepoint histograms
    group.defineHistogram('nSctSPs;nSctSPs_high',
                          title=sctT+hugeMultT,
                          xbins=200, xmin=0, xmax=1e6)
    group.defineHistogram('nSctSPs;nSctSPs_high_unpairIso', weight='unpairIso',
                          title=sctT+hugeMultT+unpairIsoT,
                          xbins=200, xmin=0, xmax=1e6)
    group.defineHistogram('nSctSPs;nSctSPs_high_unpairNonIso', weight='unpairNonIso',
                          title=sctT+hugeMultT+unpairNonIsoT,
                          xbins=200, xmin=0, xmax=1e6)

    lowMultiplicitySctGroup = helper.addGroup(monAlg,'lowMultiplicitySctGroup',
                                              'GLOBAL/BackgroundMon','run')
    lowMultiplicitySctGroup.defineHistogram('nSctSPs;nSctSPs_low',
                                            title=sctT+lowMultT,
                                            xbins=200, xmin=0, xmax=1e4)
    lowMultiplicitySctGroup.defineHistogram('nSctSPs;nSctSPs_low_unpairIso',
                                            weight='unpairIso',
                                            title=sctT+lowMultT+unpairIsoT,
                                            xbins=200, xmin=0, xmax=1e4)
    lowMultiplicitySctGroup.defineHistogram('nSctSPs;nSctSPs_low_unpairNonIso',
                                            weight='unpairNonIso',
                                            title=sctT+lowMultT+unpairNonIsoT,
                                            xbins=200, xmin=0, xmax=1e4)

    # LAr histograms
    group.defineHistogram('LArECTimeDiff',
                          title='LAr EC collision time difference;<t_{A}>-<t_{C}> [ns]',
                          xbins=100, xmin=-50, xmax=50)

    # MBTS histograms
    group.defineHistogram('MBTStimeDiff', title='MBTS collision time difference',
                          xbins=100, xmin=-50, xmax=50)
    group.defineHistogram('MBTSvetoHits',
                          title="Total MBTS Hits Setting Veto Bit;Number of MBTS 'veto' hits",
                          xbins=40, xmin=0, xmax=40)

    # Lucid histograms
    group.defineHistogram('LUCIDHits', title='Total Lucid Hits',
                          xbins=64, xmin=0, xmax=32)

    # Beam Background Identification Method
    if monAlg.Muons:
        group.defineHistogram('muonSegmentX,muonSegmentY', type='TH2F',
                              title='Position of the muon segments',
                              xbins=100, xmin=-4, xmax=4,
                              ybins=100, ymin=-4, ymax=4)

        # Distributions of beam background calorimeter clusters
        group.defineHistogram('clusterEnergy', title='Cluster Energy',
                              xbins=150, xmin=0, xmax=1500)
        group.defineHistogram('clusterEta,clusterPhi', type='TH2F', title='Cluster Eta-Phi',
                              xbins=50, xmin=-3, xmax=3, ybins=50, ymin=-3.15, ymax=3.15)
        group.defineHistogram('clusterEta,clusterTime',type='TH2F', title='Cluster Eta-Time',
                              xbins=50, xmin=-3, xmax=3, ybins=50, ymin=-25, ymax=25)

        # Index of the jets tagged by the two-sided method
        group.defineHistogram('jetIndex', title='Fake Jet Index',
                              xbins=10, xmin=0, xmax=10)

        # Distributions in events with a two-sided-method tagged leading jet
        group.defineHistogram('leadingJetPt', title='Fake jet pT',
                              xbins=150, xmin=0, xmax=1500)
        group.defineHistogram('leadingJetEta', title='Fake jet Eta',
                              xbins=50, xmin=-3, xmax=3)
        group.defineHistogram('leadingJetPhi', title='Fake jet Phi',
                              xbins=50, xmin=-3.15, xmax=3.15)
        group.defineHistogram('leadingJetTime', title='Fake jet Time',
                              xbins=120, xmin=-30, xmax=30)
        group.defineHistogram('leadingJetChf', title='Fake jet EM Charge Fraction',
                              xbins=100, xmin=0, xmax=1)
        group.defineHistogram('leadingJetEta,leadingJetTime', type='TH2F', title='Fake jet Eta-Time',
                              xbins=50, xmin=-3, xmax=3, ybins=50, ymin=-25, ymax=25)
        group.defineHistogram('leadingJetEMF,leadingJetCHF', type='TH2F', title='Fake jet EMF-CHF',
                              xbins=50, xmin=0, xmax=1, ybins=50, ymin=0, ymax=1)

        # beam background muon rate per BCID in A->C and C->A direction
        group.defineHistogram('bicd;bcid_AC', weight='isAC', title='Rate per BCID in A->C Direction',
                              xbins=3564, xmin=0, xmax=3564)
        group.defineHistogram('bcid;bcid_CA', weight='isBC', title='Rate per BCID in C->A Direction',
                              xbins=3564, xmin=0, xmax=3564)
        group.defineHistogram('bcid;bcid_ACOneSided', weight='isACOneSided',
                              title='Rate per BCID in A->C Direction (one- and two-sided)',
                              xbins=3564, xmin=0, xmax=3564)
        group.defineHistogram('bcid;bcid_CAOneSided', weight='isCAOneSided',
                              title='Rate per BCID in C->A Direction (one- and two-sided)',
                              xbins=3564, xmin=0, xmax=3564)

    group.defineHistogram('nVertex', title='Number of Primary Vertices;N_{PV}',
                          xbins=50, xmin=0, xmax=50)
    group.defineHistogram('nVertex;nVertex_unpairIso', title=pVertexT+unpairIsoT,
                          weight='unpairIso', xbins=50, xmin=0, xmax=50)
    group.defineHistogram('nVertex;nVertex_unpairNonIso', title=pVertexT+unpairNonIsoT,
                          weight='unpairNonIso', xbins=50, xmin=0, xmax=50)

    acc = helper.result()

    # need background objects
    if not isOld:
        from RecBackgroundAlgs.BackgroundAlgsConfig import BackgroundAlgsCfg
        acc.merge(BackgroundAlgsCfg(flags))

    return acc
