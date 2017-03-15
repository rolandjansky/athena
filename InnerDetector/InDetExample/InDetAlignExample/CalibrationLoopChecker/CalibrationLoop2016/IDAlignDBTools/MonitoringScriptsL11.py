#!/usr/bin/python
# -*- coding:utf-8 -*-

################################################################################
#
# MonitoringScriptsL11.py
#
# Author: Hide Oide (Hideyuki.Oide@cern.ch)
#
# Date: April 18, 2016
#
# Description:
# Collection of monitoring functions to be used in CalibrationLoopChecker.py for L11
#
################################################################################


from ROOT import TH1F, TCanvas, TLine, gPad, gStyle
from ROOT import kBlack

import tools


hists = []
lines = []
canvases = []

#----------------------------------------------------------------------------------------------------
def set_hist_style( hist ):

    hist.SetMarkerStyle(20)
    hist.SetLineColor(kBlack)
    hist.SetLineWidth(2)
    hist.GetXaxis().SetLabelSize(0.04)
    hist.GetYaxis().SetLabelSize(0.04)
    hist.GetXaxis().SetTitleSize(0.05)
    hist.GetYaxis().SetTitleSize(0.05)
    hist.GetXaxis().SetTitleOffset(3)
    hist.GetYaxis().SetTitleOffset(1.4)

#----------------------------------------------------------------------------------------------------
def set_canvas_style( canvas ):
    canvas.SetBottomMargin(0.3)
    canvas.SetRightMargin(0.05)
    canvas.SetLeftMargin(0.15)
    canvas.SetTicks(1, 1)


#----------------------------------------------------------------------------------------------------
def gen_L11_result_plots( RunNumber, alignlogfiles ):
    gStyle.SetLineColor( kBlack )
    gStyle.SetOptStat(0)
    
    map_trans = { "TransX":1, "TransY":2, "TransZ":3 }
    
    #
    # Translation
    #
    h_Trans = TH1F("Translation", "Run {0} / L11 (Iter0+1)Translation;DoF;Translation [#mum]".format(RunNumber), 22, 0, 22)
    h_Trans_Error = TH1F("Trans_Error", "Run {0} / L11 (Iter0+1)Translation Error;DoF;Translation Error [#mum]".format(RunNumber), 22, 0, 22)
    for label,bin in map_trans.items():
        packages = [ "IBL", "Pixel", "SCT ECA", "SCT ECC", "TRT Barrel", "TRT ECA", "TRT ECC" ]
        for package in packages:
            h_Trans      .GetXaxis().SetBinLabel( bin+packages.index(package)*3, " / ".join( [package, label] ) )
            h_Trans_Error.GetXaxis().SetBinLabel( bin+packages.index(package)*3, " / ".join( [package, label] ) )
            
        h_Trans.GetXaxis()      .SetBinLabel( 22, " / ".join( [packages[0], "BowX"] ) )
        h_Trans_Error.GetXaxis().SetBinLabel( 22, " / ".join( [packages[0], "BowX"] ) )
            
    hists.append( h_Trans )
    hists.append( h_Trans_Error )
    
    #
    # Rotation
    #
    map_rot = { "RotX":1, "RotY":2, "RotZ":3 }
    
    h_Rot = TH1F("Rotlation", "Run {0} / L11 (Iter0+1)Rotation;DoF;Rotlation [mrad]".format(RunNumber) , 21, 0, 21)
    h_Rot_Error = TH1F("Rot_Error", "Run {0} / L11 (Iter0+1)Rotation Error;DoF;Rotlation Error [mrad]".format(RunNumber) , 21, 0, 21)
    for label,bin in map_rot.items():
        packages = [ "IBL", "Pixel", "SCT ECA", "SCT ECC", "TRT Barrel", "TRT ECA", "TRT ECC" ]
        for package in packages:
            h_Rot.GetXaxis()      .SetBinLabel( bin+packages.index(package)*3, " / ".join( [package, label] ) )
            h_Rot_Error.GetXaxis().SetBinLabel( bin+packages.index(package)*3, " / ".join( [package, label] ) )
        
    hists.append( h_Rot )
    hists.append( h_Rot_Error )
    
    # Histogram styles
    for hist in [ h_Trans, h_Trans_Error, h_Rot, h_Rot_Error ]:
        set_hist_style( hist )
    
    # Read the log file and fill the values to histograms
    for alignlogfile in alignlogfiles:
        for line in open( alignlogfile ):
            if line.find("Alignment parameters for module") >= 0:
                package = line[:-1].split(": ")[-1]
            if line.find("+/-") >= 0 :
                tokens = line.split()
                dof = tokens[0]
                value = float( tokens[1] )
                error = float( tokens[3] )
                #print "{0:20s}: {1:20s} {2:8.2e} {3:8.2e} {4:8.2f}".format( package, dof, value, error, value/error)
                
                if package == "Pixel/IBL" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof], value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof], error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof], error*1.e3)
                if package == "Pixel/IBL" and dof.find("Bow")>=0:
                    h_Trans.AddBinContent     (22, value*1.e3)
                    h_Trans.SetBinError       (22, error*1.e3)
                    h_Trans_Error.SetBinError (22, error*1.e3)
                if package == "Pixel/oldPixel" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof]+3, value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof]+3, error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof]+3, error*1.e3)
                if package == "SCT EndCap A" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof]+6, value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof]+6, error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof]+6, error*1.e3)
                if package == "SCT EndCap C" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof]+9, value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof]+9, error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof]+9, error*1.e3)
                if package == "TRT Barrel" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof]+12, value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof]+12, error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof]+12, error*1.e3)
                if package == "TRT EndCap A" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof]+15, value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof]+15, error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof]+15, error*1.e3)
                if package == "TRT EndCap C" and dof.find("Trans")>=0:
                    h_Trans.AddBinContent     (map_trans[dof]+18, value*1.e3)
                    h_Trans.SetBinError       (map_trans[dof]+18, error*1.e3)
                    h_Trans_Error.SetBinError (map_trans[dof]+18, error*1.e3)
                    
                if package == "Pixel/IBL" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof], value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof], error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof], error*1.e3)
                if package == "Pixel/oldPixel" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof]+3, value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof]+3, error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof]+3, error*1.e3)
                if package == "SCT EndCap A" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof]+6, value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof]+6, error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof]+6, error*1.e3)
                if package == "SCT EndCap C" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof]+9, value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof]+9, error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof]+9, error*1.e3)
                if package == "TRT Barrel" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof]+12, value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof]+12, error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof]+12, error*1.e3)
                if package == "TRT EndCap A" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof]+15, value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof]+15, error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof]+15, error*1.e3)
                if package == "TRT EndCap C" and dof.find("Rot")>=0:
                    h_Rot.AddBinContent       (map_rot[dof]+18, value*1.e3)
                    h_Rot.SetBinError         (map_rot[dof]+18, error*1.e3)
                    h_Rot_Error.SetBinError   (map_rot[dof]+18, error*1.e3)
                
        
    # Create canvases
    c_Trans       = TCanvas("c_Trans", "L11 (Iter0+1)Translation", 0, 0, 800, 600)
    c_Rot         = TCanvas("c_Rot", "L11 (Iter0+1)Rotation", 0, 0, 800, 600)
    c_Trans_Error = TCanvas("c_Trans_Error", "L11 (Iter0+1)Translation Error", 0, 0, 800, 600)
    c_Rot_Error   = TCanvas("c_Rot_Error", "L11 (Iter0+1)Rotation Error", 0, 0, 800, 600)
    
    # Canvas styles and register canvases to the global list
    for canvas in [ c_Trans, c_Rot, c_Trans_Error, c_Rot_Error ]:
        set_canvas_style( canvas )
        canvases.append( canvas )
    
    # Drawing histograms
    for canvas, hist, in zip( [ c_Trans, c_Rot, c_Trans_Error, c_Rot_Error ], [ h_Trans, h_Rot, h_Trans_Error, h_Rot_Error ] ):
        canvas.cd()
        hist.GetXaxis().LabelsOption("v")
        hist.Draw("pe" if hist.GetName().find("Error")>=0 else "pe")
        line0 = TLine(0.0, 0.0, hist.GetXaxis().GetXmax(), 0.0)
        line0.SetLineStyle(3)
        line0.Draw()
        for s in range(1, hist.GetNbinsX()/3+1):
            line_v = TLine(s*3, hist.GetMinimum(), s*3, hist.GetMaximum() )
            line_v.SetLineStyle(3)
            line_v.Draw()
            lines.append( line_v )
        gPad.Update()
        canvas.SaveAs( "log_forEachRun/plots/Run{0}_L11_{1}.png".format( RunNumber, canvas.GetName()[2:] ) )
        
        lines.append( line0 )
    


#----------------------------------------------------------------------------------------------------
def check_validity( package, dof, value, error) :
    
    if abs(value)/error > 4.0:
        tools.write_log_warning( "{0:20s} : {1:10s} {2:10.5f} +/- {3:10.5f} ==> {4:6.1f} sigma deviation".format( package, dof, value, error, abs(value)/error ) )
    
    if   package == "Pixel/IBL"      and dof == "TransX": return True if error < 0.5    else False
    elif package == "Pixel/IBL"      and dof == "TransY": return True if error < 0.5    else False
    elif package == "Pixel/IBL"      and dof == "TransZ": return True if error < 2.0    else False
    elif package == "Pixel/IBL"      and dof == "BowX"  : return True if error < 0.5    else False
    elif package == "Pixel/IBL"      and dof == "RotX"  : return True if error < 0.001  else False
    elif package == "Pixel/IBL"      and dof == "RotY"  : return True if error < 0.001  else False
    elif package == "Pixel/IBL"      and dof == "RotZ"  : return True if error < 0.010  else False
    
    elif package == "Pixel/oldPixel" and dof == "TransX": return True if error < 0.5    else False
    elif package == "Pixel/oldPixel" and dof == "TransY": return True if error < 0.5    else False
    elif package == "Pixel/oldPixel" and dof == "TransZ": return True if error < 2.0    else False
    elif package == "Pixel/oldPixel" and dof == "RotX"  : return True if error < 0.0005 else False
    elif package == "Pixel/oldPixel" and dof == "RotY"  : return True if error < 0.0005 else False
    elif package == "Pixel/oldPixel" and dof == "RotZ"  : return True if error < 0.0010 else False
    
    elif package == "SCT EndCap A"   and dof == "TransX": return True if error < 1.0    else False
    elif package == "SCT EndCap A"   and dof == "TransY": return True if error < 1.0    else False
    
    elif package == "SCT EndCap C"   and dof == "TransX": return True if error < 1.0    else False
    elif package == "SCT EndCap C"   and dof == "TransY": return True if error < 1.0    else False
    
    elif package == "SCT EndCap A"   and dof == "RotX"  : return True if error < 0.0015 else False
    elif package == "SCT EndCap A"   and dof == "RotY"  : return True if error < 0.0015 else False
    elif package == "SCT EndCap A"   and dof == "RotZ"  : return True if error < 0.0015 else False
    
    elif package == "SCT EndCap C"   and dof == "RotX"  : return True if error < 0.0015 else False
    elif package == "SCT EndCap C"   and dof == "RotY"  : return True if error < 0.0015 else False
    elif package == "SCT EndCap C"   and dof == "RotZ"  : return True if error < 0.0015 else False
    
    elif package == "TRT Barrel"     and dof == "TransX": return True if error < 0.5    else False
    elif package == "TRT Barrel"     and dof == "TransY": return True if error < 0.5    else False
    elif package == "TRT Barrel"     and dof == "RotX"  : return True if error < 0.0015 else False
    elif package == "TRT Barrel"     and dof == "RotY"  : return True if error < 0.0015 else False
    elif package == "TRT Barrel"     and dof == "RotZ"  : return True if error < 0.0010 else False
    
    elif package == "TRT EndCap A"   and dof == "TransX": return True if error < 1.0    else False
    elif package == "TRT EndCap A"   and dof == "TransY": return True if error < 1.0    else False
    elif package == "TRT EndCap A"   and dof == "RotZ"  : return True if error < 0.0015 else False
    
    elif package == "TRT EndCap C"   and dof == "TransX": return True if error < 1.0    else False
    elif package == "TRT EndCap C"   and dof == "TransY": return True if error < 1.0    else False
    elif package == "TRT EndCap C"   and dof == "RotZ"  : return True if error < 0.0015 else False
    
    else :
        print package, dof, " ==> Not found!!"
        return False
          


#----------------------------------------------------------------------------------------------------
def judge_L11_result_using_errors( RunNumber, alignlogfiles ):
    """
    Judge if the alignment calculation result is fine
    looking at each parameter's uncertainty value
    """
    
    """
    The following function just produces plots
    which summarises the status of L11 (Iter0 and Iter1)
    """
    gen_L11_result_plots( RunNumber, alignlogfiles )
    
    """
    This list holds two elements: Iter0 and Iter1 alignment results
    """
    aligndatasetL11 = []
    
    
    """
    Loop over alignlogfiles of Iter0 and Iter1
    """
    for alignlogfile in alignlogfiles:
        
        """
        alignresults will be stored to the above aligndatasetL11
        """
        alignresults = []
        
        
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
            
            We need to identify the (value, error) of the given (package, DoF).
            The package is tagged using the line
            
            "Alignment parameters for module: Pixel/IBL"
            
            and then the tagged package is used until the next similar line appears.
            
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
    alignResultIterSum = [ { "package" :alidof_iter0["package"], \
                             "dof"     :alidof_iter0["dof"],   \
                             "value"   :alidof_iter0["value"]+alidof_iter1["value"], \
                             "error"   :alidof_iter1["error"] } \
                             for alidof_iter0, alidof_iter1 in zip( aligndatasetL11[0], aligndatasetL11[1] ) ]
    
    """
    check_validity() gives back True/False for a given DoF (e.g. IBL TransX).
    This is looped over for each DoF element of alignResultIterSum and listed as validity_flag
    """
    validity_flags = [ check_validity( elem["package"], elem["dof"], elem["value"], elem["error"] ) for elem in alignResultIterSum ]
    
    
    
    """
    Print out the result of judge
    """
    print "============================================================"
    print " Result of the judge of validity for each DoF"
    print "============================================================"
    for dofElem, validity_flag in zip( alignResultIterSum, validity_flags ):
        print "{0:20s} : {1:10s} ==> {2:20s}".format( dofElem["package"], dofElem["dof"], "Valid" if validity_flag else "Invalid" )
        
        if not validity_flag :
            tools.write_log_error( "{0:20s} : {1:10s} value = {2:.3e}, error = {3:.3e} ==> {4:20s}".format( dofElem["package"], dofElem["dof"], float(dofElem["value"]), float(dofElem["error"]), "Valid" if validity_flag else "Invalid" ) )
    print "============================================================"
    
    
    
    """
    OK, the following line looks like a magic, but it just does a simple task.
    This gives back a one True/False for a given list of booleans [ T/F, T/F, T/F, ... ]
    If any False is found, the result is False.
    """
    judge = reduce( (lambda a, b: a and b), validity_flags )
    
    print ""
    print ""
    print "judge_L11_result_using_errors(): result = ", "Valid" if judge else "Invalid"
    print ""
    print "============================================================"
    print "============================================================"
    
    return judge


#----------------------------------------------------------------------------------------------------
# main
# 
# Note: this main is only supposed to test the functions
if __name__ == "__main__":
    import os, sys, glob
    import CalibrationLoopChecker
    
    from ROOT import gROOT
    gROOT.SetBatch( True )
    
    RunNumber = 284484 if len(sys.argv) < 1 else int(sys.argv[1])
    theRunDirectory = CalibrationLoopChecker.get_run_directory( RunNumber )
    
    """
    This processID stands for the suffix of the directory, like .c0, .c1
    """
    processID = CalibrationLoopChecker.get_latest_CLprocessID( theRunDirectory )
    
    
    
    """
    Identify the tar archive files
    """
    alignlogfiles = []
    
    for iteration in [0, 1]:
        tarfiles = glob.glob( "{0}/*TAR_ALIGNFILES.Iter{1}*{2}/*".format(theRunDirectory, iteration, processID) )
        if len( tarfiles ) == 1 :
            tarfile = tarfiles[0]
            workdir = os.path.basename( tarfile )
        elif len( tarfiles ) == 0 :
            print "NULL files were found --> L11 alignment result is missing."
            sys.exit()
        else:
            print "TAR_ALIGNFILES are not unique!!"
            print tarfiles
            sys.exit()
        
        
        """
        Expand the tar archive
        """
        CalibrationLoopChecker.expand_tar_archive( tarfile )
        
        
        alignlogfile = workdir + "/alignlogfile.txt"
        
        alignlogfiles.append( alignlogfile )
    
    judge_L11_result_using_errors( RunNumber, alignlogfiles )
    
