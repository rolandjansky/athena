include.block("TrigDiMuon/TriggerConfig_TrigDiMuon.py")
#
# Configure a suitable TrigDiMuon Algorithm instance
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

class TriggerConfig_TrigDiMuon:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        self.__instname__ = "TrigDiMuonFast_1_"
        self.__sequence__ = "TrigDiMuonFast/TrigDiMuonFast/1"

        self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]



include( "TrigT1RPCRecRoiSvc/TrigT1RPCRecRoiSvcJobOptions.py" )
include( "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.py" )
theApp.Dlls += [ "TrigT1TGC" ]

#load the libraries for the TrigDiMuonFast algorithm
theApp.Dlls += [ "TrigDiMuon" ]

haveTiltedBField=False
RpcLut="Lut"
if (DetDescrVersion.find("CSC")!=-1):
    fieldty=DetDescrVersion[13:15].strip()
    if fieldty!="00":
       haveTiltedBField=True
       RpcLut="Lut-CSC-01-02-00"

# configure the standard algorithm
Algorithm("TrigDiMuonFast_1_L2").UseTiltedBField = haveTiltedBField
Algorithm("TrigDiMuonFast_1_L2").RpcPhiFactor = 2.
Algorithm("TrigDiMuonFast_1_L2").RpcEtaFactor = 2.
Algorithm("TrigDiMuonFast_1_L2").TgcPhiFactor = 2.
Algorithm("TrigDiMuonFast_1_L2").TgcEtaFactor = 2.
Algorithm("TrigDiMuonFast_1_L2").MdtEtaFactor = 2.
Algorithm("TrigDiMuonFast_1_L2").doTiming = False
Algorithm("TrigDiMuonFast_1_L2").doNtuple = False
Algorithm("TrigDiMuonFast_1_L2").doTruth = False
Algorithm("TrigDiMuonFast_1_L2").doLutTgcRdo = True
Algorithm("TrigDiMuonFast_1_L2").LutFileName = RpcLut
Algorithm("TrigDiMuonFast_1_L2").CreateLUT = False
