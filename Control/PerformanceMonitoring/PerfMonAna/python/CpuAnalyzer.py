# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: CpuAnalyzer.py
# @purpose: a set of classes to analyze (CPU) data from a perfmon tuple
# @author: Sebastien Binet <binet@cern.ch>

""" A set of classes to analyze (CPU) data from a perfmon tuple
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.19 $"
__doc__     = "A set of classes to analyze (CPU) data from a perfmon tuple."

import logging
import numpy
import matplotlib.pyplot as plt
from .PyRootLib import importRoot
from .Analyzer  import Analyzer,bookAvgHist
from .Constants import Units

class CpuAnalyzer( Analyzer ):
    """analyzer working on CPU related quantities. It reads the perfmon tuple
    and extracts user/sys/real CPU timings for the whole Configuration step,
    the initialize and finalize steps (on a per-algorithm basis) and the
    execute step (on a per-event and per-algorithm basis).
    """

    def __init__(self, name):
        Analyzer.__init__(self, name, 'cpu')
        self.msg = logging.getLogger( "Cpu-%s" % name )
        self.msg.setLevel( logging.INFO )
        return

    def visit(self, monComp):
        if monComp.type not in ['alg','user']:
            self.msg.debug( " skipping %s [%s]",monComp.name,monComp.type )
            return False
        return True

    def bookHistos(self, monComp):
        ROOT = importRoot()
        #Analyzer.__bookHistos(self)
        from .PyRootLib import setupRootStyle
        setupRootStyle()

        from .App import DataSetMgr
        for dataSetName in monComp.data.keys():
            dataSet  = DataSetMgr.instances[dataSetName]
            nEntries = len(dataSet.bins)
            minEvt   = dataSet.bins[self.minEvt]
            maxEvt   = dataSet.bins[-1]

            ## print ":::",dataSetName,minEvt,maxEvt,nEntries
            histos   = monComp.data[dataSetName]['histos']

            monCompName = monComp.name.replace( "/", "#" )
            hId   = 'cpu_%s.%s' % (monCompName, dataSetName)
            hName = 'cpu_%s'    %  dataSetName
            histos[hName] = ROOT.TH1F( hId,
                                       "[%s] CPU usage [%s];%s;%s" % \
                                       ( monComp.name, dataSetName,
                                         "event nbr", "Timing [ms]" ),
                                       nEntries, minEvt, maxEvt )
        return

    def fillHistos(self, monComp):

        from .App import DataSetMgr
        self.msg.debug("filling histograms...")

        # short-hands
        msg = self.msg

        # milliseconds
        ms = Units.ms

        dsNames = DataSetMgr.names()
        yMin = []
        yMax = []
        allGood = True

        for dsName in dsNames:
            if dsName not in monComp.data:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            data = data['evt']
            if data is None:
                continue

            if 'cpu' not in data.dtype.names:
                allGood = False
                msg.debug('component [%s] has empty cpu/user infos for '
                          'dataset [%s]',
                          monComp.name,
                          dsName)
                continue
            cpu = data['cpu']['cpu']
            yMin.append(cpu[self.minEvt:,2].min() * ms)
            yMax.append(cpu[self.minEvt:,2].max() * ms)

        if not allGood:
            msg.debug("Something bad happened for component [%s] "
                      "during 'CpuAnalyzer.fillHistos' !!",
                      monComp.name)
            return

        if len(yMin) == 0 and len(yMax) == 0:
            msg.debug("Component [%s] has no 'evt' level data", monComp.name)
            return

        yMin = min(yMin)
        yMax = max(yMax)

        for dsName in dsNames:
            if dsName not in monComp:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            bins = DataSetMgr.instances[dsName].bins
            if 'evt/cpu' not in monComp.figs:
                monComp.figs['evt/cpu'] = plt.figure()
                monComp.figs['evt/cpu'].add_subplot(211).hold(True)
                monComp.figs['evt/cpu'].add_subplot(212).hold(True)
            fig = monComp.figs['evt/cpu']
            ax = fig.axes[0]
            cpu = data['evt']['cpu']
            cpu_c = cpu['cpu']

            binMax = len(cpu_c[self.minEvt:len(bins)])
            ax.plot(bins[self.minEvt:binMax],
                    cpu_c[self.minEvt:binMax,2]*ms,
                    linestyle = 'steps',
                    label = dsName)
            ax.grid(True)
            ax.set_title ( "CPU time [%s]" % monComp.name )
            ax.set_ylabel( 'CPU time [ms]' )
            ax.set_xlabel( 'Event number'  )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            h,b = numpy.histogram(
                cpu_c[self.minEvt:binMax,2]*ms,
                bins  = 20,
                range = ( yMin*0.90, yMax*1.10 )
                )
            ax = fig.axes[1]
            ax.plot( b[:-1], h, label=dsName, ls='steps' )
            ax.grid( True )
            ax.set_xlabel( 'CPU time [ms]' )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            h    = data['histos']['cpu_%s' % dsName]
            hAvg = bookAvgHist(h, cpu_c[:,2] * ms)
            data['histos'][h.GetName()] = hAvg
            maxRange = min(len(cpu_c), len(bins))

            for i in range(maxRange):
                cpuTime = cpu_c[i,2] * ms
                h.Fill( float(bins[i]), cpuTime )
                hAvg.Fill( cpuTime )

            pass # loop over datasets

        for ax in monComp.figs['evt/cpu'].axes:
            ax.legend( DataSetMgr.labels( dsNames ), loc='best' )

        self.msg.debug( "filling histograms... [DONE]" )
        return

    def fitHistos(self, monComp):
        import PerfMonAna.PyRootLib as prl
        ROOT = importRoot(batch=True)
        RootFct = ROOT.TF1
        dummyCanvas = ROOT.TCanvas( 'dummyFitCanvas' )

        histos = [ h for h in self.histos.values()
                   if hasattr(h, 'tag') and h.tag == 'summary' and \
                   not h.GetName().startswith("cfg.") ]

        for h in histos:
            x = h.GetXaxis()
            xMin = x.GetXmin()
            xMax = x.GetXmax()
            name = h.GetName()

            prl.Polynom( degree = 1 )
            fct = RootFct( 'fitFct_%s' % name, "pol1", xMin, xMax )
            ## Q: quiet
            ## R: Use the range specified in the function range
            ## O: Do not plot the result of the fit.
            h.Fit(fct, "QOR")
            nPars = fct.GetNpar()
            fitRes= "\t".join( "p[%i] = %12.3f ms " % \
                    ( i, fct.GetParameter(i)) for i in range(nPars)
            )
            msg = "[%s] fit results: %s \tchi2/NDF = %12.3f/%i" % \
                  ( name, fitRes, fct.GetChisquare(), fct.GetNDF() )
            self.msg.info( msg )
            pass

        del dummyCanvas
        return

    pass # class CpuAnalyzer
