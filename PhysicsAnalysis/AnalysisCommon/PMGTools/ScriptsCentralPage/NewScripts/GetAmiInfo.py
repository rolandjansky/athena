#! /usr/bin/python                                                                                                                                                   

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
  
import subprocess,sys
import os,string
import time
import pyAMI
import pyAMI.client
import pyAMI.atlas.api

def FindAllContainersArray(List, DSID, Type):
    NewArray = []

    for entry in List:
        if not DSID in entry:
            continue
        if not Type in entry:
            continue
        
        NewArray.append(entry)

    return NewArray

def FindAllContainers(mcType, DSID, Type):
    client   = pyAMI.client.Client('atlas')
    #print "before init"
    pyAMI.atlas.api.init()
    #print "after init"

    currentPattern = "" 
    if   Type == "DAOD":
        currentPattern = mcType+"."+str(DSID)+"%merge.DAOD_%/"
    elif Type ==  "AOD":
        currentPattern = mcType+"."+str(DSID)+"%merge.AOD%/"
    elif Type ==  "EVNT":
        currentPattern = mcType+"."+str(DSID)+"%evgen.EVNT%/"
    else:
        exit

    #print "before list datasets"

    FullList = pyAMI.atlas.api.list_datasets(client, patterns=currentPattern) 

    #print "after list datasets"

    Keywords = ""

    ContainerList = []
    for entry in FullList:
        ContainerName = entry["ldn"]
        #Keywords      = entry["keyword"]
        ContainerList.append(ContainerName)
    
    SortedList = sorted(ContainerList)
    
    return SortedList

def GetKeywords(client, ContainerName):
    #client   = pyAMI.client.Client('atlas')
    #pyAMI.atlas.api.init()
    #FullListAll = [["empty"]]
    try:
        FullListAll = pyAMI.atlas.api.get_dataset_info(client, ContainerName)
        FullList    = FullListAll[0]
        Keywords    = FullList["keyword"]
        return Keywords
    except:
        return "Keywords empty"

