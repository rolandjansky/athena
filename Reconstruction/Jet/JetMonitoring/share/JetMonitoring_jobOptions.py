# Inspired by PhysVal_jobOptions.py

jetMonMan = AthenaMonManager( "JetMonManager",
                           FileKey = DQMonFlags.monManFileKey(),
                           Environment = DQMonFlags.monManEnvironment(),
                           ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                           DataType = DQMonFlags.monManDataType() )
topSequence += jetMonMan
 
 
from JetMonitoring.JetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller

## # -------------------------------------
## # Configuration example.
## # Build histograms in the context of a Physics Validation job.
## # Start with a high-level histo filler for a JetContainer
## akt4Filler = JetContainerHistoFiller("AntiKt4LCTopoJetsMon",JetContainer = "AntiKt4LCTopoJets")

## akt4Filler.HistoTools += [
##     # Give the tool the list of histo building tools

##     # **********************
##     # Low-level configuration examples (not expected to be used in practice, just to
##     # show the exact low level config).
##     # build a specialized kinematic histos tool (this is probably redundant w.r.t attribute tools)
##     JetKinematicHistos("alljetkine",PlotOccupancy=True, PlotAveragePt=True, PlotNJet=True) ,
    
    
##     # Or a JetAttributeHisto  :
##     ## JetAttributeHisto(  HistoDef = HistoDefinitionTool("BchCorrCell", title="Jet Bad channel correction;BchCorrCell", nbinsx=60,xlow=0,xup=1.0),
##     ##                     AttributeTypes=["float"] , AttributeNames=["BchCorrCell"]),
    
##     # **********************

    
##     # **********************
##     # In practice, we can use  predefined tools and high-level configuration helpers to combine them.
##     # Predefined tools are centralized in the JetHistoManager object : jhm

##     jhm.Width,          # -> Width attribute
##     jhm.leadingjetrel,  # -> relation (distances, ration) between leading jets. 
##     # or, equivalently ask with the tool() method (needed when the tool name contains special characters)
##     jhm.tool("JVF[0]"), # JVF attribute. Plot only first value of the JVF vector.

##     # can combine 2 known 1D attribute histos into a 2D histo
##     jhm.tool("EMFrac:NegativeE"),
    
    
##     # To draw a set of histos for a particular jet selection,
##     # combine a selection tool with a  list of low-level histogram tools
##     #  - selection is either a name of a predefined tool  or a HistosForJetSelection instance
##     #  - similarly, list contains names or tool instances
##     selectionAndHistos( "leadingjet" , [ "basickinematics", "NegativeE", "JVF[0]:Width", jhm.Width] ),
##     selectionAndHistos( "subleadingjet" , [ "basickinematics", ] ),
##     selectionAndHistos( jhm.alljets , [ jhm.Timing ] ),
    

##     ]


## jetMonTool = JetMonitoringTool(HistoTools = [akt4Filler],
##                                IntervalType = 6 # 6 is 'Interval_t::run' interval
##                                )

from JetMonitoring.JetMonitoringHistos import athenaMonitoringTool

jetMonTool = athenaMonitoringTool()
ToolSvc += jetMonTool

jetMonMan.AthenaMonTools += [  jetMonTool ]
