# File: AthExJobOptions/AthExJobOptions_CustomToolJopOptions.py
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get hold of the algorithms that we want to run
from AthExJobOptions.AthExJobOptionsConf import TopAlgorithm
#    ^^package name  ^^generated file           ^^Gaudi component

# Custom configured tool to use
from AthExJobOptions.CustomToolUsingTool import CustomToolUsingTool
#    ^^package name  ^^user-authored file       ^^custom class

# Add the algorithm, and set its tool
job += TopAlgorithm( 'MyTopAlg' )
job.MyTopAlg.TheTool = CustomToolUsingTool( 'CustomTool' )
job.MyTopAlg.TheTool.Factor = 14.

# Another custom configured tool to use
from AthExJobOptions.CustomToolUsingTool2 import CustomToolUsingTool2
job += TopAlgorithm( 'YourTopAlg' )
job.YourTopAlg.OutputKey = 'YourKey'
job.YourTopAlg.TheTool = CustomToolUsingTool2( 'CustomTool2' )
job.YourTopAlg.TheTool.Factor = 18.

#job.MyTopAlg.OutputLevel = DEBUG
#job.MyTopAlg.TheTool.OutputLevel = DEBUG


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 2

# End of AthExJobOptions_CustomTopAlgorithmJobOptions.py
