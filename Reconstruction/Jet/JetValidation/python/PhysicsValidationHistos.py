# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PyUtils import AthFile


af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0]) #opens the first file from the InputCollections list
af.fileinfos #this is a dict of dicts, take a look at what's available! Below are some examples:
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
beam_energy = af.fileinfos['beam_energy']
conditions_tag = af.fileinfos['conditions_tag'] #useful for figuring out which mc production this is
print "PhysicsValidationHistos: isMC=",isMC, " beam=",beam_energy," conditions_tag=",conditions_tag

def commonPhysValTool(container, refcontainer="", onlyKinematics = False, globalSelection= ""):
    containerfiller = JetContainerHistoFiller(container+"HistoFiller",JetContainer = container)
    filler = containerfiller
    
    if globalSelection !="":
        # here we create an intermediate selection tool, we'll pass it the histo builders
        # instead of passing them to containerfiller
        # Create the intermediate tool and attach it to containerfiller:
        containerfiller.HistoTools = [ selectionAndHistos( globalSelection,
                                                           [] , # no histo builders, will be set below
                                                           histoNameSuffix= "none", # same histo names as without global selection
                                                           )
                                       ]
        # attach the selection :
        filler = containerfiller.HistoTools[0]
        containerfiller.OutputLevel = 2        
        filler.OutputLevel = 2
    
    # Give a list of predefined tools from jhm or a combination of such tools
    filler.HistoTools = [
        # build a special tool without 2D hists :
        JetKinematicHistos("kinematics",PlotOccupancy=False, PlotAveragePt=False, PlotNJet=True) ,
        ]

    if onlyKinematics:
        # return now
        return containerfiller

    filler.HistoTools += [
        
        # Draw a set of histo for a particular jet selection :
        selectionAndHistos( "leadingjet" , [ "basickinematics", ] ),
        selectionAndHistos( "subleadingjet" , [ "basickinematics"] ),

        jhm.Width,

        # distances between 2 leading jets.
        jhm.leadingjetrel,
        ]

    if "Topo" in container or "PFlow" in container:
        filler.HistoTools += [

            # jet states
            jhm.basickinematics_emscale,
            jhm.basickinematics_constscale,

            # track variables
            jhm.tool("JVF[0]"),
            jhm.SumPtTrkPt500,
            jhm.tool("SumPtTrkPt500[0]"),
            jhm.tool("NumTrkPt500[0]"),
            jhm.tool("NumTrkPt1000[0]"),
            jhm.tool("TrackWidthPt1000[0]"),
            jhm.GhostTrackCount,
            jhm.GhostTruthAssociationFraction,
            jhm.GhostMuonSegmentCount,
            jhm.JVF,
            jhm.JVFCorr,
            jhm.Jvt,
            jhm.JvtRpt,

            # calo variables
            jhm.NegativeE,
            jhm.EMFrac,
            jhm.HECFrac,
            jhm.Timing,
            jhm.LArQuality,

            jhm.EMB2,
            jhm.EMB3,            
            jhm.EME2,
            jhm.EME3,
            jhm.HEC2,
            jhm.HEC3,
            jhm.FCAL0,
            jhm.FCAL1,
            jhm.TileBar0,
            jhm.TileBar1,
            jhm.TileExt0,
            jhm.TileExt1,

            ]
        
        if refcontainer:
            # efficiency
            filler.HistoTools += [jhm.effresponse,]
            filler.HistoTools['effresponse'].RefContainer = refcontainer

        if "Trimmed" in container:
            filler.HistoTools += [
                jhm.KtDR,
                jhm.Tau1,
                jhm.Tau2,
                jhm.Tau3,
                jhm.Tau1_wta,
                jhm.Tau2_wta,
                jhm.Tau3_wta,
                jhm.Tau21,
                jhm.Tau32,
                jhm.Tau21_wta,
                jhm.Tau32_wta,
                jhm.Dip12,
                jhm.Dip13,
                jhm.Dip23,
                jhm.DipExcl12,
                jhm.Split12,
                jhm.Split23,
                jhm.Split34,
                jhm.ZCut12,
                jhm.ZCut23,
                jhm.ZCut34,
                jhm.Angularity,
                jhm.PlanarFlow,
                jhm.Mu12,
                jhm.ECF1,
                jhm.ECF2,
                jhm.ECF3,
                jhm.ECF1_Beta2,
                jhm.ECF2_Beta2,
                jhm.ECF3_Beta2,
                jhm.C1,
                jhm.C2,
                jhm.D2,
                jhm.C1_Beta2,
                jhm.C2_Beta2,
                jhm.D2_Beta2,
                jhm.ThrustMin,
                jhm.ThrustMaj,
                jhm.FoxWolfram0,
                jhm.FoxWolfram1,
                jhm.FoxWolfram2,
                jhm.FoxWolfram3,
                jhm.FoxWolfram4,
                jhm.Sphericity,
                jhm.Aplanarity,
                jhm.PullMag,
                jhm.PullPhi,
                jhm.Pull_C00,
                jhm.Pull_C01,
                jhm.Pull_C10,
                jhm.Pull_C11,
                jhm.Charge,
                jhm.ShowerDeconstructionW,
                jhm.ShowerDeconstructionTop,
                jhm.Volatility,
                ]

    #filler.OutputLevel =2 
    return containerfiller

from RecExConfig.RecFlags import rec 
akt4refContainer = "AntiKt4TruthJets" if rec.doTruth() else ""

#globalSelection = "0.7<JVF[0]"
globalSelection = ""

##
# necessary updates for validation directories
###

athenaMonTool = JetMonitoringTool(HistoTools = [
    commonPhysValTool( "AntiKt4LCTopoJets", akt4refContainer ,globalSelection = globalSelection),
    commonPhysValTool( "AntiKt4EMTopoJets", akt4refContainer ,globalSelection = globalSelection),
#   the containers that are commented out are kept so as to make it possible to swicth them on in the future if needed
#   commonPhysValTool( "AntiKt10LCTopoJets" ),
#   commonPhysValTool( "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" ),
#   commonPhysValTool( "AntiKt2PV0TrackJets" ),
    commonPhysValTool( "AntiKt4EMPFlowJets" ),
    commonPhysValTool( "AntiKt4TruthJets" ),
#    commonPhysValTool( "AntiKt10TruthJets" ),
    ], IntervalType=8) # 8 == HistoGroupBase::all


if (isMC==False):
  athenaMonTool = JetMonitoringTool(HistoTools = [
    commonPhysValTool( "AntiKt4LCTopoJets", akt4refContainer ,globalSelection = globalSelection),
    commonPhysValTool( "AntiKt4EMTopoJets", akt4refContainer ,globalSelection = globalSelection),
#   the containers that are commented out are kept so as to make it possible to swicth them on in the future if needed
#    commonPhysValTool( "AntiKt10LCTopoJets" ),
#    commonPhysValTool( "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" ),
#    commonPhysValTool( "AntiKt2PV0TrackJets" ),
    commonPhysValTool( "AntiKt4EMPFlowJets" ),
    ], IntervalType=8) # 8 == HistoGroupBase::all

ToolSvc += athenaMonTool

def athenaPhysValTool():
    return athenaMonTool

