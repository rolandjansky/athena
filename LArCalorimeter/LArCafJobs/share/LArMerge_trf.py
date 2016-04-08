#!/usr/bin/env python

#########################################################################
##
## Tier-0 TAG file merging transformation
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##     1) 'inputTagFiles': python list ['datasetname#filename1','datasetname#filename2',...]
##        (input dataset + file names)
##     2) 'outputTagFile': string 'datasetname#filename'
##        (output dataset name + file)
##
##  - generates merged TAG output file
##
## (C) Jack Cranshaw, Luc Goossens, David Malon, Armin Nairz, Nicolas Berger
##     (July 2008, April 2009, July 2010, May 2011)
##
#########################################################################

import sys, string, commands, os, pickle, time, pprint, subprocess

#########################################################################

# Utility function

def getFileMap(fname, dsname, nevts=0, guid='') :
  if os.path.isfile(fname) :
    dataMap = { 'lfn': fname,
                'dataset' : dsname,
                'events' : nevts
          }
    if guid : 
      dataMap['GUID'] = guid      
  else : 
    dataMap = {}
  return dataMap

#########################################################################


def larMerge(dataMap) :

  tstart = time.time()

  print "\n##################################################################"
  print   "##                 ATLAS Tier0 LAr CAF file Merging             ##"
  print   "##################################################################\n"

  print "\nFull Tier-0 run options:\n"
  pprint.pprint(dataMap)
  
  inputList = []
  inputDSName = None

  for val in dataMap['inputLArFiles'] :
    inputList.append(val.split('#')[1]) 
    if not inputDSName : dsName = val.split('#')[0]
    
  print "\ninputLArFiles list:\n"
  pprint.pprint(inputList)

  # output file
  outputDSName = dataMap['outputLArFile'].split('#')[0]
  outputFile = dataMap['outputLArFile'].split('#')[1]

  print '\nOutput file name:', outputFile

  retcode = 0
  acronym = 'OK'
  txt = 'trf finished OK'

  outFiles = []
  inFiles = []
  nEvents=0

  cmdline=["LArSamplesMerge.exe"]
  for fileName in inputList:
    if os.access(fileName,os.R_OK):
      cmdline.append(fileName)
    else:
      print 'ERROR : could not open file', fileName
      retcode = 74001
      acronym = 'TRF_LAR_FILE_INPUT_ERROR'
      txt = 'LAr input file not accessible'
      break
    pass
  
  if (retcode==0):
    writepath=os.path.split(outputFile)[0]
    if writepath=="": writepath="./"
    if (os.access(writepath,os.W_OK)):
      cmdline.append(outputFile)
    else:
      print "ERROR, not allowed to write to oututfile", outputFile
      retcode = 74002
      acronym = 'TRF_LAR_MERGE_ERROR'
      txt = 'LAR merging error'
      pass
    pass
  
  if (retcode==0):

    logfile=open("log.LArMerge","w")

    try:
      subprocess.check_call(cmdline,stdout=logfile,stderr=subprocess.STDOUT)
    except Exception,e:
      print "ERROR exectution of subprocess failed"
      print e
      acronym = 'TRF_LAR_MERGE_ERROR'
      txt = 'LAR merging error'
      pass

    logfile.close()
    logfile=open("log.LArMerge","r")

    for line in logfile:
      print line,
      if line.startswith("Open file "):
        linetok=line.split(" ")
        inFiles.append(getFileMap(linetok[2], None, nevts=int(linetok[4])))
        pass
      if line.startswith("Wrote output file "):
        linetok=line.split(" ")
        nEvents=int(linetok[5])
        outFiles=[getFileMap(linetok[3], outputDSName, nevts=nEvents),]
        pass
      
    logfile.close()

    

  # assemble job report map, pickle it
  outMap = { 'prodsys': { 'trfCode': retcode,
                          'trfAcronym': acronym,  
                          'jobOutputs': outFiles,
                          'jobInputs': inFiles,
                          'nevents': nEvents,              
                        }
           }              
  f = open('jobReport.gpickle', 'w')
  pickle.dump(outMap, f)
  f.close()

  print "\n##################################################################"
  print   "## End of job."
  print   "##################################################################\n"

  print outMap

########################################
## main()
########################################

if __name__ == "__main__":

  #inputs = {
    #'inputLArFiles': ['data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0001.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0002.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0003.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0004.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0005.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0006.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-10._0007.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0001.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0002.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0003.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0004.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0005.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0006.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-11._0007.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0001.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0002.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0003.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0004.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0005.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0006.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-12._0007.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0001.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0002.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0003.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0004.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0005.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0006.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-4._0007.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0001.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0002.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0003.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0004.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0005.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0006.1',
                    #'data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784#data11_hi.00193558.calibration_LArCellsEmpty.recon.NTUP_SAMPLESMON.c784._lb0000._SFO-9._0007.1'],
    #'outputLArFile': 'data11_hi.00193558.calibration_LArCellsEmpty.merge.NTUP_SAMPLESMON.c784_c785#data11_hi.00193558.calibration_LArCellsEmpty.merge.NTUP_SAMPLESMON.c784_c785._0001'}

  #larMerge(inputs)
  #exit(0)

  if len(sys.argv) == 2 and sys.argv[1].startswith('--argdict=') :
    picklefile = sys.argv[1][len('--argdict='):]
    # extract parameters from pickle file
    print "Using pickled file ", picklefile, " for input parameters"
    f = open(picklefile, 'r')
    dataMap = pickle.load(f)
    f.close()
    larMerge(dataMap)
  elif len(sys.argv) == 3 and sys.argv[1].startswith('--inputFiles=') and sys.argv[2].startswith('--outputFile=') :
    inputFiles = sys.argv[1][13:].split(',')
    outputFile = sys.argv[2][13:]
    dataMap = {}
    dataMap['inputLArFiles'] = inputFiles
    dataMap['outputLArFile'] = outputFile
    larMerge(dataMap)
  else :
    print "Input format wrong --- either have: "
    print "   --argdict=<pickled-dictionary containing input info> "
    print "   with key/value pairs: "
    print "   or (old way): "
    print "   --inputFiles=file1,file2,...,fileN --outputFile=file"
    exit(1)
