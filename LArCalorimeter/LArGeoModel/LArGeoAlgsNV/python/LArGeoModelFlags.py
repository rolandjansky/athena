from AthenaConfiguration.ConfigFlags import ConfigFlag

class doAlign(ConfigFlag):
    """ Flag to activate LAr Alignment
    """  
    @staticmethod
    def getDefault(prevFlags):
        return prevFlags.get("AthenaConfiguration.GlobalConfigFlags.GeoLayout")=="atlas"
