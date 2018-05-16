# inputFile = '/net/s3_datac/dzhang/SAMPLES/R20/MC15/mc15_13TeV.402211.MGPy8EG_A14N_C1C1_800_10_LLE12k.merge.AOD.e4097_a766_a768_r6282/AOD.05819610._000001.pool.root.1'
# 
# Input file
inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zmumu/147807/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114._000187.1";
#inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";
#inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zee/147806/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
#inputFile = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_ggH125_gamgam/160009/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
# inputFile = "/afs/cern.ch/user/k/kaplan/public/DAOD_SUSY5.output.pool.root"

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections= [ inputFile ]

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
IsoCorrectionTool = CP__IsolationCorrectionTool ("NewLeakageCorrTool",IsMC = isMC)
ToolSvc += IsoCorrectionTool

from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

from CaloIdentifier import SUBCALO
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
CaloIsoTool = xAOD__CaloIsolationTool("CaloIsoTool")
CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.NewLeakageCorrTool
CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
CaloIsoTool.UseEMScale  = True
CaloIsoTool.saveOnlyRequestedCorrections = True
# CaloIsoTool.OutputLevel = 2
ToolSvc += CaloIsoTool

import ROOT, cppyy
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone20]
deco_prefix = 'MUON1_'


algseq = CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.IsolationTool_AthTest("myIsolationToolTest")
algseq.myIsolationToolTest.TrackIsolationTool = TrackIsoTool
algseq.myIsolationToolTest.CaloIsolationTool = CaloIsoTool
algseq.myIsolationToolTest.ptcones = deco_ptcones
algseq.myIsolationToolTest.topoetcones = deco_topoetcones
algseq.myIsolationToolTest.Prefix = deco_prefix
algseq.myIsolationToolTest.TargetContainer = 'Muons'
algseq.myIsolationToolTest.OutputLevel = 2

theApp.EvtMax=10
