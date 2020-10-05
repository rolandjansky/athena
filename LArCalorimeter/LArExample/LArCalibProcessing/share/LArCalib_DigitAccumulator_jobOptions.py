from future import standard_library
standard_library.install_aliases()
import subprocess

###############################################################################
#
# <Sandrine.Laplace@lapp.in2p3.fr>
#
# Example jobOptions to reconstruction one or more pedestal runs from  data
#         using accumulated pedestals
# Flags:
#  - Pedestal: run LArPedestalBuilder that reads the DSP LArAccumulatedDigits
#              dump corresponding Pedestals and LArAccumulatedDigits (key: gain) into ntuple
#  - AutoCorr: run LArAutorCorrBuilder that reads the DSP LArAccumulatedDigits
#              dump corresponding AutoCorr and (same than above) LArAccumulatedDigits into ntuple
#  - Accumulator: run LArDigitsAccumulator to crosscheck DSP computation
#              dump corresponding LArAccumulatedDigits (key: gain+AccuDigOff)
#                                                                             
###############################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

#######################################################
#                Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "Barrel"
   
if not 'RunNumberList' in dir():
   RunNumberList = [ 26924 ]

if not 'FilePrefix' in dir():
     FilePrefix = "daq.Pedestal"   
   
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2008"
   
if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"     

if not 'FullFileName' in dir():
   Trigger = "*"+Partition
   FullFileName =[]
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,RunNumber,FilePrefix,Trigger)

if not 'GainList' in dir():
   GainList = [ "HIGH" ]

if not 'KeyOutputAccum' in dir():  
   KeyOutputAccum    = "AccuDigOff"

if not 'GainListAccum' in dir():
   GainListAccum =[]
   for i in GainList:
      newKey=i+KeyOutputAccum
      GainListAccum.append(newKey)
      printfunc ("GainListAccum = ",GainListAccum)

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"

if not 'ChannelSelection' in dir():
   # read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"   

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"


#######################################################
#                Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = False

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = False

if not 'online' in dir():
   online = False
   
#######################################################
#       Pedestal and AutoCorrelation  properties
#######################################################

if not 'Pedestal' in dir():
   Pedestal = True
   
if not 'AutoCorr' in dir():
   AutoCorr = True

if not 'Accumulator' in dir():
   Accumulator = True

if not 'EventsRef' in dir():
   EventsRef     = 10
   
if not 'NSigma' in dir():
   NSigma = 5

if not 'NSamples' in dir():
   NSamples = 7   
   
if not 'WhichSample' in dir():
   WhichSample = 0

#######################################################
#      Pedestal and AutoCorrelation  output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = RunNumberList[0]
   
if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"   

if not 'OutputPedAutoCorrRootFileDir' in dir():
   OutputPedAutoCorrRootFileDir  = subprocess.getoutput("pwd")
   
if not 'OutputPedPoolFileDir' in dir():
   OutputPedPoolFileDir  = subprocess.getoutput("pwd")

if not 'PedLArCalibFolderTag' in dir():
   PedLArCalibFolderTag = LArCalib_Flags.tagSuffix 
   
if not 'OutputAutoCorrPoolFileDir' in dir():
   OutputAutoCorrPoolFileDir  = subprocess.getoutput("pwd")

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)   

if not 'KeyOutputAC' in dir():  
   KeyOutputAC      = "LArAutoCorr" 

if not 'KeyOutputPed' in dir():  
   KeyOutputPed      = "Pedestal" # need Pedestal to merge with oracle, LArPedestal doesn't work

if not 'BaseFileNamePed' in dir():
   BaseFileNamePed = "LArPedestal"

if not 'BaseFileNameAutoCorr' in dir():
   BaseFileNameAutoCorr = "LArAutoCorr"

if not 'BaseFileNameAccum' in dir():
   BaseFileNameAccum = "LArAccumulatedDigits"

for RunNumber in RunNumberList :
   if Pedestal :
      BaseFileNamePedAutoCorr  = BaseFileNamePed + "_"
      
   if AutoCorr :
      if Pedestal :
         BaseFileNamePedAutoCorr = BaseFileNamePedAutoCorr + BaseFileNameAutoCorr + "_"
      else :   
         BaseFileNamePedAutoCorr = BaseFileNameAutoCorr + "_"

   if Accumulator :
      BaseFileNamePedAutoCorr = BaseFileNameAccum + "_"

   BaseFileNamePedAutoCorr = BaseFileNamePedAutoCorr + str(RunNumber)+"_"+Partition.replace("*","") 
   BaseFileNamePed = BaseFileNamePed + "_" + str(RunNumber)+"_"+Partition.replace("*","") 
   BaseFileNameAutoCorr = BaseFileNameAutoCorr + "_" + str(RunNumber)+"_"+Partition.replace("*","")
   
if not 'OutputPedAutoCorrRootFileName' in dir():
   OutputPedAutoCorrRootFileName  = BaseFileNamePedAutoCorr+ ".root"
    
if not 'OutputAutoCorrPoolFileName' in dir():
   OutputAutoCorrPoolFileName  = BaseFileNameAutoCorr + ".pool.root"

if not 'OutputPedPoolFileName' in dir():
   OutputPedPoolFileName  = BaseFileNamePed + ".pool.root"

if not 'OutputObjectSpecPed' in dir():
   OutputObjectSpecPed = "LArPedestalComplete#"+KeyOutputPed+"#"+LArCalib_Flags.LArPedestalFolder
   OutputTagSpecPed=LArCalibFolderTag(LArCalib_Flags.LArPedestalFolder,PedLArCalibFolderTag)

if not 'OutputObjectSpecAutoCorr' in dir():
   OutputObjectSpecAutoCorr = "LArAutoCorrComplete#"+KeyOutputAC+"#"+LArCalib_Flags.LArAutoCorrFolder
   OutputTagSpecAutoCorr=LArCalibFolderTag(LArCalib_Flags.LArAutoCorrFolder,PedLArCalibFolderTag)   

#######################################################################################
# print summary
#######################################################################################

PedestalAutoCorrLog = logging.getLogger( "PedestalAutoCorrLog" )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " ***           LAr Pedestal/AutoCorr summary          *** " )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " RunNumber                         = "+str(RunNumberList) )
PedestalAutoCorrLog.info( " SubDetector                       = "+SubDet )
PedestalAutoCorrLog.info( " Partition                         = "+Partition )
PedestalAutoCorrLog.info( " Type                              = "+str(FilePrefix.strip().split('.',9)[1]) )
PedestalAutoCorrLog.info( " LArGain                           = "+str(GainList) )
for i in range(len(FullFileName)):
   PedestalAutoCorrLog.info( " FullFileName                      = "+FullFileName[i] )
PedestalAutoCorrLog.info( " PedLArCalibFolderTag              = "+PedLArCalibFolderTag )
PedestalAutoCorrLog.info( " OutputPedAutoCorrRootFullFileName = "+OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName )
PedestalAutoCorrLog.info( " OutputPedPoolFullFileName         = "+OutputPedPoolFileDir + "/" + OutputPedPoolFileName )
PedestalAutoCorrLog.info( " OutputAutoCorrPoolFullFileName    = "+OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName )
PedestalAutoCorrLog.info( " OutputObjectSpecPed               = "+OutputObjectSpecPed )
PedestalAutoCorrLog.info( " OutputTagSpecPed                  = "+OutputTagSpecPed )
PedestalAutoCorrLog.info( " OutputObjectSpecAutoCorr          = "+OutputObjectSpecAutoCorr )
PedestalAutoCorrLog.info( " OutputTagSpecAutoCorr             = "+OutputTagSpecAutoCorr )
PedestalAutoCorrLog.info( " IOVBegin                          = "+str(IOVBegin) )
PedestalAutoCorrLog.info( " IOVEnd                            = "+str(IOVEnd) )
PedestalAutoCorrLog.info( " LArCalibOutputDB                  = "+OutputDB )
PedestalAutoCorrLog.info( " ======================================================== " )

#######################################################################################
include ("LArConditionsCommon/LArMinimalSetup.py")

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   PedestalAutoCorrLog.info( "No FullFileName! Please give a FullFileName list")
   theApp.exit(-1)
else :   
   svcMgr.EventSelector.Input=FullFileName

from LArByteStream.LArByteStreamConf import LArRodDecoder
svcMgr.ToolSvc += LArRodDecoder()
theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]

theByteStreamAddressProviderSvc.TypeNames += ["LArAccumulatedDigitContainer/HIGH"]

#######################################################
#                                                     #
#      Pedestal run reconstruction                    #
#                                                     #
#######################################################


if Pedestal :

   from LArCalibUtils.LArCalibUtilsConf import LArPedestalBuilder
   LArPedestalBuilder=LArPedestalBuilder("LArPedestalBuilder")
   LArPedestalBuilder.KeyList      = GainList
   LArPedestalBuilder.KeyOutput    = KeyOutputPed
   LArPedestalBuilder.GroupingType = GroupingType

   topSequence += LArPedestalBuilder

if AutoCorr :
   from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrBuilder
   LArAutoCorrBuilder=LArAutoCorrBuilder("LArAutoCorrBuilder")
   LArAutoCorrBuilder.KeyList      = GainList
   LArAutoCorrBuilder.KeyOutput    = KeyOutputAC
   LArAutoCorrBuilder.GroupingType = GroupingType
   
   topSequence += LArAutoCorrBuilder

if Accumulator :
   from LArROD.LArRODConf import LArDigitsAccumulator
   LArDigitsAccumulator=LArDigitsAccumulator("LArDigitsAccumulator")
   LArDigitsAccumulator.KeyList = GainList
   LArDigitsAccumulator.LArAccuDigitContainerName=KeyOutputAccum

   topSequence += LArDigitsAccumulator
   
######################################################################
#                                                                    #
#                          Output                                    #
#                                                                    #
######################################################################


if ( WriteNtuple ) :
   
   if Pedestal :
      from LArCalibTools.LArCalibToolsConf import LArPedestals2Ntuple
      LArPedestals2Ntuple = LArPedestals2Ntuple("LArPedestals2Ntuple")
      LArPedestals2Ntuple.ContainerKey = KeyOutputPed
      
      topSequence += LArPedestals2Ntuple

   if AutoCorr :
      from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
      LArAutoCorr2Ntuple = LArAutoCorr2Ntuple( "LArAutoCorr2Ntuple" )
      LArAutoCorr2Ntuple.Nsamples     = NSamples
      LArAutoCorr2Ntuple.ContainerKey = KeyOutputAC
      
      topSequence += LArAutoCorr2Ntuple

   if Pedestal or AutoCorr:
      from LArCalibTools.LArCalibToolsConf import LArAccumulatedDigits2Ntuple
      for i in GainList:
         NtupleName="NtupleDSP"+i
         NtupleDSP = LArAccumulatedDigits2Ntuple( NtupleName )
         NtupleDSP.NSamples     = NSamples
         NtupleDSP.ContainerKey = i

         topSequence += NtupleDSP

         printfunc (NtupleDSP)

   if Accumulator :
      from LArCalibTools.LArCalibToolsConf import LArAccumulatedDigits2Ntuple
      for i in GainListAccum:
         NtupleName="NtupleOff"+i
         NtupleOff = LArAccumulatedDigits2Ntuple( NtupleName )
         NtupleOff.NSamples     = NSamples
         NtupleOff.ContainerKey = i

         topSequence += NtupleOff

         printfunc (NtupleOff)

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName): 
      os.remove(OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName)
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName+"' OPT='NEW'" ]



###########################################################################

svcMgr.MessageSvc.OutputLevel  = WARNING
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theApp.AuditAlgorithms=True
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()
svcMgr.ChronoStatSvc.OutputLevel  = INFO

###########################################################################
 
