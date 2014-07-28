include.block("TrigmuFast/TriggerConfig_TrigmuFast.py")
#
# Configure a suitable TrigmuFast Algorithm instance
#
# Constructor arguments:
# level, type, threshold, isIsolated
# 
# e.g. level=L2, type=muon, threshold=30, isIsolated=None
#      level=EF, type=egamma, threshold=20, isIsolated=isolated
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a list of strings to be entered in the sequence file. This string
#              defines the class and instance name

class TriggerConfig_TrigmuFast:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "muon":
            self.__instname__ = "muFast_Muon_"
            self.__sequence__ = "muFast/muFast/Muon"
        if type == "900GeV":
            self.__instname__ = "muFast_900GeV_"
            self.__sequence__ = "muFast/muFast/900GeV"

        self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]



include( "TrigT1RPCRecRoiSvc/TrigT1RPCRecRoiSvcJobOptions.py" )
include( "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.py" )
theApp.Dlls += [ "TrigT1TGC" ]

include( "TGCgeometry/TGCgeometry_jobOptions.py" )

#load the libraries for the muFast algorithm
theApp.Dlls += [ "TrigmuFast" ]
theApp.ExtSvc += [ "MuRoadsSvc", "MuRoadsSvc/MuRoadsSvc_900GeV", "MuLUTSvc", "PtLUTSvc" ]

haveTiltedBField=False
if (DetDescrVersion.find("CSC")!=-1):
    fieldty=DetDescrVersion[13:15].strip()
    if fieldty!="00":
       haveTiltedBField=True
       
            
# configure the standard algorithm
Algorithm("muFast_Muon_L2").UseCalibration = True
Algorithm("muFast_Muon_L2").UseTiltedBField = haveTiltedBField

#configure the ROBmapper
if DetDescrVersion == "DC1":	    
    Algorithm ( "muFast_Muon_L2" ).ROBmapperFile = "RoiROBmap-P03.data"


#configure the loose algorithm
Algorithm("muFast_900GeV_L2").UseCalibration = True
Algorithm("muFast_900GeV_L2").UseTiltedBField = haveTiltedBField

#configure the ROBmapper
if DetDescrVersion == "DC1":	    
    Algorithm ( "muFast_900GeV_L2" ).ROBmapperFile = "RoiROBmap-P03.data"



muRoadsSvcLoose = Service("MuRoadsSvc_900GeV")
muRoadsSvcLoose.ScaleBarrelMiddleRoads = 3
muRoadsSvcLoose.ScaleBarrelInnerRoads = 3
muRoadsSvcLoose.ScaleBarrelOuterRoads = 3
