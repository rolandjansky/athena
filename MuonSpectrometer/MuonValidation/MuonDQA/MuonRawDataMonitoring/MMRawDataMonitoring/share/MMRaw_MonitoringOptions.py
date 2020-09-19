###### Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration #######

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
isTier0Flag = not athenaCommonFlags.isOnline()

if not 'MuonDQAFlags' in dir():
    print "MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now"
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

mmRawMonMan = AthenaMonManager(name="MMRawMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                OutputLevel         = muonOutputLevel)

############# MMRawDataValAlg #############
from MMRawDataMonitoring.MMRawDataMonitoringConf import MMRawDataValAlg

#This it the main tool w/ detailed histograms
mmRawDataValAlg_main = MMRawDataValAlg(name='mmRawDataValAlg_main',
                                       Title='MM',
                                       OutputLevel = INFO
                                       )

mmRawMonMan.AthenaMonTools += [ mmRawDataValAlg_main ]


topSequence += mmRawMonMan
print mmRawMonMan
##################################################################
