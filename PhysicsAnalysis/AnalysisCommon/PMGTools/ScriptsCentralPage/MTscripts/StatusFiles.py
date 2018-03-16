#! /usr/bin/python

import os
import sys
import datetime
import SimpleFunctions
import logging
import DefineTags

import pyAMI
import pyAMI.client
import pyAMI.atlas.api

log = logging.getLogger("myCentralPageLogger")


def getLocalJoFileName(DSID, mainFolder):

    joLinkPath = "https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/share/"
    joLinkPath_12 = "https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC12JobOptions/trunk/share/"
    localJoPath_12 = "/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/share/DSID"+DSID[0:3]+"xxx/"
    localJoPath = "/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID"+DSID[0:3]+"xxx/"

    joPath = "DoesNotExist"
    joLink = "DoesNotExist"

    if not os.path.exists(localJoPath):
        localJoPath = localJoPath_12
        if not os.path.exists(localJoPath):
            return [joPath, joLink]

    log.debug("getLocalJoFileName: %s, %s", DSID[0:3], localJoPath)

    fileList = os.listdir(localJoPath)
    for file in fileList:
        if "MC12."+str(DSID) in file:
            joLink = joLinkPath_12+"DSID"+DSID[0:3]+"xxx/"+file
            joPath = localJoPath_12+file
            break
        elif "MC15."+str(DSID) in file:
            joLink = joLinkPath+"DSID"+DSID[0:3]+"xxx/"+file
            joPath = localJoPath+file
            break

    return [joPath, joLink]


def GetFileEntry(EntryList, EntryType):
    Status = ""
    for Entry in EntryList:
        if Entry.startswith(EntryType+"."):
            Status = Entry.replace(EntryType+".", "")
            Status = Status.replace("\n", "")
            return Status
    if Status == "":
        log.error("EntryType=%s is not defined!!! ---> EXIT.", str(EntryType))
        Status = "Entry not defined!!!"
        return Status


class StatusFileCreator(object):

    def __init__(self, metadata, mainFolder, twikiFolder):

        log.debug('StatusFileCreator %s %s %s', metadata["ldn"], mainFolder, twikiFolder)

        statusFile = "StatusFile_"+metadata["processGroup"]+"_"+str(metadata["dataset_number"])+"_"+metadata["ldn"].replace("/", "")+".txt"
        newFolder = mainFolder+"DSID_"+str(metadata["dataset_number"][0:3])+"XXX/"+str(metadata["dataset_number"])+"/"
        statusFileName = newFolder+"/"+statusFile
        joPaths = getLocalJoFileName(metadata["dataset_number"], mainFolder)
        timestamp = datetime.datetime.now()

        statusFile = open(statusFileName, 'w')
        statusFile.write("DSID."+metadata["dataset_number"]+"\n")
        statusFile.write("joLink."+joPaths[1]+"\n")
        if "mc12_13TeV." not in metadata["ldn"]:
            statusFile.write("description."+metadata["physics_comment"]+"\n")
        else:
            statusFile.write("description."+metadata["ldn"]+"\n")
        statusFile.write("keywords.\n")
        statusFile.write("StatusFileName."+statusFileName+"\n")
        statusFile.write("StatusFileTimeStamp."+str(timestamp)+"\n")
        statusFile.write("SortingTag."+metadata["processGroup"]+"\n")
        statusFile.write("dataset."+metadata["ldn"]+"\n")
        statusFile.write("dataType."+metadata["type"]+"\n")
        statusFile.write("totalEvents."+metadata["events"]+"\n")
        if "mc12_13TeV." not in metadata["ldn"]:
            statusFile.write("cmEnergy."+metadata["ecm_energy"]+"\n")
        else:
            statusFile.write("cmEnergy.13000000\n")
        statusFile.write("PDF."+metadata["pdf"]+"\n")
        statusFile.write("tag."+metadata["ami_tags"]+"\n")
        statusFile.write("genName."+metadata["generator_name"]+"\n")
        '''
        tag = metadata["ami_tags"]  # getting sample tag
        etag = ''
        for subtag in tag.split("_"):
            if "e" in subtag:
                etag = subtag
        log.debug("e-tag %s", etag)

        cmd = 'AMIGetAMITagInfo -amiTag=%s' % etag
        result = client.execute(cmd, format='dict_object')
        dict = result.get_rows('amiTagInfo')
        cache = dict[0]["cacheName"]
        log.debug("AtlasProduction,%s", cache)

        gen_list = []
        if "+" in amiInfo["generatorName"]:
            gen_list = amiInfo["generatorName"].split('+')
        else:
            gen_list.append(amiInfo["generatorName"])

        log.debug("Gen list size %d", len(gen_list))

        package_tag = []
        for program in gen_list:
            query_name = '%'+program+'%'
            log.debug("Querying for %s", query_name)

            cmd2 = 'AMIGetAMITagPackageVersions -amiTag=%(tag)s --packageFilter=%(pkg)s' % {'tag': etag, 'pkg': query_name}
            result = client.execute(cmd2, format='dict_object')
            dict2 = result.get_rows()

            for element in dict2:
                if element["releaseName"] == cache:
                    if element["packageTag"] not in package_tag:
                        package_tag.append(element["packageTag"])

        genVersion = ','.join(package_tag)
        '''
        statusFile.write("genVersion.\n")
        statusFile.write("genTune."+metadata["generator_tune"]+"\n")
        statusFile.write("Xsec."+metadata["crossSection"]+"\n")
        statusFile.write("filterEff."+metadata["genFiltEff"]+"\n")
        statusFile.write("XsecLink."+metadata["crossSectionRef"]+"\n")
        statusFile.write("higherOrderXsec.Undefined\n")
        if metadata["physics_comment"] == "":
            statusFile.write("Title."+metadata["physics_short"]+"\n")
        else:
            statusFile.write("Title."+metadata["physics_comment"]+"\n")
        statusFile.write("comment."+metadata["physics_comment"]+"\n")
        statusFile.write("lastModified."+metadata["modified"]+"\n")
        statusFile.write("kFactor."+metadata["kFactor"]+"\n")

        statusFile.write(" \n")
        statusFile.close()


class StatusFileReader(object):
    def __init__(self, StatusFile):
        self.__StatusFile = StatusFile

        fInFile = open(self.__StatusFile, 'r')
        fInputLines = fInFile.readlines()
        fInFile.close()

        self.AllLines = fInputLines
        self.DSID = GetFileEntry(self.AllLines, "DSID")
        self.joLink = GetFileEntry(self.AllLines, "joLink")
        self.description = GetFileEntry(self.AllLines, "description")
        self.keywords = GetFileEntry(self.AllLines, "keywords")
        self.StatusFileName = GetFileEntry(self.AllLines, "StatusFileName")
        self.StatusFileTimeStamp = GetFileEntry(self.AllLines, "StatusFileTimeStamp")
        self.SortingTag = GetFileEntry(self.AllLines, "SortingTag")
        self.dataset = GetFileEntry(self.AllLines, "dataset")
        self.dataType = GetFileEntry(self.AllLines, "dataType")
        self.totalEvents = GetFileEntry(self.AllLines, "totalEvents")
        self.cmEnergy = GetFileEntry(self.AllLines, "cmEnergy")
        self.PDF = GetFileEntry(self.AllLines, "PDF")
        self.tag = GetFileEntry(self.AllLines, "tag")
        self.genName = GetFileEntry(self.AllLines, "genName")
        self.genVersion = GetFileEntry(self.AllLines, "genVersion")
        self.genTune = GetFileEntry(self.AllLines, "genTune")
        self.Xsec = GetFileEntry(self.AllLines, "Xsec")
        self.filterEff = GetFileEntry(self.AllLines, "filterEff")
        self.kFactor = GetFileEntry(self.AllLines, "kFactor")
        self.higherOrderXsec = GetFileEntry(self.AllLines, "higherOrderXsec")
        self.XsecLink = GetFileEntry(self.AllLines, "XsecLink")
        self.comment = GetFileEntry(self.AllLines, "comment")
        self.Title = GetFileEntry(self.AllLines, "Title")
        self.lastModified = GetFileEntry(self.AllLines, "lastModified")
