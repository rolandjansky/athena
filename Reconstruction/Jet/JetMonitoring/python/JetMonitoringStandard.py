#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file JetMonitoringExample.py
@author P-A. Delsart
@date 2019-03-12
@brief Main python configuration for the Run III Jet Monitoring
'''

from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec,  EventHistoSpec, SelectSpec, VarSpec

# *********************************************
# Define a list of histograms we want to draw for all jet containers.
# Write this list as a list of Jet histo specifications. Read comments below for details on specifications
commonHistoSpecs = [
    # Specification can be simple string.
    # in this case they are aliases to standard spec has defined in JetStandardHistoSpecs.knownHistos 
    "pt",  
    "highpt",
    "m",
    "eta",
    "rapidity",
    "phi",
    "EMFrac",
    "LArQuality",
    "AverageLArQF",
    "HECQuality",
    "HECFrac",
    "FracSamplingMax",
    "FracSamplingMaxIndex",
    "EM3Frac",
    "Tile0Frac",
    "CentroidR",
    "OotFracClusters5",
    "OotFracClusters10",
    "JVF",
    "JVF[0]",
    "JVF[1]",
    "Jvt",
    "nconstit",
    "N90Constituents",
    "leadingJetsRel",
    "JetConstitScaleMomentum_pt","JetEMScaleMomentum_pt","JetPileupScaleMomentum_pt","JetEtaJESScaleMomentum_pt",
    "JetConstitScaleMomentum_eta","JetEMScaleMomentum_eta","JetPileupScaleMomentum_eta","JetEtaJESScaleMomentum_eta",
    "JetConstitScaleMomentum_phi","JetEMScaleMomentum_phi","JetPileupScaleMomentum_phi","JetEtaJESScaleMomentum_phi",


    # or we can directly add our custom histo specification in the form of a HistoSpec
    #   (HistoSpec is nothing more than a specialized python dictionnary)
    EventHistoSpec('njets',(100,0.,100.),title="Number of Jets;Njets;"),
    EventHistoSpec('njetsPt20',(100,0.,100.),title="Number of Jets pT>20 GeV;Njets;"),
    EventHistoSpec('njetsPt200',(800,0.,8000.),title="Number of Jets pT>200 GeV;Njets;"),
    # the basic call is : HistoSpec( variable, histobins, title='histotile;xtitle,ytitle')


    # 2D histos, by concatenating 1D histo specifications :
    "pt;m",
    "pt;eta",
    "phi;eta",


    # TProfile2D : just use 3 variables. For now the sytem will automatically
    #  interpret it as a TProfile2D (the 3rd variable being profiled)
    "phi;eta;e",  # --> Average Energy vs pt and eta
    "phi;eta;pt", # --> Average pt vs phi and eta

    # Histograms build from a selection of filtered jets.
    #  Use a SelectSpec dictionary to define the selection, and the histo to be drawn from it.
    SelectSpec( 'central',   # the name of the selection
                '|eta|<0.5', # selection expression. The form 'min<var<max' is automatically interpreted.
                path='CentralJets', # force the path where the histos are saved in the final ROOT file
                FillerTools = ["pt","m","eta","phi","EMFrac","Tile0Frac","LArQuality","nconstit","N90Constituents",
                "JetConstitScaleMomentum_pt","JetEMScaleMomentum_pt","JetPileupScaleMomentum_pt","JetEtaJESScaleMomentum_pt",
                "JetConstitScaleMomentum_eta","JetEMScaleMomentum_eta","JetPileupScaleMomentum_eta","JetEtaJESScaleMomentum_eta",
                "JetConstitScaleMomentum_phi","JetEMScaleMomentum_phi","JetPileupScaleMomentum_phi","JetEtaJESScaleMomentum_phi",
                ] ),

     SelectSpec( 'tilegap3',   # the name of the selection
                '1.0<|eta|<1.4', # selection expression. The form 'min<var<max' is automatically interpreted.
                path='TileGap3', # force the path where the histos are saved in the final ROOT file
                FillerTools = ["pt","m","eta","phi","EMFrac","Tile0Frac","HECFrac","LArQuality","nconstit","N90Constituents",
                ] ),

    # another selection : only leading jets
    SelectSpec( 'leading',
                '', # no selection on variables
                SelectedIndex=0, # force 1st (leading) jet (we would set 1 for sub-leading jets)
                path='LeadingJets', # force the path where the histos are saved in the final ROOT file
                FillerTools = ["pt","m","eta","phi","EMFrac","Tile0Frac","HECFrac","LArQuality","nconstit","N90Constituents",
                "JetConstitScaleMomentum_pt","JetEMScaleMomentum_pt","JetPileupScaleMomentum_pt","JetEtaJESScaleMomentum_pt",
                "JetConstitScaleMomentum_eta","JetEMScaleMomentum_eta","JetPileupScaleMomentum_eta","JetEtaJESScaleMomentum_eta",
                "JetConstitScaleMomentum_phi","JetEMScaleMomentum_phi","JetPileupScaleMomentum_phi","JetEtaJESScaleMomentum_phi",
                ] ),

    # another selection : only subleading jets
    SelectSpec( 'subleading',
                '', # no selection on variables
                SelectedIndex=1, # force 2nd (sub-leading) jet
                path='SubleadingJets', # force the path where the histos are saved in the final ROOT file
                FillerTools = ["pt","m","eta","phi","EMFrac","Tile0Frac","HECFrac","LArQuality","nconstit","N90Constituents",
                "JetConstitScaleMomentum_pt","JetEMScaleMomentum_pt","JetPileupScaleMomentum_pt","JetEtaJESScaleMomentum_pt",
                "JetConstitScaleMomentum_eta","JetEMScaleMomentum_eta","JetPileupScaleMomentum_eta","JetEtaJESScaleMomentum_eta",
                "JetConstitScaleMomentum_phi","JetEMScaleMomentum_phi","JetPileupScaleMomentum_phi","JetEtaJESScaleMomentum_phi",
                ] ),

    # another selection : only very high pT jets
    SelectSpec( 'highptrange2TeVto8TeV','2000<pt<8000',path='highptrange2TeVto8TeV',FillerTools = ["highpt","m","eta","phi","eta;phi",] ),
    SelectSpec( 'highptrange1TeVto2TeV','1000<pt<2000',path='highptrange1TeVto2TeV',FillerTools = ["highpt","m","eta","phi","eta;phi",] ),
    SelectSpec( 'highptrange500GeVto1TeV','500<pt<1000',path='highptrange500GeVto1TeV',FillerTools = ["highpt","m","eta","phi","eta;phi",] ),
    SelectSpec( 'highptrange200GeVto500GeV','200<pt<500',path='highptrange200GeVto500GeV',FillerTools = ["highpt","m","eta","phi","eta;phi",] ),

    # Selecting jets failing the LooseBad selection from the JetCleaningTool.
    SelectSpec( 'LooseBadFailedJets', 'LooseBad', InverseJetSel=True, 
                FillerTools = ["pt","phi","eta","m","EMFrac","LArQuality","Tile0Frac","HECFrac","nconstit","N90Constituents",
                "phi;eta","phi;eta;e","phi;eta;pt",
                ]),   

    # Selecting jets passing the LooseBad selection from the JetCleaningTool.
    SelectSpec( 'LooseBadJets',
                'LooseBad', # this is not in the form 'min<x<max', so it will be assumed 'LooseBad' is an entry existing in JetStandardHistoSpecs.knownSelector
                FillerTools = [ "pt","eta","phi","m","EMFrac","LArQuality","Tile0Frac","HECFrac","nconstit","N90Constituents",
                "phi;eta","phi;eta;e","phi;eta;pt",
                ] ),
    ] # end commonHistoSpecs

# Separate these out because they cannot run in cosmics
jvfHistosSpec = [
    SelectSpec( 'highJVF',
                '0.3<JVF[0]', # JVF is a vector<float> for each jets. Here we cut on the 0th entry of this vector
                FillerTools = ["pt","m","eta","phi",
                ] ),
]

topoHistosSpec = [
    # histos common to all topo jets. Energy By Sampling layer plots.
    HistoSpec("PreSamplerB",(100,0.,200.),title="Energy in PreSamplerB;E;Entries",xvar=VarSpec('EnergyPerSampling[0]')),
    HistoSpec("EMB1",(100,0.,200.),title="Energy in EMB1;E;Entries",xvar=VarSpec('EnergyPerSampling[1]')),
    HistoSpec("EMB2",(100,0.,200.),title="Energy in EMB2;E;Entries",xvar=VarSpec('EnergyPerSampling[2]')),
    HistoSpec("EMB3",(100,0.,200.),title="Energy in EMB3;E;Entries",xvar=VarSpec('EnergyPerSampling[3]')),
    HistoSpec("PreSamplerE",(100,0.,200.),title="Energy in PreSamplerE;E;Entries",xvar=VarSpec('EnergyPerSampling[4]')),
    HistoSpec("EME1",(100,0.,200.),title="Energy in EME2;E;Entries",xvar=VarSpec('EnergyPerSampling[5]')),
    HistoSpec("EME2",(100,0.,200.),title="Energy in EME2;E;Entries",xvar=VarSpec('EnergyPerSampling[6]')),
    HistoSpec("EME3",(100,0.,200.),title="Energy in EME3;E;Entries",xvar=VarSpec('EnergyPerSampling[7]')),
    HistoSpec("HEC0",(100,0.,200.),title="Energy in HEC0;E;Entries",xvar=VarSpec('EnergyPerSampling[8]')),
    HistoSpec("HEC1",(100,0.,200.),title="Energy in HEC1;E;Entries",xvar=VarSpec('EnergyPerSampling[9]')),
    HistoSpec("HEC2",(100,0.,200.),title="Energy in HEC2;E;Entries",xvar=VarSpec('EnergyPerSampling[10]')),
    HistoSpec("HEC3",(100,0.,200.),title="Energy in HEC3;E;Entries",xvar=VarSpec('EnergyPerSampling[11]')),
    HistoSpec("TileBar0",(100,0.,200.),title="Energy in TileBar0;E;Entries",xvar=VarSpec('EnergyPerSampling[12]')),
    HistoSpec("TileBar1",(100,0.,200.),title="Energy in TileBar1;E;Entries",xvar=VarSpec('EnergyPerSampling[13]')),
    HistoSpec("TileBar2",(100,0.,200.),title="Energy in TileBar1;E;Entries",xvar=VarSpec('EnergyPerSampling[14]')),
    HistoSpec("TileGap1",(100,0.,200.),title="Energy in TileGap1;E;Entries",xvar=VarSpec('EnergyPerSampling[15]')),
    HistoSpec("TileGap2",(100,0.,200.),title="Energy in TileGap2;E;Entries",xvar=VarSpec('EnergyPerSampling[16]')),
    HistoSpec("TileGap3",(100,0.,200.),title="Energy in TileGap3;E;Entries",xvar=VarSpec('EnergyPerSampling[17]')),
    HistoSpec("TileExt0",(100,0.,200.),title="Energy in TileExt0;E;Entries",xvar=VarSpec('EnergyPerSampling[18]')),
    HistoSpec("TileExt1",(100,0.,200.),title="Energy in TileExt1;E;Entries",xvar=VarSpec('EnergyPerSampling[19]')),
    HistoSpec("TileExt2",(100,0.,200.),title="Energy in TileExt2;E;Entries",xvar=VarSpec('EnergyPerSampling[20]')),
    HistoSpec("FCAL0",(100,0.,200.),title="Energy in FCAL0;E;Entries",xvar=VarSpec('EnergyPerSampling[21]')),
    HistoSpec("FCAL1",(100,0.,200.),title="Energy in FCAL1;E;Entries",xvar=VarSpec('EnergyPerSampling[22]')),
    HistoSpec("FCAL2",(100,0.,200.),title="Energy in FCAL2;E;Entries",xvar=VarSpec('EnergyPerSampling[23]')),
]

pflowHistosSpec = [
    # histos common to all PFlow jets. These are defined in JetStandardHistoSpecs.py
    "SumPtChargedPFOPt500[0]",
    "SumPtTrkPt500[0]",
    "NumTrkPt500[0]",
    "NumTrkPt1000[0]",
    "fCharged",
]



def jetMonAlgConfig(  jetName, inputFlags, truthJetName='', trigger=''):
    """returns a specification of a JetMonitoringAlg (in the form of a JetMonAlgSpec dictionnary).
    """
    
    # we use a specialized dictionnary (JetMonAlgSpec) which will be translated into the final C++ tool
    jetAlgConfig = JetMonAlgSpec(
        jetName+"MonAlg",
        JetContainerName = jetName,
        TriggerChain = trigger ,  
    )

    # the list of histos specifications
    histoSpecs = []

    # then add pre-defined lists as defined above :
    histoSpecs += commonHistoSpecs 

    if inputFlags.DQ.DataType != 'cosmics':
        histoSpecs += jvfHistosSpec

    if 'Topo' in jetName:
        histoSpecs += topoHistosSpec
    if 'PFlow' in jetName:
        histoSpecs += pflowHistosSpec


    if truthJetName != "" :
        # then add histos showing efficiency and pT responses vs True
        from JetMonitoring.JetStandardHistoSpecs import responseAndEffSpecMap
        if truthJetName not in responseAndEffSpecMap:
            print( "ERROR !! can't schedule a JetHistoResponseAndEff for truth container : ",truthJetName, ". No specification available" )
            return None

        histoSpecs +=[ responseAndEffSpecMap[truthJetName]  ]

    # finally all all histos specs to our JetMonitoringAlg specification :
    jetAlgConfig.appendHistos( * histoSpecs)

    return jetAlgConfig
    


def standardJetMonitoring(inputFlags):
    """Standard jet monitoring function to be inserted from top-level algs. 
    returns an a component accumulator as given by AthMonitorCfgHelper.result()
    Details of what goes into jet monitoring is implemented by dedicated functions such as jetMonAlgConfig().
    """

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    rv = ComponentAccumulator()

    # do not run monitoring in RAWtoESD
    if inputFlags.DQ.Environment == 'tier0Raw':
        return rv

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'JetMonitoring')


    # create a list of JetMonitoringAlg specifications
    jetAlgConfs = [
        # use the helper function defined above :
        #jetMonAlgConfig( "AntiKt4LCTopoJets", truthJetName="AntiKt4TruthJets"),     #How can we make sure truth jets are available ??
        jetMonAlgConfig( "AntiKt4LCTopoJets", inputFlags),
        jetMonAlgConfig( "AntiKt4EMTopoJets", inputFlags),
        jetMonAlgConfig( "AntiKt4EMPFlowJets", inputFlags),
        ]
    
    # schedule each JetMonitoringAlg by invoking the toAlg() methods of the config specification
    for conf in jetAlgConfs:        
        conf.toAlg(helper) # adds the conf as a JetMonitoringAlg to the helper
    
    rv.merge(helper.result())  # the AthMonitorCfgHelper returns an accumulator to be used by the general configuration system.
    return rv
