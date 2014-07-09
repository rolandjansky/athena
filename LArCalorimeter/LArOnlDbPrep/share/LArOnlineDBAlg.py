###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber=0x7FFFFFFE # Largest possible run-number
ServiceMgr.EventSelector.EventsPerRun=100000
ServiceMgr.EventSelector.FirstEvent=1


from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_commis()

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-08-00-00'

#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()


from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit
import DetDescrCnvSvc.DetStoreConfig
svcMgr.DetDescrCnvSvc.IdDictFromRDB = True 
#from AtlasGeoModel import GeoModelInitStandard
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
svcMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import LArGM
#from AtlasGeoModel import TileGM

from AtlasGeoModel import SetupRecoGeometry
#svcMgr.GeoModelSvc.DetectorTools["LArDetectorToolNV"].GeometryConfig = "RECO"
#svcMgr.GeoModelSvc.DetectorTools[ "LArDetectorToolNV" ].BuildEndcap=False # Did not help a lot

include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
 

from LArConditionsCommon.LArExtendedFTGrouping import LArExtendedFTGrouping
theLArExtendedFTGrouping=LArExtendedFTGrouping()
theLArExtendedFTGrouping.setWithCorr(True)
sel=theLArExtendedFTGrouping.getChannelSelectionAllGains(["EMBC","EMBCPS"])

print "COOL channel selection:",sel

selection="<channelSelection>"+sel+"</channelSelection>" 
db="<db>COOLONL_LAR/COMP200</db>"
#db="<db>oracle://ATLR;schema=ATLAS_COOLONL_LAR;dbname=COMP200;user=ATLAS_COOL_READER;password=COOLRED4PRO</db>"
svcMgr.PoolSvc.ReadCatalog += ["prfile:poolcond/PoolCat_diskbuffer.xml"]

ServiceMgr.IOVDbSvc.GlobalTag="COMCOND-ES1P-002-00"


#conddb.addFolder("","/LAR/BadChannels/BadChannels"+db)
#conddb.addFolder("","/LAR/BadChannels/MissingFEBs"+db)
conddb.addFolder("","/LAR/Configuration/DSPThreshold/Thresholds"+db)

conddb.addFolder("","/LAR/ElecCalibOnl/Pedestal"+selection+db)
conddb.addFolder("","/LAR/ElecCalibOnl/Ramp"+selection+db)
conddb.addFolder("","/LAR/ElecCalibOnl/MphysOverMcal"+selection+db)
conddb.addFolder("","/LAR/ElecCalibOnl/OFC"+selection+db)
conddb.addFolder("","/LAR/ElecCalibOnl/Shape"+selection+db)
conddb.addFolder("","/LAR/ElecCalibOnl/DAC2uA"+db)
conddb.addFolder("","/LAR/ElecCalibOnl/uA2MeV"+db)
conddb.addFolder("","/LAR/ElecCalibOnl/HVScaleCorr"+selection+db)

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel      = WARNING #INFO #ERROR
#MessageSvc.debugLimit       = 100000
#MessageSvc.infoLimit        = 100000
MessageSvc.warningLimit     = 5000

theApp.EvtMax = 1


def getFebs():
  return [
    939524096,
    939556864,
    939589632,
    939622400,
    939655168,
    939687936,
    939720704,
    939753472,
    939786240,
    939819008,
    939851776,
    939884544,
    939917312,
    939950080,
    940048384,
    940081152,
    940113920,
    940146688,
    940179456,
    940212224,
    940244992,
    940277760,
    940310528,
    940343296,
    940376064,
    940408832,
    940441600,
    940474368,
    940572672,
    940605440,
    940638208,
    940670976,
    940703744,
    940736512,
    940769280,
    940802048,
    940834816,
    940867584,
    940900352,
    940933120,
    940965888,
    940998656,
    941096960,
    941129728,
    941162496,
    941195264,
    941228032,
    941260800,
    941293568,
    941326336,
    941359104,
    941391872,
    941424640,
    941457408,
    941490176,
    941522944,
    941621248,
    941654016,
    941686784,
    941719552,
    941752320,
    941785088,
    941817856,
    941850624,
    941883392,
    941916160,
    941948928,
    941981696,
    942014464,
    942047232,
    942145536,
    942178304,
    942211072,
    942243840,
    942276608,
    942309376,
    942342144,
    942374912,
    942407680,
    942440448,
    942473216,
    942505984,
    942538752,
    942571520,
    942669824,
    942702592,
    942735360,
    942768128,
    942800896,
    942833664,
    942866432,
    942899200,
    942931968,
    942964736,
    942997504,
    943030272,
    943063040,
    943095808,
    943194112,
    943226880,
    943259648,
    943292416,
    943325184,
    943357952,
    943390720,
    943423488,
    943456256,
    943489024,
    943521792,
    943554560,
    943587328,
    943620096,
    943718400,
    943751168,
    943783936,
    943816704,
    943849472,
    943882240,
    943915008,
    943947776,
    943980544,
    944013312,
    944046080,
    944078848,
    944111616,
    944144384,
    944242688,
    944275456,
    944308224,
    944340992,
    944373760,
    944406528,
    944439296,
    944472064,
    944504832,
    944537600,
    944570368,
    944603136,
    944635904,
    944668672,
    944766976,
    944799744,
    944832512,
    944865280,
    944898048,
    944930816,
    944963584,
    944996352,
    945029120,
    945061888,
    945094656,
    945127424,
    945160192,
    945192960,
    945291264,
    945324032,
    945356800,
    945389568,
    945422336,
    945455104,
    945487872,
    945520640,
    945553408,
    945586176,
    945618944,
    945651712,
    945684480,
    945717248,
    945815552,
    945848320,
    945881088,
    945913856,
    945946624,
    945979392,
    946012160,
    946044928,
    946077696,
    946110464,
    946143232,
    946176000,
    946208768,
    946241536,
    946339840,
    946372608,
    946405376,
    946438144,
    946470912,
    946503680,
    946536448,
    946569216,
    946601984,
    946634752,
    946667520,
    946700288,
    946733056,
    946765824,
    946864128,
    946896896,
    946929664,
    946962432,
    946995200,
    947027968,
    947060736,
    947093504,
    947126272,
    947159040,
    947191808,
    947224576,
    947257344,
    947290112,
    947388416,
    947421184,
    947453952,
    947486720,
    947519488,
    947552256,
    947585024,
    947617792,
    947650560,
    947683328,
    947716096,
    947748864,
    947781632,
    947814400,
    947912704,
    947945472,
    947978240,
    948011008,
    948043776,
    948076544,
    948109312,
    948142080,
    948174848,
    948207616,
    948240384,
    948273152,
    948305920,
    948338688,
    948436992,
    948469760,
    948502528,
    948535296,
    948568064,
    948600832,
    948633600,
    948666368,
    948699136,
    948731904,
    948764672,
    948797440,
    948830208,
    948862976,
    948961280,
    948994048,
    949026816,
    949059584,
    949092352,
    949125120,
    949157888,
    949190656,
    949223424,
    949256192,
    949288960,
    949321728,
    949354496,
    949387264,
    949485568,
    949518336,
    949551104,
    949583872,
    949616640,
    949649408,
    949682176,
    949714944,
    949747712,
    949780480,
    949813248,
    949846016,
    949878784,
    949911552,
    950009856,
    950042624,
    950075392,
    950108160,
    950140928,
    950173696,
    950206464,
    950239232,
    950272000,
    950304768,
    950337536,
    950370304,
    950403072,
    950435840,
    950534144,
    950566912,
    950599680,
    950632448,
    950665216,
    950697984,
    950730752,
    950763520,
    950796288,
    950829056,
    950861824,
    950894592,
    950927360,
    950960128,
    951058432,
    951091200,
    951123968,
    951156736,
    951189504,
    951222272,
    951255040,
    951287808,
    951320576,
    951353344,
    951386112,
    951418880,
    951451648,
    951484416,
    951582720,
    951615488,
    951648256,
    951681024,
    951713792,
    951746560,
    951779328,
    951812096,
    951844864,
    951877632,
    951910400,
    951943168,
    951975936,
    952008704,
    952107008,
    952139776,
    952172544,
    952205312,
    952238080,
    952270848,
    952303616,
    952336384,
    952369152,
    952401920,
    952434688,
    952467456,
    952500224,
    952532992,
    952631296,
    952664064,
    952696832,
    952729600,
    952762368,
    952795136,
    952827904,
    952860672,
    952893440,
    952926208,
    952958976,
    952991744,
    953024512,
    953057280,
    953155584,
    953188352,
    953221120,
    953253888,
    953286656,
    953319424,
    953352192,
    953384960,
    953417728,
    953450496,
    953483264,
    953516032,
    953548800,
    953581568,
    953679872,
    953712640,
    953745408,
    953778176,
    953810944,
    953843712,
    953876480,
    953909248,
    953942016,
    953974784,
    954007552,
    954040320,
    954073088,
    954105856,
    954204160,
    954236928,
    954269696,
    954302464,
    954335232,
    954368000,
    954400768,
    954433536,
    954466304,
    954499072,
    954531840,
    954564608,
    954597376,
    954630144,
    954728448,
    954761216,
    954793984,
    954826752,
    954859520,
    954892288,
    954925056,
    954957824,
    954990592,
    955023360,
    955056128,
    955088896,
    955121664,
    955154432,
    955252736,
    955285504,
    955318272,
    955351040,
    955383808,
    955416576,
    955449344,
    955482112,
    955514880,
    955547648,
    955580416,
    955613184,
    955645952,
    955678720,
    955777024,
    955809792,
    955842560,
    955875328,
    955908096,
    955940864,
    955973632,
    956006400,
    956039168,
    956071936,
    956104704,
    956137472,
    956170240,
    956203008,
    ]


myFebList=getFebs()

#febs=open("BarCFTs.txt","r")
#for l in febs:
#    myFebList+=[int(l),]
#    print "    "+l[:-1]+","
#febs.close()
#print myFebList

from LArOnlDbPrep.LArOnlDbPrepConf import LArOnlDbPrepAlg 
theLArOnlDbPrepAlg=LArOnlDbPrepAlg()
theLArOnlDbPrepAlg.febList=myFebList

job+=theLArOnlDbPrepAlg

#Possible futher speed improvment:
# *) Don't load material description (ask Vakho how to do this)
# *) Try only one database (ask Richard how to do this)
# *) Minimize the number of POOL file catalogs, avoid writing default catalog
# *) Move to "smaller" project, AtlasConditions could be enough? No patching via AltasTier0..
