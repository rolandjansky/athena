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
mysfx="H6PROD.QGSPBERT.vv06.100004.20072010.test5c"
from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCDeadMaterial
GetLCDM = GetLCDeadMaterial("GetLCDM")
GetLCDM.isTestbeam = True
GetLCDM.NormalizationTypeForFit = "Const"
GetLCDM.NormalizationTypeForMinim = "Lin"
GetLCDM.HadDMCoeffInputFile = "CaloHadDMCoeff_H6_init_v2.txt"
#GetLCDM.HadDMCoeffInputFile = "CaloLocalHadCoeff_output_H6PROD.QGSPBERT.vv07.100004.11072010.test1a.txt"
GetLCDM.HadDMCoeffOutputFile = "CaloLocalHadCoeff_output_"+mysfx+".txt"
GetLCDM.ReportProfiles = "report_CaloLocalHadCoeff_profiles_"+mysfx+".ps"
GetLCDM.ReportMinimization = "report_CaloLocalHadCoeff_minim_"+mysfx+".ps"
GetLCDM.ReportCheck = "report_CaloLocalHadCoeff_check_"+mysfx+".ps"
GetLCDM.DoFit = True
GetLCDM.DoMinimization = True
GetLCDM.DoPool = True
GetLCDM.DoCheck = True
 
GetLCDM.CorrectionKey="HadDMCoeff2"

#
#GetLCDM.InputRootFiles =  ["/ltmp/gdp/h6prod/DeadMaterialTree/bak1/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000040._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/bak1/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000042._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000043._sum.root"]

#GetLCDM.InputRootFiles =   ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000040_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000042_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000043_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000044_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000049_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000050_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000045_v3c._sum.root"]

#GetLCDM.InputRootFiles =  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000040_v3c.OldMaker1._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000042_v3c.OldMaker1._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000043_v3c.OldMaker1._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000044_v3c.OldMaker1._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000049_v3c.OldMaker1._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000050_v3c.OldMaker1._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000045_v3c.OldMaker1._sum.root"]

GetLCDM.InputRootFiles =  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000040_v3d._sum.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000042_v3d._sum.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000043_v3d._sum.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.jid000044_v3d._sum.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000049_v3d._sum.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.jid000050_v3d._sum.root"]
GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.jid000045_v3d._sum.root"]


# === ftfpbert ---
#GetLCDM.InputRootFiles =  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.ftfpbert.jid000054_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.ftfpbert.jid000055_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.ftfpbert.jid000056_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piminus_logE.ftfpbert.jid000058_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.ftfpbert.jid000059_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.ftfpbert.jid000060_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.ftfpbert.jid000061_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.pizero_logE.ftfpbert.jid000062_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.ftfpbert.jid000063_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.ftfpbert.jid000064_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.ftfpbert.jid000065_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.ftfpbert.jid000066_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.ftfpbert.jid000068_v3c._sum.root"]
#GetLCDM.InputRootFiles +=  ["/ltmp/gdp/h6prod/DeadMaterialTree/LCDeadMaterialTree_h6prod.100004.piplus_logE.ftfpbert.jid000069_v3c._sum.root"]


topSequence += GetLCDM


# Number of events to be processed (default is 10)
theApp.EvtMax = 1

AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA","CaloLocalHadCoeff.dmcorr."+mysfx+".pool.root")
# List of objects container type#key#foldername
myOCA.ObjectList = ["CaloLocalHadCoeff#HadDMCoeff2#/CALO/HadCalibration2/CaloDMCorr2"]
myOCA.WriteIOV   = True
#myOCA.IOVTagList = ["CaloHadDMCorr2-CSC05-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-MC09V-BERT" ]

#myOCA.IOVTagList = ["CaloHadDMCorr2-H6-2004-01-FTFP-BERT" ]
myOCA.IOVTagList = ["CaloHadDMCorr2-H6-2004-01-QGSP-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-GEO08-QGSP-BERT" ]
#myOCA.IOVTagList = ["CaloHadDMCorr2-GEO08-FTFP-BERT" ]
myOCA.OutputLevel= DEBUG

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = Lvl.INFO

DetectorStore = svcMgr.DetectorStore
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = Lvl.INFO

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel = Lvl.INFO
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myH60401DmFTFPBERT200.db;dbname=OFLP200"
svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myH60401DmQGSPBERT200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo08DmQGSPBERT200.db;dbname=OFLP200"
#svcMgr.IOVDbSvc.dbConnection = "sqlite://X;schema=myGeo08DmFTFPBERT200.db;dbname=OFLP200"

