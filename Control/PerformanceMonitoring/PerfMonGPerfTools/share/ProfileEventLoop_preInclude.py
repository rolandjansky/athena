# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: ProfileEventLoop_preInclude.py 496380 2012-04-18 12:28:09Z ritsch $
#
# This jobO fragment can be pre-included in Reco_trf.py jobs to profile
# just the event loop of those jobs with GPT.
#

# First off, let's disable PerfMon. It doesn't mix well with GPT.
include( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" ) # noqa: F821

# Set up the profiler service:
from AthenaCommon.AppMgr import ServiceMgr
from PerfMonGPerfTools.PerfMonGPerfToolsConf import GPT__ProfilerService
ServiceMgr += GPT__ProfilerService( "ProfilerService" )
ServiceMgr.ProfilerService.ControlledProfiling = False
# By default the event loop profiling only starts after 10 events
# have already been processed:
ServiceMgr.ProfilerService.InitEvent       = 10
ServiceMgr.ProfilerService.ProfileFileName = "gpt-execute.profile"

# Set up the profiler service as the first service to be created:
theApp.CreateSvc.insert( 0, "GPT::ProfilerService/ProfilerService" ) # noqa: F821

# Print a message with what happened:
from AthenaCommon.Logging import logging
log = logging.getLogger( "ProfileEventLoop_preInclude" )
log.info( "The job's event loop will get CPU profiling" )
