# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PerfMonVTune.PerfMonVTuneConf import VTuneProfilerService as _VTuneProfilerService

class VTuneProfilerService( _VTuneProfilerService ):
    
    # No new properties
    __slots__ = ()

    # Initialize from the base class
    def __init__( self, name = "VTuneProfilerService", **kwargs ):
        kwargs['name'] = name
        super( VTuneProfilerService, self ).__init__( **kwargs )

    # Enable the auditor hooks
    from AthenaCommon.AppMgr import theApp
    theApp.AuditAlgorithms = True
    theApp.AuditTools      = True
    theApp.AuditServices   = True

    # Set up the profiler service as the first service to be created:
    theApp.CreateSvc.insert( 0, "VTuneProfilerService/VTuneProfilerService" )
