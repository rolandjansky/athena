#!/usr/bin/env python

### last change by $Javier Jimenez$
### $22-03-2016$
### $0.9$

#########################################################################
##
## Accumulate trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##
##     1) 'inputRAWFiles': python list 
##          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) 
##        or list of file dictionaries 
##          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...},  
##           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...] 
##     2) 'outputTFile': string 'datasetname#filename' 
##        (output TFile containing the matrix, vector...) 
##     3) 'outputMonitoringFile': string 'datasetname#filename' 
##        (output monitoring file) 
##     4) 'iteration': integer 
##        (number of current iteration)
##     5) 'events': integer 
##        (number of events to process, -1 for all)
##
##  - assembles custom jobOptions file, runs athena
##  - generates an outputTFile and a monitoring.root file
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib, random

#########################################################################

# Utility functions

def getFileMap(fname, dsname, nevts=0) :
    if os.path.isfile(fname) :
        sz = os.path.getsize(fname)
        map = { 'lfn': fname,
                'dataset': dsname,
                'size': sz,
                'events': nevts
              }
    else : 
        map = {}
    return map

#------------------------------------------------------------------------

def fileFilter(fname) : 
    patterns = ['SFO-1', 'SFO-2', 'SFO-ALL']
    retval = False
    for p in patterns :
        retval = (fname.find(p) > -1)
        if retval : break

    return retval
    
#########################################################################

        
def accumData(picklefile) :
    t0 = time.time()
    
    print "\n##################################################################"
    print   "##             ATLAS Tier-0 Alignment Processing                ##"
    print   "##################################################################\n"

    # extract parameters from pickle file
    print "Using pickled file ", picklefile, " for input parameters"
    f = open(picklefile, 'r')
    parmap = pickle.load(f)
    f.close()

    print "\nAlignment Processing run options:\n"
    pprint.pprint(parmap)
      
    inputfilelist = parmap.get('inputRAWFiles', [])
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
        inputdsname = ''
        inputfilenames = []
        svcclass = ''
        if isinstance(inputfilelist[0], str) :  
            inputdsname = (inputfilelist[0]).split('#')[0]
            for val in inputfilelist :
                inputfname = val.split('#')[1]
                if fileFilter(inputfname) :
                    inputfilenames.append(inputfname)
        
        elif isinstance(inputfilelist[0], dict) :
            inputdsname = inputfilelist[0]['dsn']
            svcclass = inputfilelist[0].get('svcclass', '')
            for fdict in inputfilelist :
                if fdict.has_key('pfn') : 
                    # full remote path, for direct access on storage
                    prefix = ''
                    if fdict['pfn'].startswith('/castor') :
                        prefix = 'root://castoratlas/'
                    elif fdict['pfn'].startswith('/eos') :
                        prefix = 'root://eosatlas/'
                    inputfname = prefix + fdict['pfn']
                else :
                    inputfname = fdict['lfn']
                if fileFilter(inputfname) :
                    inputfilenames.append(inputfname)       


        # if filtered input file list is empty ==> quit job, return success
        if not len(inputfilenames) :
            print "\nInput file(s) not matching filter criteria ==> exiting."
            dt = int(time.time() - t0) 
            retcode = 0
            acronym = 'OK'
            txt = 'empty filtered input file list'  
            outmap = { 'prodsys': { 'trfCode': retcode,
                                    'trfAcronym': acronym,  
                                    'jobOutputs': [],
                                    'jobInputs': [],
                                    'nevents': 0,
                                    'more': { 'num1': 0, 'num2': dt, 'txt1': txt }
                                  }
                     }
        else : 

            # proceed with filtered input file list
            runnr = int(inputdsname.split('.')[1])
            project = inputdsname.split('.')[0] 
            events = int(parmap.get('maxEvents', '5000'))
            skipEvents = int(parmap.get('skipEvents', '0'))
            iteration = parmap['iteration']  
            #JAVI: extracting actual LB number from the inputFile name
            LB = int(inputfilenames[0].split('._')[1][2:6])

            blockLBsize = 100
            nLBtoSkip = 60

            #JAVI: For iteration==3 the proper alignment constants have to be taken
            #JAVI: Calculating relative position to the first LB
            #JAVI: We have to take special care of the first nLBtoSkip LB and the last ones
            #JAVI: there is need to know how many blocks of nLBtoSkip LB are. 
            if iteration == 3 : 
                listOfLB = []
                f1 = open("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (project, runnr, runnr), 'r')
                for line in f1 :
                    lineNumber = int(line.replace('\n', '').split('._')[1][2:])
                    if lineNumber in listOfLB : pass
                    else : listOfLB.append(lineNumber)
                listOfLB.sort()
                
                #JAVI: Already have a sorted list of LB for the run, lets determine first and last one and the relative position of the actual one
                #firstLB = listOfLB[0]
                nBlocks = (len(listOfLB) - nLBtoSkip) // blockLBsize
                if nBlocks == 0 : nBlocks += 1
                
                #JAVI: It is possible that some LB were not recorded, has to correct for that.
                position = 0
                relativePosition = 0
                for x in listOfLB:
                    if LB == x: relativePosition = position
                    position += 1
                
                #block = (LB - firstLB - nLBtoSkip) // blockLBsize
                block = (relativePosition - nLBtoSkip) // blockLBsize
                if block < 0 : block = 0 #JAVI: this is the case for runs from the first nLBtoSkip LB
                if block == nBlocks : block -= 1 #JAVI: this is the case for the last LB, wich are added to previous block of nLBtoSkip LB              

            tfiledsname = (parmap['outputTFile']).split('#')[0]
            tfile = (parmap['outputTFile']).split('#')[1]
                  
            mondsname = (parmap['outputMonitoringFile']).split('#')[0]
            monfile = (parmap['outputMonitoringFile']).split('#')[1]

            # get beamspot tag
            #from InDetBeamSpotExample import COOLUtils
            #beamspottag = COOLUtils.resolveCurrentBeamSpotFolder()

            IBLDistDB = parmap.get('IBLDistDB', '/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/InnerDetector/InDetExample/InDetAlignExample/BowingDB/BowingNULL.db')
            inputAlignmentConstants = parmap.get('inputAlignmentConstants', '')

            if iteration >= 1 :
                inputAlignmentConstants = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d.root" % (project, runnr, runnr, iteration-1)
                IBLDistDB = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpIBLDistDB/%d/%d_IBLDistDB_Iter%d.db" % (project, runnr, runnr, iteration-1)
            if iteration == 3 :
                inputAlignmentConstants = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d_Block%02d.root" % (project, runnr, runnr, iteration-1, block)
                IBLDistDB = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpIBLDistDB/%d/%d_IBLDistDB_Iter%d_Block%02d.db" % (project, runnr, runnr, iteration-1, block)

            (s,o) = commands.getstatusoutput('rm -f myJobOptions.py')
            jOFile = open('myJobOptions.py', 'w')
            if iteration <= 1 :
                cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles = %s
inputAlignmentConstants = "%s"
numberOfEvents = %d
SkipEvents = %d
inputBowingDatabase = "%s"

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) != 0 :
    readConstantsFromPool = True
    inputPoolFiles = [inputAlignmentConstants]
else :
    readConstantsFromPool = False

WriteTFile    = True
runAccumulate = True
runSolving    = False

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS       = False
doBSConstraint = False
doPVConstraint = False
doTrkNtuple    = False
#doMonitoring   = True

Cosmics     = False
realData    = True
useTRT      = True
BField      = True
dataSource  = 'data' 
projectName = '%s'
PtCut       = 3000

#Only for first collisions of 2016
#use50ns     = True

applyLBibldistTweak = False
writeIBLDistDB = True

if projectName == "data15_hi":
    HeavyIons = True                                                                                                                                                                                       

#Alignment Method is GX2
runLocal      = False
solveLocal    = False
solvingOption =	1

# Temp configuration
ModCut = 0
detectorDescription = "ATLAS-R2-2015-04-00-00"
#globalTag = "CONDBR2-ES1PA-2015-11" #
globalTag = "CONDBR2-BLKPA-2016-12"
#globalTag = "CONDBR2-ES1PA-2015-10"


#AlignIBLbutNotPixel=True

#siAlignmentTag = "InDetAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#Use no tags for Heavy Ions as last constants are in bulk.

#siAlignmentTag = "InDetAlign-RUN2-IBLm20"
#siAlignmentTag = "InDetAlign-RUN2-REPRO25NS"
#siAlignmentTag = "InDetAlign_R2_2016_AfterFirstStableBeams"
#siAlignmentTag = "InDetAlign_R2_IBLPlus5Extrapolated_June2016"
#siAlignmentTag = "InDetAlign_R2_Post_TS1_June2016" # since June 14, 2016
siAlignmentTag  = "InDetAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016

#trtAlignmentTag   = "TRTAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#trtAlignmentTagL3 = "TRTCalibDX_EoverP_189598_189845v1"
#trtAlignmentTag   = "TRTAlign-RUN2-25NS"
#trtAlignmentTag   = "TRTAlign-RUN2-REPRO25NS"
#trtAlignmentTag   = "TRTAlign_R2_2016_AfterFirstStableBeams"
#trtAlignmentTag   = "TRTAlign_R2_Post_TS1_June2016"
trtAlignmentTag    = "TRTAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016


## Pixel Alignment Levels 
pixelAlignmentLevel = 11
pixelAlignmentLevelBarrel = -1
pixelAlignmentLevelEndcaps = -1

## Pixel alignment DoFs
alignPixel = True
pixelAlignBarrel = True
pixelAlignEndcaps = True
## alignment parameters Barrel
pixelAlignBarrelBowX = True
pixelAlignBarrelX = True
pixelAlignBarrelY = True
pixelAlignBarrelZ = True
pixelAlignBarrelRotX = True
pixelAlignBarrelRotY = True
pixelAlignBarrelRotZ = True

Remove_IBL_Rz = True # This turns off IBL Rz while keeping Pixel one

## alignment parameters Endcap
pixelAlignEndcapX = True
pixelAlignEndcapY = True
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = False
pixelAlignEndcapRotY = False
pixelAlignEndcapRotZ = True

## SCT Alignment Levels 
sctAlignmentLevel = 1
sctAlignmentLevelBarrel = -1
sctAlignmentLevelEndcaps = -1

## SCT alignment DoFs
alignSCT = True
sctAlignBarrel = True
sctAlignEndcaps = True

## alignment parameters Barrel
sctAlignBarrelX = False
sctAlignBarrelY = False
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = False
sctAlignBarrelRotZ = False
## alignment parameters Endcap
sctAlignEndcapX = True
sctAlignEndcapY = True
sctAlignEndcapZ = False
sctAlignEndcapRotX = False
sctAlignEndcapRotY = False
sctAlignEndcapRotZ = True

## TRT Alignment Levels 
alignTRT = True
trtAlignmentLevel = 1
trtAlignBarrel = True
trtAlignEndcaps = True
## alignment parameters Barrel
trtAlignBarrelX = True
trtAlignBarrelY = True
trtAlignBarrelZ = False
trtAlignBarrelRotX = True
trtAlignBarrelRotY = True
trtAlignBarrelRotZ = True
## alignment parameters Endcap
trtAlignEndcapX = True
trtAlignEndcapY = True
trtAlignEndcapZ = False
trtAlignEndcapRotX = False
trtAlignEndcapRotY = False
trtAlignEndcapRotZ = True

##-------- End of custom options -------------

##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------

##-------- Load Reconstruction -----------------------------
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
                ''' % (str(inputfilenames), inputAlignmentConstants, events, skipEvents, IBLDistDB, project)
            else :
                cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles = %s
inputAlignmentConstants = "%s"
numberOfEvents = %d
SkipEvents = %d
inputBowingDatabase = "%s"

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) != 0 :
    readConstantsFromPool = True
    inputPoolFiles = [inputAlignmentConstants]
else :
    readConstantsFromPool = False

WriteTFile    = True
runAccumulate = True
runSolving    = False

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS       = False
doBSConstraint = False
doPVConstraint = False
doTrkNtuple    = False
#doMonitoring   = True

Cosmics     = False
realData    = True
useTRT      = True
BField      = True
dataSource  = 'data' 
projectName = '%s'
PtCut       = 3000

#Only for first collisions of 2016
#use50ns     = False

applyLBibldistTweak = False
writeIBLDistDB = True

if projectName == "data15_hi":
    HeavyIons = True

#Alignment Method is GX2
runLocal      = False
solveLocal    = False
solvingOption =	1

# Temp configuration
ModCut = 0
detectorDescription = "ATLAS-R2-2015-04-00-00"
#globalTag = "CONDBR2-ES1PA-2015-10" #CONDBR2-BLKPA-2015-14
globalTag = "CONDBR2-BLKPA-2016-12"

AlignIBLbutNotPixel=True

#siAlignmentTag = "InDetAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#Use no tags for Heavy Ions as last constants are in bulk.
#siAlignmentTag = "InDetAlign-RUN2-IBLm20"
#trtAlignmentTag   = "TRTAlign-RUN2-25NS"

#siAlignmentTag = "InDetAlign-RUN2-REPRO25NS"
#trtAlignmentTag = "TRTAlign-RUN2-REPRO25NS"

#siAlignmentTag  = "InDetAlign_R2_IBLPlus5Extrapolated_June2016"
#siAlignmentTag  = "InDetAlign_R2_2016_AfterFirstStableBeams"
siAlignmentTag  = "InDetAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016

#trtAlignmentTag = "TRTAlign_R2_2016_AfterFirstStableBeams"
#trtAlignmentTag   = "TRTAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#trtAlignmentTagL3 = "TRTCalibDX_EoverP_189598_189845v1"
trtAlignmentTag    = "TRTAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016

## Pixel Alignment Levels 
pixelAlignmentLevel = 16
pixelAlignmentLevelBarrel = -1
pixelAlignmentLevelEndcaps = -1

## Pixel alignment DoFs
alignPixel = True
pixelAlignBarrel = True
pixelAlignEndcaps = False
## alignment parameters Barrel
pixelAlignBarrelX = False
pixelAlignBarrelY = False
pixelAlignBarrelZ = False
pixelAlignBarrelRotX = False
pixelAlignBarrelRotY = False
pixelAlignBarrelRotZ = False
pixelAlignBarrelBowX = True
## alignment parameters Endcap
pixelAlignEndcapX = False
pixelAlignEndcapY = False
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = False
pixelAlignEndcapRotY = False
pixelAlignEndcapRotZ = False

## SCT Alignment Levels 
sctAlignmentLevel = 1
sctAlignmentLevelBarrel = -1
sctAlignmentLevelEndcaps = -1

## SCT alignment DoFs
alignSCT = False
sctAlignBarrel = False
sctAlignEndcaps = False

## alignment parameters Barrel
sctAlignBarrelX = False
sctAlignBarrelY = False
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = False
sctAlignBarrelRotZ = False
## alignment parameters Endcap
sctAlignEndcapX = False
sctAlignEndcapY = False
sctAlignEndcapZ = False
sctAlignEndcapRotX = False
sctAlignEndcapRotY = False
sctAlignEndcapRotZ = False

## TRT Alignment Levels 
alignTRT = False
trtAlignmentLevel = 1
trtAlignBarrel = False
trtAlignEndcaps = False
## alignment parameters Barrel
trtAlignBarrelX = False
trtAlignBarrelY = False
trtAlignBarrelZ = False
trtAlignBarrelRotX = False
trtAlignBarrelRotY = False
trtAlignBarrelRotZ = False
## alignment parameters Endcap
trtAlignEndcapX = False
trtAlignEndcapY = False
trtAlignEndcapZ = False
trtAlignEndcapRotX = False
trtAlignEndcapRotY = False
trtAlignEndcapRotZ = False

##-------- End of custom options -------------

##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------

##-------- Load Reconstruction -----------------------------
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
                ''' % (str(inputfilenames), inputAlignmentConstants, events, skipEvents, IBLDistDB, project)

            jOFile.write(cont)
            jOFile.close()
            
            # run athena
            cmd = ""
            if svcclass : 
                cmd += "export STAGE_SVCCLASS=%s ; " % svcclass
            cmd += "python -u `which athena.py` myJobOptions.py"
            
            print "\nRun command:"
            print cmd
            print "\nLogfile:"
            print "------------------------------------------------------------------"
            retcode = os.system(cmd)
            print "------------------------------------------------------------------"
            dt = int(time.time() - t0)
            print "\n## athena.py finished with retcode = %s" % retcode
            print   "## ... elapsed time: ", dt, " sec"
            
            # rename output files
            try :
                os.system("mv monitoring.root %s" % monfile)
                os.system("mv AlignmentTFile.root %s" % tfile)
            except :
                pass
            
            # assemble report pickle file
            outfiles = []
            infiles = []
            nevts = 0
            acronym = 'OK'
            txt = 'trf finished OK'  
            
            # get info for report gpickle file
            if retcode == 0 :
                if iteration == 0 :
                    if not os.path.isfile("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (project, runnr, runnr)) : 
                        os.system("mkdir -p /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d" % (project, runnr))
                        os.system("touch /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (project, runnr, runnr))
                    for fn in inputfilenames : 
                        rc = os.system("echo \"%s\" >> /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (fn, project, runnr, runnr))
                        # retry in case of problems
                        if rc :
                            time.sleep(random.randint(1,20))
                            os.system("echo \"%s\" >> /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (fn, project, runnr, runnr))
            
                # one could determine the number of processed events here, e.g. by grep'ping in the logfile
                tfilemap = getFileMap(tfile, tfiledsname)
                if tfilemap : 
                    outfiles.append(tfilemap)
                monmap = getFileMap(monfile, mondsname)   
                if monmap : 
                    outfiles.append(monmap)
                infiles = inputfilelist
            else :
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

if __name__ == "__main__" :

    if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argdict=')) :
        print "Input format wrong --- use "
        print "   --argdict=<pickled dictionary containing input info> "
        print "   with key/value pairs: "
        print "     1) 'inputRAWFiles': python list "
        print "          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) "
        print "        or list of file dictionaries "
        print "          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...}, " 
        print "           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...] "
        print "     2) 'outputTFile': string 'datasetname#filename' "
        print "        (output TFile containing the matrix, vector...) "
        print "     3) 'outputMonitoringFile': string 'datasetname#filename' "
        print "        (output monitoring file) "
        print "     4) 'iteration': integer "
        print "        (number of current iteration)"
        print "     5) 'events': integer "
        print "        (number of events to process, -1 for all)"
        print " "    
        sys.exit(-1)
  
    else :
        picklefile = sys.argv[1][len('--argdict='):]
        accumData(picklefile)


