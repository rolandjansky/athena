from AthenaConfiguration.ConfigFlags import ConfigFlag

class doHVCorr(ConfigFlag):
    """ Flag to activate  HV correction at LAr cell level from DCS cool database undoing the online applied factor
    """  
    @staticmethod
    def getDefault(prevFlags):
        return not prevFlags.get("AthenaConfiguration.GlobalConfigFlags.isMC")
       
class doLArCellEmMisCalib(ConfigFlag):
    """ Flag to activate artificial miscalibration of simulated cell energies
    """
    @staticmethod
    def getDefault(prevFlags):
        return prevFlags.get("AthenaConfiguration.GlobalConfigFlags.isMC")
       

class fixedLumiForNoise(ConfigFlag):
    """Hardcoded luminosity value for pileup noise scaling"""
    @staticmethod
    def getDefault(prevFlags):
        return -1

class useCaloNoiseLumi(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return True
