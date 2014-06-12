#InputDB="sqlite://;schema=freshConstants.db;dbname=COMP200"
InputDB="sqlite://;schema=myDB200.db;dbname=COMP200"
#InputDB="COOLONL_LAR/COMP200"

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit 

#Get identifier mapping (needed by LArConditionsContainer)
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

# Read all channels .....
ChannelSelection = "<channelSelection>0,3:238</channelSelection>"
#######################
#for now we have only high gain
#     ChannelSelection="0" # high gain correction channel
#for all high gain channels in EMB/EMEC/FCAL/HEC/PS 
#     ChannelSelection = "0,3:238" #All high-gain cool channels
#if haveEMB:
#     ChannelSelection+=",3:66,117:180" # Barrel accordion and Presampler, high gain only
#if haveEMECA:
#     ChannelSelection+=",92:116,206,230,232,234,236,238" #Endcap A, HEC, FCAL, Presampler and Inner Wheel
#if haveEMECC:
#     ChannelSelection+=",67:91,181:205,231,233,235,237" #Endcap C, HEC, FCAL, Presampler and Inner Wheel

theApp.EvtMax = 100
svcMgr.EventSelector.RunNumber = 10000000
from IOVDbSvc.CondDB import conddb
svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1C-000-00"
conddb.addFolder("LAR","/LAR/BadChannels/BadChannels")
conddb.addFolder("LAR","/LAR/BadChannels/MissingFEBs")

##### if InputDB=sqlite, then Read from local Database (sqlite)
conddb.addFolder("","/LAR/ElecCalib/Pedestals/Pedestal<db>"+InputDB+"</db><tag>LARElecCalibPedestalsPedestal-UPD3-00</tag>"+ChannelSelection)
#### the Caliwave is in different forder for EMB/EMEC and FCAL/HEC
#### if EMB/EMEC
conddb.addFolder("","/LAR/ElecCalib/CaliWaves/CaliWaveXtalkCorr<db>"+InputDB+"</db><tag>LARElecCalibCaliWavesCaliWaveXtalkCorr-UPD3-00</tag>"+ChannelSelection)
#### if HEC/FCAL
#conddb.addFolder("","/LAR/ElecCalib/CaliWaves/CaliWave<db>"+InputDB+"</db><tag>LARElecCalibCaliWavesCaliWave-UPD3-00</tag>"+ChannelSelection)


from LArCalibDataQuality.LArCalibDataQualityConf import LArBadChannelHunter
theLArBadChannelHunter=LArBadChannelHunter()
theLArBadChannelHunter.PedestalKey="Pedestal"
#theLArBadChannelHunter.RampKey="LArRamp"
theLArBadChannelHunter.CaliWaveKey="LArCaliWave"
theLArBadChannelHunter.AverageType="PHI" # "FEB" Not working well
theLArBadChannelHunter.RecalcPer=0.03 # negative value will switch off recalculation
theLArBadChannelHunter.OutFileName="bad.txt"
theLArBadChannelHunter.OutOnlyNew=False;

theLArBadChannelHunter.CutType="PER" # SIG
theLArBadChannelHunter.LowNoiseThresholdHG=12.0; #6.0 (for EMB/EMEC)
#theLArBadChannelHunter.LowNoiseThresholdHG=10.; #15.0 (for FCAL)
#theLArBadChannelHunter.LowNoiseThresholdHG=20.; #15.0 (for HEC)

theLArBadChannelHunter.HighNoiseThresholdHG=80.0; #100.0
theLArBadChannelHunter.DeadThresholdAmpHG=80.0; #60.0
theLArBadChannelHunter.DeadThresholdWidHG=100.0; #100.0

theLArBadChannelHunter.DistortThresholdAmpHG=7.5; #4.5 (for EMB/EMEC)
#theLArBadChannelHunter.DistortThresholdAmpHG=10.; #4.5 (for FCAL/HEC)

theLArBadChannelHunter.DistortThresholdWidHG=8.0; #4.5 (for EMB/EMEC)
#theLArBadChannelHunter.DistortThresholdWidHG=10.; #4.5 (for FCAL/HEC)

theLArBadChannelHunter.DistortThresholdTmaxHG=10.0; #5.0


theLArBadChannelHunter.OutputLevel=2
topSequence+=theLArBadChannelHunter


svcMgr.DetectorStore.Dump=False


