###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from TBBExamples.TBBExamplesConf import HelloGraphAlg
job += HelloGraphAlg( "HelloWorld" )   # 1 alg, named "HelloWorld"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

svcMgr.MessageSvc.OutputLevel = Lvl.DEBUG


# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 2

#--------------------------------------------------------------
# Algorithms Private Options (all optional)
#--------------------------------------------------------------

# For convenience, get a reference to the HelloParForAlg Algorithm
# named "HelloWorld" in the job
HelloWorld = job.HelloWorld

# Max number of pipe stages to run in parallel
HelloWorld.NIters = 10

#--------------------------------------------------------------
# Algorithms Tool Usage Private Options (advanced and optional)
#--------------------------------------------------------------

# Import configurable for using our HelloTool
from TBBExamples.TBBExamplesConf import TBBTool
# Setup a public tool so that it can be used (again, note name)
ToolSvc += TBBTool( "PublicHello" )
ToolSvc.PublicHello.MyIncrement = 100

# Tell "HelloWorld" to use this tool ("MyPublicHelloTool" is a
# ToolHandle property of HelloGraphAlg)
HelloWorld.MyPublicHelloTool = ToolSvc.PublicHello

# Hand "HelloWorld" a private HelloTool ("MyPrivateHelloTool" is
# a ToolHandler property of HelloGraphAlg)
HelloWorld.MyPrivateHelloTool = TBBTool( "HelloTool" )
HelloWorld.MyPrivateHelloTool.MyIncrement = -200

#==============================================================
#
# End of job options file
#
###############################################################

