#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigHLTMonitorAlgorithm.py
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-09-10
@brief TrigHLTMonitoring top-level files, general monitoring
'''

def createHLTDQConfigFlags():
    from AthenaConfiguration.AthConfigFlags import AthConfigFlags
    acf=AthConfigFlags()

    acf.addFlag('DQ.Steering.HLT.doGeneral', True)
    acf.addFlag('DQ.Steering.HLT.doEgamma', True)
    acf.addFlag('DQ.Steering.HLT.doMET', True)
    acf.addFlag('DQ.Steering.HLT.doJet', True)
    acf.addFlag('DQ.Steering.HLT.doBjet', True)
    acf.addFlag('DQ.Steering.HLT.doCalo', True)
    acf.addFlag('DQ.Steering.HLT.doMuon', True)
    acf.addFlag('DQ.Steering.HLT.doBphys', True)
    acf.addFlag('DQ.Steering.HLT.doMinBias', True)
    acf.addFlag('DQ.Steering.HLT.doTau', True)

    return acf


def TrigHLTMonTopConfig(inputFlags):
    '''Function to configure some algorithms in the monitoring system.'''

    ########
    #HLT top-level steering

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    if inputFlags.DQ.Steering.HLT.doGeneral:
        from TrigHLTMonitoring.TrigHLTMonitorAlgorithm import TrigHLTMonConfig
        result.merge(TrigHLTMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doEgamma:
        from TrigEgammaMonitoring.TrigEgammaMonitorAlgorithm import TrigEgammaMonConfig
        result.merge(TrigEgammaMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doMET:
        from TrigMETMonitoring.TrigMETMonitorAlgorithm import TrigMETMonConfig
        result.merge(TrigMETMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doJet:
        from TrigJetMonitoring.TrigJetMonitorAlgorithm import TrigJetMonConfig
        result.merge(TrigJetMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doBjet:
        from TrigBjetMonitoring.TrigBjetMonitorAlgorithm import TrigBjetMonConfig
        result.merge(TrigBjetMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doCalo:
        from TrigCaloMonitoring.TrigCaloMonitorAlgorithm import TrigCaloMonConfig
        result.merge(TrigCaloMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doMuon:
        from TrigMuonMonitoringMT.TrigMuonMonitoringMTConfig import TrigMuonMonConfig
        result.merge(TrigMuonMonConfig(inputFlags))

    if inputFlags.DQ.Steering.HLT.doBphys:
        from TrigBphysMonitoring.TrigBphysMonitorAlgorithm import TrigBphysMonConfig
        result.merge(TrigBphysMonConfig(inputFlags))
        
    if inputFlags.DQ.Steering.HLT.doMinBias:
        from TrigMinBiasMonitoring.TrigMinBiasMonitoringMT import TrigMinBias
        result.merge(TrigMinBias(inputFlags))

    if inputFlags.DQ.Steering.HLT.doTau:
        from TrigTauMonitoring.TrigTauMonitorAlgorithm import TrigTauMonConfig
        result.merge(TrigTauMonConfig(inputFlags))

    return result



def TrigHLTMonConfig(inputFlags):
    '''Function to configure some algorithms in the monitoring system.'''

    ################################
    # HLT general monitoring


    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigHLTAthMonitorCfg')


    ### STEP 2 ###
    # Adding algorithms to the helper. 
    # The added algorithm must exist as a .h file 

    from AthenaConfiguration.ComponentFactory import CompFactory
    trigHLTMonAlg = helper.addAlgorithm(CompFactory.TrigHLTMonitorAlgorithm,'TrigHLTMonAlg')
    signatureTrigHLTMonAlg = helper.addAlgorithm(CompFactory.TrigHLTMonitorAlgorithm,'SignatureTrigHLTMonAlg')


    ### STEP 3 ###
    # Edit properties of a algorithm
    # Placeholder

    ### STEP 4 ###
    # Add a generic monitoring tool (a "group" in old language). 
    myGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTMonitor',
        'HLT/ResultMon/'
    )

    # Add a GMT for the other example monitor algorithm
    signatureGroup = helper.addGroup(signatureTrigHLTMonAlg,'TrigHLTMonitor','HLT/ResultMon/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    
    myGroup.defineHistogram('HLTResultHLT', title='HLT Result PLACEHOLDER;result;Events',
                            path='',xbins=3,xmin=0,xmax=3)
    #TODO Need to set alphanumeric bin labels. C code:
    #std::string help[] = {"HLTResult","isAccepted","isPassThrough"};
    #for(int i=1;i<=3;i++) hist(resultname)->GetXaxis()->SetBinLabel(i,help[i-1].c_str());


    signatureGroup.defineHistogram('placeholder',title='Placeholder;X;Y',
                                  path='Electrons',xbins=1000000,xmin=-0.5,xmax=999999.5)

    ### STEP 6 ###
    # Finalize. 
    return helper.result()
    

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TrigHLTMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigHLTMonitorAcc = TrigHLTMonConfig(ConfigFlags)
    cfg.merge(trigHLTMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigHLTMonitorAcc.getEventAlgo('TrigHLTMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

