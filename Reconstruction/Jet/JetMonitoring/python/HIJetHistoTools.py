# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetMonitoringConf import JetKinematicHistos, HistosForJetSelection, LeadingJetsRelations, HIEfficiencyResponseHistos, HIJetUEMonitoring

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
 
#
    "pt"               : (("Jet Pt;Pt [MeV];", 100, 0, 200) ,     ("pt","float","gev" ) ),    
    "Width"            : (("Jet Width;Width;", 50, 0, 1.0) ,         ("Width","float" ) ),
#    "Width15"          : (("Jet Width;Width;", 50, 0, 1.5) ,         ("Width","float" ) ),
    "EMFrac"           : (("EM Fraction;EM fraction;", 50, -0.1, 1.4),  ("EMFrac", "float") ),
    "HECFrac"          : (("HEC Fraction;HEC fraction;", 50, -0.1, 1.4),  ("HECFrac", "float") ),
    "Timing"           : (("Jet Time info;Time;", 40, -20, 20) ,     ("Timing", "float") ),    
    "LArQuality"       : (("LAr quality;Energy;", 50, -0.4, 1.2),  ("LArQuality", "float") ),
    "ptN"              : (("Jet Pt;Pt [GeV];", 250, 0., 5000.) ,     ("pt","float","gev" ) ), 
    "SumPtTrkPt1000"   : (("Sum Pt of all tracks above 1GeV:SumPt;", 100,0,200), ("SumPtTrkPt1000", "vector<float>", "gev") ),
    ## Jet Cleaning variables ##
    "AverageLArQF"     : (("Average LAr QF;AverageLArQF;",100,0, 65535), ("AverageLArQF", "float") ),
    "HECQuality"       : (("HEC Quality;HEC Quality;",50,-0.1, 1.4), ("HECQuality", "float") ),
    "FracSamplingMax"  : (("FracSamplingMax; FracSamplingMax;",50,-0.1, 1.2), ("FracSamplingMax", "float") ),
    # binning optimisation from Emma Tolley 
    #"FracSamplingMaxIndex" : (("FracSamplingMaxIndex; FracSamplingMaxIndex;",23,0,23), ("FracSamplingMaxIndex", "int") ),
    "FracSamplingMaxIndex" : (("FracSamplingMaxIndex; FracSamplingMaxIndex;",24,0,24), ("FracSamplingMaxIndex", "int") ),
    "N90Constituents"  : (("N90Constituents; N90Constituents;",15,0,15), ("N90Constituents", "float") ),
  
    # 2D Histo format is
    # "histoname" : ( binning, attributeInfo1, attributeInfo2 )
    # where
    #  - binning is ("title;labelx;labely", nbins, xlow, xup, nbinsy, ylow, yup) as in TH2 ctor
    #  - attributeInfo is ("attribute", "attribute type") or ("attribute", "attribute type", "gev")    
###########HI moments
        
    "JetUnsubtractedScaleMomentum_pt"      : (("JetUnsubtractedScaleMomentum p_{T}; p_{T} [MeV];",100,0,300000),("JetUnsubtractedScaleMomentum_pt","float")),
    "JetUnsubtractedScaleMomentum_eta"     : (("JetUnsubtractedScaleMomentum eta; #eta;",50,-6,6),("JetUnsubtractedScaleMomentum_eta","float")),
    "JetUnsubtractedScaleMomentum_phi"     : (("JetUnsubtractedScaleMomentum phi; #phi;",50,-3.3,3.3),("JetUnsubtractedScaleMomentum_phi","float")),
    "JetUnsubtractedScaleMomentum_m"       : (("JetUnsubtractedScaleMomentum m; Mass [MeV];",50,0,100000),("JetUnsubtractedScaleMomentum_m","float")),
    "JetSubtractedScaleMomentum_pt"        : (("JetSubtractedScaleMomentum p_{T}; p_{T} [MeV];",50,-5000,100000),("JetSubtractedScaleMomentum_pt","float")),
    "JetSubtractedScaleMomentum_eta"       : (("JetSubtractedScaleMomentum eta; #eta;",50,-6,6),("JetSubtractedScaleMomentum_eta","float")),
    "JetSubtractedScaleMomentum_phi"       : (("JetSubtractedScaleMomentum phi; #phi;",50,-3.3,3.3),("JetSubtractedScaleMomentum_phi","float")),
    "JetSubtractedScaleMomentum_m"         : (("JetSubtractedScaleMomentum m; Mass [MeV];",25,-5000,50000),("JetSubtractedScaleMomentum_m","float")),
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
#jhm.addTool( JetKinematicHistos("basickinematics_emscale", JetScale="JetEMScaleMomentum")  )
#jhm.addTool( JetKinematicHistos("basickinematics_constscale", JetScale="JetConstitScaleMomentum")  )

jhm.addTool( LeadingJetsRelations("leadingjetrel",
                                  HistoDef = [
    hdef('ljrDeltaEta', "#Delta #eta (lead, sublead);#Delta#eta;Entries",100,-10,10 ),
    hdef('ljrDeltaPhi', "#Delta #Phi (lead, sublead);#Delta#Phi;Entries",100,0,3.142 ),
    hdef('ljrDeltaR', "#Delta R (lead, sublead);#Delta R;Entries",100,0,10 ),
    hdef('ljrFrac', "(sublead Pt)/(lead Pt);ratio;Entries",100,0,1. ),
    
    #hdef('ljrEta1Eta2', "sublead #eta vs lead #eta;lead #eta;sublead #eta",100,-5,5.,100,-5,5 ),

    ]
                                  )  )

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
            
            hdef('erhEfficiencyR1_0_10', "Jet p_{T} Efficiency in 0-10% #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR2_0_10', "Jet p_{T} Efficiency in 0-10% #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR3_0_10', "Jet p_{T} Efficiency in 0-10% #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhResponse_0_10', "Jet p_{T} Response in 0-10%;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
            hdef('erhResponseVsEta_0_10', "Jet p_{T} Response vs #eta in 0-10%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsEta_highpT_0_10', "Jet p_{T} Response vs #eta (pT>100 GeV) in 0-10%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsPt_0_10', "Jet p_{T} Response vs p_{T} in 0-10%;p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),

            hdef('erhEfficiencyR1_10_20', "Jet p_{T} Efficiency in 10-20% #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR2_10_20', "Jet p_{T} Efficiency in 10-20% #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR3_10_20', "Jet p_{T} Efficiency in 10-20% #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhResponse_10_20', "Jet p_{T} Response in 10-20%;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
            hdef('erhResponseVsEta_10_20', "Jet p_{T} Response vs #eta in 10-20%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsEta_highpT_10_20', "Jet p_{T} Response vs #eta (pT>100 GeV) in 10-20%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsPt_10_20', "Jet p_{T} Response vs p_{T} in 10-20%;p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),

            hdef('erhEfficiencyR1_20_40', "Jet p_{T} Efficiency in 20-40% #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR2_20_40', "Jet p_{T} Efficiency in 20-40% #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR3_20_40', "Jet p_{T} Efficiency in 20-40% #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhResponse_20_40', "Jet p_{T} Response in 20-40%;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
            hdef('erhResponseVsEta_20_40', "Jet p_{T} Response vs #eta in 20-40%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsEta_highpT_20_40', "Jet p_{T} Response vs #eta (pT>100 GeV) in 20-40%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsPt_20_40', "Jet p_{T} Response vs p_{T} in 20-40%;p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),

            hdef('erhEfficiencyR1_60_100', "Jet p_{T} Efficiency in 60-100% #DeltaR = 0.1;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR2_60_100', "Jet p_{T} Efficiency in 60-100% #DeltaR = 0.2;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhEfficiencyR3_60_100', "Jet p_{T} Efficiency in 60-100% #DeltaR = 0.3;p_{T}^{Truth} (GeV);Efficiency",50,0,100 ),
            hdef('erhResponse_60_100', "Jet p_{T} Response in 60-100%;Number of jets;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-1,1 ),
            hdef('erhResponseVsEta_60_100', "Jet p_{T} Response vs #eta in 60-100%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsEta_highpT_60_100', "Jet p_{T} Response vs #eta (pT>100 GeV) in 60-100%;#eta of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-5,5 ),
            hdef('erhResponseVsPt_60_100', "Jet p_{T} Response vs p_{T} in 60-100%;p_{T}^{Truth} of jet;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,1000 ),


            hdef('erhResponse_RP', "Jet p_{T} Response vs #Psi_{2}; #Psi_{2};#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-3.3,3.3 ),
            hdef('erhResponse_2Dphi', "Jet p_{T} Response vs 2|#phi-#Psi_{2}|; 2|#phi - #Psi_{2}|;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,3.3 ),
            hdef('erhResponse_highpT_RP', "Jet p_{T} Response vs #Psi_{2} (pT>100 GeV); #Psi_{2};#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,-3.3,3.3 ),
            hdef('erhResponse_highpT_2Dphi', "Jet p_{T} Response vs 2|#phi-#Psi_{2}| (pT>100 GeV); 2|#phi - #Psi_{2}|;#frac{p_{T}^{Jet} - p_{T}^{Truth}}{p_{T}^{Truth}}",50,0,3.3 ),
            ]    
                                        ) )

# Selection tools
jhm.addTool( HistosForJetSelection("alljets", SelectionType=0) )
jhm.addTool( HistosForJetSelection("leadingjet", SelectionType=1) )
jhm.addTool( HistosForJetSelection("subleadingjet", SelectionType=2) )


jhm.addTool( HIJetUEMonitoring("centrality", 
                               HistoDef = [
            hdef('FCalET', "FCalET; FCal ET [TeV]; entries",50,0,10),
            hdef('SubtractedET_Centrality', "UnsubScMomentum-SubScMomentum vs FCalET; FCal ET [TeV]; SubtractedET [GeV]",50,0,10),
            hdef('2dSubtractedET_Centrality', "UnsubScMomentum-SubScMomentum vs FCalET; FCal ET [TeV]; SubtractedET [GeV]",50,0,10,50,0,250 ),

            hdef('2dSubtractedET_pT', "UnsubScMomentum-SubScMomentum vs pT; jet pT [GeV]; SubtractedET [GeV]",100,0,1000,50,0,250 ),
            
            hdef('2dSubtractedET_Expected_Centrality', "(UnsubScMomentum-SubScMomentum)/Expected vs FCalET; FCal ET [TeV]; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,0,10,100,0,5 ),

            hdef('2dSubtractedET_Expected_eta', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6,100,0,5 ),
            hdef('SubtractedET_Expected_eta', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6 ),

            hdef('2dSubtractedET_2Dphi', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}|; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3, 50,0,250 ),

            hdef('SubtractedET_eta', "UnsubScMomentum-SubScMomentum vs #eta; #eta; SubtractedET [GeV]",50,-6,6 ),
            hdef('SubtractedET_pt', "UnsubScMomentum-SubScMomentum vs p_{T}; p_{T} [GeV]; SubtractedET [GeV]",100,0,1000 ),

            hdef('SubtractedET_2Dphi', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}|; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3 ),

# 0-10 %
            hdef('JetUnsubtractedScaleMomentum_pt_0_10', "JetUnsubtractedScaleMomentum p_{T} - 0-10%;p_{T} [GeV]",100,0,300 ),
            hdef('JetUnsubtractedScaleMomentum_eta_0_10', "JetUnsubtractedScaleMomentum eta - 0-10%; #eta",50,-6,6 ),
            hdef('JetUnsubtractedScaleMomentum_phi_0_10', "JetUnsubtractedScaleMomentum phi - 0-10%; #phi",50,-3.3,3.3 ),
            hdef('JetUnsubtractedScaleMomentum_m_0_10', "JetUnsubtractedScaleMomentum m - 0-10%; Mass [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_pt_0_10', "JetSubtractedScaleMomentum p_{T} - 0-10%;p_{T} [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_eta_0_10', "JetSubtractedScaleMomentum eta - 0-10%; #eta",50,-6,6 ),
            hdef('JetSubtractedScaleMomentum_phi_0_10', "JetSubtractedScaleMomentum phi - 0-10%; #phi",50,-3.3,3.3 ),
            hdef('JetSubtractedScaleMomentum_m_0_10', "JetSubtractedScaleMomentum m - 0-10%; Mass [GeV]",25,0,50 ),

            hdef('2dSubtractedET_Expected_eta_0_10', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 0-10%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6,100,0,5 ),
            hdef('SubtractedET_Expected_eta_0_10', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 0-10%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6 ),

            hdef('2dSubtractedET_2Dphi_0_10', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 0-10%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3, 40,0,200 ),

            hdef('SubtractedET_eta_0_10', "UnsubScMomentum-SubScMomentum vs #eta - 0-10%; #eta; SubtractedET [GeV]",50,-6,6 ),
            hdef('SubtractedET_pt_0_10', "UnsubScMomentum-SubScMomentum vs p_{T} - 0-10%; p_{T} [GeV]; SubtractedET [GeV]",100,0,1000 ),

            hdef('SubtractedET_2Dphi_0_10', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 0-10%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3 ),

# 10-20 %
            hdef('JetUnsubtractedScaleMomentum_pt_10_20', "JetUnsubtractedScaleMomentum p_{T} - 10-20%;p_{T} [GeV]",100,0,300 ),
            hdef('JetUnsubtractedScaleMomentum_eta_10_20', "JetUnsubtractedScaleMomentum eta - 10-20%; #eta",50,-6,6 ),
            hdef('JetUnsubtractedScaleMomentum_phi_10_20', "JetUnsubtractedScaleMomentum phi - 10-20%; #phi",50,-3.3,3.3 ),
            hdef('JetUnsubtractedScaleMomentum_m_10_20', "JetUnsubtractedScaleMomentum m - 10-20%; Mass [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_pt_10_20', "JetSubtractedScaleMomentum p_{T} - 10-20%;p_{T} [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_eta_10_20', "JetSubtractedScaleMomentum eta - 10-20%; #eta",50,-6,6 ),
            hdef('JetSubtractedScaleMomentum_phi_10_20', "JetSubtractedScaleMomentum phi - 10-20%; #phi",50,-3.3,3.3 ),
            hdef('JetSubtractedScaleMomentum_m_10_20', "JetSubtractedScaleMomentum m - 10-20%; Mass [GeV]",25,0,50 ),

            hdef('2dSubtractedET_Expected_eta_10_20', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 10-20%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6,100,0,5 ),
            hdef('SubtractedET_Expected_eta_10_20', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 10-20%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6 ),

            hdef('2dSubtractedET_2Dphi_10_20', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 10-20%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3, 40,0,200 ),

            hdef('SubtractedET_eta_10_20', "UnsubScMomentum-SubScMomentum vs #eta - 10-20%; #eta; SubtractedET [GeV]",50,-6,6 ),
            hdef('SubtractedET_pt_10_20', "UnsubScMomentum-SubScMomentum vs p_{T} - 10-20%; p_{T} [GeV]; SubtractedET [GeV]",100,0,1000 ),

            hdef('SubtractedET_2Dphi_10_20', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 10-20%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3 ),

# 20-40 %
            hdef('JetUnsubtractedScaleMomentum_pt_20_40', "JetUnsubtractedScaleMomentum p_{T} - 20-40%;p_{T} [GeV]",100,0,300 ),
            hdef('JetUnsubtractedScaleMomentum_eta_20_40', "JetUnsubtractedScaleMomentum eta - 20-40%; #eta",50,-6,6 ),
            hdef('JetUnsubtractedScaleMomentum_phi_20_40', "JetUnsubtractedScaleMomentum phi - 20-40%; #phi",50,-3.3,3.3 ),
            hdef('JetUnsubtractedScaleMomentum_m_20_40', "JetUnsubtractedScaleMomentum m - 20-40%; Mass [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_pt_20_40', "JetSubtractedScaleMomentum p_{T} - 20-40%;p_{T} [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_eta_20_40', "JetSubtractedScaleMomentum eta - 20-40%; #eta",50,-6,6 ),
            hdef('JetSubtractedScaleMomentum_phi_20_40', "JetSubtractedScaleMomentum phi - 20-40%; #phi",50,-3.3,3.3 ),
            hdef('JetSubtractedScaleMomentum_m_20_40', "JetSubtractedScaleMomentum m - 20-40%; Mass [GeV]",25,0,50 ),

            hdef('2dSubtractedET_Expected_eta_20_40', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 20-40%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6,100,0,5 ),
            hdef('SubtractedET_Expected_eta_20_40', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 20-40%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6 ),

            hdef('2dSubtractedET_2Dphi_20_40', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 20-40%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3, 40,0,200 ),

            hdef('SubtractedET_eta_20_40', "UnsubScMomentum-SubScMomentum vs #eta - 20-40%; #eta; SubtractedET [GeV]",50,-6,6 ),
            hdef('SubtractedET_pt_20_40', "UnsubScMomentum-SubScMomentum vs p_{T} - 20-40%; p_{T} [GeV]; SubtractedET [GeV]",100,0,1000 ),

            hdef('SubtractedET_2Dphi_20_40', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 20-40%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3 ),
# 60-100 %
            hdef('JetUnsubtractedScaleMomentum_pt_60_100', "JetUnsubtractedScaleMomentum p_{T} - 60-100%;p_{T} [GeV]",100,0,300 ),
            hdef('JetUnsubtractedScaleMomentum_eta_60_100', "JetUnsubtractedScaleMomentum eta - 60-100%; #eta",50,-6,6 ),
            hdef('JetUnsubtractedScaleMomentum_phi_60_100', "JetUnsubtractedScaleMomentum phi - 60-100%; #phi",50,-3.3,3.3 ),
            hdef('JetUnsubtractedScaleMomentum_m_60_100', "JetUnsubtractedScaleMomentum m - 60-100%; Mass [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_pt_60_100', "JetSubtractedScaleMomentum p_{T} - 60-100%;p_{T} [GeV]",50,0,100 ),
            hdef('JetSubtractedScaleMomentum_eta_60_100', "JetSubtractedScaleMomentum eta - 60-100%; #eta",50,-6,6 ),
            hdef('JetSubtractedScaleMomentum_phi_60_100', "JetSubtractedScaleMomentum phi - 60-100%; #phi",50,-3.3,3.3 ),
            hdef('JetSubtractedScaleMomentum_m_60_100', "JetSubtractedScaleMomentum m - 60-100%; Mass [GeV]",25,0,50 ),

            hdef('2dSubtractedET_Expected_eta_60_100', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 60-100%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6,100,0,5 ),
            hdef('SubtractedET_Expected_eta_60_100', "(UnsubScMomentum-SubScMomentum)/Expected vs #eta - 60-100%; #eta; (A^{Jet}/A^{FCal})xSubtractedET/FCalET",50,-6,6 ),

            hdef('2dSubtractedET_2Dphi_60_100', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 60-100%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3, 40,0,200 ),

            hdef('SubtractedET_eta_60_100', "UnsubScMomentum-SubScMomentum vs #eta - 60-100%; #eta; SubtractedET [GeV]",50,-6,6 ),
            hdef('SubtractedET_pt_60_100', "UnsubScMomentum-SubScMomentum vs p_{T} - 60-100%; p_{T} [GeV]; SubtractedET [GeV]",100,0,1000 ),

            hdef('SubtractedET_2Dphi_60_100', "UnsubScMomentum-SubScMomentum vs 2|#phi-#Psi_{2}| - 60-100%; 2|#phi-#Psi_{2}|; SubtractedET [GeV]",25,0,3.3 ),

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
        print ("ERROR can't build histo tool for a jet selection. Uknown or wrong selection request :",selectType , selTool)
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
    for k,v in otherArgs.items():
        setattr(selTool, k, v)
    return selTool


