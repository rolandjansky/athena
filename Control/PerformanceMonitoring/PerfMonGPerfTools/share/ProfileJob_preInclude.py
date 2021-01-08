# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: ProfileJob_preInclude.py 496380 2012-04-18 12:28:09Z ritsch $
#
# This job fragment can be pre-included in Reco_trf.py jobs to profile
# the entrie job (initialization, event loop and finalization) using GPT.
#
# WARNING: Unfortunately at the moment this doesn't really work.
#          GPT has serious issues with CPU profiling while Athena is
#          still loading its shared libraries. This leads to lots of
#          semi-random crashes. So, avoid this preInclude unless
#          you're trying to get it to work...
#

# First off, let's disable PerfMon. It doesn't mix well with GPT.
include( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" ) # noqa: F821

# Set up the profiler service:
from AthenaCommon.AppMgr import ServiceMgr
from PerfMonGPerfTools.PerfMonGPerfToolsConf import GPT__ProfilerService
ServiceMgr += GPT__ProfilerService( "ProfilerService" )
ServiceMgr.ProfilerService.ControlledProfiling = False
# Tell the service to profile the entire job:
ServiceMgr.ProfilerService.InitEvent       = -1
ServiceMgr.ProfilerService.ProfileFileName = "gpt-fulljob.profile"

# Set up the profiler service as the first service to be created:
theApp.CreateSvc.insert( 0, "GPT::ProfilerService/ProfilerService" ) # noqa: F821

# Print a message with what happened:
from AthenaCommon.Logging import logging
log = logging.getLogger( "ProfileJob_preInclude" )
log.info( "The entire job will get CPU profiling" )
