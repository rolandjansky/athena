# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller
from AthenaCommon.AppMgr import ToolSvc
from JetRec.JetRecFlags import jetFlags
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
        selectionAndHistos( "20000<pt<500000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents", "SumPtTrkPt500[0]", "FracSamplingMax"], "highpt_20_500" ),
        selectionAndHistos( "500000<pt<1000000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents", "SumPtTrkPt500[0]",  "FracSamplingMax"], "highpt_500_1000" ),
        selectionAndHistos( "1000000<pt<2000000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents", "SumPtTrkPt500[0]",  "FracSamplingMax"], "highpt_1000_2000" ),
        selectionAndHistos( "2000000<pt<8000000" , [ "allkinematics", "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", "AverageLArQF", "N90Constituents", "SumPtTrkPt500[0]",  "FracSamplingMax"], "highpt_2000_8000" ),
        selectionAndHistos( "LooseBadJets" ,  [  "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True)]),
        selectionAndHistos( "1.0<eta<1.4" , [  "ptN", "Timing", "EMFrac", "HECFrac", "LArQuality", JetKinematicHistos("kinematicsTileGap",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True)], "eta_1_14" ),

        jhm.Width,

        # distances between 2 leading jets.
        jhm.leadingjetrel,
        ]

    if "Topo" in container:
        filler.HistoTools += [
            
            # jet states
            jhm.basickinematics_emscale,
            jhm.basickinematics_constscale,


            # calo variables
            jhm.NegativeE,
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
            jhm.CentroidR,
            jhm.OotFracClusters5,
            jhm.OotFracClusters10,
            jhm.ptN,
            jhm.LeadingClusterCenterLambda,
            jhm.LeadingClusterSecondLambda,
            jhm.LeadingClusterSecondR,

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
            
            ]
        if jetFlags.useTracks:
            filler.HistoTools += [
                # track variables
                jhm.tool("JVF[0]"),
                jhm.SumPtTrkPt500,
                jhm.tool("SumPtTrkPt500[0]"),
                jhm.tool("NumTrkPt500[0]"),
                jhm.tool("NumTrkPt1000[0]"),
                jhm.tool("TrackWidthPt1000[0]"),
                jhm.GhostTrackCount,
                ]
            
        
        if refcontainer:
            # efficiency
            filler.HistoTools += [jhm.effresponse,]                                                          
            filler.HistoTools['effresponse'].RefContainer = refcontainer

    #filler.OutputLevel =2 
    return filler


def commonMonitoringTool_LB(container, refcontainer="", pathSuffix=''):
    filler = JetContainerHistoFiller(container+pathSuffix+"HistoFiller_LB",JetContainer = container, HistoDir=container+pathSuffix+'_LB/')

    # Give a list of predefined tools from jhm or a combination of such tools
    filler.HistoTools = [
        JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True) ,
            jhm.ptN,
            jhm.Timing,
            jhm.EMFrac,
            jhm.HECFrac,
            jhm.LArQuality,
        ]
    return filler


athenaMonTool = JetMonitoringTool(HistoTools = [  commonMonitoringTool( "AntiKt4LCTopoJets" ), # if truth is present, we could add : , "AntiKt4TruthJets" ,                                                  
                                                  commonMonitoringTool( "AntiKt4EMTopoJets" ),
                                                  commonMonitoringTool( "AntiKt10LCTopoJets" ),       
                                                  ],
                                  IntervalType = 6,) # 6 is 'Interval_t::run' interval
if monitorTracks :
     athenaMonTool.HistoTools += [ commonMonitoringTool( "AntiKt3PV0TrackJets" ) ]
#if jetFlags.useTracks:
#    athenaMonTool.HistoTools += [ commonMonitoringTool( "AntiKt3PV0TrackJets" ) ]

#cbg
athenaMonTool.FilterTools += [ monbadlb ]

ToolSvc += athenaMonTool

athenaMonTool_LB = JetMonitoringTool("JetMonitoring_LB", HistoTools = [commonMonitoringTool_LB("AntiKt4EMTopoJets") ], IntervalType = 2)

#cbg
athenaMonTool_LB.FilterTools += [ monbadlb ]

ToolSvc += athenaMonTool_LB

from AthenaMonitoring.DQMonFlags import DQMonFlags
if DQMonFlags.useTrigger() :
    athenaMonTool_trig = JetMonitoringTool("JetMonitoring_trig",HistoTools =
                                           [
                                               commonMonitoringTool( "AntiKt4LCTopoJets", pathSuffix='_trig' ),
                                               commonMonitoringTool( "AntiKt4EMTopoJets", pathSuffix='_trig' ),
                                               commonMonitoringTool( "AntiKt10LCTopoJets", pathSuffix='_trig' )
                                               ] , IntervalType = 6 )
    #cbg
    athenaMonTool_trig.FilterTools += [ monbadlb ]
    ToolSvc += athenaMonTool_trig
    athenaMonTool_trig.TrigDecisionTool =  getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
    athenaMonTool_trig.TriggerChain =  "CATEGORY_monitoring_jet"
    #athenaMonTool_trig.TriggerChain =  "HLT_j25,HLT_j60,HLT_j200_jes_PS" 
    #athenaMonTool_trig.OutputLevel = 2


def athenaMonitoringTools():
    if DQMonFlags.useTrigger():
        return [  athenaMonTool, athenaMonTool_LB, athenaMonTool_trig ]
    else:
        return [  athenaMonTool, athenaMonTool_LB ]
