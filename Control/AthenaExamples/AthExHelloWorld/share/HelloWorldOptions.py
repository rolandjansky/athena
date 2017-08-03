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
from AthExHelloWorld.AthExHelloWorldConf import HelloAlg
job += HelloAlg( "HelloWorld" )   # 1 alg, named "HelloWorld"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for HelloAlg only (note name: instance, not type)
job.HelloWorld.OutputLevel = INFO

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

# For convenience, get a reference to the HelloAlg Algorithm
# named "HelloWorld" in the job
HelloWorld = job.HelloWorld

# Set an int property
HelloWorld.MyInt = 42

# Set a boolean property (False, True, 0, 1)
HelloWorld.MyBool = True

# Set a double property
HelloWorld.MyDouble = 3.14159

# Set a vector of strings property ...
HelloWorld.MyStringVec = [ "Welcome", "to", "Athena", "Framework", "Tutorial" ]

# ... and add one more:
HelloWorld.MyStringVec += [ "!" ]

# Set a map of strings to strings property ...
HelloWorld.MyDict = { 'Bonjour'      : 'Guten Tag',
                      'Good Morning' : 'Bonjour' , 
                      'one' : 'ein',
                      'two' : 'dos',
                      'three' : 'trois' }

# ... and add one more:
HelloWorld.MyDict[ "Goeiedag" ] = "Ni Hao"

# Set a table (a vector of pairs of doubles) ...
HelloWorld.MyTable = [ ( 1 , 1 ) , ( 2 , 4 ) , ( 3 , 9 ) ]

# ... and one more:
HelloWorld.MyTable += [ ( 4, 16 ) ]

# Set a matrix (a vector of vectors) ...
HelloWorld.MyMatrix = [ [ 1, 2, 3 ],
                        [ 4, 5, 6 ] ]

# ... and some more:
HelloWorld.MyMatrix += [ [ 7, 8, 9 ] ]

#--------------------------------------------------------------
# Algorithms Tool Usage Private Options (advanced and optional)
#--------------------------------------------------------------

# Import configurable for using our HelloTool
from AthExHelloWorld.AthExHelloWorldConf import HelloTool

# Setup a public tool so that it can be used (again, note name)
ToolSvc += HelloTool( "PublicHello" )
ToolSvc.PublicHello.MyMessage = "A Public Message!"

# Tell "HelloWorld" to use this tool ("MyPublicHelloTool" is a
# ToolHandle property of HelloAlg)
HelloWorld.MyPublicHelloTool = ToolSvc.PublicHello

# Hand "HelloWorld" a private HelloTool ("MyPrivateHelloTool" is
# a ToolHandler property of HelloAlg)
HelloWorld.MyPrivateHelloTool = HelloTool( "HelloTool" )
HelloWorld.MyPrivateHelloTool.MyMessage = "A Private Message!"

#==============================================================
#
# End of job options file
#
###############################################################

