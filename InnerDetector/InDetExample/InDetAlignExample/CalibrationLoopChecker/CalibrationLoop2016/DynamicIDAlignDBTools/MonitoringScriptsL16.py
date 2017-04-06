#!/usr/bin/python
# -*- coding:utf-8 -*-

################################################################################
#
# MonitoringScriptsL16.py
#
# Author: Hide Oide (Hideyuki.Oide@cern.ch)
#
# Date: April 19, 2016
#
# Description:
# Collection of monitoring functions to be used in CalibrationLoopChecker.py for L16
#
################################################################################


from ROOT import TH1F, TCanvas, TLine, gPad, gStyle
from ROOT import kBlack, kRed

import os, pprint

import tools

#----------------------------------------------------------------------------------------------------
def set_hist_style( hist ):

    hist.SetMarkerStyle(21)
    hist.SetMarkerColor(kRed+1)
    hist.SetLineColor(kRed+1)
    hist.SetLineWidth(1)
    hist.SetTitleSize(0.3)
    
    hist.GetXaxis().SetLabelSize(0.1)
    hist.GetXaxis().SetTitleSize(0.07)
    hist.GetXaxis().SetTitleOffset(2.2)
    
    hist.GetYaxis().SetLabelSize(0.05)
    hist.GetYaxis().SetTitleSize(0.07)
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.GetYaxis().SetTickLength(0.1)
    
    hist.SetStats(0)

#----------------------------------------------------------------------------------------------------
def set_canvas_style( canvas ):
    canvas.SetBottomMargin(0.3)
    canvas.SetRightMargin(0.05)
    canvas.SetLeftMargin(0.15)
    canvas.SetTicks(1, 1)


#----------------------------------------------------------------------------------------------------
def check_validity( package, dof, value, error) :
    return True if error < 4.0 else False

#----------------------------------------------------------------------------------------------------
def get_align_results( alignlogfile ):
    
    alignresults = {}
    """
    Scan the alignlogfile.txt of the correspondint Iter
    """
    for line in open( alignlogfile ):
        """
        This is a simple parser of alignlogfile
        The log file looks like as follows:
        
        ============================================
        ...
        Alignment parameters for module: Pixel/IBL
        Number of tracks passing: 2526044
        Number of hits seen:      2671829
        Number of tracks seen:    2526044
        TransX    0.0041452    +/- 4.2286e-05  
        TransY    -0.0039621   +/- 4.2487e-05  
        TransZ    0.00041507   +/- 0.00042143  
        RotX      5.0849e-06   +/- 1.4928e-07  
        RotY      1.4479e-05   +/- 1.4965e-07  
        RotZ      2.4469e-08   +/- 1.4183e-06  
        BowX      -0.026019    +/- 5.3556e-05  
        ...
        ============================================
        
        We need to identify the (value, error) of the given (stave, DoF).
        The stave is tagged using the line
        
        "Alignment parameters for module: Pixel/IBL"
        
        and then the tagged stave is used until the next similar line appears.
        
        The DoF line is tagged with tagging "+/-".
        
        """
        
        """
        Tagging the stave info
        """
        if line.find("Alignment parameters for module") >= 0:
            stave = (line[:-1].split(": ")[-1]).split("/")[-1]
        
        """
        Tagging the DoF info
        """
        if line.find("+/-") >= 0 :
            tokens = line.split()
            dof = tokens[0]
            value = float( tokens[1] )
            error = float( tokens[3] )
        
            """
            Store the set of {stave, dof, value, error} in alignresults
            """
            alignresults.update( { stave:{ "value":value*1.e3, "error":error*1.e3 } } )
            #print "{0:20s}: {1:10s} {2:9.2e} {3:9.2e} {4:8.2f}".format( stave, dof, value, error, value/error)
            
        """
        End of the parser
        """
        
    return alignresults


histos = []
canvases = []

#----------------------------------------------------------------------------------------------------
def gen_L16_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles ):
    pprint.pprint( alignlogfiles )
    
    """
    Loop is LB-segment (list) --> Iteration (list) --> Stave (dict)
    """
    ResultsArray = [ [ get_align_results( aligndata_iter ) for aligndata_iter in aligndata_segment  ] for aligndata_segment in alignlogfiles ]
    
    #pprint.pprint( ResultsArray )
    
    """
    Extract data for each stave
    """
    
    canvas = TCanvas("c_L16_summary", "L16 Summary", 0, 0, 1600, 1200)
    set_canvas_style( canvas )
    canvas.Divide(5, 3)
    
    canvas_err = TCanvas("c_L16_err", "L16 Summary Error", 0, 0, 1600, 1200)
    set_canvas_style( canvas_err )
    canvas_err.Divide(5, 3)
    
    for stave in range(0, 14):
        stave_name = 'PhiStave_{0}'.format( stave )
        
        segment_results = []
        hist = TH1F("hist_Stave{0}".format( stave ), \
                    "Run {0} Stave {1};LB Segment;Bowing Mag [#mum]".format( RunNumber, stave ), \
                    len(ResultsArray), 0, len(ResultsArray) )
        hist.GetYaxis().SetRangeUser(-9.0, 9.0)
        
        hist_err = TH1F("hist_err_Stave{0}".format( stave ), \
                        "Run {0} Stave {1};LB Segment;Bowing Mag Error [#mum]".format( RunNumber, stave ), \
                        len(ResultsArray), 0, len(ResultsArray) )
        hist_err.GetYaxis().SetRangeUser(-0.5, 0.5)
        
        for h in [ hist, hist_err ]:
            set_hist_style( h )
            histos.append( h )
            
        for result_segment in ResultsArray:
            
            segment_id = ResultsArray.index( result_segment )
            
            #print len( result_segment[1] )
            error = result_segment[1][stave_name]['error']
            residual = sum( [ result_segment[0][stave_name]['value'], result_segment[1][stave_name]['value'] ] )
            
            hist.SetBinContent( segment_id+1, residual )
            hist.SetBinError  ( segment_id+1, error    )
            
            hist_err.SetBinContent( segment_id+1, 0 )
            hist_err.SetBinError  ( segment_id+1, error    )
            
            if( len( LBHeads ) > 1 ):
                hist.GetXaxis().SetBinLabel( segment_id+1, \
                                             "{0}-{1}".format( LBHeads[segment_id], \
                                                               LBHeads[segment_id+1]-1 if segment_id < len(ResultsArray)-1 \
                                                                                       else LBlist[-1] ) )
                
                hist_err.GetXaxis().SetBinLabel( segment_id+1, \
                                                 "{0}-{1}".format( LBHeads[segment_id], \
                                                                   LBHeads[segment_id+1]-1 if segment_id < len(ResultsArray)-1 \
                                                                                           else LBlist[-1] ) )
            else:
                hist.GetXaxis().SetBinLabel( segment_id+1, \
                                                 "{0}-{1}".format( LBHeads[0], LBlist[-1] ) )
                
                
                hist_err.GetXaxis().SetBinLabel( segment_id+1, \
                                                     "{0}-{1}".format( LBHeads[0], LBlist[-1] ) )

            
        canvas.cd( stave+1 )
        gPad.SetTicks(0, 1)
        gPad.SetTopMargin(0.005)
        gPad.SetBottomMargin(0.3)
        gPad.SetLeftMargin(0.2)
        gPad.SetRightMargin(0.01)
        hist.GetXaxis().LabelsOption("v")
        hist.Draw("he")
    
        canvas_err.cd( stave+1 )
        gPad.SetTicks(0, 1)
        gPad.SetTopMargin(0.005)
        gPad.SetBottomMargin(0.3)
        gPad.SetLeftMargin(0.2)
        gPad.SetRightMargin(0.01)
        hist_err.GetXaxis().LabelsOption("v")
        hist_err.Draw("he")
    
    canvas.cd()
    canvas.Update()
    canvas.SaveAs("log_forEachRun/plots/Run{0}_L16.png".format(RunNumber) )
            
    canvas_err.cd()
    canvas_err.Update()
    canvas_err.SaveAs("log_forEachRun/plots/Run{0}_L16_err.png".format(RunNumber) )
            
    


#----------------------------------------------------------------------------------------------------
def judge_L16_result_segment( LBHead, alignIterLogs ):
    
    """
    This list holds two elements: Iter2 and Iter3 alignment results
    """
    aligndatasetL16 = []
    
    """
    Loop over Iter2 and Iter3
    """
    for alignlog in alignIterLogs:
        """
        alignresults will be stored to the above aligndatasetL11
        """
        alignresults = []
        
        """
        Scan the alignlogfile.txt of the correspondint Iter
        """
        for line in open( alignlog ):
            """
            This is a simple parser of alignlogfile
            The log file looks like as follows:
            
            ============================================
            ...
            Alignment parameters for module: Pixel/IBL
            Number of tracks passing: 2526044
            Number of hits seen:      2671829
            Number of tracks seen:    2526044
            TransX    0.0041452    +/- 4.2286e-05  
            TransY    -0.0039621   +/- 4.2487e-05  
            TransZ    0.00041507   +/- 0.00042143  
            RotX      5.0849e-06   +/- 1.4928e-07  
            RotY      1.4479e-05   +/- 1.4965e-07  
            RotZ      2.4469e-08   +/- 1.4183e-06  
            BowX      -0.026019    +/- 5.3556e-05  
            ...
            ============================================
            
            We need to identify the (value, error) of the given (stave, DoF).
            The stave is tagged using the line
            
            "Alignment parameters for module: Pixel/IBL"
            
            and then the tagged stave is used until the next similar line appears.
            
            The DoF line is tagged with tagging "+/-".
            
            """
            
            """
            Tagging the stave info
            """
            if line.find("Alignment parameters for module") >= 0:
                stave = (line[:-1].split(": ")[-1]).split("/")[-1]
                
            """
            Tagging the DoF info
            """
            if line.find("+/-") >= 0 :
                tokens = line.split()
                dof = tokens[0]
                value = float( tokens[1] )
                error = float( tokens[3] )
                
                """
                Store the set of {stave, dof, value, error} in alignresults
                """
                alignresults.append( { "stave":stave, "dof":dof, "value":value*1.e3, "error":error*1.e3 } )
                #print "{0:20s}: {1:10s} {2:9.2e} {3:9.2e} {4:8.2f}".format( stave, dof, value, error, value/error)
        """
        End of the parser
        """
        
        """
        Now store the alignresults of Iter-X to aligndatasetL11
        """
        aligndatasetL16.append( alignresults )
    
    
    """
    Merge Iter2 and Iter3 for the value. For error, just use Iter1.
    """
    alignResultIterSum = [ { "stave"   :alidof_iter2["stave"], \
                             "dof"     :alidof_iter2["dof"],   \
                             "value"   :alidof_iter2["value"]+alidof_iter3["value"], \
                             "error"   :alidof_iter3["error"] } \
                             for alidof_iter2, alidof_iter3 in zip( aligndatasetL16[0], aligndatasetL16[1] ) ]
    
    
    """
    check_validity() gives back True/False for a given DoF (e.g. IBL TransX).
    This is looped over for each DoF element of alignResultIterSum and listed as validity_flag
    """
    validity_flags = [ check_validity( elem["stave"], elem["dof"], elem["value"], elem["error"] ) for elem in alignResultIterSum ]
    
    """
    Print out the result of judge
    """
    print""
    print""
    print "============================================================"
    print " Result of the judge of validity for each DoF (LB : {0}~ )".format( LBHead )
    print "============================================================"
    for dofElem, validity_flag in zip( alignResultIterSum, validity_flags ):
        print "{0:20s} : {1:10s} {2:7.3f} um   ==> {3:20s}" \
            .format( dofElem["stave"], dofElem["dof"], dofElem["error"], "Valid" if validity_flag else "Invalid" )
    print "============================================================"
    
    """
    OK, the following line looks like a magic, but it just does a simple task.
    This gives back a one True/False for a given list of booleans [ T/F, T/F, T/F, ... ]
    If any False is found, the result is False.
    """
    
    judge = reduce( (lambda a, b: a and b), validity_flags ) if len( validity_flags ) > 1 else validity_flags[0]
    
    print ""
    print ""
    print "judge_L16_segment(): result = ", "Valid" if judge else "Invalid"
    print ""

    
    return judge
    



#----------------------------------------------------------------------------------------------------
def judge_L16_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles ):
    print""
    print""
    print"=========================================================="
    print"List of alignment log files"
    print"=========================================================="
    pprint.pprint( alignlogfiles )
    
    judgeFlags = [ judge_L16_result_segment( LBHead, alignIterLogs ) for LBHead, alignIterLogs in zip( LBHeads, alignlogfiles) ]
    
    
    if( os.getenv("run_mode") == "TEST_L16_DIAGNOSIS" ):
        test_index = int( os.getenv("L16_diagnosis_test_index") )
        print ""
        print "**** TEST_L16_DIAGNOSIS MODE ==> Forced to overwrite index {0} to False. ****".format( test_index )
        print ""
        tools.write_log_warning( "" )
        tools.write_log_warning( "**** WARNING:: TEST_L16_DIAGNOSIS MODE ==> Forced to overwrite index {0} to False. ****".format( test_index ) )
        tools.write_log_warning( "" )
        judgeFlags[ test_index ] = False
    
    judge = reduce( (lambda a, b : a and b), judgeFlags ) if len( judgeFlags ) > 1 else judgeFlags[0]
    
    print ""
    print ""
    print "judge_L16_result_using_errors(): result = ", "Valid" if judge else "Invalid"
    print ""
    print "============================================================"
    
    return judge, judgeFlags






#----------------------------------------------------------------------------------------------------
# main
# 
# Note: this main is only supposed to test the functions
if __name__ == "__main__":
    import os, sys, glob, pprint
    import CalibrationLoopChecker
    
    from ROOT import gROOT
    gROOT.SetBatch( True )
    
    RunNumber = 306448
    if len( sys.argv ) >= 2:
        RunNumber = int( sys.argv[1] )
    
    theRunDirectory = CalibrationLoopChecker.get_run_directory( RunNumber )
    
    
    """
    This processID stands for the suffix of the directory, like .c0, .c1
    """
    processID = CalibrationLoopChecker.get_latest_CLprocessID( theRunDirectory )
    
    """
    Define list variables
    - alignlogfiles: container of alignlogfile. This is supposed to store the alignlogfile as follows:
      [ [Segment0_Iter2, Segment0_Iter3],
        [Segment1_Iter2, Segment1_Iter3],
        [Segment2_Iter2, Segment2_Iter3], ... ]
    """
    alignlogfiles = []
    DbFiles       = []
    RootFiles     = []
    
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
            CalibrationLoopChecker.expand_tar_archive( tarfile )
        
            alignlogfile = workdir + "/alignlogfile.txt"
            
            alignlogfiles[ tarfiles.index(tarfile) ].append( alignlogfile )
            
            """
            Copy the .db and .root file from the TAR directory
            (only in the case if the last Iter)
            """
            if iteration == 3:
                list_DbFiles   = glob.glob( "{0}/*ROOT_DB*".format( workdir ) )
                if len( list_DbFiles ) == 1 :
                    DbFile = list_DbFiles[0]
                else:
                    print "ROOT_DB file is not unique!"
                    sys.exit()
                    
                list_RootFiles   = glob.glob( "{0}/*ROOT_CONSTANTS*".format( workdir ) )
                if len( list_RootFiles ) == 1 :
                    RootFile = list_RootFiles[0]
                else:
                    print "ROOT_DB file is not unique!"
                    sys.exit()
        
                """
                Copy the Db file and ROOT (POOL) file temporarily
                """
                filebasename = workdir.replace("TAR_ALIGNFILES.", "")
                os.system( "cp {0} {1}.db"  .format( DbFile,   filebasename ) )
                os.system( "cp {0} {1}.root".format( RootFile, filebasename ) )
                
                DbFiles   .append( "{0}.db"  .format( filebasename ) )
                RootFiles .append( "{0}.root".format( filebasename ) )
                
    
    print"=========================================================="
    print"List of DB files for LB-segment"
    print"=========================================================="
    pprint.pprint( alignlogfiles )
    
    print"=========================================================="
    print"List of DB files for LB-segment"
    print"=========================================================="
    pprint.pprint( DbFiles )
    
    
    print"=========================================================="
    print"List of ROOT files for LB-segment"
    print"=========================================================="
    pprint.pprint( RootFiles )
    
    
    """
    Get the RAW data file list and create LB segments
    """
    LBlist = []
    for line in open( CalibrationLoopChecker.get_raw_filelist( RunNumber ) ):
        """
        extract LB number from the filename like
        data15_13TeV.00284484.calibration_IDTracks.merge.RAW._lb0022._SFO-ALL._0001.1
        """
        
        LB = int( line.split(".")[-4][-4:] )
        LBlist.append( LB )
    
    LBlist = sorted( LBlist )
    
    LBHeads = filter( (lambda LB: (LBlist.index(LB)%20 == 0 and (LBlist[-1] - LB) > 20) if LBlist.index(LB) <= 60 else LBlist.index(LB)%100 == 59 and (LBlist[-1] - LB) > 100) , LBlist )
    
    print"=========================================================="
    print"List of LB segments"
    print"=========================================================="
    pprint.pprint( LBHeads )
    
    """
    Now, alignlogfiles lists up a pair of alignlogfiles (Iter2, Iter3) for each LB-segment.
    Check these results using the following macro.
    """
    judge_L16_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles )
    
    gen_L16_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles )

