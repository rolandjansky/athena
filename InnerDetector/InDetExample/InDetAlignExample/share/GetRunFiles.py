##--------------------------
# usage:
#
# GetRunFiles.py RunNumber
#
##--------------------------

import sys, os, fileinput

##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("--DaqMerge", dest="inputDaqMerge", help="folder type name where the files stand: either daq or merge", default="merge") 
	parser.add_option("--Label", dest="inputLabel", help="suffix label for the output file", default="") 
	parser.add_option("--Project", dest="inputProject", help="define the data project. Necessary to search for the files (default data15_13TeV)", default= "data15_13TeV")
	parser.add_option("--Run", dest="inputRun", help="run number", default="")
	parser.add_option("--Stream", dest="inputStream", help="wanted data stream (default calibration_IDTracks)", default= "calibration_IDTracks")
	
	(config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

	return config
	
##########################################################
#               Option Initialisation                    #
##########################################################
config = optParsing()
inputRun = config.inputRun
FileLabel = config.inputLabel
DataProject = config.inputProject
DataStream = config.inputStream
daqmerge = config.inputDaqMerge

#in case only one argument is given, it is taken as the run number
if (len(inputRun) == 0): 
    RunNumber =  sys.argv[1]
else:
    RunNumber = inputRun


##########################################################
#               other variables
##########################################################
#if ("IDTracks" in DataStream): daqmerge = "daq"

#######################
tempOutputFile = "Run_"+RunNumber+"_temp.txt"

OutputFile = "Run_" + RunNumber + ".txt"
if (len(FileLabel)>1): 
    OutputFile = "Run_" + RunNumber + "_" + FileLabel + ".txt"

print " <GetRunFiles> RunNumber    = ", RunNumber
print " <GetRunFiles> Project      = ", DataProject
print " <GetRunFiles> Stream       = ", DataStream
print " <GetRunFiles> DaqMerge     = ", daqmerge
print " <GetRunFiles> FileLabel    = ", FileLabel
print " <GetRunFiles> OutputFile   = ", OutputFile
 
#os.system("ls -l ~/eos/atlas/atlastier0/rucio/%s/%s/00%s/data15_comm.00%s.%s.merge.RAW" %(DataProject, DataStream,  RunNumber, RunNumber,  DataStream))
os.system("ls -l  ~/eos/atlas/atlastier0/rucio/%s/%s/00%s/%s.00%s.%s.%s.RAW | gawk \'{print \"root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/%s/%s/00%s/%s.00%s.%s.%s.RAW/\"$9}\' > %s"  %(DataProject, DataStream,  RunNumber, DataProject, RunNumber,  DataStream, daqmerge, DataProject, DataStream, RunNumber, DataProject, RunNumber, DataStream, daqmerge, tempOutputFile)) 

outputf = open(OutputFile, 'w')
count=0
with open (tempOutputFile,'rb') as f:
    for line in f:
        count+=1
        if (count > 1):
            #print " line: ", count, "  = ",line
            outputf.write(line)
        
outputf.close()

os.system("rm -f %s" %tempOutputFile)
 
print " File --> ", OutputFile, " has ", count-1, " lines"
if (count == 0):
    print " <GetRunFiles> WARNING: when selecting files make sure what type of folder contains the files: either daq or merge suffix"

    
print " <GetRunFiles> GAME OVER. INSERT COIN. "
