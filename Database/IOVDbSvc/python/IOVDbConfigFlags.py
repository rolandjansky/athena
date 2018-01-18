from AthenaConfiguration.ConfigFlags import ConfigFlag
from AthenaConfiguration.CfgLogMsg import cfgLogMsg

class DatabaseInstance(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        isMC=prevFlags.get("AthenaConfiguration.GlobalConfigFlags.isMC")
        if (isMC):
            return "OFLP200"
    
        # real-data
        projectName=prevFlags.get("AthenaConfiguration.GlobalConfigFlags.projectName")
        try:
            year=int(projectName[4:6]);
        except:
            cfgLogMsg.warning("Failed to extract year from project tag "+ projectName+". Guessing run2")
            return "CONDBR2"
        
        if (year>13):
            return "CONDBR2"
        else:
            return "COMP200" 
    

class GlobalTag(ConfigFlag):
    @staticmethod
    def getDefault(prevFlags):
        return "CONDBR2-BLKPA-2017-05"
