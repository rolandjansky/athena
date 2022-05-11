#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

"""!@file LArRawChannelMonAlg.py
@date Nov. 2021
@brief configuration of LArRawChannels monitoring algorithm following
the implementation model of other LAr DQM algorithms, and default
settings taken from LArMonTools/LArRawChannelMonTool_joboptions.py
"""

from LArMonitoring.GlobalVariables import lArDQGlobals
from LArMonTools.LArMonFlags import LArMonFlags
from GaudiKernel.SystemOfUnits import MeV, GeV

_USE_LEGACY_BINNING_IN_ENDCAPS = True


def LArRawChannelMonConfigOld(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    from AthenaCommon.BeamFlags import jobproperties
    from LArMonitoring.LArMonitoringConf import LArRawChannelMonAlg
    cosmics = jobproperties.Beam.beamType() == 'cosmics'
    stream = _get_stream(DQMonFlags)
    helper = AthMonitorCfgHelperOld(inputFlags, 'LArRawChannelMonAlgOldCfg')
    alg = LArRawChannelMonConfigCore(
        helper, instance=LArRawChannelMonAlg, inputFlags=inputFlags,
        cosmics=cosmics, stream=stream)
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if not athenaCommonFlags.isOnline():
       from AthenaCommon.AlgSequence import AthSequencer
       #if not hasattr (condSeq,"Calo_"+alg.NoiseKey+"Alg"):
       if len([_ for _ in AthSequencer("AthCondSeq") if _.getName()=="Calo_"+str(alg.NoiseKey)+"Alg"]) == 0:
          from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
          CaloNoiseCondAlg(noisetype=alg.NoiseKey.Path)
       from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool
       alg.AtlasReadyFilterTool = [GetAtlasReadyFilterTool()]
    return helper.result()


def LArRawChannelMonConfig(inputFlags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaMonitoring import AthMonitorCfgHelper
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    from AthenaMonitoring.AtlasReadyFilterConfig import AtlasReadyFilterCfg
    from AthenaConfiguration.Enums import BeamType
    cosmics = (inputFlags.Beam.Type is BeamType.Cosmics)
    stream = _get_stream(inputFlags.DQ)
    helper = AthMonitorCfgHelper(inputFlags, 'LArRawChannelMonAlgCfg')
    alg = LArRawChannelMonConfigCore(
        helper, instance=CompFactory.LArRawChannelMonAlg,
        inputFlags=inputFlags, cosmics=cosmics, stream=stream)
    noise_alg = CaloNoiseCondAlgCfg(inputFlags, noisetype=alg.NoiseKey.Path)
    accumulator = ComponentAccumulator()
    accumulator.merge(noise_alg)
    alg.AtlasReadyFilterTool.append(
        accumulator.popToolsAndMerge(AtlasReadyFilterCfg(inputFlags)))
    accumulator.merge(helper.result())
    return accumulator


def LArRawChannelMonConfigCore(helper, instance, inputFlags, cosmics, stream):
    alg = helper.addAlgorithm(instance, 'LArRawChannelMonAlg')
    alg.occupancy_thresholds = [
        100 * MeV,  # EMBA
        100 * MeV,  # EMBC
        100 * MeV,  # EMECA
        100 * MeV,  # EMECC
        500 * MeV,  # HECA
        500 * MeV,  # HECC
        700 * MeV,  # FCALA
        700 * MeV,  # FCALC
    ]
    alg.signal_thresholds = [-9999. * GeV ] * 8
    alg.pos_noise_thresholds = [3] * 8
    alg.neg_noise_thresholds = [3] * 8
    alg.bcid_signal_threshold = 500. * MeV

    alg.time_threshold = 5
    alg.quality_threshold = 65530
    alg.noise_threshold = 3
    alg.noise_burst_percent_threshold = [
        1,  # EMBA
        1,  # EMBC
        1,  # EMECA
        1,  # EMECC
        1,  # HECA
        1,  # HECC
        2,  # FCALA
        2,  # FCALC
    ]
    alg.noise_burst_nChannel_threshold = [10] * 8
    alg.monitor_occupancy = True # False
    if cosmics:
        alg.NoiseKey = 'electronicNoise'
        alg.monitor_signal = True
        alg.monitor_positive_noise = True
        alg.monitor_negative_noise = True
    else:
        alg.NoiseKey = 'totalNoise'
        alg.monitor_signal = LArMonFlags.doLArRawMonitorSignal()
        alg.monitor_positive_noise = LArMonFlags.doLArRawMonitorSignal()
        alg.monitor_negative_noise = LArMonFlags.doLArRawMonitorSignal()
    alg.monitor_time = False
    alg.monitor_quality = LArMonFlags.doLArRawMonitorSignal()
    alg.monitor_burst = True
    if stream in ('express', 'RNDM'):
        alg.noise_streams = ['RNDM']
        alg.TriggerChain = 'L1_RD0_EMPTY'
    else:
        alg.noise_streams = ['RNDM']
    alg.ProblemsToMask = [
        'highNoiseHG',
        'highNoiseMG',
        'highNoiseLG',
        'deadReadout',
        'deadPhys',
        'almostDead',
        'short',
        'sporadicBurstNoise',
    ]
    alg.db_and_ofc_only = True

    # Histograms for different partitions are handled together via a
    # GenericMonitoringArray, but since some of the ranges and titles
    # vary between partitions, the definition of histograms must be
    # done separately for each partition, using the 'pattern' argument:
    montool = helper.addArray([lArDQGlobals.Partitions], alg,
        'LArRawChannelMon', topPath='/LAr/RawChannel')
    for index, partition in enumerate(lArDQGlobals.Partitions):
        _define_histograms(partition, index, montool, alg)
    return alg


def _define_histograms(partition, part_index, montool, alg):
    common_args = {
        'path': '{0}',
        'pattern': [partition],
        'opt' : 'kAlwaysCreate'
        # 'merge' unspecified: the default should suit all histograms
    }
    hargs2d = {
        **common_args,
        'type': 'TH2I',
        **_superslot_channel_axis_ranges(partition),
    }
    lumiblock_binning = {'xbins': 3000, 'xmin': 0.5, 'xmax': 3000.5}
    title = ' as a function of FEB and channel in {0};' \
        'Halfcrate (+ increasing slot);Channel'
    sigma_pos = '{:g}#sigma'.format(alg.pos_noise_thresholds[part_index])
    sigma_neg = '{:g}#sigma'.format(alg.neg_noise_thresholds[part_index])
    montool.defineHistogram(
        'S,C;{0}_occupancy', **hargs2d,
        cutmask='occ',
        title='Number of events above {:g} MeV{}'.format(
            alg.occupancy_thresholds[part_index] / MeV,
            title))
    hargs2d['type'] = 'TProfile2D'
    title = title.replace('{0}', '{0} (no LArEventInfo::ERROR)')
    if alg.monitor_signal:
        montool.defineHistogram(
            'S,C,E;{0}_signal_AVG', **hargs2d,
            cutmask='sig',
            title='{0}{1};{0}'.format('Average Energy (MeV)', title))
        montool.defineHistogram(
            'S,C,G;{0}_gain', **hargs2d,
            cutmask='sig',
            title='{0}{1};{0}'.format('Average gain', title))
    title += ';Percentage Accepted'
    percent = 'Percentage of events '
    if alg.monitor_positive_noise:
        montool.defineHistogram(
            'S,C,posn;{0}_acceptance_AVG', **hargs2d,
            title='{} above {} total noise{}'.format(
                percent, sigma_pos, title))
    if alg.monitor_negative_noise:
        montool.defineHistogram(
            'S,C,negn;{0}_noise_acceptance_AVG', **hargs2d,
            title='{} below -{} total noise{}'.format(
                percent, sigma_neg, title))
    if alg.monitor_quality:
        montool.defineHistogram(
            'S,C,Q4k;{0}_quality_AVG', **hargs2d,
            title='{} with q-factor above {:g}{}'.format(
                percent, alg.quality_threshold, title))
    if alg.monitor_quality:
        montool.defineHistogram(
            'nQ4k;{0}_quality_nChannel', **common_args,
            type='TH1D', xbins=50, xmin=-0.5, xmax=49.5,
            title = 'Number of channels in {{0}} with q-factor > {};' \
            'Number of channels;Number of events per channel'.format(
                alg.quality_threshold))
        montool.defineHistogram(
            'lb;{0}_quality_burst', **common_args,
            type='TH1D', **lumiblock_binning, cutmask='qburst',
            title = 'Number of events with more than {:g}% ' \
                'of all channels in {{0}} reporting q-factor > {};' \
                'Luminosity Block;Number of events per LB'.format(
                    alg.noise_burst_percent_threshold[part_index],
                    alg.quality_threshold))
    if alg.monitor_burst:
        title = 'Yield of channels with E > +{t} in {{0}}{cut};' \
            'Percent of channels;Number of events per 0.02%'
        hargs = {
            **common_args,
            'type': 'TH1D',
            'xbins': 375,
            'xmin': 0.,
            'xmax': 7.5
        }
        name = '%noisy;{0}_noise_fraction'
        montool.defineHistogram(
            name, **hargs, title=title.format(t=sigma_pos, cut=''))
        montool.defineHistogram(
            '%noisy_neg;{0}_noise_fraction_Neg', **hargs,
            title=title.replace('> +', '< -').format(t=sigma_neg, cut=''))
        montool.defineHistogram(
            name + '_W', **hargs, cutmask='quietW',
            title=title.format(t=sigma_pos, cut=' (no LArNoisyRO_StdOpt)'))
        montool.defineHistogram(
            name + '_NoLArNoisyRO', **hargs, cutmask='quiet',
            title=title.format(t=sigma_pos, cut=' (no LArNoisyRO_Std)'))
        montool.defineHistogram(
            name + '_TimeVetoLArNoisyRO', **hargs, cutmask='quietITW',
            title=title.format(t=sigma_pos, cut=' (time vetoed)'))
        title = 'Number of events with Y(3#sigma) > {t} in {{0}}{cut};' \
            'Luminosity Block;Number of events per LB'
        hargs = {**common_args, 'type': 'TH1D', **lumiblock_binning}
        tb = '{:g}%'.format(alg.noise_burst_percent_threshold[part_index])
        montool.defineHistogram(
            'lb;{0}_burst', **hargs, cutmask='burst',
            title=title.format(t=tb, cut=''))
        montool.defineHistogram(
            'lb;{0}_timeVetoBurst', **hargs, cutmask='burst_quietW',
            title=title.format(t=tb, cut=' (time vetoed)'))
        if alg.monitor_time:
            montool.defineHistogram(
                'T;{0}_mean_feb_time', **common_args, type='TH1D',
                xbins=101, xmin=-50.5, xmax=50.5,
                title='Average time of channels in each FEB of {{0}} ' \
                    'reporting E > {} #sigma;<t(FEB)> - <t(event)> (ns);' \
                    'Number of FEBs per ns'.format(alg.time_threshold))
    if alg.monitor_signal:
        montool.defineHistogram(
            'lb,detE;{0}_pedestal_evolution', **common_args, type='TProfile',
            **lumiblock_binning, cutmask='quietW',
            title='Energy sum (time vetoed) in {0};' \
                'Luminosity Block;Mean total energy(MeV)')
        montool.defineHistogram(
            'bc,detE;{0}_sumE_vs_BCID', **common_args, type='TProfile',
            xbins=3564, xmin=0.5, xmax=3564.5,
            title = 'Energy sum per bunch crossing in {0};' \
                'Bunch Crossing ID;Mean total energy (MeV)')


def _get_stream(flags):
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    flag = flags.useTrigger
    if callable(flag): flag = flag()
    if flag and not athenaCommonFlags.isOnline():
        from PyUtils.MetaReaderPeeker import metadata
        return (metadata.get('stream', '') + '_').split('_')[1]
    return ''


def _superslot_channel_axis_ranges(partition):
    n = lArDQGlobals.Feedthrough_Slot_Nbins.get(partition, 1)
    bins = lArDQGlobals.Feedthrough_Slot_range.get(partition, (0., 1.))
    labels = ['?'] * n
    if partition.startswith('EMB'):
        if len(lArDQGlobals.Feedthrough_Slot_labels_Barrel) == n:
            labels = lArDQGlobals.Feedthrough_Slot_labels_Barrel
    elif len(lArDQGlobals.Feedthrough_Slot_labels_Endcap) == n:
        labels = lArDQGlobals.Feedthrough_Slot_labels_Endcap
    axis_ranges = {
        'xbins': n,
        'xmin': bins[0],
        'xmax': bins[1],
        'xlabels': labels,
        'ybins': lArDQGlobals.FEB_N_channels,
        'ymin': -0.5,
        'ymax': lArDQGlobals.FEB_N_channels - 0.5,
    }
    if _USE_LEGACY_BINNING_IN_ENDCAPS and not partition.startswith('EMB'):
        merged_bins = {
            'EMEC': [
                (14, 15), (29, 30), (48, 60), (74, 75), (89, 105), (119, 120),
                (134, 135), (153, 165), (179, 180), (194, 195), (209, 210),
                (224, 225), (243, 255), (269, 270), (284, 285), (299, 300),
                (314, 315), (333, 345), (359, 360), (374, 375)
                ],
            'HEC': [
                (1, 49), (56, 154), (161, 244), (251, 334), (341, 375)
                ],
            'FCal': [(1, 90), (106, 375)],
        }
        bins = [0.5]
        for b in merged_bins[partition[:-1]]:
            bins += [x + 0.5 for x in range(int(bins[-1] + 0.5), b[0])]
            bins.append(b[1] + 0.5)
        axis_ranges['xbins'] = bins
        # Try to obtain meaningful bin labels from GlobalVariables.py:
        if n > 1 and axis_ranges['xmax'] > axis_ranges['xmin']:
            labels = [''] * (len(bins) - 1)
            coeff =  n / (axis_ranges['xmax'] - axis_ranges['xmin'])
            nslots = lArDQGlobals.FEB_Slot[partition][1]
            for b, x in enumerate(bins[:-1]):
                i = (int(coeff * (x - axis_ranges['xmin'])))
                i -= (i % nslots)
                if bins[b+1] - x < 1.5 and 0 <= i < n:
                    txt = axis_ranges['xlabels'][i]
                    if txt not in labels:
                        labels[b] = txt
            axis_ranges['xlabels'] = labels
        else:
            axis_ranges.pop('xlabels')
        axis_ranges.pop('xmin')
        axis_ranges.pop('xmax')
    return axis_ranges


if __name__=='__main__':
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import WARNING
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    log.setLevel(WARNING)

    from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
    createLArMonConfigFlags()

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.Enums import BeamType
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Output.HISTFileName = 'LArRawChannelMonOutput.root'
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.Beam.Type = BeamType.Collisions
    ConfigFlags.lock()

    from CaloRec.CaloRecoConfig import CaloRecoCfg
    cfg = CaloRecoCfg(ConfigFlags)
    acc = LArRawChannelMonConfig(ConfigFlags)
    cfg.merge(acc)
    f = open("LArRawChannelMon.pkl", "wb")
    cfg.store(f)
    f.close()

    #in case you need directly run uncomment:
    #cfg.run(100,OutputLevel=WARNING)
