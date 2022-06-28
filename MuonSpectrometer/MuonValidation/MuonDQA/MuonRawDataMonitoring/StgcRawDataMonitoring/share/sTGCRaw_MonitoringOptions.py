#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration #
#

if not 'MuonDQAFlags' in dir():
    print("MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now")
    from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

sTgcRawMonMan = AthenaMonManager(name="sTGCRawMonManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 OutputLevel         = muonOutputLevel
)

############# sTgcRawDataValAlg #############
from sTGCRawDataMonitoring.sTGCRawDataMonitoringConf import sTGCRawDataValAlg

#This it the main tool w/ detailed histograms
sTgcRawDataValAlg_main = sTGCRawDataValAlg(name='sTgcRawDataValAlg_main',
                                           Title='sTgc',
                                           OutputLevel = INFO
)

sTgcRawMonMan.AthenaMonTools += [ sTgcRawDataValAlg_main ]

topSequence += sTgcRawMonMan
print sTgcRawMonMan

