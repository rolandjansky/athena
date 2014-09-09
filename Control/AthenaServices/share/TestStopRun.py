##@file TestStopRun.py
# @brief test the IEventProcessor::stopRun mechanism
# @author Paolo Calafiura
# $Id: TestStopRun.py,v 1.1 2007-08-09 21:42:09 calaf Exp $
#==============================================================
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from GaudiExamples.GaudiExamplesConf import StopperAlg
job+=StopperAlg(StopCount=2)

from AthenaCommon.AppMgr import theApp
theApp.EvtMax=10

