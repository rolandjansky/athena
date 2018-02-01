from AthenaConfiguration.ConfigFlags import ConfigFlag

class doAlign(ConfigFlag):
    """ Flag to activate LAr Alignment
    """  
    def getDefault(self,prevFlags):
        return prevFlags.get("AthenaConfiguration.GlobalConfigFlags.GeoLayout")=="atlas"
