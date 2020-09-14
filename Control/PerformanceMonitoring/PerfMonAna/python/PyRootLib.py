# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PyRootLib.py
# @purpose: a set of classes to ease the day-to-day work with ROOT
# @author: Sebastien Binet <binet@cern.ch>

""" a set of classes to ease the day-to-day work with ROOT
"""
#
#
__author__  = 'Sebastien Binet'
__doc__     = "a set of classes to ease the day-to-day work with ROOT."

### globals -------------------------------------------------------------------
_root_files = []

def importRoot( batch = True ):
    import os
    import ROOT
    x_display = None
    if batch:
        ROOT.gROOT.SetBatch(True)
        if 'DISPLAY' in os.environ:
            x_display = os.environ['DISPLAY']
            del os.environ['DISPLAY']
    
    ## few customizations
    ROOT.gErrorIgnoreLevel = ROOT.kError
    if ROOT.gROOT.GetVersionInt() >= 51800:
        ROOT.kOrange
        ROOT.kMyBlue    = ROOT.kBlue -4
        ROOT.kMyRed     = ROOT.kRed  -4
        ROOT.kMyGreen   = ROOT.kGreen+1
        ROOT.kDarkGreen = ROOT.kGreen +1
    else:
        ROOT.kOrange    = ROOT.kYellow
        ROOT.kMyBlue    = ROOT.kBlue
        ROOT.kMyRed     = ROOT.kRed
        ROOT.kMyGreen   = ROOT.kGreen+100
        ROOT.kDarkGreen = ROOT.kGreen +100
        
    ##
    if batch:
        if x_display:
            os.environ['DISPLAY'] = x_display
    #
    # Root has a global dtor ordering problem: the cintex trampolines
    # may be deleted before open files are closed.  Workaround is to explicitly
    # close open files before terminating.
    #
    global _root_files
    def _close_files():
        global _root_files
        for f in _root_files:
            if hasattr (f, 'Close'):
                f.Close()
        del _root_files[0:-1]
        return
    import atexit
    atexit.register(_close_files)

    # monkey-patch TFile.Open to populate the _allfiles attribute
    def fopen(*args):
        global _root_files
        f = ROOT.TFile.Open(*args)
        if not f in _root_files:
            _root_files.append(f)
        return f
    ROOT.fopen = fopen
    return ROOT

ROOT = importRoot()

def setupRootStyle():
    """Somehow beautify the ugly default ROOT display style"""
        
    style = ROOT.gROOT.GetStyle( "Plain" )

    # no borders, white color
    style.SetCanvasColor(ROOT.kWhite)
    style.SetTitleFillColor(ROOT.kWhite)
    #style.SetTitleBorderSize(0)

    # title in the middle
    #style.SetTitleAlign(23)
    style.SetTitleX(0.30)
    style.SetTitleY(0.97)
    style.SetTitleW(0.60)
    #style.SetPadTopMargin(0.15)
    style.SetTitleSize(0.035)
    style.SetTitleOffset(1.1)
    style.SetTitleSize(0.035,"Y")
    style.SetTitleOffset(1.1,"Y")
    
    # canvas stuff
    style.SetCanvasBorderSize(0)
    style.SetCanvasDefH( 800)
    style.SetCanvasDefW(1000)
    style.SetFrameBorderMode(0)
    style.SetFrameBorderSize(0)
    
    style.SetStatX(0.95)
    style.SetStatY(0.9)
    style.SetStatW(0.18)
    style.SetStatH(0.2)
    style.SetStatColor(ROOT.kWhite)
    # style.SetAxisColor( ROOT.kBlack )
    # style.SetAxisColor( ROOT.kBlack, "Y")
    # style.SetLabelColor( ROOT.kBlack )
    # style.SetLabelColor( ROOT.kBlack, "Y" )
    style.SetLabelSize(0.04)
    style.SetLabelSize(0.04,"Y")

##         style.SetPadTopMargin(0.05);
##         style.SetPadRightMargin(0.05);
##         style.SetPadBottomMargin(0.16);
##         style.SetPadLeftMargin(0.15);

    style.SetPalette(1)
    #style.SetOptStat(111111)
    style.SetOptStat(0)
    
    ROOT.gROOT.SetStyle( "Plain" )
    ROOT.gROOT.ForceStyle()

    return

def rtString( inStr ):
    """ normalize an input string (ie: it makes sure that it can be used by
    ROOT as an id). eg: replaces '/' by '_'.
    """
    outStr = inStr\
             .replace( '/', '_' )\
             .replace( ':', '_' )\
             .replace( '#', '_' )
    return outStr

class Linear:
   def __call__(self, x, par):
      return par[0] + x[0]*par[1]

class Polynom:
    def __init__(self, degree=1):
        self.n = degree
    def __call__(self, x, par):
        return sum( (x[0]**i) + par[i] for i in range(self.n+1) )
    
class OptionStyle:
    """
    Struct to hold options (color/width/style) for a TStyle-like object
    """
    def __init__(self, color=ROOT.kRed, width=2, style=ROOT.kLineSolid):
        self.color = color
        self.width = width
        self.style = style
        return

    def setColor(self, color):
        self.color = color
        return
    def setWidth(self,width):
        self.width = width
        return
    def setStyle(self,style):
        self.style = style
        return

class Style(object):
    """
    Simple class to automate a few boring things when comes to TStyle in ROOT
    """
    def __init__(self,
                 lineOptions=None, markerOptions=None, fillStyle=None):
        object.__init__(self)
        self.line   = lineOptions
        self.marker = markerOptions
        self.fillStyle = fillStyle
        
        if self.line   == None: self.line   = OptionStyle()
        if self.marker == None: self.marker = OptionStyle()
        return
    def getLine(self):
        return self.line
    def getMarker(self):
        return self.marker
    
