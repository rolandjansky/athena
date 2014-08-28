###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# No event selector needed for basic 'Hello World!' Algorithm

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from AraToolExample.AraToolExampleConf import AraToolExampleAlg
job += AraToolExampleAlg("AraToolExampleAlg")

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for HelloAlg only (note name: instance, not type)
job.AraToolExampleAlg.OutputLevel = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 10

from AraToolExample.AraToolExampleConf import AraAlgToolWrapper_AnExampleAraTool_
job.AraToolExampleAlg.TheTool=AraAlgToolWrapper_AnExampleAraTool_("AraAlgToolWrapper_AnExampleAraTool_")

#==============================================================
#
# End of job options file
#
###############################################################

