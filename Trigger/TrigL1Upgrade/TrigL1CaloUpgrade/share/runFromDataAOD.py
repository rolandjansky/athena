

import ROOT

# later on we could define this in ROOT
TestFile="myAODTTv3.root"
OutputFile="test.output.root"
PoolAODInput=[TestFile]

# set conditions
DetDescrVersion="ATLAS-R2-2015-03-01-00"
#ConditionsTag="OFLCOND-RUN12-SDR-31"
ConditionsTag="CONDBR2-BLKPA-2015-16"
# set everything
include ("RecExCond/RecExCommon_DetFlags.py")
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)
globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
globalflags.DataSource.set_Value_and_Lock('data')
printfunc (globalflags)
include ("RecExCond/AllDet_detDescr.py")
printfunc (svcMgr.IOVDbSvc)
svcMgr.IOVDbSvc.DBInstance=''

# Set the input file
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = [TestFile]

# import the algorithm in python and make sure it
# is attached to the sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#theApp.initialize();

from LArL1Sim.LArL1SimConf import LArSCSimpleMaker
theLArSCSimpleMaker = LArSCSimpleMaker()
theLArSCSimpleMaker.SCellContainer="SCell"
topSequence+=theLArSCSimpleMaker

from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SimpleSuperCellChecks
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloEFex
from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import EFexAnalysis
a=SimpleSuperCellChecks()
b=TrigT1CaloEFex(EnableMonitoring=True)
c=EFexAnalysis(EnableMonitoring=True,DoTruth=False)
topSequence+=a
topSequence+=b
topSequence+=c

#for i in range(0,100):
#  theApp.nextEvent().isFailure()

#theApp.finalize();
#theApp.exit()
