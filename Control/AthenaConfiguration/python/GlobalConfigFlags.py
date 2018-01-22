from AthenaConfiguration.ConfigFlags import ConfigFlag

class isMC(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return True


class isOnline(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return False

class GeoLayout(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return "atlas"

class projectName(ConfigFlag):
    #should autoconfigure! 
    @staticmethod 
    def getDefault(prevFlags):
        return "data17_13TeV"

class InputFiles(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return ["_ATHENA_GENERIC_INPUFILE_NAME_"]


#Beam-related flags:
class bunchSpacing(ConfigFlag):
    """ Bunch spacing in ns"""
    @staticmethod
    def getDefault(prevFlags):
        return 25

class numberOfCollisions(ConfigFlag):
    """ Number of collisions per beam crossing
    Should be 2.3*(L/10**33)*(bunchSpacing/25 ns)
    """
    @staticmethod
    def getDefault(prevFlags):
        return 0

class beamType(ConfigFlag):
    """ Specify data taking type : with colliding beams (default),
    single beam or cosmics 
    """  
    @staticmethod
    def getDefault(prevFlags):
        return 'collisions'

class energy (ConfigFlag):
    """ Specify beam energy (MeV)     """ 
    @staticmethod
    def getDefault(prevFlags):
        from AthenaCommon.SystemOfUnits import TeV
        return 7*TeV

class estimatedLuminosity(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return 1E33*(prevFlags.get("AthenaConfiguration.GlobalConfigFlags.numberOfCollisions")/2.3)* \
            (25./prevFlags.get("AthenaConfiguration.GlobalConfigFlags.bunchSpacing"))
                     

