# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

'''create files necessary to run the job options, based on an input data file'''
import os, sys, runInfo,re
#create new inputBS file
def inputBs(dataFilename):
  outputFile='input_BS.py' #the file to write will be an input file to the jobOptions
  f=open(outputFile,'w')
  f.write('ServiceMgr.EventSelector.Input = [\n')
  f.write('"'+dataFilename+'"\n]\n')
  f.close()
  
  
#extract run number from data filename
def runNumberStr(dataFilename):
  return dataFilename.split('.')[2]

#based on run number, create new SCTCalibConfig job option
def newCalibConfig(runNumber):
  path="../share/"
  filename="SCTCalibConfig.py"
  fullFilename=path+filename
  source=open(fullFilename)
  target=open(filename, 'w')
  t=source.read()
  source.close()
  #replace the run number
  replacement='RunNumber = '+str(int(runNumber))
  resultTxt=re.sub(r'RunNumber\s*=\s*\d*',replacement, t)
  if resultTxt == t:
    print 'WARNING: No run number update was made to %s' % filename
  else:
    print '%s was written to have run number %s'%(filename, runNumber)
  #replace min stats to be 100
  replacement='NoisyMinStat = 100'
  finalTxt=re.sub(r'NoisyMinStat\s*=\s*\d*', replacement,resultTxt)
  target.write(finalTxt)
  target.close()
  
#based on data filename, create new SCTCalibConfig job option 
def sctCalibConfig(dataFilename):
  runStr=runNumberStr(dataFilename)
  newCalibConfig(runStr)
  
def createRunInfoTxt(dataFilename):
  runStr=runNumberStr(dataFilename)
  runInfo.main(int(runStr))
  
def main(dataFilename):
  createRunInfoTxt(dataFilename)
  inputBs(dataFilename)
  sctCalibConfig(dataFilename)
  try:
    os.mkdir("copytoshare")
  except OSError, e:
    print "Directory 'copytoshare' exists"
  os.renames("runInfo.txt","copytoshare/runInfo.txt")
  os.renames("input_BS.py","copytoshare/input_BS.py")
  os.renames("SCTCalibConfig.py","copytoshare/SCTCalibConfig.py")
  return 0
 
if __name__ == "__main__":
  arguments=sys.argv[1:]
  sys.exit(main(arguments[0]))
