# File: CaloLocalHadCalib/share/GetLCDeadMaterial_jobOptions.py
# Created: Febryary 2009, G. Pospelov
# Purpose: Run postprocessing algorithms for the special DeadMaterialTree
#

#
# set inFileName and outFileName to convert TProfile2D classification to pool
#
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
import IOVDbSvc.IOVDb

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###############################################################################
#
###############################################################################
#mysfx="ATLAS.QGSPBERT.MC11.SINGLEPI.20062011.v1"
mysfx="mc10_7TeV.107410-107412.r2424.piplusminuszero.LCWeightsDM.16652.v2" # 15.06.2011
from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCDeadMaterial
GetLCDM = GetLCDeadMaterial("GetLCDM")
GetLCDM.HadDMCoeffInputFile = "CaloHadDMCoeff_init_v2.txt"
#GetLCDM.HadDMCoeffInputFile = "CaloLocalHadCoeff_output_ATLAS.QGSPBERT.MC10.SINGLEPI.21102010.v8.txt"
GetLCDM.HadDMCoeffOutputFile = "CaloLocalHadCoeff_output_"+mysfx+".txt"
GetLCDM.ReportProfiles = "report_CaloLocalHadCoeff_profiles_"+mysfx+".ps"
GetLCDM.ReportMinimization = "report_CaloLocalHadCoeff_minim_"+mysfx+".ps"
GetLCDM.ReportCheck = "report_CaloLocalHadCoeff_check_"+mysfx+".ps"
GetLCDM.DoFit = True
GetLCDM.DoMinimization = True
GetLCDM.DoPool = True
GetLCDM.DoCheck = True
# new feature to run on dmtrees from di-jets
#GetLCDM.NormalizationTypeForFit="const"
#GetLCDM.NormalizationTypeForMinim="const"
#GetLCDM.ClassificationType = "particleid"

GetLCDM.CorrectionKey="HadDMCoeff2"

#GetLCDM.InputRootFiles =  ["/ltmp/gdp/LCWeights/def1/user.pospelov.mc10_7TeV.107412.singlepiminus.LCWeights.16016.def1._sum.dmc.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/LCWeights/def1/user.pospelov.mc10_7TeV.107412.singlepizero.LCWeights.16016.def1._sum.dmc.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/LCWeights/def1/user.pospelov.mc10_7TeV.107412.singlepiplus.LCWeights.16016.def1._sum.dmc.root"]
#GetLCDM.InputRootFiles =  ["/ltmp/gdp/LCWeights/def2/user.pospelov.mc10_7TeV.107412.singlepiminus.LCWeights.16016.def2._sum.dmc.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/LCWeights/def2/user.pospelov.mc10_7TeV.107412.singlepizero.LCWeights.16016.def2._sum.dmc.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/LCWeights/def2/user.pospelov.mc10_7TeV.107412.singlepiplus.LCWeights.16016.def2b._sum.dmc.root"]
GetLCDM.InputRootFiles =  ["/ltmp/gdp/aaa/user.pospelov.mc10_7TeV.107410.singlepiplus.r2424.LCWeights.16652.v1._sum.dmc.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/aaa/user.pospelov.mc10_7TeV.107411.singlepiminus.r2424.LCWeights.16652.v1._sum.dmc.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/aaa/user.pospelov.mc10_7TeV.107412.singlepizero.r2424.LCWeights.16652.v1._sum.dmc.root"]
#GetLCDM.MaxEventsPerFile = 500000

topSequence += GetLCDM


# Number of events to be processed (default is 10)
theApp.EvtMax = 1

AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
#myOCA=OutputConditionsAlg("myOCA","CaloLocalHadCoeff.dmcorr."+mysfx+".pool.root")
myOCA=OutputConditionsAlg("myOCA",mysfx+".pool.root")
# List of objects container type#key#foldername
myOCA.ObjectList = ["CaloLocalHadCoeff#HadDMCoeff2#/CALO/HadCalibration2/CaloDMCorr2"]
myOCA.WriteIOV   = True
#myOCA.IOVTagList = ["CaloHadDMCorr2-CSC05-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-MC09V-BERT" ]

#myOCA.IOVTagList = ["CaloHadDMCorr2-GEO08-QGSP-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-GEO08-FTFP-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-GEO16-QGSP-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-GEO16-QGSP-BERT-Side" ]
myOCA.IOVTagList = ["CaloHadDMCorr2-GEO180100-QGSP-BERT" ]
myOCA.OutputLevel= DEBUG

AthenaPoolCnvSvc.PoolAttributes += [ "STREAM_MEMBER_WISE = '0'" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = Lvl.INFO

DetectorStore = svcMgr.DetectorStore
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = Lvl.INFO

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel = Lvl.INFO
svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo18DmQGSPBERT200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo08DmFTFPBERT200.db;dbname=OFLP200"

