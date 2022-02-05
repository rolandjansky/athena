#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@brief configuration for the min bias monitoring
'''


def TrigMBTS(configFlags):
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(configFlags, 'HLTMBTSMonitoringAlgMTflags')

    from AthenaConfiguration.ComponentFactory import CompFactory
    alg = monConfig.addAlgorithm(
        CompFactory.HLTMBTSMonitoringAlgMT, 'HLTMBTSMonitoringAlgMT')
    alg.triggerList = ["HLT_mb_mbts_L1MBTS_1", "HLT_mb_mbts_L1RD0_FILLED", "HLT_mb_mbts_L1MBTS_1_EMPTY"]
    alg.MBTS_channelID = [f'A{i:0>2d}' for i in range(16)]
    alg.MBTS_channelID += [f'C{i:0>2d}' for i in range(16)]

    length = len(alg.triggerList)
    MBTS_countsSideA = 16
    MBTS_countsSideC = 16

    mbtsGroup = monConfig.addGroup(alg, 'MBTSall', topPath='HLT/MBTSMon/')
    mbtsGroup.defineHistogram('TrigCounts', title='Trigger counts;;Event rate',
                              xbins=length, xmin=0, xmax=length, xlabels=list(alg.triggerList))

    for chain in alg.triggerList:

        mbShiftGroup = monConfig.addGroup(alg, chain+'_shifter',
                                          topPath='HLT/MBTSMon/'+chain+'/')

        mbShiftGroup.defineHistogram('MBTS_A_hits', type='TH1I', title='MBTS hits side A; Entry rate; MBTS side A',
                                     xbins=MBTS_countsSideA+1, xmin=-0.5, xmax=MBTS_countsSideA+0.5)
        mbShiftGroup.defineHistogram('MBTS_C_hits', type='TH1I', title='MBTS hits side C; Entry rate; MBTS side C',
                                     xbins=MBTS_countsSideC+1, xmin=-0.5, xmax=MBTS_countsSideC+0.5)
        mbShiftGroup.defineHistogram('MBTS_A_hits,MBTS_C_hits', type='TH2I', title='MBTS hits correlation; MBTS side A; MBTS side C',
                                     xbins=MBTS_countsSideA+1, xmin=-0.5, xmax=MBTS_countsSideA+0.5, ybins=MBTS_countsSideC+1, ymin=-0.5, ymax=MBTS_countsSideC+0.5)

        mbShiftGroup.defineHistogram(
            'MBTStime', type='TH1F', title='MBTS time; MBTS time [ns]', xbins=100, xmin=-100, xmax=100)
        mbShiftGroup.defineHistogram(
            'MBTSenergy', type='TH1F', title='MBTS energy; MBTS energy [pC]', xbins=100, xmin=-100, xmax=100)
        mbShiftGroup.defineHistogram('MBTS_channelID', type='TH1F', title='MBTS Channel ID; Channel ID; Entry rate',
                                     xbins=MBTS_countsSideA+MBTS_countsSideC, xmin=0, xmax=MBTS_countsSideA+MBTS_countsSideC, xlabels=list(alg.MBTS_channelID))
        mbShiftGroup.defineHistogram('MBTS_channelID,MBTS_time', type='TH2F', title='MBTS time; Channel ID;MBTS time [ns]; Entry rate',
                                     xbins=MBTS_countsSideA+MBTS_countsSideC, xmin=0, xmax=MBTS_countsSideA+MBTS_countsSideC, xlabels=list(alg.MBTS_channelID), ybins=100, ymin=-100, ymax=100)
        mbShiftGroup.defineHistogram('MBTS_channelID,MBTS_time;MBTS_time_zoom_vs_MBTS_channel_ID', type='TH2F', title='MBTS time zoom; Channel ID;MBTS time [ns]; Entry rate',
                                     xbins=MBTS_countsSideA+MBTS_countsSideC, xmin=0, xmax=MBTS_countsSideA+MBTS_countsSideC, xlabels=list(alg.MBTS_channelID), ybins=100, ymin=-10, ymax=10)
        mbShiftGroup.defineHistogram('MBTS_channelID,MBTS_energy', type='TH2F', title='MBTS energy; Channel ID;MBTS energy [pC]; Entry rate',
                                     xbins=MBTS_countsSideA+MBTS_countsSideC, xmin=0, xmax=MBTS_countsSideA+MBTS_countsSideC, xlabels=list(alg.MBTS_channelID), ybins=100, ymin=-100, ymax=100)

        mbExpGroup = monConfig.addGroup(alg, chain+'_expert',
                                        topPath='HLT/MBTSMon/'+chain+'/')
        mbExpGroup.defineHistogram(
            'MBTShits', type='TH2F', title='MBTS total hits;# of Hits;Entry Rate', xbins=24, xmin=0, xmax=100)

        mbExpGroup.defineHistogram('MBTS_A_meanEnergy', type='TH1F',
                                   title='Mean MBTS Energy A side passed;MBTS Energy [pC];Entry Rate', xbins=100, xmin=-1, xmax=1)
        mbExpGroup.defineHistogram('MBTS_C_meanEnergy', type='TH1F',
                                   title='Mean MBTS Energy C side passed;MBTS Energy [pC];Entry Rate', xbins=100, xmin=-1, xmax=1)

        mbExpGroup.defineHistogram('MBTS_A_meanTime', type='TH1F',
                                   title='Mean MBTS Time A side; MBTS time [ns]', xbins=100, xmin=-100, xmax=100)
        mbExpGroup.defineHistogram('MBTS_C_meanTime', type='TH1F',
                                   title='Mean MBTS Time C side; MBTS time [ns]', xbins=100, xmin=-100, xmax=100)

    return monConfig.result()

if __name__ == '__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Output.HISTFileName = 'TestMBTSMonitorOutput.root'
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigMBTS(ConfigFlags))

    cfg.getEventAlgo('HLTMBTSMonitoringAlgMT').OutputLevel = DEBUG  # DEBUG
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run()  # use cfg.run(20) to only run on first 20 events
    # to run:
    # python -m TrigMinBiasMonitoring.TrigMBTSMonitoringMT --filesInput=
