from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <pavol@mail.cern.ch>
#
# jO to reconstruct a DELAY per calibration line 
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "EndCap"

if not 'RunNumberList' in dir():
   RunNumberList = [ '00121536','00121539','00121542' ]
   
if not 'AllWavesPerCh' in dir():
      AllWavesPerCh      =  True

if not 'MaxCalLinePerCh' in dir():
      MaxCalLinePerCh = 4

if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Delay"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2008/"+RunNumberList[0]
   
if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition = "EB-EC*"
   else:
      Partition = "EB-EMB*"

if not 'doCaliWaveSelector' in dir():
   doCaliWaveSelector = False      

#if not 'RecAll' in dir():
#   RecAll = False

if not 'Trigger' in dir():
   if (int(RunNumberList[0]))<99800 :
      Trigger = "*"+Partition
   else :
      Trigger = "calibration_LArElec-Delay"+".*"+Partition
   
if not 'FullFileName' in dir():
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
#                Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = False

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = False  

if not 'online' in dir():
   online = False
   
#######################################################
#       Delay properties
#######################################################

if not 'DACs' in dir():
   DACs = [500] # Needed if runAccumulator = True

if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True # Xtalk correction for strips

if not 'ShortCorrector' in dir():
   ShortCorrector = True # correctio for short

if not "ADCSaturation" in dir():
   ADCsaturation  = 4095 # Set to 0 if you want to keep saturating pulses otherwise 4095
   
if not 'SubtractPed' in dir(): # Set whether to substract pedestals. Pedestals come from COOL, or local Pool file if 'PedPoolFileName' is defined, or fake pedestal subraction as 'extrema ratio'
   SubtractPed = True

if not 'CheckEmptyPhases' in dir():
   CheckEmptyPhases = True

if not 'SaveDerivedInfo' in dir():
   SaveDerivedInfo = True

if not 'SaveJitter' in dir():
   SaveJitter = True

if not 'CorrectBadChannels' in dir():
   CorrectBadChannels=True
  
   
#######################################################
#      Delay output name
#######################################################

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
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;user=ATLAS_COOL_READER"

## Pedestall   
   
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
   rs=FolderTagResover()
   LArCalibFolderOutputTag = rs.getFolderTagSuffix(LArCalib_Flags.LArCaliWaveFolder)    
   
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
   
if AllWavesPerCh:
   KeyOutputSplitted = []
   for i in range(0, MaxCalLinePerCh):
            KeyOutputSplitted.append(KeyOutput+str(i))

if not 'BaseFileName' in dir():
   BaseFileName = "LArCaliWave"
   
for RunNumber in RunNumberList :
   BaseFileName = BaseFileName+"_"+str(RunNumber)

if StripsXtalkCorr :
   BaseFileName = BaseFileName+"_StripsXtalkCorr"

if AllWavesPerCh:
   BaseFileName = BaseFileName+"_single"
else:
   BaseFileName = BaseFileName+"_averaged"
     
if not 'OutputCaliWaveRootFileName' in dir():
   OutputCaliWaveRootFileName = BaseFileName+".root"
   
if not 'OutputCaliWavePoolFileName' in dir():
   OutputCaliWavePoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecCaliWave' in dir():
   if StripsXtalkCorr:
      OutputObjectSpecCaliWave = ["LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolderXtlk]
      OutputTagSpecCaliWave = LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolderXtlk,LArCalibFolderOutputTag)
   else:
      if ( AllWavesPerCh ) :
         OutputObjectSpecCaliWave = []
         OutputTagSpecCaliWave = []
         for i in range(0, MaxCalLinePerCh):
            if doCaliWaveSelector:
               OutputObjectSpecCaliWave.append("LArCaliWaveContainer#"+KeyOutputSplitted[i]+"Sel"+"#"+LArCalib_Flags.LArCaliWaveFolder+str(i))
            else:   
               OutputObjectSpecCaliWave.append("LArCaliWaveContainer#"+KeyOutputSplitted[i]+"#"+LArCalib_Flags.LArCaliWaveFolder+str(i))
            OutputTagSpecCaliWave.append(LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolder+str(i),LArCalibFolderOutputTag))   
      else:         
         if doCaliWaveSelector:
            OutputObjectSpecCaliWave = ["LArCaliWaveContainer#"+KeyOutput+"Sel"+"#"+LArCalib_Flags.LArCaliWaveFolder]
         else:   
            OutputObjectSpecCaliWave = ["LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolder]
         OutputTagSpecCaliWave = [LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolder,LArCalibFolderOutputTag)]
   
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
      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;user=ATLAS_COOL_READER"
      InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"      
      
#######################################################################################
# print summary
#######################################################################################

DelayLog = logging.getLogger( "DelayLog" )
DelayLog.info( " ======================================================== " )
DelayLog.info( " ***                 LAr Delay summary                *** " )
DelayLog.info( " ======================================================== " )
DelayLog.info( " RunNumber                          = "+str(RunNumberList) )
DelayLog.info( " SubDetector                        = "+SubDet )
DelayLog.info( " Partition                          = "+Partition )
DelayLog.info( " Type                               = Delay "  )
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
#DelayLog.info( " OutputObjectSpecCaliWave           = "+OutputObjectSpecCaliWave )
##DelayLog.info( " OutputTagSpecCaliWave              = "+OutputTagSpecCaliWave )
DelayLog.info( " IOVBegin                           = "+str(IOVBegin) )
DelayLog.info( " IOVEnd                             = "+str(IOVEnd) )
DelayLog.info( " LArCalibOutputDB                   = "+OutputDB )
DelayLog.info( " ======================================================== " )
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

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   DelayLog.info( "No FullFileName! Please give a FullFileName list.")
   theApp.exit(-1)
else :   
   svcMgr.EventSelector.Input=FullFileName
   
svcMgr.EventSelector.MaxBadEvents = 0

##############################################################################################
#                                                                                            #
# FullFileName is already the list of files compute by the fonction GetInputFilesFromTokens  #
#                                                                                            #
# maybe useful one day                                                                       #
#                                                                                            #
# else                                                                                        #
#   svcMgr.EventSelector.Input=OneFileName                                           #
#   for i in range(len(svcMgr.EventSelector.Input)):                                 #
#      theByteStreamInputSvc.NumFile+=[10000]                                                #
##############################################################################################
   

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

#ToolSvc.LArRodDecoder.BEPreselection     = [0]                                                   ## : [Barrel=0,Endcap=1]
#ToolSvc.LArRodDecoder.PosNegPreselection = [1]                                                   ## : [C-side (negative eta)=0, A-side (positive eta)=1]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]               ## : first half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31]     ## : second half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,4,7,8,11,12,13,14,17,18,19,20,23,24]             ## : [EMEC Standard feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [2,9,15,21]                                           ## : [EMEC Special feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [3,10,16,22]                                          ## : [HEC feedthrough numbers]  (note: slots 1&2 are EMEC slots)
#ToolSvc.LArRodDecoder.FTNumPreselection  = [6]                                                   ## : [FCAL feedthrough number]
   

###########################################################################
#                                                                         #
#                          Accumulator                                    #
#                                                                         #
###########################################################################

if ( runAccumulator ) :
   # this is a OLD jobOptions which can maybe work but only for the barrel                        #
   # can be used as a skeleton if needed but                                                      #
   # need to be updated for the barrel and the patterns for EMEC, HEC and FCAL need to be added   #
   include("LArCalibProcessing/LArCalib_CalibrationPatterns.py")

else:
   theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
   theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/HIGH"  ]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/MEDIUM"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/LOW"   ]

## This algorithm verifies that no FEBs are dropping out of the run
## If it finds corrupt events, it breaks the event loop and terminates the job rapidly
include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")   
#from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
#theLArBadEventCatcher=LArBadEventCatcher()
#theLArBadEventCatcher.CheckAccCalibDigitCont=True
#theLArBadEventCatcher.CheckBSErrors=True
#theLArBadEventCatcher.KeyList=GainList
#theLArBadEventCatcher.StopOnError=False
#topSequence+=theLArBadEventCatcher      
      
##########################################################################
#                                                                        #
#                      Delay run reconstruction                          #
#                                                                        #
##########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

include ("LArCalibProcessing/LArCalib_BadChanTool.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

BadChannelsFolder="/LAR/BadChannelsOfl/BadChannels"
MissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"

if not 'InputBadChannelSQLiteFile' in dir():
   DelayLog.info( "Read Bad Channels from Oracle DB")
else :   
   DelayLog. info( "Read Bad Channels from SQLite file") 

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

svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "prfile:poolcond/PoolCat_oflcond.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_diskbuffer_afs.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond09_data.000001.lar.COND_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond08_data.000001.lar.COND_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000005.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000006.lar_conditions.recon.pool.v0000_castor.xml"]


if ( doLArCalibDataQuality  ) :
   ## The reference is the Oracle DB
   if StripsXtalkCorr:
      conddb.addFolder("",LArCalib_Flags.LArCaliWaveFolderXtlk+"<key>LArCaliWaveRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)
   else:
      conddb.addFolder("",LArCalib_Flags.LArCaliWaveFolder+"<key>LArCaliWaveRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)

if (SubtractPed):
   if ( ReadPedFromCOOL ):
      if not 'InputPedSQLiteFile' in dir():
         DelayLog.info( "Read Pedestal from Oracle DB")
      else :   
         DelayLog.info( "Read Pedestal from SQLite file") 

      PedestalFolder = LArCalib_Flags.LArPedestalFolder
      if 'PedLArCalibFolderTag' in dir() :
         PedestalTagSpec = LArCalibFolderTag (PedestalFolder,PedLArCalibFolderTag)
         conddb.addFolder("",PedestalFolder+"<tag>"+PedestalTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection)
      else :   
         conddb.addFolder("",PedestalFolder+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection)    
   else:
      if 'InputPedPoolFileName' in dir():
         DelayLog.info( "Read Pedestal from POOL file") 
         PoolFileList += [ InputPedPoolFileDir+"/"+InputPedPoolFileName ]

      else:
         DelayLog.info( "LArCaliWaveBuilder WARNING : Using fake pedestals subtraction for all channels.")
   
if ( len(PoolFileList)>0 ):
   
   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

if ( ShortCorrector ):
   
   from LArCalibUtils.LArCalibUtilsConf import LArCalibShortCorrector
   theLArCalibShortCorrector = LArCalibShortCorrector()
   theLArCalibShortCorrector.KeyList = GainList
   theLArCalibShortCorrector.OutputLevel=INFO
   topSequence +=theLArCalibShortCorrector
   
if ( StripsXtalkCorr ) :

   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   from LArCalibUtils.LArCalibUtilsConf import LArStripsCrossTalkCorrector
   theLArStripsCrossTalkCorrector = LArStripsCrossTalkCorrector()
   theLArStripsCrossTalkCorrector.KeyList = GainList
   theLArStripsCrossTalkCorrector.ADCsaturation = ADCsaturation
   theLArStripsCrossTalkCorrector.NoXtalkCorr=LArBadChannelMasker("NoXtalkCorr",
                                                               DoMasking=True,
                                                               ProblemsToMask=["deadReadout","deadPhys","deadCalib","almostDead"]
                                                               )
   theLArStripsCrossTalkCorrector.DontUseForXtalkCorr=LArBadChannelMasker("DontUseForXtalkCorr",
                                                                       DoMasking=True,
                                                                       ProblemsToMask=["short","peculiarCalibrationLine"]
                                                                       )                           
   theLArStripsCrossTalkCorrector.AcceptableDifference=25.0 #in per-cent                                                                       
   topSequence +=theLArStripsCrossTalkCorrector
 
# Now the complicated part, splitter or not
from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveBuilder
if ( AllWavesPerCh ) :
   ContNameSplitted = []
   for i in range(0, MaxCalLinePerCh):
          ContNameSplitted.append("CalLine"+str(i))

   from LArCalibUtils.LArCalibUtilsConf import LArAccumulatedCalibDigitContSplitter
   LArAccumulatedCalibDigitContSplitter = LArAccumulatedCalibDigitContSplitter("LArAccumulatedCalibDigitContSplitter")
   LArAccumulatedCalibDigitContSplitter.KeyList         = GainList
   LArAccumulatedCalibDigitContSplitter.KeyOutputList = ContNameSplitted
   LArAccumulatedCalibDigitContSplitter.NumberSplitted  = MaxCalLinePerCh
   LArAccumulatedCalibDigitContSplitter.UseDacAndIsPulsedIndex = True
   LArAccumulatedCalibDigitContSplitter.OutputLevel = ERROR
   topSequence += LArAccumulatedCalibDigitContSplitter

   LArCaliWaveBuilderVec = []
   if(doCaliWaveSelector):
           from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveSelector
           LArCaliWaveSelVec = []

   for i in range(0, MaxCalLinePerCh):
            topSequence += LArCaliWaveBuilder("LArCaliWaveBuilder"+str(i+1))
            exec 'LArCaliWaveBuilderVec.append( topSequence.LArCaliWaveBuilder%(fn)s )' % {'fn' :i+1}
            LArCaliWaveBuilderVec[i].KeyList         = [ ContNameSplitted[i] ]
            LArCaliWaveBuilderVec[i].KeyOutput       = KeyOutputSplitted[i]
            LArCaliWaveBuilderVec[i].ADCsaturation   = ADCsaturation
            LArCaliWaveBuilderVec[i].GroupingType    = GroupingType
            LArCaliWaveBuilderVec[i].SubtractPed     = SubtractPed
            LArCaliWaveBuilderVec[i].CheckEmptyPhases= CheckEmptyPhases
            LArCaliWaveBuilderVec[i].NBaseline       = 0
#            LArCaliWaveBuilderVec[i].RecAllCells     = RecAll
            LArCaliWaveBuilderVec[i].UseDacAndIsPulsedIndex = True
            LArCaliWaveBuilderVec[i].OutputLevel     = ERROR
            if(doCaliWaveSelector):
                 topSequence += LArCaliWaveSelector("LArCaliWaveSelector"+str(i+1))
                 exec 'LArCaliWaveSelVec.append( topSequence.LArCaliWaveSelector%(fn)s )' % {'fn' :i+1}
                 LArCaliWaveSelVec[i].KeyList         = [ KeyOutputSplitted[i] ]
                 LArCaliWaveSelVec[i].KeyOutput       = KeyOutputSplitted[i]+"Sel"       
                 LArCaliWaveSelVec[i].SelectionList   = [ "HEC/0/1/3600","HEC/1/1/3600","HEC/2/1/1800","HEC/3/1/1800"]
                 LArCaliWaveSelVec[i].SelectionList   += [ "HEC/0/0/460","HEC/1/0/460","HEC/2/0/230","HEC/3/0/230"]
                 LArCaliWaveSelVec[i].SelectionList   += [ "HEC/0/2/24000","HEC/1/2/24000","HEC/2/2/18000","HEC/3/2/18000"]
                 LArCaliWaveSelVec[i].OutputLevel     = ERROR

   #LArCaliWaveBuilderVec[3].OutputLevel     = DEBUG

else :
   LArCaliWaveBuilder = LArCaliWaveBuilder()
   LArCaliWaveBuilder.KeyList          = GainList
   LArCaliWaveBuilder.KeyOutput        = KeyOutput
   LArCaliWaveBuilder.GroupingType     = GroupingType
   LArCaliWaveBuilder.SubtractPed      = SubtractPed
   LArCaliWaveBuilder.CheckEmptyPhases = CheckEmptyPhases
   LArCaliWaveBuilder.NBaseline        = 0 # to avoid the use of the baseline when Pedestal are missing
   LArCaliWaveBuilder.UseDacAndIsPulsedIndex = False # should have an impact only for HEC

   if StripsXtalkCorr:
      LArCaliWaveBuilder.ADCsaturation = 0
   else:
      LArCaliWaveBuilder.ADCsaturation = ADCsaturation
   
   topSequence+=LArCaliWaveBuilder

   if (doCaliWaveSelector) :
      from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveSelector
      LArCaliWaveSelector = LArCaliWaveSelector("LArCaliWaveSelector")
      LArCaliWaveSelector.KeyList         = [ KeyOutput ]
      LArCaliWaveSelector.KeyOutput       = KeyOutput+"Sel"
      LArCaliWaveSelector.SelectionList = [ "HEC/0/0/460","HEC/1/0/460","HEC/2/0/230","HEC/3/0/230" ] 
      LArCaliWaveSelector.SelectionList += [ "HEC/0/1/3600","HEC/1/1/3600","HEC/2/1/1800","HEC/3/1/1800"]
      LArCaliWaveSelector.SelectionList += [ "HEC/0/2/24000","HEC/1/2/24000","HEC/2/2/18000","HEC/3/2/18000" ]
      
      topSequence+=LArCaliWaveSelector

######################################################################
#                                                                    #
#                          Correction                                #
#                                                                    #
######################################################################
# Not done on splitted caliwaves yet
if CorrectBadChannels:

   from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArCaliWaveContainer_
   theLArCaliWavePatcher=LArCalibPatchingAlg_LArCaliWaveContainer_("LArCaliWavePatcher")
   theLArCaliWavePatcher.ContainerKey=KeyOutput
   #theLArCaliWavePatcher.PatchMethod="PhiNeighbor" ##take the first neigbour
   theLArCaliWavePatcher.PatchMethod="PhiAverage" ##do an aveage in phi after removing bad and empty event
   theLArCaliWavePatcher.OutputLevel=INFO
   
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
   theLArRCBMasker.DoMasking=True
   theLArRCBMasker.ProblemsToMask=[
      "deadCalib","deadReadout","deadPhys","almostDead","short",
      ]
   ToolSvc+=theLArRCBMasker
   theLArCaliWavePatcher.MaskingTool=theLArRCBMasker
   topSequence+=theLArCaliWavePatcher
 
##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

if ( doLArCalibDataQuality  ) :

   from LArCalibDataQuality.LArCalibDataQualityConf import LArCaliWaveValidationAlg
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   theLArDelayValBCMask=LArBadChannelMasker("DelayValBCMask",
                                            DoMasking=True,
                                            ProblemsToMask=["deadReadout","deadCalib","deadPhys","almostDead",
                                                            "highNoiseHG","highNoiseMG","highNoiseLG"]
                                            )
   svcMgr.ToolSvc+=theLArDelayValBCMask
                    
   theCaliWaveValidationAlg=LArCaliWaveValidationAlg()
   theCaliWaveValidationAlg.BadChannelMaskingTool=theLArDelayValBCMask
   theCaliWaveValidationAlg.ValidationKey=KeyOutput
   theCaliWaveValidationAlg.ReferenceKey="LArCaliWaveRef"
   theCaliWaveValidationAlg.MsgLevelForDeviations=INFO
   theCaliWaveValidationAlg.ListOfDevFEBs="caliWaveFebs.txt"
   topSequence+=theCaliWaveValidationAlg
   
   ## second instance of the validation tool to detect "bad" channel 
   theBadCaliWave=LArCaliWaveValidationAlg("theBadCaliWave")
   theBadCaliWave.BadChannelMaskingTool=theLArDelayValBCMask
   theBadCaliWave.ValidationKey=KeyOutput
   theBadCaliWave.ReferenceKey="LArCaliWaveRef"
   theBadCaliWave.MsgLevelForDeviations=INFO
   theBadCaliWave.ListOfDevFEBs="Bad_caliWaveFebs.txt"
   topSequence+=theBadCaliWave
   

if ( doMonitoring ) :
   
   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   topSequence += AthenaMonManager( "LArMon" )
   LArMon = topSequence.LArMon

   ## old style
   #LArMon.FileKey = "AllMon"
   #LArMon.ManualDataTypeSetup = True
   #LArMon.Environment         = "user"
   #LArMon.ManualRunLBSetup    = True
   #LArMon.Run                 = 1
   #LArMon.LumiBlock           = 1
   #LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run

   ## tier0 style
   LArMon.FileKey = "GLOBAL"
   LArMon.ManualDataTypeSetup = True
   LArMon.Environment         = "tier0"
   LArMon.ManualRunLBSetup    = True
   LArMon.Run                 = int(RunNumberList[0])
   LArMon.LumiBlock           = 1
   LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run
   
   include("LArMonTools/LArFEBMon_jobOptions.py")
   
   RootHistOutputFileName='RootHistos_'+BaseFileName+".root"

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import THistSvc
   if os.path.exists(OutputCaliWaveRootFileDir+ "/" +RootHistOutputFileName): 
      os.remove(OutputCaliWaveRootFileDir + "/" +RootHistOutputFileName)
   svcMgr += THistSvc()
   #svcMgr.THistSvc.Output = ["AllMon DATAFILE='"+OutputCaliWaveRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"] 

   svcMgr.THistSvc.Output =  ["GLOBAL DATAFILE='"+OutputCaliWaveRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"]


if (WriteNtuple):
   from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple
   if ( AllWavesPerCh ) :
        LArCaliWaves2NtupleVec = []
        for i in range(0, MaxCalLinePerCh):
              thisAlgo =  "LArCaliWaves2Ntuple"+str(i+1)
              topSequence += LArCaliWaves2Ntuple(thisAlgo)
              exec 'LArCaliWaves2NtupleVec.append( topSequence.LArCaliWaves2Ntuple%(fn)s )' % {'fn': i+1}
              LArCaliWaves2NtupleVec[i].NtupleName = "CALIWAVE"+str(i+1)
              if (doCaliWaveSelector):
                  LArCaliWaves2NtupleVec[i].KeyList    = [ KeyOutputSplitted[i]+"Sel" ]
              else:    
                  LArCaliWaves2NtupleVec[i].KeyList    = [ KeyOutputSplitted[i] ]
              LArCaliWaves2NtupleVec[i].SaveJitter = SaveJitter    
              LArCaliWaves2NtupleVec[i].SaveDerivedInfo = SaveDerivedInfo

        #LArCaliWaves2NtupleVec[3].OutputLevel=DEBUG    

   else :
        LArCaliWaves2Ntuple = LArCaliWaves2Ntuple( "LArCaliWaves2Ntuple" )
        LArCaliWaves2Ntuple.NtupleName  = "CALIWAVE"
        LArCaliWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
        LArCaliWaves2Ntuple.SaveJitter = SaveJitter
        if (doCaliWaveSelector):
           LArCaliWaves2Ntuple.KeyList     = [ KeyOutput+"Sel" ]
        else:   
           LArCaliWaves2Ntuple.KeyList     = [ KeyOutput ]
        topSequence+=LArCaliWaves2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   if not hasattr(svcMgr, 'NTupleSvc'):
      from GaudiSvc.GaudiSvcConf import NTupleSvc
      svcMgr += NTupleSvc()   
   if os.path.exists(OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName):
      os.remove(OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName)
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputCaliWaveRootFileDir+"/"+OutputCaliWaveRootFileName+"' OPT='NEW'" ]

if ( WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName): 
      os.remove(OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName)
   OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputCaliWavePoolFileDir+"/"+OutputCaliWavePoolFileName,
                                           OutputObjectSpecCaliWave,OutputTagSpecCaliWave,WriteIOV)
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

###########################################################################

