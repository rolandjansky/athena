#! /usr/bin/python

import pyAMI
import pyAMI.client
import pyAMI.atlas.api


def FindAllContainersArray(list, dsid, type):
    newArray = []

    for entry in list:
        if dsid not in entry:
            continue
        if type not in entry:
            continue

        newArray.append(entry)

    return newArray


def FindAllContainers(mcType, dsid, type):
    client = pyAMI.client.Client('atlas')
    pyAMI.atlas.api.init()

    currentPattern = ""
    if type == "DAOD":
        currentPattern = mcType+"."+str(dsid)+"%deriv.DAOD_%/"
    elif type == "AOD":
        currentPattern = mcType+"."+str(dsid)+"%merge.AOD%/"
    elif type == "EVNT":
        currentPattern = mcType+"."+str(dsid)+"%evgen.EVNT%/"
    else:
        exit

    fullList = pyAMI.atlas.api.list_datasets(client, patterns=currentPattern)

    containerList = []
    for entry in fullList:
        containerName = entry["ldn"]
        containerList.append(containerName)

    sortedList = sorted(containerList)

    return sortedList


def getKeywords(client, containerName):

    try:
        fullListAll = pyAMI.atlas.api.get_dataset_info(client, containerName)
        fullList = fullListAll[0]
        keywords = fullList["keyword"]
        return keywords
    except:
        return "Keywords empty"
