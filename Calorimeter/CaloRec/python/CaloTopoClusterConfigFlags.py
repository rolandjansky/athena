from AthenaConfiguration.ConfigFlags import ConfigFlag

class doTwoGaussianNoise(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return True


class doTreatEnergyCutAsAbsolute(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return False

