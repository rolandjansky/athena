# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: MemAnalyzer.py
# @purpose: a set of classes to analyze (Mem) data from a perfmon tuple
# @author: Sebastien Binet <binet@cern.ch>

""" A set of classes to analyze (Mem) data from a perfmon tuple
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.21 $"
__doc__     = "A set of classes to analyze (Mem) data from a perfmon tuple."

import logging
import numpy
import matplotlib.pyplot as plt
from .PyRootLib import importRoot
from .Analyzer  import Analyzer, bookAvgHist
from .Constants import Units

class MemAnalyzer( Analyzer ):
    """analyzer working on memory related quantities. It reads the perfmon
    tuple and extracts virtual memory and resident set size memory consumptions
    for the whole Configuration step (at jobOption-include boundaries), the
    initialize, finalize and execute steps (on a per-algorithm and per-event
    basis).
    """

    typeName = 'mem'

    def __init__(self, name):
        Analyzer.__init__(self, name, 'mem')
        self.msg = logging.getLogger( "Mem-%s" % name )
        self.msg.setLevel( logging.INFO )
        return

    def visit(self, monComp):
        if monComp.type not in ['alg','user']:
            self.msg.debug( " skipping %s [%s]",monComp.name,monComp.type )
            return False

        return True

    def bookHistos(self, monComp):
        ROOT = importRoot()
        from .PyRootLib import setupRootStyle
        setupRootStyle()

        from .App import DataSetMgr
        for dataSetName in monComp.data.keys():
            dataSet  = DataSetMgr.instances[dataSetName]
            nEntries = len(dataSet.bins)
            minEvt   = dataSet.bins[self.minEvt]
            maxEvt   = dataSet.bins[-1]

            histos = monComp.data[dataSetName]['histos']

            monCompName = monComp.name.replace( "/", "#" )
            hId   = "mem_%s.%s" % (monCompName, dataSetName)
            hName = "mem_%s"    % dataSetName
            histos[hName] = ROOT.TH1F(
                hId,
                "[%s] Mem usage [%s];%s;%s" % (
                    monComp.name, dataSetName,
                    "event nbr", "V-mem [kB]"
                    ),
                nEntries, minEvt, maxEvt
                )

            hId   = "malloc_%s.%s" % (monCompName, dataSetName)
            hName = "malloc_%s"    % dataSetName
            histos[hName] = ROOT.TH1F(
                hId,
                "[%s] Malloc usage [%s];%s;%s" % (
                    monComp.name, dataSetName,
                    "event nbr", "Malloc'ed mem [kB]"
                    ),
                nEntries, minEvt, maxEvt
                )
        return

    def fillHistos(self, monComp):

        from .App import DataSetMgr
        self.msg.debug("filling histograms...")

        # short-hands
        msg = self.msg

        # convert page-size into kB
        kb =  Units.kb

        dsNames = DataSetMgr.names()
        ymin = {'vmem':[], 'malloc':[]}
        ymax = {'vmem':[], 'malloc':[]}
        allGood = True

        for dsName in dsNames:
            if dsName not in monComp.data:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            data = data['evt']
            if data is None:
                ## print "---",dsName,monComp.name,"data['evt'] is None"
                continue

            if 'mem' not in data.dtype.names:
                allGood = False
                msg.debug('component [%s] has empty mem infos for '
                          'dataset [%s]',
                          monComp.name,
                          dsName)
                continue

            mem = data['mem']
            dvmem = mem['vmem'][:,2]
            ymin['vmem'].append(dvmem[self.minEvt:].min()*kb)
            ymax['vmem'].append(dvmem[self.minEvt:].max()*kb)

            dmall = mem['mall'][:,2]
            ymin['malloc'].append(dmall[self.minEvt:].min()*kb)
            ymax['malloc'].append(dmall[self.minEvt:].max()*kb)
            pass

        if not allGood:
            msg.debug("Something bad happened for component [%s] "
                      "during 'MemAnalyzer.fillHistos' !!",
                      monComp.name)
            return

        if len(ymin['vmem']) == 0 and len(ymax['vmem']) == 0:
            self.msg.debug("Component [%s] has no 'evt' level data",
                           monComp.name)
            return

        for k in ymin.keys():
            ymin[k] = min(ymin[k])
            ymax[k] = max(ymax[k])
            pass


        for dsName in dsNames:
            if dsName not in monComp.data:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            data = data['evt']
            if data is None:
                continue

            if 'mem' not in data.dtype.names:
                continue

            bins = DataSetMgr.instances[dsName].bins
            if 'evt/mem' not in monComp.figs:
                monComp.figs['evt/mem'] = plt.figure()
                monComp.figs['evt/mem'].add_subplot(221).hold( True )
                monComp.figs['evt/mem'].add_subplot(222).hold( True )
                monComp.figs['evt/mem'].add_subplot(223).hold( True )
                monComp.figs['evt/mem'].add_subplot(224).hold( True )

            fig = monComp.figs['evt/mem']
            mem = data['mem']
            dvmem = mem['vmem'][:,2]
            dmall = mem['mall'][:,2]

            ## VMem
            ax = fig.axes[0]
            binMax = len(dvmem[self.minEvt:len(bins)])
            ax.plot(bins[self.minEvt:binMax],
                    dvmem[self.minEvt:binMax] * kb,
                    linestyle = 'steps',
                    label     = dsName)
            ax.grid(True)
            ax.set_title ('Delta V-Mem\n[%s]' % monComp.name)
            ax.set_ylabel('Delta V-Mem [kb]')
            ax.set_xlabel('Event number')
            ax.set_ylim((ax.get_ylim()[0]*0.9,
                         ax.get_ylim()[1]*1.1))

            h,b = numpy.histogram(
                dvmem[self.minEvt:binMax] * kb,
                bins  = 20,
                range = (ymin['vmem']*0.90, ymax['vmem']*1.10)
                )

            ax = fig.axes[2]
            ax.plot(b[:-1], h, label=dsName, ls='steps')
            ax.grid(True)
            ax.set_xlabel('Delta V-Mem [kb]')
            ax.set_ylim((ax.get_ylim()[0]*0.9, ### ???
                         ax.get_ylim()[1]*1.1))

            ## Malloc
            ax = fig.axes[1]
            ax.plot(bins[self.minEvt:binMax],
                    dmall[self.minEvt:binMax] * kb,
                    linestyle = 'steps',
                    label     = dsName)
            ax.grid(True)
            ax.set_title ('Delta Malloc\n[%s]' % monComp.name)
            ax.set_ylabel('Delta Malloc [kb]')
            ax.set_xlabel('Event number')
            ax.set_ylim((ax.get_ylim()[0]*0.9,
                         ax.get_ylim()[1]*1.1))

            h,b = numpy.histogram(
                dmall[self.minEvt:binMax] * kb,
                bins  = 20,
                range = (ymin['malloc']*0.90, ymax['malloc']*1.10)
                )

            ax = fig.axes[3]
            ax.plot(b[:-1], h, label=dsName, ls='steps')
            ax.grid(True)
            ax.set_xlabel('Delta Malloc [kb]')
            ax.set_ylim( (ax.get_ylim()[0]*0.9,
                          ax.get_ylim()[1]*1.1) )

            ## ROOT-histos
            data = monComp.data[dsName]
            hVmem    = data['histos']['mem_%s' % dsName]
            hAvgVmem = bookAvgHist(hVmem, dvmem * kb)
            data['histos'][hVmem.GetName()] = hAvgVmem
            hMall    = data['histos']['malloc_%s' % dsName]
            hAvgMall = bookAvgHist(hMall, dmall * kb)
            data['histos'][hMall.GetName()] = hAvgMall
            maxRange = min(len(dvmem), len(bins))

            for i in range(maxRange):
                dVmem = dvmem[i] * kb
                hVmem.Fill( float(bins[i]), dVmem )
                hAvgVmem.Fill( dVmem )
                dMall = dmall[i] * kb
                hMall.Fill( float(bins[i]), dMall )
                hAvgMall.Fill( dVmem )
            pass # loop over datasets

        for ax in monComp.figs['evt/mem'].axes:
            ax.legend(DataSetMgr.labels(), loc='best')

        self.msg.debug( "filling histograms... [DONE]" )
        return

    def fitHistos(self, monComp):
        # convert page-size into kB
        kb = Units.kb

        import PerfMonAna.PyRootLib as prl
        ROOT = importRoot()
        RootFct = ROOT.TF1
        dummyCanvas = ROOT.TCanvas( 'dummyFitCanvas' )

        histos = [ h for h in self.histos.values()
                   if hasattr(h, 'tag') and h.tag == 'summary' and \
                      not h.GetName().startswith("cfg.")
        ]

        for h in histos:
            x = h.GetXaxis()
            xMin = x.GetXmin()
            xMax = x.GetXmax()
            name = h.GetName()

            # FIXME: do we want to fit only on half the interval ?
            #        could make sense for mem-leak...
            xMin = xMin + ( xMax-xMin) / 2.

            prl.Polynom( degree = 1 )
            fct = RootFct( 'fitFct_%s' % name, "pol1", xMin, xMax )
            ## Q: quiet
            ## R: Use the range specified in the function range
            ## O: Do not plot the result of the fit.
            h.Fit(fct, "QOR")
            nPars = fct.GetNpar()
            fitRes= "\t".join( "p[%i] = %12.3f kB " % \
                    ( i, fct.GetParameter(i)*kb) for i in range(nPars)
            )
            msg = "[%s] fit results: %s \tchi2/NDF = %12.3f/%i" % \
                  ( name, fitRes, fct.GetChisquare(), fct.GetNDF() )
            self.msg.info( msg )
            pass
        del dummyCanvas

        return

    pass # class MemAnalyzer
