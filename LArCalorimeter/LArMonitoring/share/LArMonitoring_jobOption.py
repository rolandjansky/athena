
include.block("LArMonitoring/LArMonitoring_jobOption.py")

#Create the set of flags
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags import globalflags
from LArMonTools.LArMonFlags import LArMonFlags

from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()

if 'ESD' not in DQMonFlags.monManEnvironment():
    include ("LArCellRec/LArCollisionTime_jobOptions.py")
    from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfigOld
    topSequence +=LArCollisionTimeMonConfigOld(DQMonFlags)
    if globalflags.DataSource()=='data' and 'online' not in DQMonFlags.monManEnvironment():
        from LArMonitoring.LArAffectedRegionsAlg import LArAffectedRegionsConfigOld
        topSequence +=LArAffectedRegionsConfigOld(DQMonFlags)

if 'ESD' not in DQMonFlags.monManEnvironment():
    from LArMonitoring.LArNoisyROMonAlg import LArNoisyROMonConfigOld
    topSequence += LArNoisyROMonConfigOld(DQMonFlags)

if globalflags.DataSource() == 'data' and 'online' not in DQMonFlags.monManEnvironment():
    from LArMonitoring.LArHVCorrMonAlg import LArHVCorrMonConfigOld
    topSequence += LArHVCorrMonConfigOld(DQMonFlags)

if 'ESD' not in DQMonFlags.monManEnvironment() and globalflags.DataSource() == 'data':
    if LArMonFlags.doLArDigitMon():
       from LArMonitoring.LArDigitMonAlg import LArDigitMonConfigOld
       topSequence +=LArDigitMonConfigOld(DQMonFlags)

#    if not DQMonFlags.doLArMon():
    from LArMonitoring.LArRODMonAlg import LArRODMonConfigOld
    if LArMonFlags.doLArRODMonTool():
       topSequence +=LArRODMonConfigOld(DQMonFlags)

    from LArMonitoring.LArFEBMonAlg import LArFEBMonConfigOld
    if LArMonFlags.doLArFEBMon():
       topSequence +=LArFEBMonConfigOld(DQMonFlags)

    from LArMonitoring.LArCoverageAlg import LArCoverageConfigOld
    topSequence +=LArCoverageConfigOld(DQMonFlags)

    from LArMonitoring.LArCosmicsMonAlg import LArCosmicsMonConfigOld
    if LArMonFlags.doLArCosmicsMonTool():
       topSequence +=LArCosmicsMonConfigOld(DQMonFlags)

    from LArMonitoring.LArRawChannelMonAlg import LArRawChannelMonConfigOld
    if LArMonFlags.doLArRawChannelMon():
       topSequence += LArRawChannelMonConfigOld(DQMonFlags)
       # for cosmics needs also elec noise
       from AthenaCommon.BeamFlags import jobproperties
       if jobproperties.Beam.beamType() == 'cosmics':
          from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
          CaloNoiseCondAlg(noisetype='electronicNoise') 


