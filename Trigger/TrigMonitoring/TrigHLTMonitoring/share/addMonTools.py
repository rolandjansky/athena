#--------------------------------------------------------------
#-- Job Options for HLTMonitoring
#--------------------------------------------------------------

#############################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

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
         print "Problems with the general HLTMonTool"

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
        print "Problems with HLTEgammaTool, tool not enabled"

    # HLTCaloTool - Makes basic comparisons between online/offline
    if HLTMonFlags.doCalo:
      try:
       if rec.doCalo and rec.doInDet:
        from TrigCaloMonitoring.TrigCaloMonitoringConfig import HLTCaloMonitoringTool
        HLTMonManager.AthenaMonTools+=HLTCaloMonitoringTool()
      except:
        print "Problems with HLTCaloTool, tool not enabled"
    
    # HLTMuonMonTool - Multiple Muon algorithms EDM Dumper
    if HLTMonFlags.doMuon:
      try:
        from TrigMuonMonitoring.TrigMuonMonitoringConfig import TrigMuonMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMuonMonitoringTool()
      except:
        print "Problems with HLTMuonTool, tool not enabled"

    # IDtrk HLTMonTool
    if HLTMonFlags.doIDtrk:
      try:
        from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()
      except:
        print "Problems with HLTIDtrkTool, tool not enabled"

    # MET HLTMonTool
    if HLTMonFlags.doMET:
      try:
        from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
        HLTMonManager.AthenaMonTools += HLTMETMonitoringTool()
      except:
        print "Problems with HLTMETTool, tool not enabled"

    # Tau HLTMonTool
    if HLTMonFlags.doTau:
      try:
        from TrigTauMonitoring.TrigTauMonitoringConfig import TrigTauMonitoringTool
        HLTMonManager.AthenaMonTools += TrigTauMonitoringTool()
      except:
        print "Problems with HLTTauTool, tool not enabled"
        
    # Jet HLTMonTool
    if HLTMonFlags.doJet:
      try:
        from TrigJetMonitoring.TrigJetMonitoringConfig import TrigJetMonitoringTool
        HLTMonManager.AthenaMonTools += TrigJetMonitoringTool()
      except:
        print "Problems with HLTJetTool, tool not enabled"
    
    # b-jet HLTMonTool
    if HLTMonFlags.doBjet and rec.doInDet:
      try:
        from TrigBjetMonitoring.TrigBjetMonitoringConfig import TrigBjetMonitoringConfig
        HLTMonManager.AthenaMonTools += TrigBjetMonitoringConfig()
      except:
        print "Problems with HLTBjetTool, tool not enabled"
    
    # B-phys HLTMonTool
    if HLTMonFlags.doBphys:
      try:
        from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
        HLTMonManager.AthenaMonTools += TrigBphysMonitoringTool()
      except:
        print "Problems with HLTBphysTool, tool not enabled"

    # MinBias HLTMonTool
    if HLTMonFlags.doMinBias:
      try:
        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMinBiasMonitoringTool()
      except:
        print "Problems with HLTMinBiasDumpTool, tool not enabled"

    # MinBias HLTMonTool
    if HLTMonFlags.doIDJpsiMon:
      try:
        from TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig import TrigIDJpsiMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDJpsiMonitoringTool()
      except:
        print "Problems with, TrigIDJpsiMonTool, tool not enabled"


################ Dump Tools ################

if HLTMonFlags.doDump:

    # HLTEgammaDumpTool - HLT EDM Egamma Dumper
    if HLTMonFlags.doEgamma:
      try:
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import HLTEgammaMonitoringDumpTool
        HLTMonManager.AthenaMonTools += HLTEgammaMonitoringDumpTool()
      except:
        print "Problems with HLTEgammaDumpTool, tool not enabled"


    #Make the custom tau TTP ntuples.
    if HLTMonFlags.doOfflineTauTTP :      
     try:
      from TrigTauPerformAthena.TrigTauPerformAthenaConf import TrigTauPerformAthenaAlgo
      ttpalgo = TrigTauPerformAthenaAlgo()
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      topSequence += ttpalgo          
     except:
      print "Problems with OfflineTauTTP, tool not enabled"

############################################

########## Menu-aware Monitoring ###########

HLTMonFlags.doMaM = False
if HLTMonFlags.doMaM:    

    # MaM needs to check whether it is running in a Trigger reprocessing job or not, and start an instance of MaM connected to the correct database accordingly
    trigger_reco_tf_job = False
    
    if hasattr(runArgs, "DBserver") and runArgs.DBserver == "TRIGGERDBREPR":
        trigger_reco_tf_job = True            
        
    elif hasattr(runArgs, "triggerConfig") and "TRIGGERDBREPR" in runArgs.triggerConfig:
        trigger_reco_tf_job = True            
    
    from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring
    if trigger_reco_tf_job:
        mam = MenuAwareMonitoring("TRIGGERDBREPR")
    else:
        mam = MenuAwareMonitoring()
    
    if mam.connected_to_oracle == False:
        # how to raise this as an error?
        print "Menu-aware Monitoring error: Cannot doMaM without database connection. Exiting MaM."
    else:

        # if we are applying configurations to tools according to an MCK, then do that here
        if  HLTMonFlags.doMaM_ApplyMCK:

            # if a specific Monitoring Configuration Key (MCK) has been set, then use it
            if HLTMonFlags.MCK.StoredValue > 0:
                if mam.does_mck_athena_version_match_current_athena_version( HLTMonFlags.MCK.StoredValue ):
                    print "Using trigger Monitoring Configuration Key (MCK)",HLTMonFlags.MCK.StoredValue
                    mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                else: 
                    print "MCK",HLTMonFlags.MCK.StoredValue,"cannot be applied as it is a patch for a different Athena version. No MCK applied."
                    # should MAM check Cool in this case? no

            # if HLTMonFlags.MCK is -1 (the default) we get the MCK from Cool, if there is no MCK there then we pick the MCK based on the SMK
            if HLTMonFlags.MCK.StoredValue == -1:
                                
                if trigger_reco_tf_job:
                    # for trigger repro jobs, need to check the transform arguments and get the SMK from there, then use the linked MCK.
                    # no Cool interaction in these jobs
                    SMKrepr = None
                    
                    if hasattr(runArgs, "DBsmkey") and runArgs.DBsmkey!="NONE":
                        SMKrepr = int(runArgs.DBsmkey)                        

                    elif hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig!="NONE":
                        SMKrepr = int(runArgs.triggerConfig.split(":")[-1].split(",")[0])
                
                    else: 
                        print "Could not get SMK from DBsmkey or triggerConfig runArgs. No MCK applied."
                            
                    if SMKrepr is not None:
                        print "SMK SuperMasterKey from runArgs =",SMKrepr
                        # We now have the required input info. Use mam to get the appropriate MCK
                        MCKfromSMKrepr = mam.get_mck_id_from_smk(SMKrepr)
                        
                        # If the MCK is > 0 and is from the right release then apply it, otherwise use the default tool configurations
                        if MCKfromSMKrepr > 0:
                            if mam.does_mck_athena_version_match_current_athena_version( MCKfromSMKrepr ):
                                HLTMonFlags.MCK.StoredValue = MCKfromSMKrepr
                                print "Using trigger Monitoring Configuration Key (MCK) from SMK link =",HLTMonFlags.MCK.StoredValue
                                mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                            else:
                                print "MCK",MCKfromSMKrepr,"cannot be applied as it is a patch for a different Athena version. No MCK applied."
                        elif MCKfromSMKrepr == 0:
                            HLTMonFlags.MCK.StoredValue = MCKfromSMKrepr
                            print "Monitoring Configuration Key (MCK) from SMK link = 0, default configuration used"
                
                else:                
                    # try to get the MCK from Cool
                    from RecExConfig.InputFilePeeker import inputFileSummary
                    #print "inputFileSummary =",inputFileSummary
                    if inputFileSummary.__contains__('bs_metadata'):
                        # get the run number and lumi_block for the input
                        run_number = inputFileSummary['bs_metadata']['run_number']
                        #lumi_block = inputFileSummary['bs_metadata']['LumiBlock']
                        from PyCool import cool

                        pointintime = (int(run_number)<<32) + int(lumi_block) # start from lumiblock 0 (or 1?)
                        
                        # get the database instance for getting the SMK
                        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
                        #print "svcMgr.IOVDbSvc.properties() =",svcMgr.IOVDbSvc.properties()
                        DBInstance = svcMgr.IOVDbSvc.properties()['DBInstance']
                                        
                        # try to connect to the COOL database
                        from CoolConvUtilities.AtlCoolLib import indirectOpen
                        connstring = "COOLONL_TRIGGER/"+str(DBInstance) 
                        coolDB=indirectOpen(connstring,oracle='True')
                        if coolDB is None:
                            print "Unable to connect to",connstring,"to get MCK from Cool. Will also not be able to get the SMK so no MCK will be applied."
                        else:
                            # try to get the MCK out of COOL
                            foldername = 'MenuAwareMonConfigKey'
                            MCKfolder=coolDB.getFolder('/TRIGGER/HLT/' + foldername ) #what if the folder doesn't exist?
                            release_tag = foldername + '-' + mam.current_athena_version
                        
                            # need to retrieve for the right release using tags using a try-except  
                            try:
                                retrieved_obj=MCKfolder.findObject(pointintime,0,release_tag)
                                retrieved_payload=retrieved_obj.payload()
                                retrieved_format=retrieved_payload['MonConfigKey']
                                MonitoringConfigurationKey = int(retrieved_format)
                                HLTMonFlags.MCK.StoredValue = MonitoringConfigurationKey

                                # do not check athena version - we know the release is right because of the tag
                                if HLTMonFlags.MCK.StoredValue > 0:
                                    print "Using trigger Monitoring Configuration Key (MCK) from Cool =",HLTMonFlags.MCK.StoredValue
                                    mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                                elif HLTMonFlags.MCK.StoredValue == 0:
                                    print "Monitoring Configuration Key (MCK) from Cool = 0, default configuration used"
                            
                            except ( ObjectNotFound, TagNotFound ):

                                print "No MCK in Cool folder",foldername,"for release",mam.current_athena_version,", checking via SMK"
                                                    
                                # if MCK version doesn't match, check SMK  
                                # If MCK version didn't match, add the new MCK to Cool with a new tag
                                SMKfolder=coolDB.getFolder('/TRIGGER/HLT/HltConfigKeys')
                                retrieved_obj=SMKfolder.findObject(pointintime,0)
                                retrieved_payload=retrieved_obj.payload()
                                retrieved_format=retrieved_payload['MasterConfigurationKey'] 
                                SuperMasterKey = int(retrieved_format)
                                
                                print "SMK SuperMasterKey from Cool =",SuperMasterKey
                                
                                # We now have the required input info. Use mam to get the appropriate MCK
                                MCKfromSMK = mam.get_mck_id_from_smk(SuperMasterKey)
                                
                                # If the MCK is > 0 and is from the right release then apply it, otherwise use the default tool configurations
                                if MCKfromSMK > 0:
                                    if mam.does_mck_athena_version_match_current_athena_version( MCKfromSMK ):
                                        HLTMonFlags.MCK.StoredValue = MCKfromSMK
                                        print "Using trigger Monitoring Configuration Key (MCK) from SMK link =",HLTMonFlags.MCK.StoredValue
                                        mam.apply_mck( HLTMonFlags.MCK.StoredValue )
                                        # If MCK version didn't match, should we update the MCK in Cool? no
                                    else:
                                        print "MCK",MCKfromSMK,"cannot be applied as it is a patch for a different Athena version. No MCK applied."
                                elif MCKfromSMK == 0:
                                    HLTMonFlags.MCK.StoredValue = MCKfromSMK
                                    print "Monitoring Configuration Key (MCK) from SMK link = 0, default configuration used"
                                        
                                if MCKfromSMK is 0 or mam.does_mck_athena_version_match_current_athena_version( MCKfromSMK ):        
                                
                                        # add the MCK to Cool if we used it 
                                        # Store MCK=0 if what is linked to the SMK is explicitly 0 

                                        payloadSpec = cool.RecordSpecification()
                                        payloadSpec.extend('MonConfigKey',cool.StorageType.UInt32)
                                        dataforCool = cool.Record(payloadSpec)
                                        dataforCool['MonConfigKey'] = HLTMonFlags.MCK.StoredValue

                                        iovsince = (int(run_number)<<32)
                                        iovuntil = (int(run_number+1)<<32)-1  
                                        print "Storing MCK",HLTMonFlags.MCK.StoredValue,"in Cool for IOVs",runiov,"to",iovuntil 
                                        MCKfolder.storeObject(runiov,iovuntil,dataforCool,0,release_tag)
                            
        # if dumping the tool configurations (as a .json file) has been requested, then do that here
        if  HLTMonFlags.doMaM_ExtractAndDumpConfigs:

            # get updated configs for all tools, and dump them to HLTMonFlags.MaM_OutputJSON.StoredValue
            mam.get_current_local_info()
            mam.dump_local_config_to_json(HLTMonFlags.MaM_OutputJSON.StoredValue)

############################################

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

#---------------------------------------------------
#----   End
#---------------------------------------------------
