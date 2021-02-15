# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: Analyzer.py
# @purpose: a set of classes to analyze data from a perfmon tuple
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import print_function

""" A set of classes to analyze data from a perfmon tuple
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.15 $"
__doc__     = "A set of classes to analyze data from a perfmon tuple."

import logging

from PerfMonAna.PyRootLib import importRoot

def getAnalyzer(monVarName, monName):
    Klass = NoopAnalyzer
    if monVarName == 'cpu':
        from .CpuAnalyzer import CpuAnalyzer
        Klass = CpuAnalyzer
    elif monVarName == 'mem':
        from .MemAnalyzer import MemAnalyzer
        Klass = MemAnalyzer
    elif monVarName == 'io':
        from .IoAnalyzer import IoAnalyzer
        Klass = IoAnalyzer
    elif (monVarName.count('.') > 0 or monVarName.count('/')>0):
        Klass = None
        monName = monVarName.split('/')

        klass_path = monName[0].split('.')
        m = __import__('.'.join(klass_path[:-1]), fromlist=klass_path[-1:])
        Klass = getattr(m, klass_path[-1])
        if len(monName)==2:
            monName = monName[1]
    else:
        raise RuntimeError('unknown analyzer [monVarName=%r, monName=%r]' %\
                           (monVarName, monName))
    return Klass( monName )

def bookAvgHist(hist, data):
    ROOT = importRoot( batch = True )
    hId   = "avg_%s" % hist.GetName()
    nBins = int(hist.GetNbinsX() / 2.)
    xMin  = data.min() * 0.95
    xMax  = data.max() * 1.05

    return ROOT.TH1F(
        hId,
        "%s;%s;%s" % ( hist.GetTitle(),
                       hist.GetYaxis().GetTitle(),
                       "" ),
        nBins, xMin, xMax
        )

def project(tree, id, varexp, selection="", opt="L ",
            nentries=1000000000,
            firstentry=0):
    """Pythonize the TTree::Project method
    """
    ROOT = importRoot()
    findObj = ROOT.gROOT.FindObject
    #DR FIXME
    if findObj(id):
        # effectively delete the histogram
        findObj(id).SetDirectory(None)
    n = tree.Project(id, varexp, selection, opt, nentries, firstentry)
    if n <= 0:
        raise ValueError("No entry satisfying [%s]"%varexp)
    h = findObj(id)
    return n,h

def mon_project(tree, monInfos, id, varexp, selection="", opt="",
                nentries=1000000000, firstentry=0):
    """Helper method to get both the evolution of a monitored variable (cpu,
    vmem, malloc,...) and its distribution.
    Returns the 2 histograms. hEvol and hDistrib
    """
    ROOT = importRoot()
    opt = "L "+opt
    n,h = project(tree, "avg_"+id, varexp, selection,
                  opt, nentries, firstentry)
    hDistrib = h
    hDistrib.SetTitle(monInfos[0])
    hDistrib.GetXaxis().SetTitle(monInfos[2])

    varexp = varexp+":evtNbr"
    n,h = project(tree, "_tmp", varexp, selection, opt, nentries, firstentry)
    v1,v2 = tree.GetV1(), tree.GetV2()
    binWide = 1
    hEvol = ROOT.TH1F(id, "%s;%s;%s" % (monInfos[0], monInfos[1], monInfos[2]),
                      n, v2[0]-binWide/2., v2[n-1]+binWide/2.)
    _fill = hEvol.Fill
    for i in range(n):
        _fill(v2[i], v1[i])
    return (hEvol,hDistrib)

def make_stack( canvas, pad_nbr, title, drawopt="nostack" ):
    ROOT = importRoot()
    stack_name = 'st_%s_pad_%i' % (canvas.GetName(), pad_nbr)
    stack = ROOT.gROOT.FindObject(stack_name)
    #DR if stack is None:
    if not stack:
        stack = ROOT.THStack(stack_name, title)
        setattr(stack, '_graphs', [])
    return stack

def make_canvas(name, title, items, shape=None):
    ROOT = importRoot()
    nItems = len(items)
    if shape is None:
        shape=(1,nItems)
    c = ROOT.gROOT.FindObject(name)
    #DR if c is None:
    if not c:
        drawOpt = ""
        c = ROOT.TCanvas(name, title)
        setattr(c, '_stacks', [make_stack(c,i,title) for i in range(nItems)])
        setattr(c, '_shape',  shape)
        def _plot(self):
            return
            #DR if self._shape is None: return
            if not self._shape :
                return
            for ipad in range(self._shape[0]*self._shape[1]):
                stack = self._stacks[ipad]
                print ("-->",ipad,self.GetName(),stack.GetName())
                stack.Draw("nostack")
                for gr in stack._graphs:
                    gr.Draw("SAME")
            return
        setattr(c, '_plot', _plot)
        if nItems>=1:
            c.Divide(shape[0], shape[1])
        elif nItems==0:
            return c
    else:
        drawOpt = " SAME"

    if nItems>=1:
        for i,o in enumerate(items):
            pad = c.cd(i+1)
            pad.SetGrid()
            drawOpt = o[1]+drawOpt
            o = o[0]
            if isinstance(o, ROOT.TGraph):
                o.GetHistogram().Draw(drawOpt)
                o.Draw(drawOpt)
            else:
                o.Draw(drawOpt)
    #c._plot(c)
    return c

class Analyzer(object):
    """
    The base object for analyzing data from a perfmon tuple
    """

    def __init__(self, name, typeName):
        object.__init__(self)
        self.msg      = logging.getLogger( "Analyzer" )
        self.msg.setLevel( logging.INFO )

        self.name     = name
        self.typeName = typeName

        self.nEntries = None
        self.minEvt   = None
        self.maxEvt   = None

        self.histos  = { }
        return

    def initialize(self, monComp):

        self.minEvt = 1
        self.maxEvt = self.nEntries
        self.ratio = 1. # = (1 / number of events per bin)

        self.bookHistos( monComp )
        return

    def run(self, monComp):

        if self.visit( monComp ):
            self.initialize( monComp )
            self.fillHistos( monComp )
            self.fitHistos ( monComp )
        return

    def fitHistos(self, monComp):
        return

##
class NoopAnalyzer(Analyzer):

    def __init__(self, name):
        Analyzer.__init__(self, name, 'cpu')
        self.msg = logging.getLogger( "Cpu-%s" % name )
        self.msg.setLevel( logging.INFO )
        return

    def run(self, monComp):
        return
    pass
