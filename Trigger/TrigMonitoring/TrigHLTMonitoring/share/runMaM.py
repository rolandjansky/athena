# Authors: Ben Smart (ben.smart@cern.ch), Xanthe Hoad (xanthe.hoad@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigHLTMonitoring/MenuAwareMonitoring' )

########## Menu-aware Monitoring ###########

if HLTMonFlags.doMaM == True:

    # check whether we are running in a trigger reprocessing job or not
    checkSMKrepr = True;
    for arg in sys.argv:
        if 'HLTMon_tf' in arg:
            HLTMonFlags.doMaM_UseReproDB.set_Value_and_Lock( True )
            checkSMKrepr = False
    if HLTMonFlags.doMaM_UseReproDB.get_Value() == False:
        if hasattr(runArgs, "DBserver") and runArgs.DBserver == "TRIGGERDBREPR":
            HLTMonFlags.doMaM_UseReproDB.set_Value_and_Lock( True )
        elif hasattr(runArgs, "triggerConfig") and "TRIGGERDBREPR" in runArgs.triggerConfig:
            HLTMonFlags.doMaM_UseReproDB.set_Value_and_Lock( True )

    # start an instance of MaM connected to the correct database accordingly
    from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring
    if HLTMonFlags.doMaM_UseReproDB.get_Value() == True:
        log.info("Will attempt to doMaM with TRIGGERDBREPR")
        mam = MenuAwareMonitoring("TRIGGERDBREPR_R")
    else:
        log.info("Will attempt to doMaM with TRIGGERDB")
        mam = MenuAwareMonitoring()

    if mam.ms.connected_to_oracle == False:
        log.error("Cannot doMaM without database connection")
    else:
        # if a specific Monitoring Configuration Key (MCK) has been set, then use it
        if HLTMonFlags.MCK.get_Value() > 0:
            if mam.ms.oi.check_if_mck_id_exists( HLTMonFlags.MCK.get_Value() ):
                if mam.does_mck_athena_version_match_current_athena_version( HLTMonFlags.MCK.get_Value() ):
                    log.info("MCK found via transform %d" % HLTMonFlags.MCK.get_Value())
                    # if we are applying configurations to tools according to an MCK, then do that here
                    if HLTMonFlags.doMaM_ApplyMCK:
                        log.info("Applying MCK %d" % HLTMonFlags.MCK.get_Value())
                        mam.apply_mck( HLTMonFlags.MCK.get_Value() )
                else:
                    log.error("MCK for a different release found via transform: %d" % HLTMonFlags.MCK.get_Value() )
            else:
                log.error("MCK found via transform (%d) is not a valid MCK" % HLTMonFlags.MCK.get_Value() )

        # if HLTMonFlags.MCK is -1 (the default) we try to determine the MCK automatically, as long as this is not MC
        isNotSIM = True
        from RecExConfig.InputFilePeeker import inputFileSummary
        if inputFileSummary.__contains__('evt_type'):
            if 'IS_SIMULATION' in inputFileSummary['evt_type']:
                log.info("Will not try to get MCK automatically as we are running on MC")
                isNotSIM = False

        if HLTMonFlags.MCK.get_Value() == -1 and isNotSIM:
            if HLTMonFlags.doMaM_UseReproDB.get_Value() == True:
                if checkSMKrepr == True:
                    # for Trig_reco_tf, check the transform arguments and get the SMK from there, then use the linked MCK
                    # don't check via SMK for TrigHLTMon_tf
                    # no COOL interaction in these jobs
                    SMKrepr = None
                    if hasattr(runArgs, "DBsmkey") and runArgs.DBsmkey!="NONE":
                        SMKrepr = int(runArgs.DBsmkey)
                    elif hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig!="NONE":
                        SMKrepr = int(runArgs.triggerConfig.split(":")[-1].split(",")[0])
                    else:
                        log.info("Could not get SMK from DBsmkey or triggerConfig runArgs")

                    if SMKrepr is not None:
                        log.info("SMK from runArgs %d" % SMKrepr)
                        # we now have the required input info. Use MaM to get the appropriate MCK
                        MCKfromSMKrepr = mam.get_mck_id_from_smk( SMKrepr )
                        # if the MCK is > 0 and is from the right release then apply it, otherwise use the default tool configurations
                        if MCKfromSMKrepr > 0:
                            if mam.does_mck_athena_version_match_current_athena_version( MCKfromSMKrepr ):
                                HLTMonFlags.MCK.set_Value_and_Lock( MCKfromSMKrepr )
                                log.info("MCK %d found via SMK link" % HLTMonFlags.MCK.get_Value())
                                if HLTMonFlags.doMaM_ApplyMCK:
                                    log.info("Applying MCK %d" % HLTMonFlags.MCK.get_Value())
                                    mam.apply_mck( HLTMonFlags.MCK.get_Value() )
                            else:
                                log.info("MCK %d found via SMK link is not valid in this release -> no MCK will be applied" % HLTMonFlags.MCK.get_Value())
                        elif MCKfromSMKrepr == 0:
                            HLTMonFlags.MCK.set_Value_and_Lock( MCKfromSMKrepr )
                            log.info("MCK 0 found via SMK link -> no MCK will be applied")
                else:
                    log.info("Not checking for MCK via SMK link (TrigHLTMon_tf mode)")

            else:
                # try to get the MCK from COOL
                if inputFileSummary.__contains__('bs_metadata') or inputFileSummary.__contains__('run_number'):

                    # get the run number
                    if inputFileSummary.__contains__('bs_metadata'):
                        run_number = inputFileSummary['bs_metadata']['run_number']
                    else:
                        run_number = int(inputFileSummary['run_number'][0])
                    pointintime = (int(run_number)<<32)

                    # try to connect to the COOL database
                    from PyCool import cool
                    from CoolConvUtilities.AtlCoolLib import indirectOpen
                    connstring = "COOLONL_TRIGGER/CONDBR2" # get the MCK from COOL
                    coolDB=indirectOpen(connstring,oracle='True')

                    if coolDB is None:
                        log.error("Unable to connect to %s to get MCK from COOL" % connstring)
                    else:
                        # try to get the MCK out of COOL
                        foldername = 'MenuAwareMonConfigKey'
                        MCKfolder=coolDB.getFolder('/TRIGGER/HLT/' + foldername)
                        release_tag = foldername + '-' + mam.ms.current_athena_version

                        # try retrieve and MCK for the correct release
                        try:
                            retrieved_obj=MCKfolder.findObject(pointintime,0,release_tag)
                            retrieved_payload=retrieved_obj.payload()
                            retrieved_format=retrieved_payload['MonConfigKey']
                            MonitoringConfigurationKey = int(retrieved_format)
                            HLTMonFlags.MCK.set_Value_and_Lock( MonitoringConfigurationKey )
                            coolDB.closeDatabase()
                            if HLTMonFlags.MCK.get_Value() == 0:
                                log.info("MCK 0 found in COOL folder %s for release %s -> no MCK will be applied" % (foldername,mam.ms.current_athena_version))
                        except:
                            log.info("No MCK in COOL folder %s for release %s -> no MCK will be applied" % (foldername,mam.ms.current_athena_version))

                        if HLTMonFlags.MCK.get_Value() > 0:
                            if mam.ms.oi.check_if_mck_id_exists(HLTMonFlags.MCK.get_Value()):
                                if mam.does_mck_athena_version_match_current_athena_version(HLTMonFlags.MCK.get_Value()):
                                    log.info("MCK %d found in COOL folder %s for release %s" % (HLTMonFlags.MCK.get_Value(),foldername,release_tag))
                                    if HLTMonFlags.doMaM_ApplyMCK:
                                        log.info("Applying MCK %d" % HLTMonFlags.MCK.get_Value())
                                        mam.apply_mck(HLTMonFlags.MCK.get_Value())
                                else:
                                    log.error("MCK %d found in COOL folder %s with tag %s is not a valid in this release" % (HLTMonFlags.MCK.get_Value(),foldername,release_tag))
                            else:
                                log.error("MCK %d found in COOL folder %s with tag %s is not a valid MCK" % (HLTMonFlags.MCK.get_Value(),foldername,release_tag))
                else:
                    log.error("Unable to get run number from metadata")

        # if dumping the tool configurations (as a .json file) has been requested, then do that here
        if HLTMonFlags.doMaM_ExtractAndDumpConfigs == True:
            log.info("Will attempt to extract final trigger monitoring tool configurations and dump them to %s" % (HLTMonFlags.MaM_OutputJSON.get_Value()))
            # get updated configs for all tools, and dump them to HLTMonFlags.MaM_OutputJSON.get_Value()
            mam.get_current_local_info(print_output_here=False)
            mam.make_default_json(HLTMonFlags.MaM_OutputJSON.get_Value(),comment="Final job config")
