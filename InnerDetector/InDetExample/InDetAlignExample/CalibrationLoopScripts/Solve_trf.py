#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Solve trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##
##     1) 'inputTFiles': python list 
##          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) 
##        or list of file dictionaries 
##          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...},  
##           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...] 
##     2) 'inputAlignmentConstants':  string
##        (full path of input alignment constants file; optional) 
##     3) 'outputAlignmentConstants': string 'datasetname#filename' 
##        (output alignment constants, ROOT file) 
##     4) 'outputAlignmentConstantsdb': string 'datasetname#filename' 
##        (output alignment constants, DB file) 
##     5) 'outputAlignFiles': string 'datasetname#filename' 
##        (output alignment files) 
##     6) 'iteration': integer 
##        (number of current iteration)
##
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
                'dataset': dsname,
                'size': sz,
                'events': nevts
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

    print "\nAlignment Processing run options:\n"
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

        runnr = int(inputdsname.split('.')[1])
        print "runnr = ", runnr 
        project = inputdsname.split('.')[0] 

        # original RAW input files of accumulation step
        with open ("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt"  % (project, runnr, runnr), "r") as myfile :
            firstfile = myfile.readline().replace('\n', '')
        inputRAWFileList = [firstfile]
      
        # input alignment constants
        iteration = int(parmap['iteration']) 
        try :
            inputAlignmentConstants = (parmap['inputAlignmentConstants']).split('#')[1]
        except :
            inputAlignmentConstants = ""
        if iteration >= 1 :
            inputAlignmentConstants = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d.root" % (project, runnr, runnr, iteration-1)
      
        # output file
        outputAlignmentConstantsds = (parmap['outputAlignmentConstants']).split('#')[0]
        outputAlignmentConstants = (parmap['outputAlignmentConstants']).split('#')[1]
      
        # output db file
        outputAlignmentConstantsdbds = (parmap['outputAlignmentConstantsdb']).split('#')[0]
        outputAlignmentConstantsdb = (parmap['outputAlignmentConstantsdb']).split('#')[1]
      
        outputAlignFilesds = (parmap['outputAlignFiles']).split('#')[0]
        outputAlignFiles = (parmap['outputAlignFiles']).split('#')[1]
      
        # get beamspot tag
        #from InDetBeamSpotExample import COOLUtils
        #beamspottag = COOLUtils.resolveCurrentBeamSpotFolder()
      
        # assemble jobOptions fragment
        (s,o) = commands.getstatusoutput('rm -f myJobOptions.py')
        jOFile = open('myJobOptions.py', 'w')
        if iteration <= 1 :
            cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles  = %s
inputTFiles = %s
inputAlignmentConstants = "%s"
outputPoolFile = "%s"
numberOfEvents = 1  # Has to be one to get the reconstruction options

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) != 0 :
    readConstantsFromPool = True
    inputPoolFiles = [inputAlignmentConstants]
else :
    readConstantsFromPool = False

WriteTFile    = True
runAccumulate = False
runSolving    = True

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS       = False
doBSConstraint = False
doPVConstraint = False
doTrkNtuple    = False
doMonitoring   = False

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

## solving options
##   0 - No global solving
##   1 - Lapack
##   2 - Eigen
##   6 - ROOT
##   7 - CLHEP

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
sctAlignBarrelY = True
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = False
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

##-------- Load Reconstruction -----------------------------
import os
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
            ''' % (str(inputRAWFileList), str(inputfilenames), inputAlignmentConstants, outputAlignmentConstants, project)#,beamspottag)
        else :
            cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles  = %s
inputTFiles = %s
inputAlignmentConstants = "%s"
outputPoolFile = "%s"
numberOfEvents = 1  # Has to be one to get the reconstruction options

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) != 0 :
    readConstantsFromPool = True
    inputPoolFiles = [inputAlignmentConstants]
else :
    readConstantsFromPool = False

WriteTFile    = True
runAccumulate = False
runSolving    = True

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS       = False
doBSConstraint = False
doPVConstraint = False
doTrkNtuple    = False
doMonitoring   = False

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

## solving options
##   0 - No global solving
##   1 - Lapack
##   2 - Eigen
##   6 - ROOT
##   7 - CLHEP

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

##-------- Load Reconstruction -----------------------------
import os
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
            ''' % (str(inputRAWFileList), str(inputfilenames), inputAlignmentConstants, outputAlignmentConstants, project)#,beamspottag)
    
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

        if retcode == 0 :
            # Move alignment constants db file
            os.system("mv mycool.db %s" % outputAlignmentConstantsdb)
            if iteration >= 0 :
                os.system("mkdir -p /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d" % (project, runnr))
                os.system("cp %s /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d.root" % (outputAlignmentConstants, project, runnr, runnr, iteration))
                
                # extract original RAW input dataset name (accumulation step) from PFN 
                pfn = inputRAWFileList[0]           # e.g. '/castor/cern.ch/grid/atlas/.../data14_cos.00247688.physics_IDCosmic.merge.RAW._lb0552._SFO-ALL._0001.1' 
                lfn = pfn[pfn.rfind('/')+1:]        # e.g. 'data14_cos.00247688.physics_IDCosmic.merge.RAW._lb0552._SFO-ALL._0001.1' 
                inputdataset = lfn.split('._')[0]   # e.g. 'data14_cos.00247688.physics_IDCosmic.merge.RAW'
                # sign off next iteration through passing a message to TOM
                os.system("touch /afs/cern.ch/atlas/tzero/run/cafTOM/msgbox/idalignTOM.msg")
                os.system("echo \"call pdb updateDatasetPState %s aligned Iter%d\" >> /afs/cern.ch/atlas/tzero/run/cafTOM/msgbox/idalignTOM.msg" % (inputdataset, iteration))
            
            os.system("tar -cf %s *.txt *.root %s %s" % (outputAlignFiles, outputAlignmentConstantsdb, outputAlignmentConstants)) 
            #os.system("./makeRootFile.py alignlogfile.txt %s\n" % outputCorrectionsTree)
            # move TFile
            #os.system("mv AlignmentTFile.root TotalAlignmentTFile.root")
    
        # assemble report pickle file
        outfiles = []
        infiles = []
        nevts = 0
        acronym = 'OK'
        txt = 'trf finished OK'  

        # get info for report gpickle file
        if retcode == 0 :
            # one could determine the number of processed events here, e.g. by grep'ping in the logfile
            constantsmap = getFileMap(outputAlignmentConstants, outputAlignmentConstantsds, nevts=nevts)
            outfiles.append(constantsmap)
            constantsdbmap = getFileMap(outputAlignmentConstantsdb, outputAlignmentConstantsdbds, nevts=nevts)
            outfiles.append(constantsdbmap)
            outputAlignFilesmap = getFileMap(outputAlignFiles, outputAlignFilesds, nevts=nevts)
            outfiles.append(outputAlignFilesmap)
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
        print "     1) 'inputTFiles': python list "                                                                                   
        print "          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) " 
        print "        or list of file dictionaries " 
        print "          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...}, "  
        print "           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...] "
        print "     2) 'inputAlignmentConstants':  string "
        print "        (full path of input alignment constants file; optional) " 
        print "     3) 'outputAlignmentConstants': string 'datasetname#filename' " 
        print "        (output alignment constants, ROOT file) " 
        print "     4) 'outputAlignmentConstantsdb': string 'datasetname#filename' "
        print "        (output alignment constants, DB file) " 
        print "     5) 'outputAlignFiles': string 'datasetname#filename' " 
        print "        (output alignment files) " 
        print "     6) 'iteration': integer " 
        print "        (number of current iteration) "
        print " "    
        sys.exit(-1)
    
    else :
        picklefile = sys.argv[1][len('--argdict='):]
        runAthena(picklefile)
