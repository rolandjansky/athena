#!/usr/bin/python
# -*- coding:utf-8 -*-

################################################################################
#
# MonitoringScriptsL11.py
#
# Author: Hide Oide (Hideyuki.Oide@cern.ch)
#
# Update: Aug 21, 2016
#
# Description:
# Collection of monitoring functions to be used in CalibrationLoopChecker.py for L11
#
################################################################################


from ROOT import TH1F, TCanvas, TLine, gPad, gStyle, TLegend
from ROOT import kBlack, kCyan, kRed, kGreen, kYellow, kMagenta

import os, sys, glob, pprint
import tools
import numpy


gHists = []
lines = []
canvases = []

#----------------------------------------------------------------------------------------------------
def set_hist_style( hist ):

    hist.SetLineWidth(2)
    hist.SetTitleSize(0.3)
    
    hist.GetXaxis().SetNdivisions(505);
    hist.GetXaxis().SetLabelSize(0.06)
    hist.GetXaxis().SetTitleSize(0.07)
    hist.GetXaxis().SetTitleOffset(1.4)
    
    hist.GetYaxis().SetLabelSize(0.05)
    hist.GetYaxis().SetTitleSize(0.07)
    hist.GetYaxis().SetTitleOffset(1.3)
    
    hist.SetStats(0)

#----------------------------------------------------------------------------------------------------
def set_canvas_style( canvas ):
    canvas.SetBottomMargin(0.3)
    canvas.SetRightMargin(0.05)
    canvas.SetLeftMargin(0.15)
    canvas.SetTicks(1, 1)


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
        
        if line.find("Alignment parameters for module") >= 0:
            package = line[:-1].split(": ")[-1]
        if line.find("+/-") >= 0 :
            tokens = line.split()
            dof = tokens[0]
            value = float( tokens[1] )
            error = float( tokens[3] )
            #print "{0:20s}: {1:20s} {2:8.2e} {3:8.2e} {4:8.2f}".format( package, dof, value, error, value/(error+1.e-9) )

        
            """
            Store the set of {package, dof, value, error} in alignresults
            """
            alignresults.update( { (package,dof) : { "value":value*1.e3, "error":error*1.e3 } } )

            
        """
        End of the parser
        """
        
    return alignresults

#----------------------------------------------------------------------------------------------------
def get_dof_tuples():
    dof_tuples = []
    for package in [ "Pixel/IBL", "Pixel/oldPixel", "SCT EndCap A", "SCT EndCap C", "TRT Barrel", "TRT EndCap A", "TRT EndCap C" ]:
        dof_tuples.append( (package, "TransX" ) ) 
        dof_tuples.append( (package, "TransY" ) ) 
        dof_tuples.append( (package, "TransZ" ) ) 
        dof_tuples.append( (package, "RotX" ) ) 
        dof_tuples.append( (package, "RotY" ) ) 
        dof_tuples.append( (package, "RotZ" ) ) 
        if package == "Pixel/IBL" :
            dof_tuples.append( (package, "BowX" ) ) 
    
    return dof_tuples

#----------------------------------------------------------------------------------------------------
def gen_L11_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles ):
    gStyle.SetLineColor( kBlack )
    gStyle.SetOptStat(0)
    gStyle.SetPadLeftMargin(0.15)
    gStyle.SetPadBottomMargin(0.25)
    gStyle.SetPadRightMargin(0.01)
    gStyle.SetTitleOffset(1.3)
    
    #pprint.pprint( alignlogfiles )
    
    """
    Loop is LB-segment (list) --> Iteration (list) --> package-dof (dict)
    """
    ResultsArray = [ [ get_align_results( aligndata_iter ) for aligndata_iter in aligndata_segment  ] for aligndata_segment in alignlogfiles ]
    
    #pprint.pprint( ResultsArray )
    
    map_package_color = { "Pixel/IBL"      :kMagenta,
                          "Pixel/oldPixel" :kBlack,
                          "SCT EndCap A"   :kRed,
                          "SCT EndCap C"   :kRed,
                          "TRT Barrel"     :kGreen+1,
                          "TRT EndCap A"   :kGreen+1,
                          "TRT EndCap C"   :kGreen+1 }
    
    map_package_marker = { "Pixel/IBL"      :21,
                           "Pixel/oldPixel" :21,
                           "SCT EndCap A"   :20,
                           "SCT EndCap C"   :20,
                           "TRT Barrel"     :22,
                           "TRT EndCap A"   :22,
                           "TRT EndCap C"   :22 }
    
    map_package_style = { "Pixel/IBL"      :1,
                          "Pixel/oldPixel" :1,
                          "SCT EndCap A"   :2,
                          "SCT EndCap C"   :3,
                          "TRT Barrel"     :1,
                          "TRT EndCap A"   :2,
                          "TRT EndCap C"   :3 }
    
    map_dof_pad = { "TransX":1, "TransY":2, "TransZ":3, "BowX":4,
                    "RotX":5, "RotY":6, "RotZ":7 }
    
    canvas = TCanvas("c_L11_summary", "L16 Summary", 0, 0, 2400, 1200)
    set_canvas_style( canvas )
    canvas.SetBottomMargin(0.5)
    canvas.SetRightMargin(0.01)
    canvas.SetLeftMargin(0.10)
    canvas.SetTicks(1, 1)
    canvas.Divide(4, 2)
    
    dof_hists = { "TransX":[], "TransY":[], "TransZ":[], "RotX":[], "RotY":[], "RotZ":[], "BowX":[] }

    sections = [float(head) for head in LBHeads] + [ float( LBlist[-1] ) ]
    
    for dof_tuple in get_dof_tuples():
        package = dof_tuple[0]
        dof     = dof_tuple[1]
        
        hist = TH1F("h_{0}_{1}".format( package, dof ),
                    "Run {0} / L11 (Iter0+1) {1};LB;{1} {2}".format( RunNumber, dof, "[mrad]" if dof.find("Rot")==0 else "[#mum]" ),
                    len(LBHeads), numpy.array(sections).flatten('C') )
        
        set_hist_style( hist )
        for func in [ hist.SetMarkerColor, hist.SetLineColor ]:
            func( map_package_color[ package ] )
        hist.SetLineStyle( map_package_style[ package ] )
        hist.SetMarkerStyle( map_package_marker[ package ] )
        hist.SetLineWidth(2)
        hist.SetMarkerSize(1.3)
        
        gHists.append( hist )
        dof_hists[dof].append( hist )
        
        for result_segment in ResultsArray:
            segment_id = ResultsArray.index( result_segment )
            
            try:
                error = result_segment[1][dof_tuple]['error']
                residual = sum( [ result_segment[0][dof_tuple]['value'], result_segment[1][dof_tuple]['value'] ] )
                
                hist.SetBinContent( segment_id+1, residual )
                hist.SetBinError  ( segment_id+1, error )
            except:
                continue
        
            """
            if( len( LBHeads ) > 1 ):
                hist.GetXaxis().SetBinLabel( segment_id+1,
                                             "{0}-{1}".format( LBHeads[segment_id],
                                                               LBHeads[segment_id+1]-1 if segment_id < len(ResultsArray)-1 else LBlist[-1] ) )
                
            else:
                hist.GetXaxis().SetBinLabel( segment_id+1,
                                             "{0}-{1}".format( LBHeads[0], LBlist[-1] ) )
            """
                

        canvas.cd( map_dof_pad[ dof ] )
        hist.Draw("he" if package == "Pixel/IBL" else "he,same")
    
    for dof, hists in dof_hists.items():
        v_min = min( [ hist.GetMinimum() for hist in hists ] )
        v_max = max( [ hist.GetMaximum() for hist in hists ] )
        for hist in hists:
            hist.GetYaxis().SetRangeUser( (v_max+v_min)/2.0-(v_max-v_min), (v_max+v_min)/2.0+(v_max-v_min) )
            
    canvas.cd(8)
    lgd = TLegend(0.1, 0.2, 0.9, 0.95)
    lgd.SetBorderSize(0)
    lgd.SetFillColor(0)
    for hist in dof_hists["TransX"]:
        lgd.AddEntry(hist, hist.GetName().split("_")[1], "lp")
    lgd.Draw()
    
    canvas.Update()
    
    canvases.append( canvas )
    canvas.SaveAs( "log_forEachRun/plots/Run{0}_{1}.png".format( RunNumber, canvas.GetName()[2:] ) )


    
#----------------------------------------------------------------------------------------------------
def check_validity( package, dof, value, error, error_scale) :
    
    tools.write_log_debug( "{0:20s} : {1:10s} {2:10.5f} +/- {3:10.5f} ==> {4:6.1f} sigma deviation".format( package, dof, value, error, abs(value)/(error+1.e-9) ) )
    """
    if abs(value)/(error +1.e-9)> 4.0:
        tools.write_log_warning( "{0:20s} : {1:10s} {2:10.5f} +/- {3:10.5f} ==> {4:6.1f} sigma deviation".format( package, dof, value, error, abs(value)/error ) )
    """
    
    if os.getenv("L11_CRITERIA") == None or os.getenv("L11_CRITERIA") == "TIGHT":
        if   package == "Pixel/IBL"      and dof == "TransX": return True if error/error_scale < 0.5    else False
        elif package == "Pixel/IBL"      and dof == "TransY": return True if error/error_scale < 0.5    else False
        elif package == "Pixel/IBL"      and dof == "TransZ": return True if error/error_scale < 2.0    else False
        elif package == "Pixel/IBL"      and dof == "BowX"  : return True if error/error_scale < 0.5    else False
        elif package == "Pixel/IBL"      and dof == "RotX"  : return True if error/error_scale < 0.005  else False
        elif package == "Pixel/IBL"      and dof == "RotY"  : return True if error/error_scale < 0.005  else False
        elif package == "Pixel/IBL"      and dof == "RotZ"  : return True if error/error_scale < 0.010  else False
        
        elif package == "Pixel/oldPixel" and dof == "TransX": return True if error/error_scale < 0.5    else False
        elif package == "Pixel/oldPixel" and dof == "TransY": return True if error/error_scale < 0.5    else False
        elif package == "Pixel/oldPixel" and dof == "TransZ": return True if error/error_scale < 2.0    else False
        elif package == "Pixel/oldPixel" and dof == "RotX"  : return True if error/error_scale < 0.0010 else False
        elif package == "Pixel/oldPixel" and dof == "RotY"  : return True if error/error_scale < 0.0010 else False
        elif package == "Pixel/oldPixel" and dof == "RotZ"  : return True if error/error_scale < 0.0010 else False
        
        elif package == "SCT EndCap A"   and dof == "TransX": return True if error/error_scale < 1.0    else False
        elif package == "SCT EndCap A"   and dof == "TransY": return True if error/error_scale < 1.0    else False
        
        elif package == "SCT EndCap C"   and dof == "TransX": return True if error/error_scale < 1.0    else False
        elif package == "SCT EndCap C"   and dof == "TransY": return True if error/error_scale < 1.0    else False
        
        elif package == "SCT EndCap A"   and dof == "RotX"  : return True if error/error_scale < 0.0015 else False
        elif package == "SCT EndCap A"   and dof == "RotY"  : return True if error/error_scale < 0.0015 else False
        elif package == "SCT EndCap A"   and dof == "RotZ"  : return True if error/error_scale < 0.0015 else False
        
        elif package == "SCT EndCap C"   and dof == "RotX"  : return True if error/error_scale < 0.0015 else False
        elif package == "SCT EndCap C"   and dof == "RotY"  : return True if error/error_scale < 0.0015 else False
        elif package == "SCT EndCap C"   and dof == "RotZ"  : return True if error/error_scale < 0.0015 else False
        
        elif package == "TRT Barrel"     and dof == "TransX": return True if error/error_scale < 0.5    else False
        elif package == "TRT Barrel"     and dof == "TransY": return True if error/error_scale < 0.5    else False
        elif package == "TRT Barrel"     and dof == "RotX"  : return True if error/error_scale < 0.0015 else False
        elif package == "TRT Barrel"     and dof == "RotY"  : return True if error/error_scale < 0.0015 else False
        elif package == "TRT Barrel"     and dof == "RotZ"  : return True if error/error_scale < 0.0010 else False
        
        elif package == "TRT EndCap A"   and dof == "TransX": return True if error/error_scale < 2.0    else False
        elif package == "TRT EndCap A"   and dof == "TransY": return True if error/error_scale < 2.0    else False
        elif package == "TRT EndCap A"   and dof == "RotX"  : return True if error/error_scale < 0.0020 else False
        elif package == "TRT EndCap A"   and dof == "RotY"  : return True if error/error_scale < 0.0020 else False
        elif package == "TRT EndCap A"   and dof == "RotZ"  : return True if error/error_scale < 0.0020 else False
        
        elif package == "TRT EndCap C"   and dof == "TransX": return True if error/error_scale < 2.0    else False
        elif package == "TRT EndCap C"   and dof == "TransY": return True if error/error_scale < 2.0    else False
        elif package == "TRT EndCap C"   and dof == "RotX"  : return True if error/error_scale < 0.0020 else False
        elif package == "TRT EndCap C"   and dof == "RotY"  : return True if error/error_scale < 0.0020 else False
        elif package == "TRT EndCap C"   and dof == "RotZ"  : return True if error/error_scale < 0.0020 else False
        
        else :
            print package, dof, " ==> Not found!!"
            return False
    
    elif os.getenv("L11_CRITERIA") == "LOOSE":
        if   package == "Pixel/IBL"      and dof == "TransX": return True if error/error_scale < 0.8    else False
        elif package == "Pixel/IBL"      and dof == "TransY": return True if error/error_scale < 0.8    else False
        elif package == "Pixel/IBL"      and dof == "TransZ": return True if error/error_scale < 10.0   else False
        elif package == "Pixel/IBL"      and dof == "BowX"  : return True if error/error_scale < 0.5    else False
        elif package == "Pixel/IBL"      and dof == "RotX"  : return True if error/error_scale < 0.005  else False
        elif package == "Pixel/IBL"      and dof == "RotY"  : return True if error/error_scale < 0.005  else False
        elif package == "Pixel/IBL"      and dof == "RotZ"  : return True if error/error_scale < 0.010  else False
        
        elif package == "Pixel/oldPixel" and dof == "TransX": return True if error/error_scale < 0.8    else False
        elif package == "Pixel/oldPixel" and dof == "TransY": return True if error/error_scale < 0.8    else False
        elif package == "Pixel/oldPixel" and dof == "TransZ": return True if error/error_scale < 10.0    else False
        elif package == "Pixel/oldPixel" and dof == "RotX"  : return True if error/error_scale < 0.0050 else False
        elif package == "Pixel/oldPixel" and dof == "RotY"  : return True if error/error_scale < 0.0050 else False
        elif package == "Pixel/oldPixel" and dof == "RotZ"  : return True if error/error_scale < 0.0050 else False
        
        elif package == "SCT EndCap A"   and dof == "TransX": return True if error/error_scale < 10.0    else False
        elif package == "SCT EndCap A"   and dof == "TransY": return True if error/error_scale < 10.0    else False
        
        elif package == "SCT EndCap C"   and dof == "TransX": return True if error/error_scale < 10.0    else False
        elif package == "SCT EndCap C"   and dof == "TransY": return True if error/error_scale < 10.0    else False
        
        elif package == "SCT EndCap A"   and dof == "RotX"  : return True if error/error_scale < 0.015 else False
        elif package == "SCT EndCap A"   and dof == "RotY"  : return True if error/error_scale < 0.015 else False
        elif package == "SCT EndCap A"   and dof == "RotZ"  : return True if error/error_scale < 0.015 else False
        
        elif package == "SCT EndCap C"   and dof == "RotX"  : return True if error/error_scale < 0.015 else False
        elif package == "SCT EndCap C"   and dof == "RotY"  : return True if error/error_scale < 0.015 else False
        elif package == "SCT EndCap C"   and dof == "RotZ"  : return True if error/error_scale < 0.015 else False
        
        elif package == "TRT Barrel"     and dof == "TransX": return True if error/error_scale < 2.0    else False
        elif package == "TRT Barrel"     and dof == "TransY": return True if error/error_scale < 2.0    else False
        elif package == "TRT Barrel"     and dof == "RotX"  : return True if error/error_scale < 0.015 else False
        elif package == "TRT Barrel"     and dof == "RotY"  : return True if error/error_scale < 0.015 else False
        elif package == "TRT Barrel"     and dof == "RotZ"  : return True if error/error_scale < 0.010 else False
        
        elif package == "TRT EndCap A"   and dof == "TransX": return True if error/error_scale < 8.0    else False
        elif package == "TRT EndCap A"   and dof == "TransY": return True if error/error_scale < 8.0    else False
        elif package == "TRT EndCap A"   and dof == "RotX"  : return True if error/error_scale < 0.020 else False
        elif package == "TRT EndCap A"   and dof == "RotY"  : return True if error/error_scale < 0.020 else False
        elif package == "TRT EndCap A"   and dof == "RotZ"  : return True if error/error_scale < 0.020 else False
        
        elif package == "TRT EndCap C"   and dof == "TransX": return True if error/error_scale < 8.0    else False
        elif package == "TRT EndCap C"   and dof == "TransY": return True if error/error_scale < 8.0    else False
        elif package == "TRT EndCap C"   and dof == "RotX"  : return True if error/error_scale < 0.020 else False
        elif package == "TRT EndCap C"   and dof == "RotY"  : return True if error/error_scale < 0.020 else False
        elif package == "TRT EndCap C"   and dof == "RotZ"  : return True if error/error_scale < 0.020 else False
        
        else :
            print package, dof, " ==> Not found!!"
            return False
          


#----------------------------------------------------------------------------------------------------
def judge_L11_result_segment( LBHead, alignIterLogs, error_scale ):
    
    """
    This list holds two elements: Iter0 and Iter1 alignment results
    """
    aligndatasetL11 = []
    
    """
    Loop over Iter0 and Iter1
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
            Tagging the package info
            """
            if line.find("Alignment parameters for module") >= 0:
                package = line[:-1].split(": ")[-1]
            
            """
            Tagging the DoF info
            """
            if line.find("+/-") >= 0 :
                tokens = line.split()
                dof   = tokens[0]
                value = float( tokens[1] )
                error = float( tokens[3] )
                
                """
                Store the set of {package, dof, value, error} in alignresults
                """
                alignresults.append( { "package":package, "dof":dof, "value":value*1.e3, "error":error*1.e3 } )
                #print "{0:20s}: {1:10s} {2:9.2e} {3:9.2e} {4:8.2f}".format( stave, dof, value, error, value/error)
        """
        End of the parser
        """
        
        """
        Now store the alignresults of Iter-X to aligndatasetL11
        """
        aligndatasetL11.append( alignresults )
    
    
    """
    Merge Iter0 and Iter1 for the value. For error, just use Iter1.
    """
    alignResultIterSum = [ { "package" :alidof_iter2["package"], \
                             "dof"     :alidof_iter2["dof"],   \
                             "value"   :alidof_iter2["value"]+alidof_iter3["value"], \
                             "error"   :alidof_iter3["error"] } \
                             for alidof_iter2, alidof_iter3 in zip( aligndatasetL11[0], aligndatasetL11[1] ) ]
    
    
    """
    check_validity() gives back True/False for a given DoF (e.g. IBL TransX).
    This is looped over for each DoF element of alignResultIterSum and listed as validity_flag
    """
    validity_flags = [ check_validity( elem["package"], elem["dof"], elem["value"], elem["error"], error_scale ) for elem in alignResultIterSum ]
    
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
            .format( dofElem["package"], dofElem["dof"], dofElem["error"], "Valid" if validity_flag else "Invalid" )
    print "============================================================"
    
    """
    OK, the following line looks like a magic, but it just does a simple task.
    This gives back a one True/False for a given list of booleans [ T/F, T/F, T/F, ... ]
    If any False is found, the result is False.
    """
    
    judge = reduce( (lambda a, b: a and b), validity_flags ) if len( validity_flags ) > 1 else validity_flags[0]
    
    print ""
    print ""
    print "judge_L11_segment(): result = ", "Valid" if judge else "Invalid"
    print ""

    
    return judge



#----------------------------------------------------------------------------------------------------
def judge_L11_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles ):
    print""
    print""
    print"=========================================================="
    print"List of alignment log files"
    print"=========================================================="
    pprint.pprint( alignlogfiles )
    
    error_scales = [ 2.2 if index < 3 else 1.0 for index in range(0, len(LBHeads) ) ]
    
    judgeFlags = [ judge_L11_result_segment( LBHead, alignIterLogs, error_scale ) for LBHead, alignIterLogs, error_scale in zip( LBHeads, alignlogfiles, error_scales) ]
    
    
    if( os.getenv("run_mode") == "TEST_L11_DIAGNOSIS" ):
        test_index = int( os.getenv("L11_diagnosis_test_index") )
        print ""
        print "**** TEST_L11_DIAGNOSIS MODE ==> Forced to overwrite index {0} to False. ****".format( test_index )
        print ""
        tools.write_log_warning( "" )
        tools.write_log_warning( "**** WARNING:: TEST_L11_DIAGNOSIS MODE ==> Forced to overwrite index {0} to False. ****".format( test_index ) )
        tools.write_log_warning( "" )
        judgeFlags[ test_index ] = False
    
    judge = reduce( (lambda a, b : a and b), judgeFlags ) if len( judgeFlags ) > 1 else judgeFlags[0]
    
    print ""
    print ""
    print "judge_L11_result_using_errors(): result = ", "Valid" if judge else "Invalid"
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
      [ [Segment0_Iter0, Segment0_Iter1],
        [Segment1_Iter0, Segment1_Iter1],
        [Segment2_Iter0, Segment2_Iter1], ... ]
    """
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
            CalibrationLoopChecker.expand_tar_archive( tarfile )
        
            alignlogfile = workdir + "/alignlogfile.txt"
            
            alignlogfiles[ tarfiles.index(tarfile) ].append( alignlogfile )
            
            """
            Copy the .db and .root file from the TAR directory
            (only in the case if the last Iter)
            """
            if iteration == 1:
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
        
                
    
    print"=========================================================="
    print"List of align log files for LB-segment"
    print"=========================================================="
    
    
    """
    Get the RAW data file list and create LB segments
    """
    LBlist = []
    with open( CalibrationLoopChecker.get_raw_filelist( RunNumber ) ) as file:
        for line in file:
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
    Now, alignlogfiles lists up a pair of alignlogfiles (Iter0, Iter1) for each LB-segment.
    Check these results using the following macro.
    """
    judge_L11_result_using_errors( RunNumber, LBlist, LBHeads, alignlogfiles )
    
    gen_L11_result_plots( RunNumber, LBlist, LBHeads, alignlogfiles )

