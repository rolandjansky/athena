
include.block("LArMonitoring/LArMonitoring_jobOption.py")

#Create the set of flags
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags import globalflags

from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()

if 'ESD' not in DQMonFlags.monManEnvironment():
    include ("LArCellRec/LArCollisionTime_jobOptions.py")
    from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfigOld
    topSequence +=LArCollisionTimeMonConfigOld(DQMonFlags)
    if globalflags.DataSource()=='data':
        from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfigOld
        topSequence +=LArAffectedRegionsConfigOld(DQMonFlags)

if 'ESD' not in DQMonFlags.monManEnvironment():
    from LArMonitoring.LArNoisyROMonAlg import LArNoisyROMonConfigOld
    topSequence += LArNoisyROMonConfigOld(DQMonFlags)

if globalflags.DataSource == 'data' and 'online' not in DQMonFlags.monManEnvironment():
    from LArMonitoring.LArHVCorrMonAlg import LArHVCorrMonConfigOld
    topSequence += LArHVCorrMonConfigOld(DQMonFlags)

if 'ESD' not in DQMonFlags.monManEnvironment() and globalflags.DataSource == 'data':
    from LArMonitoring.LArDigitMonAlg import LArDigitMonConfigOld
    topSequence +=LArDigitMonConfigOld(DQMonFlags)

    from LArMonitoring.LArRODMonAlg import LArRODMonConfigOld
    topSequence +=LArRODMonConfigOld(DQMonFlags)

    from LArMonitoring.LArFEBMonAlg import LArFEBMonConfigOld
    topSequence +=LArFEBMonConfigOld(DQMonFlags)

    from LArMonitoring.LArCoverageAlg import LArCoverageConfigOld
    topSequence +=LArCoverageConfigOld(DQMonFlags)

    from LArMonitoring.LArCosmicsMonAlg import LArCosmicsMonConfigOld
    topSequence +=LArCosmicsMonConfigOld(DQMonFlags)

