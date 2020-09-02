###########################################################################
#
#  * Read parameters from ascii file into the appropriate conditions object
#  
#  FILE FORMAT:
#  -----------
#  * Each record in the file refers to a channel, which can be identified using
#  * either offline coordinates or online coordinates.
#
#  *------------------------------------------------------------------------------------------
#  * File record format for OFFLINE coordinates (for readout channels only):
#  * det subdet barrel_ec layer region eta phi   parameters...  (using offline channel id)
#  *  > det       = 4 for LAr Calorimeter
#  *  > subdet    = 1(EMB+EMEC) , 2(HEC) , 3(FCAL)
#  *  > barrel_ec = +/-1(barrel) , +/-2(emec outer wheel) , +/-3(emec inner wheel)
#  *  > region,eta,phi: see explanation in CaloIdentifier/LArEM_ID LArHEC_ID and LArFCAL_ID.
#  *  > parameters: values to be set for each channel
#  *------------------------------------------------------------------------------------------
#  * File record format for ONLINE coordinates (for readout and calibration channels):
#  * barrel_ec posneg feedthrough slot channel      parameters...  (using online channel id)
#  *  > barrel_ec   = 0(barrel) , 1(em+had+fcal endcap)
#  *  > posneg      = 0(neg.eta,C) , 1(pos.eta,A)
#  *  > feedthrough: see explanation in LArIdentifier/LArOnlineID.h
#  *  > parameters: values to be set for each channel
#  *------------------------------------------------------------------------------------------
#
###########################################################################
#
# define here the name of data structure to be read and saved to POOL.
# you need to set DataStructure (the name of the transient data class to fill)
# and DetStoreKey (the key to record/retrieve to/from detector store)
#
# Possible data structures are:
#
# - LArCaliPulseParamsComplete
# - LArDetCellParamsComplete
# - LArPhysCaliTdiffComplete
# - LArTdriftComplete
# - LArMphysOverMcalComplete
# - LArRinjComplete
# - LArTshaperComplete
# - LArEMEC_CphiComplete
# - LArEMEC_HValphaComplete
# - LArEMEC_HVbetaComplete
# - LArCableLengthComplete
# - LArCableAttenuationComplete
# - LArCaliPulseParamsVsCalib
#
# DetStoreKey must the same as DataStructure, without 'Complete' or 'VsCalib'
#

#DataStructure = "MyData"        # used by LArReadParamsFromFile and LArParams2Ntuple
#DetStoreKey   = "Key"           # used to extract collections from det.store and save to POOL
#AsciiFileName = "MyAsciiFile"   # input ascii file
#ChannelIdType = "OfflineID"     # file uses 'offline coordinates' (readout only)
#ChannelIdType = "OnlineID"      # file uses 'online coordinates' (readout channels)
#ChannelIdType = "OnlineCalibID" # file uses 'online coordinates' (calibration lines)

# An example for 'Complete' conditions object:
#--------------------------------------------
#DataStructure = "LArDetCellParamsComplete"
#DetStoreKey   = "LArDetCellParams"
#AsciiFileName = "/afs/cern.ch/user/m/mfanti/scratch0/Athena/Parameters/EMB/detcell_atlas_ft0.dat"
#ChannelIdType = "OfflineID"

# An example for 'VsCalib' conditions object:
# ------------------------------------------
#AsciiFileName = "/afs/cern.ch/user/m/mfanti/scratch0/Athena/Parameters/CTB-Barrel/Marco/CaliPulse_averaged.dat"
#DataStructure = "LArCaliPulseParamsVsCalib"
#DetStoreKey   = "LArCaliPulseParams"
#ChannelIdType = "OnlineCalibID"

# the following one is a non-physical test:
# ----------------------------------------
#AsciiFileName = "/afs/cern.ch/user/m/mfanti/scratch0/Athena/Parameters/TestCalibLines/testAllCalibLinesH8.dat"
#DataStructure = "LArCaliPulseParamsVsCalib"
#DetStoreKey   = "LArCaliPulseParams"
#ChannelIdType = "OnlineCalibID"

WritePoolFile = True
WriteRootFile = True

RootFileName = "params.root"
PoolFileName = "params.pool.root"

###########################################################################
#
# Global settings
#
###########################################################################

from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

GlobalFlags.Print()

from AthenaCommon.DetFlags import DetFlags

DetFlags.digitize.em_on()
DetFlags.detdescr.em_setOn()
DetFlags.readRDOBS.em_setOn()
DetFlags.makeRIO.em_setOn()

DetFlags.Print()

LArCondCnvDbServer = 'atlobk02.cern.ch'
DetDescrVersion    = "CTB"
useLArH8CondCool   = True
        
include("LArDetDescr/LArDetDescr_H8_joboptions.py")
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=False


###########################################################################
#
# Reading ascii file...
#
###########################################################################

theApp.Dlls += ["LArCalibTools"]

AlgName = "LArReadParamsFromFile<" + DataStructure + ">/LArReadFile"

theApp.TopAlg += [ AlgName ]
LArReadFile=Algorithm( "LArReadFile" )
LArReadFile.File = AsciiFileName
LArReadFile.ChannelIdType = ChannelIdType
#LArReadFile.OutputLevel = VERBOSE

###########################################################################
#
# Output
#
###########################################################################

if ( WriteRootFile  ) :
	
	theApp.TopAlg += [ "LArParams2Ntuple" ]
	LArParams2Ntuple=Algorithm( "LArParams2Ntuple" )
	LArParams2Ntuple.KeyList += [ DataStructure ]
	#LArParams2Ntuple.DumpAllOnlineChannels = "dumpOnlineChannels_H8.txt" # ascii dump of all channels
	#LArParams2Ntuple.UseAbstractInterface = True    # access data set through abstract interface
	#LArParams2Ntuple.AllChannels2Ntuple = True      # loop through ALL existing online channels
	#LArParams2Ntuple.OutputLevel = VERBOSE
	
	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]

if ( WritePoolFile ) :
        include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
        theApp.Dlls += [ "LArAthenaPoolPoolCnv" ]
        theApp.Dlls += [ "LArCondAthenaPoolPoolCnv" ]

        include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
        ToolSvc.ConditionsAlgStream.OutputFile = PoolFileName
        
	OutputConditionsAlg.ObjectList = [ DataStructure + "#" + DetStoreKey ]
	
###########################################################################

theApp.EvtMax = 1

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

#ToolSvc.LArOnOffIdMap.OutputLevel = VERBOSE
#ToolSvc.LArOnOffIdMapCnv.OutputLevel = VERBOSE

###########################################################################
