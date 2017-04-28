#! /usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys
import operator
import glob
import re



#class Sample(object):
class Sample():
    Title = ""
    DSID = ""
    lines = []

    # The class "constructor" - It's actually an initializer
    def __init__(self, Title, DSID, lines):
        self.Title = Title
        self.DSID = DSID
        self.lines = lines

def make_sample():
    sample = Student(Title, DSID, lines)
    return sample

def HTMLify(Title):
    s = list(Title)
    n = []
    for c in s:
        if "@" in c:
            c = "&#64;"
        elif "<" in c:
            c = "&lt;"
        elif ">" in c:
            c = "&gt;"
        n.append(c)
    htmlified = "".join(n)

    return htmlified

twikiFolder   = "/afs/cern.ch/user/a/atltopmc/www/TopMC12twiki/TestCentralMC15ProductionPage/"

TwikiList = glob.glob(twikiFolder+"/MainTwiki_Exotics*")
#TwikiList = glob.glob(twikiFolder+"/test.*")


for TwikiFile in TwikiList:
    print "Processing %s" % TwikiFile
    fInFile     = open(TwikiFile, 'r')
    fInputLines = fInFile.readlines()
    fInFile.close()
    if len(fInputLines) < 1:
        continue
    if "Grouped" in fInputLines[0]:
        print "%s has already been sorted into grouped titles" % TwikiFile
        continue
    LineNum = 0
    sampleStart = [] ## line numbers for the first line of each sample
    sampleEnd = [] ## line numbers for the last line of each sample
    for line in fInputLines:
        if "---++++" in line:
            sampleStart.append(LineNum)
        if "<br> <br><br>" in line:
            sampleEnd.append(LineNum)
        LineNum += 1

    sampleNumber = 0
    AllSamples = []
    TopSamples = []
    AllTitles = {} ## a dictionary containing the sample titles and the sample numbers for which they appear. dictionary key:value pairs are "title:samples"
    AllDSIDs = {} ## a dictionary containing the sample titles and the DSIDs for which they appear. dictionary key:value pairs are "title:DSIDs"

    for starts in sampleStart:
        sampleTitle = fInputLines[sampleStart[sampleNumber]]
        reDSID = re.search('\d{6}', fInputLines[sampleStart[sampleNumber]+1])
        sampleDSID = reDSID.group(0)
        sampleLines = []
        for index in range(sampleStart[sampleNumber],sampleEnd[sampleNumber]+1):
            sampleLines.append(fInputLines[index])

        if sampleTitle not in AllTitles:
            matchedSamples = [sampleNumber]
            AllTitles.update({sampleTitle:matchedSamples})
            matchedDSIDs = [sampleDSID]
            AllDSIDs.update({sampleTitle:matchedDSIDs})
            TopSamples.append(sampleNumber)
        else:
            matchedSamples = AllTitles.get(sampleTitle)
            matchedSamples.append(sampleNumber)
            AllTitles.update({sampleTitle:matchedSamples})
            matchedDSIDs = AllDSIDs.get(sampleTitle)
            matchedDSIDs.append(sampleDSID)
            AllDSIDs.update({sampleTitle:matchedDSIDs})

#            SharedTitles.append(sampleNumber)

        thisSample = Sample(sampleTitle,sampleDSID,sampleLines)
        AllSamples.append(thisSample)
        sampleNumber += 1
        # print fInputLines[index]
        # print "\n"
#        sorted_x = sorted(AllTitles.items(), key=operator.itemgetter(1))
#        print sorted_x
#        AllTitles = sorted(AllTitles, key=lambda sample:sample.Title)


    fOutFile     = open(TwikiFile, 'w')
    fOutFile.write("<!-- Grouped -->\n")
    for sample in TopSamples: ## DSID-ordered list of top level samples
        thisBlock = AllTitles[AllSamples[sample].Title] ## all samples with this title
        theseDSIDs = AllDSIDs[AllSamples[sample].Title] ## DSIDs of all samples with this title
        dsidTOC = ["%TWISTY{mode=\"div\"showlink=\"List of DSIDs\"hidelink=\"List of DSIDs\"showimgleft=\"%ICONURLPATH{toggleopen-small}%\"hideimgleft=\"%ICONURLPATH{toggleclose-small}%\"}%\n"]
        dsidCounter = 1
        for dsid in theseDSIDs:
            dsidTOC.append("| [[#DsidAnchor"+dsid+"]["+dsid+"]]  ")
            if dsidCounter%15 == 0:
                dsidTOC.append("|\n")
            if dsidCounter == len(theseDSIDs) and not dsidCounter%15 == 0:
                dsidTOC.append("|\n")
            dsidCounter += 1

        dsidTOC.append("%ENDTWISTY%\n")

        sampleCounter = 0
        for block in thisBlock: ## loop over all DSIDs sharing a title
            blockSample = AllSamples[block]
            blockLines = blockSample.lines
            blockLines[0] = HTMLify(blockLines[0])
            blockLines[0] = blockLines[0].rstrip() ## remove whitespace from end of title
            subTitle = blockLines[0]
            subTitle = subTitle[8:]
            if len(thisBlock) > 1:
                if sampleCounter == 0: ## first DSID with this title
                    nDSIDs = str(len(theseDSIDs))
                    blockLines[0] += " ("+nDSIDs+" samples)\n"
                    blockLines[0] = blockLines[0]+(''.join(dsidTOC))+"#DsidAnchor"+blockSample.DSID+"\n---+++++!! "+ blockSample.DSID+" &ndash; "+ subTitle +"\n"
                else:

                    blockLines[0] = "#DsidAnchor"+blockSample.DSID+"\n---+++++!! "+ blockSample.DSID+" &ndash; "+ subTitle +"\n"
            else:
                blockLines[0] += "\n"
            fOutFile.writelines(blockLines)
            sampleCounter += 1

    fOutFile.close()

