# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def pathToProjectCMTDir(project, projectCMTDirs, useRunTime=False):
    if useRunTime: return projectCMTDirs[project]['RunTime']
    return projectCMTDirs[project]['Release']

    # -------------------------------------------------------------------------------
    
def allProjectCMTDirs(projectCMTDirs, useRunTime=False):
    releaseVals = []
    runTimeVals = []
    for releaseRunTimeDict in projectCMTDirs.values():
        releaseVals.append(releaseRunTimeDict['Release'])
        runTimeVals.append(releaseRunTimeDict['RunTime'])

    if useRunTime:
        return runTimeVals
    return releaseVals
    
def allProjects(projectCMTDirs):
    return projectCMTDirs.keys()

