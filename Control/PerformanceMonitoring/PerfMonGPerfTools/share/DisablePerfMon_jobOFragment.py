# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: DisablePerfMon_jobOFragment.py 631453 2014-11-27 09:26:40Z will $
#
# This jobO fragment is used when setting up the profiling of an Athena job
# to turn off PerfMon. The PerfMon code can in some cases interfere with
# GPT in some weird ways...
#

# It should only be loaded once:
include.block( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" ) # noqa: F821

# Disable PerfMon as much as we can:
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring.set_Value_and_Lock( False )

#this is in a try block because AthAnalysisBase doesn't come with RecExConfig
try:
    from RecExConfig.RecFlags import rec
    rec.doPerfMon.set_Value_and_Lock( False )
    rec.doDetailedPerfMon.set_Value_and_Lock( False )
    rec.doSemiDetailedPerfMon.set_Value_and_Lock( False )
except ImportError:
    pass
