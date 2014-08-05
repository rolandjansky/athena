if not 'doSim' in dir():
	doSim = False
	
if not 'ComputeBeamEnergy' in dir() :
	ComputeBeamEnergy = True

theApp.Dlls += [ "TBRec" ]

from TBCondRunPar.TBCondRunParConf import TBCondRunParTool
ToolSvc += TBCondRunParTool()
include ("TBCondRunPar/TBCondRunParTool.py")
include ("TBCaloGeometry/TBCaloGeometry_H8_joboptions.py")

if ( ComputeBeamEnergy ):
	include ("TBRec/TBBeamEnergyCondDBFolders.py")

#CBNT_Athena.Members += [ "CBNT_TBInfo" ]
#CBNT_TBInfo = Algorithm( "CBNT_TBInfo" )
from TBRec.TBRecConf import CBNTAA_TBInfo
CBNT_TBInfo = CBNTAA_TBInfo("CBNT_TBInfo")

#CBNT_TBInfo.OutputLevel = DEBUG

if ( doSim ) :
	CBNT_TBInfo.DBRead = False
	if not 'BeamEnergy' in dir() :
		BeamEnergy = -999 
	if not 'BeamType' in dir() :
		BeamType = - 999
	CBNT_TBInfo.BeamEnergy = BeamEnergy
	CBNT_TBInfo.BeamType   = BeamType

if ( (not doSim) and ComputeBeamEnergy ) :
	CBNT_TBInfo.DBRead            = True
	CBNT_TBInfo.ComputeBeamEnergy = True
	CBNT_TBInfo.DumpBeamLine      = True

CBNT_AthenaAware += CBNT_TBInfo
