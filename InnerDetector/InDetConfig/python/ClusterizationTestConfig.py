import TrackReco 	   as TrackReco
import PixelGeoModelConfig      as PixelGeoModelConfig
import SCT_GeoModelConfig	as SCT_GeoModelConfig

arg_TrackingCuts             = 'TrackingCuts'
arg_TrackCollectionKeys      = 'tracks'
arg_TrackCollectionTruthKeys = 'truth'

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

#@extraArgs( redoPatternRecoAndTracking = False )
def InDetRecPreProcessingSiliconAlgorithmsCfg(flags, **kwargs) :

    #redoPatternRecoAndTracking = kwargs.pop('redoPatternRecoAndTracking')
    # @TODO propagate suffix and/or prefix ?
    result = ComponentAccumulator()
    result.merge(TrackReco.InDetBCM_ZeroSuppressionCfg(flags))
   
    # Pixel clusterization
    # @TODO why flags.DetFlags.makeRIO.SCT_on ?
    if flags.DetFlags.makeRIO.pixel_on or flags.DetFlags.makeRIO.SCT_on:
        result.merge(TrackReco.InDetPixelClusterizationCfg(flags, **kwargs) )
        if flags.InDetFlags.doSplitReco :
            result.merge(TrackReco.InDetPixelClusterizationPUCfg(flags, **kwargs) )

    # SCT clusterization
    if flags.DetFlags.makeRIO.SCT_on and flags.InDetFlags.doSCT_PRDFormation:
        result.merge( TrackReco.InDet_SCTClusterizationCfg(flags, **kwargs) )
        if flags.InDetFlags.doSplitReco :
            result.merge( TrackReco.InDet_SCTClusterizationPUCfg(flags, **kwargs) )
    

    return result



from IOVDbSvc.IOVDbSvcConfig import addFolders,addFoldersSplitOnline
###from code of Soshi Tsuno: 
###https://gitlab.cern.ch/atlas/athena/blob/nightly/master/2020-01-15T2134/InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py
from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelCablingCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg, 
    PixelDCSCondHVAlgCfg, PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, 
    PixelDCSCondTempAlgCfg, PixelDistortionAlgCfg, 
    PixelHitDiscCnfgAlgCfg, PixelOfflineCalibCondAlgCfg, PixelReadoutSpeedAlgCfg, 
    PixelTDAQCondAlgCfg
# NEW FOR RUN3    PixelDeadMapCondAlgCfg
)
from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg





def PixelDigitizationBasicToolCfg(flags, name="PixelDigitizationBasicTool", **kwargs):
    """Return ComponentAccumulator with configured PixelDigitizationTool"""
    acc = PixelGeometryCfg(flags)
    
    # module parameters
    acc.merge(PixelConfigCondAlgCfg(flags,
                                    UseCalibConditions=True,
                                    UseDeadmapConditions=True,
                                    UseDCSStateConditions=False,
                                    UseDCSStatusConditions=False,
                                    UseDCSHVConditions=True,
                                    UseDCSTemperatureConditions=True,
                                    UseTDAQConditions=False))
    # charge calibration
    acc.merge(PixelChargeCalibCondAlgCfg(flags))
    # DCS setup
    acc.merge(PixelDCSCondHVAlgCfg(flags))
    acc.merge(PixelDCSCondTempAlgCfg(flags))
    # cabling setup
    acc.merge(PixelHitDiscCnfgAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))
    acc.merge(PixelCablingCondAlgCfg(flags))
    # deadmap
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    # NEW FOR RUN3    acc.merge(PixelDeadMapCondAlgCfg(flags))
    acc.merge(PixelTDAQCondAlgCfg(flags))
    # offline calibration
    acc.merge(PixelDistortionAlgCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))
    ########print('digitization')
    return acc



from SiLorentzAngleTool.SCT_LorentzAngleConfig import (SCT_LorentzAngleToolCfg, SCT_LorentzAngleCfg)


# the requires a single additional layer where the algorithm returned by getInDetPixelClusterization is collected:
def main(flags) :
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg,MainServicesSerialCfg,MainServicesMiniCfg
    #top_acc=MainServicesMiniCfg(flags)
    top_acc=MainServicesThreadedCfg(flags)
    #top_acc=MainServicesSerialCfg()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(flags))
   
    ######acc_bare.merge( IDGeoModel_bare.InDetGeoModelSvcCfg(flags) )
    print("merge Pixel and SCT Geo models configs...")
    top_acc.merge( PixelGeoModelConfig.PixelGeometryCfg(flags) )
    top_acc.merge( SCT_GeoModelConfig.SCT_GeometryCfg(flags) )
    top_acc.merge( InDetRecPreProcessingSiliconAlgorithmsCfg(flags) )
    print("merge PixelDigitizationTool...")    
    top_acc.merge( PixelDigitizationBasicToolCfg(flags))
    ##top_acc.merge(SCT_LorentzAngleToolCfg(flags))
    ##top_acc.merge(SCT_LorentzAngleCfg(flags))
    print("merge LorentzAngleTool...")    
    LorentzAngleTool = top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(flags))
    ####SiliCondTool = SCT_LorentzAngleToolCfg(flags)
    ####top_acc.setPrivateTools(SiliCondTool)

    return top_acc


if __name__ == "__main__":
    
    from AthenaCommon.Configurable import Configurable
    #Configurable.configurableRun3Behavior=1

    import IDTestFlags
    flags = IDTestFlags.IDTestFlags_q221()
    ##from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ##flags = ConfigFlags
    flags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    flags.Input.isMC = True
    flags.Detector.SimulatePixel = False
    flags.Detector.SimulateSCT   = False
    flags.Detector.SimulateTRT   = False
    flags.GeoModel.Align.Dynamic    = False
    #######flags.InDetFlags.doPixelClusterSplitting = True

    flags.Input.Files=["myRDO.pool.root"]
    flags.lock()

    acc = main(flags)
    acc.setAppProperty("EvtMax",25)
    #acc.store(open("test_SiClusterization.pkl", "w"))

    acc.run(25)