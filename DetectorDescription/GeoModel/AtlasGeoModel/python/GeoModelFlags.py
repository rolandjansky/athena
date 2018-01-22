from AthenaConfiguration.ConfigFlags import ConfigFlag

class AtlasVersion(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return "ATLAS-R2-2016-01-00-01" 
