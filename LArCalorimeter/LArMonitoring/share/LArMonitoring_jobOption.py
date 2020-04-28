
include.block("LArMonitoring/LArMonitoring_jobOption.py")

#Create the set of flags
from AthenaMonitoring.DQMonFlags import DQMonFlags

#Add colltime algo to sequence

from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()

if DQMonFlags.monManEnvironment() == 'tier0ESD':
   from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfigOld
   topSequence +=LArCollisionTimeMonConfigOld(DQMonFlags)
   from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfigOld
   topSequence +=LArAffectedRegionsConfigOld(DQMonFlags)

if DQMonFlags.monManEnvironment() == 'tier0Raw':
    from LArMonitoring.LArNoisyROMonAlg import LArNoisyROMonConfigOld
    topSequence += LArNoisyROMonConfigOld(DQMonFlags)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource == 'data':
    from LArMonitoring.LArHVCorrMonAlg import LArHVCorrMonConfigOld
    topSequence += LArHVCorrMonConfigOld(DQMonFlags)

if DQMonFlags.monManEnvironment() == 'tier0Raw' and globalflags.DataSource == 'data':
    from LArMonitoring.LArDigitMonAlg import LArDigitMonConfigOld
    topSequence +=LArDigitMonConfigOld(DQMonFlags, topSequence)

    from LArMonitoring.LArRODMonAlg import LArRODMonConfigOld
    topSequence +=LArRODMonConfigOld(DQMonFlags)

    from LArMonitoring.LArFEBMonAlg import LArFEBMonConfigOld
    topSequence +=LArFEBMonConfigOld(DQMonFlags)

    from LArMonitoring.LArCoverageAlg import LArCoverageConfigOld
    topSequence +=LArCoverageConfigOld(DQMonFlags)

#print topSequence


