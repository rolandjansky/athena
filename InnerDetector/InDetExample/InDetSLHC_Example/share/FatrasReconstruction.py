################################################
# Fatras Reconstruction setup
# S. Fleischmann, 15/07/2007
################################################
if 'topSequence' not in dir() :
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence() 

class FatrasReconstruction :
  # constructor of Fatras Reco
  def __init__ (self, fatrasJob = None, noiseLevels = []) :
                      
    ############################################################################
    # PRINT MESSAGE IF NOTHING IS PROVIDED
    if fatrasJob is None :
        printfunc ('CONFIGURATION PROBLEM in FatrasReconstruction setup: ')
        printfunc ('    - providing a FatrasJob instance is mandatory')
         
    fatrasTools                = fatrasJob.tools()
    fatrasPrimary              = fatrasJob.primarySimulation()
    fatrasExtrapolation        = fatrasJob.extrapolation()

    # import the jobProperties
    from FatrasExample.FatrasJobProperties import FatrasFlags
    from FatrasExample.FatrasValidation import FatrasValidationFlags

    # prepare the StoreGate (configurable for Event overlay)
    from FastSimulationConfig.FastSimulationFlags import jobproperties 
    EventStoreName = jobproperties.FastSimulation.EventStoreName()

    from StoreGate.StoreGateConf import StoreGateSvc
    FatrasEventStore = StoreGateSvc(EventStoreName)

    # register
    fatrasJob.registerReconstruction(self)
    #--------------------------------------------------------------
    # load ServiceMgr, ToolSvc, topSequence
    #--------------------------------------------------------------
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr
    if 'ToolSvc' not in dir() :
       from AthenaCommon.AppMgr import ToolSvc
    if 'topSequence' not in dir() :
       from AthenaCommon.AlgSequence import AlgSequence
       topSequence = AlgSequence()
    
    #--------------------------------------------------------------
    # InDetFlags configuration
    #--------------------------------------------------------------
    if FatrasFlags.SimulationID():
      # --- setup InDetFlags
      # --- setup InDetFlags according to InDetFlagsJobProperty
      from InDetRecExample.InDetJobProperties import InDetFlags
      # the different pattern components
      InDetFlags.doNewTracking   = True
      InDetFlags.doBackTracking  = True
      InDetFlags.doTruth         = True
      InDetFlags.preProcessing   = True
      InDetFlags.doPRDFormation  = False
      InDetFlags.doFatras        = True
      # switch the detector control system off
      InDetFlags.useDCS          = False

      if len(noiseLevels) > 0 :
        ToolSvc.FatrasPixelPostProcessor.NoiseFraction  = noiseLevels[0]
      if len(noiseLevels) > 1 :      
        ToolSvc.FatrasSCT_PostProcessor.NoiseFraction   = noiseLevels[1]
      if len(noiseLevels) > 2 :        
        ToolSvc.FatrasTRT_PostProcessor.NoiseFraction   = noiseLevels[2]
      #--------------------------------------------------------------
      # Load Inner Detector reconstruction
      #--------------------------------------------------------------
      if FatrasFlags.ReconstructionID():
        include( "InDetRecExample/InDetRec_jobOptions.py" )
      elif FatrasFlags.RefittingID():
        include( "InDetSLHC_Example/InDetRec_jobOptions_refit.py" )
      else :
        include( "InDetRecExample/InDetRecConditionsAccess.py")

    #--------------------------------------------------------------
    # Muon Flags configuration
    #--------------------------------------------------------------
    if FatrasFlags.SimulationMS() :
    
       from MuonRecExample.MuonRecFlags import muonRecFlags
     
       # let everything be configured by the Muon Reconstruction
       if FatrasFlags.ReconstructionMS(): 
         muonRecFlags.doDigitization = FatrasFlags.DigitizationMS()
         muonRecFlags.doMuonboy      = FatrasFlags.ReconstructionMS()
         muonRecFlags.doMoore        = FatrasFlags.ReconstructionMS()
         ServiceMgr.PoolSvc.AttemptCatalogPatch=True

         # this is to set the Amdc Source format
         include( "AmdcAth/AmdcAth_jobOptions.py" )

         # necessary to avoid a clinch between MetaDataSvc and McEventSelector
         if FatrasFlags.SingleTrackSimulation() :
            include.block('AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py')
            
            # overwrite Muon containers from bytestream
            from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderTool
            MDT_RawDataProviderTool              = Muon__MDT_RawDataProviderTool(name = 'MDT_RawDataProviderTool')
            MDT_RawDataProviderTool.RdoLocation = 'MDT_dump'
            ToolSvc += MDT_RawDataProviderTool

            from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderTool
            RPC_RawDataProviderTool              = Muon__RPC_RawDataProviderTool(name = 'RPC_RawDataProviderTool')
            RPC_RawDataProviderTool.RdoLocation = 'RPC_dump'
            ToolSvc += RPC_RawDataProviderTool
            
            from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderTool
            TGC_RawDataProviderTool              = Muon__TGC_RawDataProviderTool(name = 'TGC_RawDataProviderTool')
            TGC_RawDataProviderTool.RdoLocation = 'TGC_dump'
            ToolSvc += TGC_RawDataProviderTool

            from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
            CSC_RawDataProviderTool              = Muon__CSC_RawDataProviderTool(name = 'CSC_RawDataProviderTool')
            CSC_RawDataProviderTool.RdoLocation = 'CSC_dump'
            ToolSvc += CSC_RawDataProviderTool
                                                                                 
         if FatrasValidationFlags.MuonMonitoringNtuple():         
           #--------------------------------------------------------------
           #build MuonCalibrationNtuple
           #--------------------------------------------------------------
           from MuonRecExample.MuonRecFlags import muonRecFlags
           from MuonCalibAlgs.MuonCalibFlags import muonCalibFlags
           muonRecFlags.doCalib    = True
           muonCalibFlags.Mode     = 'trackNtuple'
           muonCalibFlags.EventTag = 'Moore'
           #--------------------------------------------------------------     
                          
         import MuonCnvExample.MuonCablingConfig                          
         # include the MuonRecExample jobOptions
         include( "MuonRecExample/MuonRec_jobOptions.py" )
         include.block("MuonRecExample/MuonRec_jobOptions.py")
         # when fast calo sim steered from Fatras 
         if FatrasFlags.FastCaloSim() :
            from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
            include( "MuonCombinedRecExample/MuonCombinedRec_config.py")
         
       if FatrasValidationFlags.MuonSimHitNtuple():
         #
         from FatrasMuonAlgs.FatrasMuonAlgsConf import Fatras__MuonHitNtuple
         MuonHitNtuple = Fatras__MuonHitNtuple(  name = 'MuonHitNtuple' )
         MuonHitNtuple.EventStore           = FatrasEventStore
         MuonHitNtuple.ProcessFatrasTracks  = True
         MuonHitNtuple.McEventCollection    = FatrasKeyFlags.McEventCollection()
         MuonHitNtuple.Extrapolator         = "Trk::Extrapolator/FatrasRecoExtrapolator" 
         MuonHitNtuple.TreeFolderLocation   = '/'+FatrasValidationFlags.ValidationStream()+'/hits'
         topSequence += MuonHitNtuple
         printfunc (MuonHitNtuple)
      
       
        
