InputDB="sqlite://;schema=freshConstants.db;dbname=COMP200"
InputTag="LARElecCalibPedestalsPedestal-comm-08"

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

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 10000000
svcMgr.IOVDbSvc.GlobalTag="COMCOND-004-01"
conddb.addFolder("LAR","/LAR/BadChannels/BadChannels")


# Pedestal:
conddb.addFolder("","/LAR/ElecCalib/Pedestals/Pedestal<db>"+InputDB+"</db><tag>LARElecCalibPedestalsPedestal-comm-08</tag>")
conddb.addFolder("LAR","/LAR/ElecCalib/Pedestals/Pedestal<key>PedestalRef</key>")
from LArCalibDataQuality.LArCalibDataQualityConf import LArPedestalValidationAlg
thePedestalValidationAlg=LArPedestalValidationAlg()
thePedestalValidationAlg.IgnoreBadChannels=True
thePedestalValidationAlg.ValidationKey="Pedestal"
thePedestalValidationAlg.ReferenceKey="PedestalRef"
thePedestalValidationAlg.MsgLevelForDeviations=INFO
thePedestalValidationAlg.ListOfDevFEBs="pedFebs.txt"
topSequence+=thePedestalValidationAlg



# AutoCorr:
conddb.addFolder("","/LAR/ElecCalib/AutoCorr<db>"+InputDB+"</db><tag>LARElecCalibAutoCorr-comm-08</tag>")
conddb.addFolder("LAR","/LAR/ElecCalib/AutoCorr<key>LArAutoCorrRef</key>")
from LArCalibDataQuality.LArCalibDataQualityConf import LArAutoCorrValidationAlg
theAutoCorrValidationAlg=LArAutoCorrValidationAlg()
theAutoCorrValidationAlg.IgnoreBadChannels=True
theAutoCorrValidationAlg.ValidationKey="LArAutoCorr"
theAutoCorrValidationAlg.ReferenceKey="LArAutoCorrRef"
theAutoCorrValidationAlg.MsgLevelForDeviations=INFO
theAutoCorrValidationAlg.ListOfDevFEBs="ACFebs.txt"
topSequence+=theAutoCorrValidationAlg



# Ramp:
conddb.addFolder("","/LAR/ElecCalib/Ramps/RampLinea<db>"+InputDB+"</db><tag>LARElecCalibRampsRampLinea-comm-08</tag>")
conddb.addFolder("LAR","/LAR/ElecCalib/Ramps/RampLinea<key>LArRampRef</key>")
from LArCalibDataQuality.LArCalibDataQualityConf import LArRampValidationAlg
theRampValidationAlg=LArRampValidationAlg()
theRampValidationAlg.IgnoreBadChannels=True
theRampValidationAlg.ValidationKey="LArRamp"
theRampValidationAlg.ReferenceKey="LArRampRef"
theRampValidationAlg.MsgLevelForDeviations=INFO
theRampValidationAlg.ListOfDevFEBs="rampFebs.txt"
theRampValidationAlg.ThinnedValContainer="ThinRampsVal"
theRampValidationAlg.ThinnedRefContainer="ThinRampsRef"
topSequence+=theRampValidationAlg


from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
ValRamps2Ntuple=LArRamps2Ntuple("ValRamps2Ntuple")
ValRamps2Ntuple.NtupleName = "VALRAMPS"
ValRamps2Ntuple.RawRamp = False
ValRamps2Ntuple.RampKey = "ThinRampsVal"
topSequence+=ValRamps2Ntuple

RefRamps2Ntuple=LArRamps2Ntuple("RefRamps2Ntuple")
RefRamps2Ntuple.NtupleName = "REFRAMPS"
RefRamps2Ntuple.RawRamp = False
RefRamps2Ntuple.RampKey = "ThinRampsRef"
topSequence+=RefRamps2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='RampComparison.root' OPT='NEW'" ]



svcMgr.DetectorStore.Dump=True


