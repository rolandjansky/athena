#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file JetMonitoringExample.py
@author P-A. Delsart
@date 2019-03-12
@brief Example python configuration for the Run III Jet Monitoring
'''


def jetMonitoringExampleConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'ExampleAthMonitorCfg')

    ### STEP 2 ###
    # Declare a configuration dictionnary for a JetContainer
    from JetMonitoring.JetMonitoringConfig import JetMonAlgSpec, HistoSpec,  SelectSpec, ToolSpec
    #from JetMonitoring.JetStandardHistoSpecs import knownHistos
    
    # we use a specialized dictionnary (JetMonAlgSpec) which will be translated into the final C++ tool
    ak4conf = JetMonAlgSpec(
        "ak4lcMon",
        JetContainerName = "AntiKt4LCTopoJets",
        # TriggerChain = '' ,  --> no trigger filter yet.
    )

    # Then set the list of all histos we want to fill with this alg.
    # We append each histo specification as arguments of the appendHistos()
    # method of our specialized dictionnary.
    # (again, each of this histo spec will be translated as C++ tool and then passed into
    #  'FillerTools' property of the actual JetMonitoring C++ class)

    # Now start filling the histo spec list    
    ak4conf.appendHistos(
        # This python list can be filled with various type of entries :
        # We can give aliases of standard spec :        
        "pt",  
        "highpt",
        "m",
        "eta",
        
        "leadingJetsRel",

        # See knownHistos in JetStandardHistoSpecs.py for the list of standard specification.
        

        # or we can directly add our custom histo specification in the form of a HistoSpec :
        HistoSpec('HECFrac', (50,-0.1,1.4), title="HEC E fraction;HEC frac;" ),
        # the basic call is : HistoSpec( variable, histobins, title='histotile;xtitle,ytitle')

        
        # Say we want a 2nd 'pt' plot but with different a binning than in the standard spec.
        # WARNING : we can not re-use the same spec name in a given JetMonitoringAlg !!!
        # so we give a new name AND we specify the actual variable with the argument 'xvar'
        #   (the ':GeV' means the variable is to be set at GeV scale)
        HistoSpec( 'lowpt',  (100,0,150) , title='p_{T};p_{T} [GeV];', xvar='pt:GeV'),            
        # An equivalent solution would have been to clone the existing spec like in :
        # knownHistos.pt.clone('lowpt',bins= (100,0,200) ),


        # 2D histos are usually refered to by concatenating vars with a ';' as in 'varx;vary' 
        "pt;m",
        # if the 'vax;vary' alias doesn't exist in knownHistos but 'varx' and 'vary'
        # do exist, then a spec fot 'vax;vary' will be automatically generated.
        "pt;eta",

        
        # TProfile : to be done ...

        # TProfile2D : just use 3 variables. For now the sytem will automatically
        #  interpret it as a TProfile2D (the 3rd variable being profiled)
        "phi;eta;e", # --> Average Energy vs pt and eta

        
        # Histograms build from a selection of filtered jets.
        # we declare a selection using a SelectSpec dictionnary.  Here for  central jets :
        SelectSpec( 'central',   # the name of the selection
                    '|eta|<0.5', # selection expression. The form 'min<var<max' is automatically interpreted.

                    # Then give a list of histo specs for the filtered jets
                    #  these histos will be put under the 'central/' sub-directory
                    FillerTools = [
                        # exactly the same type of entries as for the JetMonAlgSpec/
                        # we can re-use aliases since we're going in a different path.
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
    

    
    # then we turn the full specification into properly configured algorithm and tools.
    # we use the method 'toAlg()' defined for the specialized dictionnary 'JetMonAlgSpec'
    ak4conf.toAlg(helper) 


    ########### A more elaborate example for a trigger efficiency tool.
    # We schedule a new JetAlg which will be acting only when a TriggerChain fired (using the TriggerChain from the base classes).
    # We'll plot 1 histo build by a dedicated JetHistoTriggEfficiency tool.
    # So we'll have to explicitely give a specification via the generic dicionnary 'ToolSpec'
    # This implies defining a little function which declares to the monitoring framework which variables to histogram and how.
    #  this is done here.
    def defineHistoForJetTrigg(conf, parentAlg, monhelper , path):
        # create a monitoring group with the histo path starting from the parentAlg
        group = monhelper.addGroup(parentAlg, conf.Group,  'Jets/'+parentAlg.JetContainerName)
        # define the histogram
        group.defineHistogram('trigPassed,jetVar',title='titletrig', type="TEfficiency", path='jetTrigg', xbins=100 , xmin=0, xmax=500000. ,)

    
    from JetMonitoring.JetMonitoringConfig import retrieveVarToolConf
    trigConf = JetMonAlgSpec( # the usual JetMonAlgSpec 
        "ak4lcTrigMon",
        JetContainerName = "AntiKt4LCTopoJets",
        TriggerChain = "HLT_j260",
        )
    trigConf.appendHistos(
            # we pass directly the ToolSpec
            ToolSpec('JetHistoTriggEfficiency', 'jetTrigg',
                     # below we pass the Properties of this JetHistoTriggEfficiency tool :
                     Group='jetTrigGroup',
                     Var=retrieveVarToolConf("pt"), # In this context we can not just pass a str alias to describe a histo variable
                                                    # so we use retrieveVarToolConf("pt") which returns a full specification for the "pt" histo variable.
                     ProbeTrigChain="HLT_j260",defineHistoFunc=defineHistoForJetTrigg),
    )

    trigConf.toAlg(helper)
    
    return helper.result() # the AthMonitorCfgHelper returns an accumulator to be used by the general configuration system.

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO #,DEBUG 
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    #fileName = 'TestAOD.root'
    fileName = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [fileName]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'JetExampleMonitorOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = jetMonitoringExampleConfig(ConfigFlags)
    
    cfg.merge(exampleMonitorAcc)
    #cfg.run()
    cfg.run(100)
