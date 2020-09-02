
###############################################################
#
# Job options file
#
#==============================================================

# for Testing different classes in the serializer algorithm.
# To run in command line
#     athena.py HelloSerializerWorld.py
#    This will test all the TestClasses

# To specify a TestClass to be tested:
#    athena.py -c 'RunTest=i' HelloSerialzerWorld.py
# Where i is the number of the TestClass to be tested.

# To run a number of TestClasses
#    for i in 1 2 3 4 5 ; do athena.py -c "RunTest=$i" HelloSerializerWorld.py; done

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
#theApp.setup( MONTECARLO )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
#AthenaSealSvc.DictNames += [ "HelloSerializerWorldDictDict" ]
theApp.Dlls += [ "TrigSerializeResult" ]

#top algorithms to be run
theApp.TopAlg = [ "HelloSerializerWorld" ]
HelloWorld = Algorithm( "HelloSerializerWorld" )

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

MessageSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

#--------------------------------------------------------------
# Algorithims Private Options
#--------------------------------------------------------------

# For the HelloSerializerWorld alorithm


if (not 'RunTest' in dir()):
    HelloWorld.TestRunNumber=-1
else:
    HelloWorld.TestRunNumber=RunTest

    

