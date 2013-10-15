#**************************************************************
#
# MTHelloWorld example
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from TrigExMTHelloWorld.TrigExMTHelloWorldConf import MTHelloWorld
HelloWorld = MTHelloWorld("HelloWorld")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += HelloWorld

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

# For the genuine HelloWorld algorithm
HelloWorld.MyInt = 42
HelloWorld.MyBool = TRUE
HelloWorld.MyDouble = 3.14159
HelloWorld.MyStringVec = [ "Welcome", "to", "Athena", "Framework", "Tutorial" ]

# For a special HelloWorld algorithm
#HelloWorld__1 = Algorithm( "HelloWorld__1" )
#HelloWorld__1.MyInt = 21
#HelloWorld__1.MyBool = FALSE
#HelloWorld__1.MyDouble = 6.28
#HelloWorld__1.MyStringVec = [ "Welcome", "to", "Thread", "1" ]

#==============================================================
#
# End of MTHelloWorld example
#
#**************************************************************
