##########################################################################################################
##													##
##													##
##     Script to read .db and .pool files and create .txt with the StatusHT contants                    ##
##													##
##########################################################################################################


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock("atlas")
globalflags.DataSource.set_Value_and_Lock("geant4")
#globalflags.DataSource.set_Value_and_Lock("data")


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

from IOVDbSvc.CondDB import conddb

# These addFolder lines need to be outcommented if you want to dump your own sqlite db to text file.
# Dead/Noisy Straw Lists
if not conddb.folderRequested('/TRT/Cond/Status'):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')
if not conddb.folderRequested('/TRT/Cond/StatusPermanent'):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')
# Argon straw list
if not conddb.folderRequested('/TRT/Cond/StatusHT'):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')

# Alignment folders (not dynamic)
if ((not DetFlags.simulate.SCT_on()) or (not DetFlags.simulate.pixel_on())) or (DetFlags.overlay.SCT_on() or DetFlags.overlay.pixel_on()):
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer")
else:
    conddb.addFolderSplitOnline("INDET","/Indet/Onl/Align","/Indet/Align")

    if (not DetFlags.simulate.TRT_on()) or DetFlags.overlay.TRT_on():
       conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align",className="AlignableTransformContainer")
    else:
       conddb.addFolderSplitOnline("TRT","/TRT/Onl/Align","/TRT/Align")
# TRT Lvl 3 alignment
conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/DX","/TRT/Calib/DX")



from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


include("RegistrationServices/RegistrationServices_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#conddb.setGlobalTag("CONDBR2-BLKPA-2018-14")
conddb.setGlobalTag("OFLCOND-MC16-SDR-27")   # choose here a global tag
#svcMgr.IOVDbSvc.forceRunNumber=300000       # choose here a number within the IoV of interest

svcMgr.IOVSvc.preLoadData 	   = True
svcMgr.MessageSvc.OutputLevel      = INFO


from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawStatusWrite
TSW=TRTStrawStatusWrite(name="TSW")
TSW.StatusInputFileHT		=""
topSequence +=TSW

theApp.EvtMax = 1

# These addFolder lines need to be uncommented if you want to dump your own sqlite db to text file.
#conddb.blockFolder("/TRT/Cond/StatusHT")
#conddb.addFolder("","<dbConnection>sqlite://;schema=TRT_StrawStatusHT_RUN2.db;dbname=CONDBR2</dbConnection>/TRT/Cond/StatusHT<tag>TrtStrawStatusHT-RUN2-UPD4-03-00</tag>",force=True)
#conddb.addFolder("","<dbConnection>sqlite://;schema=TRT_Cond_StatusHT.db;dbname=OFLP200</dbConnection>/TRT/Cond/StatusHT<tag>TrtStrawStatusHT-MC-run2-scenario5_00-03</tag>",force=True)
# or in case you have a sqLite file
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=CONDBR2</dbConnection>/TRT/Cond/StatusHT<tag>TrtStrawStatusHT-MC-run2-scenario5_00-03</tag>",force=True)




