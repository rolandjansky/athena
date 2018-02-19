from AthenaConfiguration.ConfigFlags import ConfigFlag

class doHVCorr(ConfigFlag):
    """ Flag to activate  HV correction at LAr cell level from DCS cool database undoing the online applied factor
    """  
    def getDefault(self,prevFlags):
        return not prevFlags.get("AthenaConfiguration.GlobalConfigFlags.isMC")
       
class doLArCellEmMisCalib(ConfigFlag):
    """ Flag to activate artificial miscalibration of simulated cell energies
    """
    def getDefault(self,prevFlags):
        return prevFlags.get("AthenaConfiguration.GlobalConfigFlags.isMC")
       

class fixedLumiForNoise(ConfigFlag):
    """Hardcoded luminosity value for pileup noise scaling"""
    def getDefault(self,prevFlags):
        return -1

class useCaloNoiseLumi(ConfigFlag):
    def getDefault(self,prevFlags):
        return True
