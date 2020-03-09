# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('IOVDbAutoCfgFlags')

def getDatabaseInstanceDefault(prevFlags):
    isMC=prevFlags.Input.isMC
    if (isMC):
        return "OFLP200"
    
    # real-data
    projectName=prevFlags.Input.ProjectName
    try:
        year=int(projectName[4:6])
    except Exception:
        log.warning("Failed to extract year from project tag "+ projectName+". Guessing run2")
        return "CONDBR2"
        
    if (year>13):
        return "CONDBR2"
    else:
        return "COMP200" 
