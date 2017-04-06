#!/usr/bin/env python2.7
# -*- coding:utf-8 -*-

################################################################################
#
# CalibrationLoopChecker.py
# -- Dynamic Edition --
#
# Author: Hide Oide (Hideyuki.Oide@cern.ch)
#
# Date: August 23, 2016
# Last Update: November 11, 2016
#
# Description
# This script was used for updating the DB in November 2016
# for the entire 2015+2016 pp collision dataset.
#
################################################################################


import sys, os, glob, subprocess, pprint, traceback
import datetime, time
from getpass import getpass
import hashlib

import tools
import MonitoringScriptsL11
import MonitoringScriptsL16

filebasename = ""

#----------------------------------------------------------------------------------------------------
def terminate(message, status="SUCCESS" ):
    
    global RunNumber
    
    tools.write_log_info("===========================================================================")
    tools.write_log_info("")
    tools.write_log_info("main:: {0}".format( message ) )
    tools.write_log_info("")
    tools.write_log_info("===========================================================================")
    tools.save_log( RunNumber, status )
    
    sys.exit()

#----------------------------------------------------------------------------------------------------
def cleanup():
    
    global RunNumber
    
    tools.write_log_info("cleanup")
    os.system( "rm *.xml".format( RunNumber ) )
    os.system( "rm -r data*{0}*".format( RunNumber ) )
    os.system( "mv -f Run{0}*.{{root,db}} dbfiles".format( RunNumber ) )
    

#----------------------------------------------------------------------------------------------------
def opening_message():
    
    global RunNumber
    
    tools.write_log_info("===========================================================================")
    tools.write_log_info("")
    tools.write_log_info("CalibrationLoopChecker - Dynamic Edition for Reprocessing")
    tools.write_log_info("main:: Started Process for Run {0}.".format( RunNumber ) )
    if os.getenv("IOVSinceRunNumber") != None:
        tools.write_log_info("main:: will cover since Run {0}.".format( os.getenv("IOVSinceRunNumber") ) )
    if os.getenv("IOVUntilRunNumber") != None:
        tools.write_log_info("main:: will cover until Run {0}.".format( os.getenv("IOVUntilRunNumber") ) )
    tools.write_log_info("")
    tools.write_log_info("===========================================================================")
    tools.write_log_info("run_mode            = {0} {1}".format( os.getenv("run_mode"), os.getenv("exec") ) )
    tools.write_log_info("do_upload_dynamic   = {0}".format( os.getenv("do_upload_dynamic") ) )
    tools.write_log_info("do_upload_static    = {0}".format( os.getenv("do_upload_static" ) ) )
    tools.write_log_info("do_upload_reprocess = {0}".format( os.getenv("do_upload_reprocess") ) )
    tools.write_log_info("DEBUG               = {0}".format( os.getenv("DEBUG") ) )
    if( os.getenv("do_upload_dynamic") == "1" ):
        tools.write_log_info("===========================================================================")
        tools.write_log_info("Dynamic Alignment Folder Tags")
        tools.write_log_info("IndetAlignL1ID_UPD1_TAG  = {0}" .format( os.getenv("IndetAlignL1ID_UPD1_TAG") ) )
        tools.write_log_info("IndetAlignL1ID_UPD4_TAG  = {0}" .format( os.getenv("IndetAlignL1ID_UPD4_TAG") ) )
        tools.write_log_info("IndetAlignL2PIX_UPD1_TAG = {0}" .format( os.getenv("IndetAlignL2PIX_UPD1_TAG") ) )
        tools.write_log_info("IndetAlignL2PIX_UPD4_TAG = {0}" .format( os.getenv("IndetAlignL2PIX_UPD4_TAG") ) )
        tools.write_log_info("IndetAlignL2SCT_UPD1_TAG = {0}" .format( os.getenv("IndetAlignL2SCT_UPD1_TAG") ) )
        tools.write_log_info("IndetAlignL2SCT_UPD4_TAG = {0}" .format( os.getenv("IndetAlignL2SCT_UPD4_TAG") ) )
        tools.write_log_info("IndetAlignL3_UPD1_TAG    = {0}" .format( os.getenv("IndetAlignL3_UPD1_TAG") ) )
        tools.write_log_info("IndetAlignL3_UPD4_TAG    = {0}" .format( os.getenv("IndetAlignL3_UPD4_TAG") ) )
        tools.write_log_info("IndetIBLDist_UPD1_TAG    = {0}" .format( os.getenv("IndetIBLDist_UPD1_TAG") ) )
        tools.write_log_info("IndetIBLDist_UPD4_TAG    = {0}" .format( os.getenv("IndetIBLDist_UPD4_TAG") ) )
        tools.write_log_info("TRTAlignL1_UPD1_TAG      = {0}" .format( os.getenv("TRTAlignL1_UPD1_TAG") ) )
        tools.write_log_info("TRTAlignL1_UPD4_TAG      = {0}" .format( os.getenv("TRTAlignL1_UPD4_TAG") ) )
        tools.write_log_info("TRTAlignL2_UPD1_TAG      = {0}" .format( os.getenv("TRTAlignL2_UPD1_TAG") ) )
        tools.write_log_info("TRTAlignL2_UPD4_TAG      = {0}" .format( os.getenv("TRTAlignL2_UPD4_TAG") ) )
    if( os.getenv("do_upload_static") == "1" ):
        tools.write_log_info("===========================================================================")
        tools.write_log_info("Static Alignment Folder Tags")
        tools.write_log_info("IndetAlign_UPD1_TAG = {0}" .format( os.getenv("IndetAlign_UPD1_TAG") ) )
        tools.write_log_info("IndetAlign_UPD4_TAG = {0}" .format( os.getenv("IndetAlign_UPD4_TAG") ) )
        tools.write_log_info("TRTAlign_UPD1_TAG   = {0}" .format( os.getenv("TRTAlign_UPD1_TAG") ) )
        tools.write_log_info("TRTAlign_UPD4_TAG   = {0}" .format( os.getenv("TRTAlign_UPD4_TAG") ) )
    if( os.getenv("do_upload_reprocess") == "1" ):
        tools.write_log_info("===========================================================================")
        tools.write_log_info("Reprocessc Alignment Folder Tags")
        tools.write_log_info("IndetAlignL1ID_UPD3_TAG  = {0}" .format( os.getenv("IndetAlignL1ID_UPD3_TAG") ) )
        tools.write_log_info("IndetAlignL2PIX_UPD3_TAG = {0}" .format( os.getenv("IndetAlignL2PIX_UPD3_TAG") ) )
        tools.write_log_info("IndetAlignL2SCT_UPD3_TAG = {0}" .format( os.getenv("IndetAlignL2SCT_UPD3_TAG") ) )
        tools.write_log_info("IndetIBLDist_UPD3_TAG    = {0}" .format( os.getenv("IndetIBLDist_UPD3_TAG") ) )
        tools.write_log_info("TRTAlignL1_UPD3_TAG      = {0}" .format( os.getenv("TRTAlignL1_UPD3_TAG") ) )
    tools.write_log_info("===========================================================================")


#----------------------------------------------------------------------------------------------------
def external_command( command, failure_message="" ):
    
    global RunNumber
    
    process = subprocess.Popen( command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    
    stdout = process.communicate()
    
    if process.returncode != 0:
        
        tools.write_log_error( command )
        tools.write_log_error( "" )
        tools.write_log_error( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> stdout >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" )
        for line in stdout:
            if line.find("\n")>=0:
                for line_split in line.split("\n"):
                    tools.write_log_error( line_split )
            else:
                tools.write_log_error( line )
        tools.write_log_error( "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< stdout <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" )
        tools.write_log_error( "" )
        
        raise Exception( failure_message )
    
    return process.returncode


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
    Stream           = "*"
    RunNumber_format = "{0:08d}".format( RunNumber )
    
    tools.write_log_debug(  "/".join( [w0dir, Project, Stream, RunNumber_format] ) )
    rundirs = glob.glob( "/".join( [w0dir, Project, Stream, RunNumber_format] ) )
    
    sel_index = 0
    if len( rundirs ) == 0 :
        tools.write_log_error("Unable to obtain the run directory.")
        return "RUN_NOT_FOUND"
    elif len( rundirs ) > 1:
        tools.write_log_warning("Multiple candidates found:")
        for rundir in rundirs:
            tools.write_log_warning( "[{0}] : {1}".format( rundirs.index( rundir ), rundir)  )
        sel_index = int( raw_input("Which dir to pick? : ") )
        
    return rundirs[sel_index]

#----------------------------------------------------------------------------------------------------
def get_raw_filelist( RunNumber ):

    tools.write_log_debug("get_raw_filelist()")
    
    w0dir            = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/Tier0_TEST"
    Project          = "data*"
    Stream           = "FileLists"
    RunNumber_format = "{0}".format( RunNumber )
    
    tools.write_log_debug( "/".join( [w0dir, Project, Stream, RunNumber_format, "Run"+RunNumber_format+".txt" ] ) )
    paths = glob.glob( "/".join( [w0dir, Project, Stream, RunNumber_format, "Run"+RunNumber_format+".txt" ] ) )
    if len( paths ) < 1 :
        tools.write_log_error("Unable to obtain the RAW file list.")
        tools.write_log_debug( "/".join( [w0dir, Project, Stream, RunNumber_format, "Run"+RunNumber_format+".txt" ] ) )
        return "RAW_FILE_LIST_NOT_FOUND"

    global projectName
    projectName = paths[-1].split("/")[-4]
    return paths[-1]

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
    external_command( "tar -C {0} -xf {1}".format( basename, file_fullpath ), "EXPAND_TAR_ARCHIVE" )


#----------------------------------------------------------------------------------------------------
def get_map_folder_tag_dynamic():
    map_folder_tag_dynamic = {
        #("/Indet/AlignL3",     "UPD1")  : os.getenv("IndetAlignL3_UPD1_TAG"),
        #("/Indet/AlignL3",     "UPD4")  : os.getenv("IndetAlignL3_UPD4_TAG"),
        #("/TRT/AlignL2",       "UPD1")  : os.getenv("TRTAlignL2_UPD1_TAG"),
        #("/TRT/AlignL2",       "UPD4")  : os.getenv("TRTAlignL2_UPD4_TAG"),
        ("/Indet/AlignL1/ID",  "UPD1")  : os.getenv("IndetAlignL1ID_UPD1_TAG"),
        ("/Indet/AlignL1/ID",  "UPD4")  : os.getenv("IndetAlignL1ID_UPD4_TAG"),
        ("/Indet/AlignL2/PIX", "UPD1")  : os.getenv("IndetAlignL2PIX_UPD1_TAG"),
        ("/Indet/AlignL2/PIX", "UPD4")  : os.getenv("IndetAlignL2PIX_UPD4_TAG"),
        ("/Indet/AlignL2/SCT", "UPD1")  : os.getenv("IndetAlignL2SCT_UPD1_TAG"),
        ("/Indet/AlignL2/SCT", "UPD4")  : os.getenv("IndetAlignL2SCT_UPD4_TAG"),
        ("/Indet/IBLDist",     "UPD1")  : os.getenv("IndetIBLDist_UPD1_TAG"),
        ("/Indet/IBLDist",     "UPD4")  : os.getenv("IndetIBLDist_UPD4_TAG"),
        ("/TRT/AlignL1/TRT",   "UPD1")  : os.getenv("TRTAlignL1_UPD1_TAG"),
        ("/TRT/AlignL1/TRT",   "UPD4")  : os.getenv("TRTAlignL1_UPD4_TAG"),
        }
    return map_folder_tag_dynamic
        
#----------------------------------------------------------------------------------------------------
def get_map_folder_tag_static():
    map_folder_tag_static = {
        ("/Indet/Align",       "UPD1")  : os.getenv("IndetAlign_UPD1_TAG"),
        ("/Indet/Align",       "UPD4")  : os.getenv("IndetAlign_UPD4_TAG"),
        ("/TRT/Align",         "UPD1")  : os.getenv("TRTAlign_UPD1_TAG"),
        ("/TRT/Align",         "UPD4")  : os.getenv("TRTAlign_UPD4_TAG")
        }
    return map_folder_tag_static


#----------------------------------------------------------------------------------------------------
def get_map_folder_tag_reprocess():
    map_folder_tag_reprocess = {}
    map_folder_tag_reprocess = {
        ("/Indet/AlignL1/ID",  "UPD3")  : os.getenv("IndetAlignL1ID_UPD3_TAG"),
        ("/Indet/AlignL2/PIX", "UPD3")  : os.getenv("IndetAlignL2PIX_UPD3_TAG"),
        ("/Indet/AlignL2/SCT", "UPD3")  : os.getenv("IndetAlignL2SCT_UPD3_TAG"),
        ("/Indet/IBLDist",     "UPD3")  : os.getenv("IndetIBLDist_UPD3_TAG"),
        ("/TRT/AlignL1/TRT",   "UPD3")  : os.getenv("TRTAlignL1_UPD3_TAG"),
        }
    return map_folder_tag_reprocess
        
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
def gen_modified_LBHeads( LBHeads, merged_validity ):
    modified_LBHeads = []
    
    for pair in enumerate( zip( LBHeads, merged_validity ) ):
        index    = pair[0]
        LBHead   = pair[1][0]
        validity = pair[1][1]
        if validity == True:
            previous_validities = merged_validity[0:index]
            previous_validities.reverse()
            last_valid_index = index - previous_validities.index( True ) if True in previous_validities else -1
        else:
            last_valid_index = -2
        
        if last_valid_index == -2:
            modified_LBHeads += [ -1 ]
        elif last_valid_index == -1:
            modified_LBHeads += [ 0 ]
        else:
            modified_LBHeads += [ LBHeads[ last_valid_index ] ]
    
    tools.write_log_info("Modified LB-segments were calculated: {0}".format( modified_LBHeads ) )
    return modified_LBHeads


#----------------------------------------------------------------------------------------------------
def generate_DbFile_for_UPD4( RunNumber, LBHeads, DbFiles ):
    """
    Call set_ibldist.py to create the desired DB
    First, make sure no more old mycool.db is present
    """
    tools.write_log_debug("generate_L16_DbFile_for_UPD4()")
    
    DbFile_UPD4 = "Run{0}_UPD4.db".format( RunNumber )
    
    global LBlist, L11_validity, L16_validity
    
    merged_validity = [ (L11 and L16) for L11, L16 in zip( L11_validity, L16_validity ) ]
    
    tools.write_log_info("Merged Block validity: {0}".format( merged_validity ) )
    
    modified_LBHeads = gen_modified_LBHeads( LBHeads, merged_validity )
    
    map_folder_tag_dynamic = get_map_folder_tag_dynamic()

    if os.path.exists( DbFile_UPD4 ):
        os.system( "rm {0}".format( DbFile_UPD4 ) )
    
    for index in range(0, len(modified_LBHeads)):
        
        if not merged_validity[index]: continue
        
        inputDB = DbFiles[index]
        outputDB = DbFile_UPD4
        
        for folder_tuple, tag in sorted( map_folder_tag_dynamic.iteritems() ):
            folder = folder_tuple[0]
            upd    = folder_tuple[1]
            
            if( upd != "UPD4" ): continue
            
            if index == 0 and index == len(modified_LBHeads)-1 :
                lbsince = 0
                command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                               "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                               " -nrls {3} {4} " + \
                               " -outtag {5} " + \
                               " -tl \"Dynamic UPD4 DB File from Run {3}\"") \
                               .format( inputDB, outputDB, folder, RunNumber, lbsince, tag )
            
            elif index == 0:
                lbsince = 0
                lbuntil = modified_LBHeads[index+1]
                command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                               "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                               " -nrls {3} {4} -nrlu {3} {5} " + \
                               " -outtag {6} " + \
                               " -tl \"Dynamic UPD4 DB File from Run {3}\"") \
                               .format( inputDB, outputDB, folder, RunNumber, lbsince, lbuntil, tag )
                
            elif index == len(modified_LBHeads)-1:
                lbsince = modified_LBHeads[index]
                command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                               "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                               " -nrls {3} {4} " + \
                               " -outtag {5} " + \
                               " -tl \"Dynamic UPD4 DB File from Run {3}\"") \
                               .format( inputDB, outputDB, folder, RunNumber, lbsince, tag )
                
            else:
                lbsince = modified_LBHeads[index]
                lbuntil = modified_LBHeads[index+1]
                command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                               "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                               " -nrls {3} {4} -nrlu {3} {5} " + \
                               " -outtag {6} " + \
                               " -tl \"Dynamic UPD4 DB File from Run {3}\"") \
                               .format( inputDB, outputDB, folder, RunNumber, lbsince, lbuntil, tag )
            
            tools.write_log_debug( command )
            external_command( command, "ATLCOOLCOPY_DYNAMIC_UPD4" )
        
    return  DbFile_UPD4


#----------------------------------------------------------------------------------------------------
def generate_DbFile_for_UPD1( RunNumber, representative_LB_index, DbFiles ):
    
    DbFile_UPD1 = "Run{0}_UPD1.db".format( RunNumber )
    
    map_folder_tag_dynamic = get_map_folder_tag_dynamic()
    
    for folder_tuple, tag in sorted( map_folder_tag_dynamic.iteritems() ):
        folder = folder_tuple[0]
        upd    = folder_tuple[1]
        
        if( upd != "UPD1" ): continue
        
        command = ("AtlCoolCopy.exe " + \
                       "\"sqlite://schema=" + DbFiles[representative_LB_index] +";dbname=CONDBR2\" " + \
                       "\"sqlite://;schema="+ DbFile_UPD1 +";dbname=CONDBR2\" " + \
                       " -create -f {1} -outtag {2}" + \
                       " -go -tl \"Dynamic UPD1 DB File from Run {0}\"") \
                       .format( RunNumber, folder, tag )
        
        tools.write_log_debug( command )
        external_command( command, "ATLCOOLCOPY_DYNAMIC_UPD1" )

    
    return DbFile_UPD1

#----------------------------------------------------------------------------------------------------
def generate_DbFile_for_UPD3( RunNumber, LBHeads, DbFiles ):
    """
    Call set_ibldist.py to create the desired DB
    First, make sure no more old mycool.db is present
    """
    tools.write_log_debug("generate_L16_DbFile_for_UPD3()")
    
    DbFile_UPD3 = "Run{0}_UPD3.db".format( RunNumber )
    
    global LBlist, L11_validity, L16_validity
    
    merged_validity = [ (L11 and L16) for L11, L16 in zip( L11_validity, L16_validity ) ]
    
    tools.write_log_info("Merged Block validity: {0}".format( merged_validity ) )
    
    modified_LBHeads = gen_modified_LBHeads( LBHeads, merged_validity )
    
    map_folder_tag_reprocess = get_map_folder_tag_reprocess()

    if os.path.exists( DbFile_UPD3 ):
        os.system( "rm {0}".format( DbFile_UPD3 ) )
    
    comment = "{0}. Last Update {1} using Run {2}".format( os.getenv("DbComment"), datetime.datetime.today().strftime("%Y-%m-%d %H:%M"), RunNumber )
    
    for index in range(0, len(modified_LBHeads)):
        inputDB = DbFiles[index]
        outputDB = DbFile_UPD3
        
        if not merged_validity[index]: continue
        
        for folder_tuple, tag in sorted( map_folder_tag_reprocess.iteritems() ):
            folder = folder_tuple[0]
            upd    = folder_tuple[1]
            
            if( upd != "UPD3" ): continue
            
            
            if index == 0 and index == len(modified_LBHeads)-1 :
                lbsince = 0
                lbuntil = LBlist[-1]
                
                if os.getenv("IOVUntilRunNumber") != None and os.getenv("IOVSinceRunNumber") != None:
                    IOVSinceRunNumber = os.getenv("IOVSinceRunNumber")
                    IOVUntilRunNumber = os.getenv("IOVUntilRunNumber")
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4} -nrlu {5} 0 " + \
                                   " -outtag {6} " + \
                                   " -tl \"{7}\"") \
                                   .format( inputDB, outputDB, folder, IOVSinceRunNumber, lbsince, IOVUntilRunNumber, tag, comment )
                elif os.getenv("IOVUntilRunNumber") != None:
                    IOVUntilRunNumber = os.getenv("IOVUntilRunNumber")
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4} -nrlu {5} 0 " + \
                                   " -outtag {6} " + \
                                   " -tl \"{7}\"") \
                                   .format( inputDB, outputDB, folder, RunNumber, lbsince, IOVUntilRunNumber, tag, comment )
                else:
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4} " + \
                                   " -outtag {5} " + \
                                   " -tl \"{7}\"") \
                                   .format( inputDB, outputDB, folder, RunNumber, lbsince, tag, comment )
            
            elif index == 0:
                lbsince = 0
                lbuntil = modified_LBHeads[index+1]
                if os.getenv("IOVSinceRunNumber") != None:
                    IOVSinceRunNumber = os.getenv("IOVSinceRunNumber")
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4} -nrlu {5} {6} " + \
                                   " -outtag {7} " + \
                                   " -tl \"{8}\"") \
                                   .format( inputDB, outputDB, folder, IOVSinceRunNumber, lbsince, RunNumber, lbuntil, tag, comment )
                else:
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4} -nrlu {3} {5} " + \
                                   " -outtag {6} " + \
                                   " -tl \"{7}\"") \
                                   .format( inputDB, outputDB, folder, RunNumber, lbsince, lbuntil, tag, comment )
                
            elif index == len(modified_LBHeads)-1:
                lbsince = modified_LBHeads[index]
                lbuntil = LBlist[-1]
                
                if os.getenv("IOVUntilRunNumber") != None:
                    IOVUntilRunNumber = os.getenv("IOVUntilRunNumber")
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4} -nrlu {5} 0" + \
                                   " -outtag {6} " + \
                                   " -tl \"{7}\"") \
                                   .format( inputDB, outputDB, folder, RunNumber, lbsince, IOVUntilRunNumber, tag, comment )
                else:
                    command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                                   "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                                   " -nrls {3} {4}" + \
                                   " -outtag {5} " + \
                                   " -tl \"{6}\"") \
                                   .format( inputDB, outputDB, folder, RunNumber, lbsince, tag, comment )
                
            else:
                lbsince = modified_LBHeads[index]
                lbuntil = modified_LBHeads[index+1]
                command = ("AtlCoolCopy.exe \"sqlite://schema={0};dbname=CONDBR2\" " + \
                               "\"sqlite://schema={1};dbname=CONDBR2\" -c -a -f {2} " + \
                               " -nrls {3} {4} -nrlu {3} {5} " + \
                               " -outtag {6} " + \
                               " -tl \"{7}\"") \
                               .format( inputDB, outputDB, folder, RunNumber, lbsince, lbuntil, tag, comment )
            
            tools.write_log_debug( command )
            external_command( command, "ATLCOOLCOPY_REPROCESS_UPD3" )
        
    return  DbFile_UPD3




#----------------------------------------------------------------------------------------------------
def validate_L11_constant():
    
    global RunNumber, LBlist, LBHeads, L11_validity
    
    tools.write_log_debug("validate_L11_constant()")
    
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
    
    """
    Check the number of L11 segments in the run
    """
    listTarIter0 = glob.glob( "{0}/*TAR_ALIGNFILES.Iter0*{1}/*".format(theRunDirectory, processID) )
    for tar in listTarIter0:
        alignlogfiles.append( [] )
    
    """
    Loop over Iter0 and Iter1
    """
    for iteration in [0, 1]:
        
        """
        List-up the L11 TAR files of the corresponding iteration.
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
            
    
    print"=========================================================="
    print"List of align log files for LB-segment"
    print"=========================================================="
    
    
    """
    Now, alignlogfiles lists up a pair of alignlogfiles (Iter0, Iter1) for each LB-segment.
    Check these results using the following macro.
    """
    
    """
    Check the quality of alignlogfile (monitoring)
    """
    tools.write_log_info("Checking the validity of the L11 result...")
    
    MonitoringScriptsL11.gen_L11_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles )
    judge, judge_flags = MonitoringScriptsL11.judge_L11_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles )
    
    L11_validity = judge_flags
    
    if not judge:
        tools.write_log_error("L11 result is not good.")
        conditional = False
        for pair in enumerate(judge_flags):
            tools.write_log_info( "Block {0:02d}: {1}".format( pair[0], "Valid" if pair[1] == True else "Invalid" ) )
            if pair[1] == True:
                conditional = True
        
        if conditional == True:
            return "L11_RESULTS_PARTIALLY_VALID"
        else:
            return "L11_RESULTS_NOT_GOOD"
    
    tools.write_log_info(" ==> L11 result is validated.")
    tools.write_log_info(" --------------------------------")
    
    return "L11_RESULTS_GOOD"
    



#----------------------------------------------------------------------------------------------------
def convert_new_to_old_format( representative_LB_index ):
    
    target_dir = glob.glob( "*TAR_ALIGNFILES.Iter1*" )[ representative_LB_index ]
    tools.write_log_info( "Converting {0} to the old format...".format( target_dir ) )
    
    outputbase="Run{0}_L11_StaticAlign".format( RunNumber )
    os.system( "if [ -e mycool.db ] ; then rm mycool.db; fi" )
    os.system( "rm *.txt *.xml *.tmp *.BAK mycool.db" )
    
    pwd     = os.environ['PWD']+"/"
    Sifile = "%sSi.txt"%pwd
    TRTfile = "%sTRT.txt"%pwd
    
    external_command("cat %s/OutputSiGlobalFolder.txt %s/OutputSiAlignment.txt > %s"%(target_dir,target_dir,Sifile), "PREP_NEW_TO_OLD_CONVERSION_1")
    external_command("cat %s/OutputTRTGlobalFolder.txt %s/OutputTRTAlignment.txt > %s"%(target_dir,target_dir,TRTfile), "PREP_NEW_TO_OLD_CONVERSION_2")

    external_command("sed -i -e 's/AlignL1/Align/g' %s"%Sifile, "PREP_NEW_TO_OLD_CONVERSION_FORMAT_1")
    external_command("sed -i -e 's/AlignL2/Align/g' %s"%Sifile, "PREP_NEW_TO_OLD_CONVERSION_FORMAT_2")
    external_command("sed -i -e 's/AlignL3/Align/g' %s"%Sifile, "PREP_NEW_TO_OLD_CONVERSION_FORMAT_3")
    
    external_command("sed -i -e 's/AlignL1/Align/g' %s"%TRTfile, "PREP_NEW_TO_OLD_CONVERSION_FORMAT_4")
    external_command("sed -i -e 's/AlignL2/Align/g' %s"%TRTfile, "PREP_NEW_TO_OLD_CONVERSION_FORMAT_5")
    
    execCommandOptions = " -c 'FileInSi=\"%s\"; FileInTRT=\"%s\"' "%(Sifile,TRTfile)
    
    tools.write_log_debug( " <poolToTxt_converter> execCommandOptions = {0}".format( execCommandOptions ) )
    
    convertCommand = "athena.py poolToTxt_converter.py"+execCommandOptions

    external_command( convertCommand, "POOL_TO_TXT_CONVERTER")
    
    validationCommand = "athena.py poolToTxt_TextDumper.py -c \"inputCollections = ['OldFolderScheme.root']\""
    
    external_command( validationCommand, "POOL_TO_TXT_DUMP" )
    
    '''
    Validation - count the number of lines in the dumped text files
    '''
    for suffix in ["Si", "TRT"]:
        with open("DumpPoolFileContents_{0}.txt".format( suffix ) ) as dumpfile:
            if not sum(1 for line in dumpfile) == (6223 if suffix == "Si" else 185) :
                raise Exception("POOLFILE_{0}_CONTENTS_INVALID".format( suffix ) )
            else:
                tools.write_log_info("The New->Old converted POOL file was validated.")
            
    '''
    Claenup subsidiary files and rename result files
    '''
    external_command( ("rm {{Si,TRT,TRTOutFile,eventLoopHeartBeat}}.txt PoolFileCatalog.xml* hostnamelookup.tmp* *.bak " + \
                           "DumpPoolFileContents_*.txt OutputTRTAlignment_Straw_out.txt OldFolderScheme.root;" + \
                           "mv mycool.db {0}.db; " + \
                           "mv TESTING.pool.root {0}.root" ) \
                          .format( outputbase ),
                      "CONVERT_NEW_TO_OLD_CLENUP" )
    
    PoolFile_Static = "{0}.root".format( outputbase )
    
    return PoolFile_Static, outputbase


#----------------------------------------------------------------------------------------------------

UPD4_DbFile = ""
UPD1_DbFile = ""
UPD3_DbFile = ""
UPD4_DbFile_Static = ""
UPD1_DbFile_Static = ""
PoolFile_Static = ""


def validate_and_merge_L16_constant():
    
    global RunNumber, LBlist, LBHeads, L16_validity
    
    tools.write_log_debug("validate_and_merge_L16_constant()")
    
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
    RootFiles     = []
    DbFiles       = []
    
    """
    Check the number of L16 segments in the run
    """
    listTarIter3 = glob.glob( "{0}/*TAR_ALIGNFILES.Iter3*{1}/*".format(theRunDirectory, processID) )
    for tar in listTarIter3:
        alignlogfiles.append( [] )

    """
    Loop over Iter2, Iter3
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
            list_DbFiles   = glob.glob( "{0}/*ROOT_DB.Iter{1}*{2}/*".format( theRunDirectory, iteration, processID ) )
            list_RootFiles   = glob.glob( "{0}/*ROOT_CONSTANTS.Iter{1}*{2}/*".format( theRunDirectory, iteration, processID ) )
            
            for tarfile, DbFile, RootFile in zip( tarfiles, list_DbFiles, list_RootFiles ):
                """
                Copy the Db file and ROOT (POOL) file temporarily
                """
                filebasename = os.path.basename( tarfile ).replace("TAR_ALIGNFILES.", "")
                external_command( "cp {0} {1}.db"  .format( DbFile,   filebasename ), "COPY_DOTDB_FILE" )
                external_command( "cp {0} {1}.root".format( RootFile, filebasename ), "COPY_POOL_FILE" )
                
                DbFiles   .append( "{0}.db"  .format( filebasename ) )
                RootFiles .append( "{0}.root".format( filebasename ) )

    
    tools.write_log_info(" ==> Succeeded in fetching alignlogfiles for L16.")
    print""
    print""
    print"=========================================================="
    print"List of alignlog files for LB-segment"
    print"=========================================================="
    pprint.pprint( alignlogfiles )
    
    print""
    print""
    print"=========================================================="
    print"List of .db files for LB-segment"
    print"=========================================================="
    pprint.pprint( DbFiles )
    
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
    
    L16_validity = L16_segment_flags
    
    if not isGood_L16:
        return "L16_RESULT_NOT_GOOD"
    
    
    tools.write_log_info(" ==> L16 result is validated.")
    print""
    print""
    print"=========================================================="
    print""
    print"==> Proceed to prepare for .db files"
    print""
    print""
    
    global UPD4_DbFile
    global UPD1_DbFile
    global UPD3_DbFile
    global UPD4_DbFile_Static
    global UPD1_DbFile_Static
    global PoolFile_Static
    
    tools.write_log_info(" ==> Proceed to prepare for .db files")
    
    n_segments = len( LBHeads )
    representative_LB_index = 3+(n_segments-3)/2 if n_segments>3 else n_segments/2
    tools.write_log_info("Representative index is {0}.".format( representative_LB_index ) )
    pprint.pprint( DbFiles )
    tools.write_log_info("==> Selected {0} as the repsresentative LB segment.".format( DbFiles[representative_LB_index] ) )
    
    """
    First: create UPD4 (for BLK)
    """
    if os.getenv("do_upload_dynamic") == "1":
        UPD4_DbFile = generate_DbFile_for_UPD4( RunNumber, LBHeads, DbFiles )
        tools.write_log_info("Generated BLK-UPD4 DB File: {0}".format( UPD4_DbFile ) )
    
    """
    Second: create UPD1 (for ES1)
    """
    if os.getenv("do_upload_dynamic") == "1":
        UPD1_DbFile = generate_DbFile_for_UPD1( RunNumber, representative_LB_index, DbFiles )
        tools.write_log_info("Generated ES1-UPD1 DB File: {0}".format( UPD1_DbFile ) )    
    
    """
    Third: create UPD3 (for reprocess)
    """
    if os.getenv("do_upload_reprocess") == "1":
        UPD3_DbFile = generate_DbFile_for_UPD3( RunNumber, LBHeads, DbFiles )
        tools.write_log_info("Generated reprocess BLK-UPD3 DB File: {0}".format( UPD3_DbFile ) )
    
    """
    Fourth: convert each .db file to the old folder scheme {.pool, .db} files
    These files are used to update UPD4/UPD1 of the old static COOL folders
    """
    if os.getenv("do_upload_static") == "1":
        PoolFile_Static, outputbase = convert_new_to_old_format( representative_LB_index  )
        
        map_folder_tag_static = get_map_folder_tag_static()
        
        for folder_tuple, tag in sorted( map_folder_tag_static.iteritems() ):
            folder = folder_tuple[0]
            upd    = folder_tuple[1]
            
            if upd == "UPD4":
                print "Creating the UPD4 format..."
                command = ( "AtlCoolCopy.exe " + \
                                "\"sqlite://schema={0}.db;dbname=CONDBR2\" " + \
                                "\"sqlite://;schema={0}_UPD4.db;dbname=CONDBR2\" " + \
                                " -a -create -f {3} " + \
                                " -nrls {1} {2} -outtag {4} -tl \"Static UPD1 DB File from Run {1}\"") \
                                .format( outputbase, RunNumber, 0, folder, tag )
                external_command(command, "ATLCOOLCOPY_STATIC_UPD4")
                
                UPD4_DbFile_Static = "{0}_UPD4.db".format( outputbase )
                
            elif upd == "UPD1":
                print "Creating the UPD1 format..."
                command = ( "AtlCoolCopy.exe " + \
                                "\"sqlite://schema={0}.db;dbname=CONDBR2\" " + \
                                "\"sqlite://;schema={0}_UPD1.db;dbname=CONDBR2\" " + \
                                " -a -create -f {2} " + \
                                " -go -outtag {3} -tl \"Static UPD1 DB File from Run {1}\"") \
                                .format( outputbase, RunNumber, folder, tag )
                external_command(command, "ATLCOOLCOPY_STATIC_UPD1")
                
                UPD1_DbFile_Static = "{0}_UPD1.db".format( outputbase )
                
        tools.write_log_info("Generated BLK-UPD4 DB File for static alignment folders : {0}".format( UPD4_DbFile_Static ) )
        tools.write_log_info("Generated ES1-UPD1 DB File for static alignment folders : {0}".format( UPD1_DbFile_Static ) )
    
    tools.write_log_info(" ==> Preparation for uploading is done.")
    tools.write_log_info(" --------------------------------")
    
    return "L16_RESULT_GOOD"


#----------------------------------------------------------------------------------------------------
def validate_and_merge_L11_constant():
    
    global RunNumber, LBlist, LBHeads, L11_validity, L16_validity
    
    tools.write_log_debug("validate_and_merge_L16_constant()")
    
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
    
    
    tools.write_log_info("Fetching L11 alignlogfiles...")
    """
    Define list variables
    - alignlogfiles: container of alignlogfile. This is supposed to store the alignlogfile as follows:
      [ [Segment0_Iter0, Segment0_Iter1],
        [Segment1_Iter0, Segment1_Iter1],
        [Segment2_Iter0, Segment2_Iter1], ... ]
    """
    alignlogfiles = []
    RootFiles     = []
    DbFiles       = []
    
    """
    Check the number of L11 segments in the run
    """
    listTarIter1 = glob.glob( "{0}/*TAR_ALIGNFILES.Iter1*{1}/*".format(theRunDirectory, processID) )
    for tar in listTarIter1:
        alignlogfiles.append( [] )

    """
    Loop over Iter0, Iter1
    """
    for iteration in [0, 1]:
        
        """
        List-up the L11 TAR files of the corresponding iteration.
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
        if iteration == 1:
            list_DbFiles   = glob.glob( "{0}/*ROOT_DB.Iter{1}*{2}/*".format( theRunDirectory, iteration, processID ) )
            list_RootFiles   = glob.glob( "{0}/*ROOT_CONSTANTS.Iter{1}*{2}/*".format( theRunDirectory, iteration, processID ) )
            
            for tarfile, DbFile, RootFile in zip( tarfiles, list_DbFiles, list_RootFiles ):
                """
                Copy the Db file and ROOT (POOL) file temporarily
                """
                filebasename = os.path.basename( tarfile ).replace("TAR_ALIGNFILES.", "")
                external_command( "cp {0} {1}.db"  .format( DbFile,   filebasename ), "COPY_DOTDB_FILE" )
                external_command( "cp {0} {1}.root".format( RootFile, filebasename ), "COPY_POOL_FILE" )
                
                DbFiles   .append( "{0}.db"  .format( filebasename ) )
                RootFiles .append( "{0}.root".format( filebasename ) )

    
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
    print"List of .db files for LB-segment"
    print"=========================================================="
    pprint.pprint( DbFiles )
    
    """
    Now, alignlogfiles lists up a pair of alignlogfiles (Iter0, Iter1) for each LB-segment.
    Check these results using the following macro.
    
    isGood_L11 is just a simple boolean to see the overall validity,
    L11_segment_flags is an array of booleans [ T/F, T/F, .... ]
    corresponding to each LB-segment.
    """
    tools.write_log_info("Checking the validity of the L11 result...")
    MonitoringScriptsL11.gen_L11_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles )
    isGood_L11, L11_segment_flags = MonitoringScriptsL11.judge_L11_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles )
    
    L11_validity = L11_segment_flags
    L16_validity = L11_validity
    
    if not isGood_L11:
        tools.write_log_error("L11 result is not good.")
        conditional = False
        for pair in enumerate(L11_segment_flags):
            tools.write_log_info( "Block {0:02d}: {1}".format( pair[0], "Valid" if pair[1] == True else "Invalid" ) )
            if pair[1] == True:
                conditional = True
        
        if conditional == True:
            return "L11_RESULTS_PARTIALLY_VALID"
        else:
            return "L11_RESULTS_NOT_GOOD"
    
    
    tools.write_log_info(" ==> L11 result is validated.")
    print""
    print""
    print"=========================================================="
    print""
    print"==> Proceed to prepare for .db files"
    print""
    print""
    
    global UPD4_DbFile
    global UPD1_DbFile
    global UPD3_DbFile
    global UPD4_DbFile_Static
    global UPD1_DbFile_Static
    global PoolFile_Static
    
    tools.write_log_info(" ==> Proceed to prepare for .db files")
    
    n_segments = len( LBHeads )
    representative_LB_index = 3+(n_segments-3)/2 if n_segments>3 else n_segments/2
    tools.write_log_info("Representative index is {0}.".format( representative_LB_index ) )
    tools.write_log_info("==> Selected {0} as the repsresentative LB segment.".format( DbFiles[representative_LB_index] ) )
    
    """
    First: create UPD4 (for BLK)
    """
    if os.getenv("do_upload_dynamic") == "1":
        UPD4_DbFile = generate_DbFile_for_UPD4( RunNumber, LBHeads, DbFiles )
        tools.write_log_info("Generated BLK-UPD4 DB File: {0}".format( UPD4_DbFile ) )
    
    """
    Second: create UPD1 (for ES1)
    """
    if os.getenv("do_upload_dynamic") == "1":
        UPD1_DbFile = generate_DbFile_for_UPD1( RunNumber, representative_LB_index, DbFiles )
        tools.write_log_info("Generated ES1-UPD1 DB File: {0}".format( UPD1_DbFile ) )    
    
    """
    Third: create UPD3 (for reprocess)
    """
    if os.getenv("do_upload_reprocess") == "1":
        UPD3_DbFile = generate_DbFile_for_UPD3( RunNumber, LBHeads, DbFiles )
        tools.write_log_info("Generated reprocess BLK-UPD3 DB File: {0}".format( UPD3_DbFile ) )
    
    """
    Fourth: convert each .db file to the old folder scheme {.pool, .db} files
    These files are used to update UPD4/UPD1 of the old static COOL folders
    """
    if os.getenv("do_upload_static") == "1":
        PoolFile_Static, outputbase = convert_new_to_old_format( representative_LB_index  )
        
        map_folder_tag_static = get_map_folder_tag_static()
        
        for folder_tuple, tag in sorted( map_folder_tag_static.iteritems() ):
            folder = folder_tuple[0]
            upd    = folder_tuple[1]
            
            if upd == "UPD4":
                print "Creating the UPD4 format..."
                command = ( "AtlCoolCopy.exe " + \
                                "\"sqlite://schema={0}.db;dbname=CONDBR2\" " + \
                                "\"sqlite://;schema={0}_UPD4.db;dbname=CONDBR2\" " + \
                                " -a -create -f {3} " + \
                                " -nrls {1} {2} -outtag {4} -tl \"Static UPD1 DB File from Run {1}\"") \
                                .format( outputbase, RunNumber, 0, folder, tag )
                external_command(command, "ATLCOOLCOPY_STATIC_UPD4")
                
                UPD4_DbFile_Static = "{0}_UPD4.db".format( outputbase )
                
            elif upd == "UPD1":
                print "Creating the UPD1 format..."
                command = ( "AtlCoolCopy.exe " + \
                                "\"sqlite://schema={0}.db;dbname=CONDBR2\" " + \
                                "\"sqlite://;schema={0}_UPD1.db;dbname=CONDBR2\" " + \
                                " -a -create -f {2} " + \
                                " -go -outtag {3} -tl \"Static UPD1 DB File from Run {1}\"") \
                                .format( outputbase, RunNumber, folder, tag )
                external_command(command, "ATLCOOLCOPY_STATIC_UPD1")
                
                UPD1_DbFile_Static = "{0}_UPD1.db".format( outputbase )
                
        tools.write_log_info("Generated BLK-UPD4 DB File for static alignment folders : {0}".format( UPD4_DbFile_Static ) )
        tools.write_log_info("Generated ES1-UPD1 DB File for static alignment folders : {0}".format( UPD1_DbFile_Static ) )
    
    tools.write_log_info(" ==> Preparation for uploading is done.")
    tools.write_log_info(" --------------------------------")
    
    return "L11_RESULT_GOOD"


#----------------------------------------------------------------------------------------------------
def is_pool_file_registered( PoolFile_Static ):
    
    """
    Check if the pool file is already registered in the catalog
    """
    
    fetch_guid_command = "pool_extractFileIdentifier.py {0} 2>/dev/null | grep {0} | grep -v Success | awk \"{{print $1;}}\"".format( PoolFile_Static )
    fetch_guid = subprocess.Popen( fetch_guid_command, shell=True, stdout=subprocess.PIPE )
    fetch_guid.wait()
    fetch_guid_out, fetch_guid_err = fetch_guid.communicate()
    print "fetch_guid_out = ", fetch_guid_out.split()[0]
    
    tools.write_log_info( "GUID for {0} is identified to be {1}".format( PoolFile_Static, fetch_guid_out ) )
    
    check_guid_command = "cat /cvmfs/atlas.cern.ch/repo/conditions/poolcond/PoolFileCatalog.xml | grep {0} | wc -l".format( fetch_guid_out.split()[0] )
    check_guid = subprocess.Popen( check_guid_command, shell=True, stdout=subprocess.PIPE )
    check_guid.wait()
    check_guid_out, check_guid_err = check_guid.communicate()
    
    flag = ( int( check_guid_out ) != 0 )
    
    return flag


#----------------------------------------------------------------------------------------------------
def upload_constants_dynamic( RunNumber, user ):
    global UPD4_DbFile
    global UPD1_DbFile
    
    map_folder_tag_dynamic = get_map_folder_tag_dynamic()
    
    """
    Dynamic Folder Uploads
    """
    
    tools.write_log_info("--------------------------------------------------" )
    tools.write_log_info("Uploading dynamic alignment constants..." )
    
    for folder_tuple, outtag in sorted( map_folder_tag_dynamic.iteritems() ):
        folder = folder_tuple[0]
        upd    = folder_tuple[1]
        
        passphrase = ""
        if folder.find("Indet")>=0 :
            passphrase = "ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1" 
        elif folder.find("TRT")>=0 :
            passphrase = "ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2"
        
        if upd == "UPD1":
            
            mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                       "--debug --batch --nobackup " + \
                       "--comment \"ES1 UPD1 DB File from Run {0}\" " + \
                       "--tag {1} --passopt='-cti' " + \
                       "--folder {2} {3} CONDBR2 {4}" ) \
                       .format( RunNumber, outtag, folder, UPD1_DbFile, passphrase )
            
        elif upd == "UPD4":
            
            """
            
            ----------------------------------------------------------------------------
            
            !!!!!!!! VERY IMPORTANT COMMENT. PLEASE READ THIS. (Hide: 2016-08-22) !!!!!!!!
            
            ----------------------------------------------------------------------------
            
            Check if the corresponding tag is already registered in the COOLOFL database.
            If the tag is not present, one needs to create a new UPD4 tag,
            but in that case it is necessary to use --ignoremode=BackDoor option.
            
            Otherwise that tag will be created as a UPD1 tag, and it is not possible
            to change the type of the UPD after creation.
            
            This is a quite dangerous feature of the COOL database...
            
            """
            
            """
            listfoldertags.sh calls AtlCoolConsole.py, and lists up the registered tag inside the specified folder.
            """
            
            schema="COOLOFL_INDET/CONDBR2" if folder.find("Indet")>=0 else "COOLOFL_TRT/CONDBR2"
            
            tools.write_log_info( "Examining {0}, {1} tags...".format( schema, folder ) )
            checkTagCommand = subprocess.Popen( "./listfoldertags.sh {0} {1}".format( schema, folder ), shell=True, stdout=subprocess.PIPE )
            checkTagCommand.wait()
            checkTag_out, checkTag_err = checkTagCommand.communicate()
            
            if checkTag_out.find( outtag ) >= 0 :
                
                mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                                    "--debug --batch --nobackup " + \
                                    "--comment \"BLK UPD4 DB File from Run {0}\" " + \
                                    "--tag {1} --passopt='-cti' " + \
                                    "--folder {2} {3} CONDBR2 {4}" ) \
                                    .format( RunNumber, outtag, folder, UPD4_DbFile, passphrase )
    
            else:
                
                tools.write_log_warning( "Outtag {0} is not registered ==> Create new with --ignoremode=BackDoor".format( outtag ) )
                    
                mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                                    "--debug --batch --nobackup " + \
                                    "--comment \"BLK UPD4 DB File from Run {0}\" " + \
                                    "--tag {1} --ignoremode=BackDoor --passopt='-cti' " + \
                                    "--folder {2} {3} CONDBR2 {4}" ) \
                                    .format( RunNumber, outtag, folder, UPD4_DbFile, passphrase )
    
    
        if os.getenv("exec") == "--exec" :
            tools.write_log_info("Uploading dynamic alignment constants of {0} for {1} to tag {2}...".format( folder, upd, outtag ) )
            external_command( mergeCommand, "ATLCOOLMERGE_DYNAMIC" )  
        else:
            tools.write_log_info( mergeCommand )
            tools.write_log_warning( "Dry Run : Constants are not uploaded" )
        
    tools.write_log_info("Uploading for dynamic alignment constants is done." )


#----------------------------------------------------------------------------------------------------
def upload_constants_static( RunNumber, user ):
    
    global UPD4_DbFile_Static
    global UPD1_DbFile_Static
    global PoolFile_Static
    
    map_folder_tag_static  = get_map_folder_tag_static()
    
    """
    Static Folder Uploads
    """
    
    """ POOL files """
    tools.write_log_info("--------------------------------------------------" )
    tools.write_log_info("Uploading static alignment constants (for backward compatibility)..." )
    
    if is_pool_file_registered( PoolFile_Static ):
        tools.write_log_warning("The POOL file {0} is already registered ==> Skip the registration".format( PoolFile_Static ) )
        
    else:
        registerFileCommand = "~atlcond/utils/registerFiles2 --wait condR2_data.gen.COND {0}".format( PoolFile_Static )
        if os.getenv("exec") == "--exec" :
            tools.write_log_debug( registerFileCommand )
            tools.write_log_info("Uploading the POOL file : {0}".format( PoolFile_Static ) )
            external_command( registerFileCommand, "REGISTERFILE2" )  
        else:
            tools.write_log_info("Uploading the POOL file : {0}".format( PoolFile_Static ) )
            tools.write_log_info( registerFileCommand )
            tools.write_log_warning( "Dry Run : The POOL file is not uploaded." )

    
    """ DB files """
    for folder_tuple, outtag in sorted( map_folder_tag_static.iteritems() ):
        folder = folder_tuple[0]
        upd    = folder_tuple[1]
        
        passphrase = ""
        if folder.find("Indet")>=0 :
            passphrase = "ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1" 
        elif folder.find("TRT")>=0 :
            passphrase = "ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2"
        
        if upd == "UPD1":
            mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                       "--debug --batch --nobackup " + \
                       "--comment \"ES1 UPD1 DB File from Run {0}\" " + \
                       "--tag {1} --passopt='-cti' " + \
                       "--folder {2} {3} " + \
                       "CONDBR2 {4}" ) \
                       .format( RunNumber, outtag, folder, UPD1_DbFile_Static, passphrase )
        elif upd == "UPD4":
            
            """
            
            ----------------------------------------------------------------------------
            
            !!!!!!!! VERY IMPORTANT COMMENT. PLEASE READ THIS. (Hide: 2016-08-22) !!!!!!!!
            
            ----------------------------------------------------------------------------
            
            Check if the corresponding tag is already registered in the COOLOFL database.
            If the tag is not present, one needs to create a new UPD4 tag,
            but in that case it is necessary to use --ignoremode=BackDoor option.
            
            Otherwise that tag will be created as a UPD1 tag, and it is not possible
            to change the type of the UPD after creation.
            
            This is a quite dangerous feature of the COOL database...
            
            """
            
            """
            listfoldertags.sh calls AtlCoolConsole.py, and lists up the registered tag inside the specified folder.
            """
            
            schema="COOLOFL_INDET/CONDBR2" if folder.find("Indet")>=0 else "COOLOFL_TRT/CONDBR2"
            
            tools.write_log_info( "Examining {0}, {1} tags...".format( schema, folder ) )
            checkTagCommand = subprocess.Popen( "./listfoldertags.sh {0} {1}".format( schema, folder ), shell=True, stdout=subprocess.PIPE )
            checkTagCommand.wait()
            checkTag_out, checkTag_err = checkTagCommand.communicate()
            
            if checkTag_out.find( outtag ) >= 0 :
                
                mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                                    "--debug --batch --nobackup " + \
                                    "--comment \"BLK UPD4 DB File from Run {0}\" " + \
                                    "--tag {1} --passopt='-cti' " + \
                                    "--folder {2} {3} " + \
                                    " CONDBR2 {4}" ) \
                                    .format( RunNumber, outtag, folder, UPD4_DbFile_Static, passphrase )
            else:
                
                tools.write_log_warning( "Outtag {0} is not registered ==> Create new with --ignoremode=BackDoor".format( outtag ) )
                    
                mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                                    "--debug --batch --nobackup " + \
                                    "--comment \"BLK UPD4 DB File from Run {0}\" " + \
                                    "--tag {1} --ignoremode=BackDoor --passopt='-cti' " + \
                                    "--folder {2} {3} " + \
                                    " CONDBR2 {4}" ) \
                                    .format( RunNumber, outtag, folder, UPD4_DbFile_Static, passphrase )
    
    
        if os.getenv("exec") == "--exec" :
            tools.write_log_debug( mergeCommand )
            tools.write_log_info("Uploading static alignment constants of {0} for {1} to tag {2}...".format( folder, upd, outtag ) )
            external_command( mergeCommand, "ATLCOOLMERGE_STATIC" )
        
        else:
            tools.write_log_info( mergeCommand )
            tools.write_log_warning( "Dry Run : Constants are not uploaded" )
        
    tools.write_log_info("Uploading for static alignment constants (for backward compatibility) is done." )
    
    logfile=open("/afs/cern.ch/user/a/atlidali/w0/calibLoop/checker/log/CalibLoopChecker_log.txt", "a")
    now = datetime.datetime.now()
    logfile.write( "{0} - {1} - {2}\n".format( now.strftime("%Y-%m-%d-%H:%M:%S"), int(time.mktime(now.timetuple())), RunNumber ) )
    logfile.close()
    
    
#----------------------------------------------------------------------------------------------------
def upload_constants_reprocess( RunNumber, user ):
    global UPD3_DbFile
    
    map_folder_tag_reprocess = get_map_folder_tag_reprocess()
    
    """
    Reprocess Folder Uploads
    """
    
    tools.write_log_info("--------------------------------------------------" )
    tools.write_log_info("Uploading reprocess alignment constants..." )
    
    for folder_tuple, outtag in sorted( map_folder_tag_reprocess.iteritems() ):
        folder = folder_tuple[0]
        upd    = folder_tuple[1]
        
        schema = ""
        passphrase = ""
        if folder.find("Indet")>=0 :
            schema="COOLOFL_INDET/CONDBR2"
            passphrase = "ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1" 
        elif folder.find("TRT")>=0 :
            schema="COOLOFL_TRT/CONDBR2"
            passphrase = "ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2"
        
            
        """
        
        ----------------------------------------------------------------------------
        
        !!!!!!!! VERY IMPORTANT COMMENT. PLEASE READ THIS. (Hide: 2016-09-05) !!!!!!!!
        
        ----------------------------------------------------------------------------
        
        For UPD3 tags, 
        
        """
        
        tools.write_log_info( "Examining {0}, {1} tags...".format( schema, folder ) )
        checkTagCommand = subprocess.Popen( "./listfoldertags.sh {0} {1}".format( schema, folder ), shell=True, stdout=subprocess.PIPE )
        checkTagCommand.wait()
        checkTag_out, checkTag_err = checkTagCommand.communicate()
        
        if checkTag_out.find( outtag ) >= 0 :
            
            mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                                "--debug --batch --nobackup " + \
                                "--comment \"BLK UPD3 DB File from Run {0}\" " + \
                                "--tag {1}  --passopt='-cti' " + \
                                "--folder {2} {3} CONDBR2 {4}" ) \
                                .format( RunNumber, outtag, folder, UPD3_DbFile, passphrase )
            
        else:
            
            mergeCommand = ("/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py " + \
                                "--debug --batch --nobackup " + \
                                "--comment \"BLK UPD3 DB File from Run {0}\" " + \
                                "--tag {1}  --ignoremode=BackDoor --passopt='-cti' " + \
                                "--folder {2} {3} CONDBR2 {4}" ) \
                                .format( RunNumber, outtag, folder, UPD3_DbFile, passphrase )
            
            
        if os.getenv("exec") == "--exec" :
            tools.write_log_info("Uploading reprocess alignment constants of {0} for {1} to tag {2}...".format( folder, upd, outtag ) )
            external_command( mergeCommand, "ATLCOOLMERGE_REPROCESS" )  
        else:
            tools.write_log_info( mergeCommand )
            tools.write_log_warning( "Dry Run : Constants are not uploaded" )
        
    tools.write_log_info("Uploading for reprocess alignment constants is done." )


#----------------------------------------------------------------------------------------------------
def upload_constants( RunNumber, user ):
    
    tools.write_log_info("Submitting constants under authentication of {0}".format( user ) )    
    
    global UPD4_DbFile
    global UPD1_DbFile
    global UPD4_DbFile_Static
    global UPD1_DbFile_Static
    global PoolFile_Static
    
    if os.getenv("do_upload_dynamic") == "1":
        upload_constants_dynamic( RunNumber, user )
    else:
        tools.write_log_warning("do_upload_dynamic flag is false ==> Skip upload_constants_dynamic().")
        
    if os.getenv("do_upload_static") == "1":
        upload_constants_static( RunNumber, user )
    else:
        tools.write_log_warning("do_upload_static flag is false ==> Skip upload_constants_static().")
    
    if os.getenv("do_upload_reprocess") == "1":
        upload_constants_reprocess( RunNumber, user )
    else:
        tools.write_log_warning("do_upload_reprocess flag is false ==> Skip upload_constants_reprocess().")
        
    
    """
    Handshake
    """
    
    global projectName
    if user == "CRONJOB" and os.getenv("exec") == "--exec" :
        handshake_command = "./HandShake.py {0} {1} {2} {3} --exec".format( RunNumber, projectName, os.getenv("HandshakeStream"), user )
        tools.write_log_info( handshake_command )
        external_command( handshake_command, "HANDSHAKE" )
        tools.write_log_info( "Handshake is done." )
    

    
#----------------------------------------------------------------------------------------------------
def upload_constants_manual( RunNumber ):
    print "\n\n\n-----------------------------------------------------------------------------------\n\n"
    while True:
        print "\n\nPreparation of constants are done.\n\n\n"
        print "You are going to upload constants to the following tags:"
        print ""
        if os.getenv("do_upload_dynamic") == "1":
            print "===================================="
            print "For the new scheme"
            print "===================================="
            print ""
            print "/Indet/AlignL1/ID  -  ES1-UPD1:   {0}".format( os.getenv("IndetAlignL1ID_UPD1_TAG") )
            print "/Indet/AlignL1/ID  -  BLK-UPD4:   {0}".format( os.getenv("IndetAlignL1ID_UPD4_TAG") )
            print ""
            print "/Indet/AlignL2/PIX -  ES1-UPD1:   {0}".format( os.getenv("IndetAlignL2PIX_UPD1_TAG") )
            print "/Indet/AlignL2/PIX -  BLK-UPD4:   {0}".format( os.getenv("IndetAlignL2PIX_UPD4_TAG") )
            print ""
            print "/Indet/AlignL2/SCT -  ES1-UPD1:   {0}".format( os.getenv("IndetAlignL2SCT_UPD1_TAG") )
            print "/Indet/AlignL2/SCT -  BLK-UPD4:   {0}".format( os.getenv("IndetAlignL2SCT_UPD4_TAG") )
            print ""
            print "/Indet/IBLDist     -  ES1-UPD1:   {0}".format( os.getenv("IndetIBLDist_UPD1_TAG") )
            print "/Indet/IBLDist     -  BLK-UPD4:   {0}".format( os.getenv("IndetIBLDist_UPD4_TAG") )
            print ""
            print "/TRT/AlignL1/TRT   -  ES1-UPD1:   {0}".format( os.getenv("TRTAlignL1_UPD1_TAG") )
            print "/TRT/AlignL1/TRT   -  BLK-UPD4:   {0}".format( os.getenv("TRTAlignL1_UPD4_TAG") )
            print ""
            
        if os.getenv("do_upload_static") == "1":
            print ""
            print "===================================="
            print "For the old schemes:"
            print "===================================="
            print ""
            print "/Indet/Align    -  ES1-UPD1:   {0}".format( os.getenv("IndetAlign_UPD1_TAG") )
            print "/Indet/Align    -  BLK-UPD4:   {0}".format( os.getenv("IndetAlign_UPD4_TAG") )
            print ""
            print "/TRT/Align      -  ES1-UPD1:   {0}".format( os.getenv("TRTAlign_UPD1_TAG") )
            print "/TRT/Align      -  BLK-UPD4:   {0}".format( os.getenv("TRTAlign_UPD4_TAG") )
            print ""
        print "\n\n\n-----------------------------------------------------------------------------------\n\n"
        
        if os.getenv("do_upload_reprocess") == "1":
            print "===================================="
            print "For the new scheme"
            print "===================================="
            print ""
            print "/Indet/AlignL1/ID  -  BLK-UPD3:   {0}".format( os.getenv("IndetAlignL1ID_UPD3_TAG") )
            print ""
            print "/Indet/AlignL2/PIX -  BLK-UPD3:   {0}".format( os.getenv("IndetAlignL2PIX_UPD3_TAG") )
            print ""
            print "/Indet/AlignL2/SCT -  BLK-UPD3:   {0}".format( os.getenv("IndetAlignL2SCT_UPD3_TAG") )
            print ""
            print "/Indet/IBLDist     -  BLK-UPD3:   {0}".format( os.getenv("IndetIBLDist_UPD3_TAG") )
            print ""
            print "/TRT/AlignL1/TRT   -  BLK-UPD3:   {0}".format( os.getenv("TRTAlignL1_UPD3_TAG") )
            print ""
            
        sys.stdout.write("\a\a\a\a")
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
        if( m.hexdigest() == "2aec02827c060d8293c6f1d7858a6e60" ):
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
    
    try:
        
        RunNumber = int( sys.argv[1] )
        
        cleanup()
        
        opening_message()
    
        """
        Get the RAW data file list and create LB segments
        """
        LBlist = []
        with open( get_raw_filelist( RunNumber ) ) as file:
            for line in file:
                if line.find("express_express") >= 0:
                    LB = int( line.split(".")[-4][-4:] )
                    LBlist.append( LB )
                else:
                    LB = int( line.split(".")[-4][-4:] )
                    LBlist.append( LB )
                
        LBlist = sorted( LBlist )
        
        print("projectName = {0}".format( projectName ) )
        
        # LBHeads = filter( (lambda LB: (LBlist.index(LB)%20 == 0 and ((len(LBlist) - LBlist.index(LB)) >= 20)
        #                        if LBlist.index(LB) <= 60 else LBlist.index(LB)%100 == 60 and (len(LBlist) - LBlist.index(LB)) > 100) ),
        #                   LBlist )
        LBHeads = filter( (lambda LB: (LBlist.index(LB)%60 == 0 and ((len(LBlist) - LBlist.index(LB)) >= 60)
                               if LBlist.index(LB) <= 60 else LBlist.index(LB)%10000 == 60 and (len(LBlist) - LBlist.index(LB)) > 10000) ),
                          LBlist )
        
        print"=========================================================="
        print"List of LB segments"
        print"=========================================================="
        tools.write_log_info("Length of the LBlist = {0}, first = {1}, last = {2}".format( len(LBlist), LBlist[0], LBlist[-1] ) )
        tools.write_log_info("LB-segments were calculated: {0}".format( LBHeads ) )
        tools.write_log_info("Number of segments: {0}".format( len(LBHeads) ) )
        
        
        if os.getenv("DB_FOR_UPLOAD") == None or os.getenv("DB_FOR_UPLOAD") == "Iter3":
            L11_validity = []
            
            L11_result = validate_L11_constant()
            tools.write_log_info(L11_result)
        
            if L11_result == "L11_RESULTS_NOT_GOOD":
                raise Exception( L11_result )
            
            L16_validity = []
            
            L16_result = validate_and_merge_L16_constant()
            tools.write_log_info(L16_result)
            
            if L16_result != "L16_RESULT_GOOD":
                raise Exception( L16_result )
        elif os.getenv("DB_FOR_UPLOAD") == "Iter1":
            tools.write_log_warning("Using Iter1 result (L11) for uploading the DB" )
            L11_result = validate_and_merge_L11_constant()
            tools.write_log_info(L11_result)
        
            if L11_result == "L11_RESULTS_NOT_GOOD":
                raise Exception( L11_result )
            
        
        """
        If the script comes to this line, all preparations before submitting DB files are done.
        """
        
        if os.getenv("run_mode") == "CRONJOB":
            upload_constants( RunNumber, "CRONJOB" )
        
        elif os.getenv("run_mode") == "MANUAL":
            upload_constants_manual( RunNumber )
            
        else:
            tools.write_log_warning("Dry Run ==> Uploading is skipped")
            
    except:
        except_traceback = traceback.format_exc()
        for line in except_traceback.split("\n"):
            tools.write_log_error( line )
        terminate("Caught Exception", "FAILURE" )
            
    cleanup()
    terminate("Completed Process Successfully.", "SUCCESS" )
        
