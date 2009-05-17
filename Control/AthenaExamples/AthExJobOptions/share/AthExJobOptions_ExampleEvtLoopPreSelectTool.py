# File: AthExJobOptions/AthExJobOptions_ExampleEvtLoopPreSelectTool.py
# Author: Marjorie Shapiro <mdshapiro@lbl.gov> based on work by
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get hold of the algorithms that we want to run
from AthExJobOptions.CustomTopAlgorithm import CustomTopAlgorithm
#    ^^package name  ^^user-authored file      ^^custom class

# Add the algorithm
job += CustomTopAlgorithm( 'MyCustomAlg' )

# Now, we are instantiating two instances of the example preselect tool
# This example tool passes the event is Mod(event number,PassIfMod)==0
from AthExJobOptions.AthExJobOptionsConf import ExampleEvtLoopPreSelectTool
myTool = ExampleEvtLoopPreSelectTool( "myTool" )
myTool.OutputLevel = DEBUG
myTool2 = ExampleEvtLoopPreSelectTool( "myTool2" )
myTool2.OutputLevel = DEBUG
myTool2.PassIfMod = 3

# And attaching these tools to the EventLoopManager
# The tools are run in order and stop processing if either
# one returns a false
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
#AthenaEventLoopMgr.PreSelectTools += [ "ExampleEvtLoopPreSelectTool/myTool" ]
AthenaEventLoopMgr.PreSelectTools += [ myTool ]
AthenaEventLoopMgr.PreSelectTools += [ myTool2 ]

# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 50

# Print the final job for reference purposes
print job

# End of AthExJobOptions_ExampleEvtLoopPreSelectTool.py
