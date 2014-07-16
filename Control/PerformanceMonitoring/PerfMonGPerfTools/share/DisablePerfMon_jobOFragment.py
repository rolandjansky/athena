# $Id: DisablePerfMon_jobOFragment.py 496380 2012-04-18 12:28:09Z ritsch $
#
# This jobO fragment is used when setting up the profiling of an Athena job
# to turn off PerfMon. The PerfMon code can in some cases interfere with
# GPT in some weird ways...
#

# It should only be loaded once:
include.block( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" )

# Disable PerfMon as much as we can:
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring.set_Value_and_Lock( False )
from RecExConfig.RecFlags import rec
rec.doPerfMon.set_Value_and_Lock( False )
rec.doDetailedPerfMon.set_Value_and_Lock( False )
rec.doSemiDetailedPerfMon.set_Value_and_Lock( False )
