# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, HistosForJetSelection, LeadingJetsRelations, EfficiencyResponseHistos

from RecExConfig.RecFlags import rec 
if rec.doHeavyIon:
    from JetMonitoring.JetMonitoringConf import HIJetUEMonitoring, HIEfficiencyResponseHistos

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

#
#  Jet moments added from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/Run2JetMoments/
#
    "KtDR"             : (("KtDR;KtDR;",100,0,10),("KtDR","float")),
#
    "Tau1"             : (("Tau1;Tau1;",100,0,1.0),("Tau1","float")),
    "Tau2"             : (("Tau2;Tau2;",100,0,1.0),("Tau2","float")),
    "Tau3"             : (("Tau3;Tau3;",100,0,1.0),("Tau3","float")),
    "Tau1_wta"         : (("Tau1_wta;Tau1_wta;",100,0,1.0),("Tau1_wta","float")),
    "Tau2_wta"         : (("Tau2_wta;Tau2_wta;",100,0,1.0),("Tau2_wta","float")),
    "Tau3_wta"         : (("Tau3_wta;Tau3_wta;",100,0,1.0),("Tau3_wta","float")),
    "Tau21"            : (("Tau21;Tau21;",100,0,1.0),("Tau21","float")),
    "Tau32"            : (("Tau32;Tau32;",100,0,1.0),("Tau32","float")),
    "Tau21_wta"        : (("Tau21_wta;Tau21_wta;",100,0,1.0),("Tau21_wta","float")),
    "Tau32_wta"        : (("Tau32_wta;Tau32_wta;",100,0,1.0),("Tau32_wta","float")),
#
    "Dip12"            : (("Dip12;Dip12;",100,-1,2),("Dip12","float")),
    "Dip13"            : (("Dip13;Dip13;",100,-1,2),("Dip13","float")),
    "Dip23"            : (("Dip23;Dip23;",100,-1,2),("Dip23","float")),
    "DipExcl12"        : (("DipExcl12;DipExcl12;",100,-1,2),("DipExcl12","float")),
#
    "Split12"          : (("Split12;Split12;",100,0,5000),("Split12","float")),
    "Split23"          : (("Split23;Split23;",100,0,5000),("Split23","float")),
    "Split34"          : (("Split34;Split34;",100,0,5000),("Split34","float")),
#
    "ZCut12"           : (("ZCut12;ZCut12;",100,0,1.0),("ZCut12","float")),
    "ZCut23"           : (("ZCut23;ZCut23;",100,0,1.0),("ZCut23","float")),
    "ZCut34"           : (("ZCut34;ZCut34;",100,0,1.0),("ZCut34","float")),
#
    "Angularity"       : (("Angularity;Angularity;",100,-.1,.1), ("Angularity","float")),
#
    "PlanarFlow"       : (("PlanarFlow;PlanarFlow;",100,-1,1.1),("PlanarFlow","float")),
#
    "Mu12"             : (("Mu12;Mu12;",100,0,1.0),("Mu12","float")),
#
    "ECF1"             : (("ECF1;ECF1;",100,0,10000),("ECF1","float")),
    "ECF2"             : (("ECF2;ECF2;",100,0,10000),("ECF2","float")),
    "ECF3"             : (("ECF3;ECF3;",100,0,10000),("ECF3","float")),
    "ECF1_Beta2"       : (("ECF1_Beta2;ECF1_Beta2;",100,-1,1),("ECF1_Beta2","float")),
    "ECF2_Beta2"       : (("ECF2_Beta2;ECF2_Beta2;",100,-1,1),("ECF2_Beta2","float")),
    "ECF3_Beta2"       : (("ECF3_Beta2;ECF3_Beta2;",100,-1,1),("ECF3_Beta2","float")),
#
    "C1"               : (("C1;C1;",100,-1,1), ("C1","float")),
    "C2"               : (("C2;C2;",100,-1,1), ("C2","float")),
    "D2"               : (("D2;D2;",100,-1,1), ("D2","float")),
    "C1_Beta2"         : (("C1;C1;",100,-1,1), ("C1","float")),
    "C2_Beta2"         : (("C2_Beta2;C2_Beta2;",100,-1,1), ("C2_Beta2","float")),
    "D2_Beta2"         : (("D2_Beta2;D2_Beta2;",100,-1,1), ("D2_Beta2","float")),
#
    "ThrustMin"        : (("ThrustMin;ThrustMin;",100,-1,2),("ThrustMin","float")),
    "ThrustMaj"        : (("ThrustMaj;ThrustMaj;",100,-1,2),("ThrustMaj","float")),
    "FoxWolfram0"      : (("FoxWolfram0;FoxWolfram0;",100,-1,1),("FoxWolfram0","float")),
    "FoxWolfram1"      : (("FoxWolfram0;FoxWolfram1;",100,-1,1),("FoxWolfram1","float")),
    "FoxWolfram2"      : (("FoxWolfram0;FoxWolfram2;",100,-1,1),("FoxWolfram2","float")),
    "FoxWolfram3"      : (("FoxWolfram0;FoxWolfram3;",100,-1,1),("FoxWolfram3","float")),
    "FoxWolfram4"      : (("FoxWolfram0;FoxWolfram4;",100,-1,1),("FoxWolfram4","float")),
    "Sphericity"       : (("Sphericity;Sphericity;",100,0,1),("Sphericity","float")),
    "Aplanarity"       : (("Aplanarity;Aplanarity;",100,0,1),("Aplanarity","float")),
#
    "PullMag"          : (("PullMag;PullMag;",100,0,100),("PullMag","float")),
    "PullPhi"          : (("PullPhi;PullPhi;",100,-6.3,6.3),("PullPhi","float")),
    "Pull_C00"         : (("Pull_C00;Pull_C00;",100,-1,1),("Pull_C00","float")), 
    "Pull_C01"         : (("Pull_C01;Pull_C01;",100,-1,1),("Pull_C01","float")),                                                                                         
    "Pull_C10"         : (("Pull_C10;Pull_C10;",100,-1,1),("Pull_C10","float")),                                                                                         
    "Pull_C11"         : (("Pull_C11;Pull_C11;",100,-1,1),("Pull_C11","float")),                                                                                         
#   
    "Charge"           : (("Charge;Charge;",100,-2,2),("Charge","float")),
#
    "ShowerDeconstructionW": (("ShowerDeconstructionW;ShowerDeconstructionW;",100,-100,100),("ShowerDeconstructionW","float")),
    "ShowerDeconstructionTop": (("ShowerDeconstructionTop;ShowerDeconstructionTop;",100,-100,100),("ShowerDeconstructionTop","float")),
#
    "Volatility"       : (("Volatility;Volatility;",100,-100,100),("Volatility","float")),
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
    ## Jet Cleaning variables ##
    "AverageLArQF"     : (("Average LAr QF;AverageLArQF;",50,-0.1, 5.), ("AverageLArQF", "float") ),
    "HECQuality"       : (("HEC Quality;HEC Quality;",50,-0.1, 1.4), ("HECQuality", "float") ),
    "FracSamplingMax"  : (("FracSamplingMax; FracSamplingMax;",50,-0.1, 1.2), ("FracSamplingMax", "float") ),
    # binning optimisation from Emma Tolley 
    #"FracSamplingMaxIndex" : (("FracSamplingMaxIndex; FracSamplingMaxIndex;",23,0,23), ("FracSamplingMaxIndex", "int") ),
    "FracSamplingMaxIndex" : (("FracSamplingMaxIndex; FracSamplingMaxIndex;",24,0,24), ("FracSamplingMaxIndex", "int") ),
    # binning optimisation from Emma Tolley
    #"N90Constituents"  : (("N90Constituents; N90Constituents;",50,0,10), ("N90Constituents", "float") ), 
    "N90Constituents"  : (("N90Constituents; N90Constituents;",15,0,15), ("N90Constituents", "float") ),
    "CentroidR"        : (("CentroidR; CentroidR;",100,0,7500), ("CentroidR", "float") ),
    "OotFracClusters5" : (("OotFracClusters5; OotFracClusters5;",50,-0.1,1.2), ("OotFracClusters5", "float") ),
    "OotFracClusters10": (("OotFracClusters10; OotFracClusters10;",50,-0.1,1.2), ("OotFracClusters10", "float") ),
    #CBG
    "ptN"               : (("Jet Pt;Pt [GeV];", 250, 0., 5000.) ,     ("pt","float","gev" ) ), 
    "LeadingClusterCenterLambda": (("LeadingClusterCenterLambda; LeadingClusterCenterLambda;",100,0.,10000.), ("LeadingClusterCenterLambda", "float") ),
    "LeadingClusterSecondLambda": (("LeadingClusterSecondLambda; LeadingClusterSecondLambda;",100,0.,10000.), ("LeadingClusterSecondLambda", "float") ),
    # binning optimisation from Emma Tolley 
    #"LeadingClusterSecondR": (("LeadingClusterSecondR; LeadingClusterSecondR;",100,0.,1000.), ("LeadingClusterSecondR", "float") ), 
    "LeadingClusterSecondR": (("LeadingClusterSecondR; LeadingClusterSecondR;",100,0.,100000.), ("LeadingClusterSecondR", "float") ), 
    "CHF": (("SumPtTrkPt1000/pT; SumPtTrkPt1000/pT;",50,-1.,1.2), ("chf", "vector<float>", "gev") ), 
    #CBG 
    # 

    # 2D Histo format is
    # "histoname" : ( binning, attributeInfo1, attributeInfo2 )
    # where
    #  - binning is ("title;labelx;labely", nbins, xlow, xup, nbinsy, ylow, yup) as in TH2 ctor
    #  - attributeInfo is ("attribute", "attribute type") or ("attribute", "attribute type", "gev")    
###########HI moments50,-5, 50
        
    "JetUnsubtractedScaleMomentum_pt"      : (("JetUnsubtractedScaleMomentum p_{T}; p_{T} [MeV];",100,0,300000),("JetUnsubtractedScaleMomentum_pt","float")),
    "JetUnsubtractedScaleMomentum_eta"     : (("JetUnsubtractedScaleMomentum eta; #eta;",50,-6,6),("JetUnsubtractedScaleMomentum_eta","float")),
    "JetUnsubtractedScaleMomentum_phi"     : (("JetUnsubtractedScaleMomentum phi; #phi;",50,-3.3,3.3),("JetUnsubtractedScaleMomentum_phi","float")),
    "JetUnsubtractedScaleMomentum_m"       : (("JetUnsubtractedScaleMomentum m; Mass [MeV];",50,0,100000),("JetUnsubtractedScaleMomentum_m","float")),
    "JetSubtractedScaleMomentum_pt"        : (("JetSubtractedScaleMomentum p_{T}; p_{T} [MeV];",50,-5000,100000),("JetSubtractedScaleMomentum_pt","float")),
    "JetSubtractedScaleMomentum_eta"       : (("JetSubtractedScaleMomentum eta; #eta;",50,-6,6),("JetSubtractedScaleMomentum_eta","float")),
    "JetSubtractedScaleMomentum_phi"       : (("JetSubtractedScaleMomentum phi; #phi;",50,-3.3,3.3),("JetSubtractedScaleMomentum_phi","float")),
    "JetSubtractedScaleMomentum_m"         : (("JetSubtractedScaleMomentum m; Mass [MeV];",25,-5000,50000),("JetSubtractedScaleMomentum_m","float")),
    "JetSubtractedScaleNoVnMomentum_pt"    : (("JetSubtractedScaleNoVnMomentum p_{T}; p_{T} [MeV];",50,0,100000),("JetSubtractedScaleNoVnMomentum_pt","float")),
    "JetSubtractedScaleNoVnMomentum_eta"   : (("JetSubtractedScaleNoVnMomentum eta; #eta;",50,-6,6),("JetSubtractedScaleNoVnMomentum_eta","float")),
    "JetSubtractedScaleNoVnMomentum_phi"   : (("JetSubtractedScaleNoVnMomentum phi; #phi;",50,-3.3,3.3),("JetSubtractedScaleNoVnMomentum_phi","float")),
    "JetSubtractedScaleNoVnMomentum_m"     : (("JetSubtractedScaleNoVnMomentum m; Mass [MeV];",25,-5000,50000),("JetSubtractedScaleNoVnMomentum_m","float")),
    "MaxOverMean"                          : (("MaxOverMean; MaxOverMean;",50,0,20),("MaxOverMean","float")),
    "MaxConstituentET"                     : (("MaxConstituentET; E_{T} [MeV];",100,0,10000),("MaxConstituentET","float")),
    }

# then fill the pers calo sampling attributes one by one :
caloSamples =["PreSamplerB", "EMB1", "EMB2", "EMB3", "PreSamplerE", "EME1", "EME2", "EME3", "HEC0", "HEC1", "HEC2", "HEC3", "TileBar0", "TileBar1", "TileBar2", "TileGap1", "TileGap2", "TileGap3", "TileExt0", "TileExt1", "TileExt2", "FCAL0", "FCAL1", "FCAL2",         ]
for i,c in enumerate(caloSamples):
    compactSpecification[ c ] = ( ("Energy in "+c+";Energy(GeV);",100,-10,1000), ("EnergyPerSampling[%d]"%(i,), "vector<float>", "gev"))


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
if rec.doHeavyIon:
    jhm.addTool( HIEfficiencyResponseHistos("effresponse", 
                                      HistoDef = [
                hdef('erhEfficiencyR1', "Jet p_{T} Efficiency #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
                hdef('erhEfficiencyR2', "Jet p_{T} Efficiency #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
                hdef('erhEfficiencyR3', "Jet p_{T} Efficiency #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
                
                hdef('erhResponse', "Jet p_{T} Response;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
                hdef('erhResponseVsEta', "Jet p_{T} Response vs #eta;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
                hdef('erhResponseVsEta_highpT', "Jet p_{T} Response vs #eta (pT>100 GeV);#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
                hdef('erhResponseVsPt', "Jet p_{T} Response vs p_{T};p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),
                
                hdef('erhDeltaR', "#DeltaR between Jet and closest Truth Jet;#DeltaR;Number of jets",50,0,4 ),

                hdef('erhEfficiencyR1_CentralColl', "Jet p_{T} Efficiency in Central Coll. #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
                hdef('erhEfficiencyR2_CentralColl', "Jet p_{T} Efficiency in Central Coll. #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
                hdef('erhEfficiencyR3_CentralColl', "Jet p_{T} Efficiency in Central Coll. #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
                hdef('erhResponse_CentralColl', "Jet p_{T} Response in Central Coll.;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
                hdef('erhResponseVsEta_CentralColl', "Jet p_{T} Response vs #eta in Central Coll.;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
                hdef('erhResponseVsEta_highpT_CentralColl', "Jet p_{T} Response vs #eta (pT>100 GeV) in Central Coll.;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
                hdef('erhResponseVsPt_CentralColl', "Jet p_{T} Response vs p_{T} in Central Coll.;p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),
                hdef('erhResponse_RP', "Jet p_{T} Response vs #Psi_{2}; #Psi_{2};#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-3.3,3.3 ),
                hdef('erhResponse_2Dphi', "Jet p_{T} Response vs 2|#phi-#Psi_{2}|; 2|#phi - #Psi_{2}|;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,3.3 ),
                hdef('erhResponse_highpT_RP', "Jet p_{T} Response vs #Psi_{2} (pT>100 GeV); #Psi_{2};#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-3.3,3.3 ),
                hdef('erhResponse_highpT_2Dphi', "Jet p_{T} Response vs 2|#phi-#Psi_{2}| (pT>100 GeV); 2|#phi - #Psi_{2}|;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,3.3 ),
                ]    
                                          ) )
else :
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

if rec.doHeavyIon:
    jhm.addTool( HIJetUEMonitoring("centrality", 
                                   HistoDef = [
                hdef('JetUnsubtractedScaleMomentum_pt_CentralColl', "JetUnsubtractedScaleMomentum p_{T} in Central Coll.;p_{T} [GeV];",100,0,300 ),
                hdef('JetUnsubtractedScaleMomentum_eta_CentralColl', "JetUnsubtractedScaleMomentum eta in Central Coll.; #eta;",50,-6,6 ),
                hdef('JetUnsubtractedScaleMomentum_phi_CentralColl', "JetUnsubtractedScaleMomentum phi in Central Coll.; #phi;",50,-3.3,3.3 ),
                hdef('JetUnsubtractedScaleMomentum_m_CentralColl', "JetUnsubtractedScaleMomentum m in Central Coll.; Mass [GeV];",50,0,100 ),
                hdef('JetSubtractedScaleMomentum_pt_CentralColl', "JetSubtractedScaleMomentum p_{T} in Central Coll.;p_{T} [GeV];",50,0,100 ),
                hdef('JetSubtractedScaleMomentum_eta_CentralColl', "JetSubtractedScaleMomentum eta in Central Coll.; #eta;",50,-6,6 ),
                hdef('JetSubtractedScaleMomentum_phi_CentralColl', "JetSubtractedScaleMomentum phi in Central Coll.; #phi;",50,-3.3,3.3 ),
                hdef('JetSubtractedScaleMomentum_m_CentralColl', "JetSubtractedScaleMomentum m in Central Coll.; Mass [GeV];",25,0,50 ),
                hdef('JetSubtractedScaleNoVnMomentum_pt_CentralColl', "JetSubtractedScaleNoVnMomentum p_{T} in Central Coll.;p_{T} [GeV];",50,0,100 ),
                hdef('JetSubtractedScaleNoVnMomentum_eta_CentralColl', "JetSubtractedScaleNoVnMomentum eta in Central Coll.; #eta;",50,-6,6 ),
                hdef('JetSubtractedScaleNoVnMomentum_phi_CentralColl', "JetSubtractedScaleNoVnMomentum phi in Central Coll.; #phi;",50,-3.3,3.3 ),
                hdef('JetSubtractedScaleNoVnMomentum_m_CentralColl', "JetSubtractedScaleNoVnMomentum m in Central Coll.; Mass [GeV];",25,0,50 ),
                hdef('MaxOverMean_CentralColl', "MaxOverMean in Central Coll.;MaxOverMean;",50,0,20 ),
                hdef('MaxConstituentET_CentralColl', "MaxConstituentET in Central Coll.;  E_{T} [MeV];",100,0,10000 ),

                hdef('SubtractedET_Centrality', "UnsubScMomentum-SubScMomentum vs FCalET; FCal ET [TeV]; SubtractedET [GeV];",50,0,5,100,0,200 ),
                hdef('2dSubtractedET_Centrality', "UnsubScMomentum-SubScMomentum vs FCalET; FCal ET [TeV]; SubtractedET [GeV];",50,0,5,100,0,200 ),
                hdef('2dSubtractedETNoVn_Centrality', "JetSubtractedScaleMomentum(NoVn - SubScMom) vs FCalET; FCal ET [TeV]; SubtractedET [GeV];",50,0,5,20,-20,20 ),
                hdef('2dUnSubtractedETNoVN_Centrality', "Unsubtracted-JetSubtractedScaleNoVnMomentum vs FCalET; FCal ET [TeV]; SubtractedET [GeV];",50,0,5,100,0,200 ),

                hdef('2dSubtractedET_Expected_Centrality', "(UnsubScMomentum-SubScMomentum)/Expected vs FCalET; FCal ET [TeV]; (A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,0,5,100,0,2000 ),
                hdef('2dSubtractedETNoVn_Expected_Centrality', "JetSubtractedScaleMomentum(SubScMom - NoVn)/Expected  vs FCalET; FCal ET [TeV];(A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,0,5,100,-200,200 ),
                hdef('2dUnSubtractedETNoVN_Expected_Centrality', "(Unsubtracted-JetSubtractedScaleNoVnMomentum)/Expected  vs FCalET; FCal ET [TeV];(A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,0,5,100,0,2000 ),

                hdef('2dSubtractedET_Expected_eta', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta; #eta; (A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,-6,6,100,0,2000 ),
                hdef('2dSubtractedETNoVn_Expected_eta', "JetSubtractedScaleMomentum(NoVn - SubScMom)/Expected  vs #eta; #eta;(A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,-6,6,100,-200,200 ),
                hdef('2dUnSubtractedETNoVN_Expected_eta', "(Unsubtracted-JetSubtractedScaleNoVnMomentum)/Expected  vs #eta; #eta;(A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,-6,6,100,0,2000 ),
                hdef('SubtractedET_Expected_eta', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta; #eta; (A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,-6,6 ),
                hdef('SubtractedETNoVn_Expected_eta', "JetSubtractedScaleMomentum(NoVn - SubScMom)/Expected  vs #eta; #eta;(A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,-6,6 ),
                hdef('UnSubtractedETNoVN_Expected_eta', "Unsubtracted-JetSubtractedScaleNoVnMomentum/Expected  vs #eta; #eta;(A^{FCal}/A^{Jet})xSubtractedET/FCalET;",50,-6,6, ),



                hdef('SubtractedET_eta', "UnsubScMomentum-SubScMomentum vs #eta;  #eta; SubtractedET [GeV];",50,-6,6 ),
                hdef('SubtractedET_RP', "UnsubScMomentum-SubScMomentum vs #Psi_{2};  #Psi_{2}; SubtractedET [GeV];",50,-3.3,3.3 ),
                hdef('SubtractedET_2Dphi', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}|; 2|#phi-#Psi_{2}|; SubtractedET [GeV];",25,0,3.3 ),
                hdef('2dSubtractedET_2Dphi', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}|; 2|#phi-#Psi_{2}|; SubtractedET [GeV];",25,0,3.3, 100,0,200 ),
                hdef('SubScMom-NoVn_2Dphi', "JetSubtractedScaleMomentum(SubScMom - NoVn) vs 2|#phi-#Psi_{2}|; 2|#phi - #Psi_{2}|; SubScMom - NoVn [GeV];",25,0,3.3 ),
                hdef('2dSubScMom-NoVn_2Dphi', "JetSubtractedScaleMomentum(SubScMom - NoVn) vs 2|#phi-#Psi_{2}|; 2|#phi - #Psi_{2}|; SubScMom - NoVn [GeV];",25,0,3.3, 50,0,20 ),

                hdef('SubtractedET_eta_CentralColl', "UnsubScMomentum-SubScMomentum vs #eta in MidCentral Coll.;  #eta; SubtractedET [GeV];",50,-6,6,0,200 ),
                hdef('SubtractedET_RP_CentralColl', "UnsubScMomentum-SubScMomentum vs #Psi_{2} in MidCentral Coll.;  #Psi_{2}; SubtractedET [GeV];",50,-3.3,3.3 ),
                hdef('SubtractedET_2Dphi_CentralColl', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| in MidCentral Coll.; 2|#phi-#Psi_{2}|; SubtractedET [GeV];",25,0,3.3 ),
                hdef('2dSubtractedET_2Dphi_CentralColl', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| in MidCentral Coll.; 2|#phi-#Psi_{2}|; SubtractedET [GeV];",25,0,3.3, 100,0,200 ),
                hdef('SubScMom-NoVn_2Dphi_CentralColl', "SubScMom - NoVn vs 2|#phi-#Psi_{2}| in MidCentral Coll.; 2|#phi - #Psi_{2}|; JetSubtractedScaleMomentum(SubScMom - NoVn) [GeV];",25,0,3.3 ),
                hdef('2dSubScMom-NoVn_2Dphi_CentralColl', "SubScMom - NoVn vs 2|#phi-#Psi_{2}| in MidCentral Coll.; 2|#phi - #Psi_{2}|; JetSubtractedScaleMomentum(SubScMom - NoVn) [GeV];",25,0,3.3, 50,0,20 ),
                ]
                                   ))

#**************************************
#**************************************
# a helper function to combine selection with histo tools.
def selectionAndHistos( selectType, histos, selectionName="", histoNameSuffix="",**otherArgs):

    if isinstance(selectType, str):
        # interpret selectType as a string. For ex "20000<pt<500000" or "subleadingjet"
        tool = jhm.tool(selectType)
        if issubclass( tool.__class__ , HistosForJetSelection):
            # we're done.
            selTool = tool
        else:
            # assume we have or need an attribute selector
            attSel = tool
            if tool is None:
                attSel = attributeHistoManager.addSelector(selectType)
            # and we rebuild a HistosForJetSelection in any case.
            if histoNameSuffix=="" : histoNameSuffix = attSel.getName()
            selTool = HistosForJetSelection("hjsel_"+attSel.getName(), SelectionType=3,
                                            JetSelectorTool = attSel ,
                                            HistoTitleSuffix = ' ('+selectType+')',
                                            HistoNameSuffix = histoNameSuffix)
    else:
        selTool = selectType
    
    if selTool is None :
        print "ERROR can't build histo tool for a jet selection. Uknown or wrong selection request :",selectType , selTool
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


