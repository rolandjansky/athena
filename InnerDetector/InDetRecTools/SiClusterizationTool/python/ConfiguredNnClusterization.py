## Config file for handling all setup of NnClusterizationFactory.
## This is where we will change the default calibration files when
## we have updated the neural nets.
## It should then be inherited by all other files which set up
## NnClusterizationFactory so that defaults are handled from one spot.

class PixelNNCalibrationFinder:
    
  # List of default values.
  # Update here when required.
  default_configs = {
    "TTrainedNetwork" : "/cvmfs/atlas-condb.cern.ch/repo/conditions/cond09/cond09_mc.000130.gen.COND/cond09_mc.000130.gen.COND._0001.pool.root"
  }

  def get(self, number_config="") :

    from AthenaCommon.GlobalFlags import globalflags
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory
    from AthenaCommon.AppMgr import ToolSvc
    from InDetRecExample.InDetJobProperties import InDetFlags

    # Original note on "Run 1" setup:
    # --- put in a temporary hack here for 19.1.0, to select the necessary settings when running on run 1 data/MC
    # --- since a correction is needed to fix biases when running on new run 2 compatible calibation
    # --- a better solution is needed...    

    if not "R2" in globalflags.DetDescrVersion() and not "IBL3D25" in globalflags.DetDescrVersion():
        NnClusterizationFactory = InDet__NnClusterizationFactory( name                 = "NnClusterizationFactory",
                                                                  NetworkToHistoTool   = ToolSvc.NeuralNetworkToHistoTool,
                                                                  doRunI = True,
                                                                  useToT = False,
                                                                  correctLorShiftBarrelWithTracks = 0.030,
                                                                  LoadTTrainedNetworks   = True)

    else:
        NnClusterizationFactory = InDet__NnClusterizationFactory( name                 = "NnClusterizationFactory",
                                                                  NetworkToHistoTool   = ToolSvc.NeuralNetworkToHistoTool,
                                                                  useToT = InDetFlags.doNNToTCalibration(),
                                                                  LoadTTrainedNetworks = True)

    if not number_config :
      NnClusterizationFactory.TTrainedNetwork_configFile = self.default_configs["TTrainedNetwork"]
    else :
      NnClusterizationFactory.TTrainedNetwork_configFile = number_config

    return NnClusterizationFactory

NnCalibMaker = PixelNNCalibrationFinder()
