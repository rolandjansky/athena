#!/usr/bin/env python

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
##     2) 'inputAlignmentConstants': string
##        (full path of input alignment constants file; optional) 
##     3) 'outputAlignmentConstants': string 'datasetname#filename' 
##        (output alignment constants, ROOT file) 
##     4) 'outputAlignmentConstantsdb': string 'datasetname#filename' 
##        (output alignment constants, DB file) 
##     5) 'outputAlignFiles': string 'datasetname#filename' 
##        (output alignment files) 
##     6) 'iteration': integer 
##        (number of current iteration; optional -- if not given, will be extracted from input dataset name)
##
##  - assembles custom jobOptions file, runs athena
##
##########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib, smtplib

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

# Function for e-mail notification

def sendStatusMail(runNumber) :
    mserver = 'cernmx.cern.ch'
    mfrom = 'atltzp1@cern.ch'
    mto = ['Pierfrancesco.Butti@cern.ch', 'Matthias.Danninger@cern.ch', 'Heather.Gray@cern.ch', 'Shih-Chieh.Hsu@cern.ch', 'Javier.Jimenez.Pena@cern.ch', 'Hideyuki.Oide@cern.ch', 'Armin.Nairz@cern.ch']
    msubject = "ID Alignment Processing for Run %d Completed" % runNumber

    # assemble mail body
    mbody  = " %s: Tier-0 ID alignment processing finished for run %d. \n\n" % (time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime()), runNumber)
    mbody += " Output can be found on EOS and AFS: \n"
    mbody += "    /eos/atlas/atlastier0/tzero/caf/IDAlignment/data16_13TeV/calibration_IDTracks/00%d/ \n" % runNumber
    mbody += "    /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/data16_13TeV/calibration_IDTracks/00%d/ \n" % runNumber
    rc = 0
    try :
        con = smtplib.SMTP(mserver)
        if isinstance(mto, str) :
            con.sendmail(mfrom, mto, 'Subject:' + str(msubject) + '\n\n' + str(mbody))
        elif isinstance(mto, list) :
            for onemto in mto :
                con.sendmail(mfrom, onemto, 'Subject:' + str(msubject) + '\n\n' + str(mbody))
        con.quit()
    except :
        rc = 1

    return rc

#########################################################################

#########################################################################

# Function for adding run to list of succesfully ended runs

def AddToListOfGoodRuns(runNumber, project) :

    # assemble line to be written
    line = "%s - %08d - %f" % (time.strftime("%Y/%m/%d %H:%M:%S", time.localtime()), runNumber, time.time())
    rc = os.system("echo \"%s\" >> /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/GoodEndedRuns/GoodEndedRuns.txt" % (line, project))
    
    return rc


def AddToListOfBadRuns(runNumber, project) :

    # assemble line to be written
    line = "%s - %08d - %f" % (time.strftime("%Y/%m/%d %H:%M:%S", time.localtime()), runNumber, time.time())
    rc = os.system("echo \"%s\" >> /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/BadEndedRuns/BadEndedRuns.txt" % (line, project))
    
    return rc
#########################################################################

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
    #JAVI: I will use these as the Total number of LB
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
        svcclass = 't0atlas'

        blockLBsize = parmap.get('blockLBSize', 100)
        nLB_FastPixelMovement = parmap.get('nLB_FastPixelMovement', 60)
        blockLBsize_FastStep = parmap.get('blockLBSize_FastStep', 20)

        #JAVI: Last block should take the last LBs, eg: blockLBsize + 56 LB
        #JAVI: For all iterations, we should have a list of lists:
        #JAVI: inputfilenames = [[] for x in xrange(nBlocks)]

        #JAVI: Making first an auxiliary input file name list, ordering it and then removing first nLB_FastPixelMovement LB
        inputfilenamesAUX = []
        
        if isinstance(inputfilelist[0], str) :  
            inputdsname = (inputfilelist[0]).split('#')[0]
            for val in inputfilelist :
                inputfilenamesAUX.append(val.split('#')[1])

        elif isinstance(inputfilelist[0], dict) :
            inputdsname = inputfilelist[0]['dsn']
            svcclass = inputfilelist[0].get('svcclass', 't0atlas')
            for fdict in inputfilelist :
                if fdict.has_key('pfn') : 
                    # full remote path, for direct access on storage
                    if fdict['pfn'].startswith('/castor') :
                        inputfilenamesAUX.append('root://castoratlas/' + fdict['pfn'])                        
                    elif fdict['pfn'].startswith('/eos') :
                        inputfilenamesAUX.append('root://eosatlas/' + fdict['pfn'])
                    else :
                        inputfilenamesAUX.append(fdict['pfn'])
                else :
                    inputfilenamesAUX.append(fdict['lfn'])    

        inputfilenamesAUX.sort()
        
        # fall-back: extract iteration from input dataset name, e.g. data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0
        iterAUX = int((inputdsname.split('.')[5]).replace('Iter', ''))
        iteration = parmap.get('iteration', iterAUX) 

        #JAVI: Next lines determine how many LB have been processed per job
        #JAVI: They can come in bunches of 5 LB each
        #JAVI: The format can be something as ._lb0540 in the case of only one lb per job
        #JAVI: or as ._lb0540-lb0544 in the case of multiple lb per job, 5 in this case
        infoLB = inputfilenamesAUX[0].split('._')[1]
        jobFirstLB = int(infoLB[2:6]) #JAVI: as it is already sorted by LB
        if len(infoLB) == 6 :  
            jobLBsize = 1
        else : 
            jobLBsize = int(infoLB[-4:]) - int(infoLB[2:6]) + 1


        #JAVI: For all iterations, we need the list of lists:
        #JAVI: For all nBlocks calculation, need to take into account first shorter blocks
        nBlocks = nLB_FastPixelMovement // blockLBsize_FastStep + (nfiles*jobLBsize - nLB_FastPixelMovement) // blockLBsize
        if nBlocks == 0: nBlocks += 1
        inputfilenames = [[] for x in xrange(nBlocks)]

        count = 0
        usedLB = 0 
        for val in inputfilenamesAUX :
            if count < nLB_FastPixelMovement :
                #JAVI: Assigning first nLB_FastPixelMovement LBs to finer blocks
                usedblock = usedLB // blockLBsize_FastStep
                #JAVI: divide in groups of blockLBsize LB
                inputfilenames[usedblock].append(val)
                usedLB += jobLBsize
            else :
                #JAVI: After nLB_FastPixelMovement, bigger blockLBsize is used
                usedblock = nLB_FastPixelMovement // blockLBsize_FastStep + ( usedLB - nLB_FastPixelMovement ) // blockLBsize
                #JAVI: divide in groups of blockLBsize LB
                if usedblock == nBlocks : usedblock -= 1
                #JAVI: Last list solves for blockLBsize LB plus the lasting ones. 
                inputfilenames[usedblock].append(val)
                usedLB += jobLBsize
            count += jobLBsize

        runnrstr = inputdsname.split('.')[1]
        runnr = int(runnrstr)
        print "runnr = ", runnr 
        project = inputdsname.split('.')[0]
        stream = inputdsname.split('.')[2] 

        # original RAW input files of accumulation step
        with open ("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/FileLists/%d/Run%d.txt"  % (project, runnr, runnr), "r") as myfile :
            firstfile = myfile.readline().replace('\n', '')
        inputRAWFileList = [firstfile]
        
        # copy RAW input file to local dir if necessary (A.N., 2015-03-13, 2016-04-05)
        if not (firstfile.startswith('root://')) :
            rawdsname = firstfile[:firstfile.find('._')]
            # try first to get source file from EOS, dir /eos/atlas/atlastier0/rucio/<project>/<stream>/<runnr>/<dataset>/
            eospath = '/eos/atlas/atlastier0/rucio/%s/%s/%s/%s/%s' % (project, stream, runnrstr, rawdsname, firstfile)
            retcode = os.system('xrdcp --nopbar root://eosatlas/' + eospath + ' .')
            # if retrieval from EOS fails, fall back to CASTOR, dir /castor/cern.ch/grid/atlas/rucio/raw/<project>/<stream>/<runnr>/<dataset>/
            if retcode :
                castorpath = '/castor/cern.ch/grid/atlas/rucio/raw/%s/%s/%s/%s/%s' % (project, stream, runnrstr, rawdsname, firstfile)
                retcode = os.system('export STAGE_SVCCLASS=%s; rfcp %s .' %  (svcclass, castorpath))  
            
        # input alignment constants
        #JAVI: Now we are going to load the alignment constants: a string for the two first iterations and a list of strings in the last two ones
        #JAVI: Same way goes for the IBLDistDB
        inputAlignmentConstants = []
        IBLDistDB = []

        if iteration == 0:
            for x in xrange(nBlocks) : 
                inputAlignmentConstants.append(parmap.get('inputAlignmentConstants', ''))
                IBLDistDB.append(parmap.get('IBLDistDB', '/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/InnerDetector/InDetExample/InDetAlignExample/BowingDB/BowingNULL.db'))

        else:
            for x in xrange(nBlocks) :
                inputAlignmentConstants.append("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d_Block%02d.root" % (project, runnr, runnr, iteration-1, x))
                IBLDistDB.append("/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpIBLDistDB/%d/%d_IBLDistDB_Iter%d_Block%02d.db" % (project, runnr, runnr, iteration-1, x))
                    
        # output file
        outputAlignmentConstantsds = (parmap['outputAlignmentConstants']).split('#')[0]
        outputAlignmentConstants = (parmap['outputAlignmentConstants']).split('#')[1]
        outputAlignmentConstantsBlock = []
            
        # output db file
        outputAlignmentConstantsdbds = (parmap['outputAlignmentConstantsdb']).split('#')[0]
        outputAlignmentConstantsdb = (parmap['outputAlignmentConstantsdb']).split('#')[1]
        outputAlignmentConstantsdbBlock = []
        
        outputAlignFilesds = (parmap['outputAlignFiles']).split('#')[0]
        outputAlignFiles = (parmap['outputAlignFiles']).split('#')[1]
        outputAlignFilesBlock = []

        #JAVI: For all iterations multiple outputs are needed.
        for x in xrange(nBlocks) :  
            outputAlignmentConstantsBlock.append(outputAlignmentConstants.split('.Iter')[0] + '.Iter%d.Block%02d' % (iteration, x) + outputAlignmentConstants.split('.Iter')[1][1:])
            outputAlignmentConstantsdbBlock.append(outputAlignmentConstantsdb.split('.Iter')[0] + '.Iter%d.Block%02d' % (iteration, x) + outputAlignmentConstantsdb.split('.Iter')[1][1:])
            outputAlignFilesBlock.append(outputAlignFiles.split('.Iter')[0] + '.Iter%d.Block%02d' % (iteration, x) + outputAlignFiles.split('.Iter')[1][1:])


        # assemble jobOptions fragment
        retcode = {}
        outfiles = []
        infiles = []
        retcode_all = 0

        for x in xrange(nBlocks) :
            #JAVI: For all iterations we process each blockLBsize LB with different alignment constants, in a consecutive way
            #JAVI: as it is done one after other, processing time will increase by a factor totalLB/blockLBsize
            #JAVI: The processing time didn't that much at the end. This is good.

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
inputBowingDatabase = "%s"
inputDynamicGlobalDatabase = "%s"

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) != 0 :
    readConstantsFromPool = True
    inputPoolFiles = [inputAlignmentConstants]
else :
    readConstantsFromPool = False

WriteTFile    = True
runAccumulate = False
runSolving    = True

doReadBS       = False
doBSConstraint = False
doPVConstraint = False
doTrkNtuple    = False
doMonitoring   = False

Cosmics     = False
realData    = True
useTRT      = True
BField      = True
dataSource  = 'data' 
projectName = '%s'
PtCut       = 3000

applyLBibldistTweak = False
writeIBLDistDB = True

if projectName == "data15_hi":
    HeavyIons = True

#Alignment Method is GX2
runLocal      = False
solveLocal    = False
solvingOption =	1

## solving options
##   0 - No global solving
##   1 - Lapack
##   2 - ***REMOVED***
##   6 - ROOT
##   7 - CLHEP

# Temp configuration
ModCut = 0

detectorDescription = "ATLAS-R2-2015-04-00-00"
globalTag = "CONDBR2-BLKPA-2016-12"

siAlignmentTag   = "InDetAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016
trtAlignmentTag    = "TRTAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016

#---- New dynamic DB configuration
useDynamicAlignFolders = True                
writeDynamicDB = True

dynamicL1IDTag = "InDetAlignL1_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL2PIXTag = "InDetAlignL2PIX_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL2SCTTag = "InDetAlignL2SCT_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL3SiTag = "InDetAlign_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL1TRTag = "TRTAlignL1_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL2TRTTag = "TRTAlign_R2dynamic_Post_TS1_June2016_d0corrected" 

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

##-------- Load Reconstruction -----------------------------
import os
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
                ''' % (str(inputRAWFileList), str(inputfilenames[x]), inputAlignmentConstants[x], outputAlignmentConstantsBlock[x], IBLDistDB[x], IBLDistDB[x], project)
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
inputBowingDatabase = "%s"
inputDynamicGlobalDatabase = "%s"

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) != 0 :
    readConstantsFromPool = True
    inputPoolFiles = [inputAlignmentConstants]
else :
    readConstantsFromPool = False

WriteTFile    = True
runAccumulate = False
runSolving    = True

doReadBS       = False
doBSConstraint = False
doPVConstraint = False
doTrkNtuple    = False
doMonitoring   = False

Cosmics     = False
realData    = True
useTRT      = True
BField      = True
dataSource  = 'data' 
projectName = '%s'
PtCut       = 3000

applyLBibldistTweak = False
writeIBLDistDB = True

if projectName == "data15_hi":
    HeavyIons = True

#Alignment Method is GX2
runLocal      = False
solveLocal    = False
solvingOption =	1

## solving options
##   0 - No global solving
##   1 - Lapack
##   2 - ***REMOVED***
##   6 - ROOT
##   7 - CLHEP

# Temp configuration
ModCut = 0

detectorDescription = "ATLAS-R2-2015-04-00-00"
globalTag = "CONDBR2-BLKPA-2016-12"

siAlignmentTag   = "InDetAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016
trtAlignmentTag    = "TRTAlign_R2_Post_TS1_June2016_d0corrected" # since July 21, 2016

#---- New dynamic DB configuration
useDynamicAlignFolders = True                
writeDynamicDB = True

dynamicL1IDTag = "InDetAlignL1_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL2PIXTag = "InDetAlignL2PIX_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL2SCTTag = "InDetAlignL2SCT_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL3SiTag = "InDetAlign_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL1TRTag = "TRTAlignL1_R2dynamic_Post_TS1_June2016_d0corrected"
dynamicL2TRTTag = "TRTAlign_R2dynamic_Post_TS1_June2016_d0corrected" 

## Pixel Alignment Levels 
pixelAlignmentLevel = 16
pixelAlignmentLevelBarrel = -1
pixelAlignmentLevelEndcaps = -1

AlignIBLbutNotPixel=True

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


##-------- Load Reconstruction -----------------------------
import os
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
                ''' % (str(inputRAWFileList), str(inputfilenames[x]), inputAlignmentConstants[x], outputAlignmentConstantsBlock[x], IBLDistDB[x], IBLDistDB[x], project)#,beamspottag)

            jOFile.write(cont)
            jOFile.close()

            # run athena
            cmd = ""
            if svcclass : 
                cmd += "export STAGE_SVCCLASS=%s ; " % svcclass
            cmd += "python -u `which athena.py` myJobOptions.py"

            print "\nRun command:"
            print cmd
            print "\nLogfile of Block %d:" % x
            print "------------------------------------------------------------------"
            #JAVI: Creating a vector of retcodes
            retcode[x] = os.system(cmd)
            retcode_all += retcode[x]
            print "------------------------------------------------------------------"
            dt = int(time.time() - t0)
            print "\n## athena.py finished with retcode = %s" % retcode[x]
            print   "## ... elapsed time: ", dt, " sec"

            nevts = 0
                
            if retcode[x] == 0 :
                # Move alignment constants db file
                os.system("mv mycool.db %s" % outputAlignmentConstantsdbBlock[x])

                if iteration ==  0: 
                    os.system("mkdir -p /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d" % (project, runnr))
                    os.system("mkdir -p /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpIBLDistDB/%d" % (project, runnr))

                os.system("cp %s /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpconstants/%d/%d_AlignmentConstants_Iter%d_Block%02d.root" % (outputAlignmentConstantsBlock[x], project, runnr, runnr, iteration, x))
                #JAVI: Adding a IBLDistDB folder:
                os.system("cp %s /afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0/%s/tmpIBLDistDB/%d/%d_IBLDistDB_Iter%d_Block%02d.db" % (outputAlignmentConstantsdbBlock[x], project, runnr, runnr, iteration, x))
                    
                os.system("tar -cf %s *.txt *.root %s %s" % (outputAlignFilesBlock[x], outputAlignmentConstantsdbBlock[x], outputAlignmentConstantsBlock[x])) 
                
                # assemble report pickle file
                acronym = 'OK'
                txt = 'trf finished OK'  

                # get info for report gpickle file

                # one could determine the number of processed events here, e.g. by grep'ping in the logfile
                constantsmap = getFileMap(outputAlignmentConstantsBlock[x], outputAlignmentConstantsds, nevts=nevts)
                outfiles.append(constantsmap)
                constantsdbmap = getFileMap(outputAlignmentConstantsdbBlock[x], outputAlignmentConstantsdbds, nevts=nevts)
                outfiles.append(constantsdbmap)
                outputAlignFilesmap = getFileMap(outputAlignFilesBlock[x], outputAlignFilesds, nevts=nevts)
                outfiles.append(outputAlignFilesmap)

            else :
                print "ERROR: athena.py execution problem!"
                acronym = 'TRF_ATHENA_EXE'
                txt = "athena.py execution problem"

        if retcode_all == 0 :
            #JAVI: only do this if all the processes ended correctly
        
            acronym = 'OK'
            txt = 'trf finished OK'
 
            # extract original RAW input dataset name (accumulation step) from PFN 
            pfn = inputRAWFileList[0]           # e.g. '/castor/cern.ch/grid/atlas/.../data14_cos.00247688.physics_IDCosmic.merge.RAW._lb0552._SFO-ALL._0001.1' 
            lfn = pfn[pfn.rfind('/')+1:]        # e.g. 'data14_cos.00247688.physics_IDCosmic.merge.RAW._lb0552._SFO-ALL._0001.1' 
            inputdataset = lfn.split('._')[0]   # e.g. 'data14_cos.00247688.physics_IDCosmic.merge.RAW'
            # sign off next iteration through passing a message to TOM
            os.system("touch /afs/cern.ch/atlas/tzero/run/run2cafTOM/msgbox/idalignTOM.msg")
            os.system("echo \"call pdb updateDatasetPState %s aligned Iter%d\" >> /afs/cern.ch/atlas/tzero/run/run2cafTOM/msgbox/idalignTOM.msg" % (inputdataset, iteration))
                
            # send e-mail notification on completion
            if iteration == 3 :
                rc = sendStatusMail(runnr)
                rc2 = AddToListOfGoodRuns(runnr, project)
                if rc : 
                    print "INFO: e-mail notification failed!" 

        else: 
            acronym = 'TRF_ATHENA_EXE'
            txt = "athena.py execution problem"

        # assemble job report map
        #JAVI: not sure if I can pass a vector of retcodes or just one vale (retcode_all = sum of retcodes)
        outmap = { 'prodsys': { 'trfCode': retcode_all,
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
        print "        (number of current iteration; optional -- if not given, will be extracted from input dataset name) "
        print " "    
        sys.exit(-1)
    
    else :
        picklefile = sys.argv[1][len('--argdict='):]
        runAthena(picklefile)
