

def getDatabaseInstanceDefault(prevFlags):
    isMC=prevFlags.get("global.isMC")
    if (isMC):
        return "OFLP200"
    
    # real-data
    projectName=prevFlags.get("global.ProjectName")
    try:
        year=int(projectName[4:6]);
    except:
        cfgLogMsg.warning("Failed to extract year from project tag "+ projectName+". Guessing run2")
        return "CONDBR2"
        
    if (year>13):
        return "CONDBR2"
    else:
        return "COMP200" 
