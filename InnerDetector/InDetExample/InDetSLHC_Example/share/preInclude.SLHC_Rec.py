#
# --- Only do for RAW-ESD stage
#
from RecExConfig.RecFlags import rec
if rec.doWriteESD() or rec.doWriteAOD() or ('doWriteESD' in dir() and doWriteESD) or ('doWriteAOD' in dir() and doWriteAOD) or InDetFlags.doTrkNtuple() or InDetFlags.doTrkD3PD():

    # --- no conditions for SLHC
    if len(globalflags.ConditionsTag())!=0:
       from IOVDbSvc.CondDB import conddb
       conddb.setGlobalTag(globalflags.ConditionsTag())

    # --- turn off commissioning flags
    rec.Commissioning = False

    # --- turn off trigger
    #rec.doTrigger = False
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.doLVL2.set_Value_and_Lock(False)
    TriggerFlags.doEF.set_Value_and_Lock(False)

    # --- this is the SLHC 
    InDetFlags.doSLHC = True

    # switch of tracking and vertex finding for AOD and ESD making
    printfunc ('RecoStep:',rec.OutputFileNameForRecoStep())
    if ( rec.OutputFileNameForRecoStep() == 'ESDtoAOD' or
         rec.OutputFileNameForRecoStep() == 'ESDtoDPD' ):
        printfunc ('switching off tracking and vertex finding for AOD and ESD making')
        InDetFlags.doNewTracking  =         False
        InDetFlags.doVertexFinding =        False

    else:
        # --- no pixel cluster splitting
        if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
            InDetFlags.doPixelClusterSplitting = False

        # --- no segment finding
        InDetFlags.doTrackSegmentsPixel = False
        InDetFlags.doTrackSegmentsSCT = False
        InDetFlags.doTrackSegmentsTRT = False

        # --- run NewTracking
        InDetFlags.doNewTracking = True
        # --- turn off rest 
        InDetFlags.doLowPt = False
        InDetFlags.doBackTracking = False
        InDetFlags.doTRTStandalone = False
        InDetFlags.loadTRTSeededSPFinder = False
        #InDetFlags.doSingleSpBackTracking.set_Value_and_Lock(False)
        # --- no special tracking
        InDetFlags.doBeamGas = False
        InDetFlags.doBeamHalo = False
        InDetFlags.doCosmics = False
        # --- Turn on Brem-Recovery 
        InDetFlags.doBremRecovery = True; 
        InDetFlags.doCaloSeededBrem = False; 
        # --- Turn on Forward tracking 
        InDetFlags.doForwardTracks = True; 

        # --- not sure this is needed
        InDetFlags.doSharedHits = True
        # --- Turn off track slimming
        InDetFlags.doSlimming = False
        # --- do not use DCS
        InDetFlags.useDCS = False
        # --- no SG deletion
        InDetFlags.doSGDeletion = False

        # --- de-activate monitorings
        InDetFlags.doMonitoringGlobal = False
        InDetFlags.doMonitoringPixel = False
        InDetFlags.doMonitoringSCT = False
        InDetFlags.doMonitoringTRT = False
        InDetFlags.doMonitoringAlignment = False

        # --- activate (memory/cpu) monitoring
        # InDetFlags.doPerfMon.set_Value_and_Lock(True)

        # --- activate creation of standard plots
        InDetFlags.doPhysValMon = False
        # --- do tracking ntuple
        #InDetFlags.doTrkNtuple.set_Value_and_Lock(False)
        # --- set properties of ntuple
        InDetFlags.doPixelTrkNtuple = False
        InDetFlags.doSctTrkNtuple = False
        InDetFlags.doTrtTrkNtuple = False
        if InDetFlags.doTrkNtuple():
            InDetFlags.doSctClusterNtuple = True
        else:
            InDetFlags.doSctClusterNtuple = False

        # --- do tracking D3PD
        #InDetFlags.doTrkD3PD.set_Value_and_Lock(False)

        # --- activate the print InDetXYZAlgorithm statements
        InDetFlags.doPrintConfigurables.set_Value_and_Lock(True)

        # --- activate RDO output
        InDetFlags.writeRDOs.set_Value_and_Lock(True)
    

# --- setup of tracking geometry for IBL beampipe
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
#TrkDetFlags.BeamPipeRadius.set_Value_and_Lock(27.)
#TrkDetFlags.BeamPipeThickness.set_Value_and_Lock(1.)
#############################################################
TrkDetFlags.SLHC_Geometry                   = True
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
TrkDetFlags.MaterialFromCool                = True
TrkDetFlags.MaterialDatabaseLocal           = False and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
if SLHC_Flags.SLHC_Version() is '' :
    TrkDetFlags.MaterialMagicTag         = jobproperties.Global.DetDescrVersion()
else :
    TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version()
if TrkDetFlags.MaterialDatabaseLocal() is True :
    TrkDetFlags.MaterialDatabaseLocalPath    = ''
    TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'

# TrkDetFlags.MagneticFieldCallbackEnforced         = False
# TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
# TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False
#############################################################

# from RecExConfig.RecoFunctions import RunInnerDetectorOnly
# RunInnerDetectorOnly()

