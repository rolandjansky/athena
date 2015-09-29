# Inspired by PhysVal_jobOptions.py

# Set up the reading of the input xAOD:
FNAME = "AOD.pool.root"
#include( "AthenaPython/iread_file.py" )
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=[FNAME]


import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "PhysVal"
topSequence += monMan



from JetMonitoring.JetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller

# -------------------------------------
# Configuration example.
# Build histograms in the context of a Physics Validation job.
# Start with a high-level histo filler for a JetContainer
akt4Filler = JetContainerHistoFiller("AntiKt4LCTopoJetsMon",JetContainer = "AntiKt4LCTopoJets")

akt4Filler.HistoTools += [
    # Give the tool the list of histo building tools

    # **********************
    # Low-level configuration examples (not expected to be used in practice, just to
    # show the exact low level config).
    # build a specialized kinematic histos tool (this is probably redundant w.r.t attribute tools)
    JetKinematicHistos("alljetkine",PlotOccupancy=True, PlotAveragePt=True, PlotNJet=True) ,
    
    
    # Or a JetAttributeHisto  :
    JetAttributeHisto(  HistoDef = HistoDefinitionTool("BchCorrCellDef", title="Jet Bad channel correction;BchCorrCell", nbinsx=60,xlow=0,xup=1.0),
                        AttributeTypes=["float"] , AttributeNames=["BchCorrCell"]),
    
    # **********************

    
    # **********************
    # In practice, we can use  predefined tools and high-level configuration helpers to combine them.
    # Predefined tools are centralized in the JetHistoManager object : jhm

    jhm.Width,          # -> Width attribute
    jhm.leadingjetrel,  # -> relation (distances, ration) between leading jets. 
    # or, equivalently ask with the tool() method (needed when the tool name contains special characters)
    jhm.tool("JVF[0]"), # JVF attribute. Plot only first value of the JVF vector.

    # can combine 2 known 1D attribute histos into a 2D histo
    jhm.tool("EMFrac:NegativeE"),
    
    
    # To draw a set of histos for a particular jet selection,
    # combine a selection tool with a  list of low-level histogram tools
    #  - selection is either a name of a predefined tool  or a HistosForJetSelection instance
    #  - similarly, list contains names or tool instances
    selectionAndHistos( "leadingjet" , [ "basickinematics", "NegativeE", "JVF[0]:Width", jhm.Width] ),
    selectionAndHistos( "subleadingjet" , [ "basickinematics", ] ),
    selectionAndHistos( jhm.alljets , [ jhm.Timing ] ),
    

    # Other types of selection with simple expression.
    # Only jets satisfying the expression will fill the histos
    # expresion must be in the form : '1.23<var<3.45'
    # 'var' must be a jet attribute (float, int or vector<float>)
    selectionAndHistos( "-1<eta<1" , [ "basickinematics", ] ),
    selectionAndHistos( "eta<1" , [ "basickinematics", ] ),
    selectionAndHistos( "JVF[0]<0.8" , [ "basickinematics", "JVF[0]"] ),

    ]


jetMonTool = JetMonitoringTool(HistoTools = [akt4Filler])
ToolSvc += jetMonTool
monMan.AthenaMonTools += [  jetMonTool ]




from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]


from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
