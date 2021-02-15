# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: App.py
# @purpose: a set of classes to post-process/analyze a (set of) perfmon tuples
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import print_function

""" A set of classes and utilities to post-process/analyze a (set of)
    perfmon tuples.
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.20 $"
__doc__     = "A set of classes and utilities to post-process/analyze a (set of) perfmon tuples."

### imports ---
import sys
import os
import logging
import numpy
import six

import tempfile
mplconfig_dir = tempfile.mkdtemp(prefix='matplotlib-%s-' % os.getpid())
os.environ['MPLCONFIGDIR'] = mplconfig_dir

import atexit
def del_mplconfig_dir():
    import os
    if os.system('/bin/rm -rf %s' % mplconfig_dir):
        print ("** could not remove temporary $MPLCONFIGDIR **")
    return
atexit.register(del_mplconfig_dir)
#print ("===>",os.environ['MPLCONFIGDIR'])

import matplotlib
if 'matplotlib.backends' not in sys.modules:
    matplotlib.use('pdf')
import matplotlib.pyplot as pyplot
_rc = pyplot.rcParams
_rc['legend.fontsize'] = 'medium'
_rc['axes.titlesize']  = 'medium'
_rc['xtick.labelsize'] = 'small'
_rc['ytick.labelsize'] = 'small'
_rc['font.size'] = 7.0
_rc['figure.dpi'] = 100
_rc['figure.subplot.bottom'] = 0.05
_rc['figure.subplot.hspace'] = 0.3
_rc['figure.subplot.right']  = 0.95
_rc['figure.subplot.top']    = 0.95
_rc['figure.subplot.wspace'] = 0.3

_pyplot_legend = pyplot.legend
def my_legend(*args, **kwargs):
    print ("====>")
    kwargs['alpha'] = 0.
    return _pyplot_legend(*args, **kwargs)
pyplot.legend = my_legend
import pylab
pylab.legend = my_legend
del my_legend

from .DataLoader import DataLoader

__do_monitoring = False
#__do_monitoring = True
def _monitor(tag):
    global __do_monitoring
    if __do_monitoring:
        from resource import RUSAGE_SELF,getrusage
        cpu = getrusage( RUSAGE_SELF )
        from time import time
        start = time() * 1e3
        print ("===>",tag)
        print ("cpu: u=",cpu.ru_utime * 1e3)
        print ("cpu: s=",cpu.ru_stime * 1e3)
        print ("time:    ",start)
    return

def _installLogger( lvl = logging.INFO ):
    import sys
    # define a Handler which writes DEBUG messages or higher to the sys.stdout
    logger = logging.StreamHandler(sys.stdout)
    logger.setLevel(logging.DEBUG)
    # set a format which is simpler for console use
    formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
    # tell the handler to use this format
    logger.setFormatter(formatter)
    # add the handler to the root logger
    logging.getLogger('').addHandler(logger)

    ## pre-defining some loggers with default logging-level
    log = logging.getLogger("AppMgr")
    log.setLevel( lvl )
    for i in range(10):
        log = logging.getLogger("AnaMgr-" + str(i).zfill(3))
        log.setLevel( lvl )
    log = logging.getLogger("Ana-chk")
    log.setLevel( logging.ERROR )
    log = logging.getLogger("Ana-ref")
    log.setLevel( logging.ERROR )
    log = logging.getLogger("Analyzer")
    log.setLevel( logging.ERROR )
    return

## install the logger at py-module import and clean-up
_installLogger()
del _installLogger

class ExitCodes:

    SUCCESS = 0
    ERROR   = 1

    UNKNOWN = 255
    pass

class AppMgr(object):
    """
    The main class to perform an analysis on perfmon data
    """

    def __init__(self, inputFileNames, outFileName="",
                 analyzers = ("cpu", "mem"),
                 dsLabels  = None,
                 fitSlice  = None):
        super(AppMgr,self).__init__()

        self.msg = logging.getLogger("AppMgr")
        self.msg.info( "Initializing the application manager..." )

        self.inputFiles = inputFileNames
        self.outputFile = os.path.splitext(outFileName)[0]
        if dsLabels is None:
            dsLabels = [ str(i).zfill(3) for i in range(len(inputFileNames)) ]
        elif len(dsLabels) < len(inputFileNames):
            dsLabels += [ str(i).zfill(3) for i in range(len(dsLabels),
                                                         len(inputFileNames)) ]
        self.anaMgrs = [ AnaMgr(idx, inFileName, dsLabels[idx]) \
                         for idx,inFileName in enumerate(inputFileNames) ]

        self.summary = None
        self.monComps = MonitoredComponent.instances
        self.msg.info( "found [%i] MonComponents", len(self.monComps) )
        filtered = [ m for m in MonitoredComponent.instances.values()
                     if m.name in ( 'TopAlg', 'Streams',
                                    'PerfMonSlice',
                                    'PerfMonSliceIo' ) or \
                     self.__filter(m) ]
        self.msg.info( "\tafter filtering: [%i]", len(filtered) )

        if fitSlice is None:
            self._fitSlice = '1:'
        elif ':' not in fitSlice:
            bins  = self.anaMgrs[0].bins
            nbins = len(bins[1:])
            try:
                fitSlice = float(fitSlice)
            except ValueError:
                raise
            if fitSlice <= 0. or fitSlice > 1.:
                raise ValueError (
                      "You have to give a fitSlice in (0.,1.] (got: %r)" %
                      fitSlice)
            # get the last x percent of the total range,
            _ratio = (1.- float(fitSlice))*nbins
            # convert into a suitable python slice object
            # To avoid jump in memory due to root flushing
            if nbins <= 95:
                self._fitSlice = "%i:" % int( _ratio )
            elif nbins > 95 and nbins < 115 :
                self._fitSlice = "%i:" % int( _ratio )
                self._fitSlice += "95"
            elif nbins > 120 :
                self._fitSlice = "105:"
        else:
            self._fitSlice = fitSlice
        self.msg.info( "fit slice: [%s]", self._fitSlice )
        self.analyzers = analyzers
        self.msg.info( "Scheduled analyzers: %r", self.analyzers )
        self.msg.info( "Initializing the application manager... [DONE]" )
        return

    def run(self):
        """
        main entry point to run the post-processing of a perfmon job
        """
        self.msg.info( "running app..." )
        ## check everybody has the same bins
        for i in range(len(self.anaMgrs)):
            ## print ("-->",self.anaMgrs[i].name,len(self.anaMgrs[i].bins))
            for j in range(i+1,len(self.anaMgrs)):
                if len(self.anaMgrs[i].bins) != len(self.anaMgrs[j].bins):
                    self.msg.warning( "Not running on same number of evts !" )
                    self.msg.warning( " [%s] : %r",
                                      self.anaMgrs[i].name,
                                      self.anaMgrs[i].bins )
                    self.msg.warning( " [%s] : %r",
                                      self.anaMgrs[j].name,
                                      self.anaMgrs[j].bins )

        self.msg.info( "nbr of datasets: %i", len(DataSetMgr.instances.keys()) )
        from . import Analyzer
        self.msg.info( "running analyzers..." )
        for monComp in self.monComps.values():
            self.msg.debug( " ==> [%s]... (%s)", monComp.name, monComp.type )
            monVars = self.analyzers
            if (monComp.name in [ 'TopAlg', 'Streams' ] or
                self.__filter(monComp)):
                for monVar in monVars:
                    analyzer = Analyzer.getAnalyzer(monVar, monComp.name)
                    analyzer.run(monComp)
                    pass
            else:
                self.msg.debug( " skipped [%s]", monComp.name )
            pass

        self.msg.info( "creating summary..." )
        ## create the summary
        from . import SummaryCreator
        self.summary = SummaryCreator.SummaryCreator( fitSlice= self._fitSlice )
        self.summary.process( DataSetMgr.instances,
                              MonitoredComponent.instances )

        self.msg.info( "creating output files..." )
        self.__writeRootFile()
        self.__writeAsciiFile()
        self.__writePdfFile()

        self.msg.info( "running app... [DONE]" )
        return ExitCodes.SUCCESS

    def __filter(self, monComp):
        """hook for the user to filter out some MonitoredComponent"""
        ## user filtering fct
        from .UserFct import FilterFct
        fct = FilterFct()
        return fct(monComp)

    def __writeRootFile(self):
        self.msg.debug( "create ROOT file..." )
        from .PyRootLib import importRoot
        ROOT = importRoot( batch = True )
        outName = self.outputFile+".root"
        outFile = ROOT.fopen( outName, 'RECREATE' )
        for dsName in DataSetMgr.names():
            outFile.cd( "/" )
            outFile.mkdir( dsName )
            outFile.cd( dsName )
            for m in MonitoredComponent.instances.values():
                if (not m.name.startswith('PerfMonSlice') and
                    not self.__filter(m)):
                    continue
                if dsName not in m.data:
                    continue

                for h in m.data[dsName]['histos'].values():
                    self.msg.debug( "writing %s/%s", dsName, h.GetName() )
                    h.Write()
        outFile.cd()
        if os.path.exists( outName ):
            self.msg.info( " --> (%10.3f kB) [%s]",
                           os.stat(outName).st_size / 1024.,
                           outName )
        self.msg.debug( "create ROOT file... [DONE]" )
        return

    def __writePdfFile(self):

        figs = []
        for k in [ 'ini', '1st', 'evt', 'fin', 'io' ]:
            if 'fig' in self.summary.sum[k]:
                f = self.summary.sum[k]['fig']
                if isinstance(f, list):
                    figs.extend(f)
                else:
                    figs += [ f ]
        jobSlice = MonitoredComponent.instances['PerfMonSlice']
        for k in [ 'cpu', 'mem', 'io' ]:
            fig = 'evt/%s' % k
            if fig in jobSlice.figs:
                figs.append( jobSlice.figs[fig] )
            pass
        algFigs = [ ]
        ioFigs  = [ ]
        for m in MonitoredComponent.instances.values():
            if not self.__filter(m):
                continue
            if   m.type in ('alg','algtool','svc'):
                algFigs += m.figs.values()
            elif m.type == 'io':
                ioFigs  += m.figs.values()
            else:
                continue

        figs += algFigs
        figs += ioFigs

        self.msg.debug( "create PDF..." )
        pdf = PdfMgr( 'perf' )
        pdf.save( self.outputFile+".pdf", figs, orientation='landscape' )

        outName = self.outputFile+".pdf"
        if os.path.exists( outName ):
            self.msg.info( " --> (%10.3f kB) [%s]",
                           os.stat(outName).st_size / 1024.,
                           outName )
        return

    def __writeAsciiFile(self):
        """Fill an ASCII with the summary data in a 'nice' format
        """
        outName = self.outputFile+".summary.txt"
        o = open( outName, 'w' )
        _txt = self.summary.txt
        print (":"*80, file=o)
        for c in ( 'slice', 'comps' ):
            for i in ( 'ini','1st','evt','fin'):
                print ("=== [%s - %s] ===" % (i,c), file=o)
                for j in ( 'mem', 'cpu', 'allocs', ):
                    for z in _txt[i][j][c]:
                        print (z, file=o)
            print (":"*80, file=o)
        print ("="*80, file=o)
        o.close()

        if os.path.exists( outName ):
            self.msg.info( " --> (%10.3f kB) [%s]",
                           os.stat(outName).st_size / 1024.,
                           outName )
        return

class AnaMgr(object):
    """
    The main class to analyze the content of a perfmon tuple
    """

    def __init__(self, name, inputFileName, dsLabel):
        object.__init__(self)

        if isinstance(name, str):
            self.name = name
        else:
            self.name = str(name).zfill(3)
        self.msg = logging.getLogger("AnaMgr-%s" % self.name)
        self.inFileName = inputFileName

        self.monComps = {}

        self._loadData( dsLabel )

        ## annotate the dataset
        DataSetMgr.instances[self.name].label = dsLabel
        return

    def _loadData(self, dsLabel):
        self.msg.info( " [%s]: %s", dsLabel, self.inFileName )
        self._dataLoader = DataLoader(self.inFileName)
        data = self._dataLoader.load()

        _monitor('3')
        storeNames = set()
        compNames  = set( list(data['meta']['components'  ].keys()) +
                          data['meta']['iocontainers']        )
        storeNames = [ k for k in six.iterkeys(data['data']) if k != 'meta' ]
        compNames  = [ c for c in compNames ]

        _monitor('4')
        dataSetName = self.name
        for compName in compNames:
            monComp = MonitoredComponent(compName, dataSetName)
            monData = monComp.data[dataSetName]
            for storeName in storeNames:
                try:
                    monData[storeName] = data['data'][storeName][compName]
                except KeyError:
                    monData[storeName] = None
                if storeName == 'io' and compName == 'PerfMonSlice':
                    monData[storeName] = data['data']['io']['PerfMonSliceIo']

            pass
        self.bins = numpy.arange(len(data['data']['evt']['PerfMonSlice']))
        _monitor('5')

        _compsDb = data['meta']['components'  ]
        _comps   = list(data['meta']['components'  ].keys())
        _ioconts = data['meta']['iocontainers']
        for monComp in MonitoredComponent.instances.values():
            if monComp.type is not None:
                continue
            monName = monComp.name
            if monName in _comps:
                monComp.type = _compsDb[monName]
                if isinstance(monComp.type,dict):
                    monComp.type   = _compsDb[monName]['type']
                    ## FIXME: not there yet...
##                     monComp.domain = domain(_compsDb[monName]['class'],
##                                             _compsDb[monName]['module'])
            elif monName in _ioconts:
                monComp.type = 'io'
            else:
                monComp.type = 'usr'
            pass

        _monitor('6')
        ## push the data into the according dataset
        dataSet = DataSetMgr(dataSetName, data)
        dataSet.bins = self.bins

        self.msg.debug( "Loading perfmon data... [OK]" )
        return

class MonitoredComponent(object):
    """
    An object modelling a (Gaudi) component which has been monitored with the
    PerfMon services and tools
    """
    instances = { } # all monitored components (by name)
    __slots__ = {
        'name' : None,
        'data' : None,
        'figs' : None,  # matplotlib figures
        #'res'  : { },  # results of the post-processing
        'type' : None }

    def __new__(cls, *p, **kw):
        if len(p) > 0:
            kw['name'] = p[0]
        if len(p) > 1:
            kw['dataSetName'] = p[1]
        if len(p) > 2:
            kw['data'] = p[2]

        # already created...
        if kw['name'] in cls.instances.keys():
            return cls.instances[kw['name']]

        # create instance
        obj = object.__new__(cls)
        object.__init__(obj)

        for k in cls.__slots__.keys():
            setattr(obj, k, cls.__slots__[k])

        # update repository of instances
        cls.instances[kw['name']] = obj

        return obj

    def __init__(self, name, dataSetName):

        object.__init__(self)
        self.name = name

        if not self.data:
            self.data = {}

        if not self.figs:
            self.figs = {}

        if dataSetName not in  self.data:
            self.data[dataSetName] = {}

        for k in ['histos']:
            if k not in self.data[dataSetName]:
                self.data[dataSetName][k] = {}

        return

    def monVars(self):
        """
        Return the list of monitored variables for this component
        ex: ['io','cpu','mem']
        """
        monKeys = [ ]
        for ds in self.data.values():
            for storeName,store in ds.items():
                monKeys += [ k.split("/")[0] for k in store.keys() ]
        return [ k for k in set(monKeys) ]

class DataSetMgr(object):
    """Borg-class (python-singleton) to hold the different 'dataset'
    """
    instances = { }
    __slots__ = {
        'name' : "",
        'data' : None,
        'label': None,
        'bins' : None,
        }

    def __new__(cls, *args, **kw):
        if len(args) > 0:
            kw['name' ] = args[0]
        if len(args) > 1:
            kw['data' ] = args[1]
        if len(args) > 2:
            kw['label'] = args[2]

        # already created ?
        if kw['name'] in cls.instances.keys():
            return cls.instances[kw['name']]

        # create new instance
        obj = object.__new__(cls)
        object.__init__(obj)

        for k in cls.__slots__.keys():
            setattr(obj, k, cls.__slots__[k])

        # update repository of instances
        cls.instances[kw['name']] = obj

        return obj


    @staticmethod
    def labels( keys = None ):
        if keys is None:
            keys = list(DataSetMgr.instances.keys())
            keys.sort()
        return [DataSetMgr.instances[k].label for k in keys]

    @staticmethod
    def names():
        keys = list(DataSetMgr.instances.keys())
        keys.sort()
        return keys

    @staticmethod
    def colorIter():
        import pylab
        # skip indigo...
        color = iter(list(pylab.cm.colors.cnames.keys())[1:])
        return color

    def __init__(self, name, data, label=None):

        object.__init__(self)
        self.name = name
        if not self.data:
            self.data = data
        if not self.bins:
            self.bins = []

        if not self.label:
            self.label = name

        if label is None:
            self.label = self.name

        return


class PdfMgr(object):
    """Borg-class (python-singleton) to hold different Pdf files, containing
    multiple figures
    """
    instances = { }
    __slots__ = {
        'name' : "",
        'figs' : None,
        }

    def __new__(cls, *args, **kw):
        if len(args) > 0:
            kw['name'] = args[0]
        if len(args) > 1:
            kw['figs'] = args[1]

        # already created ?
        if kw['name'] in cls.instances.keys():
            return cls.instances[kw['name']]

        # create new instance
        obj = object.__new__(cls)
        object.__init__(obj)

        for k in cls.__slots__.keys():
            setattr(obj, k, cls.__slots__[k])

        # update repository of instances
        cls.instances[kw['name']] = obj

        return obj


    def __init__(self, name, figs = None):

        object.__init__(self)
        self.name = name
        return

    def save(self, pdfFileName, figs, orientation='portrait'):

        from matplotlib.backends.backend_pdf import PdfPages

        import os
        _monitor('7')
        if os.path.exists( pdfFileName ):
            os.remove( pdfFileName )
        out = PdfPages(pdfFileName)
        for idx,fig in enumerate(figs):
            out.savefig(fig)
            ## closing canvas to recover some memory
            fig.clear()
            del fig
            figs[idx] = None

        _monitor('8')
        out.close()
        return

"""

def legend(*args, **kwargs):
    \"""
    Overwrites the pylab legend function.

    It adds another location identifier 'outer right'
    which locates the legend on the right side of the plot

    The args and kwargs are forwarded to the pylab legend function
    \"""
    if 'loc in kwargs:
        loc = kwargs['loc']
        loc = loc.split()

        if loc[0] == 'outer':
            # make a legend with out the location
            # remove the location setting from the kwargs
            kwargs.pop('loc')
            leg = pylab.legend(loc=(0,0), *args, **kwargs)
            frame = leg.get_frame()
            currentAxes = pylab.gca()
            currentAxesPos = currentAxes.get_position()
            # scale plot by the part which is taken by the legend
            plotScaling = frame.get_width()/currentAxesPos[2]

            if loc[1] == 'right':
                # scale the plot
                currentAxes.set_position((currentAxesPos[0],currentAxesPos[1],
                                          currentAxesPos[2] * (1-plotScaling),
                                          currentAxesPos[3]))
                # set x and y coordinates of legend
                leg._loc = (1 + leg.axespad, 1 - frame.get_height())

            # doesn't work
            #if loc[1] == 'left':
            #    # scale the plot
            #    currentAxes.set_position((currentAxesPos[0] +frame.get_width(),
            #                              currentAxesPos[1],
            #                              currentAxesPos[2] *(1-plotScaling),
            #                              currentAxesPos[3]))
            #    # set x and y coordinates of legend
            #    leg._loc = (1 -.05 -  leg.axespad - frame.get_width(), 1 -frame.get_height())

            pylab.draw_if_interactive()
            return leg

    return pylab.legend(*args, **kwargs)

"""
