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
#myfiles = DefaultFiles.default('15GeV_data','files')
myfiles = DefaultFiles.default('SGLMU_repro09_v01_data','files')
#myfiles = DefaultFiles.manualFile(["/tmp/roberth/group.perf-muons.00165591.physics_Muons.reco.TMU15GEV.20101116.s1_ESD"],'','ESD','','',840,'group.det-muon*physics_Muons.reco.20101119_EEalign_V6_ESD') 

#-------------------------
# Set iterator job flags
#-------------------------
iterator = GlobalChi2AlignIterator(logfile='joblog.txt')
iterator.readEventList    = False
iterator.files            = myfiles
iterator.runargsJobOption = "runargs.ESDtoESD.py"
iterator.doAccumulate     = True 
iterator.doSolve          = True
iterator.runMode          = 'local' #local, sge, lxbatch, pathena
iterator.runLocation      = 'lxplus' # bnl,lxplus
iterator.dataType         = 'data'# MC,cosmic,data
iterator.MCmomentum       = 20
iterator.resumePathenaJob = False 
iterator.rootName         = 'L1_1file_noscat'
iterator.inputAsztFile    = '' # use for L3 only
iterator.asztRootName     = ''
iterator.doMonitoring     = False
iterator.outputDatasetDef = '' 
#iterator.site             = 'ANALY_BNL_ATLAS_1'
iterator.site             = ''
iterator.solveLocally     = True
iterator.alignmentLevel   = 'L1'
iterator.alignmentJob     = 'L1_MSID' # L3Mdt, L1_MSID, L3CSC
iterator.doEndcapOnly     = False
iterator.doBarrelOnly     = False
iterator.nEventsPerJob    = -1 
iterator.nFilesPerJob     = 5
#iterator.nFilesPerJob     = 10
#iterator.skipEvents       = 132
iterator.nCPUs            = 1 
iterator.errorScaleDb     = ''
iterator.tag              = '' #"NLooseMuon>0 && fabs(LooseMuonPt1)/1000>6 && fabs(LooseMuonEta1)>1 && LooseMuonMDTHits1>12 && (LooseMuonTGCHits1>0 || LooseMuonRPCHits1>0)"

iterator.firstIteration   = 2
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
 
