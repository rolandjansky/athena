#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file JetMonitoringExample.py
@author P-A. Delsart
@date 2019-03-12
@brief Main python configuration for the Run III Jet Monitoring
'''

#from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec,  EventHistoSpec, SelectSpec
from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec,  EventHistoSpec, SelectSpec

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
    "CentroidR",
    "OotFracClusters5",
    "OotFracClusters10",
    "Jvt",
    "nconstit",
    "N90Constituents",     
    "JVF",
    "leadingJetsRel",


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
                FillerTools = ["pt","m","eta","phi","EMFrac","HECFrac","LArQuality","nconstit","N90Constituents",
                ] ),

     SelectSpec( 'tilegap3',   # the name of the selection
                '1.0<|eta|<1.4', # selection expression. The form 'min<var<max' is automatically interpreted.
                FillerTools = ["pt","m","eta","phi","EMFrac","HECFrac","LArQuality","nconstit","N90Constituents",
                ] ),

    # another selection : only leading jets
    SelectSpec( 'leading',
                '', # no selection on variables
                SelectedIndex=0, # force 1st (leading) jet (we would set 1 for sub-leading jets)
                path='standardHistos', # force the path where the histos are saved in the final ROOT file
                FillerTools = ["pt","m","eta","phi",
                ] ),

    # another selection : only subleading jets
    SelectSpec( 'subleading',
                '', # no selection on variables
                SelectedIndex=1, # force 2nd (sub-leading) jet
                path='standardHistos', # force the path where the histos are saved in the final ROOT file
                FillerTools = ["pt","m","eta","phi",
                ] ),

    # another selection : only very high pT jets
    SelectSpec( 'highptrange2TeVto8TeV','2000<pt<8000',path='highptrange2TeVto8TeV',FillerTools = ["pt","m","eta","phi","eta;phi",] ),
    SelectSpec( 'highptrange1TeVto2TeV','1000<pt<2000',path='highptrange1TeVto2TeV',FillerTools = ["pt","m","eta","phi","eta;phi",] ),
    SelectSpec( 'highptrange500GeVto1TeV','500<pt<1000',path='highptrange500GeVto1TeV',FillerTools = ["pt","m","eta","phi","eta;phi",] ),
    SelectSpec( 'highptrange200GeVto500GeV','200<pt<500',path='highptrange200GeVto500GeV',FillerTools = ["pt","m","eta","phi","eta;phi",] ),

    # Selecting jets failing the LooseBad selection from the JetCleaningTool.
    SelectSpec( 'LooseBadFailedJets', 'LooseBad', InverseJetSel=True, 
                FillerTools = ["pt","phi","eta","m","EMFrac","LArQuality","HECFrac","nconstit","N90Constituents","phi;eta","phi;eta;e","phi;eta;pt",
                ]),   

    # Selecting jets passing the LooseBad selection from the JetCleaningTool.
    SelectSpec( 'LooseBadJets',
                'LooseBad', # this is not in the form 'min<x<max', so it will be assumed 'LooseBad' is an entry existing in JetStandardHistoSpecs.knownSelector
                FillerTools = [ "pt","eta","phi","m","EMFrac","LArQuality","HECFrac","nconstit","N90Constituents","phi;eta","phi;eta;e","phi;eta;pt",
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
    # histos common to all topo jets
    # to be filled ...
]

pflowHistosSpec = [
    # histos common to all PFlow jets
    # to be filled ...
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
