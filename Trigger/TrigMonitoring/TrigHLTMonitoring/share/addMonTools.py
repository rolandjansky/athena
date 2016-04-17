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

if HLTMonFlags.doMaM:

    # set up menu-aware monitoring
    from TrigHLTMonitoring.MenuAwareMonitoring import MenuAwareMonitoring
    mam = MenuAwareMonitoring()
    
    if mam.connected_to_oracle == False:
        # how to raise this as an error?
        print "Menu-aware Monitoring error: Cannot doMaM without database connection. Exiting MaM."
    else:

        # if we are applying configurations to tools according to an MCK, then do that here
        if  HLTMonFlags.doMaM_ApplyMCK:

            # if a specific Monitoring Configuration Key (MCK) has been set, then use it
            if HLTMonFlags.MCK.StoredValue > 0:

                print "Using trigger Monitoring Configuration Key (MCK)",HLTMonFlags.MCK.StoredValue
                mam.apply_mck( HLTMonFlags.MCK.StoredValue )

            # if HLTMonFlags.MCK is -1 (the default) then we pick the MCK based on the SMK
            if HLTMonFlags.MCK.StoredValue == -1:

                # MaM will translate the SMK into an MCK, and apply it
                from RecExConfig.InputFilePeeker import inputFileSummary
                #print "inputFileSummary =",inputFileSummary
                if inputFileSummary.__contains__('bs_metadata'):
                    # get the run number and lumi_block for the input
                    run_number = inputFileSummary['bs_metadata']['run_number']
                    lumi_block = inputFileSummary['bs_metadata']['LumiBlock']
                    runiov = (int(run_number)<<32) + int(lumi_block)

                    # get the database instance for getting the SMK
                    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
                    #print "svcMgr.IOVDbSvc.properties() =",svcMgr.IOVDbSvc.properties()
                    DBInstance = svcMgr.IOVDbSvc.properties()['DBInstance']

                    # try to connect to the COOL database
                    from PyCool import cool
                    from CoolConvUtilities.AtlCoolLib import indirectOpen
                    connstring = "COOLONL_TRIGGER/"+str(DBInstance)
                    trigDB=indirectOpen(connstring,oracle='True')
                    if trigDB is None:
                        print "Unable to connect to",connstring
                    else:
                        # get the SMK out of COOL
                        folder=trigDB.getFolder('/TRIGGER/HLT/HltConfigKeys')
                        retrieved_obj=folder.findObject(runiov,0)
                        retrieved_payload=retrieved_obj.payload()
                        retrieved_format=retrieved_payload['MasterConfigurationKey']
                        SuperMasterKey = int(retrieved_format)

                        print "SMK SuperMasterKey =",SuperMasterKey

                        # We now have the required input info. Use mam to get the appropriate MCK
                        HLTMonFlags.MCK.StoredValue = mam.get_mck_id_from_smk(SuperMasterKey)
                        print "Have used Menu-aware monitoring to extract MCK",HLTMonFlags.MCK.StoredValue,"linked to SMK",SuperMasterKey

                        # If the MCK is > 0 then apply it, otherwise use the default tool configurations
                        if HLTMonFlags.MCK.StoredValue > 0:
                            print "Using trigger Monitoring Configuration Key (MCK)",HLTMonFlags.MCK.StoredValue
                            mam.apply_mck( HLTMonFlags.MCK.StoredValue )

        # if dumping the tool configurations (as a .json file) has been requested, then do that here
        if  HLTMonFlags.doMaM_ExtractAndDumpConfigs:

            # mam.setup_all_local_tools()         
            # get updated configs for all tools, and dump them to HLTMonFlags.MaM_OutputJSON.StoredValue
            mam.get_current_local_info()
            mam.dump_local_config_to_json(HLTMonFlags.MaM_OutputJSON.StoredValue)

############################################

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

#---------------------------------------------------
#----   End
#---------------------------------------------------

#try:
#    print "ToolSvc.HLTMETMon.muon_pt_thresh",ToolSvc.HLTMETMon.muon_pt_thresh
#except:
#    print "HLTMETMon is not set up, cannot print value"
