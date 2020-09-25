# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import  SystemOfUnits
from JetMonitoring.JetMonitoringConfig import HistoSpec, EventHistoSpec, VarSpec, ConfigDict, ToolSpec

# ***********************************************    
# ***********************************************
# The list of "jet variable"" which are not simple float. Or aliases to simple float.
# A VarSpec specify :
#   1) the name of a jet attribute
#   2) its type
#   3) optional and only if type is vector<X>, the index of the element of the vector to be plotted
#  (simple float can be generated on-the-fly just by their name. they don't need spec)

knownVar = dict(    
    mass = VarSpec('m:GeV', 'float'),
    JVF = VarSpec('JVF', 'vecfloat'),
    EPS = VarSpec('EnergyPerSampling', 'vecfloat'),

    # this variable has an index specified. It will thus has only 1 value per jet : the JVF at pos 0
    JVF0 = VarSpec('JVF', 'vecfloat', 0),

)


knownEventVar = dict(    
    # These always are of type 'float'
    avgMu = ToolSpec('EventHistoVarTool', 'avgMu', Attribute='averageInteractionsPerCrossing'),
    actMu = ToolSpec('EventHistoVarTool', 'actMu', Attribute='actualInteractionsPerCrossing'),
    njets = ToolSpec('NumJetVarTool', 'njets', ),
    njetsPt20 = ToolSpec('NumJetVarTool', 'njetsPt20', PtCut=20.),
    njetsPt50 = ToolSpec('NumJetVarTool', 'njetsPt50', PtCut=50.),
    njetsEt20 = ToolSpec('NumJetVarTool', 'njetsEt20', EtCut=20.),
    njetsEt50 = ToolSpec('NumJetVarTool', 'njetsEt50', EtCut=50.),
    njetsEt20Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt20Eta0_32', EtCut=20., EtaMin=0., EtaMax=3.2),
    njetsEt50Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt50Eta0_32', EtCut=50., EtaMin=0., EtaMax=3.2),
    njetsEt70Eta0_24 = ToolSpec('NumJetVarTool', 'njetsEt70Eta0_24', EtCut=70., EtaMin=0., EtaMax=2.4),
    njetsEt60Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt60Eta0_32', EtCut=60., EtaMin=0., EtaMax=3.2),
    njetsEt100Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt100Eta0_32', EtCut=100., EtaMin=0., EtaMax=3.2),
    njetsEt260Eta32_49 = ToolSpec('NumJetVarTool', 'njetsEt260Eta32_49', EtCut=260., EtaMin=3.2, EtaMax=4.9),
    njetsEt200Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt200Eta0_32', EtCut=200., EtaMin=0., EtaMax=3.2),
    njetsEt330Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt330Eta0_32', EtCut=330., EtaMin=0., EtaMax=3.2),

)

# ***************************************
# The list of standard jet histograms.
# This is a list of specification, each describing how to histogram a jet variable into 1D or 2D histos.
# A specification is a specialized dictionnary (a HistoSpec, see JetMonitoringConfig.py)
# The format is :
#    HistoSpec( name , bins , optional_arguments... )
#  where 'name' is a string and 'bins' is a tuple as in (nbin,xmin,xmax) or (nbinx,xmin,xmax,nbiny,ymin,ymax) 
# See various commented examples below for optional arguments.

_knownHistos = [
    # Simple form : histogram of variable 'eta' (the name of spec is the same as the name of variable)
    #        As in TH1 ctor, ';' in the title is interpreted as in "Main Title;Title xAxis;Title yAxis"
    HistoSpec( 'eta',  (50,-5,5) , title='#eta;#eta;Entries'),
    HistoSpec( 'phi',  (50,-3.3,3.3) , title='#phi;#phi;Entries'),
    # Same but we indicate that the variable is to be plotted in GeV by appending ':GeV'
    HistoSpec( 'pt:GeV',  (100,0,750) , title='p_{T};p_{T} [GeV];'),    
    HistoSpec( 'm:GeV',  (100,0,300) , title='mass;mass [GeV];'),
    HistoSpec( 'e:GeV',  (100,0,750) , title='E;E [GeV];'),    
    HistoSpec( 'et:GeV', (100,0,750), title='E_{T};E_{T} [GeV],'),

    # We want an other pT histo, with different bins.
    # We add a new spec with a new name and we indicate the actual variable with the argument xvar
    HistoSpec( 'highpt',  (100,0.,4000) , title='p_{T};p_{T} [GeV];', xvar='pt:GeV'),    

    #EventHistoSpec( 'njets', (30,0,30), title='Jet Multiplicity;Njets;Entries' ),
    # When the jet variable is not a simple float, use the xvar argument to refer to a detailed variable spec in 'knownVar'
    HistoSpec( 'JVF',  (100,0,1.2) , title='Jet Vtx Frac;JVF;Entries', xvar='JVF'),    
    # if the var name contains '[N]' the system will assume the variable is a vector<float> and setup tools accordingly (so we don't need to specify 'xvar')
    HistoSpec( 'JVF[0]',  (100,0,1.2) , title='JVF for vtx 0;JVF[0];Entries', ), 
    HistoSpec( 'JVF[1]',  (100,0,1.2) , title='JVF for vtx 1;JVF[1];Entries', ),


    # full list
    HistoSpec('ptN', (250, 0.0, 5000.0), title='Jet Pt;Pt [GeV];', xvar='pt:GeV'),

    HistoSpec('EMFrac', (50, -0.1, 1.4), title='EM Fraction;EM Fraction;Entries', ),
    HistoSpec('LArQuality', (50, -0.4, 1.2), title='LAr quality;Energy;Entries', ),
    HistoSpec('HECQuality', (50, -0.1, 1.4), title='HEC Quality;HEC Quality;Entries', ),
    HistoSpec('HECFrac', (50, -0.1, 1.4), title='HEC Fraction;HEC Fraction;Entries', ),
    HistoSpec('AverageLArQF', (100, 0, 65535), title='Average LAr QF;AverageLArQF;Entries', ),
    HistoSpec('FracSamplingMaxIndex', (24, 0, 24), title='FracSamplingMaxIndex; FracSamplingMaxIndex;Entries', xvar=VarSpec('FracSamplingMaxIndex','int')),
    HistoSpec('FracSamplingMax', (50, -0.1, 1.2), title='FracSamplingMax; FracSamplingMax;Entries', ),
    HistoSpec('Timing', (40, -20, 20), title='Jet Time info;Time;', ),

    
    HistoSpec('LeadingClusterSecondLambda', (100, 0.0, 10000.0), title='LeadingClusterSecondLambda; LeadingClusterSecondLambda;', ),
    HistoSpec('LeadingClusterSecondR', (100, 0.0, 100000.0), title='LeadingClusterSecondR; LeadingClusterSecondR;', ),
    HistoSpec('OotFracClusters5', (50, -0.1, 1.2), title='OotFracClusters5; OotFracClusters5;', ),
    HistoSpec('OotFracClusters10', (50, -0.1, 1.2), title='OotFracClusters10; OotFracClusters10;', ),
    
    HistoSpec('Jvt', (70, -0.2, 1.2), title='Jet JVT;JVT;Entries',  ),
    HistoSpec('JVFCorr', (120, -1.2, 1.2), title='Jet JVT; JVFCorr;Entries', ),
    HistoSpec('JvtRpt', (75, 0, 1.5), title='Jet JVT Rpt; JVTRpt;Entries', ),
    HistoSpec('EM3Frac', (50,-0.1,1.0), title="EM3 fraction;EM3 fraction;Entries"),
    HistoSpec('Tile0Frac', (50,-0.1,1.0), title="Tile0 fraction;Tile0 fraction;Entries"),


    HistoSpec('GhostMuonSegmentCount', (60, 0, 60), title='Number of associated muon segments;Number;', xvar=VarSpec('GhostMuonSegmentCount','int')),
    HistoSpec('GhostTruthCount', (60, 0, 60), title='Number of associate truth part;Number;', xvar=VarSpec('GhostTruthCount','int')),
    HistoSpec('GhostTrackCount', (60, 0, 60), title='Number of associate tracks;Number;', xvar=VarSpec('GhostTrackCount','int')),
    HistoSpec('GhostTruthAssociationFraction', (50, 0, 1.0), title='Fraction of associated truth particles from a matched truth jet jet;GhostTruthAssociationFraction;', ),
    
    HistoSpec('Width', (50, 0, 1.0), title='Jet Width;Width;', ),
    HistoSpec('Width15', (50, 0, 1.5), title='Jet Width;Width;', xvar='Width'),
    HistoSpec('Mu12', (100, 0, 1.0), title='Mu12;Mu12;', ),

    HistoSpec('NumTrkPt500[0]', (100, 0, 100), title='Number of tracks from PV0 above 0.5 GeV:N_{tracks}(p_{T}>0.5 GeV);NumTrkPt500;Entries', ),
    HistoSpec('NumTrkPt1000[0]', (100, 0, 100), title='Number of all tracks above 1 GeV:N_{tracks}(p_{T}>1 GeV);NumTrkPt1000;Entries', ),
    HistoSpec('SumPtTrkPt500[0]:GeV', (100, 0, 200), title='Sum Pt of all tracks above 0.5 GeV:SumPtTrk(p_{T}>0.5 GeV);SumPtTrkPt500 [GeV];Entries', ),
    HistoSpec('SumPtChargedPFOPt500[0]:GeV', (100, 0, 200), title='Sum Pt of all charged PFO above 0.5 GeV:SumPtChargedPFO(p_{T}>0.5 GeV);SumPtChargedPFOPt500 [GeV];Entries', ),
    HistoSpec('fCharged', (100, 0, 2), title='Normalised sum Pt of all charged PFO above 0.5 GeV:fCharged(p_{T}>0.5 GeV);fCharged;Entries', ),

    HistoSpec('FoxWolfram4', (100, -1, 1), title='FoxWolfram0;FoxWolfram4;', ),
    HistoSpec('FoxWolfram0', (100, -1, 1), title='FoxWolfram0;FoxWolfram0;', ),
    HistoSpec('FoxWolfram1', (100, -1, 1), title='FoxWolfram0;FoxWolfram1;', ),
    HistoSpec('FoxWolfram2', (100, -1, 1), title='FoxWolfram0;FoxWolfram2;', ),
    HistoSpec('FoxWolfram3', (100, -1, 1), title='FoxWolfram0;FoxWolfram3;', ),

    HistoSpec('ZCut12', (100, 0, 1.0), title='ZCut12;ZCut12;', ),
    HistoSpec('ZCut23', (100, 0, 1.0), title='ZCut23;ZCut23;', ),
    HistoSpec('ZCut34', (100, 0, 1.0), title='ZCut34;ZCut34;', ),

    HistoSpec('KtDR', (100, 0, 10), title='KtDR;KtDR;', ),
    
    HistoSpec('Split12', (100, 0, 5000), title='Split12;Split12;', ),
    HistoSpec('Split23', (100, 0, 5000), title='Split23;Split23;', ),
    HistoSpec('Split34', (100, 0, 5000), title='Split34;Split34;', ),

    HistoSpec('D2', (100, -1, 1), title='D2;D2;', ),
    HistoSpec('D2_Beta2', (100, -1, 1), title='D2_Beta2;D2_Beta2;', ),

    HistoSpec('ThrustMaj', (100, -1, 2), title='ThrustMaj;ThrustMaj;', ),
    HistoSpec('ThrustMin', (100, -1, 2), title='ThrustMin;ThrustMin;', ),
    
    HistoSpec('ECF2', (100, 0, 10000), title='ECF2;ECF2;', ),
    HistoSpec('ECF3', (100, 0, 10000), title='ECF3;ECF3;', ),
    HistoSpec('ECF1', (100, 0, 10000), title='ECF1;ECF1;', ),
    HistoSpec('ECF1_Beta2', (100, -1, 1), title='ECF1_Beta2;ECF1_Beta2;', ),
    HistoSpec('ECF3_Beta2', (100, -1, 1), title='ECF3_Beta2;ECF3_Beta2;', ),
    HistoSpec('ECF2_Beta2', (100, -1, 1), title='ECF2_Beta2;ECF2_Beta2;', ),

    HistoSpec('DipExcl12', (100, -1, 2), title='DipExcl12;DipExcl12;', ),
    HistoSpec('Dip12', (100, -1, 2), title='Dip12;Dip12;', ),
    HistoSpec('Dip23', (100, -1, 2), title='Dip23;Dip23;', ),
    HistoSpec('Dip13', (100, -1, 2), title='Dip13;Dip13;', ),
    
    HistoSpec('C1_Beta2', (100, -1, 1), title='C1;C1;', xvar='C1'),
    HistoSpec('C2_Beta2', (100, -1, 1), title='C2_Beta2;C2_Beta2;', ),
    HistoSpec('C2', (100, -1, 1), title='C2;C2;', ),
    HistoSpec('C1', (100, -1, 1), title='C1;C1;', ),

    HistoSpec('NegativeE:GeV', (80, -10, 0), title='Negative E in Jet;Energy;', ),
    HistoSpec('N90Constituents', (15, 0, 15), title='N90Constituents; N90Constituents;', ),

    HistoSpec('BchCorrDotx', (50, 0, 1), title='BchCorrDotx:BchCorrDotx;', ),
    HistoSpec('BchCorrCell', (50, 0, 1), title='BchCorrCell:BchCorrCell;', ),

    
    HistoSpec('TrackWidthPt1000[0]', (75, 0.0, 1.5), title='Width from tracks from PV0 above 1 GeV:Track Width(p_{T}>1 GeV);', ),


    HistoSpec('Volatility', (100, -100, 100), title='Volatility;Volatility;', ),
    HistoSpec('PlanarFlow', (100, -1, 1.1), title='PlanarFlow;PlanarFlow;', ),
    HistoSpec('CentroidR', (100, 0, 7500), title='CentroidR; CentroidR;', ),
    HistoSpec('Sphericity', (100, 0, 1), title='Sphericity;Sphericity;', ),
    HistoSpec('Aplanarity', (100, 0, 1), title='Aplanarity;Aplanarity;', ),
    HistoSpec('Angularity', (100, -0.1, 0.1), title='Angularity;Angularity;', ),



    HistoSpec('Tau1', (100, 0, 1.0), title='Tau1;Tau1;', ),
    HistoSpec('Tau2', (100, 0, 1.0), title='Tau2;Tau2;', ),
    HistoSpec('Tau3', (100, 0, 1.0), title='Tau3;Tau3;', ),
    HistoSpec('Tau21', (100, 0, 1.0), title='Tau21;Tau21;', ),
    HistoSpec('Tau32', (100, 0, 1.0), title='Tau32;Tau32;', ),
    HistoSpec('Tau1_wta', (100, 0, 1.0), title='Tau1_wta;Tau1_wta;', ),
    HistoSpec('Tau3_wta', (100, 0, 1.0), title='Tau3_wta;Tau3_wta;', ),
    HistoSpec('Tau2_wta', (100, 0, 1.0), title='Tau2_wta;Tau2_wta;', ),
    HistoSpec('Tau21_wta', (100, 0, 1.0), title='Tau21_wta;Tau21_wta;', ),
    HistoSpec('Tau32_wta', (100, 0, 1.0), title='Tau32_wta;Tau32_wta;', ),

    HistoSpec('Charge', (100, -2, 2), title='Charge;Charge;', ),

    HistoSpec('DetectorEta', (100,-5,5), title="DetectorEta;Detector #eta;Entries" ), 
    HistoSpec('ActiveArea', (80,0,0.8), title="ActiveArea;Active Area;Entries" ), 
    HistoSpec('ActiveArea15', (80, 0, 1.5), title='Active Area;Active Area;Entries', xvar='ActiveArea'),

    HistoSpec('PullPhi', (100, -6.3, 6.3), title='PullPhi;PullPhi;', ),
    HistoSpec('PullMag', (100, 0, 100), title='PullMag;PullMag;', ),
    HistoSpec('Pull_C10', (100, -1, 1), title='Pull_C10;Pull_C10;', ),
    HistoSpec('Pull_C11', (100, -1, 1), title='Pull_C11;Pull_C11;', ),
    HistoSpec('Pull_C01', (100, -1, 1), title='Pull_C01;Pull_C01;', ),
    HistoSpec('Pull_C00', (100, -1, 1), title='Pull_C00;Pull_C00;', ),

    HistoSpec('ShowerDeconstructionW', (100, -100, 100), title='ShowerDeconstructionW;ShowerDeconstructionW;', ),
    HistoSpec('ShowerDeconstructionTop', (100, -100, 100), title='ShowerDeconstructionTop;ShowerDeconstructionTop;', ),


    HistoSpec( 'JetConstitScaleMomentum_eta',  (50,-5,5) , title='ConstitScale #eta;ConstitScale #eta;Entries'),
    HistoSpec( 'JetConstitScaleMomentum_phi',  (50,-3.3,3.3) , title='ConstitScale #phi;ConstitScale #phi;Entries'),
    HistoSpec( 'JetConstitScaleMomentum_pt:GeV',  (100,0,200) , title='ConstitScale p_{T};ConstitScale p_{T} [GeV];Entries'),    
    HistoSpec( 'JetConstitScaleMomentum_m:GeV',  (100,0,300) , title='ConstitScale mass;ConstitScale mass [GeV];Entries'),
    
    HistoSpec( 'JetEMScaleMomentum_eta',  (50,-5,5) , title='EMScale #eta;EMScale #eta;Entries'),
    HistoSpec( 'JetEMScaleMomentum_phi',  (50,-3.3,3.3) , title='EMScale #phi;EMScale #phi;Entries'),
    HistoSpec( 'JetEMScaleMomentum_pt:GeV',  (100,0,200) , title='EMScale p_{T};EMScale p_{T} [GeV];Entries'),    
    HistoSpec( 'JetEMScaleMomentum_m:GeV',  (100,0,300) , title='EMScale mass;EMScale mass [GeV];Entries'),

    HistoSpec( 'JetPileupScaleMomentum_eta',  (50,-5,5) , title='PileupScale #eta;PileupScale #eta;Entries'),
    HistoSpec( 'JetPileupScaleMomentum_phi',  (50,-3.3,3.3) , title='PileupScale #phi;PileupScale #phi;Entries'),
    HistoSpec( 'JetPileupScaleMomentum_pt:GeV',  (100,0,200) , title='PileupScale p_{T};PileupScale p_{T} [GeV];Entries'),    
    HistoSpec( 'JetPileupScaleMomentum_m:GeV',  (100,0,300) , title='PileupScale mass;PileupScale mass [GeV];Entries'),

    HistoSpec( 'JetEtaJESScaleMomentum_eta',  (50,-5,5) , title='EtaJESScale #eta;EtaJESScale #eta;Entries'),
    HistoSpec( 'JetEtaJESScaleMomentum_phi',  (50,-3.3,3.3) , title='EtaJESScale #phi;EtaJESScale #phi;Entries'),
    HistoSpec( 'JetEtaJESScaleMomentum_pt:GeV',  (100,0,200) , title='EtaJESScale p_{T};EtaJESScale p_{T} [GeV];Entries'),    
    HistoSpec( 'JetEtaJESScaleMomentum_m:GeV',  (100,0,300) , title='EtaJESScale mass;EtaJESScale mass [GeV];Entries'),
    # ---------------------
    # 2D histogram (x and y vars are separated by ';' )
    HistoSpec( 'pt:GeV;m:GeV',  (100,0,1000, 100,0,300) , title='mass vs p_{T};p_{T};mass [GeV];'),

]
    

# ---------------------
# Below we add specifications for custom monitoring tools used to create histograms not drawable from simple attribute/variables.
# We rely on the generic 'ToolSpec' dictionnary.
# Format is:  ToolSpec('ToolClassName', 'toolName', defineHistoFunc=aFunction , ...properties...) where 
#   - defineHistoFunc is mandatory. It must be a function with signature similar as HistoSpec.defineHisto
#     and dedicated to define histograms as in standard monitoring configuration.
#   - properties are properties of the 'ToolClassName'. They will be transfered to the c++
#     instance. If a property is itself a tool, it can be specified as a ToolSpec.
# 

# -- JetHistoLeadingJetsRelations specification
# The python helper defining the histograms using the monitoring framework :
def defineHistoForLeadingJets(conf, parentAlg, monhelper, path):                               
    
    # helpfor that generates the monitoring group#
    group = monhelper.addGroup(parentAlg, conf.Group,  'Jets/'+parentAlg.JetContainerName)
    path = 'standardHistos'
    group.defineHistogram('dEta;leadJetsDEta', path=path, xbins=100, xmin=-5, xmax=5)   
    group.defineHistogram('dPhi;leadJetsDPhi', path=path, xbins=100, xmin=-3, xmax=3)   
    group.defineHistogram('dR;leadJetsDR',     path=path, xbins=100, xmin=0, xmax=10)   
    

# Add the specifications
_knownHistos += [ 
    ToolSpec('JetHistoLeadingJetsRelations', 'leadingJetsRel',  defineHistoFunc=defineHistoForLeadingJets, Group='LeadingJetGroup',)
]




# -- JetHistoResponseAndEff specification
# this tools allows to plot a fixed set of efficiency and response histos vs truth (or any other reference container).
# The python helper defining the histograms using the monitoring framework :
def defineHistoForRespAndEff(conf, parentAlg, monhelper , path):
    # create a monitoring group with the histo path starting from the parentAlg
    group = monhelper.addGroup(parentAlg, conf.Group,  'Jets/'+parentAlg.JetContainerName)
    path = 'standardHistos'
    # define the histogram
    group.defineHistogram('passDr1,refPt;efficiencyR1',title='Passing deltaR<0.1', type="TEfficiency", path=path, xbins=100 , xmin=0, xmax=4000. ,)
    group.defineHistogram('passDr2,refPt;efficiencyR2',title='Passing deltaR<0.2', type="TEfficiency", path=path, xbins=100 , xmin=0, xmax=4000. ,)
    group.defineHistogram('passDr3,refPt;efficiencyR3',title='Passing deltaR<0.3', type="TEfficiency", path=path, xbins=100 , xmin=0, xmax=4000. ,)
    group.defineHistogram('relDiff',title='pT relative Diff', type="TH1F", path=path, xbins=100 , xmin=-2, xmax=2. ,)
    group.defineHistogram('refEta,relDiff',title='pT relative Diff vs Eta', type="TH2F", path=path, xbins=60 , xmin=-5, xmax=5., ybins=60 , ymin=-2, ymax=2. ,)
    group.defineHistogram('refPt,relDiff',title='pT relative Diff vs pT', type="TH2F", path=path, xbins=60 , xmin=0, xmax=5000., ybins=60 , ymin=-2, ymax=2. ,)

_knownHistos += [ 
    ToolSpec('JetHistoResponseAndEff', 'respVsAntiKt4Truth',  defineHistoFunc=defineHistoForRespAndEff, Group='AntiK4TruthRespGroup', RefContainerName="AntiKt4TruthJets", EnergyScale=1./SystemOfUnits.GeV),
    ToolSpec('JetHistoResponseAndEff', 'respVsAntiKt10TruthTrim',  defineHistoFunc=defineHistoForRespAndEff, Group='AntiK10TruthTrimRespGroup', RefContainerName="AntiKt10TruthTrimmedPtFrac5SmallR20Jets", EnergyScale=1./SystemOfUnits.GeV),
]
# purely for convenience we add a map of JetContainer name -> JetHistoResponseAndEff spec
responseAndEffSpecMap = dict(
    AntiKt4TruthJets = 'respVsAntiKt4Truth',
    AntiKt10TruthTrimmedPtFrac5SmallR20Jets = 'respVsAntiKt10TruthTrim',
    )




# --
knownHistos = ConfigDict(  )
# convert the list into a dictionnary indexed by spec name
for h in _knownHistos:
    knownHistos[h.name] = h





# **********************************************************
# commented out example of a defineHistoFunc suitable for custom tool specifications
# def defineHistoForHistoPtTool(conf, parentAlg, monhelper , path):
#     group = monhelper.addGroup(parentAlg, conf.Group,  parentAlg.JetContainerName+'/')
    
#     group.defineHistogram('jetPt', path='TestPtTool', xbins=100, xmin=0, xmax=160)


# ***************************************
# The list of known JetSelectorTool
#from JetSelectorTools.JetSelectorToolsConf import JetCleaningTool
knownSelector = dict(
    LooseBad = ToolSpec('JetCleaningTool' ,  "LooseBadJets" , CutLevel = "LooseBad")
)

