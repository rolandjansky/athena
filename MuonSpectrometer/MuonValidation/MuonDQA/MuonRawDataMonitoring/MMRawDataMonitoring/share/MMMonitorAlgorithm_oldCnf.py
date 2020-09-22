###### Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration ####### 

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = ["/afs/cern.ch/user/b/bigliett/work/DQ/group.det-muon.DiMuon10_100GeV.ESD.rel213_2020-06-18T2149_R3S_uTPC_v01_EXT1/group.det-muon.21673283.EXT1._000037.ESD.pool.root"]

include("MMRawDataMonitoring/MMMonRecOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager("mmMonManager")

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.useTrigger=False
DQMonFlags.monManFileKey             = "MON"

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.THistSvc.Output += [ DQMonFlags.monManFileKey()+" DATAFILE='monitoring.root' OPT='RECREATE'" ]

mmMonMan = topSequence.mmMonManager

from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags,'MMAthMonitorCfg')

from MMRawDataMonitoring.MMRawDataMonitoringConf import MMRawDataMonAlg
mmMonAlg = helper.addAlgorithm(MMRawDataMonAlg,'MMMonAlg')

mmMonAlg.TriggerChain = ''

mmGroup = helper.addGroup(mmMonAlg,'mmMonitor','Muon/MuonRawDataMonitoring/MM/')

mmGroup.defineHistogram('charge_all;Charge',  type='TH1F',
                        title='Charge;Charge[ADC];Number of Entries',
                        path='Overview',   xbins=20000, xmin=0., xmax=10000000.
                       )
mmGroup.defineHistogram('numberstrips;Number_of_strips',  type='TH1F',
                        title='Number_of_strips;Number of strips;Number of Entries',
                        path='Overview',   xbins=20, xmin=0., xmax=20.
                       )
mmGroup.defineHistogram('time_all;Time',  type='TH1F',
                        title='Time;Time[ns];Number of Entries',
                        path='Overview',   xbins=1000, xmin=0., xmax=100000.
                       )
mmGroup.defineHistogram('statEta,channel_mon;Channels_vs_StationEta',    type='TH2F',
                            title='Channels vs Station Eta;; Channel;',
                            path='Overview',   xbins=5, xmin=-2, xmax=3., xlabels=['#eta-2','#eta-1','','#eta1','#eta2'], ybins=5300, ymin=0., ymax=5300.
                            )

topSequence += helper.result()


