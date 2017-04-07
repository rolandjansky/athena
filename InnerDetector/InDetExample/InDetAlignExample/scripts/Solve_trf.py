#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Solve trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##     1) 'inputTFiles': python list, input TFiles 
##         ['datasetname#filename1', 'datasetname#filename2', ...] 
##     2) 'inputAlignmentConstants':  string, input alignment constants, can be empty
##         'alignmentconstantsname#filename.root' 
##
##     3) 'outputAlignmentConstants': string, output alignment constants
##         'name#AlignmentConstants.root'
##     4) 'outputAlignmentConstants': string, output alignment constants, db file
##         'name#AlignmentConstants.db'
##  - assembles custom jobOptions file, runs athena
##
##########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib

#########################################################################

# Utility function

def getFileMap(fname, dsname, nevts=0) :
  if os.path.isfile(fname) :
    sz = os.path.getsize(fname)
    map = { 'lfn': fname,
            'dataset' : dsname,
            'size' : sz,
            'events' : nevts
          }
  else : 
    map = {}
  return map

#########################################################################

        
def runAthena(picklefile) :

  t0 = time.time()
  
  print "\n##################################################################"
  print   "##             ATLAS Tier-0 Alignment Processing                   ##"
  print   "##################################################################\n"

  # extract parameters from pickle file
  print "Using pickled file ", picklefile, " for input parameters"
  f = open(picklefile, 'r')
  parmap = pickle.load(f)
  f.close()

  print "\nFull Tier-0 run options:\n"
  pprint.pprint(parmap)

  inputfilelist = parmap.get('inputTFiles', [])
  nfiles = len(inputfilelist) 
  if not nfiles :   # problem with job definition or reading pickle file
    dt = int(time.time() - t0) 
    retcode = 1
    acronym = 'TRF_NOINPUT'
    txt = 'empty input file list'  
    outmap = { 'prodsys': { 'trfCode': retcode,
                            'trfAcronym': acronym,  
                            'jobOutputs': [],
                            'jobInputs': [],
                            'nevents': 0,
                            'more': { 'num1': 0, 'num2': dt, 'txt1': txt }
                          }
             }
  
  else : 
    # assemble list of ['filename1', 'filename2', ...]
    inflist = []
    indsname = (parmap['inputTFiles'][0]).split('#')[0]
    for val in parmap['inputTFiles'] :
      infname = val.split('#')[1]
      inflist.append(infname) 
    inputTFiles = inflist
	
         
    # input alignment constants
    try:
      inputAlignmentConstants = (parmap['inputAlignmentConstants']).split('#')[1]
    except:
      inputAlignmentConstants = ""
      
    # output file
    outdsname = (parmap['outputAlignmentConstants']).split('#')[0]
    outfile = (parmap['outputAlignmentConstants']).split('#')[1]
    ouputAlignmentConstants = outfile
    
    
    # output db file
    ouputAlignmentConstantsdb = (parmap['outputAlignmentConstantsdb']).split('#')[1]
    
    # assemble jobOptions fragment
    (s,o) = commands.getstatusoutput('rm -f myJobOptions.py')
    jOFile = open('myJobOptions.py', 'w')
    cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputTFiles = %s
inputAlignmentConstants = "%s"
outputPoolFile = "%s"



##-------- Alignment Configuration --------------------
WriteTFile = True
sctAlignmentLevelEndcaps	=	-1
pixelAlignmentLevel	=	1
sctAlignmentLevel	=	1
pixelAlignmentLevelBarrel	=	-1
pixelAlignmentLevelEndcaps	=	-1
sctAlignmentLevelBarrel	=	-1
alignSCT	=	True
if len(inputAlignmentConstants)!=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputAlignmentConstants]
else:
	readConstantsFromPool	=	False
	
solveLocal	=	False
sctAlignBarrel	=	True
runAccumulate	=	False
runLocal	=	False
runSolving	=	True
pixelAlignEndcaps	=	True
alignPixel	=	True
trtAlignmentLevel	=	1
alignTRT	=	True
#inputPoolFiles	=	['']
readConstantsFromPool	=	False
pixelAlignBarrel	=	True
sctAlignEndcaps	=	True


##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------

detectorDescription	=	"ATLAS-GEO-16-00-00"
globalTag	=	"COMCOND-REPP-007-05"

##-------- Extra Configuration --------------------
ModCut	=	7
beamSpotTag	=	"IndetBeampos-ES1-UPD2"
doReadBS	=	True
doBSConstraint	=	True
useOldPreProcessor	=	False
doPVConstraint	=	False
doTrkNtuple	=	False
doMonitoring	=	True

##-------- End of custom options -------------

##-------- Load Reconstruction --------------------
import os

loadInDetRec_Options = {"detectorDescription" : "ATLAS-GEO-08-00-00"
                       ,"doReadBS"            : False
		       ,"realData"            : True
		       ,"globalTag"           : ""
	               ,"siAlignmentTag"      : ""
	               ,"trtAlignmentTag"     : ""
		       ,"beamSpotTag"         : ""
		       ,"errorScalingTag"     : ""
	 	       ,"TRTCalibTextFile"    : ""
		       ,"useTRT"              : True
		       ,"inputPoolFiles"      : ""
		       }

# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_Options:
	if var in dir():
		loadInDetRec_Options[var] = eval(var)


# To extract constants from the database select the appropriate database tag.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource='data'


globalflags.DetDescrVersion = loadInDetRec_Options["detectorDescription"]
globalflags.print_JobProperties()

if len(globalflags.ConditionsTag())!=0:
	from IOVDbSvc.CondDB import conddb
	conddb.setGlobalTag(globalflags.ConditionsTag())

if loadInDetRec_Options["globalTag"]:
	from IOVDbSvc.CondDB import conddb
	conddb.setGlobalTag(loadInDetRec_Options["globalTag"])



# Setup geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()


from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from InDetRecExample.InDetJobProperties import InDetFlags


from IOVDbSvc.CondDB import conddb



    
# Its normally OK to read in some database so this can be left True.
# In case you want to force it not to read in anything you can set this to False.


#if loadInDetRec_Options["inputPoolFiles"] : 
#	os.system("pool_insertFileToCatalog *.root")


if loadInDetRec_Options["siAlignmentTag"]:
  conddb.addOverride('/Indet/Align',loadInDetRec_Options["siAlignmentTag"])
if loadInDetRec_Options["trtAlignmentTag"]:
  conddb.addOverride('/TRT/Align',loadInDetRec_Options["trtAlignmentTag"])

if len(loadInDetRec_Options["inputPoolFiles"]):
    include ("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
    # block folders that you want to override
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    print 'Loading initial alignment File'
    print 'Loading: ', loadInDetRec_Options["inputPoolFiles"]
    ServiceMgr.CondProxyProvider.InputCollections = loadInDetRec_Options["inputPoolFiles"]
    ServiceMgr.CondProxyProvider.OutputLevel=INFO
    print ServiceMgr.CondProxyProvider
    IOVSvc = Service("IOVSvc")
    IOVSvc.preLoadData = True
    ServiceMgr.PoolSvc.AttemptCatalogPatch = True
    
if loadInDetRec_Options["errorScalingTag"]:
  if ".db" in loadInDetRec_Options["errorScalingTag"]:
    conddb.blockFolder("/Indet/TrkErrorScaling")
    conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_cscasbuilt_00</tag>", force=True)
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])

if loadInDetRec_Options["beamSpotTag"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])

if loadInDetRec_Options["TRTCalibTextFile"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr+=TRTCalibDBSvc

  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]



# Options for IOVRegistrationSvc
# Select the following to delete and recreate the folders. Default is
# NOT to recreate
#import RegistrationServices.IOVRegistrationSvc
#regSvc = svcMgr.IOVRegistrationSvc
#regSvc.RecreateFolders = True

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc





##-------- Load Alignment --------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py") 

###############################################################
  ''' % (str(inflist), inputAlignmentConstants, ouputAlignmentConstants)

    jOFile.write(cont)
    jOFile.close()

    # run athena
    cmd = "python -u `which athena.py` myJobOptions.py"

    print "\nRun command:"
    print cmd
    print "\nLogfile:"
    print "------------------------------------------------------------------"
    retcode = os.system(cmd)
    print "------------------------------------------------------------------"
    dt = int(time.time() - t0)

    print "\n## athena.py finished with retcode = %s" % retcode
    print   "## ... elapsed time: ", dt, " sec"

    # Move alignment constants db file
    os.system("mv mycool.db %s\n" % ouputAlignmentConstantsdb)
    # assemble report pickle file
    outfiles = []
    infiles = []
    nevts = 0
    acronym = 'ALLOK'
    txt = 'trf finished OK'  

    # get info for report gpickle file
    if retcode == 0 :
      # one could determine the number of processed events here, e.g. by grep'ping in the logfile
      fmap = getFileMap(outfile, outdsname, nevts=nevts)
      outfiles = [fmap]
    if retcode != 0 :
      print "ERROR: athena.py execution problem!"
      acronym = 'TRF_ATHENA_EXE'
      txt = "athena.py execution problem"

    # assemble job report map
    outmap = { 'prodsys': { 'trfCode': retcode,
                            'trfAcronym': acronym,
                            'jobOutputs': outfiles,
                            'jobInputs': infiles,
                            'more': { 'num1': int(nevts), 'num2': int(dt), 'txt1': txt }
                          }
             }
  
  # pickle report map
  f = open('jobReport.gpickle', 'w')
  pickle.dump(outmap, f)
  f.close()

  print "\n##################################################################"
  print   "## End of job."
  print   "##################################################################\n"


########################################
## main()
########################################

if __name__ == "__main__":

  if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argdict=')) :
    print "Input format wrong --- use "
    print "   --argdict=<pickled-dictionary containing input info> "
    print "   with key/value pairs: "
    print "     1) 'inputFiles': python list "
    print "          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) "
    print "     2) 'outputFile': string 'datasetname#filename' "
    print "        (merged output dataset name + file) "
    sys.exit(-1)
  
  else :
    picklefile = sys.argv[1][len('--argdict='):]
    runAthena(picklefile)
