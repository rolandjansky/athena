#! /usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys,glob
import datetime

import DefineTags
import StatusFiles
import SimpleFunctions

MainFolder    = os.getcwd()+"/AllSampleInfo/"
SummaryFolder = MainFolder+"/SummaryInfo/"
AODFile       = SummaryFolder+"/ListAllAODs.txt"
EVNTFile      = SummaryFolder+"/ListAllEVNTs.txt"
DerivFile     = SummaryFolder+"/ListAllDerivations.txt"
ReducedFile   = SummaryFolder+"/ReducedInputList.txt"

Prodq="http://bigpanda.cern.ch/prodsys/prodtask/task_table/#/?time_period=all&task_type=production&taskname="

def ReturnExpandedListFiles(ListFiles, Type):
  output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%" #<pre>"

  
  for i in range(0, len(ListFiles)):
    File    = ListFiles[i]
    #print File
    if i != len(ListFiles)-1:
      output += File[0]+"/ <br>"
    else:
      output += File[0]+"/"

  

  #output += "</pre>%ENDTWISTY%"
  output += "%ENDTWISTY%"
  return output

def ReturnExpandedListFilesDAOD(ListFiles, Type):
  output = "%TWISTY{mode=\"div\"showlink=\"Full list of "+Type+"\"hidelink=\"Full list of "+Type+"\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%" + "<pre>"

  
  for i in range(0, len(ListFiles)):
    File    = ListFiles[i]
    #print File                                                                                                                                                                                                
    if i != len(ListFiles)-1:
      output += File+"/ <br>"
    else:
      output += File+"/"

  
  output += "</pre>%ENDTWISTY%"                                                                                                                                                                               
  #output += "%ENDTWISTY%"
  return output



def ReturnAMILink(DSID, Type, Tag):
  Link = "https://ami.in2p3.fr/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.Command?Converter=/AMIXmlToAMIProdHtml.xsl&Command=FormBrowseDatasetPerParameter+-+-datasetNumber="+DSID+"+-dataType="+Type+"+-version="+str(Tag)

  return Link

def ReturnExpandedListFilesDetail(ListFiles, iEntry, DSID, Type):

  #print iEntry
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
    if not "_p" in fileName:
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
def fillTables(inputFileName, inputFolder, DSID, twikiFolder):
  # get info from input file:
  #print inputFileName


  #File = inputFolder+"/"+inputFileName

  File = inputFileName

  #print File

  if not os.path.exists(File):
    return "empty"

  AllInfo = StatusFiles.StatusFileReader(File)

  # make list of all AODs:

  EvgenMap = SimpleFunctions.GetArraysFromTxtWithString(EVNTFile, DSID)
  AODMap   = SimpleFunctions.GetArraysFromTxtWithString(AODFile, DSID)
  #DAODList = SimpleFunctions.GetArraysFromTxt(inputFolder+"/List_Derivations.txt")
  DAODList = SimpleFunctions.GetArraysFromTxtWithString(DerivFile, DSID)


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

  pandaLink       = " <br> " + '[['+Prodq+'mc15_13TeV.'+DSID+'*][PANDA link]]'

  DSIDFill         = str(DSID)
  DescrFill        = "Gen+PS: "+genName+" <br> Tune: "+genTune+" <br> PDF in ME: "+PDF
  XsecFill         = xsec+" | "+filterEff
  AODFillFS50      = ""
  AODFillFS25      = ""
  AODFillFSEarly   = ""
  AODFillAF2_50    = ""
  AODFillAF2_25    = ""
  AODFillAF2_Early = ""
  AODFillFS_MC15b  = ""
  AODFillAF2_MC15b = ""
  AODFillFS_MC15c  = ""
  AODFillAF2_MC15c = ""
  EvgenFill        = ""

  MaxEventsAF2_50        = -1
  FileMaxEventsAF2_50    = ""
  MaxEventsAF2_25        = -1
  FileMaxEventsAF2_25    = ""
  MaxEventsFS50          = -1
  FileMaxEventsFS50      = ""
  MaxEventsFS25          = -1
  FileMaxEventsFS25      = ""
  MaxEventsFSEarly       = -1
  FileMaxEventsFSEarly   = ""
  MaxEventsEvgen         = -1
  FileMaxEventsEvgen     = ""
  MaxEventsAF2_Early     = -1
  FileMaxEventsAF2_Early = ""
  MaxEventsAF2_MC15b     = -1
  FileMaxEventsAF2_MC15b = ""
  MaxEventsFS_MC15b      = -1
  FileMaxEventsFS_MC15b  = ""
  MaxEventsAF2_MC15c     = -1
  FileMaxEventsAF2_MC15c = ""
  MaxEventsFS_MC15c      = -1
  FileMaxEventsFS_MC15c  = ""


  lastUpdateEvgen     = ""
  lastUpdateFS50      = ""
  lastUpdateFS25      = ""
  lastUpdateFSEarly   = ""
  lastUpdateAF2_25    = ""
  lastUpdateAF2_50    = ""
  lastUpdateAF2_Early = ""
  lastUpdateAF2_MC15b = ""
  lastUpdateFS_MC15b  = ""
  lastUpdateAF2_MC15c = ""
  lastUpdateFS_MC15c  = ""


  updateFlagEvgen     = False
  updateFlagFS50      = False
  updateFlagFS25      = False
  updateFlagFSEarly   = False
  updateFlagAF2_25    = False
  updateFlagAF2_50    = False
  updateFlagAF2_Early = False
  updateFlagFS_MC15b  = False
  updateFlagAF2_MC15b = False
  updateFlagFS_MC15c  = False
  updateFlagAF2_MC15c = False


  MaxFSEntryEarly     = -1
  MaxFSEntry50        = -1
  MaxFSEntry25        = -1
  MaxAF2Entry_25      = -1
  MaxAF2Entry_50      = -1
  MaxAF2Entry_Early   = -1
  MaxAF2Entry_MC15b   = -1
  MaxFSEntry_MC15b    = -1
  MaxAF2Entry_MC15c   = -1
  MaxFSEntry_MC15c    = -1

  AODMapFSEarly   = []
  AODMapFS50      = []
  AODMapFS25      = []
  AODMapAF2_25    = []
  AODMapAF2_50    = []
  AODMapAF2_Early = []
  AODMapAF2_MC15b = []
  AODMapFS_MC15b  = []
  AODMapAF2_MC15c = []
  AODMapFS_MC15c  = []

  #a769: week1
  #a767: 50ns

  TagsFS_50ns   = ["r6630_r6264", "r6647_r6264", "r6655", "r6767_r6264", "r6793_r6264", "r6761","r6802_r6264","r6828_r6264"] # tight trigger (default), tight trigger -> small samples tag
  TagsFS_25ns   = ["r6768_r6282", "r6765_r6282", "r6725_r6282", "r6771_r6282", "r7042_r6282", "r7049_r6282", "r7051_r6282", "r6869_r6282"] # tight trigger (default), minbias trigger
  TagsFS_Early  = ["r6633", "6853"] # default tag
  TagsAF2_25    = ["a768_r6282", "a777_r6282"]
  TagsAF2_50    = ["a767_r6264","a780_r6264"]
  TagsAF2_Early = ["a769_"]
  TagsFS_MC15b  = ["r7267_r6282", "r7326_r6282", "r7360_r6282"] # default tag
  TagsAF2_MC15b = ["a810_r6282"] # not defined yet
  TagsFS_MC15c  = ["r7725_r7676", "r7772_r7676","r8084_r7676","r8112_r7676","r8129_r7676"]
  TagsAF2_MC15c = ["a818_r7676","a821_r7676"]
  
# Andrea: I do not show: r7051, r7050 (both no pileup)

#MC production list of tag - 1/07/2015
#       tagDict = {'r6630' : '50ns',  'a769' : 'week1', 'r6765' : '25ns',
#'r6655' : '50ns', 'a768' : '25ns', 'a767' : '50ns', 'r6633' : 'week1',
#'r6647' : '50ns', 'r6853' : 'week1', 'r6793' : '50ns', 'r6828' : '50ns',
#'r6767' : '50ns', 'r6725' : '25ns', 'r6802' : '50ns'}


  for iEntry in range(0, len(AODMap)):
    entry = AODMap[iEntry]

    wasUpdated = SimpleFunctions.HasBeenUpdated(entry[2], 7)

    SampleTag  = (entry[0].split(".merge.AOD."))[1]
    SampleTag  = SampleTag.replace("/", "")

    entry.append(SampleTag)

    if SimpleFunctions.TagsInSample(TagsFS_Early, entry[0]):
      AODMapFSEarly.append(entry)
      if wasUpdated == True:
        updateFlagFSEarly = True
        lastUpdateFSEarly = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsFSEarly):
        MaxFSEntryEarly      += 1
        MaxEventsFSEarly     = entry[1]
        FileMaxEventsFSEarly = SampleTag

    elif SimpleFunctions.TagsInSample(TagsFS_50ns, entry[0]):
      AODMapFS50.append(entry)
      if wasUpdated == True:
        updateFlagFS50 = True
        lastUpdateFS50 = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsFS50):
        MaxFSEntry50      += 1
        MaxEventsFS50     = entry[1]
        FileMaxEventsFS50 = SampleTag

    elif SimpleFunctions.TagsInSample(TagsFS_25ns, entry[0]):
      AODMapFS25.append(entry)
      if wasUpdated == True:
        updateFlagFS25 = True
        lastUpdateFS25 = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsFS25):
        MaxFSEntry25      += 1
        MaxEventsFS25     = entry[1]
        FileMaxEventsFS25 = SampleTag

    elif SimpleFunctions.TagsInSample(TagsAF2_25, entry[0]):
      AODMapAF2_25.append(entry)
      if wasUpdated == True:
        updateFlagAF2_25 = True
        lastUpdateAF2_25 = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_25):
        MaxAF2Entry_25     += 1
        MaxEventsAF2_25     = entry[1]
        FileMaxEventsAF2_25 = SampleTag

    elif SimpleFunctions.TagsInSample(TagsAF2_50, entry[0]):
      AODMapAF2_50.append(entry)
      if wasUpdated == True:
        updateFlagAF2_50 = True
        lastUpdateAF2_50 = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_50):
        MaxAF2Entry_50     += 1
        MaxEventsAF2_50     = entry[1]
        FileMaxEventsAF2_50 = SampleTag

    elif SimpleFunctions.TagsInSample(TagsAF2_Early, entry[0]):
      AODMapAF2_Early.append(entry)
      if wasUpdated == True:
        updateFlagAF2_Early = True
        lastUpdateAF2_Early = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_Early):
        MaxAF2Entry_Early     += 1
        MaxEventsAF2_Early     = entry[1]
        FileMaxEventsAF2_Early = SampleTag

    elif SimpleFunctions.TagsInSample(TagsAF2_MC15b, entry[0]):
      AODMapAF2_MC15b.append(entry)
      if wasUpdated == True:
        updateFlagAF2_MC15b = True
        lastUpdateAF2_MC15b = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_MC15b):
        MaxAF2Entry_MC15b     += 1
        MaxEventsAF2_MC15b     = entry[1]
        FileMaxEventsAF2_MC15b = SampleTag

    elif SimpleFunctions.TagsInSample(TagsFS_MC15b, entry[0]):
      AODMapFS_MC15b.append(entry)
      
      if wasUpdated == True:
        updateFlagFS_MC15b = True
        lastUpdateFS_MC15b = "%GREENBG% "+entry[2]+" %ENDBG%"

      #print entry
      #print int(entry[1]),"   ",int(MaxEventsFS_MC15b),"   ",FileMaxEventsFS_MC15b

      if int(entry[1]) > int(MaxEventsFS_MC15b):
        MaxFSEntry_MC15b     += 1
        MaxEventsFS_MC15b     = entry[1]
        FileMaxEventsFS_MC15b = SampleTag

        #print "======> new = ",MaxFSEntry_MC15b,"   ",MaxEventsFS_MC15b,"  ",FileMaxEventsFS_MC15b

      #print entry[1] 

    elif SimpleFunctions.TagsInSample(TagsAF2_MC15c, entry[0]):
      AODMapAF2_MC15c.append(entry)
      if wasUpdated == True:
        updateFlagAF2_MC15c = True
        lastUpdateAF2_MC15c = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsAF2_MC15c):
        MaxAF2Entry_MC15c     += 1
        MaxEventsAF2_MC15c     = entry[1]
        FileMaxEventsAF2_MC15c = SampleTag

    elif SimpleFunctions.TagsInSample(TagsFS_MC15c, entry[0]):
      AODMapFS_MC15c.append(entry)
      #print "-------------------------> Found MC15c !!!!! ",entry                                                                                                       
      if wasUpdated == True:
        updateFlagFS_MC15c = True
        lastUpdateFS_MC15c = "%GREENBG% "+entry[2]+" %ENDBG%"
      if int(entry[1]) > int(MaxEventsFS_MC15c):
        MaxFSEntry_MC15c     += 1
        MaxEventsFS_MC15c     = entry[1]
        FileMaxEventsFS_MC15c = SampleTag


    #else:
    #  print "WARNING(MakeInputTables): the sample "+SampleTag+" could not have been sorted, the tag is unknown!!!"

  MaxEvgenEntry = 0
  for iEntry in range(0, len(EvgenMap)):
    entry = EvgenMap[iEntry]

    wasUpdated = SimpleFunctions.HasBeenUpdated(entry[2], 7)
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

  if len(AODMapFSEarly) > 0:
    AODFillFSEarly   = ReturnExpandedListFilesDetail(AODMapFSEarly,   MaxFSEntryEarly,   DSID, "AOD")
  if len(AODMapFS50) > 0:
     AODFillFS50     = ReturnExpandedListFilesDetail(AODMapFS50,      MaxFSEntry50,      DSID, "AOD")
  if len(AODMapFS25) > 0:
    AODFillFS25      = ReturnExpandedListFilesDetail(AODMapFS25,      MaxFSEntry25,      DSID, "AOD")
  if len(AODMapFS_MC15b) > 0:
    AODFillFS_MC15b = ReturnExpandedListFilesDetail(AODMapFS_MC15b,   MaxFSEntry_MC15b,  DSID, "AOD")
  if len(AODMapFS_MC15c) > 0:
    AODFillFS_MC15c = ReturnExpandedListFilesDetail(AODMapFS_MC15c,   MaxFSEntry_MC15c,  DSID, "AOD")


  #if "361600" in DSID:
  #  print "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
  #  print AODMapFS_MC15b
  #  print AODFillFS_MC15b

  if len(AODMapAF2_25) > 0:
    AODFillAF2_25    = ReturnExpandedListFilesDetail(AODMapAF2_25,    MaxAF2Entry_25,    DSID, "AOD")
  if len(AODMapAF2_50) > 0:
    AODFillAF2_50    = ReturnExpandedListFilesDetail(AODMapAF2_50,    MaxAF2Entry_50,    DSID, "AOD")
  if len(AODMapAF2_Early) > 0:
    AODFillAF2_Early = ReturnExpandedListFilesDetail(AODMapAF2_Early, MaxAF2Entry_Early, DSID, "AOD")
  if len(AODMapAF2_MC15b) > 0:
    AODFillAF2_MC15b = ReturnExpandedListFilesDetail(AODMapAF2_MC15b, MaxAF2Entry_MC15b, DSID, "AOD")
  if len(AODMapAF2_MC15c) > 0:
    AODFillAF2_MC15c = ReturnExpandedListFilesDetail(AODMapAF2_MC15c, MaxAF2Entry_MC15c, DSID, "AOD")

  outputTable = ""
  outputTable   += "---++++ "+ title + " \n "
  outputTable   += "%TWISTY{ prefix=\"<h5>\" mode=\"div\" link=\"Full List of Available Derivations\" showimgleft=\"%ICONURLPATH{toggleopen}%\" hideimgleft=\"%ICONURLPATH{toggleclose}%\" suffix=\"</h5>\"}%"

  if(len(EvgenMap) != 0):
    outputTable += ReturnExpandedListFiles(EvgenMap, "EVNT")
  if(len(AODMap)   != 0):
    outputTable += ReturnExpandedListFiles(AODMap,    "AOD")

  DerivList = DefineTags.ReturnDerivationTags()

  allTags   = GetAllTags(DAODList)

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

  #if MaxEventsFSEarly  > 0:
  #  outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (week one)    | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFSEarly + " | "  + comment + " | " + lastUpdateFSEarly    + " | \n"

  #if MaxEventsFS50  > 0:
  #  outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (50 ns)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS50 + " | "  + comment + " | " + lastUpdateFS50    + " | \n"

  if MaxEventsFS25  > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (25 ns)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS25 + " | "  + comment + " | " + lastUpdateFS25    + " | \n"

  if MaxEventsFS_MC15b  > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC15b)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC15b + " | "  + comment + " | " + lastUpdateFS_MC15b    + " | \n"

  if MaxEventsFS_MC15c  > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | FS (MC15c)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillFS_MC15c + " | "  + comment + " | " + lastUpdateFS_MC15c    + " | \n"

    
  #if MaxEventsAF2_Early > 0:
  #  outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (week one)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_Early + " | " + comment + " | " + lastUpdateAF2_Early + " | \n"

  if MaxEventsAF2_50 > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (50 ns)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_50 + " | " + comment + " | " + lastUpdateAF2_50   + " | \n"

  if MaxEventsAF2_25 > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (25 ns)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_25 + " | " + comment + " | " + lastUpdateAF2_25   + " | \n"

  if MaxEventsAF2_MC15b > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC15b)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC15b + " | " + comment + " | " + lastUpdateAF2_MC15b   + " | \n"

  if MaxEventsAF2_MC15c > 0:
    outputTable   += "| [["+joLink+"][" + DSIDFill + "]] "+pandaLink+" | AF2 (MC15c)   | " + DescrFill + " |  | " + XsecFill + " | "+ FieldHigherOrderXsec + " | " + FieldHigherOrderXsecSample + "  | " + AODFillAF2_MC15c + " | " + comment + " | " + lastUpdateAF2_MC15c   + " | \n"


  outputTable   += "<span style=\"font-size: 80%;\">Last update: "+str(timestamp)+" </span> \n"
  outputTable   += "<br> <br>"

  #outputFileName = twikiFolder+"/Twiki_"+SortingTag+"_"+str(DSID)+".twiki"


  #if "410004" in DSID:
  #  print outputFileName
  #  print timestamp

  outputFile = open(outputFileName, "w")
  outputFile.write(outputTable)
  outputFile.close()

  return outputFileName

