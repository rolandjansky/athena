import os
import sys
import shutil
import commands

from MuonAlignExample.GlobalChi2Align_IteratorClasses import GlobalChi2AlignIterator
from MuonAlignExample.GlobalChi2Align_MonitorClasses  import GlobalChi2Plotter
from MuonAlignExample.GlobalChi2Align_MonitorClasses  import TrendPlotter


#---------------------------
# Non-Default Data Examples
#---------------------------
from MuonAlignExample.DefaultFiles import DefaultFiles
#myfiles = DefaultFiles.manualFile(['/data.old/data10_7TeV.00152221.physics_MuonswBeam.merge.DESD_SGLMU.f238_m428','/data.old/data10_7TeV.00153565.physics_MuonswBeam.merge.DESD_SGLMU.f251_m467'],"terrier09","ESD","ATLAS-GEO-10-00-00","COMCOND-ES1PST-002-00","data10_7TeV.0015221.physics_MuonswBeam.merge.DESD_SGLMU.f238_m428")
#myfiles = DefaultFiles.default('15GeV_data','dataset')
myfiles = DefaultFiles.manualFile(["/tmp/roberth/group.perf-muons.00165591.physics_Muons.reco.TMU15GEV.20101116.s1_ESD"],'','ESD','','',840,'group.det-muon.data11_7TeV*physics_Muons.*.reco.20110609_ESD') 

#-------------------------
# Set iterator job flags
#-------------------------
iterator = GlobalChi2AlignIterator(logfile='joblog.txt')
iterator.readEventList    = False 
iterator.files            = myfiles
iterator.runargsJobOption = "runargs.ESDtoESD.py"
iterator.doAccumulate     = True
iterator.doSolve          = True
iterator.runMode          = 'pathena' #local, sge, lxbatch, pathena, prun
iterator.runLocation      = 'lxplus' # bnl,lxplus
iterator.dataType         = 'data'# MC,cosmic,data
iterator.MCmomentum       = 20
iterator.resumePathenaJob = False 
iterator.rootName         = 'L3_ZMUMUskim_data_batch_EE-BIS8'
iterator.inputAsztFile    = ''
iterator.asztRootName     = ''
iterator.doMonitoring     = False
#iterator.libDS            = "user.roberth.0610193906.769101.lib._001050"
iterator.libDS            = ""
iterator.official         = True
iterator.outputDatasetDef = "group.det-muon.ZMUMU.20110609_EE-BIS8_iter0_V25"
#iterator.outputDatasetDef = "group.det-muon.TMU15GEV.20101116_EE_iter3_V14"
#iterator.site             = 'ANALY_BNL_ATLAS_1'
iterator.site             = ''
iterator.solveLocally     = True
iterator.alignmentLevel   = 'L3'
iterator.alignmentJob     = 'L3Mdt_muidSA' # L3Mdt, L1_MSID, L3CSC
iterator.doEndcapOnly     = False 
iterator.doBarrelOnly     = False
iterator.nEventsPerJob    = -1 
iterator.nFilesPerJob     = 5
#iterator.nFilesPerJob     = 10
iterator.skipEvents       = 0 
iterator.errorScaleDb     = ''
iterator.tag              = '' #"NLooseMuon>0 && fabs(LooseMuonPt1)/1000>6 && fabs(LooseMuonEta1)>1 && LooseMuonMDTHits1>12 && (LooseMuonTGCHits1>0 || LooseMuonRPCHits1>0)"
#iterator.noBuild          = True
#iterator.inTarBall        = ""
#iterator.outTarBall       = "EE-BIS8_V14
iterator.firstIteration   = 0 
iterator.iterations       = 1

iterator.configRunMode()
iterator.run()

#-------------------------
# Monitoring here
#-------------------------
if iterator.doMonitoring:
    scriptPath = currentPath+"/scripts"
    outputPath = iterator.outputPath+"/pics"

    globchi2plot = GlobalChi2Plotter(scriptPath)
    globchi2plot.inputPath     = iterator.outputPath
    globchi2plot.outputPath    = outputPath
    globchi2plot.rootName      = iterator.rootName 
    globchi2plot.absoluteLines = True
    for i in range(iterator.firstIteration,
                   iterator.firstIteration+iterator.iterations):
        globchi2plot.iteration = i
        globchi2plot.run()

    if iterator.iterations>1 or iterator.firstIteration != 0:
        trendplot = TrendPlotter(scriptPath)
        trendplot.inputPath    = iterator.outputPath
        trendplot.outputPath   = outputPath
        trendplot.rootName     = iterator.rootName 
        trendplot.drawErrors   = True
        trendplot.combineTower = False
        trendplot.run()
 
