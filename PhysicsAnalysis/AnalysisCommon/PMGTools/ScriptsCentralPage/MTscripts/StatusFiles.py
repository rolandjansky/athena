#! /usr/bin/python

import os
import sys
import datetime
import SimpleFunctions
import logging

import pyAMI
import pyAMI.client
import pyAMI.atlas.api

log = logging.getLogger("myCentralPageLogger")

localJoPath = "/afs/cern.ch/user/f/fmeloni/workarea/public/CentralPage/MC15JobOptions/share/"
joLinkPath = "https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/share/"

localJoPath_12 = "/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/CentralStatusPageMC15/MC12JobOptions/share/"
joLinkPath_12 = "https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC12JobOptions/trunk/share/"


def GetLocalJoFileName(DSID):

    joPath = "DoesNotExist"
    joLink = "DoesNotExist"
    firstThreeNum = DSID[0:3]
    jOFolder = localJoPath+"DSID"+firstThreeNum+"xxx/"

    if not os.path.exists(jOFolder):
        jOFolder = localJoPath_12+"DSID"+firstThreeNum+"xxx/"
        if not os.path.exists(jOFolder):
            return [joPath,joLink]

    #print "GetLocalJoFileName:  ",firstThreeNum,"  ",jOFolder
    fileList      = os.listdir(jOFolder)
    for file in fileList:
        if "MC12."+str(DSID) in file:
            joLink = joLinkPath_12+"DSID"+firstThreeNum+"xxx/"+file
            joPath = localJoPath_12+"DSID"+firstThreeNum+"xxx/"+file
        elif "MC15."+str(DSID) in file:
            joLink = joLinkPath+"DSID"+firstThreeNum+"xxx/"+file
            joPath = localJoPath+"DSID"+firstThreeNum+"xxx/"+file  

    return [joPath,joLink]

def GetInfoFromJO(joPath):
    if joPath == "DoesNotExist":
        return ["", ""]
    description = ""
    keywords    = ""

    fInFile  = open(joPath)
    fInLines = fInFile.readlines()
    for line in fInLines:
        currentLine = line.replace("\n", "")
        if "description" in line:
            description = line[line.find("="):]
        if "keywords" in line:
            keywords = line[line.find("="):]
    return [description, keywords]
 
def GetFileEntry(EntryList, EntryType):
    Status = ""
    for Entry in EntryList:
        if Entry.startswith(EntryType+"."):
            Status = Entry.replace(EntryType+".", "")
            Status = Status.replace("\n", "")
            return Status
    if Status == "":
        print "ERROR(GetFileEntry): EntryType="+EntryType+" is not defined!!! ---> EXIT."
        Status = "Entry not defined!!!"
        return Status
        #sys.exit()


def CalculateKfactor(AmiXsec, FilterEff, BR, HigherOrderXsec):
    Kfactor = 1.0
    # print AmiXsec,"   ",FilterEff,"   ",BR,"   ",HigherOrderXsec
    if FilterEff == "Undefined" or "Error" in str(FilterEff) or "zero" in str(FilterEff):
        return "Undefined"
    if BR == "Undefined" or "Error" in str(BR):
        return "Undefined"
    if HigherOrderXsec == "Undefined" or "Error" in str(HigherOrderXsec):
        return "Undefined"
    if AmiXsec == "Undefined" or "Error" in str(AmiXsec):
        return "Undefined"

    # Kfactor = float(HigherOrderXsec)*float(BR)/float(FilterEff)/float(AmiXsec)

    Kfactor = float(HigherOrderXsec)/float(AmiXsec)

    return Kfactor


class StatusFileCreator(object):

    def __init__(self, StatusFile, StatusFolder, Dataset, Comment, SortingTag, Xsec, Kfactor, FilterEff, BR, higherOrderXsecTag, Title, DSID):

        log.debug('StatusFileCreator %s %s %s', StatusFile, StatusFolder, Dataset)

        self.__DSID            = DSID
        self.__Dataset         = Dataset
        self.__StatusFile      = StatusFile
        self.__StatusFolder    = StatusFolder
        self.__SortingTag      = SortingTag
        #self.__higherOrderXsec = higherOrderXsec
        #self.__Title            = Title

        StatusFileName = self.__StatusFolder+"/"+self.__StatusFile

        #CheckUpdates = True

        #if os.path.exists(StatusFileName) and CheckUpdates:

         #   ## first check if the ami container has been updated! if not, keep the old file                                                                                                                 

         #   AllInfo         = StatusFileReader(StatusFileName)
         #   oldTimeStamp    = AllInfo.lastModified
         #   date_format     = "%Y-%m-%d %H:%M:%S"                                                                                                                                    
         #   OldTitle        = AllInfo.Title

         #   client          = pyAMI.client.Client('atlas')
         #   pyAMI.atlas.api.init()
         #   AMIInfoArray    = pyAMI.atlas.api.get_dataset_info(client, Dataset)
         #   AMIInfo         = AMIInfoArray[0]
         #   newTimeStamp    = AMIInfo["task_lastModified_0"]
         #   today           = datetime.datetime.now()
         #   oldTimeStampFormat = datetime.datetime.strptime(oldTimeStamp, date_format)
         #   newTimeStampFormat = datetime.datetime.strptime(newTimeStamp, date_format)
         #   delta              = newTimeStampFormat - oldTimeStampFormat
         #   difference         = delta.days
         #   #print oldTimeStampFormat,"   ",newTimeStampFormat,"  ",difference
         #   if difference != 0:
         #       os.system("rm "+StatusFileName)
         #   if OldTitle   != Title:
         #       os.system("rm "+StatusFileName)
         #   if AllInfo.comment != Comment:
         #       os.system("rm "+StatusFileName)
 
        #if not os.path.exists(StatusFileName) and not ".DAOD_" in StatusFileName:
        if not os.path.exists(StatusFileName):

            #if "187" in StatusFileName:
            #    print "make status file for "+StatusFileName

            client       = pyAMI.client.Client('atlas')
            pyAMI.atlas.api.init()
            #print Dataset
            AMIInfoArray = ""
            try:
                #print "Get dataset info..."
                AMIInfoArray = pyAMI.atlas.api.get_dataset_info(client, Dataset)
            except:
                #print "Could not get the info for ",Dataset
                return
            AMIInfo      = AMIInfoArray[0]

            #if("303193" in str(DSID)):
            #    print AMIInfo
            #    #sys.exit()

            StatusOK = False

            if AMIInfo["prodsysStatus"] == "ALL EVENTS AVAILABLE":
                StatusOK = True

            ## at the moment: write info only out for containers which are complete, properly closed and have all necessary info!!!

            #if not StatusOK:
                #print AMIInfo
            #    return

            HelpFilterEff  = "Undefined"
            HelpBR         = "Undefined"
            HelpAMIXsec    = "Undefined"
            HelpHigherXsec = "Undefined"

            joPaths   = GetLocalJoFileName(DSID)
            timestamp = datetime.datetime.now()

            StatusFile = open(StatusFileName, 'w')
            StatusFile.write("DSID."+self.__DSID+"\n")
            StatusFile.write("joLink."+joPaths[1]+"\n")
            if not "mc12_13TeV." in Dataset:
                StatusFile.write("description."+AMIInfo["physicsComment"]+"\n")
            else:
                StatusFile.write("description."+Dataset+"\n")
            StatusFile.write("keywords."+AMIInfo["keyword"]+"\n")
            StatusFile.write("StatusFileName."+self.__StatusFile+"\n")
            StatusFile.write("StatusFileTimeStamp."+str(timestamp)+"\n")
            StatusFile.write("SortingTag."+SortingTag+"\n")
            StatusFile.write("dataset."+AMIInfo["logicalDatasetName"]+"\n")
            StatusFile.write("dataType."+AMIInfo["dataType"]+"\n")
            StatusFile.write("totalEvents."+AMIInfo["totalEvents"]+"\n")
            if not "mc12_13TeV." in Dataset:
                #print Dataset
                #StatusFile.write("cmEnergy."+AMIInfo["ECMEnergy"]+"\n")
                StatusFile.write("cmEnergy."+AMIInfo["ecmEnergy"]+"\n")
            else:
                StatusFile.write("cmEnergy.13000\n")
            StatusFile.write("PDF."+AMIInfo["PDF"]+"\n")
            StatusFile.write("tag."+AMIInfo["version"]+"\n")
            StatusFile.write("genName."+AMIInfo["generatorName"]+"\n")
            StatusFile.write("genTune."+AMIInfo["generatorTune"]+"\n")
            if Xsec == "":
                if AMIInfo["dataType"] == "EVNT":
                    if not "mc12_13TeV." in Dataset:
                            
                        if AMIInfo["crossSection"]  != "NULL":
                            HelpAMIXsec   = AMIInfo["crossSection_mean"]

                            if HelpAMIXsec.isdigit() or "E-" in HelpAMIXsec or "E+" in HelpAMIXsec:
                                HelpAMIXsec = 1000.0*float(HelpAMIXsec)
                            else:
                                HelpAMIXsec = "Undefined"

                            HelpFilterEff = AMIInfo["GenFiltEff_mean"]
                            if HelpFilterEff.isdigit() or "E-" in HelpFilterEff or "E+" in HelpFilterEff:
                                HelpFilterEff = float(HelpFilterEff)
                            else:
                                HelpFilterEff = "Undefined"
                    else:
                        HelpAMIXsec   = AMIInfo["crossSection_mean"]
                        HelpFilterEff = AMIInfo["GenFiltEff_mean"]

                        if HelpAMIXsec.isdigit() or "E-" in HelpAMIXsec or "E+" in HelpAMIXsec:
                            HelpAMIXsec = 1000.0*float(HelpAMIXsec)

                        if HelpFilterEff.isdigit() or "E-" in HelpFilterEff or "E+" in HelpFilterEff:
                            HelpFilterEff = float(HelpFilterEff)

                else:
                    if AMIInfo["crossSection"]  != "NULL":
                        try:
                            HelpAMIXsec   = AMIInfo["approx_crossSection"]
                        except:
                            HelpAMIXsec = "Undefined"

                        if HelpAMIXsec.isdigit() or "E-" in HelpAMIXsec or "E+" in HelpAMIXsec:
                            HelpAMIXsec = 1000.0*float(HelpAMIXsec)
                        else:
                            HelpAMIXsec = "Undefined"

                        try:
                            HelpFilterEff = AMIInfo["approx_GenFiltEff"]
                        except:
                            HelpFilterEff = "Undefined"

                    try:
                        HelpFilterEff = AMIInfo["GenFiltEff_mean"]
                    except:
                        HelpFilterEff = "Undefined"


                    if HelpFilterEff.isdigit() or "E-" in HelpFilterEff or "E+" in HelpFilterEff:
                        HelpFilterEff = float(HelpFilterEff)


            else:
                float_val = float(Xsec)

                #if("341471" in str(DSID)):
                #    print float_val
                    #print float_val.isdigit()
                    #print HelpAMIXsec
                    
                #if float_val.isdigit():
                HelpAMIXsec = float_val

            if FilterEff != "":
                HelpFilterEff = FilterEff
            else:
                if HelpFilterEff == "Undefined":
                    try: 
                        HelpFilterEff = AMIInfo["GenFiltEff_mean"]
                    except: 
                        HelpFilterEff = "1.0"
                    FilterEff = HelpFilterEff
                else:
                    FilterEff = HelpFilterEff
                


            ########################################################
            if FilterEff == "":
                StatusFile.write("filterEff."+str(HelpFilterEff)+"\n")
            else:
                HelpFilterEff = FilterEff
                StatusFile.write("filterEff."+str(FilterEff)+"\n")    
            ########################################################     
            if BR == "":
                HelpBR = 1.0
                StatusFile.write("BR.1.0 \n")
            else:
                StatusFile.write("BR."+BR+" \n")
                HelpBR = BR
            ######################################################## 
            
            if HelpAMIXsec == "Undefined" and Xsec != "":
                StatusFile.write("Xsec."+str(Xsec)+" \n")
            else:
                StatusFile.write("Xsec."+str(HelpAMIXsec)+" \n")

            #print "==================================================================================="
            #print higherOrderXsecTag

            if higherOrderXsecTag != "":
                Info = SimpleFunctions.GetHigherOrderXsecInfo(higherOrderXsecTag)
                #print Info
                if not higherOrderXsecTag.isdigit():
                    HelpHigherXsec = Info[0]
                    StatusFile.write("XsecLink."+Info[1]+"\n")
                else:
                    HelpHigherXsec = higherOrderXsecTag
                    StatusFile.write("XsecLink.\n")
            else:
                StatusFile.write("XsecLink.\n")

            StatusFile.write("higherOrderXsec."+HelpHigherXsec+" \n")

            Title = Title.replace("\n", "")

            if Title == "":
                comment = Dataset
                try:
                    comment = AMIInfo["physicsComment"]
                    if comment == "NULL":
                        try:
                            comment = AMIInfo["physicsShort"]
                        except:
                            comment = Dataset
                except:
                    comment = Dataset
 
                StatusFile.write("Title."+comment+"\n")
            else:
                #print "Title is not empty! use Title=",Title,"  ",len(Title)
                StatusFile.write("Title."+Title+"\n")

            StatusFile.write("comment."+Comment+"\n")
            StatusFile.write("lastModified."+AMIInfo["task_lastModified_0"]+"\n")

            #print DSID
            #print DSID,"   ",HelpAMIXsec,"    ",HelpFilterEff,"    ",HelpBR,"    ",HelpHigherXsec

            kfac = ""
            if Kfactor == "":
                #print DSID,"   ",Dataset
                kfac = CalculateKfactor(HelpAMIXsec, HelpFilterEff, HelpBR, HelpHigherXsec)
            else:
                kfac = Kfactor

            #if "41000" in DSID:
            #    print DSID,"   ",HelpAMIXsec,"    ",HelpFilterEff,"    ",HelpBR,"    ",HelpHigherXsec,"  ",kfac

            if str(kfac) == "Undefined":
                kfac = "1.0"

            try:
                StatusFile.write("kFactor."+str(kfac)+" \n")
            except:
                StatusFile.write("kFactor.Undefined\n")
                

            try:
                StatusFile.write("higherOrderXsecSample."+str(float(HelpAMIXsec)*float(kfac)*float(FilterEff)*float(HelpBR))+" \n")
            except:
                StatusFile.write("higherOrderXsecSample.Undefined\n")

            #if not "Undefined" in str(HelpAMIXsec) and not "Undefined" in str(FilterEff):
            #    if not str(kfac) == "" and not str(FilterEff) == "":
            #        StatusFile.write("kFactor."+str(kfac)+" \n")
            #        StatusFile.write("higherOrderXsecSample."+str(float(HelpAMIXsec)*float(kfac)*float(FilterEff))+" \n")#

            #    else:
            #        StatusFile.write("kFactor.Undefined\n")
            #        StatusFile.write("higherOrderXsecSample.Undefined\n")
                    
            #else:
            #    StatusFile.write("kFactor.Undefined\n")
            #    StatusFile.write("higherOrderXsecSample.Undefined\n")

            date_format     = "%Y-%m-%d %H:%M:%S"
            oldTimeStamp    = AMIInfo["task_lastModified_0"]
            today           = datetime.datetime.now()
            oldTimeStampFormat = datetime.datetime.strptime(oldTimeStamp, date_format)
            delta              = today - oldTimeStampFormat
            difference         = delta.days
            if difference < 7:
                StatusFile.write("updatedStatus.True\n")
            else:
                StatusFile.write("updatedStatus.False\n")

            StatusFile.write(" \n")
            StatusFile.close()


class StatusFileReader(object):
    def __init__(self, StatusFile):
        self.__StatusFile   = StatusFile

        fInFile     = open(self.__StatusFile, 'r')
        fInputLines = fInFile.readlines()
        fInFile.close()
        #print StatusFile
        self.AllLines            = fInputLines
        self.DSID                = GetFileEntry(self.AllLines, "DSID")
        self.joLink              = GetFileEntry(self.AllLines, "joLink")
        self.description         = GetFileEntry(self.AllLines, "description")
        self.keywords            = GetFileEntry(self.AllLines, "keywords")
        self.StatusFileName      = GetFileEntry(self.AllLines, "StatusFileName")
        self.StatusFileTimeStamp = GetFileEntry(self.AllLines, "StatusFileTimeStamp")
        self.SortingTag          = GetFileEntry(self.AllLines, "SortingTag")
        self.dataset             = GetFileEntry(self.AllLines, "dataset")
        self.dataType            = GetFileEntry(self.AllLines, "dataType")
        self.totalEvents         = GetFileEntry(self.AllLines, "totalEvents")
        self.cmEnergy            = GetFileEntry(self.AllLines, "cmEnergy")
        self.PDF                 = GetFileEntry(self.AllLines, "PDF")
        #print "StatusFileReader: get PDF"
        #print self.PDF
        self.tag                 = GetFileEntry(self.AllLines, "tag")
        self.genName             = GetFileEntry(self.AllLines, "genName")
        self.genTune             = GetFileEntry(self.AllLines, "genTune")
        self.Xsec                = GetFileEntry(self.AllLines, "Xsec")
        self.filterEff           = GetFileEntry(self.AllLines, "filterEff")
        self.kFactor             = GetFileEntry(self.AllLines, "kFactor")
        self.higherOrderXsec     = GetFileEntry(self.AllLines, "higherOrderXsec")
        self.higherOrderXsecSample = GetFileEntry(self.AllLines, "higherOrderXsecSample")
        self.BR                  = GetFileEntry(self.AllLines, "BR")
        self.XsecLink            = GetFileEntry(self.AllLines, "XsecLink")
        self.comment             = GetFileEntry(self.AllLines, "comment")
        self.Title               = GetFileEntry(self.AllLines, "Title")
        self.lastModified        = GetFileEntry(self.AllLines, "lastModified")
        self.updatedStatus       = GetFileEntry(self.AllLines, "updatedStatus")
        
