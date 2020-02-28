
include.block("LArMonitoring/LArMonitoring_jobOption.py")

#Create the set of flags
from AthenaMonitoring.DQMonFlags import DQMonFlags

#Add colltime algo to sequence

from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfigOld
topSequence +=LArCollisionTimeMonConfigOld(DQMonFlags)

if DQMonFlags.monManEnvironment() == 'tier0Raw':
    from LArMonitoring.LArDigitMonAlg import LArDigitMonConfigOld
    topSequence +=LArDigitMonConfigOld(DQMonFlags)

    from LArMonitoring.LArRODMonAlg import LArRODMonConfigOld
    topSequence +=LArRODMonConfigOld(DQMonFlags)

#from LArMonitoring.LArFEBMonAlg import LArFEBMonConfigOld
#topSequence +=LArFEBMonConfigOld(DQMonFlags)

#from LArMonitoring.LArCoverageAlg import LArCoverageConfigOld
#topSequence +=LArCoverageConfigOld(DQMonFlags)

#print topSequence


