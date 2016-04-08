#!/afs/cern.ch/sw/lcg/external/Python/2.5.4/slc4_ia32_gcc34/bin/python
# =====================================================================
# Iterates in two steps
# 1) L11 (RunIterator_L11) with no LB splitting 
# 2) L16 (RunIterator_L16) with the possibility of LB splitting
# =====================================================================
import os
import sys

debug = True
##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--inputList", dest="inputList", help="Input txt File with the list of the files you want to run on", default="")
    parser.add_option("--eventType", dest="eventType", help="Which type of events: collisions / cosmics", default="collisions")
    parser.add_option("--isData",dest="isData", help="Set itif you want to run on real data (Default: False)",action="store_true", default=False)
    parser.add_option("--inputBowingDb", dest="inputBowingDb", help="In the case you want to use a specific IBL bowing  write here the absolute path to the cool file", default="")
    parser.add_option("--inputConstantsFile", dest="inputConstantsFile", help="In the case you want to use a specific set of initial constants write here the absolute path to the pool file", default="")
    parser.add_option("--nEvents", dest="nEvents", help="Maximum number of events to use", default="")
    parser.add_option("--nCpus", dest="nCpus", help="Number of CPUs to be used",default="")
    parser.add_option("--errorScalingTag",dest="errorScalingTag", help="Name of the error scaling tag or ES file to use",default="")
    parser.add_option("--ptmin",dest="userPtMin", help="Minimum pt of tracks to enter the align track selection *** in MeV ***", default = "")
    parser.add_option("--FirstLB",dest="inputFirstLB", help = " First LumiBlock to be considered ", default = 1)
    parser.add_option("--LastLB",dest="inputLastLB", help = " Last LumiBlock to be considered (default all)", default = -1 )
    parser.add_option("--NLB",dest = "inputNLB", help = " Number of lumiblocks per job ", default = 10)
    parser.add_option("--nIter",dest="inputnIter", help="Number of iterations to be executed", default = "1")
    parser.add_option("--queue",dest="inputQueue", help="name of the LSF queue were jobs will be submitted", default="")
    parser.add_option("--suffix",dest = "inputSuffix", help = " Suffix for the jobs", default ="")
    parser.add_option("--useLBselector", dest = "inputUseLBselector", help = "If True use the athena LB selector. If false, file list is split by LB.",action="store_true", default=False) 
    
    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    return config
    

##########################################################
#               Option Initialisation                    #
##########################################################
config = optParsing()
userFile=config.inputList
eventType=config.eventType
userConstantsFile=config.inputConstantsFile
usernEvents=str(config.nEvents)
isData=config.isData
userErrorScalingTag=config.errorScalingTag
usernCpus= str(config.nCpus)  
userPtMin = str(config.userPtMin)
userFirstLB = int(config.inputFirstLB)
userLastLB = int(config.inputLastLB)
userNLB = int(config.inputNLB)
userBowingDB = str(config.inputBowingDb)
userSuffix = str(config.inputSuffix)
userUseLBselector = config.inputUseLBselector
usernIter = str(config.inputnIter)
userQueue = str(config.inputQueue)

multiplicativeFactor = 1

if (userUseLBselector):
    multiplicativeFactor = 3



##########################################################           
#  copy the two RunIterators                             #
##########################################################           
os.system("get_files -jo InDetAlignExample/RunIterator_L11.py > /dev/null")
print os.system("get_files -jo InDetAlignExample/RunIterator_L11.py")

os.system("get_files -jo InDetAlignExample/RunIterator_L16.py > /dev/null")
print os.system("get_files -jo InDetAlignExample/RunIterator_L16.py")



##########################################################
#  split the input file according to the lumiblocks      #
##########################################################
initialLB = userFirstLB 
finalLB = userFirstLB - 1# intialization
upperLB = finalLB+1 # maximum value, a value that allows to enter the loop
if (userLastLB > 0): upperLB = userLastLB 
ListOfDaughterFiles = []
ListOfNFiles = []
ListOfLBranges = []

with open(userFile, 'rb') as inFile:
    while (finalLB < upperLB and finalLB < userLastLB):
        initialLB = finalLB + 1 # go for next LB
        finalLB = initialLB + userNLB - 1 #to correct for the first one. This way one may do from 1 to 10 and so on 
        if (finalLB > userLastLB): finalLB = userLastLB
        # -- open output file
        outputFileName = "SelectedLB_" + str(initialLB) + "_" + str(finalLB) + ".txt"
        if ( userNLB == 1): outputFileName = "SelectedLB_" + str(initialLB) +  ".txt"
        outFile = open(outputFileName, 'w')
        writtenLines = 0
        
        # -- write list of input files into the output files 
        for line in inFile:
            whereLB = line.find("_lb") 
            thisLB = int(line[whereLB+3:whereLB+7])
            #print whereLB, thisLB
            if ((initialLB <= thisLB and thisLB <=finalLB) or userUseLBselector):
                outFile.write(line)
                writtenLines += 1
            if (thisLB > upperLB): upperLB = thisLB
        if (writtenLines>0): print " <LumiBlockIterator> ouput file ", outputFileName, " has ", writtenLines, " lines"
        if (writtenLines > 0): 
            ListOfDaughterFiles.append(outputFileName)
            ListOfNFiles.append(writtenLines)
        else: #delete empty files
            os.remove(outputFileName)
        inFile.seek(0,0) # rewind

        # store the list of LB ranges
        ListOfLBranges.append(initialLB)
        ListOfLBranges.append(finalLB)
        
    print " <LumiBlockIterator> Lumiblock file splitting completed. In total ", len(ListOfDaughterFiles) ," daughter files with active LumiBlocks have been created"
    
#############################################################
# Execute first the L11 (no LB splitting)                   #
#############################################################
# deal with user options
execCommandOptions = ""
execCommandOptions1 = ""
execCommandOptions2 = "" # input bowing file
execCommandOptions3 = "" # for LB selections

outcommandFile = open("CommandsFile.txt","w")


if (len(userConstantsFile)>0): execCommandOptions1 = execCommandOptions1 + " --inputConstantsFile " + userConstantsFile
if (len(userErrorScalingTag)>0): execCommandOptions1 = execCommandOptions1 + " --errorScalingTag " + userErrorScalingTag
if (len(usernEvents)>0):execCommandOptions1 = execCommandOptions1 + " --nEvents " + usernEvents 
execCommandOptions1 = execCommandOptions1 + " --isData True" 
if (len(userPtMin)>0): execCommandOptions1 = execCommandOptions1 + " --ptmin " + userPtMin
if (debug): print " <LumiBlockIterator> execCommandOptions1 = ", execCommandOptions1
if (len(userBowingDB)>0):  execCommandOptions2 = execCommandOptions2 + " --inputBowingDb "+userBowingDB 

execCommandOptions0 = " --inputList " + userFile
theTail = userSuffix + "_L11" 
execCommandOptions0 = execCommandOptions0 + " --suffix " + theTail

thisnCpus = usernCpus
if (len(usernCpus)>0): execCommandOptions0 = execCommandOptions0 + " --nCpus " +  str(thisnCpus)
if (len(usernIter)>0): execCommandOptions0 = execCommandOptions0 + " --nIter " +  str(usernIter)
if (len(userQueue)>0): execCommandOptions0 = execCommandOptions0 + " --queue " +  str(userQueue)
    

execCommandOptions = execCommandOptions0 + execCommandOptions1 + execCommandOptions2 + execCommandOptions3

if (debug): print " <LumiBlockIterator> execCommandOptions = ", execCommandOptions 

outcommandFile.write("python RunIterator_L11.py "+execCommandOptions+"\n\n\n\n")

os.system("python RunIterator_L11.py "+execCommandOptions)

lastIterID = int(usernIter)-1
userConstantsFile = os.getcwd() + "/Iter" + str(lastIterID) + "_" + theTail + "/Iter"+ str(lastIterID) +"_AlignmentConstants.root"
userBowingDB = os.getcwd() + "/Iter" + str(lastIterID) + "_" + theTail + "/mycool.db"
print " -- L11 constants file: ", userConstantsFile
print " -- "
#exit()

#############################################################
# Loop over the daughter files and submit the jobs  for L16 #
#############################################################
subJob=0
if len(ListOfDaughterFiles) > 0:
    thisfile = 0

    # deal with user options
    execCommandOptions = ""
    execCommandOptions1 = ""
    execCommandOptions2 = "" # input bowing file
    execCommandOptions3 = "" # for LB selections

    # as input constants file one should use the L11 results
    if (len(userConstantsFile)>0): execCommandOptions1 = execCommandOptions1 + " --inputConstantsFile " + userConstantsFile
    if (len(userErrorScalingTag)>0): execCommandOptions1 = execCommandOptions1 + " --errorScalingTag " + userErrorScalingTag
    if (len(usernEvents)>0):execCommandOptions1 = execCommandOptions1 + " --nEvents " + usernEvents 
    execCommandOptions1 = execCommandOptions1 + " --isData True" 
    if (len(userPtMin)>0): execCommandOptions1 = execCommandOptions1 + " --ptmin " + userPtMin
    if (debug): print " <LumiBlockIterator> execCommandOptions1 = ", execCommandOptions1
    if (len(userBowingDB)>0):  execCommandOptions2 = execCommandOptions2 + " --inputBowingDb "+userBowingDB     
        
    for daughterFile in ListOfDaughterFiles:
        print "\n  <LumiBlockIterator> going to process lumiblocks in file ",  daughterFile
        whereLB = daughterFile.find("LB_")
        whereDotTxt = daughterFile.find(".txt")
        theTail = daughterFile[whereLB:whereDotTxt]
        if (len(userSuffix)>0): theTail = userSuffix + "_L16_" + daughterFile[whereLB:whereDotTxt]
        
        #if (debug): print " <LumiBlockIterator> theTail = ", theTail

        execCommandOptions0 = " --inputList " + daughterFile
        execCommandOptions0 = execCommandOptions0 + " --suffix " + theTail

        thisnCpus = usernCpus
        #if (ListOfNFiles[subJob] < usernCpus): thisnCpus = ListOfNFiles[subJob] 
        if (len(usernCpus)>0): execCommandOptions0 = execCommandOptions0 + " --nCpus " +  str( int(thisnCpus) * multiplicativeFactor)
        if (len(usernIter)>0): execCommandOptions0 = execCommandOptions0 + " --nIter " +  str(usernIter)
        if (len(userQueue)>0): execCommandOptions0 = execCommandOptions0 + " --queue " +  str(userQueue)


        # LB selection    
        execCommandOptions3 = "" # reset
        firtLB = ListOfLBranges[thisfile*2]
        lastLB = ListOfLBranges[thisfile*2+1]
        if (userUseLBselector): execCommandOptions3 =  " --LBrangeFirst " + str(firtLB) + " --LBrangeLast " + str(lastLB)
        
        # build the command options for RunIterator
        execCommandOptions = execCommandOptions0 + execCommandOptions1 + execCommandOptions2 + execCommandOptions3

        if (debug): print " <LumiBlockIterator> execCommandOptions = ", execCommandOptions 
        outcommandFile.write("python RunIterator_L16.py "+execCommandOptions+" &"+"\n\n\n\n\n\n")
        os.system("python RunIterator_L16.py "+execCommandOptions+" &")

        subJob += 1
        thisfile += 1

        os.system("sleep 2")
outcommandFile.close()
exit()    
