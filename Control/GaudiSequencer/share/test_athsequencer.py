###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

###############################
# Load perf service
###############################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "athsequencer.pmon.gz"

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 100
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Sequence(s)
#--------------------------------------------------------------

# short-hands
HelloAlg = CfgMgr.HelloAlg
AthPrescaler = CfgMgr.AthPrescaler
AthEventCounter = CfgMgr.AthEventCounter

## Sequence 1 configuration ##
s1  = AthSequencer( 'Sequence1' )
s1 += AthPrescaler( 'Prescaler1', PercentPass = 50. )
s1 += HelloAlg( 'alg1', OutputLevel = Lvl.DEBUG )
s1 += AthEventCounter( 'Counter1' )

## Sequence 2 configuration ##
s2  = AthSequencer( 'Sequence2' )
s2 += [
    AthPrescaler( 'Prescaler2', PercentPass = 10. ),
    HelloAlg( 'alg2', OutputLevel = Lvl.DEBUG ),
    AthEventCounter('Counter2'),
    ]

## Top-sequence configuration ##
topSequence += CfgMgr.AthSequencer( 'TopSequence', StopOverride = True )
topSequence.TopSequence += s1
topSequence.TopSequence += s2


#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
svcMgr.MessageSvc.defaultLimit = 4000000
#svcMgr.MessageSvc.OutputLevel  = ERROR
#==============================================================
#
# End of job options file
#
###############################################################
