#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### last change by $Vicente Lacuesta$
### $09-03-2011$
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
##########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib

#########################################################################

# Utility function

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
                inputfilenames.append(val.split('#')[1])
        elif isinstance(inputfilelist[0], dict) :
            inputdsname = inputfilelist[0]['dsn']
            svcclass = inputfilelist[0].get('svcclass', '')
            for fdict in inputfilelist :
                if fdict.has_key('pfn') : 
                    # full remote path, for direct access on storage
                    if fdict['pfn'].startswith('/castor') :
                        inputfilenames.append('root://castoratlas/' + fdict['pfn'])
                        #inputfilenames.append('rfio:' + fdict['pfn'])
                    else :
                        inputfilenames.append(fdict['pfn'])
                else :
                    inputfilenames.append(fdict['lfn'])    

        #alignmentLevel = int(parmap['alignmentLevel']) 
         
        runnr = int(inputdsname.split('.')[1])
        project = inputdsname.split('.')[0] 
        events = parmap['events']
        iteration = parmap['iteration']  

        tfiledsname = (parmap['outputTFile']).split('#')[0]
        tfile = (parmap['outputTFile']).split('#')[1]
              
        mondsname = (parmap['outputMonitoringFile']).split('#')[0]
        monfile = (parmap['outputMonitoringFile']).split('#')[1]

        # get beamspot tag
        #from InDetBeamSpotExample import COOLUtils
        #beamspottag = COOLUtils.resolveCurrentBeamSpotFolder()

        if iteration >= 1 :
            inputAlignmentConstants = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d.root" % (project, runnr, runnr, iteration-1)
        else :
            inputAlignmentConstants = ""

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
doMonitoring   = True

Cosmics     = True
realData    = True
useTRT      = True
BField      = True
dataSource  = 'data' 
projectName = '%s'
PtCut       = 2000

#Alignment Method is GX2
runLocal      = False
solveLocal    = False
solvingOption =	1

# Temp configuration
ModCut = 0
detectorDescription = "ATLAS-R2-2015-02-00-00"
globalTag = "CONDBR2-ES1PA-2014-03"

#siAlignmentTag = "InDetAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
siAlignmentTag = ""

#trtAlignmentTag   = "TRTAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#trtAlignmentTagL3 = "TRTCalibDX_EoverP_189598_189845v1"

## Pixel Alignment Levels 
pixelAlignmentLevel = 11
pixelAlignmentLevelBarrel = -1
pixelAlignmentLevelEndcaps = -1

## Pixel alignment DoFs
alignPixel = True
pixelAlignBarrel = True
pixelAlignEndcaps = True
## alignment parameters Barrel
pixelAlignBarrelX = True
pixelAlignBarrelY = True
pixelAlignBarrelZ = True
pixelAlignBarrelRotX = False
pixelAlignBarrelRotY = True
pixelAlignBarrelRotZ = True
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
sctAlignEndcaps = False

## alignment parameters Barrel
sctAlignBarrelX = True
sctAlignBarrelY = False
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = True
sctAlignBarrelRotZ = True
## alignment parameters Endcap
sctAlignEndcapX = True
sctAlignEndcapY = True
sctAlignEndcapZ = True
sctAlignEndcapRotX = True
sctAlignEndcapRotY = True
sctAlignEndcapRotZ = True

## TRT Alignment Levels 
alignTRT = False
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
trtAlignEndcapRotX = True
trtAlignEndcapRotY = True
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
            ''' % (str(inputfilenames), inputAlignmentConstants, events, project)
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
doMonitoring   = True

Cosmics     = True
realData    = True
useTRT      = True
BField      = True
dataSource  = 'data' 
projectName = '%s'
PtCut       = 2000

#Alignment Method is GX2
runLocal      = False
solveLocal    = False
solvingOption =	1

# Temp configuration
ModCut = 0
detectorDescription = "ATLAS-R2-2015-02-00-00"
globalTag = "CONDBR2-ES1PA-2014-03"

#siAlignmentTag = "InDetAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
siAlignmentTag = ""

#trtAlignmentTag   = "TRTAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#trtAlignmentTagL3 = "TRTCalibDX_EoverP_189598_189845v1"

## Pixel Alignment Levels 
pixelAlignmentLevel = 2
pixelAlignmentLevelBarrel = -1
pixelAlignmentLevelEndcaps = -1

## Pixel alignment DoFs
alignPixel = True
pixelAlignBarrel = True
pixelAlignEndcaps = True
## alignment parameters Barrel
pixelAlignBarrelX = True
pixelAlignBarrelY = True
pixelAlignBarrelZ = True
pixelAlignBarrelRotX = False
pixelAlignBarrelRotY = True
pixelAlignBarrelRotZ = True
## alignment parameters Endcap
pixelAlignEndcapX = True
pixelAlignEndcapY = True
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = False
pixelAlignEndcapRotY = False
pixelAlignEndcapRotZ = True

## SCT Alignment Levels 
sctAlignmentLevel = 2
sctAlignmentLevelBarrel = -1
sctAlignmentLevelEndcaps = 1

## SCT alignment DoFs
alignSCT = True
sctAlignBarrel = True
sctAlignEndcaps = True

## alignment parameters Barrel
sctAlignBarrelX = True
sctAlignBarrelY = False
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = True
sctAlignBarrelRotZ = True
## alignment parameters Endcap
sctAlignEndcapX = True
sctAlignEndcapY = True
sctAlignEndcapZ = False
sctAlignEndcapRotX = False
sctAlignEndcapRotY = False
sctAlignEndcapRotZ = True

## TRT Alignment Levels 
alignTRT = False
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
trtAlignEndcapRotX = True
trtAlignEndcapRotY = True
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
            ''' % (str(inputfilenames), inputAlignmentConstants, events, project)

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
        
        if iteration == 0 :
            if not os.path.isfile("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (project, runnr, runnr)) : 
                os.system("mkdir -p /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d" % (project, runnr))
                os.system("touch /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (project, runnr, runnr))
            for fn in inputfilenames : 
                os.system("echo \"%s\" >> /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt" % (fn, project, runnr, runnr))
        
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

