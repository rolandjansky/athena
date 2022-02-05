###########################################################################
#
# <Christophe.Le.Maner@cern.ch>
#
###########################################################################

#
# Default values for parameters (input directory, file name etc)
#
include("LArMonTools/LArMonVariables.py")

#######################################################################################
# print summary
#######################################################################################

Log = logging.getLogger( "RunLog" )
Log.info( " ======================================================== " )
Log.info( " ***                 LAr Run summary                  *** " )
Log.info( " ======================================================== " )
Log.info( " RunNumber                                 = " + str(RunNumber) )
Log.info( " SubDetector                               = " + SubDet )
Log.info( " Partition                                 = " + Partition )
Log.info( " Type                                      = " + Type )
Log.info( " online                                    = " + str(online) )
if not online:
    Log.info( " RootHistOutputFileName   = " + RootHistOutputFileName )  


include("LArConditionsCommon/LArMinimalSetup.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include ("LArMonTools/LArMonByteStream.py") # ByteStreamInputSvc and ByteStreamAddressProviderSvc
#include("LArMonTools/LArMonBuilders.py") # LArRampBuilder, LArCaliWaveBuilder, LArRawChannelBuilder...

