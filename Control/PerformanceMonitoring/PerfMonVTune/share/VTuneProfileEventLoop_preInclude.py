# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set up the profiler service:
from AthenaCommon.AppMgr import ServiceMgr
from PerfMonVTune.PerfMonVTuneConf import VTuneProfilerService
ServiceMgr += VTuneProfilerService("VTuneProfilerService")

ServiceMgr.VTuneProfilerService.ResumeEvent = 10

# Set up the profiler service as the first service to be created:
theApp.CreateSvc.insert( 0, "VTuneProfilerService/VTuneProfilerService" ) # noqa: F821

# Print a message with what happened:
from AthenaCommon.Logging import logging
log = logging.getLogger( "VTuneProfileEventLoop_preInclude" )
log.info( "The job's event loop will get profiling" )
