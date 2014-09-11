###############################################################
#
# Job options file
#
#==============================================================

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 20
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Sequence(s)
#--------------------------------------------------------------

## Sequencer configuration ##
job += AthSequencer ('seq')

from GaudiSequencer.PyComps import PyEvtFilter
job.seq += PyEvtFilter('alg',
                       evt_info='',
                       OutputLevel=Lvl.INFO)
job.seq += CfgMgr.AthEventCounter('counter', OutputLevel=Lvl.INFO)

job.seq.alg.evt_list = [1, 4, 5, 6]
job.seq.alg.filter_policy = 'reject'
#job.seq.alg.filter_fct = lambda run,evt: evt%2 == 0

svcMgr.MessageSvc.OutputLevel = Lvl.ERROR
#==============================================================
#
# End of job options file
#
###############################################################
