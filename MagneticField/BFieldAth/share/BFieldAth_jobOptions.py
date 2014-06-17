from AthenaCommon.Logging import logging
logging.getLogger().error("BFieldAth/BFieldAth_jobOptions.py is OBSOLETE as the package is being DISCONTINUED")
logging.getLogger().error("please use import MagFieldServices.SetupField instead")
logging.getLogger().error("package BField will be removed from the release on June 13th, 2014")

raise RuntimeError('protectedInclude(\'BFieldAth/BFieldAth_jobOptions.py\') is OBSOLETE, use instead: import MagFieldServices.SetupField')
