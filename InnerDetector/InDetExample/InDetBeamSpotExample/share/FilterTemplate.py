# $Id $
#
# JobRunner templates to filter AOD files and write out the selected events.
#
# Written by Juerg Beringer in December 2010.
#
print "InDetBeamSpotExample INFO Using $Id $"


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'inputfiles' in jobConfig:                    jobConfig['inputfiles'] = [ 'input.root' ]
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'outputfileprefix' in jobConfig:              jobConfig['outputfileprefix'] = ''
if not 'outputfile' in jobConfig:                    jobConfig['outputfile'] = jobConfig['outputfileprefix']+'dpd.root'

#include("InDetBeamSpotExample/JobSetupFragment.py")

import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AthenaCommonFlags import jobproperties as jp

jp.AthenaCommonFlags.FilesInput = jobConfig['inputfiles']

include('RecExCommon/aodtoaod.py')
rec.doAOD.set_Value_and_Lock(True)

# Number of events
theApp.EvtMax = jobConfig['evtmax']

# Event filtering
if 'bcidList' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.BCIDFilter(jobConfig['bcidList'])
if 'lbList' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.LBFilter(jobConfig['lbList'])
if 'lbData' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.LumiBlockFilter(jobConfig['lbData'])
if 'zRange' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.ZFilter(jobConfig['zRange'])

from AthenaCommon.AppMgr import theApp
StreamAOD = theApp.getOutputStream( "StreamAOD" )
StreamAOD.OutputFile = jobConfig['outputfile']

# Reduce verbosity
theApp.setOutputLevel = jobConfig['outputlevel']
ServiceMgr.MessageSvc.OutputLevel =  jobConfig['outputlevel']
#ServiceMgr.MessageSvc.Format      = "% F%40W%S%7W%R%T %0W%M"
eventLoopMgr = getattr( CfgMgr, theApp.EventLoop )()
eventLoopMgr.OutputLevel = jobConfig['outputlevel']
ServiceMgr += eventLoopMgr
