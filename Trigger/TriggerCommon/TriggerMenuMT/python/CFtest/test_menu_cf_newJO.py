#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentAccumulator import CompFactory
from AthenaCommon.CFElements import seqAND, seqOR, parOR
from TriggerMenuMT.HLT.Config.Utility.HLTMenuConfig import HLTMenuConfig



def triggerRunCfgEmu( flags, menu=None ):
    """
    top of the trigger config (for real triggering online or on MC)
    Returns: ca only
    """
    log.info("triggerRunCfgEmu")    

    acc = ComponentAccumulator()

    # L1ConfigSvc needed for HLTSeeding
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge( L1ConfigSvcCfg(flags) )

    acc.addSequence( seqOR( "HLTTop") )

    acc.addSequence( parOR("HLTBeginSeq"), parentName="HLTTop" )
    # bit of a hack as for "legacy" type JO a seq name for cache creators has to be given,
    # in newJO realm the seqName will be removed as a comp fragment shoudl be unaware of where it will be attached    
    from TriggerMenuMT.CFtest.EmuStepProcessingConfig import emulateHLTSeedingCfg
    hltSeedingAcc = emulateHLTSeedingCfg(flags)
    acc.merge( hltSeedingAcc,  sequenceName="HLTBeginSeq" )
    
    #add here emulation events
    from TriggerMenuMT.CFtest.EmuStepProcessingConfig import generateEmuEvents
    generateEmuEvents()    

    # detour to the menu here, (missing now, instead a temporary hack)
    if menu:
        menuAcc = menu( flags )
        HLTSteps = menuAcc.getSequence( "HLTAllSteps" )
        log.info( "Configured menu with %d steps", len(HLTSteps.Members))
        acc.merge( menuAcc, sequenceName="HLTTop")

    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters,triggerSummaryCfg, \
        triggerMonitoringCfg, collectDecisionObjects, collectHypoDecisionObjects, \
        triggerOutputCfg

    # collect hypothesis algorithms from all sequence
    hypos = collectHypos( HLTSteps )
    filters = collectFilters( HLTSteps )
    acc.addSequence( parOR("HLTEndSeq"), parentName="HLTTop" )
    acc.addSequence( seqAND("HLTFinalizeSeq"), parentName="HLTEndSeq" )

    summaryAcc, summaryAlg = triggerSummaryCfg( flags, hypos )
    acc.merge( summaryAcc, sequenceName="HLTFinalizeSeq" )
    acc.addEventAlgo( summaryAlg, sequenceName="HLTFinalizeSeq" )
    # TODO: Add end-of-event sequences here (port from HLTCFConfig.py)

    #once menu is included we should configure monitoring here as below
    hltSeedingAlg = hltSeedingAcc.getEventAlgo("HLTSeeding")

    monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, filters, hltSeedingAlg )
    acc.merge( monitoringAcc, sequenceName="HLTEndSeq" )
    acc.addEventAlgo( monitoringAlg, sequenceName="HLTEndSeq" )


    decObj = collectDecisionObjects( hypos, filters, hltSeedingAlg, summaryAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
    log.info( "Number of decision objects found in HLT CF %d", len( decObj ) )
    log.info( "Of which, %d are the outputs of hypos", len( decObjHypoOut ) ) 
    log.info( decObj )
    

    # Add HLT Navigation to EDM list
    from TrigEDMConfig import TriggerEDMRun3
    log.info( "Number of EDM items before adding navigation: %d", len(TriggerEDMRun3.TriggerHLTListRun3))
    TriggerEDMRun3.addHLTNavigationToEDMList(TriggerEDMRun3.TriggerHLTListRun3, decObj, decObjHypoOut)
    log.info( "Number of EDM items after adding navigation: %d", len(TriggerEDMRun3.TriggerHLTListRun3))

    # Configure output writing
    outputAcc, edmSet = triggerOutputCfg( flags, hypos )
    acc.merge( outputAcc, sequenceName="HLTTop" )

    
    return acc


def generateEmuMenu(ConfigFlags):
    """
    generate emulated chain manually, replacing loadChains in real HLT
    """
    log.info("generateEmuMenu")
    # make menu manually here:
    from TriggerMenuMT.CFtest.EmuStepProcessingConfig import generateChainsManually       
    generateChainsManually(0x2)

    from TriggerMenuMT.HLT.Config.ControlFlow.HLTCFConfig_newJO import generateDecisionTree
    import TriggerMenuMT.HLT.Config.ControlFlow.HLTCFConfig_newJO
    # set DEBUG flag on the control-flow builder (before building)
    TriggerMenuMT.HLT.Config.ControlFlow.HLTCFConfig_newJO.log.setLevel(DEBUG)
    menuAcc = generateDecisionTree(ConfigFlags, HLTMenuConfig.configsList())
    menuAcc.wasMerged()   
    menuAcc.printConfig()
    log.info('CF is built')
    # # generate JOSON representation of the config
    from TriggerMenuMT.HLT.Config.JSON.HLTMenuJSON import generateJSON_newJO
    generateJSON_newJO(ConfigFlags, HLTMenuConfig.dictsList(), HLTMenuConfig.configsList(), menuAcc.getSequence("HLTAllSteps"))

    from TriggerMenuMT.HLT.Config.JSON.HLTPrescaleJSON import generateJSON_newJO as generatePrescaleJSON_newJO
    generatePrescaleJSON_newJO(ConfigFlags, HLTMenuConfig.dictsList(), HLTMenuConfig.configsList())
   
    from TriggerMenuMT.HLT.Config.JSON.HLTMonitoringJSON import generateDefaultMonitoringJSON
    generateDefaultMonitoringJSON(ConfigFlags)

    return menuAcc




if __name__ == "__main__":
   
    from AthenaCommon.Logging import logging
    from AthenaCommon.Constants import DEBUG
    log = logging.getLogger('test_menu_cf_newJO')
    log.setLevel(DEBUG)
  
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    
    from AthenaConfiguration.Enums import Format
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    

    #flags.Exec.OutputLevel=DEBUG # to swicth all DEBUG (crashing in HLTSeeding now)
    flags.Concurrency.NumThreads = 1
    flags.Exec.MaxEvents = 4
    flags.Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO.e4993_s3214_d1697_tid26494053_00/RDO.26494053._000001.pool.root.1"]
    flags.Trigger.doHLT = True
    flags.Trigger.triggerMenuSetup="Dev_pp_run3_v1" 
    flags.Trigger.doRuntimeNaviVal=False 
    flags.Trigger.EDMVersion = 3
        
    flags.Scheduler.CheckDependencies = True
    flags.Scheduler.ShowDataDeps = True
    flags.Scheduler.ShowDataFlow = True
    flags.Scheduler.ShowControlFlow = True
    
    #needed even if not used
    flags.IOVDb.GlobalTag = "CONDBR2-HLTP-2018-03"
    flags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'
    
    # command line handling
    # options that are defined in: AthConfigFlags are handled here
    # they override values from above
    parser = flags.getArgumentParser()
    args = flags.fillFromArgs(parser=parser)
    log.info("Command line arguments:")
    import sys
    log.info(" ".join(sys.argv))
    flags.lock()
    # Enable when debugging deduplication issues
    # ComponentAccumulator.debugMode = "trackCA trackEventAlog ... and so on"
    
    acc = MainServicesCfg(flags)
    
    acc.getService('AvalancheSchedulerSvc').VerboseSubSlots = True
    
    # this delcares to the scheduler that EventInfo object comes from the input
    loadFromSG = [('xAOD::EventInfo', 'StoreGateSvc+EventInfo')]
    
    if flags.Input.Format is Format.BS:
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags))
    else:
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        acc.merge(PoolReadCfg(flags))
    
    
    from TriggerJobOpts.TriggerHistSvcConfig import TriggerHistSvcConfig
    acc.merge(TriggerHistSvcConfig(flags))
        
    menu = triggerRunCfgEmu(flags, menu=generateEmuMenu)
    
    # uncomment to obtain printout of menu (and associated components)
    menu.printConfig(withDetails=True, summariseProps=True)
    acc.merge(menu)
    
    from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
    acc.merge(LumiBlockMuWriterCfg(flags), sequenceName="HLTBeginSeq")
    acc.addEventAlgo(CompFactory.SGInputLoader(Load=loadFromSG), sequenceName="AthAlgSeq")
    
    # The L1 presacles do not get created in the menu setup
    from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu, createL1PrescalesFileFromMenu
    generateL1Menu(flags)
    createL1PrescalesFileFromMenu(flags)
    
    
    fName =  args.configOnly if args.configOnly else "test_menu_cf_newJO.pkl" 
    log.info("Storing config in the file %s ", fName)
    with open(fName, "wb") as p:
        acc.store(p)
        p.close()
    
    if not args.configOnly:
        log.info("Running ...")
        status = acc.run()
        if status.isFailure():
            import sys
            sys.exit(1)
    else:
        log.info("The configOnly option used ... exiting.")
    
    

