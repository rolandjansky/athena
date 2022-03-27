#
# Changing the names of the RdoToPrepData output Containers
#


if hasattr(topSequence,'MdtRdoToMdtPrepData'):
    topSequence.MdtRdoToMdtPrepData.DecodingTool.OutputCollection = "MDT_DriftCircles_unfiltered"

if hasattr(topSequence,'CscRdoToCscPrepData'):
    topSequence.CscRdoToCscPrepData.CscRdoToCscPrepDataTool.OutputCollection = "CSC_Measurements_unfiltered"

if hasattr(topSequence,'RpcRdoToRpcPrepData'):
    topSequence.RpcRdoToRpcPrepData.DecodingTool.TriggerOutputCollection = "RPC_Measurements_unfiltered"

if hasattr(topSequence,'TgcRdoToTgcPrepData'):
    topSequence.TgcRdoToTgcPrepData.DecodingTool.OutputCollection = "TGC_Measurements_unfiltered"


#--------------------------------------------------------------------------
# Configuration of MuonIdCutTool
#--------------------------------------------------------------------------

#multiple cuts may be selected at the same time.
#if etaCut and phiCut are both true, the program is smart enough to know that technologies
#with no lists shouldn't be cut, but if either eta or philists are set for a technology,
#both should be set or it will cut the whole technology.

from MuonPrdSelector.MuonPrdSelectorConf import MuonIdCutTool

MuonIdCutTool = MuonIdCutTool()
MuonIdCutTool.OutputLevel = INFO

MuonIdCutTool.CutStationRegion = False
MuonIdCutTool.CutStationName = False
MuonIdCutTool.CutSubstation = False
MuonIdCutTool.CutEta = False
MuonIdCutTool.CutPhi = False
MuonIdCutTool.CutEE = False


MuonIdCutTool.CutMdtStationNameList = [] #Integer, maps to BIL,EOS,etc
MuonIdCutTool.CutCscStationNameList = []
MuonIdCutTool.CutRpcStationNameList = []
MuonIdCutTool.CutTgcStationNameList = []

MuonIdCutTool.CutMdtRegionList = [] #Inner-0,Extra-1,Middle-2,Outer-3
#MuonIdCutTool.CutCscRegionList = []
MuonIdCutTool.CutRpcRegionList = []
#MuonIdCutTool.CutTgcRegionList = []
MuonIdCutTool.CutRpcRegionList = [0, 1, 2, 3]

MuonIdCutTool.CutMdtMultilayerList = []
MuonIdCutTool.CutRpcDoubletRList = []
MuonIdCutTool.CutRpcGasGapList = []
MuonIdCutTool.CutTgcGasGapList = []

#Syntax is Eta/Sector, list of EEL chambers to KEEP
MuonIdCutTool.EELList = []
#MuonIdCutTool.EELList +=["1/2"]

#note that eta/phi lists specify region to KEEP
#this was done for consistency with earlier versions
MuonIdCutTool.MdtBarrelEtaList = []#-8 to +8, 0 is eta=0
MuonIdCutTool.MdtEndcapEtaList = []#-6 to +6, abs val increaseswith increasing |R|
MuonIdCutTool.MdtSectorList = [] #1-16, increases with phi, starts x direction

MuonIdCutTool.RpcEtaList = [] #same as mdtbarrel
MuonIdCutTool.RpcSectorList = []

MuonIdCutTool.CscEtaList = [] #always 1 for all sectors
MuonIdCutTool.CscSectorList = [] #same as mdt

#be careful, the numbering/placement of tgcs is quite different
#forward tgc sectors have eta = 1
MuonIdCutTool.TgcEtaList = [] #-5 to +5 abs val increaseswith increasing |R|
MuonIdCutTool.TgcEndcapPhiList = [] #1-24
MuonIdCutTool.TgcForwardPhiList = []#1-48

#
# Creating an instance of the MuonPrdSelectorAlg
#
# Setting the names of the input/output Containers of the MuonPrdSelectorAlg.
# The output containers have the original name of the containers used by
# the reconstruction algorithms.
#
from MuonPrdSelector.MuonPrdSelectorConf import MuonPrdSelectorAlg
MuonPrdSelectorAlg = MuonPrdSelectorAlg( MDT_PRDinputContainer  = "MDT_DriftCircles_unfiltered",
                                         MDT_PRDoutputContainer = "MDT_DriftCircles",

                                         RPC_PRDinputContainer  = "RPC_Measurements_unfiltered",
                                         RPC_PRDoutputContainer = "RPC_Measurements",

                                         TGC_PRDinputContainer  = "TGC_Measurements_unfiltered",
                                         TGC_PRDoutputContainer = "TGC_Measurements",

                                         CSC_PRDinputContainer  = "CSC_Measurements_unfiltered",
                                         CSC_PRDoutputContainer = "CSC_Measurements",
                                         MuonIdCutTool = MuonIdCutTool )


topSequence += MuonPrdSelectorAlg
