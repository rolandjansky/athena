# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.HIJetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, HIJetUEMonitoring, JetKinematicHistos, JetContainerHistoFiller
from AthenaCommon.AppMgr import ToolSvc
from JetRec.JetRecFlags import jetFlags
from JetSelectorTools.JetSelectorToolsConf import JetCleaningTool
from RecExConfig.ObjKeyStore import cfgKeyStore

monitorTracks = cfgKeyStore.isInTransient('xAOD::JetContainer','AntiKt3PV0TrackJets')

cleaningTool = JetCleaningTool( "LooseBadJets" , CutLevel = "LooseBad")
jhm.addTool( cleaningTool )
ToolSvc += cleaningTool

def commonMonitoringTool(container, refcontainer="", pathSuffix=''):
    filler = JetContainerHistoFiller(container+pathSuffix+"HistoFiller",JetContainer = container, HistoDir=container+pathSuffix+'/')

    # Give a list of predefined tools from jhm or a combination of such tools
    filler.HistoTools = [
        # build a special kinematics histo tool plotting 2D hists :
        JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True) ,

        
        # Draw a set of histo for a particular jet selection :
        selectionAndHistos( "leadingjet" , [ "basickinematics", ] ),
        selectionAndHistos( "subleadingjet" , [ "basickinematics"] ),
        selectionAndHistos( "60000<pt<100000" , [ "allkinematics", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF"], "highpt_60_100" ),
        selectionAndHistos( "100000<pt<250000" , [ "allkinematics", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF"], "highpt_100_250" ),
        selectionAndHistos( "250000<pt<800000" , [ "allkinematics", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF"], "highpt_250_800" ),

        jhm.Width,

        # distances between 2 leading jets.
        jhm.leadingjetrel,
        ]

    if "HI" and not "Track" in container:
        filler.HistoTools += [
            
            # jet states
#            jhm.basickinematics_emscale,
#            jhm.basickinematics_constscale,


            # calo variables
#            jhm.NegativeE,
            jhm.EMFrac,
            jhm.HECFrac,
            jhm.Timing,
            jhm.LArQuality,

            # calo quality variables
            jhm.AverageLArQF,
            jhm.HECQuality,
            jhm.FracSamplingMax,
            jhm.FracSamplingMaxIndex,

            # energy per sampling
            jhm.PreSamplerB,
            jhm.EMB1,
            jhm.EMB2,
            jhm.EMB3,
            jhm.PreSamplerE,
            jhm.EME1,
            jhm.EME2,
            jhm.EME3,
            jhm.HEC0,
            jhm.HEC1,   
            jhm.HEC2,
            jhm.HEC3,
            jhm.FCAL0,
            jhm.FCAL1,
            jhm.FCAL2, 
            jhm.TileBar0,
            jhm.TileBar1,
            jhm.TileBar2,
            jhm.TileExt0,
            jhm.TileExt1,
            jhm.TileExt2,
            jhm.TileGap1,
            jhm.TileGap2,
            jhm.TileGap3,

############HI moments
            jhm.MaxOverMean,
            jhm.MaxConstituentET,
            jhm.JetUnsubtractedScaleMomentum_pt,
            jhm.JetUnsubtractedScaleMomentum_eta,
            jhm.JetUnsubtractedScaleMomentum_phi,
            jhm.JetUnsubtractedScaleMomentum_m,
            jhm.JetSubtractedScaleMomentum_pt,
            jhm.JetSubtractedScaleMomentum_eta,
            jhm.JetSubtractedScaleMomentum_phi,
            jhm.JetSubtractedScaleMomentum_m,
            ]

            # centrality
        filler.HistoTools += [jhm.centrality,]                                                          
        filler.HistoTools['centrality'].RefContainer = refcontainer

#        if jetFlags.useTracks:
#            filler.HistoTools += [
#                # track variables
#                jhm.tool("JVF[0]"),
#                jhm.SumPtTrkPt1000,
#                jhm.GhostTrackCount,
#                jhm.CHF,
#                ]
            
        
        if refcontainer:
            # efficiency
            filler.HistoTools += [jhm.effresponse,]                                                          
            filler.HistoTools['effresponse'].RefContainer = refcontainer

    #filler.OutputLevel =2 
    return filler



athenaMonTool = JetMonitoringTool(HistoTools = [  commonMonitoringTool( "AntiKt4HIJets" ), # if truth is present, we could add : , "AntiKt4TruthJets" ,                                                       
                                                  ],
                                  IntervalType = 6,) # 6 is 'Interval_t::run' interval
if monitorTracks :
     athenaMonTool.HistoTools += [ commonMonitoringTool( "AntiKt4HITrackJets" ) ]

ToolSvc += athenaMonTool

athenaMonTool_LB = JetMonitoringTool("JetMonitoring_LB", HistoTools = [  "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True) ] ,IntervalType = 2)

ToolSvc += athenaMonTool_LB

from AthenaMonitoring.DQMonFlags import DQMonFlags
if DQMonFlags.useTrigger() :
    athenaMonTool_trig = JetMonitoringTool("JetMonitoring_trig",HistoTools =
                                           [
                                               commonMonitoringTool( "AntiKt4HIJets", pathSuffix='_trig' ),
                                               ] , IntervalType = 6 )
    ToolSvc += athenaMonTool_trig
    athenaMonTool_trig.TrigDecisionTool =  getattr(ToolSvc, DQMonFlags.nameTrigDecTool().split('/')[-1])
    athenaMonTool_trig.TriggerChain =  "CATEGORY_monitoring_jet"
    #athenaMonTool_trig.TriggerChain =  "HLT_j25,HLT_j60,HLT_j200_jes_PS" 
#    athenaMonTool_trig.TriggerChain =  "j20 ion"
    #athenaMonTool_trig.OutputLevel = 2


def athenaMonitoringTools():
    if DQMonFlags.useTrigger():
        return [  athenaMonTool, athenaMonTool_trig ]
    else:
        return [  athenaMonTool ]
