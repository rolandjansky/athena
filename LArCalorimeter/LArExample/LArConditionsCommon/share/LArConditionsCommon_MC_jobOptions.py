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

# POOL Converters
#include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
#include ("LArRawConditions/LArRawConditionsDict_joboptions.py")


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

larCondDBFolders = [("LArRampMC","/LAR/ElecCalibMC/Ramp","LArRamp", LArRampSymAlg ),
                    ("LArAutoCorrMC","/LAR/ElecCalibMC/AutoCorr","LArAutoCorr", LArAutoCorrSymAlg),
                    ("LArDAC2uAMC","/LAR/ElecCalibMC/DAC2uA","LArDAC2uA",LArDAC2uASymAlg),
                    ("LArPedestalMC","/LAR/ElecCalibMC/Pedestal","LArPedestal",None),
                    ("LArNoiseMC","/LAR/ElecCalibMC/Noise","LArNoise",LArNoiseSymAlg),
                    ("LArfSamplMC","/LAR/ElecCalibMC/fSampl","LArfSampl",LArfSamplSymAlg),
                    ("LAruA2MeVMC","/LAR/ElecCalibMC/uA2MeV","LAruA2MeV", LAruA2MeVSymAlg),
                    ("LArMinBiasMC","/LAR/ElecCalibMC/MinBias","LArMinBias",LArMinBiasSymAlg),
                    ("LArMinBiasAverageMC","/LAR/ElecCalibMC/MinBiasAverage","LArMinBiasAverage",LArMinBiasAverageSymAlg)
                    ]

if larCondFlags.useMCShape():
    larCondDBFolders += [("LArShape32MC","/LAR/ElecCalibMC/Shape","LArShape",LArShapeSymAlg)]

include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg,LArBadFebCondAlg
conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannels"+LArDBConnection, className="CondAttrListCollection")
condSeq+=LArBadChannelCondAlg(ReadKey="/LAR/BadChannels/BadChannels")
conddb.addFolder(LArDB,"/LAR/BadChannels/MissingFEBs"+LArDBConnection, className="AthenaAttributeList")
condSeq+=LArBadFebCondAlg(ReadKey="/LAR/BadChannels/MissingFEBs")

condSeq+=LArBadFebCondAlg("LArKnownBadFebAlg",ReadKey="",WriteKey="LArKnownBadFEBs")
condSeq+=LArBadFebCondAlg("LArKnownMNBFebAlg",ReadKey="",WriteKey="LArKnownMNBFEBs")


## these may be conditional. 
if larCondFlags.hasMphys() :
  larCondDBFolders += [("LArMphysOverMcalMC","/LAR/ElecCalibMC/MphysOverMcal","LArMphysOverMcal",LArMPhysOverMcalSymAlg)]

# HV Scale Corr
if larCondFlags.hasHVCorr() :
  larCondDBFolders += [ ('LArHVScaleCorrComplete', '/LAR/ElecCalibMC/HVScaleCorr',"LArHVScaleCorr",None) ]

  
LArMCSymCondAlgDefault()
## fill them all 
for className,fldr,key,calg in larCondDBFolders:
  if calg is not None: 
    newkey=key+"Sym"
  else:
    newkey=key
  conddb.addFolder(LArDB,fldr+LArDBConnection, className=className)
  ## allow override
  larCondFlags.addTag(fldr,conddb)  
  if calg:
    condSeq+=calg(ReadKey=key,WriteKey=newkey)

## apply hierarchical tag                    
larCondFlags.addTag('/LAR/ElecCalibMC',conddb)
