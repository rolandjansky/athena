#! /usr/bin/python

import os
import datetime

import DefineTags
import StatusFiles
import SimpleFunctions

import logging
log = logging.getLogger("myCentralPageLogger")

Prodq = "https://prodtask-dev.cern.ch/prodtask/task_table/#/?time_period=all&task_type=production&taskname="


def ReturnExpandedListFiles(ListFiles, Type):
    output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%"

    for i in range(0, len(ListFiles)):
        File = ListFiles[i]
        log.debug("ReturnExpandedListFiles - File %s", str(File))

        if i != len(ListFiles)-1:
            output += File[0]+"/ <br>"
        else:
            output += File[0]+"/"

    output += "%ENDTWISTY%"
    return output


def ReturnExpandedListFilesDAOD(ListFiles, Type):
    output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%" + "<pre>"

    for i in range(0, len(ListFiles)):
        File = ListFiles[i]
        log.debug("ReturnExpandedListFilesDAOD - file: %s", str(File))

        if i != len(ListFiles)-1:
            output += File+"/ <br>"
        else:
            output += File+"/"

    output += "</pre>%ENDTWISTY%"

    return output


def ReturnAMILink(DSID, Type, Tag):
    Link = "https://atlas-ami.cern.ch/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Command=BrowseSQLQuery+-amiAdvanced%3D%22ON%22+-amiLang%3D%22english%22+-gLiteEntity%3D%22dataset%22+-gLiteQuery%3D%22SELECT+dataset.logicalDatasetName+WHERE+dataset.amiStatus%3D%26apos%3BVALID%26apos%3B+AND+(+dataset.dataType+%3D+%26apos%3B"+Type+"%26apos%3B+)+AND+(+campaign.subcampaign+%3D+%26apos%3BMC16a%26apos%3B+)+AND+dataset.datasetNumber+like+%26apos%3B"+DSID+"%26apos%3B+%22+-mode%3D%22defaultField%22+-processingStep%3D%22production%22+-project%3D%22mc16_001%22&Converter=%2Fxsl%2FAMIProdFrameHTML.xsl&Valider=Execute&Type=AtlasProduction"
#  https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Converter=/AMIXmlToAMIProdHtml.xsl&Command=FormBrowseDatasetPerParameter+-+-datasetNumber="+DSID+"+-dataType="+Type+"+-version="+str(Tag)

    return Link


def ReturnExpandedListFilesDetail(ListFiles, iEntry, DSID, Type):
    if len(ListFiles) < 1:
        return ""

    if len(ListFiles[iEntry]) < 4:
        return ""

    output = "%TWISTY{mode=\"div\"showlink=\" "+ListFiles[iEntry][3]+": "+ListFiles[iEntry][1]+"\"hidelink=\" "+ListFiles[iEntry][3]+": "+ListFiles[iEntry][1]+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}% <br> All tags: <br> "

    for i in range(0, len(ListFiles)):

        File = ListFiles[i]
        Link = ReturnAMILink(DSID, Type, File[3])
        output += " [["+Link+"]["+File[3]+" : "+File[1]+" (events) ]]  <br>"

    output += "%ENDTWISTY%"
    return output


def ReturnExpandedListWithSubLists(SubTables, Type):
    output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%"

    for table in SubTables:
        output += table

    output += "%ENDTWISTY%"
    return output


def GetAllTags(listOfAllFiles):
    listTags = []

    for fileName in listOfAllFiles:
        if len(fileName) == 0:
            continue
        if "_p" not in fileName[0]:
            continue

        tag = fileName[0].split(".deriv.DAOD_")[1].split("_p")[1]
        tag = tag.replace("/", "")

        if tag in listTags:
            continue
        else:
            listTags.append(tag)

    return listTags


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


def fillTablesMC16(inputFileName, inputFolder, DSID, twikiFolder, EVNTFile, AODFile, DerivFile):

    File = inputFileName

    if not os.path.exists(File):
        return "empty"

    AllInfo = StatusFiles.StatusFileReader(File)

    # make list of all datasets:
    EvgenMap = SimpleFunctions.GetArraysFromTxtWithString(EVNTFile, DSID)
    AODMap = SimpleFunctions.GetArraysFromTxtWithString(AODFile, DSID)
    DAODList = SimpleFunctions.GetArraysFromTxtWithString(DerivFile, DSID)

    SortingTag = AllInfo.SortingTag

    outputFileName = twikiFolder+"/Twiki_"+SortingTag+"_"+str(DSID)+".twiki"

    cmEnergy = AllInfo.cmEnergy
    PDF = AllInfo.PDF
    tag = AllInfo.tag
    genName = AllInfo.genName
    genTune = AllInfo.genTune
    xsec = AllInfo.Xsec
    if is_number(xsec):
        xsec = float(xsec)*1000.
    xsec = str(xsec)
    filterEff = AllInfo.filterEff
    kFactor = AllInfo.kFactor
    dataset = AllInfo.dataset
    totalEvents = AllInfo.totalEvents
    comment = AllInfo.comment
    joLink = AllInfo.joLink
    description = AllInfo.description
    keywords = AllInfo.keywords
    higherOrderXsec = AllInfo.higherOrderXsec
    higherOrderXsecSample = 'Undefined'
    if is_number(xsec) and is_number(kFactor) and is_number(filterEff):
        higherOrderXsecSample = str(float(xsec)*float(kFactor)*float(filterEff))
    XsecLink = AllInfo.XsecLink
    title = AllInfo.Title
    timestamp = datetime.datetime.now()

    pandaLink = " <br> " + '[['+Prodq+'mc16_13TeV.'+DSID+'.*][PANDA link]]'

    DSIDFill = str(DSID)
    DescrFill = "Gen+PS: "+genName+" <br> Tune: "+genTune+" <br> PDF in ME: "+PDF
    genVersionFill = AllInfo.genVersion.replace(",", " <br>")

    XsecFill = str(xsec) + " | " + str(filterEff)
    AODFillFS_MC16a = ""
    AODFillAF2_MC16a = ""
    AODFillFS_MC16c = ""
    AODFillAF2_MC16c = ""
    AODFillFS_MC16d = ""
    AODFillAF2_MC16d = ""
    EvgenFill = ""

    MaxEventsEvgen = -1
    FileMaxEventsEvgen = ""
    MaxEventsAF2_MC16a = -1
    FileMaxEventsAF2_MC16a = ""
    MaxEventsFS_MC16a = -1
    FileMaxEventsFS_MC16a = ""
    MaxEventsAF2_MC16c = -1
    FileMaxEventsAF2_MC16c = ""
    MaxEventsFS_MC16c = -1
    FileMaxEventsFS_MC16c = ""
    MaxEventsAF2_MC16d = -1
    FileMaxEventsAF2_MC16d = ""
    MaxEventsFS_MC16d = -1
    FileMaxEventsFS_MC16d = ""

    lastUpdateEvgen = ""
    lastUpdateAF2_MC16a = ""
    lastUpdateFS_MC16a = ""
    lastUpdateAF2_MC16c = ""
    lastUpdateFS_MC16c = ""
    lastUpdateAF2_MC16d = ""
    lastUpdateFS_MC16d = ""

    updateFlagEvgen = False
    updateFlagFS_MC16a = False
    updateFlagAF2_MC16a = False
    updateFlagFS_MC16c = False
    updateFlagAF2_MC16c = False
    updateFlagFS_MC16d = False
    updateFlagAF2_MC16d = False

    MaxAF2Entry_MC16a = -1
    MaxFSEntry_MC16a = -1
    MaxAF2Entry_MC16c = -1
    MaxFSEntry_MC16c = -1
    MaxAF2Entry_MC16d = -1
    MaxFSEntry_MC16d = -1

    AODMapAF2_MC16a = []
    AODMapFS_MC16a = []
    AODMapAF2_MC16c = []
    AODMapFS_MC16c = []
    AODMapAF2_MC16d = []
    AODMapFS_MC16d = []

    Tags_MC16a = ["r9294", "r9315"]
    Tags_MC16c = ["r9778", "r10009"]
    Tags_MC16d = ["r10210"]
    Tags_AF2 = ["a875", "a876"]

    for iEntry in range(0, len(AODMap)):
        entry = AODMap[iEntry]

        wasUpdated = SimpleFunctions.hasBeenUpdated(entry[2], 7)

        SampleTag = (entry[0].split(".merge.AOD."))[1]
        SampleTag = SampleTag.replace("/", "")

        entry.append(SampleTag)

        # look for sub-campaign first, fast/full-sim later
        # these loops should be compressed looping over dictionaries
        # this would allow to support MC15 and MC16 in the same function
        # MC16a loop
        if SimpleFunctions.TagsInSample(Tags_MC16a, entry[0]):
            if SimpleFunctions.TagsInSample(Tags_AF2, entry[0]):
                AODMapAF2_MC16a.append(entry)
                if wasUpdated is True:
                    updateFlagAF2_MC16a = True
                    lastUpdateAF2_MC16a = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsAF2_MC16a):
                    MaxAF2Entry_MC16a += 1
                    MaxEventsAF2_MC16a = entry[1]
                    FileMaxEventsAF2_MC16a = SampleTag
            else:
                AODMapFS_MC16a.append(entry)
                if wasUpdated is True:
                    updateFlagFS_MC16a = True
                    lastUpdateFS_MC16a = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsFS_MC16a):
                    MaxFSEntry_MC16a += 1
                    MaxEventsFS_MC16a = entry[1]
                    FileMaxEventsFS_MC16a = SampleTag
        # MC16c loop
        if SimpleFunctions.TagsInSample(Tags_MC16c, entry[0]):
            if SimpleFunctions.TagsInSample(Tags_AF2, entry[0]):
                AODMapAF2_MC16c.append(entry)
                if wasUpdated is True:
                    updateFlagAF2_MC16c = True
                    lastUpdateAF2_MC16c = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsAF2_MC16c):
                    MaxAF2Entry_MC16c += 1
                    MaxEventsAF2_MC16c = entry[1]
                    FileMaxEventsAF2_MC16c = SampleTag
            else:
                AODMapFS_MC16c.append(entry)
                if wasUpdated is True:
                    updateFlagFS_MC16c = True
                    lastUpdateFS_MC16c = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsFS_MC16c):
                    MaxFSEntry_MC16c += 1
                    MaxEventsFS_MC16c = entry[1]
                    FileMaxEventsFS_MC16c = SampleTag
        # MC16d loop
        if SimpleFunctions.TagsInSample(Tags_MC16d, entry[0]):
            if SimpleFunctions.TagsInSample(Tags_AF2, entry[0]):
                AODMapAF2_MC16d.append(entry)
                if wasUpdated is True:
                    updateFlagAF2_MC16d = True
                    lastUpdateAF2_MC16d = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsAF2_MC16d):
                    MaxAF2Entry_MC16d += 1
                    MaxEventsAF2_MC16d = entry[1]
                    FileMaxEventsAF2_MC16d = SampleTag
            else:
                AODMapFS_MC16d.append(entry)
                if wasUpdated is True:
                    updateFlagFS_MC16d = True
                    lastUpdateFS_MC16d = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsFS_MC16d):
                    MaxFSEntry_MC16d += 1
                    MaxEventsFS_MC16d = entry[1]
                    FileMaxEventsFS_MC16d = SampleTag

    MaxEvgenEntry = 0
    for iEntry in range(0, len(EvgenMap)):
        entry = EvgenMap[iEntry]

        wasUpdated = SimpleFunctions.hasBeenUpdated(entry[2], 7)
        SampleTag = (entry[0].split("EVNT."))[1]
        SampleTag = SampleTag.replace("/", "")

        entry.append(SampleTag)

        if wasUpdated == "True":
            updateFlagEvgen = True
            lastUpdateEvgen = "%GREENBG% "+entry[2]+" %ENDBG%"

        if int(entry[1]) > int(MaxEventsEvgen):
            MaxEvgenEntry = iEntry
            MaxEventsEvgen = entry[1]
            FileMaxEventsEvgen = SampleTag

    EvgenFill = ReturnExpandedListFilesDetail(EvgenMap,  MaxEvgenEntry, DSID, "EVNT")

    if len(AODMapFS_MC16a) > 0:
        AODFillFS_MC16a = ReturnExpandedListFilesDetail(AODMapFS_MC16a,   MaxFSEntry_MC16a,  DSID, "AOD")
    if len(AODMapFS_MC16c) > 0:
        AODFillFS_MC16c = ReturnExpandedListFilesDetail(AODMapFS_MC16c,   MaxFSEntry_MC16c,  DSID, "AOD")
    if len(AODMapFS_MC16d) > 0:
        AODFillFS_MC16d = ReturnExpandedListFilesDetail(AODMapFS_MC16d,   MaxFSEntry_MC16d,  DSID, "AOD")

    if len(AODMapAF2_MC16a) > 0:
        AODFillAF2_MC16a = ReturnExpandedListFilesDetail(AODMapAF2_MC16a, MaxAF2Entry_MC16a, DSID, "AOD")
    if len(AODMapAF2_MC16c) > 0:
        AODFillAF2_MC16c = ReturnExpandedListFilesDetail(AODMapAF2_MC16c, MaxAF2Entry_MC16c, DSID, "AOD")
    if len(AODMapAF2_MC16d) > 0:
        AODFillAF2_MC16d = ReturnExpandedListFilesDetail(AODMapAF2_MC16d, MaxAF2Entry_MC16d, DSID, "AOD")

    outputTable = ""
    outputTable += "---++++ " + title + " \n "
    outputTable += "%TWISTY{ prefix=\"<h5>\" mode=\"div\" link=\"Full List of Available Datasets\" showimgleft=\"%ICONURLPATH{toggleopen}%\" hideimgleft=\"%ICONURLPATH{toggleclose}%\" suffix=\"</h5>\"}%"

    if(len(EvgenMap) != 0):
        outputTable += ReturnExpandedListFiles(EvgenMap, "EVNT")
    if(len(AODMap) != 0):
        outputTable += ReturnExpandedListFiles(AODMap, "AOD")

    derivTagList = DefineTags.ReturnDerivationTags()
    allTags = GetAllTags(DAODList)

    for deriv in derivTagList:

        SubTables = []

        for tag in allTags:
            SubListOfDAODTag = SimpleFunctions.GetAllFilesWithStrings(DAODList, [".DAOD_"+deriv, tag])

            if len(SubListOfDAODTag) == 0:
                continue

            SubTables.append(ReturnExpandedListFilesDAOD(SubListOfDAODTag, "DAOD_"+deriv+" ("+tag+")"))

        outputTable += ReturnExpandedListWithSubLists(SubTables, ".DAOD_"+deriv)

    FieldHigherOrderXsec = ""

    if "Undefined" not in higherOrderXsec and "defined" not in kFactor:
        FieldHigherOrderXsec = str(round(float(kFactor), 4)) + " ( [[" + XsecLink + "][" + higherOrderXsec + "]] ) <br> "
    elif "defined" not in kFactor:
        FieldHigherOrderXsec = str(round(float(kFactor), 4)) + " <br> "

    FieldHigherOrderXsecSample = ""
    if "defined" not in higherOrderXsecSample:
        FieldHigherOrderXsecSample = str(round(float(higherOrderXsecSample), 6))

    outputTable += "%ENDTWISTY%"
    outputTable += "%TABLE{tablewidth=\"100%\"}% \n"
    outputTable += "| *DSID <br> (job option link) <br> Panda link * | *Sim. type* | *Brief description* | *Generator versions* | *Cross Section [pb]* | *Filter eff.* | *Global K-factor <br> (higher order xsec [pb]) *  " + "| *Total xsec sample* " + "   | *Sample Stats <br> (Link to AMI)* | *Comment* | *LastUpdated* | \n"

    if MaxEventsEvgen > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | EVGEN | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + EvgenFill + " | " + comment + " | " + lastUpdateEvgen + " | \n"

    if MaxEventsFS_MC16a > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC16a)   | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC16a + " | " + comment + " | " + lastUpdateFS_MC16a + " | \n"

    if MaxEventsFS_MC16c > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC16c)   | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC16c + " | " + comment + " | " + lastUpdateFS_MC16c + " | \n"

    if MaxEventsFS_MC16d > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC16d)   | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC16d + " | " + comment + " | " + lastUpdateFS_MC16d + " | \n"

    if MaxEventsAF2_MC16a > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC16a)   | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC16a + " | " + comment + " | " + lastUpdateAF2_MC16a + " | \n"

    if MaxEventsAF2_MC16c > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC16c)   | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC16c + " | " + comment + " | " + lastUpdateAF2_MC16c + " | \n"

    if MaxEventsAF2_MC16d > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC16d)   | " + DescrFill + " | " + genVersionFill + " | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC16d + " | " + comment + " | " + lastUpdateAF2_MC16d + " | \n"

    outputTable += "<span style=\"font-size: 80%;\">Last update: "+str(timestamp)+" </span> \n"
    outputTable += "<br> <br>"

    outputFile = open(outputFileName, "w")
    outputFile.write(outputTable)
    outputFile.close()

    return outputFileName


def fillTables(inputFileName, inputFolder, DSID, twikiFolder, EVNTFile, AODFile, DerivFile):

    File = inputFileName

    if not os.path.exists(File):
        return "empty"

    AllInfo = StatusFiles.StatusFileReader(File)

    # make list of all datasets:
    EvgenMap = SimpleFunctions.GetArraysFromTxtWithString(EVNTFile, DSID)
    AODMap = SimpleFunctions.GetArraysFromTxtWithString(AODFile, DSID)
    DAODList = SimpleFunctions.GetArraysFromTxtWithString(DerivFile, DSID)

    SortingTag = AllInfo.SortingTag

    outputFileName = twikiFolder+"/Twiki_"+SortingTag+"_"+str(DSID)+".twiki"

    cmEnergy = AllInfo.cmEnergy
    PDF = AllInfo.PDF
    tag = AllInfo.tag
    genName = AllInfo.genName
    genTune = AllInfo.genTune
    xsec = AllInfo.Xsec
    if is_number(xsec):
        xsec = float(xsec)*1000.
    xsec = str(xsec)
    filterEff = AllInfo.filterEff
    kFactor = AllInfo.kFactor
    dataset = AllInfo.dataset
    totalEvents = AllInfo.totalEvents
    comment = AllInfo.comment
    joLink = AllInfo.joLink
    description = AllInfo.description
    keywords = AllInfo.keywords
    higherOrderXsec = AllInfo.higherOrderXsec
    higherOrderXsecSample = 'Undefined'
    if is_number(xsec) and is_number(kFactor) and is_number(filterEff):
        higherOrderXsecSample = str(float(xsec)*float(kFactor)*float(filterEff))
    XsecLink = AllInfo.XsecLink
    title = AllInfo.Title
    timestamp = datetime.datetime.now()

    pandaLink = " <br> " + '[['+Prodq+'mc15_13TeV.'+DSID+'.*][PANDA link]]'

    DSIDFill = str(DSID)
    DescrFill = "Gen+PS: " + genName + " <br> Tune: " + genTune + " <br> PDF in ME: " + PDF
    XsecFill = str(xsec) + " | " + str(filterEff)
    AODFillFS_MC15c = ""
    AODFillAF2_MC15c = ""
    EvgenFill = ""

    MaxEventsEvgen = -1
    FileMaxEventsEvgen = ""
    MaxEventsAF2_MC15c = -1
    FileMaxEventsAF2_MC15c = ""
    MaxEventsFS_MC15c = -1
    FileMaxEventsFS_MC15c = ""

    lastUpdateEvgen = ""
    lastUpdateAF2_MC15c = ""
    lastUpdateFS_MC15c = ""

    updateFlagEvgen = False
    updateFlagFS_MC15c = False
    updateFlagAF2_MC15c = False

    MaxAF2Entry_MC15c = -1
    MaxFSEntry_MC15c = -1

    AODMapAF2_MC15c = []
    AODMapFS_MC15c = []

    Tags_MC15c = ["r7676", "r8052", "r8423"]
    Tags_AF2 = ["a818", "a821", "a819", "a822", "a820", "a824", "a826", "a823", "a838"]

    for iEntry in range(0, len(AODMap)):
        entry = AODMap[iEntry]

        wasUpdated = False  # SimpleFunctions.HasBeenUpdated(entry[2], 7) Ask Andrea why these are set to False

        SampleTag = (entry[0].split(".merge.AOD."))[1]
        SampleTag = SampleTag.replace("/", "")

        entry.append(SampleTag)

        # supporting only MC15c
        if SimpleFunctions.TagsInSample(Tags_MC15c, entry[0]):
            if SimpleFunctions.TagsInSample(Tags_AF2, entry[0]):
                AODMapAF2_MC15c.append(entry)
                if wasUpdated is True:
                    updateFlagAF2_MC15c = True
                    lastUpdateAF2_MC15c = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsAF2_MC15c):
                    MaxAF2Entry_MC15c += 1
                    MaxEventsAF2_MC15c = entry[1]
                    FileMaxEventsAF2_MC15c = SampleTag
            else:
                AODMapFS_MC15c.append(entry)
                if wasUpdated is True:
                    updateFlagFS_MC15c = True
                    lastUpdateFS_MC15c = "%GREENBG% "+entry[2]+" %ENDBG%"
                if int(entry[1]) > int(MaxEventsFS_MC15c):
                    MaxFSEntry_MC15c += 1
                    MaxEventsFS_MC15c = entry[1]
                    FileMaxEventsFS_MC15c = SampleTag

    MaxEvgenEntry = 0
    for iEntry in range(0, len(EvgenMap)):
        entry = EvgenMap[iEntry]

        wasUpdated = False  # SimpleFunctions.HasBeenUpdated(entry[2], 7) Ask Andrea why these are set to False
        SampleTag = (entry[0].split("EVNT."))[1]
        SampleTag = SampleTag.replace("/", "")

        entry.append(SampleTag)

        if wasUpdated == "True":
            updateFlagEvgen = True
            lastUpdateEvgen = "%GREENBG% "+entry[2]+" %ENDBG%"

        if int(entry[1]) > int(MaxEventsEvgen):
            MaxEvgenEntry = iEntry
            MaxEventsEvgen = entry[1]
            FileMaxEventsEvgen = SampleTag

    EvgenFill = ReturnExpandedListFilesDetail(EvgenMap,  MaxEvgenEntry, DSID, "EVNT")

    if len(AODMapFS_MC15c) > 0:
        AODFillFS_MC15c = ReturnExpandedListFilesDetail(AODMapFS_MC15c,   MaxFSEntry_MC15c,  DSID, "AOD")

    if len(AODMapAF2_MC15c) > 0:
        AODFillAF2_MC15c = ReturnExpandedListFilesDetail(AODMapAF2_MC15c, MaxAF2Entry_MC15c, DSID, "AOD")

    outputTable = ""
    outputTable += "---++++ " + title + " \n "
    outputTable += "%TWISTY{ prefix=\"<h5>\" mode=\"div\" link=\"Full List of Available Derivations\" showimgleft=\"%ICONURLPATH{toggleopen}%\" hideimgleft=\"%ICONURLPATH{toggleclose}%\" suffix=\"</h5>\"}%"

    if(len(EvgenMap) != 0):
        outputTable += ReturnExpandedListFiles(EvgenMap, "EVNT")
    if(len(AODMap) != 0):
        outputTable += ReturnExpandedListFiles(AODMap, "AOD")

    derivTagList = DefineTags.ReturnDerivationTags()

    allTags = GetAllTags(DAODList)

    for deriv in derivTagList:

        SubTables = []

        for tag in allTags:

            SubListOfDAODTag = SimpleFunctions.GetAllFilesWithStrings(DAODList, [".DAOD_"+deriv, tag])
            if len(SubListOfDAODTag) == 0:
                continue

            SubTables.append(ReturnExpandedListFilesDAOD(SubListOfDAODTag, "DAOD_"+deriv+" ("+tag+")"))

        outputTable += ReturnExpandedListWithSubLists(SubTables, ".DAOD_"+deriv)

    FieldHigherOrderXsec = ""

    if "Undefined" not in higherOrderXsec and "defined" not in kFactor:
        FieldHigherOrderXsec = str(round(float(kFactor), 4)) + " ( [[" + XsecLink + "][" + higherOrderXsec + "]] ) <br> "
    elif "defined" not in kFactor:
        FieldHigherOrderXsec = str(round(float(kFactor), 4)) + " <br> "

    FieldHigherOrderXsecSample = ""
    if "defined" not in higherOrderXsecSample:
        FieldHigherOrderXsecSample = str(round(float(higherOrderXsecSample), 6))

    outputTable += "%ENDTWISTY%"
    outputTable += "%TABLE{tablewidth=\"100%\"}% \n"
    outputTable += "| *DSID <br> (job option link) <br> Panda link * | *Sim. type* | *Brief description* | *Generator versions* | *AMI Xsec [pb]* | *Filter eff.* | *Global K-factor <br> (higher order xsec [pb]) *  " + "| *Total xsec sample* "  +"   | *Sample Stats <br> (Link to AMI)* | *Comment* | *LastUpdated* | \n"

    if MaxEventsEvgen > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | EVGEN | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + EvgenFill + " | "  + comment + " | " + lastUpdateEvgen + " | \n"

    if MaxEventsFS_MC15c > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC15c)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC15c + " | "  + comment + " | " + lastUpdateFS_MC15c    + " | \n"

    if MaxEventsAF2_MC15c > 0:
        outputTable += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC15c)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC15c + " | " + comment + " | " + lastUpdateAF2_MC15c   + " | \n"

    outputTable += "<span style=\"font-size: 80%;\">Last update: "+str(timestamp)+" </span> \n"
    outputTable += "<br> <br>"

    outputFile = open(outputFileName, "w")
    outputFile.write(outputTable)
    outputFile.close()

    return outputFileName
