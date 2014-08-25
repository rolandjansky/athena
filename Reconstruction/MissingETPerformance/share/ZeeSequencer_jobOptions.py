include.block( "MissingETPerformance/ZeeSequencer_jobOptions.py" )

include ( "MissingETPerformance/MissingETEventSelector_jobOptions.py" )
MissingETEventSelector.SelectionType="hasZboson_ee"

from MissingETPerformance.MissingETPerformanceConf import \
     ZeeSequencer as ConfiguredZeeSequencer

ZeeSequencer = ConfiguredZeeSequencer(
    # option="value"
    )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AlgSequence import AthSequencer
topSequence += AthSequencer('ZeeSequence')

topSequence.ZeeSequence += ZeeSequencer

print ZeeSequencer
