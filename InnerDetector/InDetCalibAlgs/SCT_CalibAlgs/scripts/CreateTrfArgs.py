# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import pickle, os.path, sys, os
def fileExists(fp):
  #caveats : http://stackoverflow.com/questions/82831/how-do-i-check-if-a-file-exists-using-python
  return os.path.isfile(fp)
  
def getPickle(fp):
  f=open(fp,'r')
  d=pickle.load(f)
  return d
  
def correctPath(myPickle):
  thisPath=os.getcwd()
  #find the variable which gives the SCTCalibConfig path
  SCTCalibConfigFile=myPickle['SCTCalibConfig']
  fileName='SCTCalibConfig.py'
  dissectedPath=thisPath.split('/')
  dissectedPath[-1]='share'
  sharePath='/'.join(dissectedPath)
  newFileName=sharePath+'/'+fileName
  myPickle['SCTcalibConfig']=newFileName
  
def writeNewPickle(newPickleName, data):
  pickleFile=open(newPickleName,'w')
  pickle.dump(data, pickleFile)
  
def swapFiles(originalFile,newFile):
  os.remove(originalFile)
  os.rename(newFile,originalFile)
  


def main():
  existingPickleFilename='./newargdict.gpickle'
  newPickleFileName='mynewpickle.gpickle'
  data=getPickle(existingPickleFilename)
  correctPath(data)
  writeNewPickle(newPickleFileName,data)
  swapFiles(existingPickleFilename,newPickleFileName)
  

if __name__=="__main__":
  sys.exit(main())