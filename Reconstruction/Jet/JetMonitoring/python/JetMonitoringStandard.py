#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file JetMonitoringExample.py
@author P-A. Delsart
@date 2019-03-12
@brief Main python configuration for the Run III Jet Monitoring
'''


def jetMonAntiKt4LCTopo():
    '''Sets-up AntiKt4 monitoring.'''


    # Declare a configuration dictionnary for a JetContainer
    from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec,  SelectSpec, ToolSpec
    
    # we use a specialized dictionnary (JetMonAlgSpec) which will be translated into the final C++ tool
    ak4conf = JetMonAlgSpec(
        "ak4lcMon",
        JetContainerName = "AntiKt4LCTopoJets",
        # TriggerChain = '' ,  --> no trigger filter yet.
    )


    # Now start filling the histo spec list    
    ak4conf.appendHistos(
        # This python list can be filled with various type of entries :
        # We can give aliases of standard spec :        
        "pt",  
        "highpt",
        "m",
        "eta",
        
        "leadingJetsRel",

        

        # or we can directly add our custom histo specification in the form of a HistoSpec :
        HistoSpec('HECFrac', (50,-0.1,1.4), title="HEC E fraction;HEC frac;" ),
        # the basic call is : HistoSpec( variable, histobins, title='histotile;xtitle,ytitle')
        

        # 2D histos
        "pt;m",
        "pt;eta",


        # TProfile2D : just use 3 variables. For now the sytem will automatically
        #  interpret it as a TProfile2D (the 3rd variable being profiled)
        "phi;eta;e", # --> Average Energy vs pt and eta

        
        # Histograms build from a selection of filtered jets.
        SelectSpec( 'central',   # the name of the selection
                    '|eta|<0.5', # selection expression. The form 'min<var<max' is automatically interpreted.

                    FillerTools = [
                        "pt",
                        "m",
                        "eta",
                    ] ),

        # another selection : only sub-leading jets
        SelectSpec( 'subleading',
                    '', # no selection on variables
                    SelectedIndex=1, # force 2nd (sub-leading) jet (we would set 0 for leading jets)
                    path='standardHistos', # force the path where the histos are saved in the final ROOT file
                    FillerTools = [
                        "pt",
                        "m",
                    ] ),
        
        
        SelectSpec( 'highJVF',
                    '0.3<JVF[0]', # JVF is a vector<float> for each jets. Here we cut on the 0th entry of this vector
                    FillerTools = [
                        "pt",
                    ] ),
        # Selecting jets passing the LooseBad selection from the JetCleaningTool.
        SelectSpec( 'LooseBadJets',
                    'LooseBad', # this is not in the form 'min<x<max', so it will be assumed 'LooseBad' is an entry existing in JetStandardHistoSpecs.knownSelector
                    FillerTools = [
                        "pt",
                    ] ),


    )
    return ak4conf

    



def standardJetMonitoring(inputFlags):
    """Standard jet monitoring function to be inserted from top-level algs. 
    returns an a component accumulator as given by AthMonitorCfgHelper.result()
    Details of what goes into jet monitoring will be implemented by dedicated functions such as jetMonAntiKt4LCTopo().
    """

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'JetMonitoring')

    ak4conf = jetMonAntiKt4LCTopo()
    ak4conf.toAlg(helper) # adds the ak4conf as a JEtMonitoringAlg to the helper
    
    return helper.result() # the AthMonitorCfgHelper returns an accumulator to be used by the general configuration system.
