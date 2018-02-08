from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.AuditorSvc.Auditors += ["ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#----------------------------
# Input Dataset
#----------------------------
import os
from glob import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = glob("/tmp/" + os.environ['USER'] + "/" + "RDO*.root*")
#athenaCommonFlags.FilesInput = ['/afs/cern.ch/work/c/ctreado/RDOAnalysis/datasets/mc15_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e4032_s2608_s2183_r6790_tid05706569_00/RDO.05706569._000011.pool.root.1']
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

#theApp.EvtMax = -1
theApp.EvtMax = 100

#----------------------------
# Message Service
#----------------------------
# set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

include("RDOAnalysis/RDOAnalysis_commonOptions.py")

