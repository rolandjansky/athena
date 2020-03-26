#----
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
from AthenaCommon.AppMgr import ServiceMgr
import AthenaCommon.AtlasUnixStandardJob

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()



if not 'NSWsetup' in dir():
    NSWsetup=True


if not 'ConditionsTag' in dir():
    ConditionsTag = "OFLCOND-DR-BS900-A1-01"
    printfunc ("MuonGeoModelTest/MuonHitRelocation Setting now ConditionsTag to ", ConditionsTag)
else:
    printfunc ("MuonGeoModelTest/MuonHitRelocation ConditionsTag already set to ", ConditionsTag)
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.ConditionsTag = ConditionsTag


if NSWsetup:
    include ("MuonGeoModelTest/NSWGeoSetup.py")
######################################################################## end setup GeoModel

if not 'inputHits' in dir():
    inputHits="atlasG4.NSWgeantinohits.pool.root"
    printfunc ("inputHits not known yet - setting to ", inputHits )
else:
    printfunc ("inputHits already known and equal to ", inputHits )

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ inputHits ]

theApp.EvtMax = 1000000000

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 9999999

theApp.HistogramPersistency = "ROOT"
ServiceMgr.NTupleSvc.Output = [ "FILE DATAFILE='Relocated.atlasG4.NSWgeantinohits.root' OPT='NEW'" ]

printfunc ("Reading alignment constants from DB")
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC',className='CondAttrListCollection')

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
condSequence+=MuonAlignmentCondAlg('MuonAlignmentCondAlg')
MuonAlignmentCondAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                       "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                       "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                       "/MUONALIGN/TGC/SIDEA",
                                       "/MUONALIGN/TGC/SIDEC"]

#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonHitRelocation
MuonHitRelocation = MuonHitRelocation()
MuonHitRelocation.OutputLevel=VERBOSE

printfunc (MuonHitRelocation)

######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += MuonHitRelocation

printfunc (" ")
printfunc ("List of all Dlls")
printfunc (theApp.Dlls)
printfunc (" ")
printfunc ("List of all Ext services")
printfunc (theApp.ExtSvc)
printfunc (" ")
printfunc ("List of all top algorithms")
printfunc (theApp.TopAlg)

#printfunc ("Print here Top Sequence" )
#printfunc (topSequence)
#printfunc ("Print here Service Manager" )
#printfunc (ServiceMgr)


