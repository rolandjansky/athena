###########################################################################
#
# TopOptions to copy a complete set of conditions for a given run to 
# one POOL file and one sqlite file
#
# Walter.Lampl@cern.ch
#
###########################################################################


import shutil, os,sys
from LArCalibProcessing.extractFolderInfo import *

if 'dbname' not in dir():
  dbname="CONDBR2"

if 'sqliteIn' not in dir():
  sqliteIn="freshConstants.db"

if 'sqliteOut' not in dir():
  sqliteOut=sqliteIn+".out"

inputDB="sqlite://;schema="+sqliteIn+";dbname="+dbname
outputDB="sqlite://;schema="+sqliteOut+";dbname="+dbname


if os.environ.has_key("CALIBMERGE_PEEKRUN"):
  peekrun=int(os.environ["CALIBMERGE_PEEKRUN"])
else:
  peekrun=2147483647

print "Peeking input database at run",peekrun

folderInfo=extractFolderInfo(inputDB,runnumber=peekrun)

mergeFoldersRec=["/LAR/ElecCalibOfl/Pedestals/Pedestal",
                 "/LAR/ElecCalibOfl/Ramps/RampLinea",
                 "/LAR/ElecCalibOfl/MphysOverMcal/RTM",
                 "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples1phase",
                 "/LAR/ElecCalibOfl/Shape/RTM/5samples1phase"
                 ]

mergeFoldersCalib=["/LAR/ElecCalibOfl/AutoCorrs/AutoCorr",
                   "/LAR/ElecCalibOfl/AutoCorrs/PhysicsAutoCorr",
                   "/LAR/ElecCalibOfl/CaliPulseParams/RTM",
                   "/LAR/ElecCalibOfl/DetCellParams/RTM",
                   "/LAR/ElecCalibOfl/OFCBin/PhysWaveShifts",
                   #"/LAR/ElecCalibOfl/OFC/CaliWave",
                   #"/LAR/ElecCalibOfl/OFC/CaliWaveXtalkCorr"
                   ]


shutil.copyfile(sqliteIn,sqliteOut)


DuplicateKeyInputPedestal = "" 
DuplicateKeyInputRamp = "" 
DuplicateKeyInputMphysMcali = "" 
DuplicateKeyInputOFC = []
DuplicateKeyInputShape = ""

if not "DuplicateConstants" in dir():
  DuplicateConstants = True

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

theApp.EvtMax = 1
conddb.setGlobalTag("COMCOND-BLKPA-006-01") #For id mapping

svcMgr.PoolSvc.CheckDictionary=False
svcMgr.PoolSvc.SortReplicas=False

runNoForFileName=2147483647

outObjectsRec=[]
outTagsRec=[]

outObjectsCalib=[]
outTagsCalib=[]

sgKeys=dict()
for f in folderInfo:
  fn=f[0]
  type=f[1]
  key=f[2]
  tag=f[3]
  since=f[4]
  until=f[5]
  print "Working on folder",fn,"Tag",tag,"Type",type,"Key",key#,"IOV:",since,"-",until

  if sgKeys.has_key(type):
    if key in sgKeys[type]:
      print "StoreGate key",key,"appeared multiple times for type",type,
      key=key+str(len(sgKeys[type]))
      print "Changed to",key
      pass
    sgKeys[type].append(key)
    pass
  else:
    sgKeys[type]=[key,]
  
  if fn in mergeFoldersRec:
    if since>0 and since<runNoForFileName:
      runNoForFileName=since
    outObjectsRec+=["#".join((type,key,fn))]
    outTagsRec+=[tag]
    conddb.addFolder("",fn+"<tag>"+tag+"</tag><key>"+key+"</key><db>"+inputDB+"</db>")
  elif fn in mergeFoldersCalib:
    if since>0 and since<runNoForFileName:
      runNoForFileName=since
    outObjectsCalib+=["#".join((type,key,fn))]
    outTagsCalib+=[tag]
    conddb.addFolder("",fn+"<tag>"+tag+"</tag><key>"+key+"</key><db>"+inputDB+"</db>")

  #set keys for DuplicateConstatns
  if fn in mergeFoldersRec:
    if fn.find("Pedestal")>0: DuplicateKeyInputPedestal = key
    elif fn.find("Ramp")>0:  DuplicateKeyInputRamp = key
    elif fn.find("MphysOverMcal")>0:  DuplicateKeyInputMphysMcali = key
    elif fn.find("Shape")>0: DuplicateKeyInputShape = key
    elif fn.find("OFC")>0: DuplicateKeyInputOFC.append(key) #Can have more than one OFC folder
    pass

if len(DuplicateKeyInputOFC)==0: DuplicateKeyInputOFC.append("")
  
svcMgr.EventSelector.RunNumber = peekrun

OutputFileRec="LArConditionsRec_"+str(runNoForFileName)+".pool.root"
if os.access(OutputFileRec,os.F_OK):
  print "File",OutputFileRec,"exists already, removing ...."
  os.remove(OutputFileRec)

OutputFileCalib="LArConditionsCalib_"+str(runNoForFileName)+".pool.root"
if os.access(OutputFileCalib,os.F_OK):
  print "File",OutputFileCalib,"exists already, removing ...."
  os.remove(OutputFileCalib)


print svcMgr.IOVDbSvc.Folders

print "============ Reco ============="
print outObjectsRec
print outTagsRec
print "============ Calib ============="
print outObjectsCalib
print outTagsCalib

if len(outObjectsRec)==0 and len(outObjectsCalib)==0:
  print "Nothing to do, exiting"
  sys.exit(0)

from LArCalibTools.LArCalibToolsConf import LArBlockCorrections
topSequence+=LArBlockCorrections()

from LArCalibTools.LArCalibToolsConf import ForceLoadCondObj
topSequence+=ForceLoadCondObj(ObjectList=outObjectsRec+outObjectsCalib)


if DuplicateConstants:
  from LArCalibUtils.LArCalibUtilsConf import LArDuplicateConstants
  lArDuplicateConstants=LArDuplicateConstants()
  lArDuplicateConstants.KeyPedestal   = DuplicateKeyInputPedestal
  lArDuplicateConstants.KeyRamp       = DuplicateKeyInputRamp
  lArDuplicateConstants.KeyMphysMcali = DuplicateKeyInputMphysMcali
  lArDuplicateConstants.KeyOFC        = DuplicateKeyInputOFC[0]
  lArDuplicateConstants.KeyShape      = DuplicateKeyInputShape
  topSequence+=lArDuplicateConstants

  for i in range(1,len(DuplicateKeyInputOFC)):
    algname="LArDuplicateConstant%i"%i
    nextDuplicateConstants=LArDuplicateConstants(algname,
                                                 KeyOFC=DuplicateKeyInputOFC[i])
    topSequence+=nextDuplicateConstants



from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

if len(outObjectsRec):
  theOutputConditionsAlgRec=OutputConditionsAlg("OutputConditionsAlgRec",OutputFileRec,
                                                outObjectsRec,outTagsRec,True)
  theOutputConditionsAlgRec.Run1=runNoForFileName

if len(outObjectsCalib):
  theOutputConditionsAlgCalib=OutputConditionsAlg("OutputConditionsAlgCalib",OutputFileCalib,
                                                  outObjectsCalib,outTagsCalib,True)


svcMgr.IOVDbSvc.dbConnection  = outputDB
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = False

svcMgr.DetectorStore.Dump=True


if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat
