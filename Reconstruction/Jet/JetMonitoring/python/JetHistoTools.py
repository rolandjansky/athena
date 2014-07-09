# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, HistosForJetSelection, LeadingJetsRelations, EfficiencyResponseHistos


from JetMonitoring.JetHistoManager import jetHistoManager as jhm
from JetMonitoring.JetAttributeHistoManager   import attributeHistoManager
from JetMonitoring.HistoDefinitionHelpers import createHistoDefTool as hdef



# The dict below defines default specification to plot Jet attributes.
# Specifications are given in a 1-line compact format.
# They are then automatically converted to proper JetAttributeHisto tools below.
compactSpecification = {

    # 1D Histo format is
    # "histoname" : ( binning, attributeInfo )
    # where
    #  - binning is ("title;labelx;labely", nbins, xlow, xup) as in TH1 ctor
    #  - attributeInfo is ("attribute", "attribute type") or ("attribute", "attribute type", "gev")    
    # 
    "pt"               : (("Jet Pt;Pt [MeV];", 100, 0, 200) ,     ("pt","float","gev" ) ),    
    "Width"            : (("Jet Width;Width;", 50, 0, 1.0) ,         ("Width","float" ) ),
    "Width15"          : (("Jet Width;Width;", 50, 0, 1.5) ,         ("Width","float" ) ),
    "EMFrac"           : (("EM Fraction;EM fraction;", 50, -0.1, 1.4),  ("EMFrac", "float") ),
    "HECFrac"          : (("HEC Fraction;HEC fraction;", 50, -0.1, 1.4),  ("HECFrac", "float") ),
    "Timing"           : (("Jet Time info;Time;", 40, -20, 20) ,     ("Timing", "float") ),    
    "NegativeE"        : (("Negative E in Jet;Energy;", 80, -10, 0),  ("NegativeE", "float", "gev") ),
    "LArQuality"       : (("LAr quality;Energy;", 50, -0.4, 1.2),  ("LArQuality", "float") ),
    
    "GhostTruthCount"  : (("Number of associate truth part;Number;", 60,0,60) , ("GhostTruthCount", "int")), 
    "GhostTrackCount"  : (("Number of associate tracks;Number;", 60,0,60), ("GhostTrackCount", "int") ), 
    "JVF"              : (("Jet Vertex Fraction;JVF;", 60,0,1.), ("JVF", "vector<float>") ), 
    "JVF[0]"           : (("Jet Vertex Fraction;JVF;", 60,0,1.), ("JVF[0]", "vector<float>") ), 
    "SumPtTrkPt1000"   : (("Sum Pt of all tracks above 1GeV:SumPt;", 100,0,200), ("SumPtTrkPt1000", "vector<float>", "gev") ),
    
    "SumPtTrkPt1000[0]": (("Sum Pt of tracks from PV0 above 1GeV:SumPt;", 100,0,200), ("SumPtTrkPt1000[0]", "vector<float>", "gev") ),

    "ActiveArea"       : (("Active Area;Area;", 80, 0, 0.8), ("ActiveArea", "float") ),
    "ActiveArea15"     : (("Active Area;Area;", 80, 0, 1.5), ("ActiveArea", "float") ),

    "BchCorrDotx"      : (("BchCorrDotx:BchCorrDotx;",50,0,1), ("BchCorrDotx", "float") ),
    "BchCorrCell"      : (("BchCorrCell:BchCorrCell;",50,0,1), ("BchCorrCell", "float") ),
    
    
    # 2D Histo format is
    # "histoname" : ( binning, attributeInfo1, attributeInfo2 )
    # where
    #  - binning is ("title;labelx;labely", nbins, xlow, xup, nbinsy, ylow, yup) as in TH2 ctor
    #  - attributeInfo is ("attribute", "attribute type") or ("attribute", "attribute type", "gev")    

    }

# then fill the pers calo sampling attributes one by one :
caloSamples =["PreSamplerB", "EMB1", "EMB2", "EMB3", "PreSamplerE", "EME1", "EME2", "EME3", "HEC0", "HEC1", "HEC2", "HEC3", "TileBar0", "TileBar1", "TileBar2", "TileGap1", "TileGap2", "TileGap3", "TileExt0", "TileExt1", "TileExt2", "FCAL0", "FCAL1", "FCAL2",         ]
for i,c in enumerate(caloSamples):
    compactSpecification[ c ] = ( ("Energy in "+c+";Energy(GeV);",50,-10,100), ("EnergyPerSampling[%d]"%(i,), "vector<float>", "gev"))


# Translate the above specifications into proper histo tools
#   and add them to jhm
attributeHistoManager.buildKnownTools(compactSpecification)


# Jet histogramming tools
jhm.addTool( JetKinematicHistos("allkinematics",PlotOccupancy=True, PlotAveragePt=True, PlotNJet=True  , PlotNConstit = True) )
jhm.addTool( JetKinematicHistos("basickinematics")  )
jhm.addTool( JetKinematicHistos("basickinematics_emscale", JetScale=0)  )
jhm.addTool( JetKinematicHistos("basickinematics_constscale", JetScale=1)  )

jhm.addTool( LeadingJetsRelations("leadingjetrel",
                                  HistoDef = [
    hdef('ljrDeltaEta', "#Delta #eta (lead, sublead);#Delta#eta;Entries",100,-10,10 ),
    hdef('ljrDeltaPhi', "#Delta #Phi (lead, sublead);#Delta#Phi;Entries",100,0,3.142 ),
    hdef('ljrDeltaR', "#Delta R (lead, sublead);#Delta R;Entries",100,0,10 ),
    hdef('ljrFrac', "(sublead Pt)/(lead Pt);ratio;Entries",100,0,1. ),
    
    #hdef('ljrEta1Eta2', "sublead #eta vs lead #eta;lead #eta;sublead #eta",100,-5,5.,100,-5,5 ),

    ]
                                  )  )

jhm.addTool( EfficiencyResponseHistos("effresponse", 
                                      HistoDef = [
    hdef('erhEfficiencyR1', "Jet p_{T} Efficiency #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
    hdef('erhEfficiencyR2', "Jet p_{T} Efficiency #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
    hdef('erhEfficiencyR3', "Jet p_{T} Efficiency #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),

    hdef('erhResponse', "Jet p_{T} Response;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
    hdef('erhResponseVsEta', "Jet p_{T} Response vs #eta;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
    hdef('erhResponseVsPt', "Jet p_{T} Response vs p_{T};p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),

    hdef('erhDeltaR', "#DeltaR between Jet and closest Truth Jet;#DeltaR;Number of jets",50,0,4 ),

    ]
                                      ) )

# Selection tools
jhm.addTool( HistosForJetSelection("alljets", SelectionType=0) )
jhm.addTool( HistosForJetSelection("leadingjet", SelectionType=1) )
jhm.addTool( HistosForJetSelection("subleadingjet", SelectionType=2) )






#**************************************
#**************************************
# a helper function to combine selection with histo tools.
def selectionAndHistos( selectType, histos, selectionName="", **otherArgs):

    if isinstance(selectType, str): 
        selTool = jhm.tool(selectType)
    else:
        selTool = selectType
    
    if selTool is None or not issubclass( selTool.__class__ , HistosForJetSelection ):
        print "ERROR can't build histo tool for a jet selection. Uknown or wrong selection request :",selectType
        return None
    
    if selectionName != "":
        selTool = selTool.clone(selectionName)

    interpretedTools = []
    for h in histos:
        if isinstance(h,str):
            h = jhm.tool(h)
        interpretedTools.append(h)

    selTool.HistoTools = interpretedTools

    # set other args if any:
    for k,v in otherArgs.iteritems():
        setattr(selTool, k, v)

    return selTool
