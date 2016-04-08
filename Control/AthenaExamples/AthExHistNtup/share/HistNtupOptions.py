###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS standard config
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

# bring in McEventSelector
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Select HBOOK or ROOT persistency (ROOT is default)
theApp.HistogramPersistency = "ROOT"

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += CfgMgr.AthEx__Hist("Hist")
job += CfgMgr.AthEx__Ntup("Ntup")

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL )
#--------------------------------------------------------------

job.Hist.OutputLevel = DEBUG
job.Ntup.OutputLevel = DEBUG
svcMgr.MessageSvc.useColors = True

#--------------------------------------------------------------
# Histogram output file 
#--------------------------------------------------------------

# Specify the appropriate output file type
if not hasattr(svcMgr, 'THistSvc'):
    svcMgr += CfgMgr.THistSvc()
hsvc = svcMgr.THistSvc

hsvc.Output += [ "stat DATAFILE='hist.root' OPT='RECREATE'" ]

#--------------------------------------------------------------
# Ntuples
#--------------------------------------------------------------
hsvc.Output += [ "rec DATAFILE='ntuple.root' OPT='RECREATE'" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = 10

# rflx-dict loading
import AthenaPython.PyAthena as PyAthena
PyAthena.std.vector('long')

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

#--------------------------------------------------------------
# Batch/Interactive Control (uncomment the lines for batch mode)
#--------------------------------------------------------------

####theApp.run( theApp.EvtMax )
####theApp.exit()

#==============================================================
#
# End of job options file
#
###############################################################
