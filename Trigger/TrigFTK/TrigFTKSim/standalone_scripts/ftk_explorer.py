#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import cmd
import os.path
import sys
import glob
import math
import code
#import pickle
import cPickle as pickle
import threading, time

import traceback
import cProfile

import optparse

# ROOT is required to load the FTK objects
import ROOT
from array import array

#
#  Global Debug
#
debug = False


import ftkutils

try :
    import cftkutils
    print "cftkutils module found."
except ImportError as errmsg:
    print errmsg
    print "To enable the C++ extension try to compile it with \"make\". The Makefile could need adjustements\nThe TrigFTKSim/standalone directory has also do be added in the LD_LIBRARY_PATH."
    sys.exit(-1)

ftkexpl_version = "3.6"
options_parser = optparse.OptionParser(usage="%prog [opts] ftkoutput1 [ftkoutput2 ....]",version="%prog " + ftkexpl_version)
options_parser.add_option("-r","--num-region",dest="numregions",
                          help="Number of regions",default=8,
                          metavar="NUM", type="int")
options_parser.add_option("-s","--num-subregions",dest="numsubregions",
                          help="Number of subregions",default=16,
                          metavar="NUM",type="int")
options_parser.add_option("-L","--num-layers",dest="nlayers",
                          help="Number of layers",default=7,
                          metavar="NUM",type="int")
options_parser.add_option("-C","--concatenate-caches",dest="concatenate",
                          help="Decide to merge the result of restored caches",default=False,
                          action="store_true")
options_parser.add_option("-i","--input",dest="inputfile",
                          help="Command inputs",default=None)
options_parser.add_option("-w","--wrappers",dest="wrapperfiles",
                          help="List of wrapper files as: comma separated list of wrapper files, directory, or ascii files. Used for truth comparison. The wrapper and otuput files are restricted to the same elements",default=None)
options_parser.add_option("-o","--root-file",dest="histofile",
                          help="ROOT output file",default="ExplorerHistos.root")
options_parser.add_option("-R","--restore",dest="restore",action="append",
                          help="path of a previous session to restore")
options_parser.add_option("-S","--savestate",dest="savestate",
                          help="path where to save the current status at the end",
                          default="ExplorerHistosCache.pickle")
options_parser.add_option("-F","--fast-connect",dest="fastconnect",
                          default=False,action="store_true",
                          help="Get the number of entries from the first valid ntuple, skip many constintency checks")
options_parser.add_option("-A","--athena-input",dest="athena",
                         default=False,action="store_true")
options_parser.add_option("-v","--verbose",dest="verbose",
                          help="Increment the verbosity level",default=0,
                          action="count")
options_parser.add_option("","--indir",dest="indir", help="Directory containing multiple raw_* directories for inputs.  For use, e.g., with effchain output.  ftk_explorer outputs are ignored if this option is used.",default=None)
options_parser.add_option("","--no-connect",dest="connect",
                          help="Disable the connection to the TTree, ignored for restored sssion (use --reconnect)",
                          default=True,action="store_false")
options_parser.add_option("","--reconnect",dest="reconnect",
                          help="Enable the connection to the TTrees for restored sessions",
                          default=False,action="store_true")
options_parser.add_option("","--profile",dest="profile",default=False,action="store_true",
                          help="Profile the code")
options_parser.add_option("","--latex",dest="latextable",default=False,action="store_true",
                          help="Save stats in latex table format")
options_parser.add_option("","--batch",dest="batchMode",default=False,action="store_true",
                          help="Run root in batch mode")
options_parser.add_option("","--filteredFile",dest="filteredFile",default="skimmed.dat",help="Run root in batch mode")
(prog_options, prog_args) = options_parser.parse_args()

if prog_options.batchMode:
    ROOT.gROOT.SetBatch(True)

# synch the verbose level between the cftkutils module and the explorer script
cftkutils.verbose = prog_options.verbose
cftkutils.athena = prog_options.athena

# define a default ROOT style
style = ROOT.TStyle("FTKExplorerStyle","FTK Explorer default style")
style.SetCanvasColor(0)
style.SetCanvasBorderSize(0)
style.SetCanvasBorderMode(0)
style.SetFrameBorderMode(0)
style.SetPadLeftMargin(.12)
style.SetPadRightMargin(.1)
style.SetPadBottomMargin(.15)
style.SetMarkerStyle(20)
style.SetLineWidth(2)
style.SetLegendBorderSize(0)
style.SetHistFillColor(ROOT.kYellow)
style.SetOptStat(0)
style.SetTitleBorderSize(0)
style.SetTitleH(0.08)
style.SetTitleX(0.95)
style.SetTitleY(0.99)
style.SetTitleAlign(33)
style.SetTitleFillColor(0)
style.SetTitleOffset(.95,"Y")
style.SetTitleSize(0.06,"X")
style.SetTitleSize(0.06,"Y")
style.SetEndErrorSize(6)
if ROOT.gROOT.GetVersionInt()>=53000 :
    style.SetLegendFillColor(0)
ROOT.gROOT.SetStyle("FTKExplorerStyle")


## Load FTK libraries
print "Load FTK library...",
res = ROOT.gSystem.Load("../standalone/libftk_classes.so")
if res == 0:
    print "Success"
elif res == 1:
    print "Library already lodaed"
else :
    print "Failed. This could be a problem"


# The following is to get the history
import atexit
historyPath = os.path.expanduser("~/.ftk_explorer_history")
try:
    import readline
except ImportError:
    print "Module readline not available."
else:
    import rlcompleter
    readline.parse_and_bind("tab: complete")
    if os.path.exists(historyPath):
        readline.read_history_file(historyPath)

# -----------------------------------------------------------------
def save_history(historyPath=historyPath):
    try:
        import readline
    except ImportError:
        print "Module readline not available."
    else:
        readline.write_history_file(historyPath)
        
atexit.register(save_history)

def pixelRowIsGanged(row):
    if row==153 or row==155 or row==157 or row==159 or row==168 or row==170 or row==172 or row==174 or (160<=row and row<=167):
        return True
    else:
        return False

class FTKExplorerStatError(Exception) :
    def __init__(self,val) :
        self.value = val
    def __str__(self) :
        return repr(self.value)

def calcErr(num,denom) :    
    if denom == 0 :
        return -1
    
    e1 = math.sqrt(num)
    e2 = math.sqrt(denom)
    w = num/denom
   
    err2 = ((1.-2.*w)*e1*e1 + w*w*e2*e2)/(denom*denom)

    return math.sqrt(err2)


def WilsonIntervalError(x,n,Z=1.96) :
    """Evaluate the Wilson interval for a poisson experiment. Z=1.96 is equivalent to a 95% interval. The interval is returned with respect to the ce"""
    xf = float(x)
    nf = float(n)
    p = xf/nf
    min = (p+Z**2/(2*nf)-Z*math.sqrt(p*(1-p)/nf+Z**2/(4*nf**2)))/(1+Z**2/nf)
    max = (p+Z**2/(2*nf)+Z*math.sqrt(p*(1-p)/nf+Z**2/(4*nf**2)))/(1+Z**2/nf)
    return (p-min,max-p)

def boxplot(values,lvl=.25,ntch=1.5) :
    "Evaluate the boxplot"""
    ### ref: http://www.lcgceurope.com/lcgceurope/data/articlestandard/lcgceurope/132005/152912/article.pdf
    ### ref: http://stat.ethz.ch/R-manual/R-devel/library/grDevices/html/boxplot.stats.html [not used by default]
    import copy
    ordvalues = copy.copy(values) # make a copy
    ordvalues.sort()

    if lvl>=1 :
        raise FTKExplorerStatError("Level value should be less than 1")
    elif ntch<0 :
        raise FTKExplorerStatError("Notches should be >=0")
    
    nvals = len(ordvalues)
    if nvals%2==1 : # odd
        median = ordvalues[nvals/2+1]
    else :# even
        median = 0.5*ordvalues[nvals/2]+0.5*ordvalues[nvals/2+1]

    # evaluate the quartiles ids and adjust them
    lowerid = nvals*lvl
    if lowerid-int(lowerid)>0 :
        lower = ordvalues[int(lowerid)]
    else :
        lowerid = int(math.trunc(lowerid))
        lower = 0.5*ordvalues[lowerid]+0.5*ordvalues[lowerid+1]
    upperid = nvals*(1-lvl)
    if upperid-int(upperid) :
        upper = ordvalues[int(upperid)]
    else :
        upperid = int(math.trunc(upperid))
        if upperid < nvals-1 :
            upper = 0.5*ordvalues[upperid]+0.5*ordvalues[upperid+1]
        else :
            upper = ordvalues[upperid]


    # evaluate the upper(lower) limit
    IQR = (upper-lower)#/math.sqrt(nvals) # R defines the IQR normalized
    lowerlim = lower-ntch*IQR
    extralow = None
    # adjust the limit
    for i, v in enumerate(ordvalues) :
        if v>= lowerlim :
            lowerlim = v
            if i>0:
                extralow = ordvalues[:i-1]
            break
    upperlim = upper+ntch*IQR
    ordvalues.reverse()
    extraup = None
    for i, v in enumerate(ordvalues) :
        if v<=upperlim :
            upperlim = v
            if i>0 :
                extraup = ordvalues[:i-1]
            break
    return {"box":(lowerlim,lower,median,upper,upperlim),
            "extralow":extralow, "extraup": extraup}

#fill labels for statistics legend
#must be consistent with "MakeRegionsPlot" and     
legStatLabels = [ "5% 50% 99%" , "25% avg 95%" ] 

def statrange(values,lvls=[0.05,0.25,0.5,0.75,0.95,.99]) :
    # Returns the values relative to a list of levels
    if len(values)<2 :
        return {"lvls": [0 for l in lvls], "range": (0,0),
                "mean": 0, "sd": 0}
        
    
    import copy
    ordvalues = copy.copy(values) # make a copy
    ordvalues.sort()

    range = (ordvalues[0],ordvalues[-1])
    nvals = len(ordvalues)

    lvlvalues = []
    for lvl in lvls :
        idf = nvals*lvl
        if idf-int(idf)>1e-6 and int(idf)<nvals-1 : 
            val = ordvalues[int(idf)+1]
        else :
            val = ordvalues[int(idf)]
        lvlvalues.append(val)

    vsum = 0
    vsum2 = 0
    for v in ordvalues :
        vsum += v
        vsum2 += v*v
    mean = vsum/nvals
    sd = math.sqrt((vsum2-nvals*mean*mean)/(nvals-1))

    return {"lvls": lvlvalues, "range": range,
            "mean": mean, "sd": sd}


def NormalizeOutputs(cross=True) :
    """This function compares the ftkouts content, extracting the files used and remove from the list the files that don't match in the different FTKTree. After the cleanup the FTKTree content is rebuilt"""
    global ftkouts
    global ftkinputs

    def make_baselist(flist, nskip) :
        newlist = []
        for f in flist :
            newlist.append(os.path.basename(f)[nskip:])
        return newlist

    def common_baselist(reflist,chklist) :
        final = []
        
        for refelem in reflist :
            if refelem in chklist :
                final.append(refelem)
        return final

    def wrapperoutname(path) :
        bsname = os.path.basename(path)
        tokens = bsname.split('.')
        #print bsname, tokens
        if len(tokens)<0 :
            raise ValueError("Wrong wrapper file name")
        
        if tokens[-1]=="dat" :
            outname = ".".join(tokens[:-1])
        elif tokens[-2]=="dat" and tokens[-1]=="bz2" :
            outname = ".".join(tokens[:-2])
        else :
            print "Invalid input file extension in:", path
            outname = "none"
        
        return outname+".root"
    
    print "Normalize internal lists of the files"
    
    # check the internal consistency of the lists and extract the base list
    print ftkouts
    for out in ftkouts :
        cmnbaselist = None # initilize the common list in the output tree
        
        # check unmerged roads
        for lst in out.unmergedroads_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,9)
            else :
                chkbaselist = make_baselist(lst,9)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # check unmerged tracks
        for lst in out.unmergedtracks_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,10)
            else :
                chkbaselist = make_baselist(lst,10)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # check merged roads
        for lst in out.mergedroads_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,9)
            else :
                chkbaselist = make_baselist(lst,9)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # check merged tracks
        for lst in out.mergedtracks_files :
            if not lst : # sub-regios without input are ignored
                continue
            if not cmnbaselist :
                cmnbaselist = make_baselist(lst,10)
            else :
                chkbaselist = make_baselist(lst,10)
                cmnbaselist = common_baselist(cmnbaselist,chkbaselist)

        # if an ftkinputs object exists a match with its content is done and the cmbbaselist is intersected
        if ftkinputs :
            cmninlist = []            
            for fn in ftkinputs.filelist :
               # the expected output name for this wrapper file is calculated
                ofn = wrapperoutname(fn)
                
                if ofn in cmnbaselist :
                    cmninlist.append(fn)
            print "Wrapper files matched %d (%d)" % (len(ftkinputs.filelist),len(cmninlist))
            out.inputfiles = cmninlist

            # synchronize the two lists
            synclist = []
            for fn in cmninlist :
                ofn = wrapperoutname(fn)
                if ofn in cmnbaselist :
                    synclist.append(ofn)
            # apply a new baselist that is compatible with the wrapper files and in sync
            cmnbaselist = synclist
            # store the list of matched input files in the  object
        print "In", os.path.basename(out.outdir), len(cmnbaselist), "files"
                
        # reduce the list of files only to files common in all the outputs
        out.outputfiles = cmnbaselist
        
    if len(ftkouts)<2 or not cross:
        return 0

    # select the reference output, the first
    cmnoutlist = ftkouts[0].outputfile
    nstart = len(cmnlist)

    print "Normalize among lists"
    for outchk in ftkouts[1:] : # output look
        cmnoutlist = common_baselist(cmnoutlist,out.outputfiles)
    # end output loop

    if ftkinputs :
        cmninlist = []            
        for fn in ftkinputs.filelist :
            # the expected output name for this wrapper file is calculated
            ofn = wrapperoutname(fn)
            if ofn in cmnbaselist :
                cmninlist.append(fn)
        for out in ftkouts :
            out.inputfiles = cmninlist

        # synchronize the two lists
        synclist = []
        for fn in cmninlist :
            ofn = wrapperoutname(fn)
            if ofn in cmnoutlist :
                synclist.append(ofn)
        # apply a new baselist that is compatible with the wrapper files and in sync
        cmnoutlist = synclist

    ndiff = nstart-len(cmnlist)
    print "Common number of files:", len(cmnoutlist), "(%d removed)" % ndiff
    # set the single lists to the new common values
    if ndiff>0 :
        for out in ftkouts :
            out.outputfiles = cmnoutlist
    return ndiff


def getrange(text,maxval) :
    """Extract a range from a string"""
    values = []
    
    # first extract disjoint values
    parts = text.split(",")

    # extract the internal ranges
    for part in parts :
        extrs = part.split("-")

        if len(extrs) == 1 :
            try :
                ival = int(extrs[0])
                values += [ival]
            except ValueError :
                print "Impossible to convert", extrs, "as a value"
                continue
        elif len(extrs) == 2 :
            if extrs[0] == "" :
                try :
                    ival = int(extrs[1])
                    values += range(0,ival+1)
                except ValueError :
                    print "Impossible to convert", extrs, "as a upper bound"
                    continue                    
            elif extrs[1] == "" :
                try :
                    ival = int(extrs[0])
                    values += range(ival,maxval)
                except ValueError :
                    print "Impossible to convert", extrs, "as a lower bound"
                    continue
            else :
                try :
                    minval = int(extrs[0])
                    maxval = int(extrs[1])
                    if maxval >= minval :
                        values += range(minval,maxval+1)
                    else :
                        print extrs, "is not a valid range"
                except ValueError :
                    print "Impossible to convert", extrs, "as a range"
                    continue
    return values


class FTKExplorerError(Exception) :
    def __init__(self,val) :
        self.value = val
    def __str__(self) :
        return repr(self.value)

class FTKExplorerSet :
    def __init__(self,name,defval,descr="",modvar=False,convfun=int) :
        self.name = name
        self.description = descr
        self.convfun = convfun
        self.value = defval
        self.modvar = modvar
        if self.modvar :
            setattr(cftkutils,self.name,self.value)
        return None

    def Message(self) :
        return "%s = %s : %s" % (self.name, str(self.value), self.description)

    def Set(self,value) :
        self.value = self.convfun(value)
        if self.modvar :
            setattr(cftkutils,self.name,self.value)
        return None

    def Value(self) :
        return self.value

    def ValueInt(self) :
        return int(self.value)

    def __str__(self) :
        return str(self.value)

    
class FTKExplorerCmd(cmd.Cmd) :
    """The class defines the command line interface to explore one or more FTKTree."""
    def __init__(self) :
        global prog_options
        cmd.Cmd.__init__(self)
        self.browser = None
        self.HistoFile = None
        self.ievt = -1

        # internal settings as dictionary with: name: (defvalue, description)
        self.settings = {"maxEvent": FTKExplorerSet("maxEvent", None, "Maximum number of events to use in the \"study\" event loop"),
                         "skipEvents": FTKExplorerSet("skipEvents", 0, "Set the number of events to skip"),
                         "PresentPrefix": FTKExplorerSet("PresentPrefix", None, "If not None each time \"present\" produces a canvas, a graphic file with the prefix is created",False,str),
                         "PresentExt": FTKExplorerSet("PresentExt", "png", "Graphics extension for the present output files (prefix has to be not None)",False,str),
                         "CacheFile": FTKExplorerSet("CacheFile",prog_options.savestate,"If a value is assigned, this is the output file where the internal status will be saved (as -R command option)",False,str),
                         "HistoFile": FTKExplorerSet("HistoFile",prog_options.histofile,"Path of the ROOT file that collects the \"study\" output.",False,str),
                         "maxPhi": FTKExplorerSet("maxPhi",math.pi,"Maximum fiducial phi value in the \"effcurve\" calculation",True,float),
                         "minPhi": FTKExplorerSet("minPhi",-math.pi,"Minimum fiducial phi value in the \"effcurve\" calculation",True,float),
                         "maxEta": FTKExplorerSet("maxEta",2.5,"Maximum fiducial eta value in the \"effcurve\" calculation",True,float),
                         "minEta": FTKExplorerSet("minEta",-2.5,"Minimum fiducial eta value in the \"effcurve\" calculation",True,float),
                         "maxD0": FTKExplorerSet("maxD0",2.,"Maximum fiducial eta value in the \"effcurve\" calculation",True,float),
                         "minD0": FTKExplorerSet("minD0",-2.,"Minimum fiducial eta value in the \"effcurve\" calculation",True,float),
                         "maxZ0": FTKExplorerSet("maxZ0",120.,"Maximum fiducial eta value in the \"effcurve\" calculation",True,float),
                         "minZ0": FTKExplorerSet("minZ0",-120.,"Minimum fiducial eta value in the \"effcurve\" calculation",True,float),
                         "maxAbsCurv": FTKExplorerSet("maxAbsCurv",5e-3,"Minimum fiducial eta value in the \"efficiency\" calculation",True,float),
                         "athena": FTKExplorerSet("athena",0,"If true, use athena options",True,bool),
                         "UseIncomplete": FTKExplorerSet("UseIncomplete",0,"If True in efficiency plots the incomplete tracks are used",True,bool),
                         "useIBL": FTKExplorerSet("useIBL",1,"If 1 (aka True) layer-0 is IBL (default 1)",True,int),
                         "verbose": FTKExplorerSet("verbose",0,"Verbosity level",True,int)}
        return None

    def HistoFileCD(self) :
        if self.HistoFile :
            self.HistoFile.cd()
        else :
            self.HistoFile = ROOT.TFile.Open(self.settings["HistoFile"].Value(),"recreate")

    def arguments_complete(self,text,line,begidx,endidx, cmdargs) :
        possible = []
        
        for arg in cmdargs :
            if arg.startswith(text) :
                possible += [arg]
        
        return possible

    def complete_add(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["nregions","nsubregions","nlayers"])
        possible += glob.glob(text+"*")
        return possible

    def do_add(self,text) :
        """Add an output directory to the list, the command syntax is: add <path>"""
        global ftkouts

        # build the command line
        args = text.split()
        outpath = args[0]

        nregions = prog_options.numregions
        nsubregions = prog_options.numsubregions
        nlayers = prog_options.nlayers

        if len(args) == 0 :
            print "Nothing to do"
            return None
        elif len(args) > 1 :
            if "nregions" in args :
                nregions = int(args[args.index("nregions")+1])
            elif "nsubregions" in args :
                nsubregions = int(args[args.index("nsubregions")+1])
            elif "nlayers" in args :
                nlayers = int(args[args.index("nlayers")+1])
                
        if os.path.isdir(outpath) :
            if len(ftkouts)>0 :
                previd = ftkouts[-1].id
            else :
                previd = -1
            out = FTKTree(previd,outpath)
            if prog_options.connect :
                out.Connect()
            else:
                print "No connection attempted"
            ftkouts.append(out)            
        else :
            print "*** Directory doesn't exist"""
        return None

    def do_browser(self,text) :
        """Open the ROOT browser to explorer the histograms."""
        self.browser = ROOT.TBrowser()  
        return None

    def complete_clear(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["unmergedroads","unmergedtracks","mergedroads","mergedtracks"])
        return possible
    
    def do_clear(self, text) :
        """Remove the list of files for some categories. Possible requirests are:
 * unmergedroads  : removes the list of the unmerged road files
 * unmergedtracks : removes the list of the unmerged track files
 * mergedroads    : removes the list of the merged road files
 * mergedtracks   : removes the list of the merged track files

If multiple outputs are used, the command will clean the lists for all the output structures.

This has an effect if the command is done before the output files are "connected", otherwise the command doesn't have any effect."""
        args = text.split()

        if "unmergedroads" in args :
            for out in ftkouts :                
                for i, flist in enumerate(out.unmergedroads_files) :
                    out.unmergedroads_files[i] = None
        if "unmergedtracks" in args :
            for out in ftkouts :                
                for i, flist in enumerate(out.unmergedtracks_files) :
                    out.unmergedtracks_files[i] = None
        if "mergedroads" in args :
            for out in ftkouts :                
                for i, flist in enumerate(out.mergedroads_files) :
                    out.mergedroads_files[i] = None
        if "mergedtracks" in args :
            for out in ftkouts :                
                for i, flist in enumerate(out.mergedtracks_files) :
                    out.mergedtracks_files[i] = None
        return None

    def do_compare(self,text) :
        """Compare the FTK outputs, if more than 1, and show which events show any difference. The first FTK output object passed in the command line is used as reference for all the others."""
        if len(ftkouts) < 2:
            print "*** not enough outputs to make a comparison"
            return None

        # compare the structure and the existing branches, if more than 2 structures are compared
        # the highest difference is reported.
        # The ouptut code is:
        # * 0 OK
        # * 1 same numebr regions/sub-regions not all the same objects exist
        # * 2 different structure, comparison not allowed
        maxSimLevel = 0 # 0 all the structure
        outref = ftkouts[0]
        for outchk in ftkouts[1:] :
            simLevel = outref.CompareStructure(outchk)
            if simLevel > maxSimLevel :
                maxSimLevel = simLevel
        if maxSimLevel==2 :
            print "*** A comparison it s not possible"
            return None
        elif maxSimLevel==1 :
            print "*** Partial comparison, some object is not present or enabled"
        else :
            print "Same structure"
                
        try :
            maxevent = int(self.settings['maxEvent'].Value())
            if maxevent>outref.nEvents :
                maxevent = outref.nEvents
        except :
            maxevent = outref.nEvents

        print "Starting comparation loop, the only messages that will be printed are eventual differences among the outputs"
        hasDifference = False
        for ievt in xrange(maxevent) : # event loop
            # move all the outputs to the same event
            for out in ftkouts :
                out.GetEntry(ievt)
            # compare all the output objects with the first
            for outchk in ftkouts[1:] :
                if outref.CompareEvent(outchk) :
                    hasDifference = True
                    
        if hasDifference :
            print "TEST FAILED ***"
        else :
            print "TEST PASSED"
        # end event loop
        return None

    def do_connect(self,text) :
        """If the explorer was launched with the option --no-connect the connection is done now"""
        for out in ftkouts :
            if not out.connected :
                out.Connect()
        return None
    
    def complete_disable(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["output","region","subregion"])
        return possible

    
    def do_disable(self,text) :
        """Disable 1 or more outputs. Trough this command is possible to disable: single output structures, regions, sub-regions. To set which part has to be disabled the syntax is:

> disable output 3 region 4 subregion 0

The keywords are followed by numbers. The keywords are not combined, in the example: output 3 is disabled, the region 4 for all the outputs is disabled and the sub-region 0 for any output and region is also disabled.

In place of the number a range definition can be used. The range can be defined with a syntax like: 0-4,7 that disable (0,1,2,3,4,7).

Any unknown sequence or argument is ignored. During the work the branches disabled are confirmed."""
        global prog_options

        args = text.split()

        nargs = len(args)

        i = 0
        while i < nargs :
            key = args[i]
            if key == "output" :
                iout = int(args[i+1])
                ftkouts[iout].isEnabled = False
                i += 1
            elif key == "region" :
                values = getrange(args[i+1],prog_options.numregions)
                print "Disabling regions:", values
                for out in ftkouts :
                    for iv in values :
                        out.Enable(False,region=iv)
                i += 1
            elif key == "subregion" :
                values = getrange(args[i+1],prog_options.numsubregions)
                print "Disabling subregions:", values
                for out in ftkouts :
                    for iv in values :
                        out.Enable(False,subreg=iv)
                i += 1
            else :
                print "*** key:", key, "not recognized"
            i += 1
                
        return None

    def complete_enable(self,text,line,begidx,endidx) :
        return self.complete_disable(text,line,begidx,endidx)
            
    def do_enable(self,text) :
        """Enable disabled outputs, regions or sub-regions. For more details read "disable" help"""
        global prog_options

        args = text.split()

        nargs = len(args)

        i = 0
        while i < nargs :
            key = args[i]
            if key == "output" :
                iout = int(args[i+1])
                ftkouts[iout].isEnabled = True
                i += 1
            elif key == "region" :
                values = getrange(args[i+1],prog_options.numregions)
                for out in ftkouts :
                    for iv in values :
                        out.Enable(True,region=iv)
                i += 1
            elif key == "subregion" :
                values = getrange(args[i+1],prog_options.numsubregions)
                for out in ftkouts :
                    for iv in values :
                        out.Enable(True,subreg=iv)
                i += 1
            else :
                print "*** key:", key, "not recognized"
            i += 1
                
        return None

    def do_roadhitstudy(self,text):
        """ Plot the number of hits per layer for all roads. Saved in a histogram file."""
        global prog_options
        global prog_args
        global ftkouts
        
        if len(text) > 0:
            out = ftkouts[int(text)]
        else:
            pattpoints = []
            
            mylist = prog_args
            if prog_options.indir != None:
                mylist = glob.glob(prog_options.indir + "/raw*")
                if len(mylist)== 0:
                    mylist =  glob.glob(prog_options.indir + "/*/raw*")

            for i, argument in enumerate(mylist):
                dirnames = argument.split("_")
                if dirnames[-1].startswith("np"):

                    if(dirnames[-1].endswith("/")):
                        pattpoints.append(int(dirnames[-1][2:-1]))
                    else:
                        pattpoints.append(int(dirnames[-1][2:]))
                    
        #get the output file with the most pattern points used
            if len(pattpoints) > 0:
                out = ftkouts[pattpoints.index(max(pattpoints))]
            else: #or use the first one in the list
                out = ftkouts[0]
        if not out.isEnabled:
            return None

        try :
            maxevent = int(self.settings['maxEvent'].Value())
        except :
            maxevent = None
            
            
        out.RoadHitStudy(maxevent)
            

        return None

    def do_fiducial(self,text):
        """ Plot the eta vs phi of the tracks in the output files. Useful for finding the fiducial region for effcurve.  By default it uses the outputfile with the largest number of pattern points, or if the number of pattern points is not in the directory name, it uses the first output file in the list. Optionally, you can give the number of the output file you would like to use by doing "fiducial N" where N is the number of the output file.  """
        global prog_options
        global prog_args
        global ftkouts
        
        if len(text) > 0:
            out = ftkouts[int(text)]
        else:
            pattpoints = []
            
            mylist = prog_args
            if prog_options.indir != None:
                mylist = glob.glob(prog_options.indir + "/raw*")
                if len(mylist)== 0:
                    mylist =  glob.glob(prog_options.indir + "/*/raw*")

            for i, argument in enumerate(mylist):
                dirnames = argument.split("_")
                if dirnames[-1].startswith("np"):

                    if(dirnames[-1].endswith("/")):
                        pattpoints.append(int(dirnames[-1][2:-1]))
                    else:
                        pattpoints.append(int(dirnames[-1][2:]))
                    
        #get the output file with the most pattern points used
            if len(pattpoints) > 0:
                out = ftkouts[pattpoints.index(max(pattpoints))]
            else: #or use the first one in the list
                out = ftkouts[0]
        if not out.isEnabled:
            return None
        out.EtaPhi()
            

        return None

    def do_setfid(self,text) :
        """ Set the fiducial region in a single command: setfid minPhi maxPhi minEta maxEta """
        vals = text.split(" ")
        if len(vals) != 4:
            print "Must set all eta phi cuts at once: setfid minPhiVal maxPhiVal minEtaVal maxEtaVal  For individual value use, e.g.,  set minEta minEtaVal."
            return None
        else:
            self.settings["minPhi"].Set(float(vals[0]))
            print "minPhi %s" %self.settings["minPhi"]
            self.settings["maxPhi"].Set(float(vals[1]))
            print "maxPhi %s" %self.settings["maxPhi"]
            self.settings["minEta"].Set(float(vals[2]))
            print "minEta %s" %self.settings["minEta"]
            self.settings["maxEta"].Set(float(vals[3]))
            print "maxEta %s" %self.settings["maxEta"]
            return None

    def complete_efficiency(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["effcurve"])
        return possible

    def do_efficiency(self,text) :
        """Evaluate the efficiency and produce a few standard plots"""

        if debug: print "In do_efficiency"


        #
        #  Make small output ntuple for res study
        #
        from ROOT import TTree,TFile,std
        import numpy as n

        resStudyFile = TFile("resStudy.root","RECREATE")
        resStudyTree = TTree("ResStudy" , "ResStudy");

        m_FTK_d0     = std.vector(float)()
        m_FTK_phi    = std.vector(float)()
        m_FTK_eta    = std.vector(float)()
        m_FTK_z0     = std.vector(float)()
        m_FTK_Ipt    = std.vector(float)()
        m_FTK_BitMask= std.vector(int)()
        m_off_d0     = std.vector(float)()
        m_off_phi    = std.vector(float)()
        m_off_eta    = std.vector(float)()
        m_off_z0     = std.vector(float)()
        m_off_Ipt    = std.vector(float)()
        m_Truth_d0   = std.vector(float)()
        m_Truth_phi  = std.vector(float)()
        m_Truth_eta  = std.vector(float)()
        m_Truth_z0   = std.vector(float)()
        m_Truth_Ipt  = std.vector(float)()

#        m_hit_x = std.vector(float)()
#        m_hit_y = std.vector(float)()
#        m_hit_z = std.vector(float)()
#        m_hit_BarEcp= std.vector(float)()
#        m_hit_layer = std.vector(float)()
#        m_hit_type = std.vector(float)()
#        m_hit_phi_module = std.vector(float)()
#        m_hit_eta_module = std.vector(float)()
#        m_hit_side = std.vector(float)()
#        m_hit_strip= std.vector(float)()
#        m_hit_num_strips= std.vector(float)()



        resStudyTree.Branch('FTK_d0',        m_FTK_d0       )
        resStudyTree.Branch('FTK_z0',        m_FTK_z0       )
        resStudyTree.Branch('FTK_phi',       m_FTK_phi      )
        resStudyTree.Branch('FTK_eta',       m_FTK_eta      )
        resStudyTree.Branch('FTK_Ipt',       m_FTK_Ipt      )
        resStudyTree.Branch('FTK_BitMask',   m_FTK_BitMask  )

        resStudyTree.Branch('Off_d0',        m_off_d0       )
        resStudyTree.Branch('Off_z0',        m_off_z0       )
        resStudyTree.Branch('Off_phi',       m_off_phi      )
        resStudyTree.Branch('Off_eta',       m_off_eta      )
        resStudyTree.Branch('Off_Ipt',       m_off_Ipt      )

        resStudyTree.Branch('Truth_d0',  m_Truth_d0 )
        resStudyTree.Branch('Truth_z0',  m_Truth_z0 )
        resStudyTree.Branch('Truth_phi', m_Truth_phi)
        resStudyTree.Branch('Truth_eta', m_Truth_eta)
        resStudyTree.Branch('Truth_Ipt', m_Truth_Ipt)


        #######


        args = text.split()
        doEffcurve = False
        if "effcurve" in args :
            doEffcurve = True
            
        self.HistoFileCD()

        effdir = ROOT.gDirectory.mkdir("Compare_To_Truth") # store the pointer to the root directory
        effdir.cd()
        effsummary = ROOT.TGraphAsymmErrors(len(ftkouts))
        effsummary.SetNameTitle("effsummary","Efficiency summary")
        effsummary.SetMarkerColor(ROOT.kRed)
        effsummary.SetLineColor(ROOT.kRed)
        offline_effsummary = ROOT.TGraphAsymmErrors(len(ftkouts))
        offline_effsummary.SetNameTitle("offline_effsummary","Offline efficiency summary")
        offline_effsummary.SetMarkerColor(ROOT.kRed)
        offline_effsummary.SetLineColor(ROOT.kRed)
        FTKeffOffline_summary = ROOT.TGraphAsymmErrors(len(ftkouts))
        FTKeffOffline_summary.SetNameTitle("FTKeffOffline_summary","Offline efficiency summary")
        FTKeffOffline_summary.SetMarkerColor(ROOT.kRed)
        FTKeffOffline_summary.SetLineColor(ROOT.kRed)
        covsummary = ROOT.TGraphAsymmErrors(len(ftkouts))
        covsummary.SetNameTitle("covsummary","Coverage summary")
        covsummary.SetMarkerColor(ROOT.kBlue)
        covsummary.SetLineColor(ROOT.kBlue)
        if doEffcurve :
            covsummary.GetXaxis().SetTitle("N Patterns (Tower)")
            covsummary.GetXaxis().SetTitle("Efficiency")
            effsummary.GetXaxis().SetTitle("N Patterns (Tower)")
            effsummary.GetXaxis().SetTitle("Efficiency")
            offline_effsummary.GetXaxis().SetTitle("N Patterns (Tower)")
            offline_effsummary.GetXaxis().SetTitle("Offline Efficiency")
            FTKeffOffline_summary.GetXaxis().SetTitle("N Patterns (Tower)")
            FTKeffOffline_summary.GetXaxis().SetTitle("FTK/Offline Efficiency")
        effdir.Add(effsummary)
        effdir.Add(offline_effsummary)
        effdir.Add(FTKeffOffline_summary)
        effdir.Add(covsummary)

        rootstack = [] # stack create to prevent the deletion of the objects when they go out of context

        # build an ordered list of the pattern point if used for an effcurve
        npseq = None
        if doEffcurve :
            npseq = []
            for out in ftkouts :
                np = int(out.outdir.split("np")[1])*out.nsubregions
                npseq.append(np)
            npseq.sort()
        
        for outid, out in enumerate(ftkouts) : # output stuctures loop
            # skip disabled FTKTree
            if not out.isEnabled:
                continue

            # check if input files were set
            if not out.inputfiles :
                print "No input files for", os.path.basename(out.outdir)
                continue

            print "Evalute efficiency for:", os.path.basename(out.outdir)
            # prepare a  directory for the current output
            curdir = effdir.mkdir(os.path.basename(out.outdir))
            curdir.cd()
            
            # create a set of histogram for any interesting quantity
            maxphi = float(self.settings["maxPhi"].Value())
            minphi = float(self.settings["minPhi"].Value())
            maxeta = float(self.settings["maxEta"].Value())
            mineta = float(self.settings["minEta"].Value())
            maxd0 = float(self.settings["maxD0"].Value())
            mind0 = float(self.settings["minD0"].Value())
            maxz0 = float(self.settings["maxZ0"].Value())
            minz0 = float(self.settings["minZ0"].Value())
            maxabscurv = float(self.settings["maxAbsCurv"].Value())
            minpt = 1./(2*maxabscurv)

            z0bins=100
            z0min=-5
            z0max=5
            pTbins=35
            pTmin=-0.7
            pTmax=0.7
            eta_bins=30
            eta_min=-3
            eta_max=3


            binPT = [0,2,4,6,8,10,12,14,16,18,20,25,30,35,40,45,50,60,70,80,90,100]


            # histograms of quantities of matched tracks
            histo_curv = ROOT.TH1F("histo_curv","All tracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_curv_mt = ROOT.TH1F("histo_curv_mt","Matched tracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_curv_omt = ROOT.TH1F("histo_curv_omt","Offline matched tracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_curv_mo = ROOT.TH1F("histo_curv_mo","FTK matched offline tracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_curv_mr = ROOT.TH1F("histo_curv_mr","Matched roads;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_pt = ROOT.TH1F("histo_pt","All tracks;p_{T};Freq.",21,array('d',binPT))
            histo_pt_mt = ROOT.TH1F("histo_pt_mt","Matched tracks;p_{T};Freq.",21,array('d',binPT))
            histo_pt_omt = ROOT.TH1F("histo_pt_omt","Offline matched tracks;p_{T};Freq.",21,array('d',binPT))
            histo_pt_mo = ROOT.TH1F("histo_pt_mo","FTK matched offlinetracks;p_{T};Freq.",21,array('d',binPT))
            histo_pt_mr = ROOT.TH1F("histo_pt_mr","Matched roads;p_{T};Freq.",21,array('d',binPT))
            histo_phi = ROOT.TH1F("histo_phi","All tracks;#phi;Freq.",50,minphi,maxphi)
            histo_phi_mt = ROOT.TH1F("histo_phi_mt","Matched tracks;#phi;Freq.",50,minphi,maxphi)
            histo_phi_omt = ROOT.TH1F("histo_phi_omt","Offline matched tracks;#phi;Freq.",50,minphi,maxphi)
            histo_phi_mo = ROOT.TH1F("histo_phi_mo","FTK matched offline tracks;#phi;Freq.",50,minphi,maxphi)
            histo_phi_mr = ROOT.TH1F("histo_phi_mr","Matched roads;#phi;Freq.",50,minphi,maxphi)
            histo_d0 = ROOT.TH1F("histo_d0","All tracks;d_{0} [mm];Freq.",50,-4,4)
            histo_d0_mt = ROOT.TH1F("histo_d0_mt","Matched tracks;d_{0} [mm];Freq.",50,-4,4)
            histo_d0_omt = ROOT.TH1F("histo_d0_omt","Offline matched tracks;d_{0} [mm];Freq.",50,-4,4)
            histo_d0_mo = ROOT.TH1F("histo_d0_mo","FTK matched offline tracks;d_{0} [mm];Freq.",50,-4,4)
            histo_d0_mr = ROOT.TH1F("histo_d0_mr","Matched roads;d_{0} [mm];Freq.",50,-4,4)
            histo_d0_ftk = ROOT.TH1F("histo_d0_ftk","Matched roads;d_{0} [mm];Freq.",50,-4,4)
            histo_eta = ROOT.TH1F("histo_eta","All tracks;#eta;Freq.",50,mineta,maxeta)
            histo_eta_mt = ROOT.TH1F("histo_eta_mt","Matched tracks;#eta;Freq.",50,mineta,maxeta)
            histo_eta_omt = ROOT.TH1F("histo_eta_omt","Offline matched tracks;#eta;Freq.",50,mineta,maxeta)
            histo_eta_mo = ROOT.TH1F("histo_eta_mo","FTK matched offline tracks;#eta;Freq.",50,mineta,maxeta)
            histo_eta_mr = ROOT.TH1F("histo_eta_mr","Matched roads;#eta;Freq.",50,mineta,maxeta)
            histo_z0 = ROOT.TH1F("histo_z0","All tracks;z_{0} [mm];Freq.",50,-110,110)
            histo_z0_mt = ROOT.TH1F("histo_z0_mt","Matched tracks;z_{0} [mm];Freq.",50,-110,110)
            histo_z0_omt = ROOT.TH1F("histo_z0_omt","Offline matched tracks;z_{0} [mm];Freq.",50,-110,110)
            histo_z0_mo = ROOT.TH1F("histo_z0_mo","FTK matched offline tracks;z_{0} [mm];Freq.",50,-110,110)
            histo_z0_mr = ROOT.TH1F("histo_z0_mr","Matched roads;z_{0} [mm];Freq.",50,-110,110)
            histo_phieta = ROOT.TH2F("histo_phieta","All tracks;#phi;#eta;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_phieta_mt = ROOT.TH2F("histo_phieta_mt","Matched tracks;#phi;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_phieta_omt = ROOT.TH2F("histo_phieta_omt","Offline matched tracks;#phi;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_phieta_mo = ROOT.TH2F("histo_phieta_mo","FTK matched offline tracks;#phi;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_phieta_mr = ROOT.TH2F("histo_phieta_mr","Matched roads;#phi;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_resphi = ROOT.TH1F("histo_resphi","Res #phi",50,0,0);
            histo_resip = ROOT.TH1F("histo_resip","Res #ip",50,-0.5,0.5);
            histo_resInvPt = ROOT.TH1F("histo_resInvPt","Res #InvPt",50,0,0);
            histo_resz0 = ROOT.TH1F("histo_resz0","Res #z0", z0bins, z0min, z0max);
            histo_reseta = ROOT.TH1F("histo_reseta","Res #eta",50,0,0);
            histo_deltaz0 = ROOT.TH1F("histo_deltaz0","deltaz0",50,0,0);
            histo_deltaR = ROOT.TH1F("histo_deltaR","deltaR",50,0,0.1);
            

            # unmatched real tracks poperties
            histo_z0_umt = ROOT.TH1F("histo_z0_umt","Unmatched truth tracks;z_{0} [mm];Freq.",50,-110,110)
            histo_d0_umt = ROOT.TH1F("histo_d0_umt","Unmatched truth tracks;d_{0} [mm];Freq.",50,-2,2)
            histo_curv_umt = ROOT.TH1F("histo_curv_umt","Unmatched truth tracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_eta_umt = ROOT.TH1F("histo_eta_umt","Unmatched truth tracks;#eta;Freq.",50,mineta,maxeta)
            histo_phi_umt = ROOT.TH1F("histo_phi_umt","Unmatched truth tracks;#phi;Freq.",50,minphi,maxphi)
            histo_x0_umt = ROOT.TH1F("histo_x0_umt","Unmatched truth tracks;x_{0} [mm];Freq.",50,0,0)
            histo_y0_umt = ROOT.TH1F("histo_y0_umt","Unmatched truth tracks;y_{0} [mm];Freq.",50,0,0)
            histo_charge_umt = ROOT.TH1F("histo_charge_umt","Unmatched truth tracks;charge [mm];Freq.",50,0,0)
            histo_pdgcode_umt = ROOT.TH1F("histo_pdgcode_umt","Unmatched truth tracks;pdgcode [mm];Freq.",50,0,0)
            histo_assoc_umt = ROOT.TH1F("histo_assoc_umt","Unmatched truth tracks;assoc [mm];Freq.",50,0,0)
            histo_barcode_frac_umt = ROOT.TH1F("histo_barcode_frac_umt","Unmatched truth tracks;barcode_frac [mm];Freq.",50,0,0)
            histo_evtindex_umt = ROOT.TH1F("histo_evtindex_umt","Unmatched truth tracks;evtindex [mm];Freq.",50,0,0)
            histo_barcode_umt = ROOT.TH1F("histo_barcode_umt","Unmatched truth tracks;barcode [mm];Freq.",50,0,0)
            histo_parentID_umt = ROOT.TH1F("histo_parentID_umt","Unmatched truth tracks;parentID [mm];Freq.",50,0,0)
            
            # resolution plots
            histo_resphi_InvPt = ROOT.TH2F("histo_resphi_InvPt","Res #phi_InvPt", 30,0,0,30,0,0);
            histo_resz0_InvPt = ROOT.TH2F("histo_resz0_InvPt","Res #z0_InvPt",z0bins, z0min, z0max, pTbins, pTmin, pTmax);
            histo_resd0_InvPt = ROOT.TH2F("histo_resd0_InvPt","Res #d0_InvPt",30,0,0,30,0,0);
            histo_resd0_phi = ROOT.TH2F("histo_resd0_phi","Res #d0_phi",30,-2,2,30,-3.14,3.14);
            histo_reseta_InvPt = ROOT.TH2F("histo_reseta_InvPt", "Res #eta_InvPt", 30,0,0,30,0,0);
 
            histo_resphi_eta = ROOT.TH2F("histo_resphi_eta","Res #phi_eta", 30,0,0,30,0,0);
            histo_resz0_eta = ROOT.TH2F("histo_resz0_eta","Res #z0_eta",z0bins, z0min, z0max, eta_bins, eta_min, eta_max);
            histo_resd0_eta = ROOT.TH2F("histo_resd0_eta","Res #d0_eta",30,0,0,30,0,0);
            histo_resInvPt_eta = ROOT.TH2F("histo_resInvPt_eta","Res #d0_phi",30,0,0,30,0,0);
            histo_reseta_eta = ROOT.TH2F("histo_reseta_eta", "Res #eta_eta", 30,0,0,30,0,0);

            histo_resphi_FInvPt = ROOT.TH2F("histo_resphi_FInvPt","Res #phi_FInvPt", 30,0,0,30,0,0);
            histo_resz0_FInvPt = ROOT.TH2F("histo_resz0_FInvPt","Res #z0_FInvPt",z0bins, z0min, z0max, pTbins, pTmin, pTmax);
            histo_resd0_FInvPt = ROOT.TH2F("histo_resd0_FInvPt","Res #d0_FInvPt",30,0,0,30,0,0);
            histo_reseta_FInvPt = ROOT.TH2F("histo_reseta_FInvPt", "Res #eta_FInvPt", 30,0,0,30,0,0);
            
            # histograms of quantities of unmatched tracks
            histo_FTKcurv = ROOT.TH1F("histo_FTKcurv","All FTKtracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_FTKcurv_umt = ROOT.TH1F("histo_FTKcurv_umt","Unmatched tracks;1/p_{T};Freq.",50,-5e-3,5e-3)
            histo_FTKhits = ROOT.TH1F("histo_FTKhits","All FTKtracks;N_{hits};Freq.",5,7.5,12.5)
            histo_FTKhits_umt = ROOT.TH1F("histo_FTKhits_umt","Unmatched tracks;N_{hits};Freq.",5,7.5,12.5)
            histo_FTKpt = ROOT.TH1F("histo_FTKpt","All FTKtracks;p_{T};Freq.",21,array('d',binPT))
            histo_FTKpt_umt = ROOT.TH1F("histo_FTKpt_umt","Unmatched tracks;p_{T};Freq.",21,array('d',binPT))

            histo_FTKchi2 = ROOT.TH1F("histo_FTKchi2","All FTKtracks;#chi^2;Freq.",50,0,5)
            histo_FTKchi2_umt = ROOT.TH1F("histo_FTKchi2_umt","Unmatched tracks;#chi^2;Freq.",50,0,5)
            histo_FTKphi = ROOT.TH1F("histo_FTKphi","All FTKtracks;#phi;Freq.",50,minphi,maxphi)
            histo_FTKphi_umt = ROOT.TH1F("histo_FTKphi_umt","Unmatched tracks;#phi;Freq.",50,minphi,maxphi)
            histo_FTKd0 = ROOT.TH1F("histo_FTKd0","All FTKtracks;d_{0} [mm];Freq.",50,-2,2)
            histo_FTKd0_umt = ROOT.TH1F("histo_FTKd0_umt","Unmatched tracks;d_{0} [mm];Freq.",50,-2,2)
            histo_FTKeta = ROOT.TH1F("histo_FTKeta","All FTKtracks;#eta;Freq.",50,mineta,maxeta)
            histo_FTKeta_umt = ROOT.TH1F("histo_FTKeta_umt","Unmatched tracks;#eta;Freq.",50,mineta,maxeta)
            histo_FTKz0 = ROOT.TH1F("histo_FTKz0","All FTKtracks;z_{0} [mm];Freq.",50,-110,110)
            histo_FTKz0_mt = ROOT.TH1F("histo_FTKz0_mt","matched FTKtracks;z_{0} [mm];Freq.",50,-110,110)
            histo_FTKz0_umt = ROOT.TH1F("histo_FTKz0_umt","Unmatched tracks;z_{0} [mm];Freq.",50,-110,110)
            histo_FTKphieta = ROOT.TH2F("histo_FTKphieta","All FTKtracks;#phi;#eta;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_FTKphieta_umt = ROOT.TH2F("histo_FTKphieta_umt","Unmatched tracks;#phi;Freq.",50,minphi,maxphi,50,mineta,maxeta)
            histo_FTKbarcode = ROOT.TH1F("histo_FTKbarcode","All FTKTracks tracks Barcode;Freq.",50,0,0)
            histo_FTKbarcode_umt = ROOT.TH1F("histo_FTKbarcode_umt","Unmatched tracks Barcode;Freq.",50,0,0)
            histo_FTKEventIndex = ROOT.TH1F("histo_FTKEventIndex","All FTKTracks tracks EventIndex;Freq.",50,0,0)
            histo_FTKEventIndex_umt = ROOT.TH1F("histo_FTKEventIndex_umt","Unmatched tracks EventIndex;Freq.",50,0,0)
            histo_FTKdeltaz0 = ROOT.TH1F("histo_FTKdeltaz0","All FTKtracks deltaz0;Freq",50,0,0);
            histo_FTKdeltaz0_mt = ROOT.TH1F("histo_FTKdeltaz0_mt","matched tracks deltaz0;Freq",50,0,0);
            histo_FTKdeltaz0_umt = ROOT.TH1F("histo_FTKdeltaz0_umt","Unmatched tracks deltaz0;Freq",50,0,0);
            histo_FTKdeltaR_mt = ROOT.TH1F("histo_FTKdeltaR_mt","Matched FTKtracks deltaR;Freq",50,0,0.1);
            histo_FTKdeltaR_umt = ROOT.TH1F("histo_FTKdeltaR_umt","Unmatched tracks deltaR;Freq",50,0,0.1);
            
            
            if debug: print "Made Histograms"


            # prepare the object to loop on the events
            fchain = ftkutils.FTKFileChain(out.inputfiles)

            try :
                maxevent = int(self.settings['maxEvent'].Value())
                if maxevent>out.nEvents :
                    maxevent = out.nEvents
            except :
                maxevent = out.nEvents

            #maxevent = 5000
                
            # loop over the events
            eventcount = 0
            eventcount_step = (maxevent+9)/10
            totaltrackcount = 0
            trackcount = 0
            matchtrackcount = 0
            unmatchtrackcount = 0
            offline_matchtrackcount = 0
            offline_unmatchtrackcount = 0
            FTKmatchOffline_trackcount =0
            FTKunmatchOffline_trackcount =0
            totalfiducialtrackcount = 0
            fiducialtrackcount = 0
            matchfiducialtrackcount = 0
            unmatchfiducialtrackcount = 0
            offline_matchfiducialtrackcount = 0
            offline_unmatchfiducialtrackcount = 0
            FTKmatchOffline_fiducialtrackcount =0
            FTKunmatchOffline_fiducialtrackcount =0
            matchroadcount = 0
            matchfiducialroadcount = 0
            matchedrealstring = ""
            FTKAll=""
            FTKunmatched=""
            
            while fchain.nextevent() :

                #
                #  Event Counting
                #
                if eventcount<int(self.settings["skipEvents"].Value()) :
                    eventcount += 1
                    continue
                if eventcount==maxevent :                    
                    break

                eventcount+=1 # increment the event counter

                if eventcount%eventcount_step == 0 :
                    print "Processed event # %d / %d" % (eventcount,maxevent)

                #
                #Get list of truth tracks for the event
                #
                if debug: print "Getting Truth"
                mytruth = fchain.FTKEvent.truth_tracks           
                if debug: print "Got Truth"

                #
                # remove tracks which do not pass fiducial cuts if doing
                #  the quick option
                #
                if debug: print "Looping on Truth"
                newtruth = []
                for i,mt in enumerate(mytruth):
#                    print i, mt
                    
                    if math.fabs(mt.pdgcode) != 13 and doEffcurve :
                        continue

                    #if mt.barcode == -1 or mt.evtindex == 0 or
                    #if mt.barcode == 0: continue
                    if mt.evtindex != 0 :
                        continue
                    
                    if mt.z0 < minz0 or mt.z0 > maxz0:
                        continue
                    if math.fabs(mt.curv) > maxabscurv:
                        continue

                    if( minphi < maxphi):
                        if float(mt.phi0) > maxphi:
                            continue
                        if float(mt.phi0) < minphi:
                            continue
                    else: #case for phi = pi
                        if float(mt.phi0) > maxphi and float(mt.phi0) < minphi:
                            continue

                    if float(mt.eta) > maxeta:
                        continue
                    if float(mt.eta) < mineta:
                        continue
                    if float(mt.d0) < mind0:
                        continue
                    if float(mt.d0) > maxd0:
                        continue
                        
                    # - pt cut
                    doPtCut = True
                    if doPtCut:
                      if (mt.pt < minpt*10.):
                        continue    

                    if math.fabs(mt.isfiducial)!=0:
                        fiducialtrackcount += 1

                    newtruth.append(mt)
                # EndTruth Loop
                
                # this method is only useful for single particle events
                if len(newtruth)!=1 and doEffcurve: 
                    continue

                #
                #  Count passing truth Tracks
                #
                trackcount += len(newtruth)               

                if debug: print "Get Entry"

                #
                #  Load the FTK and offline tracks
                #   (the event counter has a shift)
                # 
                out.GetEntry(eventcount-1) 

                #
                # Get offline and hits 
                #
                myoffline = fchain.FTKEvent.off_tracks           
                myhits    = fchain.FTKEvent.raw_hits           

                #
                #  Match FTK Tracks and roads to Truth
                #
                if debug: print "Matching To Truth"
                matchres = out.MatchTracksToTruth(newtruth)
                if debug: print "Matched Tracks"
                matchresroad = out.MatchRoadsToTruth(newtruth)
                if debug: print "Matched Roads"
                # the distributions of matched and unmatched are created

                #
                # Clear output ntuple vectors
                #
                m_FTK_d0     .clear()
                m_FTK_z0     .clear()
                m_FTK_phi    .clear()
                m_FTK_Ipt    .clear()
                m_FTK_eta    .clear()
                m_FTK_BitMask.clear()
                
                m_off_d0     .clear()
                m_off_z0     .clear()
                m_off_phi    .clear()
                m_off_Ipt    .clear()
                m_off_eta    .clear()
                
                m_Truth_d0   .clear()
                m_Truth_z0   .clear()
                m_Truth_phi  .clear()
                m_Truth_Ipt  .clear()
                m_Truth_eta  .clear()

                #
                # Truth Tracks
                #
                if debug: print "Loop on Truth"
                for i, mt in enumerate(newtruth) :
                    histo_curv.Fill(mt.curv)
                    histo_pt.Fill(1./(2000.*mt.curv))
                    histo_phi.Fill(mt.phi0)
                    histo_d0.Fill(mt.d0)
                    histo_eta.Fill(mt.eta)
                    histo_z0.Fill(mt.z0)
                    histo_phieta.Fill(mt.phi0,mt.eta)

                    #
                    #  For the Resolution Ntuple
                    #
                    m_Truth_d0 .push_back(mt.d0   )
                    m_Truth_z0 .push_back(mt.z0   )
                    m_Truth_phi.push_back(mt.phi0 )
                    m_Truth_Ipt.push_back(mt.curv )
                    m_Truth_eta.push_back(mt.eta  )

                    #
                    #  Loop on offline
                    #
                    for mo in myoffline:
                        foundOfflineMatchTruth = False

                        #
                        # match to truth
                        #
                        if mo.barcode == mt.barcode and mo.barcode_frac > 0.5:
                            m_off_d0 .push_back(mo.d0   )
                            m_off_z0 .push_back(mo.z0   )
                            m_off_phi.push_back(mo.phi0 )
                            m_off_Ipt.push_back(mo.curv *1000 )
                            m_off_eta.push_back(mo.eta  )
                            if not foundOfflineMatchTruth: # fill histograms only at first match
                                foundOfflineMatchTruth = True
                                offline_matchtrackcount += 1                            
                                if math.fabs(mt.isfiducial)!=0:
                                    offline_matchfiducialtrackcount += 1                            
                                histo_curv_omt.Fill(mt.curv)
                                histo_pt_omt.Fill(1./(2000.*mt.curv))
                                histo_phi_omt.Fill(mt.phi0)
                                histo_d0_omt.Fill(mt.d0)
                                histo_eta_omt.Fill(mt.eta)
                                histo_z0_omt.Fill(mt.z0)
                                histo_phieta_omt.Fill(mt.phi0,mt.eta)
                                if matchres[i]: 
                                    # look for offline match as "both FTK and offline match the same truth"
                                    # denominator is offline matched to truth
                                    FTKmatchOffline_trackcount += 1                            
                                    if math.fabs(mt.isfiducial)!=0:
                                        FTKmatchOffline_fiducialtrackcount += 1                            
                                    histo_curv_mo.Fill(mt.curv)
                                    histo_pt_mo.Fill(1./(2000.*mt.curv))
                                    histo_phi_mo.Fill(mt.phi0)
                                    histo_d0_mo.Fill(mt.d0)
                                    histo_eta_mo.Fill(mt.eta)
                                    histo_z0_mo.Fill(mt.z0)
                                    histo_phieta_mo.Fill(mt.phi0,mt.eta)
                                else:
                                    FTKunmatchOffline_trackcount += 1
                                    if math.fabs(mt.isfiducial)!=0:
                                        FTKunmatchOffline_fiducialtrackcount += 1

                        if not foundOfflineMatchTruth:
                            offline_unmatchtrackcount += 1
                            if math.fabs(mt.isfiducial)!=0:
                                offline_unmatchfiducialtrackcount += 1

                                #if(abs(mt.phi0-matchres[i][1]) < 0.1):
                                #    histo_resphi.Fill(math.acos(math.cos(mt.phi0-matchres[i][1])))
                                #
                                #histo_resip.Fill(mt.d0-matchres[i][3])
                                #histo_resInvPt.Fill(mt.curv-matchres[i][0])
                                #histo_resz0.Fill(mt.z0-matchres[i][4])
                                #histo_reseta.Fill(mt.eta-matchres[i][2])
                                #
                                #histo_resphi_InvPt.Fill(math.acos(math.cos(mt.phi0-matchres[i][1])),mt.curv)
                                ##ROOT.TH2F.SetOption(histo_resphi_InvPt,"CONT1")
                                #histo_resz0_InvPt.Fill(mt.z0-matchres[i][4],mt.curv)
                                #histo_resd0_InvPt.Fill(mt.d0-matchres[i][3],mt.curv)
                                #histo_resd0_phi.Fill(mt.d0-matchres[i][3],mt.phi0)
                                #
                                ##res vs eta
                                #histo_resphi_eta.Fill(math.acos(math.cos(mt.phi0-matchres[i][1])),mt.eta)
                                #histo_resz0_eta.Fill(mt.z0-matchres[i][4],mt.eta)
                                #histo_resd0_eta.Fill(mt.d0-matchres[i][3],mt.eta)
                                #histo_resInvPt_eta.Fill(mt.curv-matchres[i][0],mt.eta)
                                #histo_reseta_eta.Fill(mt.eta-matchres[i][2],mt.eta)
                                #
                                #histo_resphi_FInvPt.Fill(mt.phi0-matchres[i][1],matchres[i][0])
                                #histo_resz0_FInvPt.Fill(mt.z0-matchres[i][4],matchres[i][0])
                                #histo_resd0_FInvPt.Fill(mt.d0-matchres[i][3],matchres[i][0])
                                #histo_reseta_FInvPt.Fill(mt.eta-matchres[i][2],matchres[i][0])
                                #
                                #histo_deltaR.Fill(matchres[i][10])
                                #histo_deltaz0.Fill(matchres[i][14])
                                #histo_d0_ftk.Fill(matchres[i][3])
                            

                    #
                    # if no matching offline tracks, set it to -999
                    # 
                    if m_off_d0.size() ==0:
                        m_off_d0 .push_back(-999)
                        m_off_z0 .push_back(-999)
                        m_off_phi.push_back(-999)
                        m_off_Ipt.push_back(-999)
                        m_off_eta.push_back(-999)

                    #
                    #  Fill Matched FTK Tracks
                    #
                    if matchres[i]:

                        matchtrackcount += 1                            
                        if math.fabs(mt.isfiducial)!=0:
                            matchfiducialtrackcount += 1                            
                        histo_curv_mt.Fill(mt.curv)
                        histo_pt_mt.Fill(1./(2000.*mt.curv))
                        histo_phi_mt.Fill(mt.phi0)
                        histo_d0_mt.Fill(mt.d0)
                        histo_eta_mt.Fill(mt.eta)
                        histo_z0_mt.Fill(mt.z0)
                        histo_phieta_mt.Fill(mt.phi0,mt.eta)

                        if(abs(mt.phi0-matchres[i][1]) < 0.1):
                            histo_resphi.Fill(math.acos(math.cos(mt.phi0-matchres[i][1])))

                        histo_resip.Fill(mt.d0-matchres[i][3])
                        histo_resInvPt.Fill(mt.curv-matchres[i][0])
                        histo_resz0.Fill(mt.z0-matchres[i][4])
                        histo_reseta.Fill(mt.eta-matchres[i][2])

                        histo_resphi_InvPt.Fill(math.acos(math.cos(mt.phi0-matchres[i][1])),mt.curv)
                        #ROOT.TH2F.SetOption(histo_resphi_InvPt,"CONT1")
                        histo_resz0_InvPt.Fill(mt.z0-matchres[i][4],mt.curv)
                        histo_resd0_InvPt.Fill(mt.d0-matchres[i][3],mt.curv)
                        histo_resd0_phi.Fill(mt.d0-matchres[i][3],mt.phi0)
                       
                        #res vs eta
                        histo_resphi_eta.Fill(math.acos(math.cos(mt.phi0-matchres[i][1])),mt.eta)
                        histo_resz0_eta.Fill(mt.z0-matchres[i][4],mt.eta)
                        histo_resd0_eta.Fill(mt.d0-matchres[i][3],mt.eta)
                        histo_resInvPt_eta.Fill(mt.curv-matchres[i][0],mt.eta)
                        histo_reseta_eta.Fill(mt.eta-matchres[i][2],mt.eta)


                        histo_resphi_FInvPt.Fill(mt.phi0-matchres[i][1],matchres[i][0])
                        histo_resz0_FInvPt.Fill(mt.z0-matchres[i][4],matchres[i][0])
                        histo_resd0_FInvPt.Fill(mt.d0-matchres[i][3],matchres[i][0])
                        histo_reseta_FInvPt.Fill(mt.eta-matchres[i][2],matchres[i][0])
                        
                        histo_deltaR.Fill(matchres[i][10])
                        histo_deltaz0.Fill(matchres[i][14])
                        histo_d0_ftk.Fill(matchres[i][3])


                        #
                        #  For the Resolution ntuples
                        #
   
                        m_FTK_d0.push_back(matchres[i][3])
                        m_FTK_z0 .push_back(matchres[i][4])
                        m_FTK_phi.push_back(matchres[i][1])
                        m_FTK_Ipt.push_back(matchres[i][0])
                        m_FTK_eta.push_back(matchres[i][2])
                        m_FTK_BitMask.push_back(int(matchres[i][16]))                        



                        #################
                        
                    #
                    #  No Matching FTK Track
                    #
                    else:
                        m_FTK_d0.push_back(-999)
                        m_FTK_z0 .push_back(-999)
                        m_FTK_phi.push_back(-999)
                        m_FTK_Ipt.push_back(-999)
                        m_FTK_eta.push_back(-999)
                        m_FTK_BitMask.push_back(-999)                        


                        histo_z0_umt.Fill(mt.z0)
                        histo_d0_umt.Fill(mt.d0)
                        histo_curv_umt.Fill(mt.curv)
                        histo_eta_umt.Fill(mt.eta)
                        histo_phi_umt.Fill(mt.phi0)
                        histo_x0_umt.Fill(mt.x0)
                        histo_y0_umt.Fill(mt.y0)
                        histo_barcode_frac_umt.Fill(mt.barcode_frac)
                        histo_evtindex_umt.Fill(mt.evtindex)
                        histo_barcode_umt.Fill(mt.barcode)
                        histo_parentID_umt.Fill(mt.parentID)
                        
                   

                    if matchresroad[i]:
                        matchroadcount += 1
                        if math.fabs(mt.isfiducial)!=0:
                            matchfiducialroadcount += 1
                        histo_curv_mr.Fill(mt.curv)
                        histo_phi_mr.Fill(mt.phi0)
                        histo_d0_mr.Fill(mt.d0)
                        histo_eta_mr.Fill(mt.eta)
                        histo_z0_mr.Fill(mt.z0)
                        histo_phieta_mr.Fill(mt.phi0,mt.eta)

                # end efficiency block
                if debug: print "End Efficiency Block"

                if len(newtruth) > 0:
                    if debug: print "Fill resStudyTree"
                    resStudyTree.Fill()
                
                # fake rate evaluation block
                if debug: print "Unmatch Tracks To Truth"
                unmatchres = out.UnmatchTracksToTruth(mytruth)
                #unmatchres = []
                if debug: print "Unmatched Tracks To Truth"
                if debug: print "Unmatchres"                
                if debug: print unmatchres                
                              
                for i, umt in enumerate(unmatchres) :
                    if debug: print i,umt
                    if debug: print umt!=None

                    if umt != None:
                        if debug: print "Filling Hist"
                        histo_FTKcurv.Fill(umt[1])
                        histo_FTKphi.Fill(umt[2])
                        histo_FTKd0.Fill(umt[4])
                        histo_FTKeta.Fill(umt[3])
                        histo_FTKz0.Fill(umt[5])
                        histo_FTKphieta.Fill(umt[2],umt[3])
                        histo_FTKEventIndex.Fill(umt[8])
                        histo_FTKbarcode.Fill(umt[6])
                        histo_FTKdeltaz0.Fill(umt[13])
                        histo_FTKchi2.Fill(umt[14])
                        histo_FTKpt.Fill(1./(2000.0*umt[1]))
                        histo_FTKhits.Fill(umt[15])
                        if debug: print "Filled Hist"

                        totaltrackcount += 1
                        if math.fabs(mt.isfiducial)!=0:
                            totalfiducialtrackcount += 1
                        if debug: print "Track Count",totaltrackcount
                        FTKAll += "eventindex "+str(umt[6])+" barcodefrac "+str(umt[7])+" barcode "+str(umt[8])+"\n"
                        if unmatchres[i][0]:
                            unmatchtrackcount += 1                            
                            if math.fabs(mt.isfiducial)!=0:
                                unmatchfiducialtrackcount += 1                            
                            histo_FTKcurv_umt.Fill(umt[1])
                            histo_FTKphi_umt.Fill(umt[2])
                            histo_FTKd0_umt.Fill(umt[4])
                            histo_FTKeta_umt.Fill(umt[3])
                            histo_FTKz0_umt.Fill(umt[5])
                            histo_FTKphieta_umt.Fill(umt[2],umt[3])
                            histo_FTKEventIndex_umt.Fill(umt[8])
                            histo_FTKbarcode_umt.Fill(umt[6])
                            histo_FTKdeltaR_umt.Fill(umt[9])
                            histo_FTKdeltaz0_umt.Fill(umt[13])
                            histo_FTKchi2_umt.Fill(umt[14])
                            histo_FTKpt_umt.Fill(1.0/(2000.0*umt[1]))
                            histo_FTKhits_umt.Fill(umt[15])
                            FTKunmatched += str(umt[0])+"eventindex "+str(umt[6])+" barcodefrac "+str(umt[7])+" barcode "+str(umt[8])+"\n"
                        else :
                            histo_FTKdeltaz0_mt.Fill(umt[13])
                            histo_FTKdeltaR_mt.Fill(umt[9])
                            histo_FTKz0_mt.Fill(umt[5])
                            matchedrealstring +=str(umt[0])+"eventindex "+str(umt[7])+" barcodefrac "+str(umt[6])+" barcode "+str(umt[5])+"\n"
                    else:
                        if debug: print "umt None"
                if debug: print "done umt"
                                            
            # the distributions of matched and unmatched are created
            if debug: print "Write resStudyFile Trees"
            resStudyFile.cd()
            resStudyTree.Write()

              
            matchedrealtracks = open("matchedFTK.txt", "w")
            matchedrealtracks.write(matchedrealstring)
            matchedrealtracks.close()                         
            AllFTKtracks = open("AllFTK.txt", "w")
            AllFTKtracks.write(FTKAll)
            AllFTKtracks.close()
            unmatchedFTK = open("unmatchedFTK.txt", "w")
            unmatchedFTK.write(FTKunmatched)
            unmatchedFTK.close()
                
                #end fake rate evaluation block

            #end event loop

            # evalute efficiency
            effint = WilsonIntervalError(matchtrackcount,trackcount)
            covint = WilsonIntervalError(matchroadcount,trackcount) 
            offline_effint = WilsonIntervalError(offline_matchtrackcount,trackcount)
            FTKeffOffline_int = WilsonIntervalError(FTKmatchOffline_trackcount,offline_matchtrackcount)
            print "Coverage=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(matchroadcount)/trackcount,covint[1],covint[0],matchroadcount,trackcount)
            print "Efficiency=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(matchtrackcount)/trackcount,effint[1],effint[0],matchtrackcount,trackcount)
            print "Offline efficiency=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(offline_matchtrackcount)/trackcount,offline_effint[1],offline_effint[0],offline_matchtrackcount,trackcount)
            print "FTK/Offline efficiency=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(FTKmatchOffline_trackcount)/offline_matchtrackcount,FTKeffOffline_int[1],FTKeffOffline_int[0],FTKmatchOffline_trackcount,offline_matchtrackcount)

            fiducialeffint = WilsonIntervalError(matchfiducialtrackcount,fiducialtrackcount)
            fiducialcovint = WilsonIntervalError(matchfiducialroadcount,fiducialtrackcount) 
            fiducialoffline_effint = WilsonIntervalError(offline_matchfiducialtrackcount,fiducialtrackcount)
            fiducialFTKeffOffline_int = WilsonIntervalError(FTKmatchOffline_fiducialtrackcount,offline_matchfiducialtrackcount)
            print "Fiducial Coverage=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(matchfiducialroadcount)/fiducialtrackcount,fiducialcovint[1],fiducialcovint[0],matchfiducialroadcount,fiducialtrackcount)
            print "Fiducial Efficiency=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(matchfiducialtrackcount)/fiducialtrackcount,fiducialeffint[1],fiducialeffint[0],matchfiducialtrackcount,fiducialtrackcount)
            print "Fiducial Offline efficiency=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(offline_matchfiducialtrackcount)/fiducialtrackcount,fiducialoffline_effint[1],fiducialoffline_effint[0],offline_matchfiducialtrackcount,fiducialtrackcount)
            print "Fiducial FTK/Offline efficiency=%6.4f+%6.4f-%6.4f (%d/%d)" % (float(FTKmatchOffline_fiducialtrackcount)/offline_matchfiducialtrackcount,fiducialFTKeffOffline_int[1],fiducialFTKeffOffline_int[0],FTKmatchOffline_fiducialtrackcount,offline_matchfiducialtrackcount)

            try :
                print "Fakerates=%f (%d/%d)" % (float(unmatchtrackcount)/totaltrackcount, unmatchtrackcount, totaltrackcount)
            except ZeroDivisionError :
                print "Error calculating the fake rate:", unmatchtrackcount, totaltrackcount
            if doEffcurve :
                np = int(out.outdir.split("np")[1])*out.nsubregions
                x = np
                id = npseq.index(np)
            else :
                id = outid
                x = outid
            offline_effsummary.SetPoint(id,x,float(offline_matchtrackcount)/trackcount)
            offline_effsummary.SetPointError(id,0,0,offline_effint[0],offline_effint[1])
            FTKeffOffline_summary.SetPoint(id,x,float(FTKmatchOffline_trackcount)/offline_matchtrackcount)
            FTKeffOffline_summary.SetPointError(id,0,0,FTKeffOffline_int[0],FTKeffOffline_int[1])
            effsummary.SetPoint(id,x,float(matchtrackcount)/trackcount)
            effsummary.SetPointError(id,0,0,effint[0],effint[1])
            covsummary.SetPoint(id,x,float(matchroadcount)/trackcount)        
            covsummary.SetPointError(id,0,0,covint[0],covint[1])
            effcurv = ROOT.TGraphAsymmErrors(histo_curv_mt,histo_curv)
            effcurv.SetNameTitle("effcurv","Efficiency;1/p_{T};Eff.")
            effcurv.SetMarkerColor(ROOT.kRed)
            effcurv.SetLineColor(ROOT.kRed)
            curdir.Add(effcurv)
            offline_effcurv = ROOT.TGraphAsymmErrors(histo_curv_omt,histo_curv)
            offline_effcurv.SetNameTitle("offline_effcurv","Offline efficiency;1/p_{T};Eff.")
            offline_effcurv.SetMarkerColor(ROOT.kBlue)
            offline_effcurv.SetLineColor(ROOT.kBlue)
            curdir.Add(offline_effcurv)
            FTKeffOffline_curv = ROOT.TGraphAsymmErrors(histo_curv_mo,histo_curv_omt)
            FTKeffOffline_curv.SetNameTitle("FTKeffOffline_curv","FTK/Offline efficiency;1/p_{T};Eff.")
            FTKeffOffline_curv.SetMarkerColor(ROOT.kRed)
            FTKeffOffline_curv.SetLineColor(ROOT.kRed)
            curdir.Add(FTKeffOffline_curv)
            covcurv = ROOT.TGraphAsymmErrors(histo_curv_mr,histo_curv)
            covcurv.SetNameTitle("covcurv","Coverage;1/p_{T};Eff.")
            covcurv.SetMarkerColor(ROOT.kBlue)
            covcurv.SetLineColor(ROOT.kBlue)
            curdir.Add(covcurv)

            effpt = ROOT.TGraphAsymmErrors(histo_pt_mt,histo_pt)
            effpt.SetNameTitle("effpt","Efficiency;p_{T};Eff.")
            effpt.SetMarkerColor(ROOT.kRed)
            effpt.SetLineColor(ROOT.kRed)
            curdir.Add(effpt)
            offline_effpt = ROOT.TGraphAsymmErrors(histo_pt_omt,histo_pt)
            offline_effpt.SetNameTitle("offline_effpt","Offline efficiency;p_{T};Eff.")
            offline_effpt.SetMarkerColor(ROOT.kBlue)
            offline_effpt.SetLineColor(ROOT.kBlue)
            curdir.Add(offline_effpt)
            FTKeffOffline_pt = ROOT.TGraphAsymmErrors(histo_pt_mo,histo_pt_omt)
            FTKeffOffline_pt.SetNameTitle("FTKeffOffline_pt","FTK/Offline efficiency;p_{T};Eff.")
            FTKeffOffline_pt.SetMarkerColor(ROOT.kRed)
            FTKeffOffline_pt.SetLineColor(ROOT.kRed)
            curdir.Add(FTKeffOffline_pt)


            effphi = ROOT.TGraphAsymmErrors(histo_phi_mt,histo_phi)
            effphi.SetNameTitle("effphi","Efficiency;#phi;Eff.")
            effphi.SetMarkerColor(ROOT.kRed)
            effphi.SetLineColor(ROOT.kRed)
            curdir.Add(effphi)
            offline_effphi = ROOT.TGraphAsymmErrors(histo_phi_omt,histo_phi)
            offline_effphi.SetNameTitle("offline_effphi","Offline efficiency;#phi;Eff.")
            offline_effphi.SetMarkerColor(ROOT.kBlue)
            offline_effphi.SetLineColor(ROOT.kBlue)
            curdir.Add(offline_effphi)
            FTKeffOffline_phi = ROOT.TGraphAsymmErrors(histo_phi_mo,histo_phi_omt)
            FTKeffOffline_phi.SetNameTitle("FTKeffOffline_phi","FTK/Offline efficiency;#phi;Eff.")
            FTKeffOffline_phi.SetMarkerColor(ROOT.kRed)
            FTKeffOffline_phi.SetLineColor(ROOT.kRed)
            curdir.Add(FTKeffOffline_phi)
            covphi = ROOT.TGraphAsymmErrors(histo_phi_mr,histo_phi)
            covphi.SetNameTitle("covphi","Coverage;#phi;Eff.")
            covphi.SetMarkerColor(ROOT.kBlue)
            covphi.SetLineColor(ROOT.kBlue)
            curdir.Add(covphi)

            effeta = ROOT.TGraphAsymmErrors(histo_eta_mt,histo_eta)
            effeta.SetNameTitle("effeta","Efficiency;#eta;Eff.")
            effeta.SetMarkerColor(ROOT.kRed)
            effeta.SetLineColor(ROOT.kRed)
            curdir.Add(effeta)
            offline_effeta = ROOT.TGraphAsymmErrors(histo_eta_omt,histo_eta)
            offline_effeta.SetNameTitle("offline_effeta","Offline efficiency;#eta;Eff.")
            offline_effeta.SetMarkerColor(ROOT.kBlue)
            offline_effeta.SetLineColor(ROOT.kBlue)
            curdir.Add(offline_effeta)
            FTKeffOffline_eta = ROOT.TGraphAsymmErrors(histo_eta_mo,histo_eta_omt)
            FTKeffOffline_eta.SetNameTitle("FTKeffOffline_eta","FTK/Offline efficiency;#eta;Eff.")
            FTKeffOffline_eta.SetMarkerColor(ROOT.kRed)
            FTKeffOffline_eta.SetLineColor(ROOT.kRed)
            curdir.Add(FTKeffOffline_eta)
            coveta = ROOT.TGraphAsymmErrors(histo_eta_mr,histo_eta)
            coveta.SetNameTitle("coveta","Coverage;#eta;Eff.")
            coveta.SetMarkerColor(ROOT.kBlue)
            coveta.SetLineColor(ROOT.kBlue)
            curdir.Add(coveta)

            effz0 = ROOT.TGraphAsymmErrors(histo_z0_mt,histo_z0)
            effz0.SetNameTitle("effz0","Efficiency;z_{0};Eff.")
            effz0.SetMarkerColor(ROOT.kRed)
            effz0.SetLineColor(ROOT.kRed)
            curdir.Add(effz0)
            offline_effz0 = ROOT.TGraphAsymmErrors(histo_z0_omt,histo_z0)
            offline_effz0.SetNameTitle("offline_effz0","Offline fficiency;z_{0};Eff.")
            offline_effz0.SetMarkerColor(ROOT.kBlue)
            offline_effz0.SetLineColor(ROOT.kBlue)
            curdir.Add(offline_effz0)
            FTKeffOffline_z0 = ROOT.TGraphAsymmErrors(histo_z0_mo,histo_z0_omt)
            FTKeffOffline_z0.SetNameTitle("FTKeffOffline_z0","FTK/Offline fficiency;z_{0};Eff.")
            FTKeffOffline_z0.SetMarkerColor(ROOT.kRed)
            FTKeffOffline_z0.SetLineColor(ROOT.kRed)
            curdir.Add(FTKeffOffline_z0)
            covz0 = ROOT.TGraphAsymmErrors(histo_z0_mr,histo_z0)
            covz0.SetNameTitle("covz0","Coverage;z_{0};Eff.")
            covz0.SetMarkerColor(ROOT.kBlue)
            covz0.SetLineColor(ROOT.kBlue)
            curdir.Add(covz0)

            effd0 = ROOT.TGraphAsymmErrors(histo_d0_mt,histo_d0)
            effd0.SetNameTitle("effd0","Efficiency;d_{0};Eff.")
            effd0.SetMarkerColor(ROOT.kRed)
            effd0.SetLineColor(ROOT.kRed)
            curdir.Add(effd0)
            offline_effd0 = ROOT.TGraphAsymmErrors(histo_d0_omt,histo_d0)
            offline_effd0.SetNameTitle("offline_effd0","Offline Efficiency;d_{0};Eff.")
            offline_effd0.SetMarkerColor(ROOT.kBlue)
            offline_effd0.SetLineColor(ROOT.kBlue)
            curdir.Add(offline_effd0)
            FTKeffOffline_d0 = ROOT.TGraphAsymmErrors(histo_d0_mo,histo_d0_omt)
            FTKeffOffline_d0.SetNameTitle("FTKeffOffline_d0","FTK/Offline Efficiency;d_{0};Eff.")
            FTKeffOffline_d0.SetMarkerColor(ROOT.kRed)
            FTKeffOffline_d0.SetLineColor(ROOT.kRed)
            curdir.Add(FTKeffOffline_d0)
            covd0 = ROOT.TGraphAsymmErrors(histo_d0_mr,histo_d0)
            covd0.SetNameTitle("covd0","Coverage;d_{0};Eff.")
            covd0.SetMarkerColor(ROOT.kBlue)
            covd0.SetLineColor(ROOT.kBlue)
            curdir.Add(covd0)

            fakept = ROOT.TGraphAsymmErrors(histo_FTKpt_umt, histo_FTKpt)
            fakept.SetNameTitle("fakept", "Fakerates;p_{T};Fake.")
            fakept.SetMarkerColor(ROOT.kRed)
            fakept.SetLineColor(ROOT.kRed)
            curdir.Add(fakept)

            fakechi2 = ROOT.TGraphAsymmErrors(histo_FTKchi2_umt, histo_FTKchi2)
            fakechi2.SetNameTitle("fakechi2", "Fakerates;#chi^{2}/ndof;Fake.")
            fakechi2.SetMarkerColor(ROOT.kRed)
            fakechi2.SetLineColor(ROOT.kRed)
            curdir.Add(fakechi2)
      
            fakehits = ROOT.TGraphAsymmErrors(histo_FTKhits_umt, histo_FTKhits)
            fakehits.SetNameTitle("fakehits", "Fakerates;N_{hits};Fake.")
            fakehits.SetMarkerColor(ROOT.kRed)
            fakehits.SetLineColor(ROOT.kRed)
            curdir.Add(fakehits)

            fakecurv = ROOT.TGraphAsymmErrors(histo_FTKcurv_umt, histo_FTKcurv)
            fakecurv.SetNameTitle("fakecurv", "Fakerates;1/p_{T};Fake.")
            fakecurv.SetMarkerColor(ROOT.kRed)
            fakecurv.SetLineColor(ROOT.kRed)
            curdir.Add(fakecurv)
            fakephi = ROOT.TGraphAsymmErrors(histo_FTKphi_umt, histo_FTKphi)
            fakephi.SetNameTitle("fakephi", "Fakerates;#phi;Fake.")
            fakephi.SetMarkerColor(ROOT.kRed)
            fakephi.SetLineColor(ROOT.kRed)
            curdir.Add(fakephi)
            fakeeta = ROOT.TGraphAsymmErrors(histo_FTKeta_umt,histo_FTKeta)
            fakeeta.SetNameTitle("fakeeta","Fakerates;#eta;Fake.")
            fakeeta.SetMarkerColor(ROOT.kRed)
            fakeeta.SetLineColor(ROOT.kRed)
            curdir.Add(fakeeta)
            faked0 = ROOT.TGraphAsymmErrors(histo_FTKd0_umt,histo_FTKd0)
            faked0.SetNameTitle("faked0","Fakerates;d_{0};Fake.")
            faked0.SetMarkerColor(ROOT.kBlue)
            faked0.SetLineColor(ROOT.kBlue)
            curdir.Add(faked0)
            fakez0 = ROOT.TGraphAsymmErrors(histo_FTKz0_umt,histo_FTKz0)
            fakez0.SetNameTitle("fakez0","Fakerates;z_{0};Fake.")
            fakez0.SetMarkerColor(ROOT.kBlue)
            fakez0.SetLineColor(ROOT.kBlue)
            curdir.Add(fakez0)
            effdir.cd() # restore the current directory

            rootstack += [covcurv,effcurv,covphi,effphi,coveta,effeta,covz0,effz0,covz0,effz0]
            # Disconnect and reconnect the TTrees to clean the memory
            out.Disconnect(True)
            out.Connect(True) # ask explicitely to reconnect



            
        #end ftkouts loop

            


            print "Phi RMS %s" %histo_resphi.GetRMS()
            print "D0 RMS %s" %histo_resip.GetRMS()
            print "Curv RMS %s" %histo_resInvPt.GetRMS()
            print "Z0 RMS %s" %histo_resz0.GetRMS()
            print "Eta RMS %s" %histo_reseta.GetRMS()

        effdir.Write()
        self.HistoFile.cd()
        
        return None

    def do_clustering(self,text) :
        """Study clustering performance and produce a few standard plots"""
        #
        #  Make small output ntuple for res study
        #
        from ROOT import TTree,TFile,std
        import numpy as n

#        resStudyFile = TFile("resStudy.root","RECREATE")
#        resStudyTree = TTree("ResStudy" , "ResStudy");

#        m_FTK_d0     = std.vector(float)()
#        m_FTK_BitMask= std.vector(int)()
#        resStudyTree.Branch('FTK_d0',        m_FTK_d0       )
#        resStudyTree.Branch('FTK_BitMask',   m_FTK_BitMask  )

        #######

        eventCountPrint = 5
        args = text.split()
        useOfflineLocalPosition = False
        if "useOfflineLocalPosition" in args :
            useOfflineLocalPosition = True
        etaSizeCut = -1
        if "etaSize1" in args :
            etaSizeCut=1
        if "etaSize2" in args :
            etaSizeCut=2
        if "etaSize3" in args :
            etaSizeCut=3
        if "etaSize4" in args :
            etaSizeCut=4
            
        self.HistoFileCD()

        if etaSizeCut>0:
            cludir = ROOT.gDirectory.mkdir("Clustering_etaSize%d"%etaSizeCut) # store the pointer to the root directory
        else:
            cludir = ROOT.gDirectory.mkdir("Clustering") # store the pointer to the root directory
        cludir.cd()
#        effsummary = ROOT.TGraphAsymmErrors(len(ftkouts))
#        effsummary.SetNameTitle("effsummary","Efficiency summary")
#        effsummary.SetMarkerColor(ROOT.kRed)
#        effsummary.SetLineColor(ROOT.kRed)
#        effdir.Add(effsummary)

        rootstack = [] # stack create to prevent the deletion of the objects when they go out of context

        for outid, out in enumerate(ftkouts) : # output stuctures loop
            # skip disabled FTKTree
            if not out.isEnabled:
                continue

            # check if input files were set
            if not out.inputfiles :
                print "No input files for", os.path.basename(out.outdir)
                continue

            print "Evalute efficiency for:", os.path.basename(out.outdir)
            # prepare a  directory for the current output
            curdir = cludir.mkdir(os.path.basename(out.outdir))
            curdir.cd()
            
            # create a set of histogram for any interesting quantity
            
            # clustering resolution histograms
            maxDPhi = 1.2
            maxDEta = 1.2
            maxDPhi_mm = 0.2
            maxDEta_mm = 0.5
            if useOfflineLocalPosition:
                maxDPhi = maxDPhi_mm
                maxDEta = maxDEta_mm

            localx_bins = 100
            localx_min = -10
            localx_max = 20
            localy_bins = 100
            localy_min = -35
            localy_max = 65
            histo_layer_offline = ROOT.TH1F("histo_layer_offline","All clusters",15, 0, 15);
            histo_layer_offlineECap = ROOT.TH1F("histo_layer_offlineECap","All clusters",15, 0, 15);
            histo_layerVsECap_offline_all = ROOT.TH2F("histo_layerVsECap_offline_all","All clusters (before match)", 5, -2, 3, 15, 0, 15);
            histo_layerVsECap_offline_match = ROOT.TH2F("histo_layerVsECap_offline_match","Matched clusters", 5, -2, 3, 15, 0, 15);
            histo_layerVsECap_offline_umt = ROOT.TH2F("histo_layerVsECap_offline_umt","Unmatched clusters", 5, -2, 3, 15, 0, 15);
            histo_etaMod_offline = ROOT.TH1F("histo_etaMod_offline","All clusters",100, -50, 50);
            histo_phiMod_offline = ROOT.TH1F("histo_phiMod_offline","All clusters",100, -50, 50);
            histo_BarECap_offline = ROOT.TH1F("histo_BarEcap_offline","All clusters",30, -15, 15);

            histo_layer = ROOT.TH1F("histo_layer","All clusters",15, 0, 15);
            histo_layerVsECap_all = ROOT.TH2F("histo_layerVsECap_all","All clusters (before match)", 5, -2, 3, 15, 0, 15);
            histo_layerVsECap_match = ROOT.TH2F("histo_layerVsECap_match","Matched clusters", 5, -2, 3, 15, 0, 15);
            histo_layerVsECap_umt = ROOT.TH2F("histo_layerVsECap_umt","Unmatched clusters", 5, -2, 3, 15, 0, 15);
            histo_layer_umt = ROOT.TH1F("histo_layer_umt","Unmatched clusters",15, 0, 15);
            histo_layerECap = ROOT.TH1F("histo_layerECap","Endcap clusters",15, 0, 15);
            histo_layerECap_umt = ROOT.TH1F("histo_layerECap_umt","Unmatched endcap clusters",15, 0, 15);
            histo_etaMod = ROOT.TH1F("histo_etaMod","All clusters",100, -50, 50);
            histo_phiMod = ROOT.TH1F("histo_phiMod","All clusters",100, -50, 50);
            histo_BarECap = ROOT.TH1F("histo_BarEcap","All clusters",30, -15, 15);
            histo_BarECap_umt = ROOT.TH1F("histo_BarEcap_umt","Unmatched clusters",30, -15, 15);
            histo_localYvsX = ROOT.TH2F("histo_localYvsX","All clusters", localx_bins, localx_min, localx_max, localy_bins, localy_min, localy_max)
            histo_localYvsX_offline = ROOT.TH2F("histo_localYvsX_offline","All clusters", 100, -20, 340, 100, -40, 150)
            histo_localYzoom_offline = ROOT.TH1F("histo_localYzoom_offline","All clusters", 600, -3, 3)
            histo_localYzoom = ROOT.TH1F("histo_localYzoom","All clusters", 600, -3, 3)
            
            histo_includesGanged = ROOT.TH1F("histo_includesGanged","All clusters",2, 0, 2);
            histo_trkEta = ROOT.TH1F("histo_trkEta","All clusters", 60, -3, 3);
            histo_trkCotTheta = ROOT.TH1F("histo_trkCotTheta","All clusters", 50, -10, 10);
            histo_trkZ0Raw = ROOT.TH1F("histo_trkZ0Raw","All clusters", 80, -200, 200);
            

            histo_dPhi = ROOT.TH1F("histo_dPhi","All clusters",80,-maxDPhi,maxDPhi)
            histo_dPhi_mm = ROOT.TH1F("histo_dPhi_mm","All clusters",400,-maxDPhi_mm,maxDPhi_mm)
            histo_dPhiBarrel_mm = ROOT.TH1F("histo_dPhiBarrel_mm","All clusters",400,-maxDPhi_mm,maxDPhi_mm)
            histo_dPhiEndcap_mm = ROOT.TH1F("histo_dPhiEndcap_mm","All clusters",400,-maxDPhi_mm,maxDPhi_mm)
            histo_dEta = ROOT.TH1F("histo_dEta","All clusters",80,-maxDEta,maxDEta)
            histo_dEta_mm = ROOT.TH1F("histo_dEta_mm","All clusters",80,-maxDEta_mm,maxDEta_mm)
            histo_dEta_mm_noGanged = ROOT.TH1F("histo_dEta_mm_noGanged","All clusters",80,-maxDEta_mm,maxDEta_mm)
            histo_dPhidEta = ROOT.TH2F("histo_dEta_vs_dPhi","All clusters", 80, -maxDPhi, maxDPhi, 80, -maxDEta, maxDEta)
            histo_rowdPhi = ROOT.TH2F("histo_dPhi_vs_row","All clusters", 34, 0, 340.,80,-maxDPhi,maxDPhi)
            histo_rowdPhiBarrel = ROOT.TH2F("histo_dPhi_vs_row_Barrel","All clusters", 34, 0, 340.,80,-maxDPhi/4,maxDPhi/4)
            histo_rowdPhiEndcap = ROOT.TH2F("histo_dPhi_vs_row_Endcap","All clusters", 34, 0, 340.,80,-maxDPhi/4,maxDPhi/4)
            histo_rowdPhi_L0 = ROOT.TH2F("histo_dPhi_vs_row_L0","L0 clusters", 34, 0, 340.,80,-maxDPhi,maxDPhi)
            histo_rowdPhi_L1 = ROOT.TH2F("histo_dPhi_vs_row_L1","L1 clusters", 34, 0, 340.,80,-maxDPhi,maxDPhi)
            histo_rowdPhi_L2 = ROOT.TH2F("histo_dPhi_vs_row_L2","L2 clusters", 34, 0, 340.,80,-maxDPhi,maxDPhi)
            histo_rowdPhi_L3 = ROOT.TH2F("histo_dPhi_vs_row_L3","L3 clusters", 34, 0, 340.,80,-maxDPhi,maxDPhi)
            histo_coldPhi = ROOT.TH2F("histo_dPhi_vs_col","All clusters", 45, 0, 180.,80,-maxDPhi,maxDPhi)
            histo_colFEdPhi = ROOT.TH2F("histo_dPhi_vs_colFE","All clusters", 18, 0, 18.,80,-maxDPhi,maxDPhi)
            histo_rowdEta = ROOT.TH2F("histo_dEta_vs_row","All clusters", 34, 0, 340.,80,-maxDEta,maxDEta)
            histo_coldEta = ROOT.TH2F("histo_dEta_vs_col","All clusters", 45, 0, 180.,80,-maxDEta,maxDEta)
            histo_coldEtaECap = ROOT.TH2F("histo_dEta_vs_col_ECap","Endcap clusters", 45, 0, 180.,80,-maxDEta,maxDEta)
            histo_coldEtaL0 = ROOT.TH2F("histo_dEta_vs_colL0","L0 clusters", 45, 0, 180.,80,-maxDEta,maxDEta)
            histo_colFEdEta = ROOT.TH2F("histo_dEta_vs_colFE","All clusters", 18, 0, 18.,80,-maxDEta,maxDEta)

            histo_dEtaVSetaTrk = ROOT.TH2F("histo_dEta_vs_etaTrk","All clusters", 60, -3, 3, 80,-maxDEta,maxDEta)
            histo_dEtaVSetaTrk_noGanged = ROOT.TH2F("histo_dEta_vs_etaTrk_noGanged","All clusters", 60, -3, 3, 80,-maxDEta,maxDEta)
            histo_dEtaVSetaSize = ROOT.TH2F("histo_dEta_vs_etaSize","All clusters", 10, 0, 10, 80,-maxDEta,maxDEta)
            histo_dEtaVScluSize = ROOT.TH2F("histo_dEta_vs_size","All clusters", 10, 0, 10, 80,-maxDEta, maxDEta)
            histo_dEtaVSetaTrk_L0 = ROOT.TH2F("histo_dEta_vs_etaTrk_L0","Layer0 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 
            histo_dEtaVSetaTrk_L1 = ROOT.TH2F("histo_dEta_vs_etaTrk_L1","Layer1 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 
            histo_dEtaVSetaTrk_L2 = ROOT.TH2F("histo_dEta_vs_etaTrk_L2","Layer2 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 
            histo_dEtaVSetaTrk_L3 = ROOT.TH2F("histo_dEta_vs_etaTrk_L3","Layer3 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 
            histo_dEtaVSetaTrk_ECapL1 = ROOT.TH2F("histo_dEta_vs_etaTrk_ECapL1","Endcap Layer1 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 
            histo_dEtaVSetaTrk_ECapL2 = ROOT.TH2F("histo_dEta_vs_etaTrk_ECapL2","Endcap Layer2 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 
            histo_dEtaVSetaTrk_ECapL3 = ROOT.TH2F("histo_dEta_vs_etaTrk_ECapL3","Endcap Layer3 clusters", 60, -3, 3, 80,-maxDEta,maxDEta) 

            histo_phiSize = ROOT.TH1F("histo_SizePhi","All clusters",30,0,30)
            histo_etaSize = ROOT.TH1F("histo_SizeEta","All clusters",10,0,10)
            histo_phiVSetaSize = ROOT.TH2F("histo_Size_Phi_vs_Eta","All clusters",10,0,10,30,0,30)            
            histo_cluSize = ROOT.TH1F("histo_Size","All clusters",30,0,30)

            histo_etaModuleVScotThetaTrk_L0 = ROOT.TH2F("histo_etaModuleVScotThetaTrk_L0", "Layer0 clusters", 50, -10, 10, 28, -7, 7)
            histo_etaModuleVScotThetaTrk_L1 = ROOT.TH2F("histo_etaModuleVScotThetaTrk_L1", "Layer1 clusters", 50, -6, 6, 28, -7, 7)
            histo_etaModuleVScotThetaTrk_L2 = ROOT.TH2F("histo_etaModuleVScotThetaTrk_L2", "Layer2 clusters", 50, -5, 5, 28, -7, 7)
            histo_etaModuleVScotThetaTrk_L3 = ROOT.TH2F("histo_etaModuleVScotThetaTrk_L3", "Layer3 clusters", 50, -5, 5, 28, -7, 7)
            histo_etaPixelVScotThetaTrk_L0 = ROOT.TH2F("histo_etaPixelVScotThetaTrk_L0", "Layer0 clusters", 50, -10, 10, 28, -7, 7)
            histo_etaPixelVScotThetaTrk_L1 = ROOT.TH2F("histo_etaPixelVScotThetaTrk_L1", "Layer1 clusters", 50, -6, 6, 28, -7, 7)
            histo_etaPixelVScotThetaTrk_L2 = ROOT.TH2F("histo_etaPixelVScotThetaTrk_L2", "Layer2 clusters", 50, -5, 5, 28, -7, 7)
            histo_etaPixelVScotThetaTrk_L3 = ROOT.TH2F("histo_etaPixelVScotThetaTrk_L3", "Layer3 clusters", 50, -5, 5, 28, -7, 7)
            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L0 = ROOT.TH1F("histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L0", "Layer0 clusters", 40, -1, 1)
            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L1 = ROOT.TH1F("histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L1", "Layer1 clusters", 40, -1, 1)
            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L2 = ROOT.TH1F("histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L2", "Layer2 clusters", 40, -1, 1)
            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L3 = ROOT.TH1F("histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L3", "Layer2 clusters", 40, -1, 1)
            histo_cothetaPixel_VS_cothetaTrackZ0Corr_L0 = ROOT.TH2F("histo_cothetaPixel_VS_cothetaTrackZ0Corr_L0", "Layer0 clusters", 50, -10, 10, 50, -10, 10)
            histo_deltaLocalX_L0 = ROOT.TH1F("histo_deltaLocalX_L0", "Layer0 clusters", 300, -0.15, 0.15)
            histo_deltaLocalY_L0 = ROOT.TH1F("histo_deltaLocalY_L0", "Layer0 clusters", 120, -1.2, 1.2)
            histo_localX_ftkVSoffline_L0 = ROOT.TH2F("histo_localX_ftkVSofflineL0", "Layer0 clusters", localx_bins, localx_min, localx_max,localx_bins, localx_min, localx_max)
            histo_localY_ftkVSoffline_L0 = ROOT.TH2F("histo_localY_ftkVSofflineL0", "Layer0 clusters", localy_bins, localy_min, localy_max,localy_bins, localy_min, localy_max)

            
            # prepare the object to loop on the events
            fchain = ftkutils.FTKFileChain(out.inputfiles)

            try :
                maxevent = int(self.settings['maxEvent'].Value())
                if maxevent>out.nEvents :
                    maxevent = out.nEvents
            except :
                maxevent = out.nEvents
                
            # loop over the events
            eventcount = 0
            eventcount_step = (maxevent+9)/10
            totalclustercount = 0
            clustercount = 0
            fchain.FTKEvent.SetIBLMode(self.settings["useIBL"].Value())

            while fchain.nextevent() :
                if eventcount<int(self.settings["skipEvents"].Value()) :
                    eventcount += 1
                    continue
                if eventcount==maxevent :                    
                    break

                eventcount+=1 # increment the event counter

                if eventcount%eventcount_step == 0 :
                    print "Processed event # %d / %d" % (eventcount,maxevent)

                #Get list of offline clusters
                myOfflineClusters = None
                myOfflineClusters = fchain.FTKEvent.offline_clusters
                offlineCluList = {}
                for qwe in myOfflineClusters:
                    if qwe.BarEcp!=0:
                        histo_layer_offlineECap.Fill(qwe.layer)
                    histo_layer_offline.Fill(qwe.layer)
                    histo_layerVsECap_offline_all.Fill(qwe.BarEcp, qwe.layer)
                    histo_etaMod_offline.Fill(qwe.eta_module)
                    histo_phiMod_offline.Fill(qwe.phi_module)
                    histo_BarECap_offline.Fill(qwe.BarEcp)
                    if eventcount<eventCountPrint:    
                        print "offline"
                        qwe.Print()
                    if offlineCluList.has_key(qwe.moduleId):
                        offlineCluList[qwe.moduleId].append(qwe)
                    else:
                        offlineCluList[qwe.moduleId] = [qwe]
#                print "offline = ", offlineCluList

                #if eventcount<5:
                #    for i,moc in enumerate(myOfflineClusters):
                #        print "event =",eventcount," OffClu =",i," layer =",moc.layer," phi =",moc.offline_phi_center," eta =",moc.offline_eta_center," barcode =",moc.barcode
                    
                out.GetEntry(eventcount-1) # the event counter has a shift

                #TEST CLUSTER MATCH
                ftkCluList = {}
                # out.MatchClusters(myOfflineClusters) will return a list of FTK clusters associated to all FTK tracks
                for matchedCluster in out.MatchClusters(myOfflineClusters):
                    # create a ftkClu with dummy values
                    ftkClu = ftkutils.FTKOfflineCluster([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17], self.settings["useIBL"].Value())
                    # now replace the dummy vlues with good values returned from out.MatchClusters(....
                    ftkClu.setByTuple(matchedCluster, self.settings["useIBL"].Value())
                    histo_layerVsECap_all.Fill(ftkClu.BarEcp, ftkClu.layer)
                    if eventcount<eventCountPrint:    
                        print "------------------> match"
                        print "offline = ", matchedCluster
                        print "ftkClu = "
                        ftkClu.Print()
                    if ftkCluList.has_key(ftkClu.moduleId):
                        ftkCluList[ftkClu.moduleId].append(ftkClu)
                    else:
                        ftkCluList[ftkClu.moduleId] = [ftkClu]
                    minDist = 10000;
                    nPhiPixels = 328.
                    phiPitch = 0.05
                    nEtaPixels = 144.
                    etaPitch = 0.4
                    etaCorrection = 152./144.
                    moduleYlength = 61.2
                    if self.settings["useIBL"].Value() and ftkClu.layer==0:
                        nPhiPixels = 336.
                        phiPitch = 0.05
                        nEtaPixels = 160.
                        etaPitch = 0.25
                        etaCorrection = (162.+0.400/0.250) / nEtaPixels
                        moduleYlength = (162.+0.400/0.250) * etaPitch
                    localx = (ftkClu.offline_phi_center-0.5*nPhiPixels)*phiPitch
                    localy = (ftkClu.offline_eta_center-0.5*nEtaPixels)*etaPitch*etaCorrection;

                    offlineRef = None;
                    if offlineCluList.has_key(ftkClu.moduleId):
                        for off in offlineCluList[ftkClu.moduleId]:
                            offline_phi = off.offline_phi_center
                            offline_eta = off.offline_eta_center
                            if useOfflineLocalPosition:
                                offline_phi = off.side
                                offline_eta = off.strip
                            dPhi = ftkClu.offline_phi_center-off.offline_phi_center
                            dEta = ftkClu.offline_eta_center-off.offline_eta_center
                            if useOfflineLocalPosition:
                                dPhi = (localx-off.offline_phi_center)/phiPitch
                                dEta = (localy-off.offline_eta_center)/etaPitch

#                            distance = ftkClu.Distance(off, useOfflineLocalPosition)
                            distance = math.sqrt(dPhi**2 + dEta**2)
                            if eventcount<eventCountPrint:
                                print "(dphi,deta) = (%f,%f)" % (ftkClu.offline_phi_center-offline_phi,ftkClu.offline_eta_center-offline_eta)
                            if eventcount<eventCountPrint:                                
                                print distance
                            if distance<minDist:
                                minDist = distance
                                offlineRef = off
                    if eventcount<eventCountPrint:                                    
                        print "----", minDist

                    if offlineRef == None or minDist>2:
                        if ftkClu.BarEcp!=0:
                            histo_layerECap_umt.Fill(ftkClu.layer)
                        histo_layer_umt.Fill(ftkClu.layer)
                        histo_BarECap_umt.Fill(ftkClu.BarEcp)
                        histo_layerVsECap_umt.Fill(ftkClu.BarEcp, ftkClu.layer)

                    if offlineRef != None and (etaSizeCut<0 or etaSizeCut==ftkClu.etaSize): # and ftkClu.BarEcp!=0:
                        dPhi = ftkClu.offline_phi_center-offlineRef.offline_phi_center
                        dEta = ftkClu.offline_eta_center-offlineRef.offline_eta_center
                        if useOfflineLocalPosition:
                            dPhi = localx-offlineRef.offline_phi_center
                            dEta = localy-offlineRef.offline_eta_center

                        row = offlineRef.side
                        hasGanged = pixelRowIsGanged(row)
                        col = offlineRef.strip
                        colFE = col % 18

                        if minDist>2:
                            print "------------------> match LARGE DISTANCE: event ", eventcount
                            print "matchedCluster = ", matchedCluster
                            print "ftkClu = "
                            ftkClu.Print()
                            print "(dphi,deta) = (%f,%f)" %(dPhi, dEta) # (ftkClu.offline_phi_center-offline_phi,ftkClu.offline_eta_center-offline_eta)
                            print "----", minDist
                            print "offline min distance = "
                            offlineRef.Print()
                            print "FEcolumn = ", (ftkClu.offline_eta_center%18)

                        if eventcount<eventCountPrint:                                    
                            print "offline min distance = "
                            offlineRef.Print()
                            print "FEcolumn = ", (ftkClu.offline_eta_center%18)

                        if dPhi>maxDPhi:
                            dPhi = maxDPhi*0.99
                        if dPhi<-maxDPhi:
                            dPhi = -maxDPhi*0.99                                
                        if dEta>maxDEta:
                            dEta = maxDEta*0.99
                        if dEta<-maxDEta:
                            dEta = -maxDEta*0.99                                

                        if ftkClu.BarEcp!=0:
                            histo_layerECap.Fill(ftkClu.layer)
                        histo_layer.Fill(ftkClu.layer)
                        histo_layerVsECap_match.Fill(ftkClu.BarEcp, ftkClu.layer)
                        histo_etaMod.Fill(ftkClu.eta_module)
                        histo_phiMod.Fill(ftkClu.phi_module)
                        histo_BarECap.Fill(ftkClu.BarEcp)


                        histo_includesGanged.Fill(ftkClu.includesGanged)
                        histo_trkEta.Fill(ftkClu.trackEta)
                        histo_trkCotTheta.Fill(ftkClu.trackCotTheta)
                        histo_trkZ0Raw.Fill(ftkClu.trackZ0Raw)

                        histo_dPhi.Fill(dPhi)
                        histo_dEta.Fill(dEta)
                        if useOfflineLocalPosition:
                            histo_dPhi_mm.Fill(dPhi)
                            if ftkClu.BarEcp==0:
                                histo_dPhiBarrel_mm.Fill(dPhi)
                            else:
                                histo_dPhiEndcap_mm.Fill(dPhi)
                            histo_dEta_mm.Fill(dEta)
                            if not hasGanged:
                                histo_dEta_mm_noGanged.Fill(dEta)
                        else:
                            histo_dPhi_mm.Fill(dPhi*phiPitch)
                            if ftkClu.BarEcp==0:
                                histo_dPhiBarrel_mm.Fill(dPhi*phiPitch)
                            else:
                                histo_dPhiEndcap_mm.Fill(dPhi*phiPitch)
                            histo_dEta_mm.Fill(dEta*etaPitch)
                            if not hasGanged:
                                histo_dEta_mm_noGanged.Fill(dEta*etaPitch)
                        histo_dPhidEta.Fill(dPhi,dEta)

                        histo_rowdPhi.Fill(row,dPhi)
                        if ftkClu.BarEcp==0:
                            histo_rowdPhiBarrel.Fill(row,dPhi)
                        else:
                            histo_rowdPhiEndcap.Fill(row,dPhi)

                        histo_coldPhi.Fill(col,dPhi)
                        histo_colFEdPhi.Fill(colFE,dPhi)
                        histo_rowdEta.Fill(row,dEta)
                        histo_coldEta.Fill(col,dEta)
                        if ftkClu.BarEcp!=0:
                            histo_coldEtaECap.Fill(col,dEta)
                        histo_colFEdEta.Fill(colFE,dEta)

                        histo_dEtaVSetaTrk.Fill(ftkClu.trackEta, dEta)
                        if not hasGanged:
                            histo_dEtaVSetaTrk_noGanged.Fill(ftkClu.trackEta, dEta)
                        histo_dEtaVSetaSize.Fill(ftkClu.etaSize, dEta)
                        histo_dEtaVScluSize.Fill(ftkClu.NChannels, dEta)
                        etaModuleEstimate = ftkClu.eta_module+ftkClu.offline_eta_center/nEtaPixels-0.5
                        etaModuleEstimateZ0Corr = etaModuleEstimate - ftkClu.trackZ0Raw/moduleYlength
                        radius = 35.78 # IBL (from fig5 in IBL TDR) 
                        myLocalLayerVariable = ftkClu.layer
                        if self.settings["useIBL"].Value():
                            myLocalLayerVariable = myLocalLayerVariable - 1
                        if myLocalLayerVariable==0:
                            radius = 50.5; # Layer 0 (B-Layer)  
                        if myLocalLayerVariable==1:
                            radius = 88.5 # Layer 1
                        if myLocalLayerVariable==2:
                            radius = 122.5 # Layer 2
                        cothetaPixel = ftkClu.pixelEstimateCotTheta #etaModuleEstimate*61.2 / radius
                        cothetaTrackZ0Corr = ftkClu.trackCotTheta+ftkClu.trackZ0Raw/radius

                        histo_localYvsX.Fill(localx, localy)
                        histo_localYvsX_offline.Fill(offlineRef.offline_phi_center, offlineRef.offline_eta_center)
                        localYzoom_offline = offlineRef.offline_eta_center
                        if offlineRef.offline_eta_center > 29:
                            localYzoom_offline = offlineRef.offline_eta_center - 28
                            histo_localYzoom_offline.Fill(localYzoom_offline)
                        if offlineRef.offline_eta_center < -29:
                            localYzoom_offline = offlineRef.offline_eta_center + 28
                            histo_localYzoom_offline.Fill(localYzoom_offline)
                        localYzoom = localy
                        if localy > 29:
                            localYzoom = localy - 28
                            histo_localYzoom.Fill(localYzoom)
                        if localy < -29:
                            localYzoom = localy + 28
                            histo_localYzoom.Fill(localYzoom)
                            

                        if ftkClu.layer==0:
                            histo_coldEtaL0.Fill(col,dEta)
                            histo_dEtaVSetaTrk_L0.Fill(ftkClu.trackEta, dEta)
                            histo_etaModuleVScotThetaTrk_L0.Fill(ftkClu.trackCotTheta, ftkClu.eta_module-ftkClu.trackZ0Raw/moduleYlength)
                            histo_etaPixelVScotThetaTrk_L0.Fill(ftkClu.trackCotTheta, ftkClu.eta_module+ftkClu.offline_eta_center/nEtaPixels-0.5-ftkClu.trackZ0Raw/moduleYlength)
                            histo_cothetaPixel_VS_cothetaTrackZ0Corr_L0.Fill(cothetaTrackZ0Corr, cothetaPixel)
                            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L0.Fill(cothetaPixel-cothetaTrackZ0Corr)
                            histo_deltaLocalX_L0.Fill(localx-offlineRef.offline_phi_center)
                            histo_deltaLocalY_L0.Fill(localy-offlineRef.offline_eta_center)
                            histo_localX_ftkVSoffline_L0.Fill(offlineRef.offline_phi_center, localx)
                            histo_localY_ftkVSoffline_L0.Fill(offlineRef.offline_eta_center, localy)
                            histo_rowdPhi_L0.Fill(row,dPhi)

                        if ftkClu.layer==1:
                            histo_dEtaVSetaTrk_L1.Fill(ftkClu.trackEta, dEta)
                            if ftkClu.BarEcp!=0:
                                histo_dEtaVSetaTrk_ECapL1.Fill(ftkClu.trackEta, dEta)
                            histo_etaModuleVScotThetaTrk_L1.Fill(ftkClu.trackCotTheta, ftkClu.eta_module-ftkClu.trackZ0Raw/moduleYlength)
                            histo_etaPixelVScotThetaTrk_L1.Fill(ftkClu.trackCotTheta, ftkClu.eta_module+ftkClu.offline_eta_center/nEtaPixels-0.5-ftkClu.trackZ0Raw/moduleYlength)
                            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L1.Fill(cothetaPixel-cothetaTrackZ0Corr)
                            histo_rowdPhi_L1.Fill(row,dPhi)
                        if ftkClu.layer==2:
                            histo_dEtaVSetaTrk_L2.Fill(ftkClu.trackEta, dEta)
                            if ftkClu.BarEcp!=0:
                                histo_dEtaVSetaTrk_ECapL2.Fill(ftkClu.trackEta, dEta)
                            histo_etaModuleVScotThetaTrk_L2.Fill(ftkClu.trackCotTheta, ftkClu.eta_module-ftkClu.trackZ0Raw/moduleYlength)
                            histo_etaPixelVScotThetaTrk_L2.Fill(ftkClu.trackCotTheta, ftkClu.eta_module+ftkClu.offline_eta_center/nEtaPixels-0.5-ftkClu.trackZ0Raw/moduleYlength)
                            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L2.Fill(cothetaPixel-cothetaTrackZ0Corr)
                            histo_rowdPhi_L2.Fill(row,dPhi)
                        if ftkClu.layer==3:
                            histo_dEtaVSetaTrk_L3.Fill(ftkClu.trackEta, dEta)
                            if ftkClu.BarEcp!=0:
                                histo_dEtaVSetaTrk_ECapL3.Fill(ftkClu.trackEta, dEta)
                            histo_etaModuleVScotThetaTrk_L3.Fill(ftkClu.trackCotTheta, ftkClu.eta_module-ftkClu.trackZ0Raw/moduleYlength)
                            histo_etaPixelVScotThetaTrk_L3.Fill(ftkClu.trackCotTheta, ftkClu.eta_module+ftkClu.offline_eta_center/nEtaPixels-0.5-ftkClu.trackZ0Raw/moduleYlength)
                            histo_cothetaPixel_Minus_cothetaTrackZ0Corr_L3.Fill(cothetaPixel-cothetaTrackZ0Corr)
                            histo_rowdPhi_L3.Fill(row,dPhi)

                        histo_phiSize.Fill(ftkClu.phiSize)
                        histo_etaSize.Fill(ftkClu.etaSize)
                        histo_phiVSetaSize.Fill(ftkClu.etaSize,ftkClu.phiSize)
                        histo_cluSize.Fill(ftkClu.NChannels)
#                print "ftk = ", ftkCluList

            #resStudyFile.cd()
            #resStudyTree.Write()

            # Disconnect and reconnect the TTrees to clean the memory
            out.Disconnect(True)
            out.Connect(True) # ask explicitely to reconnect

        #end ftkouts loop

        cludir.Write()
        self.HistoFile.cd()
        
        return None

    def do_entry(self,text) :
        """Select an entry. Move the FTK simulation to a given event. The syntax \"entry N\" """
        global ftkouts

        try :
            ievt = int(text)
            self.ievt = ievt
        except ValueError :
            print "Event error not recognized:", text

        if self.ievt == -1 :
            print "*** Read can be not initialized, set a real number of event"
        print "Select event:", self.ievt
        nbytes = 0
        for out in ftkouts :
            nbytes += out.GetEntry(self.ievt)
        print "Number of bytes:", nbytes
        return None

    def complete_next(self,text,line,begidx,endidx) :
        possible = []

        # check the number of arguments in the already existing line up to the character is edited
        args = line[:endidx].split()

        if "with".startswith(text) :
            possible += ["with"]
        if "diff".startswith(text) :
            possible += ["diff"]
        if args[1] in ("with","diff","noext") :
            if "roads".startswith(text) :
                possible += ["roads"]
            if "tracks".startswith(text) :
                possible += ["tracks"]
        return possible
    
    def do_next(self,text) :
        """This method move to the next event or loops over the events until a condition happens, then the standard summary for this event is printed. If multiple outputs are used it is enough that one of them satisfy the criteria . Possible syntax are:
> next with roads
This stops to the next event with at least 1 road, also \"tracks\" is supported
> next diff roads tracks
This stops when a different number of roads or tracks is found. It is possible ask only to check roads or tracks."""
        global ftkouts

        args = text.split()
        if len(args) == 0 :
            self.ievt += 1
            self.do_entry()
        elif len(args)==1 :
            print """*** Not enough argument, \"help next\" for documentation"""
            return None
        
        if "with" == args[0] :
            if args[1] == "roads" :
                toStop = False
                while not toStop :
                    self.ievt += 1 # next event
                    print "Event", self.ievt
                    for out in ftkouts :
                        if not out.isEnabled :
                            continue
                        
                        if self.ievt >= out.nEvents :
                            print "No more events"
                            toStop = True
                            break
                        out.GetEntry(self.ievt)
                        numroads = out.GetNUnmergedRoads()
                        for n in numroads :
                            if n>0 :
                                toStop = True
                                break
                        numroads = out.GetNMergedRoads()
                        for n in numroads :
                            if n>0 :
                                toStop = True
                                break
                print "First events:", self.ievt
                for out in ftkouts :
                    if not out.isEnabled :
                        continue                    
                    out.PrintStatus(False,False,False,False)
            elif args[1] == "tracks" :
                toStop = False
                while not toStop :
                    self.ievt += 1 # next event
                    print "Event", self.ievt
                    for out in ftkouts :
                        if not out.isEnabled :
                            continue
                        if self.ievt >= out.nEvents :
                            print "No more events"
                            toStop = True
                            break
                        out.GetEntry(self.ievt)
                        numtracks = out.GetNUnmergedTracks()
                        for n in numtracks :
                            if n>0 :
                                toStop = True
                                break
                        numtracks = out.GetNMergedTracks()
                        for n in numtracks :
                            if n>0 :
                                toStop = True
                                break
                print "First events:", self.ievt
                for out in ftkouts :
                    if not out.isEnabled :
                        continue
                    out.PrintStatus(False,False,False,False)
            elif args[1] == "noext" :
                toStop = False
                while not toStop :
                    self.ievt += 1 # next event
                    print "Event", self.ievt
                    for out in ftkouts :
                        if not out.isEnabled :
                            continue
                        if self.ievt >= out.nEvents :
                            print "No more events"
                            toStop = True
                            break
                        out.GetEntry(self.ievt)
                        numtracks = out.GetNUnmergedTracks()
                        numtracksI = out.GetNUnmergedTracksI()
                        for n, nI in zip(numtracks,numtracksI) :
                            if n==0 and nI>0 :
                                toStop = True
                                break
                        numtracks = out.GetNMergedTracks()
                        numtracksI = out.GetNMergedTracksI()
                        for n, nI in zip(numtracks,numtracksI) :
                            if n==0 and nI>0 :
                                toStop = True
                                break
                print "First events:", self.ievt
                for out in ftkouts :
                    if not out.isEnabled :
                        continue
                    out.PrintStatus(False,False,False,False)

        elif "diff" == args[0] :
            if args[1] == "roads" :
                toStop = False
                while not toStop :
                    self.ievt += 1 # next event
                    print "Event", self.ievt
                    refNUmRoads = None
                    refNMRoads = None
                    for out in ftkouts :
                        if not out.isEnabled :
                            continue
                        
                        if self.ievt >= out.nEvents :
                            print "No more events"
                            toStop = True
                            break
                        out.GetEntry(self.ievt)
                        numroads = out.GetNUnmergedRoads()
                        if not refNUmRoads:
                            refNUmRoads = numroads
                        elif numroads!=refNUmRoads :
                            toStop = True
                        numroads = out.GetNMergedRoads()
                        if not refNMRoads :
                            refNMRoads = numroads
                        elif numroads != refNMRoads :
                            toStop = True
                print "First events:", self.ievt
                for out in ftkouts :
                    if not out.isEnabled :
                        continue
                    out.PrintStatus(False,False,False,False)
            elif args[1] == "tracks" :
                toStop = False
                while not toStop :
                    self.ievt += 1 # next event
                    print "Event", self.ievt
                    refNUmTracks = None
                    refNMTracks = None                                     
                    for out in ftkouts :
                        if not out.isEnabled:
                            continue
                        if self.ievt >= out.nEvents :
                            print "No more events"
                            toStop = True
                            break
                        out.GetEntry(self.ievt)
                        numtracks = out.GetNUnmergedTracks()
                        if not refNUmTracks :
                            refNUmTracks = numtracks
                        elif numtracks != refNUmTracks :
                            toStop = True
                        numtracks = out.GetNMergedTracks()
                        if not refNMTracks :
                            refNMTracks = numtracks
                        elif numtracks != refNMTracks :
                            toStop = True
                print "First events:", self.ievt
                for out in ftkouts :
                    if not out.isEnabled :
                        continue
                    out.PrintStatus(False,False,False,False)
            
        return None

    def complete_normalize(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["nocross"])
        return possible
    
    def do_normalize(self,text) :
        """Check the list of objects in the outputs and restrict the analysis only to object produced by a common input file. Thge same match is applied among the different outputs.
The cross check can be avoided with "nocross"."""

        doCross = True
        
        args = text.split()
        if "nocross" in args :
            doCross = False
            
        NormalizeOutputs(doCross)
        return None

    def complete_plot(self,text,line,begidx,endidx) :
        # complete with a name of a root object
        args = ["ls","cd"]
        
        # add all the histograms in the current directory
        keys = ROOT.gDirectroy.GetListOfKeys()
        ikey = ROOT.TIter(keys)
        for curkey in ikey.nextkey() :
            args.append(curkey.GetName())
        return self.arguments_complete(text,line,begidx,endidx,args)
    
    def do_plot(self,text) :
        """This command allows to navigate trough the produced plots. The arguments are:
* ls: list the object inside the current directory;
* cd <dir>: move inside the directory;
* <name> [opts] [file.ext]: it draws the object with the same name in the current directory, the optional string "opts" allows to specify the options to pass to the TObject::Draw() method. The 3rd optional argument, if exists, is used to save the current canvas in a file"""
        args = text.split()

        if not self.HistoFile :
            print "*** No histograms created"
            return None
        
        if len(args) == 0 :
            print "*** not enough arguments"
        elif args[0] == "ls" :
            ROOT.gDirectory.ls()
        elif args[0] == "cd" :
            if len(args) > 1 :
                ROOT.gDirectory.cd(args[1])
            else :
                print "No directory specified"
        else :
            # try to show the histogram
            histo = ROOT.gDirectory.Get(args[0])
            if len(args)>1:
                opts = args[1]
            else :
                opts = ""
            if histo :
                histo.Draw(opts)
                if len(args)>2 :
                    for fn in args[2:] :
                        ROOT.gPad.SaveAs(fn)
            else :
                print "Histogram", args[0], "not found"
            
        return None

    def DrawDistributionPlot(self,id,out,keys,colors,legend,
                             title,xlab,ylab,options) :
        print id, keys
        # the first key cis assumed the total, the others sub-categories
        if  keys[0] in out.numbers :
            values = [out.numbers[k] for k in keys]
        else :
            print "*** No  distribution for", keys[0]
            return None

        ranges = statrange(values[0])
        print ranges
        histos = []
        for i, k in enumerate(keys) :
            nbins = 100
            if ranges["range"][1] - ranges["range"][0] < 100 :
                nbits = ranges["range"][1] - ranges["range"][0] + 1
            histo = ROOT.TH1F("histo_"+k, title+" "+os.path.basename(out.outdir), 100, ranges["range"][0], ranges["range"][1]+1)
            if colors[i] :
                histo.SetFillColor(colors[i])
            histos.append(histo)
        histos[0].SetXTitle(xlab)
        histos[0].SetYTitle(ylab)

        # fill the base histogram
        for i in xrange(len(values[0])) :
            v0 = values[0][i]
            histos[0].Fill(v0)
            if v0==0 :
                continue
            v = 0.
            for j in xrange(1,len(values)) :
                v += values[j][i]
                histos[j].Fill(v0,v/v0) # rec+maj

        if id == 0:
            histos[0].Draw()
        else :
            histos[0].Draw("same")
        for ih in xrange(len(histos)-1,0,-1) :
            histos[ih].Draw("same")

        self.rootstack += histos
        
        if "lines" in options :
            ROOT.gPad.Update()
            y1 = ROOT.gPad.GetFrame().GetY1()
            y2 = ROOT.gPad.GetFrame().GetY2()
            line05 = ROOT.TLine(ranges["lvls"][0]+1,y1,ranges["lvls"][0]+1,y2)
            line05.SetLineStyle(3)
            line05.Draw()
            line25 = ROOT.TLine(ranges["lvls"][1]+1,y1,ranges["lvls"][1]+1,y2)
            line25.SetLineStyle(2)
            line25.Draw()
            line75 = ROOT.TLine(ranges["lvls"][3]+1,y1,ranges["lvls"][3]+1,y2)
            line75.SetLineStyle(2)
            line75.Draw()
            line95 = ROOT.TLine(ranges["lvls"][4]+1,y1,ranges["lvls"][4]+1,y2)
            line95.SetLineStyle(3)
            line95.Draw()
            line75.Draw()
            line99 = ROOT.TLine(ranges["lvls"][5]+1,y1,ranges["lvls"][5]+1,y2)
            line99.Draw()
            self.rootstack += [line05,line25,line75,line95,line99]

        if legend :
            height = 0.06*len(keys)
            y2 = 1-ROOT.gStyle.GetPadTopMargin()-0.03
            x2 = 1-ROOT.gStyle.GetPadRightMargin()-0.03
            leg = ROOT.TLegend(x2-.25,y2-height,x2,y2)
            leg.SetFillColor(0)
            for i, lbl in enumerate(legend) :
                leg.AddEntry(histos[i],lbl,"f")
            leg.Draw()
            self.rootstack += [leg]
        return None

    def MakeRegionsPlot(self,out,keyfmt,title,xlab,ylab) :
        global prog_options

        outcolor = [(ROOT.kBlack,ROOT.kGray+2),(ROOT.kBlue,ROOT.kBlue-7),
                    (ROOT.kRed+1,ROOT.kRed-9)]

        xshift = float(out.id)/len(ftkouts)
        
        values = []
        for ireg in xrange(prog_options.numregions) :
            dickey = keyfmt % ireg
            if  dickey in out.numbers :
                x = ireg + xshift
                values.append((x,statrange(out.numbers[dickey])))
            else :
                print "*** No distribution", keyfmt % ireg, "in", os.path.basename(out.outdir)
                continue

        if len(values) == 0 :
            print "** No graphic object created"
            return None
        
        graph50 = ROOT.TGraphAsymmErrors(len(values))
        graph90 = ROOT.TGraphAsymmErrors(len(values))

        for i, v in enumerate(values) :
            ranges = v[1]
            # Please keep this consistent with values in legStatLabels
            graph50.SetPoint(i,v[0],ranges["mean"])
            graph50.SetPointEYlow(i,ranges["mean"]-ranges["lvls"][1])
            graph50.SetPointEYhigh(i,ranges["lvls"][4]-ranges["mean"])
            graph90.SetPoint(i,v[0],ranges["lvls"][2])
            graph90.SetPointEYlow(i,ranges["lvls"][2]-ranges["lvls"][0])
            graph90.SetPointEYhigh(i,ranges["lvls"][5]-ranges["lvls"][2])
        graph90.SetTitle(title+" "+os.path.basename(out.outdir))
        graph90.GetXaxis().SetTitle(xlab)
        graph90.GetYaxis().SetTitle(ylab)
        graph90.SetLineColor(outcolor[out.id%3][1])
        graph90.SetMarkerColor(outcolor[out.id%3][1])
        graph50.SetLineColor(outcolor[out.id%3][0])
        graph50.SetMarkerColor(outcolor[out.id%3][0])
        graph50.SetLineWidth(ROOT.gStyle.GetLineWidth()*2)
        graph50.SetMarkerSize(2)
        graph90.GetXaxis().SetLimits(-.5+xshift,prog_options.numregions-.5+xshift)
        return (graph90,graph50)

    def SetRegionPlotsLimits(self,graphs) :
        refXaxis = graphs[0][0].GetXaxis()
        refYaxis = graphs[0][0].GetYaxis()
        xmin = refXaxis.GetXmin()
        xmax = refXaxis.GetXmax()
        ymin = refYaxis.GetXmin()
        ymax = refYaxis.GetXmax()

        for chkgr in graphs[1:] :
            chkXaxis = chkgr[0].GetXaxis()
            chkYaxis = chkgr[0].GetYaxis()
            if xmin > chkXaxis.GetXmin() :
                xmin = chkXaxis.GetXmin()
            if xmax < chkXaxis.GetXmax() :
                xmax = chkXaxis.GetXmax()
            if ymin > chkYaxis.GetXmin() :
                ymin = chkYaxis.GetXmin()
            if ymax < chkYaxis.GetXmax() :
                ymax = chkYaxis.GetXmax()
        
        #print xmin, xmax, ymin, ymax
        refXaxis.SetLimits(xmin,xmax)
        refYaxis.SetRangeUser(ymin,ymax)
        return None
    
    def complete_present(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,
                                           ["nclusters","nss","nclusterss","nroads","nroadsdistr",
                                            "ntfhits","ntfhitsdistr","ntracks","ntracksdistr",
                                            "nfits","nfitsdistr","refsub","clear"])
        return possible
    
    def GetPlotName(self,midname) :
        return str(self.settings["PresentPrefix"])+"_"+midname+"."+str(self.settings["PresentExt"])
    
    def do_present(self,text) :
        """Produce one or more canvases showing a standard graphical representation of common values. The command syntax is:

FTK> present name[:opt1,opt2]

Where "name" is the presentation name. The name can have additional option.

Currently known presentations are (options between brackets):
 * nclusters (leg, leg0)
 * nss (leg, leg0)
 * nclusterss (lines)
 * nroads (leg)
 * nroadsdistr (lines)
 * ntfhits (leg)
 * ntracks (leg)
 * ntracksdistr (lines)
 * nfits (leg)
 * nfitsdistr (lines)
 * clear (not do nothing, just clear the existing canvases)

The "leg" options draws a legend in the plots, "leg0" prints the leg in only the first plot of the sequence, when multiple plots are produced in the presentatoin.

If the interpreter variable PresentPrefix is not None after each study a file named: <PresentPrefix>_<study>.<PresentExt> is created. PresentPrefix could be a path or single part of file name.

Other keywords can be used, not related to a particular presentations. These word are used as global options:
 * refsub <N>
 * subtot

The command by default looks for merged objects but using the options "refsub X" the subregion X in used instead of the whole regions.

In case the merged objects are missing the "study" command may also provide merged values for specific ntuples. The option "subtot" switch to those values."""

        global prog_options

        arguments = text.split()
        # This stack collects the reference to the ROOT object, this avoids the Python GC
        # removes them as soon as they  are out of context
        # The stack is cleared any time a new presentation is asked
        self.rootstack = []
        self.canvaslist = []
        # force the closure of the list of canvases
        ROOT.gROOT.GetListOfCanvases().Delete()
        
        # When unmerged objects are used a reference sub-region can be assigned
        refsub = None
        if "refsub" in arguments :
            id = arguments.index("refsub")
            try :
                refsub = int(arguments[id+1])
                print "Reference sub-region:", refsub
                del arguments[id:id+2]
            except :
                print "refsub it is not followed from a valid number. Command interrupted"
                return None

        # In case there aren't merged object the custom merge done in FTKTree can be used
        useSubTotal = False
        if "subtot" in arguments :
            useSubTotal = True
            id = arguments.index("subtot")
            print "Use custom merged values from sub-regions"
            del arguments[id:id+1]

        for arg in arguments : # loop over the asked keys
            elems = arg.split(':')
            key = elems[0]
            if len(elems) > 1 :
                options = elems[1].split(",")
            else :
                options = []

            if key == "clear" :
                # not do nothing just clear the previous presentation
                continue
            elif key == "nclusters" :
                doLeg=False
                if "leg" in options:
                    doLeg=True

                doLeg0=False
                if "leg0" in options:
                    doLeg=True
                    doLeg0=True

                canvaslist = []
                graphstacklist = []
                for out in ftkouts :
                    for ipl in xrange(out.nlayers) :
                        if len(canvaslist)>=(ipl+1) :
                            curcanvas = canvaslist[ipl]
                            curcanvas.cd()
                            canvasInit = False
                        else :
                            curcanvas = ROOT.TCanvas("Canvas_NClustersL%d" % ipl,"NClustersL%d" % ipl,1)
                            canvaslist += [curcanvas]
                            graphstacklist.append([])
                            canvasInit = True

                        if refsub == None:
                            keyfmt = "NClustersReg%dL"+str(ipl)
                        else :
                            keyfmt = "NClustersReg%dSub"+str(refsub)+"L"+str(ipl)
                            
                        res = self.MakeRegionsPlot(out,keyfmt,
                                                   "Layer %d" % ipl,
                                                   "Region","N Hits")
                        if not res :
                            continue

                        graphstacklist[ipl] += [res]
                        if canvasInit :
                            res[0].Draw("ap")
                            res[1].Draw("p")
                        else :
                            res[0].Draw("p")
                            res[1].Draw("p")
                    # end loop over layer
                # end loop over the outputs
                
                for graphstack in graphstacklist :
                    self.SetRegionPlotsLimits(graphstack)
                    self.rootstack += graphstack

                # put a legend in each canvas
                if doLeg :
                    for ipl in xrange(prog_options.nlayers) :
                        if doLeg0 and ipl>0 :
                            break
                        curcanvas = canvaslist[ipl]
                        curcanvas.cd()
                        # create a legend
                        leg = ROOT.TLegend(0.6,0.75,0.9,0.9)
                        leg.SetFillColor(0)
                        for id, out in enumerate(ftkouts) :
                            if ipl<out.nlayers :
                                leg.AddEntry(graphstacklist[ipl][id][1],os.path.basename(out.outdir),"pl")
                        #end output loop
                        leg.Draw()
                        self.rootstack += [leg]
                        
                    #end layer loop

                for tmpcanvas in canvaslist :
                    tmpcanvas.Update()
                    if self.settings["PresentPrefix"].Value() :
                        tmpcanvas.SaveAs(self.GetPlotName(tmpcanvas.GetTitle()))
                self.canvaslist += canvaslist

            elif key == "nss" :
                doLeg=False
                if "leg" in options:
                   doLeg=True
                doLeg0=False
                if "leg0" in options:
                    doLeg=True
                    doLeg0=True
                    
                canvaslist = []
                graphstacklist = []
                for out in ftkouts :
                    for ipl in xrange(out.nlayers) :
                        if len(canvaslist)>=(ipl+1) :
                            curcanvas = canvaslist[ipl]
                            curcanvas.cd()
                            canvasInit = False
                        else :
                            curcanvas = ROOT.TCanvas("Canvas_NSSL%d" % ipl,"NSSL%d" % ipl,1)
                            canvaslist += [curcanvas]
                            graphstacklist.append([])
                            canvasInit = True

                        if refsub == None :
                            keyfmt = "NSSReg%dL"+str(ipl)
                        else :
                            keyfmt = "NSSReg%dSub"+str(refsub)+"L"+str(ipl)
                        res = self.MakeRegionsPlot(out,keyfmt,"Layer %d" % ipl,"Region","N SS")
                        if not res :
                            continue

                        graphstacklist[ipl] += [res]
                        if canvasInit :
                            res[0].Draw("ap")
                            res[1].Draw("p")
                        else :
                            res[0].Draw("p")
                            res[1].Draw("p")

                for graphstack in graphstacklist :
                    self.SetRegionPlotsLimits(graphstack)
                    self.rootstack += graphstack

                if doLeg :
                    for ipl in xrange(prog_options.nlayers) :
                        if doLeg0 and ipl>0 :
                            break
                        curcanvas = canvaslist[ipl]
                        curcanvas.cd()
                        # create a legend
                        leg = ROOT.TLegend(0.6,0.7, 0.9,0.9)
                        leg.SetFillStyle(0)
                        for id, out in enumerate(ftkouts) :
                            if ipl<out.nlayers :
                                leg.AddEntry(graphstacklist[ipl][id][1],os.path.basename(out.outdir),"pl")
                        #end output loop
                        leg.Draw()
                        self.rootstack += [leg]

                self.canvaslist += canvaslist
                for curcanvas in canvaslist :
                    curcanvas.Update()
                    if self.settings["PresentPrefix"].Value() :
                        curcanvas.SaveAs(self.GetPlotName(curcanvas.GetTitle()))
            elif key == "nclusterss" : # present number of roads per region distributions
                for ipl in xrange(prog_options.nlayers) :
                    curcanvas = ROOT.TCanvas("Canvas_NClusterSSL%d" % ipl, "N Cluster SS Distr Layer %d" % ipl,1)
                    self.canvaslist += [curcanvas]
                    for ireg in plotregions :
                        if refsub == None :
                            dickey = "NClustersSSReg%dL%d" % (ireg,ipl)
                        else :
                            dickey = "NClustersSSReg%dSub%dL%d" % (ireg,refsub,ipl)
                        for id, out in enumerate(ftkouts) :
                            if  dickey in out.numbers :                            
                                values = out.numbers[dickey]
                            else :
                                print "*** No distribution for", dickey, "in", os.path.basename(out.outdir)
                                continue
                            ranges = statrange(values)
                            nbins = ranges["range"][1]-ranges["range"][0]+1
                            histo = ROOT.TH1F("histo_nhitsssR%dL%dO%d" % (ireg,ipl,id), "Layer %d %s" % (ipl,os.path.basename(out.outdir)), nbins, ranges["range"][0], ranges["range"][1]+1)
                            histo.SetXTitle("N Hits")
                            histo.SetYTitle("N SS")
                            for v in values :
                                histo.Fill(v)

                            if ireg == 0:
                                histo.Draw()
                            else :
                                histo.Draw("same")

                            color = ROOT.gStyle.GetColorPalette(ireg)
                            histo.SetLineColor(color)
                            histo.SetFillColor(0)
                            
                            self.rootstack += [histo]
                            if "lines" in options :
                                ROOT.gPad.Update()
                                y1 = ROOT.gPad.GetFrame().GetY1()
                                y2 = ROOT.gPad.GetFrame().GetY2()
                                line95 = ROOT.TLine(ranges["lvls"][4]+1,y1,ranges["lvls"][4]+1,y2)
                                line95.SetLineStyle(3)
                                line95.SetLineColor(color);
                                line95.Draw()
                                line99 = ROOT.TLine(ranges["lvls"][5]+1,y1,ranges["lvls"][5]+1,y2)
                                line99.SetLineColor(color);
                                line99.Draw()
                                self.rootstack += [line95,line99]
                    if self.settings["PresentPrefix"].Value() :
                        curcanvas.SaveAs(self.GetPlotName("NClusterSSL%d" % ipl))
                    else :
                        curcanvas.Update()
            elif key == "nroads" : # present number of roads per region distributions
                doLeg = False
                if "leg" in options :
                    doLeg = True
                doLegStat = False     
                if "legStat" in options:
                    doLegStat = True
                    
                curcanvas = ROOT.TCanvas("Canvas_NRoads","NRoads",1)
                self.canvaslist += [curcanvas]
                canvasInit = True

                graphstack = []
                for out in ftkouts :
                    if refsub == None :
                        if useSubTotal :
                            keyfmt = "NRoadsR%dT"
                        else :
                            keyfmt = "NRoadsR%d"
                    else :
                        keyfmt = "NRoadsR%dS"+str(refsub)
                        
                    res = self.MakeRegionsPlot(out,keyfmt,
                                               "All Roads","Region","N Roads")
                    if not res :
                        continue

                    graphstack += [res]
                    if canvasInit :
                        res[0].Draw("ap")
                        res[1].Draw("p")
                        canvasInit = False
                    else :
                        res[0].Draw("p")
                        res[1].Draw("p")
                #end loop over the outputs

                # reset the regions limit to show all the values
                self.SetRegionPlotsLimits(graphstack)
                self.rootstack += graphstack

                # create a legend
                if doLeg :
                    leg = ROOT.TLegend(0.6,0.7, 0.9,0.9)
                    for id, out in enumerate(ftkouts) :
                        leg.AddEntry(graphstack[id][1],os.path.basename(out.outdir),"pl")
                    leg.Draw()
                    self.rootstack += [leg]

                #Statistics legend 
                if doLegStat:
                    legStat = ROOT.TLegend(0.6,0.78,0.9,0.88)
                    if doLeg:
                        legStat = ROOT.TLegend(0.6,0.6,0.9,0.7)
                    legStat.SetFillColor(0)
    
                    for id, out in enumerate(ftkouts) :
                        legStat.AddEntry(graphstack[id][0],legStatLabels[0],"pl")
                        legStat.AddEntry(graphstack[id][1],legStatLabels[1],"pl")
                        break
                    legStat.Draw()
                    self.rootstack += [legStat]


                if self.settings["PresentPrefix"].Value() :
                    curcanvas.SaveAs(self.GetPlotName("NRoads"))
                else :                        
                    curcanvas.Update()
            elif key == "nroadsdistr" : # present number of roads per region distributions
                for ireg in plotregions :
                    for id, out in enumerate(ftkouts) :
                        curcanvas = ROOT.TCanvas("Canvas_NRoadsR%dO%d" % (ireg,id), "Roads Distr %d Out %d" % (ireg,id),1)
                        self.canvaslist += [curcanvas]

                        
                        dickeys = ["NRoadsR%d" % ireg, "NMajRoadsR%d" % ireg]
                        colors = (None,ROOT.ROOT.kBlue)
                        legend = ["All Roads","Majorty"]
                        self.DrawDistributionPlot(id,out,dickeys,colors,legend,
                                                  "Reg %d" % ireg,"N Roads","N Events",options)
                        if self.settings["PresentPrefix"].Value() :
                            curcanvas.SaveAs(self.GetPlotName("NRoadsDistrR%dO%d" % (ireg,id)))
                        else :                        
                            curcanvas.Update()
            elif key == "ntfhits" : # present number of roads per region distributions
                doLeg=False
                if "leg" in options:
                    doLeg=True

                curcanvas = ROOT.TCanvas("Canvas_NTHFits","NTFHits",1)
                self.canvaslist += [curcanvas]
                canvasInit = True

                graphstack = []
                for out in ftkouts :
                    if refsub == None :
                        if useSubTotal :
                            keyfmt = "NTFHitsR%dT"
                        else :
                            keyfmt = "NTFHitsR%d"
                    else :
                        keyfmt = "NTFHitsR%dS"+str(refsub)
                        
                    res = self.MakeRegionsPlot(out,keyfmt,
                                               "Hits to TF","Region","N Hits")
                    if not res :
                        continue

                    graphstack += [res]
                    if canvasInit :
                        res[0].Draw("ap")
                        res[1].Draw("p")
                        canvasInit = False
                    else :
                        res[0].Draw("p")
                        res[1].Draw("p")
                self.SetRegionPlotsLimits(graphstack)
                self.rootstack += graphstack

                if doLeg :
                    leg = ROOT.TLegend(0.6,0.7, 0.9,0.9)
                    for id, out in enumerate(ftkouts) :
                        leg.AddEntry(graphstack[id][1],os.path.basename(out.outdir),"pl")
                    leg.Draw()
                    self.rootstack += [leg]

                if self.settings["PresentPrefix"].Value() :
                    curcanvas.SaveAs(self.GetPlotName("NTFHits"))
                else :                        
                    curcanvas.Update()
            elif key == "ntfhitsdistr" : # present number of roads per region distributions
                for ireg in plotregions :
                    for id, out in enumerate(ftkouts) :
                        curcanvas = ROOT.TCanvas("Canvas_NTFHitsR%dO%d" % (ireg,id), "TF Hits Distr %d Out %d" % (ireg,id),1)
                        self.canvaslist += [curcanvas]

                        
                        dickeys = ["NTFHitsR%d" % ireg]
                        colors = (None,)
                        legend = None
                        self.DrawDistributionPlot(id,out,dickeys,colors,legend,
                                                  "Reg %d" % ireg,"N Hits","N Events",options)
                        if self.settings["PresentPrefix"].Value() :
                            curcanvas.SaveAs(self.GetPlotName("NTFHitsDistrR%dO%d" % (ireg,id)))
                        else :                        
                            curcanvas.Update()
            elif key == "ntracks" : # present number of tracks per region distributions
                doLeg=False
                if "leg" in options:
                    doLeg=True

                curcanvas = ROOT.TCanvas("Canvas_NTracks","NTracks",1)
                self.canvaslist += [curcanvas]
                canvasInit = True

                graphstack = []
                for out in ftkouts :
                    if refsub == None :
                        if useSubTotal :
                            keyfmt = "NTracksR%dT"
                        else :
                            keyfmt = "NTracksR%d"
                    else :
                        keyfmt = "NTracksR%dS"+str(refsub)
                        
                    res = self.MakeRegionsPlot(out,keyfmt,
                                               "All Tracks","Region","N Tracks")
                    if not res :
                        continue

                    graphstack += [res]
                    if canvasInit :
                        res[0].Draw("ap")
                        res[1].Draw("p")
                        canvasInit = False
                    else :
                        res[0].Draw("p")
                        res[1].Draw("p")
                self.SetRegionPlotsLimits(graphstack)
                self.rootstack += graphstack

                if doLeg :
                    leg = ROOT.TLegend(0.6,0.7, 0.9,0.9)
                    for id, out in enumerate(ftkouts) :
                        leg.AddEntry(graphstack[id][1],os.path.basename(out.outdir),"pl")
                    leg.Draw()
                    self.rootstack += [leg]

                if self.settings["PresentPrefix"].Value() :
                    curcanvas.SaveAs(self.GetPlotName("NTracks"))
                else :                        
                    curcanvas.Update()
            elif key == "ntracksdistr" : # present number of roads per region distributions
                for ireg in plotregions :
                    for id, out in enumerate(ftkouts) :
                        curcanvas = ROOT.TCanvas("Canvas_NTracksR%dO%d" % (ireg,id), "Tracks Distr %d Out %d" % (ireg,id),1)
                        self.canvaslist += [curcanvas]

                        
                        dickeys = ["NTracksR%d" % ireg, "NRecTracksR%d" % ireg, "NMajTracksR%d" % ireg]
                        colors = (None,ROOT.kRed+1,ROOT.kBlue)
                        legend = ["All Tracks","Recovery","Majorty"]
                        self.DrawDistributionPlot(id,out,dickeys,colors,legend,
                                                  "Reg "+str(ireg),
                                                  "N Tracks","N Events",options)
                        if self.settings["PresentPrefix"].Value() :
                            curcanvas.SaveAs(self.GetPlotName("NTracksDistrR%dO%d" % (ireg,id)))
                        else :                        
                            curcanvas.Update()
            elif key == "nfits" : # present number of tracks per region distributions
                doLeg=False
                if "leg" in options:
                    doLeg=True
                doLegStat = False     
                if "legStat" in options:
                    doLegStat = True

                curcanvas = ROOT.TCanvas("Canvas_NFits","NFits",1)
                self.canvaslist += [curcanvas]
                canvasInit = True

                graphstack = []
                for out in ftkouts :
                    if refsub == None :
                        if useSubTotal :
                            keyfmt = "NFitsR%dT"
                        else :
                            keyfmt = "NFitsR%d"
                    else :
                        keyfmt = "NFitsR%dS"+str(refsub)
                        
                    res = self.MakeRegionsPlot(out,keyfmt,
                                               "Total Fits","Region","N Fits")
                    if not res :
                        continue

                    graphstack += [res]
                    if canvasInit :
                        res[0].Draw("ap")
                        res[1].Draw("p")
                        canvasInit = False
                    else :
                        res[0].Draw("p")
                        res[1].Draw("p")
                self.SetRegionPlotsLimits(graphstack)
                self.rootstack += graphstack

                if doLeg :
                    leg = ROOT.TLegend(0.6,0.7, 0.9,0.9)
                    for id, out in enumerate(ftkouts) :
                        leg.AddEntry(graphstack[id][1],os.path.basename(out.outdir),"pl")
                    leg.Draw()
                    self.rootstack += [leg]

                #Statistics legend 
                if doLegStat:
                    legStat = ROOT.TLegend(0.6,0.78,0.9,0.88)
                    if doLeg:
                        legStat = ROOT.TLegend(0.6,0.6,0.9,0.7)
                    legStat.SetFillColor(0)
    
                    for id, out in enumerate(ftkouts) :
                        legStat.AddEntry(graphstack[id][0],legStatLabels[0],"pl")
                        legStat.AddEntry(graphstack[id][1],legStatLabels[1],"pl")
                        break
                    legStat.Draw()
                    self.rootstack += [legStat]

                if self.settings["PresentPrefix"].Value() :
                    curcanvas.SaveAs(self.GetPlotName("NFits"))
                else :                        
                    curcanvas.Update()
            elif key == "nfitsdistr" : # present number of roads per region distributions
                for ireg in plotregions :
                    for id, out in enumerate(ftkouts) :
                        curcanvas = ROOT.TCanvas("Canvas_NFitsR%dO%d" % (ireg,id), "Fits Distr %d" % ireg,1)
                        self.canvaslist += [curcanvas]

                        if refsub == None :
                            if useSubTotal :                                
                                dickeys = ["NFitsR%dT" % ireg, "NRecFitsR%dT" % ireg, "NMajFitsR%dT" % ireg]
                            else :
                                dickeys = ["NFitsR%d" % ireg, "NRecFitsR%d" % ireg, "NMajFitsR%d" % ireg]
                        else :
                            dickeys = ["NFitsR%dS%d" % (ireg,refsub), "NRecFitsR%dS%d" % (ireg,refsub), "NMajFitsR%dS%d" % (ireg,refsub)]
                        colors = (None,ROOT.kRed+1,ROOT.kBlue)
                        legend = ["All Fits","Recovery","Majorty"]
                        self.DrawDistributionPlot(id,out,dickeys,colors,legend,
                                                  "Reg "+str(ireg),
                                                  "N Fits","N Events",options)
                        if self.settings["PresentPrefix"].Value() :
                            curcanvas.SaveAs(self.GetPlotName("NFitsDistrR%dO%d" % (ireg,id)))
                        else :                        
                            curcanvas.Update()
            else :
                print "*** Unknown key:", key
                print "*** not graphical object produced"
        # end loop over the keys
        return None

    def complete_print(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["boxplot","outliers","save"])
        return possible

    def FormatResult(self,res) :
        
        return line

    def do_rebalance(self, text) :
        """Rebalance workload by moving 25% workload from an endcap tower to the corresponding barrel tower. Where barrel_tower = endcap_tower+- 16.
        
The internal options are:
* rebalance: change the rebalance fraction. By default we move 25% workload from endcap to barrel"""
        global ftkouts
        import copy
        rebalance=0.25
        for out in ftkouts : # loop over the outputs
            if out.nregions != 64:
                print "Cannot rebalance!! Rebalance works only with 64 towers"
            rebalanceNumbers = {}
            for VarName in "NRoadsR", "NFitsR", "NFitsIR":
                for reg in xrange(16):
                    endcapA = '%s%d'%(VarName,reg) in out.numbers and '%s%d'%(VarName,reg+16) in out.numbers
                    endcapC = '%s%d'%(VarName,reg+48) in out.numbers and '%s%d'%(VarName,reg+32) in out.numbers
                    if endcapA:
                        valsEndCapA = copy.copy(out.numbers['%s%d'%(VarName,reg)])    
                        valsBarrelA = copy.copy(out.numbers['%s%d'%(VarName,reg+16)])
                        for event in xrange(len(valsEndCapA)):
                            valsEndCapA[event] = valsEndCapA[event]*(1-rebalance)
                            valsBarrelA[event] = valsBarrelA[event] + valsEndCapA[event]*rebalance
                        rebalanceNumbers['rebalance%s%d'%(VarName,reg)] = valsEndCapA
                        rebalanceNumbers['rebalance%s%d'%(VarName,reg+16)] = valsBarrelA
                    if endcapC:
                        valsEndCapC = copy.copy(out.numbers['%s%d'%(VarName,reg+48)])    
                        valsBarrelC = copy.copy(out.numbers['%s%d'%(VarName,reg+32)])                
                        for event in xrange(len(valsEndCapC)):
                            valsEndCapC[event] = valsEndCapC[event]*(1-rebalance)
                            valsBarrelC[event] = valsBarrelC[event] + valsEndCapC[event]*rebalance
                        rebalanceNumbers['rebalance%s%d'%(VarName,reg+48)] = valsEndCapC
                        rebalanceNumbers['rebalance%s%d'%(VarName,reg+32)] = valsBarrelC

            keys = rebalanceNumbers.keys()
            keys.sort()
            for k in keys :
                out.numbers[k] = rebalanceNumbers[k]
#                vals = rebalanceNumbers[k]
#                if doBoxplot :
#                    res = boxplot(vals)
#                    if not outfilepath :
#                        print k, "boxplot: (%f,%f,%f,%f,%f)" % res["box"]
#                        if showOutliers :
#                            print "lower:", res["extralow"]
#                            print "upper:", res["extraup"]
#                    else :
#                        print >>fout, k, "boxplot: (%f,%f,%f,%f,%f)" % res["box"]
#                        if showOutliers :
#                            print >>fout, "lower:", res["extralow"]
#                            print >>fout, "upper:", res["extraup"]
#                else : # standard levels
#                    res = statrange(vals)
#                    if doLatex and outfilepath:
#                        #print mean, std dev, 25%, 75%,95%,99%
#                        print >>fout, k, " & ", 
#                        print >>fout, " %5.0f & %5.2f & " %(res["mean"],res["sd"]),
#                        print >>fout, res["lvls"][1], " & ",res["lvls"][3]," & ",
#                        print >>fout, res["lvls"][4], " & ",res["lvls"][5] , " \\\\"
#                    elif not outfilepath :
#                        print k, "intervals:",
#                        for v in res["lvls"] :
#                            print v,
#                        print "mean:", res["mean"], "sd:", res["sd"],
#                        print "range: (%f,%f)" % res["range"]
#                    else :
#                        print >>fout, k, "intervals:",
#                        for v in res["lvls"] :
#                            print >>fout, v,
#                        print >>fout, "mean:", res["mean"], "sd:", res["sd"],
#                        print >>fout, "range: (%f,%f)" % res["range"]

    
    def do_print(self,text) :
        """If a "study" command ran this commands print the statistical information collected by the internal ntuples representing the hardware activity. By default, for each value collected by the "study" command, the value corresponding to different interval: 5%, 25%, 50%, 75%, 95%, 99%. Then also the mean and the standard deviations are showed.
        
The internal options are:
* boxplot: the intervals follow the boxplot method
* outliers: print the ntuples of the outliers and only the usual boxplot value
* save <filename>: the argument saves the numbers in an external file"""
        global ftkouts

        args = text.split()
        showOutliers = False
        showCompact = False
        outfilepath = None
        doBoxplot = False
        doOffice = False
        doLatex = prog_options.latextable
        
        if "outliers" in args :
            showOutliers = True
        if "compact" in args :
            showCompact = True
        if "save" in args :
            outfilepath = args[args.index("save")+1]
        if "boxplot" in args :
            # enable the box plot feature for the ranges
            doBoxplot = True

        if "office" in args :
            doOffice = True
        if "latex" in args :
            doLatex = True
            
        if outfilepath :
            fout = open(outfilepath,"w")
        for out in ftkouts : # loop over the outputs
            if not outfilepath :
                print "Summary:", out.outdir
            else :
                print >>fout, "Summary:", out.outdir

            # sort the keys for the printing
            keys = out.numbers.keys()
            keys.sort()
            for k in keys :
                vals = out.numbers[k]
                if doBoxplot :
                    res = boxplot(vals)
                    if not outfilepath :
                        print k, "boxplot: (%f,%f,%f,%f,%f)" % res["box"]
                        if showOutliers :
                            print "lower:", res["extralow"]
                            print "upper:", res["extraup"]
                    else :
                        print >>fout, k, "boxplot: (%f,%f,%f,%f,%f)" % res["box"]
                        if showOutliers :
                            print >>fout, "lower:", res["extralow"]
                            print >>fout, "upper:", res["extraup"]
                else : # standard levels
                    res = statrange(vals)
                    if doLatex and outfilepath:
                        #print mean, std dev, 25%, 75%,95%,99%
                        print >>fout, k, " & ", 
                        print >>fout, " %5.0f & %5.2f & " %(res["mean"],res["sd"]),
                        print >>fout, res["lvls"][1], " & ",res["lvls"][3]," & ",
                        print >>fout, res["lvls"][4], " & ",res["lvls"][5] , " \\\\"
                    elif not outfilepath :
                        print k, "intervals:",
                        for v in res["lvls"] :
                            print v,
                        print "mean:", res["mean"], "sd:", res["sd"],
                        print "range: (%f,%f)" % res["range"]
                    else :
                        print >>fout, k, "intervals:",
                        for v in res["lvls"] :
                            print >>fout, v,
                        print >>fout, "mean:", res["mean"], "sd:", res["sd"],
                        print >>fout, "range: (%f,%f)" % res["range"]
            
        if outfilepath :
            fout.close()
        return None
    
    def do_pyroot(self,text) :
        """This command opens a python shell that shares the ROOT module used by the main script, internal functions and objects as "ftkouts" that contains a list of the FTKTree. The shell can be used to spy the content of the internal stucture and directly use internal functions to extract numbers.

If additional text is provided, if the line ends with .py this is interpreted as script to run otherwise the line is interpreted inside the python interpreter."""
        global ftkouts

        locals = {"ROOT":ROOT,"ftkouts":ftkouts, "cmdline": self,
                  "statrange": statrange, "boxplot": boxplot}
        if len(text) > 0:
            interpreter = code.InteractiveInterpreter(locals)
            if text.endswith(".py") : 
                # execute a file
                extfile = open(text,"r")
                content = extfile.read()
                print content
                interpreter.runsource(content,text,"exec")
                extfile.close()
            else :
                # run the argument as pyhton code
                interpreter.runsource(text)
        else :
            code.interact("Internal python console. ctr-d to exit",raw_input,locals)
            print "Closing internal python console"
        return None
    
    def complete_set(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,self.settings.keys())
        return possible
    
    def do_set(self,text) :
        """Set the internal variables. With no arguments the list of internal variable with actual values is reported. Usage: set variable=value [variable=value ...]"""
        args = text.split()

        if len(args)==0 :
          for k, v in self.settings.iteritems() :
              print v.Message()            
          return None
      
        for arg in args :
          elems = arg.split("=")
          if len(elems)>2 :
            print "*** wrong number of elements:", arg
          elif len(elems)==2 :
            if elems[0] in self.settings :
              setitem = self.settings[elems[0]]
              print setitem
              print "Changing", elems[0], ":", setitem.Value(), "-->", elems[1]
              setitem.Set(elems[1])
            else :
                print "***", elems[0], "is not a valid key for setting"
          elif len(elems)==1 :
            if elems[0] in self.settings :
                print elems[0], "=", self.settings[elems[0]].Value()
            else :
                print "***", elems[0], "is not a valid key for setting"
        return None

    def complete_show(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["roads","tracks","tracksI","ss"])
        return possible

    def do_show(self,text) :
        """Show the status for an events. The supported grammars are:
> show <N>
This returns a general summary for all the outputs
> show
This return the summary for all the outputs in this event.

Possible arguments are:
* roads: show all the roads, with some details
* ss: show all the SS
* tracks: show all the tracks, with parameters, hitmaps and hits
* tracksI: show all the incomplete tracks"""
        global ftkouts

        # default additional flags
        showRoads = False
        showTracks = False
        showTracksI = False
        showSS = False
        
        args = text.split()
        if len(args)>0 :
            if args[0] == "next" or args[0] == "n" :
                # move to the next event
                self.ievt += 1
                self.do_entry(self.ievt)                                                
            else :
                self.do_entry(args[0])

            if "roads" in args :
                showRoads = True
            if "tracks" in args:
                showTracks = True
            if "tracksI" in args:
                showTracksI = True
            if "ss" in args :
                showSS = True
        if self.ievt == -1 :
            print "*** No event selected, select an event first"
            return None

        # print the status for each output, using the different parameters
        for out in ftkouts :
            if not out.isEnabled :
                continue
            out.PrintStatus(showRoads,showTracks,showTracksI,showSS)
        return None


    def do_status(self,text) :
        """Print the current status of the explorer"""
        print "Current event:", self.ievt

        print
        print "Internal variables:"
        for k, v in self.settings.iteritems() :
            print v.Message()
        return None

    def complete_study(self,text,line,begidx,endidx) :
        possible = self.arguments_complete(text,line,begidx,endidx,["roads","tracks","hits","nostore","all","default","single","ss","tfinfo"])
        return possible    

    def do_study(self,text) :
        """Collect statistics. This commands loop over all the events in the outputs and collect numbers or distributions often used to judge the FTK simulation results. This command produces ntuples that can be used by other commands to extract further information as average load or distributions.
The possible argumments are:
 * roads    : collect information on the roads
 * tracks   : collect information on the tracks
 * hits     : collect information of the hits, it enables "roads"
 * tfinfo   : collect the number of roads per sector, the number of fits per road
 * ss       : collect information of the ss
 * default  : enables roads, tracks, hits; it enables "roads"
 * all      : enables all the previous stydies
 * nostore  : this option disable the creationg of the ntuples, the histograms production.

Look plot or browser for more informations."""
        global ftkouts
        global prog_options

        args = text.split()

        countRoads = False
        countTracks = False
        countHits = False
        studySS = False
        infoTF = False
        keepStats = True

        regionLoop = False
        
        if len(args)>0 :
            if "roads" in args or "all" in args or "default" in args:
                countRoads = True
            if "tracks" in args or "all" in args or "default" in args:
                countTracks = True
            if "hits" in args or "all" in args or "default" in args:
                countHits = True
                countRoads = True # counting hits imply using roads
            if "ss" in args or "all" in args :
                studySS = True
                countRoads = True # counting roads is implied
            if "tfinfo" in args or "all" in args :
                infoTF = True
                countRoads = True # counting roads is implied
                countHits = True # attaching the hits to the road is necessary
            if "nostore" in args :
                keepStats = False
            if "single" in args :
                regionLoop = True
                
        doSome = countRoads or countTracks or countHits or studySS
        if not doSome :
            print "*** Nothing to count. You need to specify at least an argument"
            return None
        
        for out in ftkouts :
            if not out.isEnabled :
                print "Output:", out.id, "disable"
                continue
            if not out.connected :
                print "Output:", out.id, "not connected"
                continue

            print "Accumulating results for:", out.outdir

            try :
                maxevent = int(self.settings['maxEvent'].Value())
            except :
                maxevent = None

            if regionLoop :
                for ireg in xrange(out.nregions+1) :
                    out.Enable(False,ireg)
                    
                for ireg in xrange(out.nregions+1) :
                    
                    if out.Enable(True,ireg) == 0 :
                        continue # not enable, no active object for this region                    
                    res  = out.CollectStats(maxevent,countRoads,countTracks,countHits,studySS,keepStats)
                    out.Enable(False,ireg)
            else :
                res  = out.CollectStats(maxevent,countRoads,countTracks,countHits,studySS,keepStats)
            # clean some memory
            #out.DropBaskets()

            keys = res.keys()
            keys.sort()
            for k in keys :
                print k,"=", res[k]

        # produce base plots
        if not keepStats :
            return None

        # create the histogram file
        self.HistoFileCD()
        dfdir = ROOT.gDirectory.mkdir("dataflow")
        dfdir.cd()
        
        # set the home directory for all the histograms
        for out in ftkouts : # loop over the outputs
            # each output directory has a directory
            if out.outdir.endswith("/"):
                out.outdir = out.outdir[0:-1]

            dirname = os.path.basename(out.outdir)
            
            print "Produce histograms for:", dirname
            if self.HistoFile.FindKey(dirname) :
                self.HistoFile.cd(dirname)
            else :
                OutDir = dfdir.mkdir(dirname)
                out.RootDir = OutDir
                OutDir.cd()

            if prog_options.verbose>0 :
                print "Histogram summary:", dirname
            for k, vals in out.numbers.iteritems() :
                # check there are values
                if len(vals) == 0:
                    if prog_options.verbose>0 :
                        print "No values in", k
                    continue
                # try to do a smart binning                
                maxval = math.ceil(max(vals))+1
                minval = math.floor(min(vals))
                if maxval-minval<=100 :
                    nbins =  int(maxval-minval)
                else :
                    nbins = 100
                # create an histogram for any key
                if prog_options.verbose>0 :
                    print k, "Intervals: (%f,%f,%f,%f,%f)" % boxplot(vals)["box"], "binning (%f,%f,%f)" % (nbins,minval,maxval)
                histo = ROOT.TH1F("histo_"+k,k,nbins,minval,maxval) # automatic binning
                for v in vals :
                    histo.Fill(v)
                histo.Write()

        # end loop over the outputs
        dfdir.Write()        
        self.HistoFile.cd()
        
        print "Single event statistic collected. Check the results with print, present, plot, or browser."
            
        return None
    
    def do_summary(self,text) :
        """Show the summary of the FTK simulation results loaded. The summary print one line for each region and an additional line for the global merging.
For each region the message is like the following:
Region 0: RT -> -- -- -- RT -- -- -- -- -- -- -- -- -- -- -- --

The pair of symbols mean roads and tracks: R means there are roads at the specific level, T tracks, '-' nothing. The arguments are positional. The symbols before the arrow refers to merged tracks or roads, after the arrow summarize what was found for each sub region.

If lower case characters are showed the region/subregion has output but it was disabled."""
        global ftkouts
        for i, out in enumerate(ftkouts) :            
            out.PrintSummary()
        print text
        return None

    def do_time(self,text) :
        for out in ftkouts :
            if not out.isEnabled :
                continue
            out.ExecutionTime()
        return None
    
    def do_EOF(self,text) :
        print
        return self.do_exit(text)
    
    def do_exit(self,text) :
        """Exit"""
        if not text and False:
            ans = raw_input("Do you want do exit? (Y/n)> ");
        else :
            ans = text

        if not str or str == "y" or str=="Y" :
            return -1
        return -1

    def do_fakehits(self,text) :
        """
        This method create a file of fake hits, used to test hardware or different software versions.
        The method loops over the wrapper event, selecting the ones that generated some road and dump.
        Only the first output directory is used.
        """
        # set the reference output directory
        global ftkouts
        out = ftkouts[0]
        print "out is",out
        
        
        # prepare the object to loop on the events
        outFile = open(prog_options.filteredFile,"w")
        fchain = ftkutils.FTKFileChain(out.inputfiles, outFile=outFile)
        print "fchain is",fchain

        try :
            maxevent = int(self.settings['maxEvent'].Value())
            if maxevent>out.nEvents :
                maxevent = out.nEvents
        except :
            maxevent = out.nEvents

        eventcount  = 0
        eventswroad = 0

        eventcount_step = (maxevent+9)/10
        while fchain.nextevent() : # truth event loop
            if eventcount<int(self.settings["skipEvents"].Value()) :
                eventcount += 1
                continue
            if eventcount==maxevent :                    
                break

            eventcount+=1 # increment the event counter

            if eventcount%eventcount_step == 0 :
                print "Processed event # %d / %d" % (eventcount,maxevent)

            # synchronize the FTK output structure
            out.GetEntry(eventcount-1) # the event counter has a shift

            hasUMRoads = False
            # check if has unmerged roads
            for n in out.GetNUnmergedRoads() :
                if n>0 : 
                    hasUMRoads = True
                    break
 
            # check if has merged roads
            hasMRoads = False
            for n in out.GetNMergedRoads() :
                if n>0 :
                    hasMRoads = True
                    break

            # take the decision on the event
            if hasUMRoads or hasMRoads :
                #print "TODO", eventcount
                fchain.FTKEvent.WriteToFile(roads=out.GetUnmergedRoads())
                eventswroad += 1

        outFile.close()
        # Zip-it
        os.system("bzip2 "+prog_options.filteredFile)
        print eventswroad,"/",eventcount,float(eventswroad)/eventcount

    def emptyline(self) :
        return None
    
# start the main objects
if prog_options.restore :
    id = 0
    ftkouts = []
    expandedlist = []
    for cachename in prog_options.restore :
        if cachename.find("*")!=-1 : # explode
            listfiles = glob.glob(cachename)
            print "Expand:", cachename
            for item in listfiles :
                print "-->", item
            expandedlist += listfiles
        else :
            expandedlist += [cachename]
    prog_options.restore = expandedlist
    
    for cachename in prog_options.restore :
        print "Restoring old workspace in", cachename
        resfile = open(cachename,"r")
        tmpftkouts = pickle.load(resfile)
        # renumber the outputs
        for out in tmpftkouts :
            out.id = id
            id += 1
        ftkouts += tmpftkouts
        resfile.close()

    # the number of regions, sub-regions, and layers is set to the maximum    
    nregionsMax = ftkouts[0].nregions
    nsubregionsMax = ftkouts[0].nsubregions
    nlayersMax = ftkouts[0].nlayers

    if len(ftkouts)>1 :
        for out in ftkouts[1:] : # loop over the restored outputs
            if out.nregions > nregionsMax :
                numregionsMax = out.nregions
            if out.nsubregions > nsubregionsMax :
                numsubregionsMax = out.nsubregions
            if out.nlayers > nlayersMax :
                nlayersMax = out.nlayers
        # end loop over the restored outputs
    # set the right prog_options values
    print "Settinng # (regions,subregions,layers) = (%d,%d,%d)" % (nregionsMax,nsubregionsMax,nlayersMax)
    prog_options.numregions = nregionsMax
    prog_options.numsubregions = nsubregionsMax
    prog_options.nlayers = nlayersMax

    if prog_options.concatenate :
        ftkmerged = ftkouts[0]
        nels = -1
        for i, out in enumerate(ftkouts[1:]) : # loop over the restored outputs
            print "Concatinating %s..." % prog_options.restore[i]
            nels = ftkmerged.ConcatenateNumbers(out)
        ftkouts = [ftkmerged]
        print "Total number of elements:", nels
else :
    ftkouts = []
    print "Starting FTK Explorer:"


    if prog_options.indir == None:
        for i, argument in enumerate(prog_args) :
            try :
                out = cftkutils.FTKTree(i,argument,prog_options.numregions,prog_options.numsubregions,prog_options.nlayers)
                ftkouts.append(out)
                    
            except cftkutils.error, errmsg :
                print "Output directory %s skipped" % argument
                print "error:", errmsg
    else:
        if (prog_options.athena):
          dirlist = glob.glob(prog_options.indir+"/ntuples/*")
        else:
          dirlist = glob.glob(prog_options.indir+"/raw*")
        if len(dirlist) == 0:
            try:
                dirlist = glob.glob(prog_options.indir+"/*/raw*")
            except cftkutils.error, errmsg :
                print "Output directory %s and sub directories do not contain raw_ directories" % prog_options.indir
                print "error:", errmsg

        for i, argument in enumerate(dirlist) :
            try:
                out = cftkutils.FTKTree(i,argument,prog_options.numregions,prog_options.numsubregions,prog_options.nlayers)
                ftkouts.append(out)
                    
            except cftkutils.error, errmsg :
                print "Output directory %s skipped" % argument
                print "error:", errmsg

# load the list of wrapper files if required
if prog_options.wrapperfiles :
    if os.path.isdir(prog_options.wrapperfiles) :
        print "Take input from directory:", prog_options.wrapperfiles
        files_input = glob.glob(prog_options.wrapperfiles+"/*.dat*")
        files_input.sort()
    elif prog_options.wrapperfiles[-4:] == "list" :
        print "Take input from file:", prog_options.wrapperfiles        
        files_input = []
        flist = open(prog_options.wrapperfiles,"r")
        for line in flist :
            files_input.append(line.rstrip())
    ftkinputs = ftkutils.FTKFileChain(files_input)
    NormalizeOutputs(False) # the cross synchronization is not done automatically
else :
    ftkinputs = None


# connect the output to the files
if (prog_options.connect and not prog_options.restore) or (prog_options.restore and prog_options.reconnect):
    # connection happens in these two cases:
    #  1. normal read of output files, connection not disabled by the user
    #  2. restoring one or more sessions, connection has to be requested
    for out in ftkouts :
        out.Connect()
else:
    print "No connection attempted"

# check the number of output structure to explore
if len(ftkouts)==0 :
    print "*** No output to explore"

# start the interpreter
explorer = FTKExplorerCmd()
# set verbose level for the CLI
explorer.settings["verbose"].value = prog_options.verbose
explorer.prompt = "FTK> "

# if asked read the commands from an external file.
if prog_options.inputfile :
    try :
        inputfile = open(prog_options.inputfile)
        print "Inputs from:", prog_options.inputfile
        for line in inputfile :
            if line.startswith("#") :
                continue # skip the comments
            explorer.onecmd(line.strip())
    except IOError, errmsg :
        print "*** Error opening the input file:", prog_options.inputfile
        print errmsg
        sys.exit(-1)
    # After the execution of the script the program ends
else :
    explorer.cmdloop("FTK Interpreter version %s" % ftkexpl_version)

if explorer.settings["CacheFile"].Value() and (not prog_options.restore or prog_options.concatenate):
    cachename = explorer.settings["CacheFile"].Value()
    # disconnect the chain and branches
    for out in ftkouts :
        out.Disconnect()
        out.RootDir = None
    print "Saving workspace in:", cachename,
    cachefile = open(cachename,"w")
    pickle.dump(ftkouts,cachefile,2)
    print "Done"
    cachefile.close()
