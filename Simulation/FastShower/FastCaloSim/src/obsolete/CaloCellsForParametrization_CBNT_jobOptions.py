# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# DetailedCellInfo
from FastCaloSim.FastCaloSimConf import CBNTAA_DetailedCellInfo
the_CBNT_DetailedCellInfo = CBNTAA_DetailedCellInfo()
topSequence.CBNT_AthenaAware += the_CBNT_DetailedCellInfo
the_CBNT_DetailedCellInfo.CalibrationContainers = [ "LArCalibrationHitActive", "TileCalibrationCellHitCnt"]
the_CBNT_DetailedCellInfo.CalibrationContainersInactive = [ "LArCalibrationHitInactive"]

#Write a text file with the geometry information of all calorimeter cells
#the_CBNT_DetailedCellInfo.CellinfoOutfile="outcells.ATLAS-CSC-05-00-00.txt"

from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory
theFastShowerCellBuilderTool=FastCaloSimFactory()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theFastShowerCellBuilderTool
the_CBNT_DetailedCellInfo.FastShowerCellBuilderTool=theFastShowerCellBuilderTool

#Reconfigure truth output to store all particles
#from CBNT_Truth.CBNTAA_TruthCfg import CBNTAA_Truth
#theCBNTAA_Truth.All.Enable = True
