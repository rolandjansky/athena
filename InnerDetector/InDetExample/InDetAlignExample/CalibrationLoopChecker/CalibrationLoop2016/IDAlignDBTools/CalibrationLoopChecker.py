#!/usr/bin/python
# -*- coding:utf-8 -*-

################################################################################
#
# CalibrationLoopChecker.py
#
# Author: Hide Oide (Hideyuki.Oide@cern.ch), Matthias Danninger (Matthias.Danninger@cern.ch)
#
# Date: April 14, 2016
#
# Description
#
################################################################################


import sys, os, glob, subprocess, pprint
import datetime, time
from getpass import getpass
import hashlib

import tools
import MonitoringScriptsL11
import MonitoringScriptsL16

filebasename = ""
L11_filebasename =""

#----------------------------------------------------------------------------------------------------
def get_run_directory( RunNumber ):
    """
    The name of the project is uncertain at the beginning (i.e. data* )
    --> Using glob.glob(), list up the candidate directories.
        If the candidate is not unique, exits.
    """
    
    tools.write_log_debug("get_run_directory()")
    
    w0dir            = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0"
    Project          = "data*"
    Stream           = "calibration_IDTracks"
    RunNumber_format = "{0:08d}".format( RunNumber )
    
    rundirs = glob.glob( "/".join( [w0dir, Project, Stream, RunNumber_format] ) )
    if len( rundirs ) != 1 :
        tools.write_log_error("Unable to obtain the run directory.")
        return "RUN_NOT_FOUND"
        
    return rundirs[0]

#----------------------------------------------------------------------------------------------------
def get_raw_filelist( RunNumber ):

    tools.write_log_debug("get_raw_filelist()")
    
    w0dir            = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0"
    Project          = "data*"
    Stream           = "FileLists"
    RunNumber_format = "{0}".format( RunNumber )
    
    paths = glob.glob( "/".join( [w0dir, Project, Stream, RunNumber_format, "Run"+RunNumber_format+".txt" ] ) )
    if len( paths ) != 1 :
        tools.write_log_error("Unable to obtain the RAW file list.")
        return "RAW_FILE_LIST_NOT_FOUND"

    return paths[0]

#----------------------------------------------------------------------------------------------------
def get_latest_CLprocessID( directory ):
    
    tools.write_log_debug("get_latest_CLprocessID()")
    
    return list( set( [ os.path.splitext( directory+"/"+file )[1] for file in os.listdir( directory ) ] ) )[-1]


#----------------------------------------------------------------------------------------------------
def expand_tar_archive( file_fullpath ):
    
    tools.write_log_debug("expand_tar_archive()")
    
    print file_fullpath
    basename = os.path.basename( file_fullpath )
    if( os.path.exists( basename ) ):
        os.system( "rm -rf {0}".format( basename ) )
    os.system( "mkdir -p {0}".format( basename ) )
    os.system( "tar -C {0} -xf {1}".format( basename, file_fullpath ) )


#----------------------------------------------------------------------------------------------------
def complement_notGood_L16_segments( flag_array, results ):
    """
    This function patches the ill LB segment using other segment's result.
    The strategy is as follows:
    
    (1) If the previous segment exists, use it.
    (2) If the previous segment is unavailable and if the next segment exists, use the next segment.
    (3) If neither previous nor next segment is available, this means >300LBs are missing. ==> Tell that we should not update.
    """
    
    tools.write_log_debug("complement_notGood_L16_segments()")
    
    states = { "AllGood":0, "Patched":1, "NonRecoverable":2 }
    
    state = states["AllGood"]
    
    comment = ""
    
    
    """
    Loop over the segments
    """
    for flag, result, index in zip( flag_array, results, range(0, len(flag_array)) ):
        
        """
        Check if the result is good.
        If it is good, go to next.
        """
        
        if flag :
            comment += "Segment {0:02d} is Good.\n".format( index )
            continue
        
        
        """
        OK, there is some problem in this segment.
        Search if adjointing segments are availble.
        """
        
        if len( flag_array ) == 1:
            """
            Segment is singlet and non-recoverable
            """
            state = states["NonRecoverable"]
            comment += "Segment {0:02d} was non-recoverable.\n".format( index )
        
        elif index == 0:
            """
            Head segment --> check if the next segment is good.
            """
            
            if flag_array[ index+1 ] :
                results[ index ] = results[ index+1 ]
                state = states["Patched"]
                comment += "Segment {0:02d} was patched by Segment {1:02d}.\n".format( index, index+1 )
            else:
                state = states["NonRecoverable"]
                comment += "Segment {0:02d} was non-recoverable.\n".format( index )
            
        elif index == len( flag_array )-1:
            """
            Tail segment --> check if the previous segment is good.
            """

            if flag_array[ index-1 ]:
                results[ index ] = results[ index-1 ]
                state = states["Patched"]
                comment += "Segment {0:02d} was patched by Segment {1:02d}.\n".format( index, index-1 )
            else:
                state = states["NonRecoverable"]
                comment += "Segment {0:02d} was non-recoverable.\n".format( index )
        
        else:
            """
            Middle segment --> check previous first, then next
            """
    
            if flag_array[ index-1 ] :
                results[ index ] = results[ index-1 ]
                state = states["Patched"]
                comment += "Segment {0:02d} was patched by Segment {1:02d}.\n".format( index, index-1 )
            elif flag_array[ index+1 ] :
                results[ index ] = results[ index+1 ]
                state = states["Patched"]
                comment += "Segment {0:02d} was patched by Segment {1:02d}.\n".format( index, index+1 )
            else:
                state = states["NonRecoverable"]
                comment += "Segment {0:02d} was non-recoverable.\n".format( index )

    
    """
    Loop ended.
    """
    
    for line in comment.split("\n"):
        tools.write_log_info( "complement_notGood_L16_segments():: " + line )
    
    return state, comment
            
    
#----------------------------------------------------------------------------------------------------
def generate_L16_DbFile_for_UPD4( RunNumber, LBHeads, IBLDist_TXTfiles ):
    """
    Call set_ibldist.py to create the desired DB
    First, make sure no more old mycool.db is present
    """
    tools.write_log_debug("generate_L16_DbFile_for_UPD4()")
    
    DbFile_UPD4 = "Run{0}_L16_UPD4.db".format( RunNumber )

    if os.path.exists( DbFile_UPD4 ):
        os.system( "rm {0}".format( DbFile_UPD4 ) )
    
    for x in range(0, len(IBLDist_TXTfiles) ):
        if ( x == 0 and x == (len(IBLDist_TXTfiles)-1) ):
            """ Make valid from LB=0 until Inf """
            command = "./set_ibldist.py  CONDBR2 IndetIBLDist {0} {1} 0".format(IBLDist_TXTfiles[x], RunNumber)
            
        elif x == ( len(IBLDist_TXTfiles)-1 ):
            """ Make valid until Inf for last interval """
            command = "./set_ibldist.py  CONDBR2 IndetIBLDist {0} {1} {2} ".format(IBLDist_TXTfiles[x], RunNumber, LBHeads[x]+1)
            
        elif x == 0:
            """ Make valid from LB=0 for first interval """
            command = "./set_ibldist.py  CONDBR2 IndetIBLDist {0} {1} 0 {1} {2}".format(IBLDist_TXTfiles[x], RunNumber, LBHeads[x+1])
            
        else:
            """ All the normal cases for intervals in between first and last """
            command = "./set_ibldist.py  CONDBR2 IndetIBLDist {0} {1} {2} {1} {3}".format(IBLDist_TXTfiles[x], RunNumber, LBHeads[x]+1, LBHeads[x+1])
            
        tools.write_log_debug( command )
        os.system(command)
        
    command="AtlCoolCopy.exe " + \
            "\"sqlite://schema=mycool.db;dbname=CONDBR2\" " + \
            "\"sqlite://;schema=" + DbFile_UPD4 + ";dbname=CONDBR2\" " + \
            "-create -folder /Indet/IBLDist " + \
            "-outtag {0} -tl \"BLK UPD4 DB File from Run {1}\" " \
            .format( os.getenv("IBLDist_UPD4_TAG"), RunNumber )
    
    print""
    print""
    tools.write_log_debug( command )
    print"=========================================================="
    os.system( command )
    print"=========================================================="
        
    os.system( "rm mycool.db" )
    
    return  DbFile_UPD4


#----------------------------------------------------------------------------------------------------
def generate_L16_DbFile_for_UPD1( RunNumber, DbFile_UPD4 ):
    
    DbFile_UPD1 = "Run{0}_L16_UPD1.db".format( RunNumber )
    
    command = "AtlCoolCopy.exe " + \
              "\"sqlite://schema=" + DbFile_UPD4 +";dbname=CONDBR2\" " + \
              "\"sqlite://;schema="+ DbFile_UPD1 +";dbname=CONDBR2\" " + \
              "-create -folder /Indet/IBLDist " + \
              "-outtag {0} -go -tl \"ES1 UPD1 DB File from Run {1}\"" \
              .format( os.getenv("IBLDist_UPD1_TAG"), RunNumber )
    
    print""
    print""
    tools.write_log_debug( command )
    print"=========================================================="
    os.system( command )
    print"=========================================================="

    
    return DbFile_UPD1


#----------------------------------------------------------------------------------------------------
def check_and_update_L11_constant( RunNumber ):
    
    tools.write_log_debug("check_and_update_L11_constant()")
    
    tools.write_log_info("Checking the run directory exists for Run {0}...".format( RunNumber) )
    theRunDirectory = get_run_directory( RunNumber )
    if theRunDirectory == "RUN_NOT_FOUND":
        return theRunDirectory
    
    tools.write_log_info(" ==> Identified the Run directory: {0}".format( theRunDirectory) )
    
    """
    This processID stands for the suffix of the directory, like .c0, .c1
    """
    tools.write_log_info("Checking the process ID suffix...")
    processID = get_latest_CLprocessID( theRunDirectory )
    tools.write_log_info(" ==> processID suffix: {0}".format( processID ) )
    
    
    
    """
    Identify the tar archive file
    """
    tools.write_log_info("Fetching L11 alignlogfiles...")
    alignlogfiles = []
    
    for iteration in [0, 1]:
        tarfiles = glob.glob( "{0}/*TAR_ALIGNFILES.Iter{1}*{2}/*".format(theRunDirectory, iteration, processID) )
        if len( tarfiles ) == 1 :
            tarfile = tarfiles[0]
            workdir = os.path.basename( tarfile )
            
        elif len( tarfiles ) == 0 :
            tools.write_log_error("NULL files were found --> L11 alignment result is missing.")
            return "L11_RESULTS_NOT_FOUND"       
        else:
            print tarfiles
            tools.write_log_error("TAR_ALIGNFILES are not unique!!")
            return "L11_RESULTS_NOT_FOUND"
        
        
        """
        Expand the tar archive
        """
        expand_tar_archive( tarfile )
        
        
        alignlogfile = workdir + "/alignlogfile.txt"
        
        alignlogfiles.append( alignlogfile )
        
        if iteration == 1:
            DbFiles   = glob.glob( "{0}/*ROOT_DB*".format( workdir ) )
            if len( DbFiles ) == 1 :
                DbFile = DbFiles[0]
            else:
                tools.write_log_error("DB file is not unique!")
                return "L11_RESULTS_CORRUPTED"
                
        RootFiles   = glob.glob( "{0}/*ROOT_CONSTANTS*".format( workdir ) )
        if len( RootFiles ) == 1 :
            RootFile = RootFiles[0]
        else:
            tools.write_log_error("ROOT_DB file is not unique!")
            return "L11_RESULTS_CORRUPTED"
        
    
    tools.write_log_info(" ==> Succeeded in fetching alignlogfiles for L11.")
    
    """
    Check the quality of alignlogfile (monitoring)
    """
    tools.write_log_info("Checking the validity of the L11 result...")
    if not MonitoringScriptsL11.judge_L11_result_using_errors( RunNumber, alignlogfiles ):
        tools.write_log_error("L11 result is not good.")
        return "L11_RESULTS_NOT_GOOD"
    
    tools.write_log_info(" ==> L11 result is validated.")
    
    """
    Copy the Db file and ROOT (POOL) file temporarily
    """
    tools.write_log_info( "Good L11 constant is calculated. ==> Extract DB files.")
    tools.write_log_info("Retrieving .db and .root files...")
    
    global filebasename
    filebasename = workdir.replace("TAR_ALIGNFILES.", "")
    
    global L11_filebasename
    L11_filebasename = "Run{0}_L11".format( RunNumber )
    
    os.system( "cp {0} {1}.db"  .format( DbFile, L11_filebasename ) )
    tools.write_log_info("Copied L11 .db file  : {0}.db".format( L11_filebasename ) )
    
    os.system( "cp {0} {1}.root".format( RootFile, L11_filebasename ) )
    tools.write_log_info("Copied L11 .root file: {0}.root".format( L11_filebasename ) )
    
    tools.write_log_info(" ==> Preparation for L11 is done.")
    tools.write_log_info(" --------------------------------")
    
    return "L11_RESULTS_GOOD"
    



#----------------------------------------------------------------------------------------------------

L16_UPD4_DbFile = ""
L16_UPD1_DbFile = ""


def check_and_update_L16_constant( RunNumber ):
    
    tools.write_log_debug("check_and_update_L16_constant()")
    
    tools.write_log_debug("Checking the run directory exists for Run {0}...".format( RunNumber) )
    theRunDirectory = get_run_directory( RunNumber )
    if theRunDirectory == "RUN_NOT_FOUND":
        return theRunDirectory
    
    tools.write_log_debug(" ==> Identified the Run directory: {0}".format( theRunDirectory) )
    
    """
    This processID stands for the suffix of the directory, like .c0, .c1
    """
    tools.write_log_debug("Checking the process ID suffix...")
    processID = get_latest_CLprocessID( theRunDirectory )
    tools.write_log_debug(" ==> processID suffix: {0}".format( processID ) )
    
    
    tools.write_log_info("Fetching L16 alignlogfiles...")
    """
    Define list variables
    - alignlogfiles: container of alignlogfile. This is supposed to store the alignlogfile as follows:
      [ [Segment0_Iter2, Segment0_Iter3],
        [Segment1_Iter2, Segment1_Iter3],
        [Segment2_Iter2, Segment2_Iter3], ... ]
    """
    alignlogfiles = []
    IBLDist_TXTfiles = []
    
    """
    Check the number of L16 segments in the run
    """
    listTarIter3 = glob.glob( "{0}/*TAR_ALIGNFILES.Iter3*{1}/*".format(theRunDirectory, processID) )
    for tar in listTarIter3:
        alignlogfiles.append( [] )

    """
    Loop over Iter2 and Iter3
    """
    for iteration in [2, 3]:
        
        """
        List-up the L16 TAR files of the corresponding iteration.
        Needs sorted() in order to obtain sorted list.
        """
        tarfiles = sorted( glob.glob( "{0}/*TAR_ALIGNFILES.Iter{1}*{2}/*".format(theRunDirectory, iteration, processID) ) )
        
        """
        Loop over each TAR file
        """
        for tarfile in tarfiles:
            workdir = os.path.basename( tarfile )
            
            """
            Expand the tar archive
            """
            expand_tar_archive( tarfile )
        
            alignlogfile = workdir + "/alignlogfile.txt"
            
            alignlogfiles[ tarfiles.index(tarfile) ].append( alignlogfile )
            
            """
            Copy the .db and .root file from the TAR directory
            (only in the case if the last Iter)
            """
            if iteration == 3:

                IBLDist_TXTfile = workdir + "/OutputIBLDist.txt"

                IBLDist_TXTfiles.append( IBLDist_TXTfile )

    
    tools.write_log_info(" ==> Succeeded in fetching alignlogfiles for L11.")
    print""
    print""
    print"=========================================================="
    print"List of alignlog files for LB-segment"
    print"=========================================================="
    pprint.pprint( alignlogfiles )
    
    print""
    print""
    print"=========================================================="
    print"List of IBLDIst files for LB-segment"
    print"=========================================================="
    pprint.pprint( IBLDist_TXTfiles )

    """
    Get the RAW data file list and create LB segments
    """
    LBlist = []
    
    tools.write_log_info("Fetching the RAW file list...")
    filelist = get_raw_filelist( RunNumber )
    if get_raw_filelist == "RAW_FILE_LIST_NOT_FOUND":
        return get_raw_filelist
    
    for line in open( filelist ):
        """
        extract LB number from the filename like
        data15_13TeV.00284484.calibration_IDTracks.merge.RAW._lb0022._SFO-ALL._0001.1
        """
        
        LB = int( line.split(".")[-4][-4:] )
        LBlist.append( LB )
    
    LBlist = sorted( LBlist )
    
    tools.write_log_info(" ==> The RAW file was fetched. Now calculating LB-segments...")
    LBHeads = filter( (lambda LB: LBlist.index(LB)%100 == 59 and (LBlist[-1] - LB) > 100), LBlist )
    if len( LBHeads ) == 0:
        LBHeads = filter( (lambda LB: LBlist.index(LB)%100 == 59 ), LBlist )

    
    print""
    print""
    print"=========================================================="
    print"List of LB segments"
    print"=========================================================="
    pprint.pprint( LBHeads )
    
    tools.write_log_info("LB-segments were calculated: {0}".format( LBHeads ) )
    
    
    """
    Now, alignlogfiles lists up a pair of alignlogfiles (Iter2, Iter3) for each LB-segment.
    Check these results using the following macro.
    
    isGood_L16 is just a simple boolean to see the overall validity,
    L16_segment_flags is an array of booleans [ T/F, T/F, .... ]
    corresponding to each LB-segment.
    """
    tools.write_log_info("Checking the validity of the L16 result...")
    MonitoringScriptsL16.gen_L16_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles )
    isGood_L16, L16_segment_flags = MonitoringScriptsL16.judge_L16_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles )
    
    
    """
    If isGood_L16 is False, some treatments are needed to fill the missing LB-segment piece.
    """
    if not isGood_L16:
        tools.write_log_info("Some of L16 results are not good - check if they are recoverable...")
        state, comment = complement_notGood_L16_segments( L16_segment_flags, IBLDist_TXTfiles )
        
        print""
        print""
        print"=========================================================="
        print"Diagnosis and Patching Treatment Results"
        print"=========================================================="
        print comment
        print"=========================================================="
        
        print""
        print""
        print"Modified List of L16 alignment data"
        print"=========================================================="
        pprint.pprint( IBLDist_TXTfiles )
        
        """
        Do not proceed if the state of the complementing is not good.
        """
        if state == 2:
            print""
            print""
            print"=========================================================="
            print"Could not patch the L16 segments (Non-recoverable Data)"
            print"==> Give up."
            print"=========================================================="
            
            tools.write_log_error("L16 result is not recoverable.")
            return "L16_RESULT_NOT_RECOVERABLE"
        else:
            tools.write_log_info("Some of L16 results are recovered.")

    
    
    tools.write_log_info(" ==> L16 result is validated.")
    print""
    print""
    print"=========================================================="
    print""
    print"==> Proceed to prepare for .db files"
    print""
    print""
    
    tools.write_log_info(" ==> Proceed to prepare for .db files")
    """
    First create UPD4 (for BLK)
    """
    global L16_UPD4_DbFile
    L16_UPD4_DbFile = generate_L16_DbFile_for_UPD4( RunNumber, LBHeads, IBLDist_TXTfiles )
    tools.write_log_info("Generated L16 BLK-UPD4 DB File: {0}".format( L16_UPD4_DbFile ) )
    
    """
    Second create UPD1 (for ES1) using L16_UPD4_DbFile
    """
    global L16_UPD1_DbFile
    L16_UPD1_DbFile = generate_L16_DbFile_for_UPD1( RunNumber, L16_UPD4_DbFile )
    tools.write_log_info("Generated L16 ES1-UPD1 DB File: {0}".format( L16_UPD1_DbFile ) )    
    
    tools.write_log_info(" ==> Preparation for L16 is done.")
    tools.write_log_info(" --------------------------------")
    
    return "L16_RESULT_GOOD"



#----------------------------------------------------------------------------------------------------
def upload_constants( RunNumber, user ):
    
    global L11_filebasename
    
    tools.write_log_info("Submitting constants under authentication of {0}".format( user ) )
    
    """
    Call ConstantsUploaderSimple.py for uploading L11 constants
    """
    tools.write_log_info( "Uploading for L11" )
    
    command = "python ConstantsUploaderSimple.py -r {0} -f {1} --InDetUPD1 {2} --InDetUPD4 {3} --TRTUPD1 {4} --TRTUPD4 {5}" \
               .format( RunNumber, L11_filebasename, \
                        os.getenv("IndetAlign_UPD1_TAG"), \
                        os.getenv("IndetAlign_UPD4_TAG"), \
                        os.getenv("TRTAlign_UPD1_TAG"), \
                        os.getenv("TRTAlign_UPD4_TAG"),     )
    
    
    if os.getenv("exec") == "--exec" :
        command += " --exec"
        tools.write_log_debug( command )
        ret = os.system( command )
        if( ret != 0 ) :
            tools.write_log_error("ConstantsUploaderSimple.py failed (return code {0}).".format( ret ) )
            tools.write_log_info("main:: Terminated Process: Failed in ConstatnsUploaderSimple.py for L11" )
            tools.save_log( RunNumber )
            os.system( "rm -r data*{0}*".format( RunNumber ) )
            sys.exit()
        else:
            tools.write_log_info("ConstantsUploaderSimple.py is done." )
    else:
        tools.write_log_warning( "Dry Run : Constants are not uploaded" )
        
    
    """
    The IBLDist upload still needs to be implemented here!!
    Filenames are stored in the following global variables.
    """
    
    tools.write_log_info( "Uploading for L16" )
    
    global L16_UPD4_DbFile
    global L16_UPD1_DbFile

    """ First the upload of the UPD1 """ 
    command_upd1 = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment \" ES1 UPD1 DB File from Run {0} \" --tag {1} --passopt='-cti' --folder /Indet/IBLDist {2} CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1".format( RunNumber, os.getenv("IBLDist_UPD1_TAG"), L16_UPD1_DbFile )

    """ Second the upload of the UPD4 """
    command_upd4 = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment \" BLK UPD4 DB File from Run {0} \" --tag {1} --rs {0} --passopt='-cti' --folder /Indet/IBLDist {2} CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1".format( RunNumber, os.getenv("IBLDist_UPD4_TAG"), L16_UPD4_DbFile )
    
    
    if os.getenv("exec") == "--exec" :
        #tools.write_log_info( command_upd1 )
        #tools.write_log_info( command_upd4 )
        tools.write_log_info("Uploading L16 constants for UPD1..." )
        os.system( command_upd1 )  
        
        tools.write_log_info("Uploading L16 constants for UPD4..." )
        os.system( command_upd4 )  
        
        tools.write_log_info("Uploading L16 constants is done." )
    else:
        tools.write_log_warning( "Dry Run : Constants are not uploaded" )
        
    
    logfile=open("/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log/CalibLoopChecker_log.txt", "a")
    now = datetime.datetime.now()
    logfile.write( "{0} - {1} - {2}\n".format( now.strftime("%Y-%m-%d-%H:%M:%S"), int(time.mktime(now.timetuple())), RunNumber ) )
    logfile.close()
    
    
    if os.getenv("exec") == "--exec" :
        handshake_command = "./HandShake.py {0} {1} {2} --exec".format( RunNumber, os.getenv("HandshakeStream"), user )
        tools.write_log_info( handshake_command )
        os.system( handshake_command )
        tools.write_log_info( "Handshake is done." )
    

    
#----------------------------------------------------------------------------------------------------
def upload_constants_manual( RunNumber ):
    print "\n\n\n-----------------------------------------------------------------------------------\n\n"
    while True:
        print "\n\nPreparation of constants are done.\n\n\n"
        print "You are going to upload constants to the following tags:"
        print ""
        print "/Indet/Align    -  ES1-UPD1:   {0}".format( os.getenv("IndetAlign_UPD1_TAG") )
        print "/Indet/Align    -  BLK-UPD4:   {0}".format( os.getenv("IndetAlign_UPD4_TAG") )
        print ""
        print "/TRT/Align      -  ES1-UPD1:   {0}".format( os.getenv("TRTAlign_UPD1_TAG") )
        print "/TRT/Align      -  BLK-UPD4:   {0}".format( os.getenv("TRTAlign_UPD4_TAG") )
        print ""
        print "/Indet/IBLDist  -  ES1-UPD1:   {0}".format( os.getenv("IBLDist_UPD1_TAG") )
        print "/Indet/IBLDist  -  BLK-UPD4:   {0}".format( os.getenv("IBLDist_UPD4_TAG") )
        print "\n\n\n-----------------------------------------------------------------------------------\n\n"
        
        proceed = raw_input("Do you really want to upload the constants? [y/q]: ")
        if proceed == "y": break
        elif proceed == "q":
            tools.write_log_warning( " ==> Submission is canceled." )
            return
        
    while True:
        proceed = raw_input("Are you really sure? [yes/no]: ")
        if proceed == "yes": break
        elif proceed == "no":
            tools.write_log_warning( " ==> Submission is canceled." )
            return
        
    n_failure = 0
    while True:
        name = raw_input("Enter your name: ")
        password = getpass("Input submission password: ")
        m = hashlib.md5()
        m.update( password )
        if( m.hexdigest() == "d192157084949cc99b50b9a028038d8f" ):
            upload_constants( RunNumber, name )
            break
        elif n_failure == 3:
            tools.write_log_warning( " ==> Failed in authentication. Take a coffee break, and try again :-)" )
            break
        else:
            n_failure+=1

#----------------------------------------------------------------------------------------------------
# main
if __name__ == "__main__":
    
    from ROOT import gROOT
    gROOT.SetBatch( True )
    
    RunNumber = int( sys.argv[1] )
    
    tools.write_log_info("===========================================================================")
    tools.write_log_info("")
    tools.write_log_info( "main:: Started Process for Run {0}.".format( RunNumber ) )
    tools.write_log_info("")
    tools.write_log_info("===========================================================================")
    
    tools.write_log_info("IndetAlign_UPD1_TAG = {0}".format( os.getenv("IndetAlign_UPD1_TAG") ) )
    tools.write_log_info("IndetAlign_UPD4_TAG = {0}".format( os.getenv("IndetAlign_UPD4_TAG") ) )
    tools.write_log_info("TRTAlign_UPD1_TAG = {0}".format( os.getenv("TRTAlign_UPD1_TAG") ) )
    tools.write_log_info("TRTAlign_UPD4_TAG = {0}".format( os.getenv("TRTAlign_UPD4_TAG") ) )
    tools.write_log_info("IBLDist_UPD1_TAG = {0}".format( os.getenv("IBLDist_UPD1_TAG") ) )
    tools.write_log_info("IBLDist_UPD4_TAG = {0}".format( os.getenv("IBLDist_UPD4_TAG") ) )
    tools.write_log_info("===========================================================================")
    tools.write_log_info("run_mode = {0} {1}".format( os.getenv("run_mode"), os.getenv("exec") ) )
    tools.write_log_info("DEBUG = {0}".format( os.getenv("DEBUG") ) )
    tools.write_log_info("===========================================================================")
    
    
    

    L11_result = check_and_update_L11_constant( RunNumber )
    tools.write_log_info(L11_result)
    
    if L11_result == "L11_RESULTS_GOOD":
        L16_result = check_and_update_L16_constant( RunNumber )
        tools.write_log_info(L16_result)
        
        if L16_result != "L16_RESULT_GOOD":
            tools.write_log_info("main:: Terminated Process: {0}".format( L16_result) )
            tools.save_log( RunNumber )
            os.system( "rm -r data*{0}*".format( RunNumber ) )
            sys.exit()
    else:
        tools.write_log_info(L11_result)
        tools.write_log_info("main:: Terminated Process: {0}".format( L11_result ) )
        tools.save_log( RunNumber )
        os.system( "rm -r data*{0}*".format( RunNumber ) )
        sys.exit()


    """
    If the script comes to this line, all preparations before submitting DB files are done.
    """
    
    if os.getenv("run_mode") == "CRONJOB":
        upload_constants( RunNumber, "CRONJOB" )
    
    elif os.getenv("run_mode") == "MANUAL":
        upload_constants_manual( RunNumber )
    
    else:
        tools.write_log_info("Dry Run ==> Uploading is skipped")
    
        
        
    """
    Cleanup
    """
    os.system( "rm -r data*{0}*".format( RunNumber ) )
    os.system( "mv Run{0}*.* dbfiles".format( RunNumber ) )

    tools.write_log_info("===========================================================================")
    tools.write_log_info("")
    tools.write_log_info("main:: Completed Process Successfully.")
    tools.write_log_info("")
    tools.write_log_info("===========================================================================")
    tools.save_log( RunNumber )
