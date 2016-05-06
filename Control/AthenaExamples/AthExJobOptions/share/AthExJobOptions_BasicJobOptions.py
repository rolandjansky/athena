# File: AthExJobOptions/AthExJobOptions_BasicJobOptions.py
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:

MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
assert( job == AlgSequence() )     # this is a singleton

# Get hold of the algorithms that we want to run
from AthExJobOptions.AthExJobOptionsConf import TopAlgorithm
#    ^^package name  ^^generated file           ^^Gaudi component

# Some Tool algorithm classes that will/may be used
from AthExJobOptions.AthExJobOptionsConf import ConcreteTool, ToolSpace__TemplatedTool_double_, ToolUsingTool

# Next, algorithms need adding to the job. The following will
# add an algorithm named "TopAlgorithm" of type "TopAlgorithm":
job += TopAlgorithm()

# Alternatively, a specific name for an instance of the type
# "TopAlgorithm" can be chosen (here, 'MyAlg'):
job += TopAlgorithm( 'MyAlg' )

# There are now 2 algorithms in this job:
assert( len(job) == 2 )

# And another
job += TopAlgorithm( 'YourAlg' )

# Modify some job options parameters, by accessing the algorithms
# through their instance name:
job.TopAlgorithm.InputKey   = ''
job.TopAlgorithm.OutputKey  = 'AAA'
job.TopAlgorithm.TheTool = ToolSpace__TemplatedTool_double_( 'TSTT' )

job.MyAlg.InputKey   = job.TopAlgorithm.OutputKey
job.MyAlg.OutputKey  = 'BBB'

# register a service with the service manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += CfgMgr.ConcreteSvc(
   "QotdSvc",
   Qotd = "These are not the droids you are looking for"
   )
# and bind it to 'MyAlg'
job.MyAlg.TheSvc = svcMgr.QotdSvc

# overriding default list
job.MyAlg.PrivateToolList = [ ConcreteTool( "PyCT1" ),
                              ConcreteTool( "PyCT2" ),
                              ToolSpace__TemplatedTool_double_( 'PyTSTT3', Factor=30 ),
                              ToolSpace__TemplatedTool_double_( 'PyTSTT4', Factor=40 ) ]

# add my special tool
job.MyAlg.PrivateToolList += [ ToolUsingTool( 'Special',
                                              TheToolTool = ToolSpace__TemplatedTool_double_('PyTSTTSp1', Factor = 6.2830 ),
                                              TheToolToolArray = [ ConcreteTool('PySpSubTool', Factor = 3.1415 ) ] ) ]
# override some properties
job.MyAlg.PrivateToolList[ 'Special' ].Factor = 37

# try to add None as an empty private tool
job.MyAlg.EmptyPrivateTool = None
# try to add None as an empty public tool
job.MyAlg.EmptyPublicTool = None

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += ConcreteTool('PyCT5')
ToolSvc += ToolSpace__TemplatedTool_double_( 'PyTSTT7' )
# adding to default list
job.MyAlg.PublicToolList += [ ToolSvc.PyCT5, ToolSvc.PyTSTT7 ]
# Note that the parameters are type checked, this fails:
try:
   job.TopAlgorithm.InputKey = 42
except ValueError:
   log.info( "Can't set int to string property (this is correct)" )
else:
   log.fatal( "Managed to assign an int to a string property ... stop!" )
   raise RuntimeError( "Managed to assign an int to a string property" )

# Only actual properties can be set, this fails as well:
try:
   job.MyAlg.ThisDoesNotExist = None
except AttributeError:
   log.info( "Can't set non-existing property (this is correct)" )
else:
   log.fatal( "Managed to set a non-existing property ... stop!" )
   raise RuntimeError( "Managed to set a non-existing property" )

# For all TopAlgorithm instances, that do not yet have their tool defined,
# fill in a default:
for alg in job:
   if isinstance( alg, TopAlgorithm ) and not hasattr( alg, 'TheTool' ):
      alg.TheTool = ConcreteTool()

# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 2

# Hmm, you know what? Let's not run YourAlg after all
del job.YourAlg
assert( len(job) == 2 )

# Also, to see the DEBUG messages from TopAlgorithm instances, switch
# it on for all such algorithms (and only those):
def setdebug(c):
   c.OutputLevel = DEBUG
   for x in c.getAllChildren():
      setdebug(x)
   return
for alg in job:
   if isinstance( alg, TopAlgorithm ):
      setdebug(alg)

# Print the final job for reference purposes
print job

# End of AthExJobOptions_BasicJobOptions.py
