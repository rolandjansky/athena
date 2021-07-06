# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.HIJetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller
from AthenaCommon.AppMgr import ToolSvc
from JetSelectorTools.JetSelectorToolsConf import JetCleaningTool
from RecExConfig.ObjKeyStore import cfgKeyStore
#New AthenaMonitoring filter tool to be added to filter out events in non-filled BCIDs
from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
monbadlb = GetLArBadLBFilterTool()

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
        selectionAndHistos( "60000<pt<400000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents", "FracSamplingMax"], "highpt_60_400" ),
        selectionAndHistos( "400000<pt<1000000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents",  "FracSamplingMax"], "highpt_400_1000" ),
        selectionAndHistos( "1000000<pt<2500000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents",  "FracSamplingMax"], "highpt_1000_2500" ),
        selectionAndHistos( "LooseBadJets" ,  [  "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True)]),
        selectionAndHistos( "1.0<eta<1.4" , [  "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", JetKinematicHistos("kinematicsTileGap",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True)], "eta_1_14" ),
        jhm.Width,

        # distances between 2 leading jets.
        jhm.leadingjetrel,
        ]

    if "HI" in container and "Track" not in container:
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
            jhm.N90Constituents,
            jhm.ptN,

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

#cbg
athenaMonTool.FilterTools += [ monbadlb ]

#ToolSvc += athenaMonTool

athenaMonTool_LB = JetMonitoringTool("JetMonitoring_LB", HistoTools = [  "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True) ] ,IntervalType = 2)

#cbg
athenaMonTool_LB.FilterTools += [ monbadlb ]

#ToolSvc += athenaMonTool_LB

from AthenaMonitoring.DQMonFlags import DQMonFlags
if DQMonFlags.useTrigger() :
    athenaMonTool_trig = JetMonitoringTool("JetMonitoring_trig",HistoTools =
                                           [
                                               commonMonitoringTool( "AntiKt4HIJets", pathSuffix='_trig' ),
                                               ] , IntervalType = 6 )
    #cbg
    athenaMonTool_trig.FilterTools += [ monbadlb ]
    #ToolSvc += athenaMonTool_trig
    athenaMonTool_trig.TrigDecisionTool =  getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
    athenaMonTool_trig.TriggerChain =  "CATEGORY_monitoring_jet"
    #athenaMonTool_trig.TriggerChain =  "HLT_j25,HLT_j60,HLT_j200_jes_PS" 
#    athenaMonTool_trig.TriggerChain =  "j20 ion"
    #athenaMonTool_trig.OutputLevel = 2


def athenaMonitoringTools():
    if DQMonFlags.useTrigger():
        return [  athenaMonTool, athenaMonTool_trig ]
    else:
        return [  athenaMonTool ]
