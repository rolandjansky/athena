#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigTLAMonitorAlgorithm.py
@author E. Nagy
@author K. Krizka
@date 2022-06-14
@brief Example trigger python configuration for the Run III AthenaMonitoring package, based on the example by C Burton and P Onyisi
'''

def histdefs_particle(pprefix, plabel):
    """
    Return a list of histogram definitions for particle kinematics.

    @param pprefix Prefix of histogram names
    @param plabel Human-readable name of the particle to use in labeling axes
    """
    histograms=[
        {'name'  :f'n{pprefix}',
         'ylabel':'events',
         'xlabel':f'number of {plabel}s',
         'xmin'  :-0.5,
         'xmax'  :19.5,
         'xbins' :20},
        {'name'  :f'{pprefix}pt',
         'ylabel':f'{plabel}s',
         'xlabel':f'{plabel} p_{{T}}',
         'xunit' :'GeV',
         'xmin'  :0,
         'xmax'  :750,
         'xbins' :150},
        {'name'  :f'{pprefix}eta',
         'ylabel':f'{plabel}s',
         'xlabel':f'{plabel} #eta',
         'xmin'  :-4,
         'xmax'  : 4,
         'xbins' :100},
        {'name'  :f'{pprefix}phi',
         'ylabel':f'{plabel}s',
         'xlabel':f'leading {plabel} #phi',
         'xmin'  :-3.5,
         'xmax'  : 3.5,
         'xbins' :100},
        {'name'  :f'{pprefix}0pt',
         'ylabel':'events',
         'xlabel':f'leading {plabel} p_{{T}}',
         'xunit' :'GeV',
         'xmin'  :0,
         'xmax'  :750,
         'xbins' :150},
        {'name'  :f'{pprefix}0eta',
         'ylabel':'events',
         'xlabel':f'leading {plabel} #eta',
         'xmin'  :-4,
         'xmax'  : 4,
         'xbins' :100},
        {'name'  :f'{pprefix}0phi',
         'ylabel':'events',
         'xlabel':f'leading {plabel} #phi',
         'xmin'  :-3.5,
         'xmax'  : 3.5,
         'xbins' :100}
    ]
    return histograms

def histdefs_dR(p0prefix,p1prefix,p0label,p1label):
    """
    Return a list of histogram definitions for particle dR

    @param p0prefix Prefix of first particle in histogram
    @param p1prefix Prefix of second particle in histogram
    @param p0label Human-readable name of the first particle to use in labeling axes
    @param p1label Human-readable name of the second particle to use in labeling axes
    """
    histograms=[
        {'name'  :f'{p0prefix}{p1prefix}dr',
         'ylabel':'events',
         'xlabel':f'#DeltaR_{{{p0label}, {p1label}}}',
         'xmin'  :0,
         'xmax'  :6,
         'xbins' :60}]
    return histograms

def TrigTLAMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigTLAMonitorAlgorithm.py' )

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    # EN: Not needed for the moment
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigTLAAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper.

    from AthenaConfiguration.ComponentFactory import CompFactory
    trigTLAMonAlg = helper.addAlgorithm(CompFactory.TrigTLAMonitorAlgorithm,'TrigTLAMonAlg')

    ### STEP 3 ###
    # Edit properties of a algorithm
    # to enable a trigger filter, for example:
    #trigTLAMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #trigTLAMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'
    trigTLAMonAlg.TriggerChain = ''

    ### STEP 4 ###
    # Read in the TLA trigger chain names as submitted to the MonGroups

    tla_triglist = []
    # Trigger list from monitoring groups: signatures = TLAMon to be agreed w/ MonGroups management
    from TrigConfigSvc.TriggerConfigAccess import getHLTMonitoringAccess
    moniAccess=getHLTMonitoringAccess(inputFlags)
    TLAChainsE=moniAccess.monitoredChains(signatures="tlaMon",monLevels=["t0"])
    log.info (" ==> tla_chainlist t0: %s", TLAChainsE)
    for chain in TLAChainsE:
        chain = "E_"+chain
        tla_triglist.append(chain)
    TLAChainsS=moniAccess.monitoredChains(signatures="tlaMon",monLevels=["shifter"])
    log.info (" ==> tla_chainlist shifter:  %s", TLAChainsS)
    for chain in TLAChainsS:
        chain = "S_"+chain
        tla_triglist.append(chain)
    log.info (" ==> tla_triglist:  %s", tla_triglist)

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    TLAMonGroup = helper.addGroup(trigTLAMonAlg,'TrigTLAMonitor','HLT/TLAMon/')


    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    histdefs=[]
    histdefs+=histdefs_particle('jet'  ,'jet')
    histdefs+=histdefs_particle('pfjet','particle-flow jet')
    histdefs+=histdefs_particle('ph'   ,'photon')
    histdefs+=histdefs_particle('muon' ,'muon')
    histdefs+=histdefs_dR      ('jet0'  ,'jet1'  , 'leading jet'   ,'subleading jet'   )
    histdefs+=histdefs_dR      ('pfjet0','pfjet1', 'leading pf jet','subleading pf jet')
    histdefs+=histdefs_dR      ('jet0'  ,'ph0'   , 'leading jet'   ,'leading photon'   )
    histdefs+=histdefs_dR      ('pfjet0','ph0'   , 'leading pf jet','leading photon'   )

    AllChains = []
    for chain in tla_triglist :
        AllChains.append(chain[2:])

        for histdef in histdefs:
            HistName = histdef['name'] + '_' + chain[2:]

            xlabel=histdef.get('xlabel',histdef['name'  ])
            if 'xunit' in histdef:
                xlabel+=f' [{histdef["xunit"]}'
            ylabel=histdef.get('ylabel',histdef['ylabel'])

            if chain[0:1] == "E" :
                TLAMonGroup.defineHistogram(HistName, title=f'Distribution of {histdef["name"]};{xlabel};{ylabel}',
                                            path='Expert/' +chain[2:],xbins=histdef['xbins'],xmin=histdef['xmin'],xmax=histdef['xmax'])
            if chain[0:1] == "S" :
                TLAMonGroup.defineHistogram(HistName, title=f'Distribution of {histdef["name"]};{xlabel};{ylabel}',
                                            path='Shifter/'+chain[2:],xbins=histdef['xbins'],xmin=histdef['xmin'],xmax=histdef['xmax'])

    log.info (" ==> In TrigTLAMonitorAlgorithm.py: AllChains list:  %s", AllChains)
    trigTLAMonAlg.AllChains = AllChains

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()

if __name__=='__main__':
    # Get run options
    import argparse
    parser = argparse.ArgumentParser()
    # AOD file copied from Bjets, just to avoid compilation warning, for TLA need to be changed!!!!
    parser.add_argument('input',nargs='?',default='/afs/cern.ch/work/e/enagy/public/ARTfiles/MCtest160322.AOD.pool.root')
    parser.add_argument('--data',action='store_true',help='Input file is data.')
    args = parser.parse_args()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)
    # from AthenaCommon.Constants import INFO
    # log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Input files (AOD or other files, e.g. costumized RAW file, to be defined 
    ConfigFlags.Input.Files = [args.input]
    ConfigFlags.Input.isMC = not args.data

    # Output file (root)

    ConfigFlags.Output.HISTFileName = 'TrigTLAMonitorOutput.root'

    # ConfigFlags.Trigger.triggerMenuSetup="Physics_pp_v7_primaries"
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigTLAMonitorAcc = TrigTLAMonConfig(ConfigFlags)
    cfg.merge(trigTLAMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigTLAMonitorAcc.getEventAlgo('TrigTLAMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    Nevents = 25
    #cfg.run(Nevents)
    cfg.run() #use cfg.run(20) to only run on first 20 events


