include("LArCalibProcessing/LArCalib_Flags.py")

if 'Runnumber' not in dir():
    Runnumber=2147483647

globaltag="LARCALIB-000-02"

if "Objects" not in dir():
    Objects=["RAMP"]#,"CALIWAVE"]

if "sqlite" in dir():
    InputDB="sqlite://;schema="+sqlite+";dbname=CONDBR2"
else:
    InputDB="COOLOFL_LAR/CONDBR2"


if "output" in dir():
    OutputDB="sqlite://;schema="+output+";dbname=CONDBR2"
else:
    if "sqlite" in dir():
        OutputDB=InputDB
    else:
        OutputDB="sqlite://;schema=AdditionalCorrections.db;dbname=CONDBR2"

if 'poolcat' not in dir() and os.access("freshConstants.xml",os.R_OK):
    poolcat="freshConstants.xml"

if 'Partitions' not in dir():
    Partitions=['EMB','EC'] #means all
if 'Gains' not in dir():
    Gains=[0,1,2]

if not "WriteNtuple" in dir():
    WriteNtuple=True


doCW=False
doCWxtalk=False
doRamp=False

for obj in Objects:
  if upper(obj).startswith("RAMP"):
    doRamp=True

  elif upper(obj).startswith("CALIWAVE"):
    for p in Partitions:
        if p.startswith("E"): doCWxtalk=True
        #if (p.startswith("EC") or p.startswith("HEC") or p.startswith("FCAL")): doCW=True
        if (p.startswith("HEC") or p.startswith("FCAL")): doCW=True
    if doCW and doCWxtalk:
        print "ERROR: Can't do CaliWave patching for partitions",Partitions,"in one go."
        sys.exit(-1)

inputFolders=[]
if doRamp:
    inputFolders+=[LArCalib_Flags.LArRampFolder]

if doCW:
    inputFolders+=[ LArCalib_Flags.LArCaliWaveFolder]
elif doCWxtalk:
    inputFolders+=[ LArCalib_Flags.LArCaliWaveFolderXtlk]


from LArConditionsCommon.LArExtendedSubDetGrouping import LArExtendedSubDetGrouping
g=LArExtendedSubDetGrouping()
g.setWithCorr(False) #ingore correction channels
sel=g.getChannelSelection(Partitions,Gains)
ccs="<channelSelection>"+sel+"</channelSelection>"

from LArCalibProcessing.extractFolderInfo import *
folderInfo=extractFolderInfo(InputDB,globaltag,inputFolders,Runnumber,sel)
if folderInfo is None:
    print "Failed to inspect database",InputDB
    sys.exit(-1)

print folderInfo
import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

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
from AtlasGeoModel import SetupRecoGeometry

#Get identifier mapping (needed by LArConditionsContainer)
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>")
conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>")
svcMgr.IOVDbSvc.GlobalTag=globaltag
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = Runnumber

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if 'poolcat' in dir():
    svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat

IOVstart=Runnumber
IOVend=0
OutputObjectSpec=[]
OutputTagSpec=[]

print folderInfo

for f in folderInfo:
  fn=f[0]
  type=f[1]
  key=f[2]
  tag=f[3]
  since=f[4]
  until=f[5]
  print "Working on folder",fn,"IOV:",since,"-",until
  OutputObjectSpec+=["#".join((type,key,fn))]
  OutputTagSpec+=[tag]
  if since<IOVstart: IOVstart=since
  if until>IOVend: IOVend=until
  conddb.addFolder("",fn+"<key>"+key+"Input</key><tag>"+tag+"</tag><db>"+InputDB+"</db>"+ccs)


print OutputObjectSpec


i=0
outputFileName="AdditionalCorrections_%i_%i.pool.root" % (IOVstart,i)
while (os.access(outputFileName,os.F_OK)):
    i=i+1     
    outputFileName="AdditionalCorrections_%i_%i.pool.root" % (IOVstart,i)

outputRootFileName="AdditionalCorrections_%i_%i.root" % (IOVstart,i)
while (os.access(outputRootFileName,os.F_OK)):
    i=i+1     
    outputRootFileName="AdditionalCorrections_%i_%i.root" % (IOVstart,i)


from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
#theLArBadChannelTool.OutputLevel=DEBUG
ToolSvc+=theLArBadChannelTool

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
theLArRCBMasker.DoMasking=True
theLArRCBMasker.ProblemsToMask=[
    "deadCalib","deadReadout","deadPhys","almostDead","short",
    "highNoiseHG","highNoiseMG","highNoiseLG"
    ]
ToolSvc+=theLArRCBMasker


if doRamp:
  #Algo:
  from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArRampComplete_
  theLArRampPatcher=LArCalibPatchingAlg_LArRampComplete_("LArRampPatcher")
  theLArRampPatcher.ContainerKey="LArRampInput"
  theLArRampPatcher.NewContainerKey="LArRamp"
  theLArRampPatcher.PatchMethod="PhiAverage"
  #theLArRampPatcher.OutputLevel=DEBUG
  theLArRampPatcher.MaskingTool=theLArRCBMasker
  topSequence+=theLArRampPatcher

  if doCW or doCWxtalk:
    from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArCaliWaveContainer_
    theLArCaliWavePatcher=LArCalibPatchingAlg_LArCaliWaveContainer_("LArCaliWavePatcher")
    theLArCaliWavePatcher.ContainerKey="LArCaliWaveInput"
    theLArCaliWavePatcher.NewContainerKey="LArCaliWave"
    theLArCaliWavePatcher.PatchMethod="PhiAverage"
    #theLArCaliWavePatcher.OutputLevel=DEBUG
    theLArCaliWavePatcher.MaskingTool=theLArRCBMasker
    topSequence+=theLArCaliWavePatcher
  

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg", outputFileName,
                                           OutputObjectSpec,OutputTagSpec,True)

theOutputConditionsAlg.Run1 = IOVstart
theOutputConditionsAlg.Run2 = IOVend
svcMgr.IOVDbSvc.dbConnection  = OutputDB

from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = False


if WriteNtuple:
    from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
    LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
    LArRamps2Ntuple.NtupleName = "RAMPS"
    LArRamps2Ntuple.RawRamp = False
    LArRamps2Ntuple.ApplyCorr = True
    





svcMgr.DetectorStore.Dump=True

ToolSvc.LArBadChanTool.OutputLevel=DEBUG
svcMgr.IOVDbSvc.OutputLevel=DEBUG
