#--------------------------------------------------------------
#-- Job Options for HLTMonitoring
#--------------------------------------------------------------

#############################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigHLTMonitoring/addMonTools' )

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

# in order to check available detectors
from RecExConfig.RecFlags  import rec

#### flags ####

# if not 'HLTMonFlags' in dir():
#   from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
#
# if not 'DQMonFlags' in dir():
#   from AthenaMonitoring.DQMonFlags import DQMonFlags

################ Mon Tools #################

# Bphys - Running JpsiFinder
if HLTMonFlags.doMonTier0:
    if HLTMonFlags.doBphys and rec.doInDet:
      include( "TrigBphysMonitoring/RunJpsiFinder.py" )

topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

#Global HLTMonTool
if HLTMonFlags.doGeneral:
    try:
        from TrigHLTMonitoring.TrigHLTMonitoringConfig import HLTGeneralTool
        HLTMonManager.AthenaMonTools += HLTGeneralTool()
    except:
         log.info("Problems with the general HLTMonTool, tool not enabled")

if HLTMonFlags.doMonTier0:

    # HLTEgammaMonTool - Calculates basic efficiencies
    if HLTMonFlags.doEgamma:
      try:
       if rec.doCalo and rec.doInDet:
#        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import HLTEgammaMonitoringTool
#        HLTMonManager.AthenaMonTools += HLTEgammaMonitoringTool()
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
        HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()
      except:
        log.info("Problems with HLTEgammaTool, tool not enabled")

    # HLTCaloTool - Makes basic comparisons between online/offline
    if HLTMonFlags.doCalo:
      try:
       if rec.doCalo and rec.doInDet:
        from TrigCaloMonitoring.TrigCaloMonitoringConfig import HLTCaloMonitoringTool
        HLTMonManager.AthenaMonTools+=HLTCaloMonitoringTool()
      except:
        log.info("Problems with HLTCaloTool, tool not enabled")

    # HLTMuonMonTool - Multiple Muon algorithms EDM Dumper
    if HLTMonFlags.doMuon:
      try:
        from TrigMuonMonitoring.TrigMuonMonitoringConfig import TrigMuonMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMuonMonitoringTool()
      except:
        log.info("Problems with HLTMuonTool, tool not enabled")

    # IDtrk HLTMonTool
    if HLTMonFlags.doIDtrk:
      try:
        from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()
      except:
        log.info("Problems with HLTIDtrkTool, tool not enabled")

    # MET HLTMonTool
    if HLTMonFlags.doMET:
      try:
        from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
        HLTMonManager.AthenaMonTools += HLTMETMonitoringTool()
      except:
        log.info("Problems with HLTMETTool, tool not enabled")

    # Tau HLTMonTool
    if HLTMonFlags.doTau:
      try:
        from TrigTauMonitoring.TrigTauMonitoringConfig import TrigTauMonitoringTool
        HLTMonManager.AthenaMonTools += TrigTauMonitoringTool()
      except:
        log.info("Problems with HLTTauTool, tool not enabled")

    # Jet HLTMonTool
    if HLTMonFlags.doJet:
      try:
        from TrigJetMonitoring.TrigJetMonitoringConfig import TrigJetMonitoringTool
        HLTMonManager.AthenaMonTools += TrigJetMonitoringTool()
      except:
        log.info("Problems with HLTJetTool, tool not enabled")

    # b-jet HLTMonTool
    if HLTMonFlags.doBjet and rec.doInDet:
      try:
        from TrigBjetMonitoring.TrigBjetMonitoringConfig import TrigBjetMonitoringConfig
        HLTMonManager.AthenaMonTools += TrigBjetMonitoringConfig()
      except:
        log.info("Problems with HLTBjetTool, tool not enabled")

    # B-phys HLTMonTool
    if HLTMonFlags.doBphys:
      try:
        from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
        HLTMonManager.AthenaMonTools += TrigBphysMonitoringTool()
      except:
        log.info("Problems with HLTBphysTool, tool not enabled")

    # MinBias HLTMonTool
    if HLTMonFlags.doMinBias:
      try:
        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMinBiasMonitoringTool()
      except:
        log.info("Problems with HLTMinBiasDumpTool, tool not enabled")

    # MinBias HLTMonTool
    #    if HLTMonFlags.doIDJpsiMon:
    #      try:
    #        from TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig import TrigIDJpsiMonitoringTool
    #        HLTMonManager.AthenaMonTools += TrigIDJpsiMonitoringTool()
    #      except:
    #        log.info("Problems with TrigIDJpsiMonTool, tool not enabled")


################ Dump Tools ################

if HLTMonFlags.doDump:

    # HLTEgammaDumpTool - HLT EDM Egamma Dumper
    if HLTMonFlags.doEgamma:
      try:
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import HLTEgammaMonitoringDumpTool
        HLTMonManager.AthenaMonTools += HLTEgammaMonitoringDumpTool()
      except:
        log.info("Problems with HLTEgammaDumpTool, tool not enabled")


    #Make the custom tau TTP ntuples.
    if HLTMonFlags.doOfflineTauTTP :
     try:
      from TrigTauPerformAthena.TrigTauPerformAthenaConf import TrigTauPerformAthenaAlgo
      ttpalgo = TrigTauPerformAthenaAlgo()
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      topSequence += ttpalgo
     except:
      log.info("Problems with OfflineTauTTP, tool not enabled")

############################################
########## Menu-aware Monitoring ###########

#HLTMonFlags.doMaM = True
#HLTMonFlags.doMaM_ApplyMCK = True

if HLTMonFlags.doMaM == True:

    # MaM needs to check whether it is running in a Trigger reprocessing job or not, and start an instance of MaM connected to the correct database accordingly
    trigger_reco_tf_job = False

    if hasattr(runArgs, "DBserver") and runArgs.DBserver == "TRIGGERDBREPR":
        trigger_reco_tf_job = True
    elif hasattr(runArgs, "triggerConfig") and "TRIGGERDBREPR" in runArgs.triggerConfig:
        trigger_reco_tf_job = True

    from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring
    if trigger_reco_tf_job:
        log.info("Will attempt to doMaM with TRIGGERDBREPR")
        mam = MenuAwareMonitoring("TRIGGERDBREPR_R")
    else:
        log.info("Will attempt to doMaM with TRIGGERDB")
        mam = MenuAwareMonitoring()

    if mam.ms.connected_to_oracle == False:
        log.error("Menu-Aware Monitoring: Cannot doMaM without database connection")
    else:
        # if a specific Monitoring Configuration Key (MCK) has been set, then use it
        if HLTMonFlags.MCK.StoredValue > 0:
            if mam.ms.oi.check_if_mck_id_exists( HLTMonFlags.MCK.StoredValue ):
                if mam.does_mck_athena_version_match_current_athena_version( HLTMonFlags.MCK.StoredValue ):
                    log.info("MCK found via transform %d" % HLTMonFlags.MCK.StoredValue )
                    # if we are applying configurations to tools according to an MCK, then do that here
                    if  HLTMonFlags.doMaM_ApplyMCK:
                        log.info("Applying MCK %d" % HLTMonFlags.MCK.StoredValue)
                        mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                else:
                    log.error("MCK for a different release found via transform: %d" % HLTMonFlags.MCK.StoredValue )
            else:
                log.error("MCK found via transform (%d) is not a vaid MCK." % HLTMonFlags.MCK.StoredValue )

        # if HLTMonFlags.MCK is -1 (the default) we try to determine the MCK automatically, as long as this is not MC
        is_not_sim = True
        from RecExConfig.InputFilePeeker import inputFileSummary
        if inputFileSummary.__contains__('evt_type'):
            if 'IS_SIMULATION' in inputFileSummary['evt_type']:
                log.info("Will not try to get MCK automatically as we are running on MC")
                is_not_sim = False

        if HLTMonFlags.MCK.StoredValue == -1 and is_not_sim:
            if trigger_reco_tf_job:
                # for trigger repro jobs, need to check the transform arguments and get the SMK from there, then use the linked MCK.
                # no COOL interaction in these jobs
                SMKrepr = None

                if hasattr(runArgs, "DBsmkey") and runArgs.DBsmkey!="NONE":
                    SMKrepr = int(runArgs.DBsmkey)

                elif hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig!="NONE":
                    SMKrepr = int(runArgs.triggerConfig.split(":")[-1].split(",")[0])

                else:
                    log.error("Menu-Aware Monitoring: Could not get SMK from DBsmkey or triggerConfig runArgs")

                if SMKrepr is not None:
                    log.info("SMK from runArgs %d" % SMKrepr)
                    # we now have the required input info. Use mam to get the appropriate MCK
                    MCKfromSMKrepr = mam.get_mck_id_from_smk(SMKrepr)

                    # if the MCK is > 0 and is from the right release then apply it, otherwise use the default tool configurations
                    if MCKfromSMKrepr > 0:
                        if mam.does_mck_athena_version_match_current_athena_version( MCKfromSMKrepr ):
                            HLTMonFlags.MCK.StoredValue = MCKfromSMKrepr
                            log.info("MCK found via SMK link %d" % HLTMonFlags.MCK.StoredValue)
                            if HLTMonFlags.doMaM_ApplyMCK:
                                log.info("Applying MCK %d" % HLTMonFlags.MCK.StoredValue)
                                mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                        else:
                            log.info("MCK for a different release found via SMK link: %d -> MCK ignored." % HLTMonFlags.MCK.StoredValue)
                    elif MCKfromSMKrepr == 0:
                        HLTMonFlags.MCK.StoredValue = MCKfromSMKrepr
                        log.info("MCK found via SMK link: 0 -> Default configuration used.")

            else:
                # try to get the MCK from COOL
                if inputFileSummary.__contains__('bs_metadata') or inputFileSummary.__contains__('run_number'):
                    # get the run number for the input
                    if inputFileSummary.__contains__('bs_metadata'):
                        run_number = inputFileSummary['bs_metadata']['run_number']
                    else:
                        run_number = int(inputFileSummary['run_number'][0])
                    pointintime = (int(run_number)<<32)

                    from PyCool import cool
                    # try to connect to the COOL database
                    from CoolConvUtilities.AtlCoolLib import indirectOpen
                    connstring = "COOLONL_TRIGGER/CONDBR2" # get the MCK from COOL
                    coolDB=indirectOpen(connstring,oracle='True')
                    if coolDB is None:
                        log.error("Menu-Aware Monitoring: Unable to connect to %s to get MCK from COOL." % connstring)
                    else:
                        # try to get the MCK out of COOL
                        foldername = 'MenuAwareMonConfigKey'
                        MCKfolder=coolDB.getFolder('/TRIGGER/HLT/' + foldername )
                        release_tag = foldername + '-' + mam.ms.current_athena_version

                        # need to retrieve for the right release using tags using a try-except
                        try:
                            retrieved_obj=MCKfolder.findObject(pointintime,0,release_tag)
                            retrieved_payload=retrieved_obj.payload()
                            retrieved_format=retrieved_payload['MonConfigKey']
                            MonitoringConfigurationKey = int(retrieved_format)
                            HLTMonFlags.MCK.StoredValue = MonitoringConfigurationKey
                            coolDB.closeDatabase()
                            if HLTMonFlags.MCK.StoredValue == 0:
                                log.info("MCK 0 found in COOL folder %s for release %s -> No MCK applied." % (foldername,mam.ms.current_athena_version))
                        except:
                            log.info("No MCK in COOL folder %s for release %s -> No MCK applied." % (foldername,mam.ms.current_athena_version))

                        if HLTMonFlags.MCK.StoredValue > 0:
                            if mam.ms.oi.check_if_mck_id_exists( HLTMonFlags.MCK.StoredValue ):
                                if mam.does_mck_athena_version_match_current_athena_version( HLTMonFlags.MCK.StoredValue ):
                                    log.info("MCK found via COOL %d" % HLTMonFlags.MCK.StoredValue)
                                    if HLTMonFlags.doMaM_ApplyMCK:
                                        log.info("Applying MCK %d" % HLTMonFlags.MCK.StoredValue)
                                        mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                                else:
                                    log.error("MCK for a different release (%d) found in COOL folder (%s) with tag (%s)." % (HLTMonFlags.MCK.StoredValue,foldername,release_tag))
                            else:
                                log.error("MCK found via COOL (%d) in COOL folder (%s) with tag (%s) is not a vaid MCK." % (HLTMonFlags.MCK.StoredValue,foldername,release_tag))
                else:
                    log.error("Menu-Aware Monitoring: Unable to get run number from metadata")

        # if dumping the tool configurations (as a .json file) has been requested, then do that here
        if HLTMonFlags.doMaM_ExtractAndDumpConfigs == True:
            log.info("Will attempt to extract final trigger monitoring tool configurations and dump them to %s" % (HLTMonFlags.MaM_OutputJSON.StoredValue))
            # get updated configs for all tools, and dump them to HLTMonFlags.MaM_OutputJSON.StoredValue
            mam.get_current_local_info()
            mam.make_default_json(HLTMonFlags.MaM_OutputJSON.StoredValue,comment="Final job config")

############################################

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

#---------------------------------------------------
#----   End
#---------------------------------------------------
