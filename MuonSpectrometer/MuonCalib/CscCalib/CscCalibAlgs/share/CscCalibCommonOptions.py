#**************************************************************
#
#       Read in ByteStream events, and Access CSC RDO
# 
#==============================================================

from AthenaCommon.AppMgr import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-04-00-00')
globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetGeo = 'commis'
globalflags.DataSource.set_Value_and_Lock('data')
  

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")

from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    

from MuonRecExample.MuonRecFlags import muonRecFlags
# Run on CSC data
muonRecFlags.doCSCs.set_Value_and_Lock(True)
        
from RecExConfig.RecFlags import rec
rec.Commissioning.set_Value_and_Lock(True)
rec.oldRecExCommissionConfig.set_Value_and_Lock(False)
rec.oldFlagTopSteering.set_Value_and_Lock(False)
rec.ScopingLevel=4

    
# Detector Initialization 
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Tile_setOff()
DetFlags.LAr_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")


include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import CscROD_Decoder
CscRodDecoder = CscROD_Decoder(name = "CscROD_Decoder",
                               IsCosmics = False,
                               IsOldCosmics = False)
CscRodDecoder.OutputLevel = VERBOSE
ToolSvc += CscRodDecoder

from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name    = "MuonCscRawDataProviderTool",
                                                           Decoder = CscRodDecoder)
ToolSvc += MuonCscRawDataProviderTool
printfunc (MuonCscRawDataProviderTool)

# load the CscRawDataProvider
from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
topSequence += Muon__CscRawDataProvider(name         = "MuonCscRawDataProvider",
                                    ProviderTool = ToolSvc.MuonCscRawDataProviderTool)
printfunc (topSequence.MuonCscRawDataProvider)


#from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
#MdtRodDecoder = MdtROD_Decoder(name = "MdtROD_Decoder")
#ToolSvc += MdtRodDecoder
     
#from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
#MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderTool(name    = "MuonMdtRawDataProviderTool",
#                                                           Decoder = MdtRodDecoder)
#ToolSvc += MuonMdtRawDataProviderTool

#from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
#topSequence += Muon__MdtRawDataProvider(name         = "MuonMdtRawDataProvider",
#                                   ProviderTool = ToolSvc.MuonMdtRawDataProviderTool)


# --- ByteStream
theApp.Dlls   += [ "ByteStreamCnvSvc", "ByteStreamCnvSvcBase"]
# --- Services
theApp.ExtSvc += ["ByteStreamEventStorageInputSvc/ByteStreamInputSvc"]
theApp.ExtSvc += [ "ROBDataProviderSvc/ROBDataProviderSvc" ]
theApp.ExtSvc += [ "EventSelectorByteStream/EventSelector"]
theApp.EvtSel = "EventSelector"

include ( "MuonEventCnvTools/MuonEventCnvTools_jobOptions.py" )
include ( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
#include ("MuonRdoToPrepData/MuonRdoToMuonPrepData_jobOptions.py")

if('inputFiles' not in dir()):
  inputFiles = defInputFiles
  
printfunc ('the input files are: '  )
printfunc (inputFiles)
ServiceMgr.EventSelector.Input = inputFiles

include ("CscCalibTools/CscCalibTool_jobOptions.py")
ToolSvc.CscCalibTool.ReadFromDatabase = True

#--------------------------------------------------------------
# Output files.
#--------------------------------------------------------------
if('outputFile' not in dir()):
  outputFile = 'csc_calib_mon.root'

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["CscCalibMon DATAFILE='" + outputFile + "' OPT='RECREATE'"]

