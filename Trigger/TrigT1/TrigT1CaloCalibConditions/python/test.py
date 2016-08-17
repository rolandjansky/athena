# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time

from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
ServiceMgr += L1CaloCondSvc()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'data'
globalflags.DatabaseInstance = 'CONDBR2'
from IOVDbSvc.CondDB import conddb

svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib <tag>HEAD</tag>')
svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> /TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib <tag>HEAD</tag>')
svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> /TRIGGER/L1Calo/V1/Conditions/DisabledTowers <tag>HEAD</tag>')
svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> /TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels <tag>HEAD</tag>')
svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> /TRIGGER/L1Calo/V2/Configuration/PprChanDefaults <tag>HEAD</tag>')
svcMgr.IOVDbSvc.Folders.append('<db>COOLONL_TRIGGER/CONDBR2</db> /TRIGGER/L1Calo/V1/Configuration/PprChanDefaults <tag>HEAD</tag>')
svcMgr.IOVDbSvc.forceTimestamp = 1446047363
svcMgr.IOVDbSvc.forceRunNumber = 283851

from AthenaPython import PyAthena
theApp.initialize()
s = PyAthena.py_svc('L1CaloCondSvc', True, 'L1CaloCondSvc')

theApp.nextEvent()

# test L1CaloPprChanCalib (default, i.e. V2)
c = cppyy.gbl.L1CaloPprChanCalibContainer()
s.retrieve(c)
chan = c.pprChanCalib(0x100000)
assert chan.channelId() == 0x100000
assert chan.pedValue() == 33
assert chan.firCoeff1() == 2
assert chan.lutCpOffset() == 32400
assert chan.lutCpSlope() == 1138

# test L1CaloPprChanCalib (V1)
c = cppyy.gbl.L1CaloPprChanCalibV1Container()
s.retrieve(c)
chan = c.pprChanCalibV1(0x100000)
assert chan.channelId() == 0x100000
assert chan.pedValue() == 31
assert chan.firCoeff1() == 6
assert chan.lutOffset() == 42397
assert chan.lutSlope() == 1094

# test L1CaloPprChanDefaults (default, i.e. V2)
c = cppyy.gbl.L1CaloPprChanDefaultsContainer()
s.retrieve(c)
chan = c.pprChanDefaults(0x1)
assert chan.channelId() == 0x1
assert chan.decisionSource() == 0
assert chan.peakFinderCond() == 0
assert chan.satOverride1() == 1

# test L1CaloPprChanDefaults (V1)
c = cppyy.gbl.L1CaloPprChanDefaultsV1Container()
s.retrieve(c)
chan = c.pprChanDefaultsV1(0x1)
assert chan.channelId() == 0x1
assert chan.decisionSource() == 1
assert chan.peakFinderCond() == 0
assert chan.satOverride1() == 1

# test L1CaloPpmDeadChannels
c = cppyy.gbl.L1CaloPpmDeadChannelsContainer()
s.retrieve(c)
chan = c.ppmDeadChannels(0x6120d03)
assert chan.channelId() == 0x6120d03
assert chan.noiseCut() == 0
assert chan.errorCode() == 0

# test L1CaloPpmDeadChannels
c = cppyy.gbl.L1CaloDisabledTowersContainer()
s.retrieve(c)
chan = c.disabledTowers(0x71f0f03)
assert chan.channelId() == 0x71f0f03
assert chan.disabledBits() == 1

exit()
