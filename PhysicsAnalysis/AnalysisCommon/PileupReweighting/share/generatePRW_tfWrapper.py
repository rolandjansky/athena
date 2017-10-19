#this is just a simple wrapper for the joboptions
#to connect up the runArgs input and outputs

jps.AthenaCommonFlags.FilesInput = runArgs.inputAODFile
outputPRWFile = runArgs.outputPRWFile

include("PileupReweighting/generatePRW_jobOptions.py")

