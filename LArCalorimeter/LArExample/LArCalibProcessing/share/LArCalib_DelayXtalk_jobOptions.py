from future import standard_library
standard_library.install_aliases()
import subprocess


###########################################################################
#
# Crosstalk jobOptions
# Reconstruct waveshapes for non pulsed channels (LArCaliWaveBuilderXtalk)
# and compute various types of crosstalk (LArWaveXtalk)
#
# Should be run for each subdetector : BarrelA, BarrelC, EndcapA, EndcapC
# and for each calibration patterns :
#  - EM Barrel : StripSingle[1-4] / MiddleSingle[1-8] / BackSingle[1-4]
#  - EM Endcap : StripSingle[1-4] / MiddleSingle[1-4] / BackSingle[1-4]
#                and MiddleSingle[1-4] / BackSingle[1-4] for inner wheel
# For EM endcap special crates, set SpecialCrate to True. You can then
# choose the region with SpecialRegion = OuterWheel / InnerWheel.
#
# FeedthroughPul is used to find the relevant calibration pattern. Empty
# ntuples/histograms may be produced with badly set FeedthroughPul, or if
# there are bad channels or disconnected FEBs in FeedthroughPul.
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

#######################################################
#       Crosstalk parameters
#######################################################

if not 'Type' in dir():
   Type = "DelayXtalk"

## Calibration pattern
## StripSingle[1-4] / MiddleSingle[1-8] / BackSingle[1-4]
if not 'CalibPattern' in dir():
   CalibPattern = "MiddleSingle1"

## EMEC Special Crates
## if SpecialCrate=True, specify SpecialRegion
if not 'SpecialCrate' in dir():
   SpecialCrate = False         	

if not 'SpecialRegion' in dir():
   SpecialRegion = "OuterWheel" # OuterWheel / InnerWheel

## FT used to check calibration pattern
## ensure there are no bad channels / disconnected FEBs
if not 'FeedthroughPul' in dir():
   FeedthroughPul = 1


#######################################################
#       Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "Barrel"            # Barrel / Endcap

if not 'Side' in dir() :
   Side = "A"                   # A / C

if not 'RunNumberList' in dir():
   RunNumberList = [ '00157333' ]
   
if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Delay"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2010/"+RunNumberList[0]
   
if not 'Partition' in dir():
   if (SubDet=='Endcap'):
      Partition = "EB-EMEC"
   else:
      Partition = "EB-EMB"

if not 'doCaliWaveSelector' in dir():
   doCaliWaveSelector = False      

if not 'FullFileName' in dir():
   if (int(RunNumberList[0]))<99800 :
      Trigger = "*"+Partition
   else :
      Trigger = "calibration_LArElec-Delay"+".*"+Partition+Side
   
   FullFileName = []
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,Trigger)

if not 'GainList' in dir():
   GainList = [ "HIGH", "MEDIUM", "LOW" ]

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"

if not 'ChannelSelection' in dir():
   # read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"

if not 'runAccumulator' in dir(): 
   runAccumulator = False # averaged mode

from string import *
def DBConnectionFile(sqlitefile):  
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

#######################################################
#      Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = True

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = False  

if not 'online' in dir():
   online = False
   
#######################################################
#      Delay properties
#######################################################

if not "ADCSaturation" in dir():
   ADCsaturation  = 4095 # Set to 0 if you want to keep saturating pulses otherwise 4095
   
if not 'SaveDerivedInfo' in dir():
   SaveDerivedInfo = True

if not 'SaveJitter' in dir():
   SaveJitter = True
    
# CutOnSample: consider only the N=CutOnSample first samples to reduce needed memory (0 = no cut)
if not 'CutOnSample' in dir():
   CutOnSample = 0
   
#######################################################
#      Delay output name
#######################################################

if SpecialCrate:
   XtalkTag = CalibPattern +"_FT"+str(FeedthroughPul) + "_SpecialCrates" + "_" + SpecialRegion
else:
   XtalkTag = CalibPattern +"_FT"+str(FeedthroughPul)

if not 'OutputTag' in dir():
   OutputTag = XtalkTag
else:
   OutputTag = XtalkTag + "_" + OutputTag

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = int(RunNumberList[0])
   
if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"

## Pedestal
   
if not 'ReadPedFromCOOL' in dir():
   ReadPedFromCOOL = True
   
if not 'PedRunNumber' in dir():
   PedRunNumber = "1005020_1005021_1005022"
   
if not 'InputPedPoolFileDir' in dir():
   InputPedPoolFileDir  = subprocess.getoutput("pwd")
   
if not 'InputPedPoolFileName' in dir():
   InputPedPoolFileName = "LArPedestal_" + str(PedRunNumber)+"_"+Partition.replace("*","") + ".pool.root"

## Output

if not 'LArCalibFolderOutputTag' in dir():
   LArCalibFolderOutputTag = LArCalib_Flags.tagSuffix    
   
if not 'OutputCaliWaveRootFileDir' in dir():
   OutputCaliWaveRootFileDir  = subprocess.getoutput("pwd")
   
if not 'OutputCaliWavePoolFileDir' in dir():
   OutputCaliWavePoolFileDir  = subprocess.getoutput("pwd")

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)      

if not 'KeyOutput' in dir():  
   KeyOutput = "LArCaliWave" # Key of LArCaliWaveContainer saved in Pool file
   
if not 'BaseFileName' in dir():
   BaseFileName = Type

for RunNumber in RunNumberList :
   BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")+Side

BaseFileName = BaseFileName + "_" + OutputTag

if not 'OutputCaliWaveRootFileName' in dir():
   OutputCaliWaveRootFileName = BaseFileName+".root"
   
if not 'OutputCaliWavePoolFileName' in dir():
   OutputCaliWavePoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecCaliWave' in dir():
   OutputObjectSpecCaliWave = "LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolder
   OutputTagSpecCaliWave = LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolder,LArCalibFolderOutputTag)
   
if ( ReadPedFromCOOL ):      
   if 'InputPedSQLiteFile' in dir():
      InputDBConnectionPed = DBConnectionFile(InputPedSQLiteFile)
   else:
      InputDBConnectionPed = DBConnectionCOOL

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"    

DelayLog = logging.getLogger( "DelayLog" )

include ("LArConditionsCommon/LArMinimalSetup.py")

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   DelayLog.info( "No FullFileName! Please give a FullFileName list.")
   theApp.exit(-1)
else :   
   svcMgr.EventSelector.Input=FullFileName
   
svcMgr.EventSelector.MaxBadEvents = 0

theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/HIGH"  ]
theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/MEDIUM"]
theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/LOW"   ]

###############################################################
#                                                             #
#                To read BS for one or more FT                #
#                                                             #
###############################################################

## For splitting the delay runs, there are three new jobOptions.
## All three are vectors of integers
#################################################################

from LArByteStream.LArByteStreamConf import LArRodDecoder
svcMgr.ToolSvc += LArRodDecoder()

#ToolSvc.LArRodDecoder.BEPreselection     = [1]                                                   ## : [Barrel=0,Endcap=1]
#ToolSvc.LArRodDecoder.PosNegPreselection = [1]                                                   ## : [C-side (negative eta)=0, A-side (positive eta)=1]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]               ## : first half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31]     ## : second half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,4,7,8,11,12,13,14,17,18,19,20,23,24]             ## : [EMEC Standard feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [2,9,15,21]                                           ## : [EMEC Special feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [3,10,16,22]                                          ## : [HEC feedthrough numbers]  (note: slots 1&2 are EMEC slots)
#ToolSvc.LArRodDecoder.FTNumPreselection  = [6]                                                   ## : [FCAL feedthrough number]
      
##########################################################################
#                                                                        #
#                      Delay run reconstruction                          #
#                                                                        #
##########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

include ("LArCalibProcessing/LArCalib_BadChanTool.py")

if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB   
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if ( ReadPedFromCOOL ):
   PedestalFolder = LArCalib_Flags.LArPedestalFolder
   if 'PedLArCalibFolderTag' in dir() :
      PedestalTagSpec = LArCalibFolderTag (PedestalFolder,PedLArCalibFolderTag)
      conddb.addFolder("",PedestalFolder+"<tag>"+PedestalTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection)
   else:   
      conddb.addFolder("",PedestalFolder+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection)    
else:
   if 'InputPedPoolFileName' in dir():
      PoolFileList += [ InputPedPoolFileDir+"/"+InputPedPoolFileName ]
   
if ( len(PoolFileList)>0 ):
   
   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

#--------------------------------------------------
# LArCaliWaveBuilderXtalk
#--------------------------------------------------

PartitionPulsed = SubDet + Side    # Partition should be BarrelA or BarrelC or EndcapA or EndcapC
if not (PartitionPulsed=="BarrelA" or PartitionPulsed=="BarrelC" or PartitionPulsed=="EndcapA" or PartitionPulsed=="EndcapC"):
   printfunc ("[Xtalk JobOptions] Error: PartitionPulsed="+PartitionPulsed+". It should be BarrelA or BarrelC or EndcapA or EndcapC")
   theApp.exit(-1)

from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveBuilderXtalk
LArCaliWaveBuilderXtalk = LArCaliWaveBuilderXtalk("LArCaliWaveBuilderXtalk")

LArCaliWaveBuilderXtalk.KeyList         = GainList
if (doCaliWaveSelector) :
   LArCaliWaveBuilderXtalk.KeyOutput    = KeyOutput+"multi"
else :   
   LArCaliWaveBuilderXtalk.KeyOutput    = KeyOutput
LArCaliWaveBuilderXtalk.ADCsaturation   = ADCsaturation
LArCaliWaveBuilderXtalk.CalibPattern    = CalibPattern          # StripSingle[1-4] / MiddleSingle[1-8] / BackSingle[1-4]
LArCaliWaveBuilderXtalk.PartitionPulsed = PartitionPulsed       # Partition should be BarrelA or BarrelC or EndcapA or EndcapC
LArCaliWaveBuilderXtalk.SpecialCrate    = SpecialCrate          # EMEC special crates
LArCaliWaveBuilderXtalk.SpecialRegion   = SpecialRegion 	# EMEC special region
LArCaliWaveBuilderXtalk.FeedthroughPul  = FeedthroughPul	# Used to check calibration pattern, ensure there are no bad channels / disconnected FEBs
LArCaliWaveBuilderXtalk.CutOnSample     = CutOnSample   	# Cut on the number of read samples (usualy 32) to save memory

if 'Ramp' in FullFileName:                                      # for ramp runs : only one step (ie. no delay)
   printfunc ("[Xtalk JobOptions] Ramp run detected: set LArCaliWaveBuilderXtalk.NSteps to 1")
   LArCaliWaveBuilderXtalk.NSteps       = 1

topSequence+=LArCaliWaveBuilderXtalk

##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

if ( doMonitoring ) :
   
   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   topSequence += AthenaMonManager( "LArMon" )
   LArMon = topSequence.LArMon

   LArMon.FileKey = "GLOBAL"
   LArMon.ManualDataTypeSetup = True
   LArMon.Environment         = "tier0"
   LArMon.ManualRunLBSetup    = True
   LArMon.Run                 = int(RunNumberList[0])
   LArMon.LumiBlock           = 1
   LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run
   
#   include("LArMonTools/LArFEBMon_jobOptions.py")
   include("LArMonTools/LArXtalk_jobOptions.py")
   
   RootHistOutputFileName='RootHistos_'+BaseFileName+".root"

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import THistSvc
   if os.path.exists(OutputCaliWaveRootFileDir+ "/" +RootHistOutputFileName): 
      os.remove(OutputCaliWaveRootFileDir + "/" +RootHistOutputFileName)
   svcMgr += THistSvc()
   #svcMgr.THistSvc.Output = ["AllMon DATAFILE='"+OutputCaliWaveRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"] 

   svcMgr.THistSvc.Output =  ["GLOBAL DATAFILE='"+OutputCaliWaveRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"]

if ( WriteNtuple ):
   
   from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple
   LArCaliWaves2Ntuple = LArCaliWaves2Ntuple( "LArCaliWaves2Ntuple" )
   LArCaliWaves2Ntuple.NtupleName  = "CALIWAVE"
   LArCaliWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
   LArCaliWaves2Ntuple.SaveJitter = SaveJitter
   LArCaliWaves2Ntuple.KeyList     = [ KeyOutput ]
   
   topSequence+=LArCaliWaves2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName): 
      os.remove(OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName+"' OPT='NEW'" ]

if ( WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName): 
      os.remove(OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName)
   OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName,
                                           [OutputObjectSpecCaliWave],[OutputTagSpecCaliWave],WriteIOV)
   OutputConditionsAlg.Run1 = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg.Run2 = IOVEnd
   
   svcMgr.IOVDbSvc.dbConnection  = OutputDB
   
   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
   svcMgr.IOVRegistrationSvc.RecreateFolders = False

###########################################################################

svcMgr.MessageSvc.OutputLevel  = WARNING
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

##########################################################################
#                                                                        #
#                         Print summary                                  #
#                                                                        #
##########################################################################

DelayLog.info( " ======================================================== " )
DelayLog.info( " ***            LAr Delay Xtalk summary               *** " )
DelayLog.info( " ======================================================== " )
DelayLog.info( " RunNumber                          = "+str(RunNumberList) )
DelayLog.info( " SubDetector                        = "+SubDet )
DelayLog.info( " PartitionPulsed                    = "+PartitionPulsed )
DelayLog.info( " CalibPattern                       = "+CalibPattern )
DelayLog.info( " CutOnSample                        = "+str(CutOnSample) )
DelayLog.info( " FeedthroughPul                     = "+str(FeedthroughPul) )
DelayLog.info( " SpecialCrate                       = "+str(SpecialCrate) )
DelayLog.info( " SpecialRegion                      = "+SpecialRegion )
DelayLog.info( " Type                               = "+Type  )
DelayLog.info( " LArGain                            = "+str(GainList) )
for i in range(len(FullFileName)):
   DelayLog.info( " FullFileName                       = "+FullFileName[i] )
if ( ReadPedFromCOOL ):
   DelayLog.info( " InputDBConnectionPed               = "+InputDBConnectionPed)
else :
   DelayLog.info( " InputPedPoolFileName               = "+InputPedPoolFileName)
if 'PedLArCalibFolderTag' in dir() :   
   DelayLog.info( " PedLArCalibFolderTag               = "+PedLArCalibFolderTag )
DelayLog.info( " OutputCaliWaveRootFullFileName     = "+OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName )
DelayLog.info( " OutputCaliWavePoolFullFileName     = "+OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName )
DelayLog.info( " OutputObjectSpecCaliWave           = "+OutputObjectSpecCaliWave )
DelayLog.info( " OutputTagSpecCaliWave              = "+OutputTagSpecCaliWave )
DelayLog.info( " IOVBegin                           = "+str(IOVBegin) )
DelayLog.info( " IOVEnd                             = "+str(IOVEnd) )
DelayLog.info( " LArCalibOutputDB                   = "+OutputDB )
DelayLog.info( " ======================================================== " )

#######################################################################################
