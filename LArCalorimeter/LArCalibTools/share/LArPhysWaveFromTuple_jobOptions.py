###########################################################################
#
# LArPhysWaveFromTuple_jobOptions.py
# author: Alan Robinson <fbfree@interchange.ubc.ca>
# Converts formatted Root NTuple into a LArPhysWave ready for the 
# conditions DB.  Created for FCal wave shapes.
#
###########################################################################

writePOOLfile       = True
PhysWavePOOLFileOUT = "~/scratch0/poolFiles/FCalWave.pool.root"

writeNTuple         = True
PhysWaveNTFile      = "~/scratch0/rootFiles/FCalCheck.root"

InFile              = "~/scratch0/FCalWave.root"

NPoints             = 189
NSkipPoints         = 23   # Shift peak to the 3rd sample if it is at 96ns

###########################################################################
theApp.EvtMax = 1

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print = False

include( "AthenaCommon/Atlas.UnixStandardJob.py" )

# the following works for release 13 but nor for rel 12
#DetDescrVersion = "ATLAS-CSC-01-01-00"
#include( "RecExCond/RecExCommon_flags.py" )
#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.detdescr.Muon_setOff()
#include ("RecExCond/AllDet_detDescr.py")

# use this for release 12
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )


theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LArPhysWaveFromTuple" ]
LArPhysWaveFromTuple = Algorithm( "LArPhysWaveFromTuple" )
LArPhysWaveFromTuple.NPoints = NPoints
LArPhysWaveFromTuple.SkipPoints = NSkipPoints
LArPhysWaveFromTuple.PrefixPoints = 0
LArPhysWaveFromTuple.deltaT  = 1.
LArPhysWaveFromTuple.LArWaveFlag = 3 # Code for "Master Waveform Normalized"
LArPhysWaveFromTuple.FileNames = [ InFile ]
LArPhysWaveFromTuple.StoreKey = "LArPhysWave"
LArPhysWaveFromTuple.GroupingType = "ExtendedFeedThrough"


AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
###########################################################################

if ( writePOOLfile ) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
        include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
        include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
	
	include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
	ToolSvc.ConditionsAlgStream.OutputFile = PhysWavePOOLFileOUT
	
	OutputConditionsAlg.ObjectList  = [ "LArPhysWaveContainer#LArPhysWave#/LAR/ElecCalib/PhysWave" ]

if ( writeNTuple ) :
	theApp.Dlls += ["LArCalibTools"]
	theApp.TopAlg += [ "LArWaves2Ntuple/LArPhysWaves2Ntuple"]
	LArPhysWaves2Ntuple = Algorithm( "LArPhysWaves2Ntuple" )
	LArPhysWaves2Ntuple.NtupleName = "outfit"
	LArPhysWaves2Ntuple.ReadCaliWave = False
	LArPhysWaves2Ntuple.KeyList = [ "LArPhysWave" ]
	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+PhysWaveNTFile+"' OPT='NEW'" ]

