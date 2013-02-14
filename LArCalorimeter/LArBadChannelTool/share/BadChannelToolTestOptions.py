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
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelToolTest
job += LArBadChannelToolTest( "BadChanTest" )   # 1 alg, named "BadChanTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for LArBadChannelToolTest only (note name: instance, not type)
job.BadChanTest.OutputLevel = DEBUG

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 10

#--------------------------------------------------------------
# Algorithms Private Options (all optional)
#--------------------------------------------------------------

# For convenience, get a reference to the LArBadChannelToolTest Algorithm
# named "BadChanTest" in the job
BadChanTest = job.BadChanTest

# Set an int property
#BadChanTest.MyInt = 42

#--------------------------------------------------------------
# Algorithms Tool Usage Private Options (advanced and optional)
#--------------------------------------------------------------

# Import configurable for using our HelloTool
#from AthExHelloWorld.AthExHelloWorldConf import HelloTool
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool

# Setup a public tool so that it can be used (again, note name)
#ToolSvc += HelloTool( "PublicHello" )
#ToolSvc.PublicHello.MyMessage = "A Public Message!"
ToolSvc += LArBadChanTool("BadChanTool")
#ToolSvc.BadChanTool.EMBAfile = "noisePb_29142.txt"
ToolSvc.BadChanTool.EMBAfile = "badchannels.txt"
ToolSvc.BadChanTool.EMBCfile = "badchannels.txt"
ToolSvc.BadChanTool.OutputLevel = DEBUG

# Tell "HelloWorld" to use this tool ("MyPublicHelloTool" is a
# ToolHandle property of LArBadChannelToolTest)
#HelloWorld.MyPublicHelloTool = ToolSvc.PublicHello
BadChanTest.BadChannelTool = ToolSvc.BadChanTool

#==============================================================
#
# End of job options file
#
###############################################################

