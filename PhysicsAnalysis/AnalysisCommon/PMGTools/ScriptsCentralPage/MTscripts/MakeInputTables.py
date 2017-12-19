#! /usr/bin/python

import os
import datetime

import DefineTags
import StatusFiles
import SimpleFunctions

Prodq = "https://prodtask-dev.cern.ch/prodtask/task_table/#/?time_period=all&task_type=production&taskname="


def ReturnExpandedListFiles(ListFiles, Type):
    output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%" #<pre>"

    for i in range(0, len(ListFiles)):
        File = ListFiles[i]
        #print File
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
        log.debug("ReturnExpandedListFilesDAOD - file: %s", File)

        if i != len(ListFiles)-1:
            output += File+"/ <br>"
        else:
            output += File+"/"

    output += "</pre>%ENDTWISTY%"

    return output


def ReturnAMILink(DSID, Type, Tag):
  Link = "https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Converter=/AMIXmlToAMIProdHtml.xsl&Command=FormBrowseDatasetPerParameter+-+-datasetNumber="+DSID+"+-dataType="+Type+"+-version="+str(Tag)

  return Link


def ReturnExpandedListFilesDetail(ListFiles, iEntry, DSID, Type):


  #print iEntry,"   ",ListFiles

  if len(ListFiles) < 1:
    return ""

  if len(ListFiles[iEntry]) < 4:
    return ""

  #if str(DSID) == "410000":
  #  print "==========================================================================================================> ExpandedListFilesDetail!!! "
  #  print ListFiles[iEntry]

  output = "%TWISTY{mode=\"div\"showlink=\" "+ListFiles[iEntry][3]+": "+ListFiles[iEntry][1]+"\"hidelink=\" "+ListFiles[iEntry][3]+": "+ListFiles[iEntry][1]+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}% <br> All tags: <br> "

  for i in range(0, len(ListFiles)):

    File    = ListFiles[i]
    Link    = ReturnAMILink(DSID, Type, File[3])
    output +=  " [["+Link+"]["+File[3]+" : "+File[1]+" (events) ]]  <br>"

  output += "%ENDTWISTY%"
  return output


def ReturnExpandedListWithSubLists(SubTables, Type):
  output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%"

  for table in SubTables:
    output += table

  output += "%ENDTWISTY%"
  return output


def GetAllTags(ListOfAllFiles):
  ListTags = []
  for fileName in ListOfAllFiles:
    #print "FileName ",fileName

    if len(fileName) == 0:
      continue
    if not "_p" in fileName[0]:
      continue

    tag = fileName[0].split("_p")[1]
    #print "tag ",tag
    tag = tag.replace("/", "")
    isInList = False
    for tags in ListTags:
      if tag == tags:
        isInList = True
    if isInList:
      continue
    else:
      ListTags.append(tag)

  #print "List = "
  #print ListTags

  return ListTags

#def ReturnMapForFiles(ListOfAllFiles, Tag):
#  TotalList = []
#  for File in ListOfAllFiles:
#    if not Tag in File:
#      continue
#    AllInfo     = StatusFiles.StatusFileReader(File)
#    dataset     = AllInfo.dataset
#    totalEvents = AllInfo.totalEvents
#    tag         = AllInfo.tag
#    timeStamp   = AllInfo.lastModified
#    updated     = AllInfo.updatedStatus
#    HelpList    = [dataset, totalEvents, tag, timeStamp, updated]
#    TotalList.append(HelpList)

#  return TotalList

# function needs a string of filenames associated with xxxBLAHxxx


def fillTables(inputFileName, inputFolder, DSID, twikiFolder, EVNTFile, AODFile, DerivFile):
  # get info from input file:
  # print inputFileName

  #File = inputFolder+"/"+inputFileName

  File = inputFileName

  #print File

  if not os.path.exists(File):
    return "empty"

  AllInfo = StatusFiles.StatusFileReader(File)

  # make list of all AODs:

  EvgenMap = SimpleFunctions.GetArraysFromTxtWithString(EVNTFile, DSID)
  AODMap = SimpleFunctions.GetArraysFromTxtWithString(AODFile, DSID)
  DAODList = SimpleFunctions.GetArraysFromTxtWithString(DerivFile, DSID)

  #print DAODList

  #print AODMap

  #ListOfAllEvgen  = SimpleFunctions.GiveFilesWithSpecialStatus(inputFolder+"/"+str(DSID)+"/", [".EVNT"])
  #ListOfAllAOD    = SimpleFunctions.GiveFilesWithSpecialStatus(inputFolder+"/"+str(DSID)+"/", [".AOD"])
  #ListOfAllDAOD   = SimpleFunctions.GiveFilesWithSpecialStatus(inputFolder+"/"+str(DSID)+"/", [".DAOD"])

  # read now all derivations 

  #EvgenMap        = ReturnMapForFiles(ListOfAllEvgen, "EVNT")
  #AODMap          = ReturnMapForFiles(ListOfAllAOD,    "AOD")
  #DAODMap         = ReturnMapForFiles(ListOfAllDAOD,  "DAOD")

  SortingTag      = AllInfo.SortingTag

  outputFileName = twikiFolder+"/Twiki_"+SortingTag+"_"+str(DSID)+".twiki"

  cmEnergy        = AllInfo.cmEnergy
  PDF             = AllInfo.PDF
  tag             = AllInfo.tag
  genName         = AllInfo.genName
  genTune         = AllInfo.genTune
  xsec            = AllInfo.Xsec
  filterEff       = AllInfo.filterEff
  BR              = AllInfo.BR
  kFactor         = AllInfo.kFactor
  dataset         = AllInfo.dataset
  totalEvents     = AllInfo.totalEvents
  comment         = AllInfo.comment
  joLink          = AllInfo.joLink
  description     = AllInfo.description
  keywords        = AllInfo.keywords
  higherOrderXsec = AllInfo.higherOrderXsec
  higherOrderXsecSample = AllInfo.higherOrderXsecSample
  XsecLink        = AllInfo.XsecLink
  title           = AllInfo.Title
  #print DSID,title
  timestamp       = datetime.datetime.now()

  pandaLink       = " <br> " + '[['+Prodq+'mc16_13TeV.'+DSID+'.*][PANDA link]]'

  DSIDFill         = str(DSID)
  DescrFill        = "Gen+PS: "+genName+" <br> Tune: "+genTune+" <br> PDF in ME: "+PDF
  XsecFill         = xsec+" | "+filterEff
  AODFillFS_MC16a  = ""
  AODFillAF2_MC16a = ""
  AODFillFS_MC16c  = ""
  AODFillAF2_MC16c = ""
  EvgenFill        = ""

  MaxEventsEvgen         = -1
  FileMaxEventsEvgen     = ""
  MaxEventsAF2_MC16a     = -1
  FileMaxEventsAF2_MC16a = ""
  MaxEventsFS_MC16a      = -1
  FileMaxEventsFS_MC16a  = ""
  MaxEventsAF2_MC16c     = -1
  FileMaxEventsAF2_MC16c = ""
  MaxEventsFS_MC16c      = -1
  FileMaxEventsFS_MC16c  = ""


  lastUpdateEvgen     = ""
  lastUpdateAF2_MC16a = ""
  lastUpdateFS_MC16a  = ""
  lastUpdateAF2_MC16c = ""
  lastUpdateFS_MC16c  = ""

  updateFlagEvgen     = False
  updateFlagFS_MC16a  = False
  updateFlagAF2_MC16a = False
  updateFlagFS_MC16c  = False
  updateFlagAF2_MC16c = False

  MaxAF2Entry_MC16a   = -1
  MaxFSEntry_MC16a    = -1
  MaxAF2Entry_MC16c   = -1
  MaxFSEntry_MC16c    = -1

  AODMapAF2_MC16a = []
  AODMapFS_MC16a  = []
  AODMapAF2_MC16c = []
  AODMapFS_MC16c  = []

  TagsFS_MC16a  = ["r9477_r9315", "r9466_r9315", "r9364_r9315", "r9703_r9728"]
  TagsAF2_MC16a = ["a875_r9364"] # not sure
  TagsFS_MC16c  = ["r9781_r9778"]
  TagsAF2_MC16c = [] # doesn't exist yet

  for iEntry in range(0, len(AODMap)):
    entry = AODMap[iEntry]

    wasUpdated = SimpleFunctions.hasBeenUpdated(entry[2], 7)

    SampleTag  = (entry[0].split(".merge.AOD."))[1]
    SampleTag  = SampleTag.replace("/", "")

    entry.append(SampleTag)

    if SimpleFunctions.TagsInSample(TagsAF2_MC16a, entry[0]):
      AODMapAF2_MC16a.append(entry)
      if wasUpdated == True:
        updateFlagAF2_MC16a = True
        lastUpdateAF2_MC16a = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_MC16a):
        MaxAF2Entry_MC16a     += 1
        MaxEventsAF2_MC16a     = entry[1]
        FileMaxEventsAF2_MC16a = SampleTag

    elif SimpleFunctions.TagsInSample(TagsFS_MC16a, entry[0]):
      AODMapFS_MC16a.append(entry)
      
      if wasUpdated == True:
        updateFlagFS_MC16a = True
        lastUpdateFS_MC16a = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsFS_MC16a):
        MaxFSEntry_MC16a     += 1
        MaxEventsFS_MC16a     = entry[1]
        FileMaxEventsFS_MC16a = SampleTag

    elif SimpleFunctions.TagsInSample(TagsAF2_MC16c, entry[0]):
      AODMapAF2_MC16c.append(entry)
      if wasUpdated == True:
        updateFlagAF2_MC16c = True
        lastUpdateAF2_MC16c = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_MC16c):
        MaxAF2Entry_MC16c     += 1
        MaxEventsAF2_MC16c     = entry[1]
        FileMaxEventsAF2_MC16c = SampleTag

    elif SimpleFunctions.TagsInSample(TagsFS_MC16c, entry[0]):
      AODMapFS_MC16c.append(entry)
      #print "-------------------------> Found MC15c !!!!! ",entry                                                                                                       
      if wasUpdated == True:
        updateFlagFS_MC16c = True
        lastUpdateFS_MC16c = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsFS_MC16c):
        MaxFSEntry_MC16c     += 1
        MaxEventsFS_MC16c     = entry[1]
        FileMaxEventsFS_MC16c = SampleTag

    #else:
    #  print "WARNING(MakeInputTables): the sample "+SampleTag+" could not have been sorted, the tag is unknown!!!"

  MaxEvgenEntry = 0
  for iEntry in range(0, len(EvgenMap)):
    entry = EvgenMap[iEntry]

    wasUpdated = SimpleFunctions.hasBeenUpdated(entry[2], 7)
    SampleTag  = (entry[0].split("EVNT."))[1]
    SampleTag  = SampleTag.replace("/", "")

    entry.append(SampleTag)

    if wasUpdated == "True":
      updateFlagEvgen = True
      lastUpdateEvgen = "%GREENBG% "+entry[2]+" %ENDBG%"

    if int(entry[1]) > int(MaxEventsEvgen):
      MaxEvgenEntry      = iEntry
      MaxEventsEvgen     = entry[1]
      FileMaxEventsEvgen = SampleTag

  #print len(EvgenMap),"   ",MaxEvgenEntry
  
  #print EvgenMap

  EvgenFill  = ReturnExpandedListFilesDetail(EvgenMap,  MaxEvgenEntry, DSID, "EVNT")

  if len(AODMapFS_MC16a) > 0:
    AODFillFS_MC16a = ReturnExpandedListFilesDetail(AODMapFS_MC16a,   MaxFSEntry_MC16a,  DSID, "AOD")
  if len(AODMapFS_MC16c) > 0:
    AODFillFS_MC16c = ReturnExpandedListFilesDetail(AODMapFS_MC16c,   MaxFSEntry_MC16c,  DSID, "AOD")

  if len(AODMapAF2_MC16a) > 0:
    AODFillAF2_MC16a = ReturnExpandedListFilesDetail(AODMapAF2_MC16a, MaxAF2Entry_MC16a, DSID, "AOD")
  if len(AODMapAF2_MC16c) > 0:
    AODFillAF2_MC16c = ReturnExpandedListFilesDetail(AODMapAF2_MC16c, MaxAF2Entry_MC16c, DSID, "AOD")

  outputTable = ""
  outputTable   += "---++++ "+ title + " \n "
  outputTable   += "%TWISTY{ prefix=\"<h5>\" mode=\"div\" link=\"Full List of Available Datasets\" showimgleft=\"%ICONURLPATH{toggleopen}%\" hideimgleft=\"%ICONURLPATH{toggleclose}%\" suffix=\"</h5>\"}%"

  if(len(EvgenMap) != 0):
    outputTable += ReturnExpandedListFiles(EvgenMap, "EVNT")
  if(len(AODMap)   != 0):
    outputTable += ReturnExpandedListFiles(AODMap,    "AOD")

  DerivList = DefineTags.ReturnDerivationTags()

  allTags   = GetAllTags(DAODList)

  #print "Get all tags = ", allTags

  for deriv in DerivList:

    SubTables = []

    for tag in allTags:

       #SubListOfDAODTag = SimpleFunctions.GiveFilesWithSpecialStatus(inputFolder+"/"+str(DSID)+"/", [".DAOD_"+deriv, tag])

       SubListOfDAODTag = SimpleFunctions.GetAllFilesWithStrings(DAODList, [".DAOD_"+deriv, tag])

       #print SubListOfDAODTag

       if len(SubListOfDAODTag) == 0:
         continue

       #DAODMapLocal  = ReturnMapForFiles(SubListOfDAODTag, tag)
       #outputTable  += ReturnExpandedListFiles(DAODMapLocal, "DAOD_"+deriv+" ("+tag+")")
       SubTables.append(ReturnExpandedListFilesDAOD(SubListOfDAODTag, "DAOD_"+deriv+" ("+tag+")"))

    #if allTags:
    outputTable += ReturnExpandedListWithSubLists(SubTables, ".DAOD_"+deriv)

  FieldHigherOrderXsec = ""
  
  if not "Undefined" in higherOrderXsec and not "defined" in kFactor: # and higherOrderXsecSample != "Undefined":
    FieldHigherOrderXsec = str(round(float(kFactor), 4))+ " ( [["+XsecLink+"][" + higherOrderXsec + "]] ) <br> "
  elif not "defined" in kFactor:
    FieldHigherOrderXsec = str(round(float(kFactor), 4))+ " <br> "


  FieldHigherOrderXsecSample = ""
  if not "defined" in higherOrderXsecSample:

    FieldHigherOrderXsecSample = str(round(float(higherOrderXsecSample), 6))

  outputTable   += "%ENDTWISTY%"
  outputTable   += "%TABLE{tablewidth=\"100%\"}% \n"
  outputTable   += "| *DSID <br> (job option link) <br> Panda link * | *Sim. type* | *Brief description* | *Generator versions* | *AMI Xsec [pb]* | *Filter eff.* | *Global K-factor <br> (higher order xsec [pb]) *  " + "| *Total xsec sample* "  +"   | *Sample Stats <br> (Link to AMI)* | *Comment* | *LastUpdated* | \n"

  if MaxEventsEvgen  > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | EVGEN | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + EvgenFill + " | "  + comment + " | " + lastUpdateEvgen + " | \n"

  if MaxEventsFS_MC16a  > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC16a)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC16a + " | "  + comment + " | " + lastUpdateFS_MC16a    + " | \n"

  if MaxEventsFS_MC16c  > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC16c)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC16c + " | "  + comment + " | " + lastUpdateFS_MC16c    + " | \n"

  if MaxEventsAF2_MC16a > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC16a)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC16a + " | " + comment + " | " + lastUpdateAF2_MC16a   + " | \n"

  if MaxEventsAF2_MC16c > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC16c)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC16c + " | " + comment + " | " + lastUpdateAF2_MC16c   + " | \n"


  outputTable   += "<span style=\"font-size: 80%;\">Last update: "+str(timestamp)+" </span> \n"
  outputTable   += "<br> <br>"

  #outputFileName = twikiFolder+"/Twiki_"+SortingTag+"_"+str(DSID)+".twiki"

  outputFile = open(outputFileName, "w")
  outputFile.write(outputTable)
  outputFile.close()

  return outputFileName

