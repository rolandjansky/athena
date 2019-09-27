import AthenaPoolCnvSvc.ReadAthenaPool
import sys

if 'EvtMax' in dir():
    theApp.EvtMax = EvtMax
else:
    theApp.EvtMax = -1

if 'TestMemory' in dir():
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring         = True
    jobproperties.PerfMonFlags.doDetailedMonitoring = True
    jobproperties.PerfMonFlags.doExtraPrintouts     = True
    jobproperties.PerfMonFlags.doFullMon            = True

if 'printAuxVars' not in dir():
    printAuxVars = False

if 'weightsPathElec' not in dir():
    weightsPathElec = None

if 'weightsPathMuon' not in dir():
    weightsPathMuon = None


#------------------------------------------------------------------------------
import os 

if 'inputDir' in dir():
    for f in os.listdir(inputDir):
        svcMgr.EventSelector.InputCollections += ['%s/%s' %(inputDir, f)]

elif os.path.isdir('input'):
    for f in os.listdir('input'):
        svcMgr.EventSelector.InputCollections += ['input/%s' %(f)]

else:
    print 'Missing input files - OK for pathena'

#------------------------------------------------------------------------------
if not hasattr(svcMgr, 'AthenaEventLoopMg'):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    svcMgr += AthenaEventLoopMgr()

svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

if 'dumpSG' in dir():
    StoreGateSvc = Service('StoreGateSvc')
    StoreGateSvc.Dump = dumpSG

theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]

svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = False
svcMgr.ChronoStatSvc.PrintEllapsedTime = False
svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1
theApp.AuditAlgorithms = True

#------------------------------------------------------------------------------  
def getDecorateIso(lepton_name, track_jet_name):

    alg = CfgMgr.Prompt__DecoratePromptLepton('%s_decoratePromptLeptonIso' %lepton_name)

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'

    alg.OutputLevel           = DEBUG
    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2017-10-27/%s/PromptLeptonIso' %part_type
    alg.MethodTitleMVA        = 'BDT_%s_PromptLeptonIso' %part_type
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.BDTName               = 'PromptLeptonIso'
    alg.PrintTime             = False
    alg.PrintAuxVars          = printAuxVars

    if weightsPathElec != None and part_type == 'Electron':
        alg.ConfigPathOverride  = weightsPathElec
        log.info('%s: use command line config path override: %s' %(part_type, weightsPathElec))

    if weightsPathMuon != None and part_type == 'Muon':
        alg.ConfigPathOverride  = weightsPathMuon
        log.info('%s: use command line config path override: %s' %(part_type, weightsPathMuon))

    alg.StringIntVars         = ['TrackJetNTrack',
                                 'sv1_jf_ntrkv']
    alg.StringFloatVars       = ['ip2',
                                 'ip3',
                                 'LepJetPtFrac',
                                 'DRlj',
                                 'TopoEtCone30Rel',
                                 'PtVarCone30Rel']

    print alg
    return alg

#------------------------------------------------------------------------------  
def getDecorateVeto(lepton_name, track_jet_name):

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLeptonVeto - unknown lepton type: "%s"' %lepton_name)

    BDT_name = 'PromptLeptonVeto'

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('Decorate%s - unknown track-jet collection: "%s"' %(BDT_name, track_jet_name))

    alg = CfgMgr.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.OutputLevel           = DEBUG
    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2017-10-27/%s/%s' %(part_type, BDT_name)
    alg.MethodTitleMVA        = 'BDT_%s_%s' %(part_type, BDT_name)  
    alg.BDTName               = '%s' %BDT_name
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack']
    alg.StringFloatVars       = ['rnnip',
                                 'DL1mu',
                                 'PtRel',
                                 'PtFrac',
                                 'DRlj',
                                 'TopoEtCone30Rel',
                                 'PtVarCone30Rel']

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))
    print alg

    return alg

#------------------------------------------------------------------------------  
def getDecorateNonPromptVertex(lepton_name):

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('getDecorateNonPromptVertex - unknown lepton type: "%s"' %lepton_name)


    from AthenaCommon.AppMgr import ToolSvc
    from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder
    BJetSVFinderTool          = InDetVKalVxInJetFinder("BJetSVFinder")
    ToolSvc                  += BJetSVFinderTool

    from InDetSVWithMuonTool.SecVrtWithMuonFinder import SecVrtWithMuonFinder
    LepSVFinderTool           = SecVrtWithMuonFinder("LepSVFinder")
    LepSVFinderTool.OutputLevel = 2
    LepSVFinderTool.FillHist  = True
    ToolSvc                  += LepSVFinderTool
    #svcMgr.THistSvc.Output   += ["file1 DATAFILE='myfile.root' OPT='RECREATE'"] # Add output hist
    jps.AthenaCommonFlags.HistOutputs = ["file1:myfile.root"]

    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    LepVertexFitterTool                  = CfgMgr.Trk__TrkVKalVrtFitter("MuonVertexFitterTool")
    LepVertexFitterTool.Extrapolator     = "Trk::Extrapolator/AtlasExtrapolator"
    LepVertexFitterTool.AtlasMagFieldSvc = "AtlasFieldSvc"
    ToolSvc += LepVertexFitterTool
                                               
    alg = CfgMgr.Prompt__NonPromptLeptonVertexingAlg('%s_decorate%s' %(lepton_name, 'Vertex'))

    alg.OutputLevel           = DEBUG
    alg.LeptonContainerName   = lepton_name
    alg.PrintTime             = True
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.ToolPostfix           = 'InDetVKalVxInJetTool'
    alg.SVLengthName          = 'SVlength'
    alg.SVMassName            = 'SVmass'
    alg.SVmuDRName            = 'SVmuDR'
    alg.IndexVectorName       = 'SecondaryVertexIndexVector'
    alg.LepSVFinderTool       = LepSVFinderTool
    alg.VertexFitterTool      = LepVertexFitterTool

    log.info('getDecorateNonPromptVertex - prepared decorate second vertexing information algorithm for: %s' %(lepton_name))
    print alg

    return alg

#------------------------------------------------------------------------------  
algSeq = CfgMgr.AthSequencer('AthAlgSeq')

if 'TestPythonConfig' in dir():
    from LeptonTaggers import LeptonTaggersConfig

    algSeq += LeptonTaggersConfig.GetDecoratePromptLeptonAlgs()
    algSeq += LeptonTaggersConfig.GetDecoratePromptTauAlgs()

else:
    #algSeq += getDecorateVeto('Electrons', 'AntiKt4PV0TrackJets')
    #algSeq += getDecorateVeto('Muons',     'AntiKt4PV0TrackJets')

    #algSeq += getDecorateIso('Electrons', 'AntiKt4PV0TrackJets')
    #algSeq += getDecorateIso('Muons',     'AntiKt4PV0TrackJets')
    algSeq += getDecorateNonPromptVertex('Muons')



