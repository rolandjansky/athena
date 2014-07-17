#
# --- Only do for RAW-ESD stage
#
from RecExConfig.RecFlags import rec
if rec.doWriteESD() or rec.doWriteAOD() or ('doWriteESD' in dir() and doWriteESD) or ('doWriteAOD' in dir() and doWriteAOD) or InDetFlags.doTrkNtuple() or InDetFlags.doTrkD3PD():

    # --- turn of commissioning flags
    rec.Commissioning.set_Value_and_Lock(False)
    

    # --- no pixel cluster splitting
    if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
        InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)

    # --- no segment finding
    InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False)
    InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(False)

    # --- run NewTracking
    InDetFlags.doNewTracking.set_Value_and_Lock(True)
    # --- turn off rest 
    InDetFlags.doLowPt.set_Value_and_Lock(False)
    InDetFlags.doBackTracking.set_Value_and_Lock(False)
    InDetFlags.doTRTStandalone.set_Value_and_Lock(False)
    #InDetFlags.doSingleSpBackTracking.set_Value_and_Lock(False)
    # --- turn off alternatives
    InDetFlags.doxKalman.set_Value_and_Lock(False)
    InDetFlags.doiPatRec.set_Value_and_Lock(False)
    # --- no special tracking
    InDetFlags.doBeamGas.set_Value_and_Lock(False)
    InDetFlags.doBeamHalo.set_Value_and_Lock(False)
    InDetFlags.doCosmics.set_Value_and_Lock(False)
    # --- not sure this is needed
    InDetFlags.doSharedHits.set_Value_and_Lock(True)
    # --- Turn off track slimming
    InDetFlags.doSlimming.set_Value_and_Lock(False)
    # --- this is not SLHC 
    InDetFlags.doSLHC.set_Value_and_Lock(False)
    # --- do not use DCS
    InDetFlags.useDCS.set_Value_and_Lock(False)
    # --- no SG deletion
    InDetFlags.doSGDeletion.set_Value_and_Lock(False)
    
    # --- de-activate monitorings
    InDetFlags.doMonitoringGlobal.set_Value_and_Lock(False)
    InDetFlags.doMonitoringPixel.set_Value_and_Lock(False)
    InDetFlags.doMonitoringSCT.set_Value_and_Lock(False)
    InDetFlags.doMonitoringTRT.set_Value_and_Lock(False)
    InDetFlags.doMonitoringAlignment.set_Value_and_Lock(False)
    
    # --- activate (memory/cpu) monitoring
    # InDetFlags.doPerfMon.set_Value_and_Lock(True)

    # --- activate creation of standard plots
    InDetFlags.doStandardPlots.set_Value_and_Lock(False)
    # --- do tracking ntuple
    #InDetFlags.doTrkNtuple.set_Value_and_Lock(False)
    # --- set properties of ntuple
    InDetFlags.doPixelTrkNtuple.unlock()
    InDetFlags.doSctTrkNtuple.unlock()
    InDetFlags.doTrtTrkNtuple.unlock()
    InDetFlags.doPixelClusterNtuple.unlock()
    InDetFlags.doSctClusterNtuple.unlock()
    InDetFlags.doPixelTrkNtuple.set_Value_and_Lock( False )
    InDetFlags.doSctTrkNtuple.set_Value_and_Lock( False )
    InDetFlags.doTrtTrkNtuple.set_Value_and_Lock( False )
    if InDetFlags.doTrkNtuple():
        InDetFlags.doPixelClusterNtuple.set_Value_and_Lock( True )
        InDetFlags.doSctClusterNtuple.set_Value_and_Lock( True )
    else:
        InDetFlags.doPixelClusterNtuple.set_Value_and_Lock( False )
        InDetFlags.doSctClusterNtuple.set_Value_and_Lock( False )

    # --- do tracking D3PD
    #InDetFlags.doTrkD3PD.set_Value_and_Lock(False)
    
    # --- activate the print InDetXYZAlgorithm statements
    InDetFlags.doPrintConfigurables.set_Value_and_Lock(True)

    # --- activate RDO output
    InDetFlags.writeRDOs.set_Value_and_Lock(True)
    

# --- setup of tracking geometry for IBL beampipe
from AthenaCommon.BeamFlags import jobproperties
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.BeamPipeRadius.set_Value_and_Lock(27.)
TrkDetFlags.BeamPipeThickness.set_Value_and_Lock(1.)
#############################################################
# TrkDetFlags.MagneticFieldCallbackEnforced   = False
# TrkDetFlags.TRT_BuildStrawLayers            = False
try:
    TrkDetFlags.MaterialSource                  = 'COOL'
    TrkDetFlags.MaterialDatabaseLocal           = False and TrkDetFlags.MaterialSource() == 'COOL'
except:
    TrkDetFlags.MaterialFromCool                = True
    TrkDetFlags.MaterialDatabaseLocal           = False and TrkDetFlags.MaterialFromCool()

if TrkDetFlags.MaterialDatabaseLocal():
  TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version()
  TrkDetFlags.MaterialVersion                 = 15
  TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/LayerMaterial'
  TrkDetFlags.MaterialTagBase                 = 'AtlasLayerMat_v'
  TrkDetFlags.MaterialDatabaseLocalPath       = './'
  TrkDetFlags.MaterialDatabaseLocalName       = 'AtlasLayerMaterial-ATLAS-IBL-03-00-00.db'
else:
  TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()

# TrkDetFlags.MagneticFieldCallbackEnforced         = False
# TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
# TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False
#############################################################

# from RecExConfig.RecoFunctions import RunInnerDetectorOnly
# RunInnerDetectorOnly()

