#////////////////////////////////////////////////////////////////////
#
#  topOptions for PanTau:
#       Input : An ESD that contains the eflowRec container eflowObjects_tauMode and the tauRecContainer
#       Output: A TauD3PD that contains an enhanced tau block:
#               - tau_pantau block:
#                   These variables are available in official AOD
#
#               - tau_pantauFeature block:
#                   These variables are not available in the official AOD
#                   However, these are all the variables from the
#                   PanTau::TauSeed that are available
#                   These variables should be used for performance studies
#
#   This jobOption file will work when submitting GRID jobs!
#   Simply use the script 
#           submit-pathena-job_AOD_to_PanTauD3PD.sh
#   to submit the jobs for a single sample, or use
#           Launch_GridJobs.sh
#   to submit jobs for all desired samples
#
#////////////////////////////////////////////////////////////////////


#//////////////////////////////////
#   Input Files
#//////////////////////////////////

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
    #athenaCommonFlags.FilesInput=['/tmp/limbach/user.bwinter.TauPi0RecESD2ESDforD3PD_test.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_00.v00-00.130321120743/user.bwinter.000603.EXT0._02502.ESD.pool.root']
#    athenaCommonFlags.FilesInput=['/tmp/limbach/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_00/ESD.00999072._006001.pool.root.1']
    #athenaCommonFlags.FilesInput=['/tmp/limbach/user.bwinter.TauPi0RecESD2ESDforD3PD_test.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_00.v01-00.130322163340/user.bwinter.000611.EXT0._00030.ESD.pool.root']
    
    #lxplus428:
    athenaCommonFlags.FilesInput=['/tmp/limbach/user.bwinter.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_00.v00-01.130222100411/user.bwinter.000509.EXT0._01085.ESD.pool.root']
athenaCommonFlags.EvtMax=100  # number of events to process run on all file


#//////////////////////////////////
#   jobOption flags
#//////////////////////////////////

includePanTauInRecExCommon = False # Set to true for PanTau output in AOD

from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags
import AthenaCommon.SystemOfUnits as Units
PSB_Flags.Config_DoRunTimeHists = True # create root file with runtime hists from SeedBuilder
PSB_Flags.Config_EfoSel_DoEfoSelection = True # False is the default until proven 'True' is better
PSB_Flags.Config_EfoSel_PiZeroBDTThreshold = -0.04;
PSB_Flags.Config_Presel_DoPreselection = False

#tauRec vicinity check
PSB_Flags.Config_DoTauRecVicinityCheck      = False
PSB_Flags.Config_DeltaRTauRecVicinityCheck  = 1.2
PSB_Flags.Config_KeepOverlapTauRecVicinity  = False


#configure storage of output D3PD...
from PanTauAnalysis.PanTauDPDFlags import panTauDPDFlags
panTauDPDFlags.PanTauPerfD3PDName = "TauPerfTest_from__inputForTauPi0RecESD2ESDforD3PD_test.root"

#...add truth info in D3PD
from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock( True )
#rec.AutoConfiguration = [ "everything" ]

#//////////////////////////////////
#   Main Job Options
#//////////////////////////////////

# !!!                                            !!! #
# !!! You must not modify flags below this point !!! #
# !!!                                            !!! #

#Basic PanTau config
from PanTauAnalysis.PanTauFlags import panTauFlags
panTauFlags.ExtractAllFeatures.set_Value_and_Lock(True)

rec.readAOD      = False
rec.readESD      = True
rec.readRDO      = False
rec.doCBNT       = False
rec.doWriteESD   = False
rec.doWriteTAG   = False
rec.doPerfMon    = False
rec.doHist       = False

if includePanTauInRecExCommon:
    rec.doAOD.set_Value_and_Lock(True)
    rec.doWriteAOD.set_Value_and_Lock(True)
else:
    rec.doAOD.set_Value_and_Lock(False)
    rec.doWriteAOD.set_Value_and_Lock(False)

# commented out in AOD->D3PD
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr()


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# need to include PanTau explicitely, since it's "protected" by doAOD in RecExCommon
#if includePanTauInRecExCommon:
    #rec.UserAlgs=[ "PanTauAnalysis/PanTau_Main_TopOptions.py" ]
include( "RecExCommon/RecExCommon_topOptions.py" )

if not includePanTauInRecExCommon:
    #include("PanTauAnalysis/PanTau_Main_TopOptions.py")

    from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
    if rec.readESD():
        TruthTausToSG.truth_container="INav4MomTruthEvent"
    print("climbach: updated TruthTausToSG to be INav4MomTruthEvent")
    topSequence += TruthTausToSG()
    
    
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 1



#//////////////////////////////////
#   D3PD Output
#//////////////////////////////////

dpdname = panTauDPDFlags.PanTauPerfD3PDName()

from TauD3PDMaker.tauPerf import tauPerf
alg = tauPerf ( level = 10,
                   file=dpdname,
                   doPhys = True,
                   doCl = True,
                   doCell = False,
                   doPi0BonnDevVars = False,
                   doTrk = True,
                   doHad = True,
                   doOtherTrk = True,
                   tuplename = 'tau',
                   seq = topSequence,
                   D3PDSvc = 'D3PD::RootD3PDSvc',
                   doMETSkimVariables = False,
                   streamName = "TauPERF")
    

#//////////////////////////////////
#   Silence over abundant messages
#//////////////////////////////////
############################
#Silence Trig Navigation Warnings
from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
    ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
    ToolSvc.TrigDecisionTool.OutputLevel = ERROR
#############################
StoreGateSvc.OutputLevel = FATAL


#end of file

