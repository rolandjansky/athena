# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

###################################################################
#
# Job options file for accessing LArRawConditions objects from COOL
#
#==================================================================

include.block ( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )

from LArConditionsCommon.LArCondFlags import larCondFlags 

LArDBConnection = ""
LArDB = "LAR_OFL"

if larCondFlags.LArDBConnection.statusOn  :
  LArDBConnection = larCondFlags.LArDBConnection()
  LArDB = ""

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

larCondFlags.config_ElecCalibMC()

if svcMgr.MessageSvc.OutputLevel <= DEBUG :
  printfunc (larCondFlags)

if "SuperCells" not in dir():
  SuperCells = False

# Access to IOVSvc and IOVDbSvc
# Must list the folders to be used for reading
#--------------------------------------------------------------

from IOVDbSvc.CondDB import conddb

from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArDAC2uAMC_LArDAC2uASym_ as LArDAC2uASymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasMC_LArMinBiasSym_ as LArMinBiasSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArNoiseMC_LArNoiseSym_ as LArNoiseSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArRampMC_LArRampSym_ as LArRampSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArfSamplMC_LArfSamplSym_ as LArfSamplSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LAruA2MeVMC_LAruA2MeVSym_ as LAruA2MeVSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArAutoCorrMC_LArAutoCorrSym_ as LArAutoCorrSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArShape32MC_LArShape32Sym_ as LArShapeSymAlg
from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMphysOverMcalMC_LArMphysOverMcalSym_ as LArMPhysOverMcalSymAlg

from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault

if SuperCells:
  larCondDBFolders = [("CondAttrListCollection","/LAR/ElecCalibMCSC/Ramp","LArRamp", LArRampSymAlg, "LAREleCalibMCSCRamp-000" ),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/AutoCorr","LArAutoCorr", LArAutoCorrSymAlg,"LAREleCalibMCSCAutoCorr-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/DAC2uA","LArDAC2uA",LArDAC2uASymAlg,"LAREleCalibMCSCDAC2uA-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/Pedestal","LArPedestal",None,"LAREleCalibMCSCPedestal-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/Noise","LArNoise",LArNoiseSymAlg,"LAREleCalibMCSCNoise-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/fSampl","LArfSampl",LArfSamplSymAlg,"LAREleCalibMCSCfSampl-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/uA2MeV","LAruA2MeV", LAruA2MeVSymAlg,"LAREleCalibMCSCuA2MeV-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/MinBias","LArMinBias",LArMinBiasSymAlg,"LAREleCalibMCSCMinBias-000"),
                      ("CondAttrListCollection","/LAR/ElecCalibMCSC/MinBiasAverage","LArMinBiasAverage",LArMinBiasAverageSymAlg,"LAREleCalibMCSCMinBiasAverage-000")
                    ]
  
  if larCondFlags.useMCShape():
    larCondDBFolders += [("LArShape32MC","/LAR/ElecCalibMCSC/Shape","LArShape",LArShapeSymAlg,None)]
else:
  larCondDBFolders = [("LArRampMC","/LAR/ElecCalibMC/Ramp","LArRamp", LArRampSymAlg ,None),
                      ("LArAutoCorrMC","/LAR/ElecCalibMC/AutoCorr","LArAutoCorr", LArAutoCorrSymAlg,None),
                      ("LArDAC2uAMC","/LAR/ElecCalibMC/DAC2uA","LArDAC2uA",LArDAC2uASymAlg,None),
                      ("LArPedestalMC","/LAR/ElecCalibMC/Pedestal","LArPedestal",None,None),
                      ("LArNoiseMC","/LAR/ElecCalibMC/Noise","LArNoise",LArNoiseSymAlg,None),
                      ("LArfSamplMC","/LAR/ElecCalibMC/fSampl","LArfSampl",LArfSamplSymAlg,None),
                      ("LAruA2MeVMC","/LAR/ElecCalibMC/uA2MeV","LAruA2MeV", LAruA2MeVSymAlg,None),
                      ("LArMinBiasMC","/LAR/ElecCalibMC/MinBias","LArMinBias",LArMinBiasSymAlg,None),
                      ("LArMinBiasAverageMC","/LAR/ElecCalibMC/MinBiasAverage","LArMinBiasAverage",LArMinBiasAverageSymAlg,None)
                    ]
  
  if larCondFlags.useMCShape():
    larCondDBFolders += [("LArShape32MC","/LAR/ElecCalibMC/Shape","LArShape",LArShapeSymAlg,None)]

include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannels/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"
if LArDBConnection != "" and 'InputDBConnectionBadChannel' not in dir():
  InputDBConnectionBadChannel = LArDBConnection


from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg,LArBadFebCondAlg
if LArDBConnection != "":
  conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")
else:  
  conddb.addFolder(LArDB,BadChannelsFolder,className="CondAttrListCollection")
condSeq+=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)
if LArDBConnection != "":
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')
else:   
   conddb.addFolder(LArDB,MissingFEBsFolder,className='AthenaAttributeList')
condSeq+=LArBadFebCondAlg(ReadKey=MissingFEBsFolder)

condSeq+=LArBadFebCondAlg("LArKnownBadFebAlg",ReadKey="",WriteKey="LArKnownBadFEBs")
condSeq+=LArBadFebCondAlg("LArKnownMNBFebAlg",ReadKey="",WriteKey="LArKnownMNBFEBs")


## these may be conditional. 
if larCondFlags.hasMphys() :
  if SuperCells:
    larCondDBFolders += [("LArMphysOverMcalMC","/LAR/ElecCalibMCSC/MphysOverMcal","LArMphysOverMcal",LArMPhysOverMcalSymAlg,None)]
  else:
    larCondDBFolders += [("LArMphysOverMcalMC","/LAR/ElecCalibMC/MphysOverMcal","LArMphysOverMcal",LArMPhysOverMcalSymAlg,None)]

# HV Scale Corr
if larCondFlags.hasHVCorr() :
  if SuperCells:
    larCondDBFolders += [ ('LArHVScaleCorrComplete', '/LAR/ElecCalibMCSC/HVScaleCorr',"LArHVScaleCorr",None,None) ]
  else:
    larCondDBFolders += [ ('LArHVScaleCorrComplete', '/LAR/ElecCalibMC/HVScaleCorr',"LArHVScaleCorr",None,None) ]



LArMCSymCondAlgDefault()
## fill them all 
for className,fldr,key,calg,tag in larCondDBFolders:
  if calg is not None: 
    newkey=key+"Sym"
  else:
    newkey=key
  if tag is not None:
    conddb.addFolderWithTag(LArDB,fldr, tag, className=className,forceMC=True)
  else:
    conddb.addFolder(LArDB,fldr+LArDBConnection, className=className,forceMC=True)
  ## allow override
  larCondFlags.addTag(fldr,conddb)  
  if calg:
    condSeq+=calg(ReadKey=key,WriteKey=newkey)

## apply hierarchical tag                    
larCondFlags.addTag('/LAR/ElecCalibMC',conddb)
