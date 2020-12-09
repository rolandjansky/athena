# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: IoAnalyzer.py
# @purpose: a set of classes to analyze (I/O) data from a perfmon tuple
# @author: Sebastien Binet <binet@cern.ch>

""" A set of classes to analyze (I/O) data from a perfmon tuple
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.8 $"
__doc__     = "A set of classes to analyze (I/O) data from a perfmon tuple."

import logging
import pylab,numpy
from PyRootLib import importRoot
from Analyzer  import Analyzer, bookAvgHist
from Constants import Units

class IoAnalyzer( Analyzer ):
    """analyzer working on I/O related quantities. It reads the perfmon tuple
    and extracts user+sys CPU timings for each of the 'registered' T/P
    converters (for T->P and P->T transformations) as well as the overall
    @c commitOutput timing (from the @c AthenaPoolCnvSvc class) which represent
    the user CPU time needed to commit transient data on disk.
    """
    typeName = 'io'

    def __init__(self, name):
        Analyzer.__init__(self, name, 'io')
        self.msg = logging.getLogger( "Io-%s" % name )
        self.msg.setLevel( logging.INFO )
        return

    def visit(self, monComp):
        if monComp.type not in ['io']:#'cfg']:
            self.msg.debug( " skipping %s [%s]",monComp.name,monComp.type )
            return False
        return True

    def bookHistos(self, monComp):
        ROOT = importRoot()
        #Analyzer.__bookHistos(self)

        #meta = self.meta.cfg.io

        from App import DataSetMgr
        for dataSetName in DataSetMgr.names():
            dataSet  = DataSetMgr.instances[dataSetName]
            nEntries = len(dataSet.bins)
            minEvt   = dataSet.bins[self.minEvt]
            maxEvt   = dataSet.bins[-1]

            histos = monComp.data[dataSetName]['histos']

            hId   = 'io_w_%s.%s' % (monComp.name, dataSetName)
            hName = 'io_w_%s'    %  dataSetName
            histos[hName] = ROOT.TH1F( hId,
                                       "[%s] I/O usage [%s] (W);%s;%s" % \
                                       ( monComp.name,
                                         monComp.name.replace("#","/"),
                                         "event nbr", "Timing [ms]" ),
                                       nEntries, minEvt, maxEvt )

            hId   = 'io_r_%s.%s' % (monComp.name, dataSetName)
            hName = 'io_r_%s'    %  dataSetName
            histos[hName] = ROOT.TH1F( hId,
                                       "[%s] I/O usage [%s] (R);%s;%s" % \
                                       ( monComp.name,
                                         monComp.name.replace("#","/"),
                                         "event nbr", "Timing [ms]" ),
                                       nEntries, minEvt, maxEvt )

            hId   = 'io_rr_%s.%s' % (monComp.name, dataSetName)
            hName = 'io_rr_%s'    %  dataSetName
            histos[hName] = ROOT.TH1F( hId,
                                       "[%s] I/O usage [%s] (RR);%s;%s" % \
                                       ( monComp.name,
                                         monComp.name.replace("#","/"),
                                         "event nbr", "Timing [ms]" ),
                                       nEntries, minEvt, maxEvt )

        return

    def fillHistos(self, monComp):

        from App import DataSetMgr
        self.msg.debug("filling histograms...")

        # short-hands

        # milliseconds
        ms = Units.ms

        dsNames = DataSetMgr.names()
        yMinRead  = []
        yMaxRead  = []
        yMinWrite = []
        yMaxWrite = []
        for dsName in dsNames:
            if dsName not in monComp.data:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            data['evt']['io/cpu/rr'] = data['evt']['io/user/rr'] \
                                     + data['evt']['io/sys/rr' ]
            data['evt']['io/cpu/r'] = data['evt']['io/user/r'] \
                                    + data['evt']['io/sys/r' ]
            yMinRead += [ data['evt']['io/cpu/r'][self.minEvt:].min() * ms ]
            yMaxRead += [ data['evt']['io/cpu/r'][self.minEvt:].max() * ms ]

            data['evt']['io/cpu/w'] = data['evt']['io/user/w'] \
                                    + data['evt']['io/sys/w' ]
            yMinWrite += [ data['evt']['io/cpu/w'][self.minEvt:].min() * ms ]
            yMaxWrite += [ data['evt']['io/cpu/w'][self.minEvt:].max() * ms ]
        yMinRead = min(yMinRead)
        yMaxRead = max(yMaxRead)
        yMinWrite = min(yMinWrite)
        yMaxWrite = max(yMaxWrite)

        for dsName in dsNames:
            if dsName not in monComp.data:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            bins = DataSetMgr.instances[dsName].bins
            if 'evt/io' not in monComp.figs:
                monComp.figs['evt/io'] = pylab.figure()
                monComp.figs['evt/io'].add_subplot(221).hold( True )
                monComp.figs['evt/io'].add_subplot(223).hold( True )
                monComp.figs['evt/io'].add_subplot(222).hold( True )
                monComp.figs['evt/io'].add_subplot(224).hold( True )

            ## read
            fig = monComp.figs['evt/io']
            ax = fig.axes[0]
            _iy = self.minEvt + len(bins[self.minEvt:])
            ax.plot( bins[self.minEvt:],
                     data['evt']['io/cpu/r'][self.minEvt:_iy] * ms,
                     linestyle = 'steps',
                     label = dsName )

            ax.grid( True )
            ax.set_title ( "[%s]" % monComp.name )
            ax.set_ylabel( '(R) CPU time [ms]' )
            ax.set_xlabel( 'Event number'  )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            h,b = numpy.histogram(
                data['evt']['io/cpu/r'][self.minEvt:] * ms +
                data['evt']['io/cpu/r'][self.minEvt:] * ms,
                bins  = 20,
                range = ( yMinRead*0.90, yMaxRead*1.10 )
                )
            ax = fig.axes[1]
            ax.plot( b, h, label=dsName, ls='steps' )
            ax.grid( True )
            ax.set_xlabel( '(R) CPU time [ms]' )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            ## write
            fig = monComp.figs['evt/io']
            ax = fig.axes[2]
            _iy = self.minEvt + len(bins[self.minEvt:])
            ax.plot( bins[self.minEvt:],
                     data['evt']['io/cpu/w'][self.minEvt:_iy] * ms,
                     linestyle = 'steps',
                     label = dsName )
            ax.grid( True )
            ax.set_title ( "[%s]" % monComp.name )
            ax.set_ylabel( '(W) CPU time [ms]' )
            ax.set_xlabel( 'Event number'  )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            h,b = numpy.histogram(
                data['evt']['io/cpu/w'][self.minEvt:] * ms +
                data['evt']['io/cpu/w'][self.minEvt:] * ms,
                bins  = 20,
                range = ( yMinWrite*0.90, yMaxWrite*1.10 )
                )
            ax = fig.axes[3]
            ax.plot( b, h, label=dsName, ls='steps' )
            ax.grid( True )
            ax.set_xlabel( '(W) CPU time [ms]' )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            ## read
            h    = data['histos']['io_r_%s' % dsName]
            hAvg = bookAvgHist(h, data['evt']['io/cpu/r'] * ms)
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                cpuTime = data['evt']['io/cpu/r'][i] * ms
                h.Fill( float(bins[i]), cpuTime )
                hAvg.Fill( cpuTime )

            ## write
            h    = data['histos']['io_w_%s' % dsName]
            hAvg = bookAvgHist(h, data['evt']['io/cpu/w'] * ms)
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                cpuTime = data['evt']['io/cpu/w'][i] * ms
                h.Fill( float(bins[i]), cpuTime )
                hAvg.Fill( cpuTime )

            pass # loop over datasets

        for ax in monComp.figs['evt/io'].axes:
            ax.legend( DataSetMgr.labels(), loc='best' )

        ################################################
        # ROOT T/P separation overhead
        #
        for dsName in dsNames:
            if dsName not in monComp.data:
                continue
            data = monComp.data[dsName]
            if 'evt' not in data:
                continue
            bins = DataSetMgr.instances[dsName].bins
            if 'evt/rio' not in monComp.figs:
                monComp.figs['evt/rio'] = pylab.figure()
                monComp.figs['evt/rio'].add_subplot(221).hold( True )
                monComp.figs['evt/rio'].add_subplot(223).hold( True )
                monComp.figs['evt/rio'].add_subplot(222).hold( True )
                monComp.figs['evt/rio'].add_subplot(224).hold( True )

            ## read
            fig = monComp.figs['evt/rio']
            ax = fig.axes[0]
            _iy = self.minEvt + len(bins[self.minEvt:])
            ax.plot( bins[self.minEvt:],
                     data['evt']['io/cpu/rr'][self.minEvt:_iy] * ms,
                     linestyle = 'steps',
                     label = dsName )
            ax.set_title ( "[%s]" % monComp.name )
            ax.set_ylabel( '(RR) CPU time [ms]' )
            ax.set_xlabel( 'Event number'  )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            h,b = numpy.histogram(
                data['evt']['io/cpu/rr'][self.minEvt:] * ms +
                data['evt']['io/cpu/rr'][self.minEvt:] * ms,
                bins  = 20,
                range = ( yMinRead*0.90, yMaxRead*1.10 )
                )
            ax = fig.axes[1]
            ax.plot( b, h, label=dsName, ls='steps' )
            ax.set_xlabel( '(RR) CPU time [ms]' )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            _iy = self.minEvt + len(bins[self.minEvt:])
            ratios = []
            for idx,num in enumerate(data['evt']['io/cpu/rr'][self.minEvt:_iy]):
                den = data['evt']['io/cpu/r'][idx]
                if den == 0.:
                    r = 0
                else:
                    r = num/den*100.
                ratios.append(r)

            ratios = numpy.array(ratios)
            yMinRatio = min(ratios)
            yMaxRatio = max(ratios)

            ## pure ROOT read over T/P read
            fig = monComp.figs['evt/rio']
            ax = fig.axes[2]
            ax.plot( bins[self.minEvt:],
                     ratios,
                     linestyle = 'steps',
                     label = dsName )
            ax.set_title ( "[%s]" % monComp.name )
            ax.set_ylabel( 'Pure-ROOT over Full read CPU time (%)' )
            ax.set_xlabel( 'Event number'  )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            h,b = numpy.histogram(
                ratios + ratios,
                bins  = 20,
                range = ( yMinRatio*0.90, yMaxRatio*1.10 )
                )
            ax = fig.axes[3]
            ax.plot( b, h, label=dsName, ls='steps' )
            ax.set_xlabel( 'Pure-ROOT over Full read CPU time (%)' )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )

            ## read
            h    = data['histos']['io_rr_%s' % dsName]
            hAvg = bookAvgHist(h, data['evt']['io/cpu/rr'] * ms)
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                cpuTime = data['evt']['io/cpu/rr'][i] * ms
                h.Fill( float(bins[i]), cpuTime )
                hAvg.Fill( cpuTime )

            pass # loop over datasets

        for ax in monComp.figs['evt/io'].axes:
            ax.legend( DataSetMgr.labels(), loc='best' )

        self.msg.debug("filling histograms... [DONE]")
        return

    pass # class IoAnalyzer
