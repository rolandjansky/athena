###########################################################################
#    Author:      Miroslav Nozicka <nozicka@mail.desy.de>                                      
#    
#    Usage: athena.py -c "[flags]" testHLT_standaloneDB.py
#    
###########################################################################
from TrigConfOffline.HLTConfOffline import HLTConfOffline
hltConfOffline = HLTConfOffline()

# Process the commands
## @var dbSMKeyFlag
#  Either SMKey or Run number must be specified in the command option
dbSMKeyFlag = False
try :
  hltConfOffline.SMKey = DBSMKey
  dbSMKeyFlag = True
except NameError:
  pass

runNumberFlag = False
try :
  hltConfOffline.RunNumber = Run
  runNumberFlag = True
except NameError:
  pass

# Raise the Exception if neither SMKey nor Run number specified
if not (runNumberFlag or dbSMKeyFlag) :
  raise Exception('Please specify Run or SMKey')

# Process the remaining command options
try :
  hltConfOffline.OutputLevel = OutputLevel
except NameError:
  pass
  
try :
  hltConfOffline.setupSource = DBConn
except NameError:
  pass

try :
  hltConfOffline.dbType = DBType.lower()
except NameError:
  pass

try :
  hltConfOffline.dbHost = DBServer
except NameError:
  pass

try :
  hltConfOffline.dbUser = DBUser
except NameError:
  pass

try :
  hltConfOffline.dbPasswd = DBPasswd
except NameError:
  pass

try :
  hltConfOffline.dbName = DBName
except NameError:
  pass

try :
  hltConfOffline.rulesSource = Rules
except NameError:
  pass

try :
  hltConfOffline.HLTPrescaleKey = DBHLTPSKey
except NameError:
  pass

try :
  hltConfOffline.LVL1PrescaleKey = DBLVL1PSKey
except NameError:
  pass

try :
  hltConfOffline.Level = Instance
except NameError:
  pass

# Load the trigger setup
hltConfOffline.load()

# Set Maximum number of events
try:
  theApp.EvtMax = EvtMax
except NameError:
  pass

# Set input data file
try:
  inputdata = DataFile
  if isinstance(inputdata, str) or isinstance(inputdata,unicode) :
    inputdatalist = []
    inputdatalist.append(inputdata)
    inputdata=inputdatalist
  
  svcMgr = theApp.serviceMgr()
  svcMgr.ByteStreamInputSvc.FullFileName = inputdata
except NameError:
  pass
  
# Set the output ByteStreamData
try:
  outputData = BSOutput
  
  svcMgr = theApp.serviceMgr()
  from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
  svcMgr += ByteStreamEventStorageOutputSvc()
  # Properties
  ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc 
  ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamEventStorageOutputSvc"
  ByteStreamCnvSvc.InitCnvs += ["EventInfo"]
  
  # OutputStream
  from AthenaServices.AthenaServicesConf import AthenaOutputStream
  oStream = AthenaOutputStream(
      "StreamBS",
      EvtConversionSvc = "ByteStreamCnvSvc",
      OutputFile = "ByteStreamEventStorageOutputSvc"
      )
  theApp.addOutputStream( oStream )
  theApp.OutStreamType ="AthenaOutputStream";
  
  # Define the output as follows:
  outputDir = "./"
  if isinstance(outputData,str) :
    if '/' in outputData :
      outputDir = outputData[:outputData.rfind('/')]
      
  run = 0
  lb = 0
  if runNumberFlag :
    run = Run
    if isinstance(Run, str) :
      run = Run.split('/')[0]
      if len(Run.split('/') >1):
        lb = Run.split('/')[1]
      
  ByteStreamEventStorageOutputSvc = svcMgr.ByteStreamEventStorageOutputSvc
  ByteStreamEventStorageOutputSvc.OutputDirectory = outputDir
  ByteStreamEventStorageOutputSvc.AppName = "TrigConfOffline"
  
  StreamBS = AthenaOutputStream("StreamBS", EvtConversionSvc = "ByteStreamCnvSvc")
  StreamBS.ForceRead=True
  
  for container in svcMgr.ByteStreamAddressProviderSvc.TypeNames :
    namealias=container.split('/')
    cont = '%s#' % (namealias[0])
    if len(namealias)==2 :
      cont += namealias[1]
    else:
      cont += '*'
    StreamBS.ItemList += [cont]

except NameError:
  pass




