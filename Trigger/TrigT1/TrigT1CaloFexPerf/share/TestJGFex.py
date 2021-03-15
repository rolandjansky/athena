# jps.AthenaCommonFlags.AccessMode = "POOLAccess" # use POOL read mode because reading calocells
# svcMgr.EventSelector.InputCollections = jps.AthenaCommonFlags.FilesInput()
from AthenaCommon.GlobalFlags import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool

# from RecExConfig import AutoConfiguration
# AutoConfiguration.ConfigureSimulationOrRealData()
# AutoConfiguration.ConfigureGeo()
# AutoConfiguration.ConfigureConditionsTag()
# from AthenaCommon.DetFlags import DetFlags
# DetFlags.detdescr.all_setOff()
# DetFlags.detdescr.Calo_setOn()
# include("RecExCond/AllDet_detDescr.py")

# include( "CaloConditions/CaloConditions_jobOptions.py" )
include("LArDetDescr/LArDetDescr_joboptions.py")


include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

IOVBeginRun = IOVRunNumberMin
IOVEndRun = IOVRunNumberMax
IOVBeginLB = IOVLBNumberMin
IOVEndLB = IOVLBNumberMax

import RegistrationServices.IOVRegistrationSvc

regSvc = svcMgr.IOVRegistrationSvc


# svcMgr.IOVDbSvc.DBInstance=""

if "GlobalTag" not in dir():
    GlobalTag = "OFLCOND-CSC-00-01-00"  # Sadly event his doesn't work for the 14 TeV jetjet sample 'OFLCOND-MC15c-SDR-14-02' #This works for 13 TeV 'OFLCOND-CSC-00-01-00'    #No idea what this is: COMCOND-BLKPST-004-05'

# svcMgr.IOVDbSvc.GlobalTag = GlobalTag

svcMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag()

# configure detector description from metadata in input file
from RecExConfig import AutoConfiguration

AutoConfiguration.ConfigureSimulationOrRealData()
AutoConfiguration.ConfigureGeo()
AutoConfiguration.ConfigureConditionsTag()
from AthenaCommon.DetFlags import DetFlags

DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")


###########
# Start the configuration of our algorithms

# Set up the conditions algorithms
from AthenaCommon.AlgSequence import AthSequencer

condSequence = AthSequencer("AthCondSeq")
condSequence += CfgMgr.LVL1__JTowerMappingDataCondAlg(
    "JTowerMappingDataCondAlg",
    MappingData="JTowerMappingData",
    MapTileCells=False,  # TODO: FLAGS
)
condSequence += CfgMgr.LVL1__GTowerMappingDataCondAlg(
    "GTowerMappingDataCondAlg",
    MappingData="GTowerMappingData",
    MapTileCells=False,  # TODO: FLAGS
)

scellsIn = "SCell"

athAlgSeq += CfgMgr.LVL1__JGTowerBuilder(
    "JTowerBuilder",
    UseSCQuality=True,
    UseAllCalo=False,
    InputSuperCells=scellsIn,
    EmulateSuperCellTiming=False,
    MinSCETp=-1,
    MaxSCETm=1,
    MinTowerET=-9e9,
    MappingData="JTowerMappingData",
    OutputTowers="JTowers",
)

athAlgSeq += CfgMgr.LVL1__JGTowerNoiseAlg(
    "JTowerNoiseAlg",
    InputTowers="JTowers",
    DoJFEX=True,
)

athAlgSeq += CfgMgr.LVL1__JGTowerBuilder(
    "GCaloTowerBuilder",
    UseSCQuality=True,
    UseAllCalo=False,
    InputSuperCells=scellsIn,
    EmulateSuperCellTiming=False,
    MinSCETp=-1,
    MaxSCETm=1,
    MinTowerET=-9e9,
    MappingData="GTowerMappingData",
    OutputTowers="GCaloTowers",
)

athAlgSeq += CfgMgr.LVL1__GTowersFromGCaloTowers(
    "GTowersFromGCaloTowers",
    InputTowers="GCaloTowers",
    OutputTowers="GTowers",
    MappingData="GTowerMappingData",
)

athAlgSeq += CfgMgr.LVL1__JGTowerNoiseAlg(
    "GTowerNoiseAlg",
    InputTowers="GTowers",
    DoJFEX=False,
)

athAlgSeq += CfgMgr.LVL1__JTowerRhoSubtractionAlg(
    "JTowerRhoSubtractionAlg",
    InputTowers="JTowers",
    OutputTowers="JTowerRhoSubtracted",
    OutputRho="JFEXRho",
)

athAlgSeq += CfgMgr.LVL1__GTowerRhoSubtractionAlg(
    "GTowerRhoSubtractionAlg",
    InputTowers="GTowers",
    OutputTowers="GTowerRhoSubtracted",
    OutputRho="GFEXRho",
)

athAlgSeq += CfgMgr.LVL1__GBlockBuilder(
    "GBlockBuider",
    InputTowers="GTowers",
    OutputBlocks="GBlocks",
)

athAlgSeq += CfgMgr.LVL1__GBlockBuilder(
    "RhoSubtractedGBlockBuilder",
    InputTowers="GTowerRhoSubtracted",
    OutputBlocks="GBlocksRhoSubtracted",
)

athAlgSeq += CfgMgr.LVL1__METNoiseCutPerfFex(
    "jXERHOFex",
    InputTowers="JTowerRhoSubtracted",
    OutputMET="jXERHOPerf",
    InputTowerNoise="",
    UseNegativeTowers=True,
)

athAlgSeq += CfgMgr.LVL1__METNoiseCutPerfFex(
    "jXENOISECUTFex",
    InputTowers="JTowers",
    OutputMET="jXENOISECUTFex",
    InputTowerNoise="noise",
    UseNegativeTowers=False,
)

athAlgSeq += CfgMgr.LVL1__METNoiseCutPerfFex(
    "gXERHOFex",
    InputTowers="GTowerRhoSubtracted",
    OutputMET="gXERHOPerf",
    InputTowerNoise="",
    UseNegativeTowers=True,
)

athAlgSeq += CfgMgr.LVL1__METNoiseCutPerfFex(
    "gXENOISECUTFex",
    InputTowers="GTowers",
    OutputMET="gXENOISECUTFex",
    InputTowerNoise="noise",
    UseNegativeTowers=False,
)

athAlgSeq += CfgMgr.LVL1__METJWoJPerfFex(
    "gXEJWOJFex",
    InputGBlocks="GBlocks",
    OutputMET="gXEJWOJPerf",
    OutputMHT="gXEJWOJPerfMHT",
    OutputMST="gXEJWOJPerfMST",
    UseNegativeTowers=True,
    UseRho=False,
)

athAlgSeq += CfgMgr.LVL1__METJWoJPerfFex(
    "gXEJWOJRHOFex",
    InputGBlocks="GBlocksRhoSubtracted",
    OutputMET="gXEJWOJRHOPerf",
    OutputMHT="gXEJWOJRHOPerfMHT",
    OutputMST="gXEJWOJRHOPerfMST",
    UseNegativeTowers=False,
    UseRho=False,
)

athAlgSeq += CfgMgr.LVL1__METJWoJPerfFex(
    "gXEJWOJRHOHTFex",
    InputGBlocks="GBlocksRhoSubtracted",
    OutputMET="gXEJWOJRHOHTPerf",
    OutputMHT="gXEJWOJRHOHTPerfMHT",
    OutputMST="gXEJWOJRHOHTPerfMST",
    UseNegativeTowers=True,
    UseRho=True,
    InputRho="GFEXRho",
)

svcMgr.StoreGateSvc.Dump = True
svcMgr.MessageSvc.defaultLimit = 999999999