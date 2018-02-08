from AthenaConfiguration.ConfigFlags import ConfigFlag,makeFlag
from AthenaConfiguration.CfgLogMsg import cfgLogMsg

class DatabaseInstance(ConfigFlag):
    def getDefault(self,prevFlags):
        isMC=prevFlags.get("AthenaConfiguration.GlobalFlags.isMC")
        if (isMC):
            return "OFLP200"
    
        # real-data
        projectName=prevFlags.get("AthenaConfiguration.GlobalFlags.projectName")
        try:
            year=int(projectName[4:6]);
        except:
            cfgLogMsg.warning("Failed to extract year from project tag "+ projectName+". Guessing run2")
            return "CONDBR2"
        
        if (year>13):
            return "CONDBR2"
        else:
            return "COMP200" 
    
GlobalTag=makeFlag("GlobalTag","CONDBR2-BLKPA-2017-05")

