# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: SummaryCreator.py
# @purpose: a set of classes to create a summary from a perfmon tuple
# @author: Sebastien Binet <binet@cern.ch>

""" a set of classes to create a summary from a perfmon tuple
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.20 $"
__doc__     = "a set of classes to create a summary from a perfmon tuple"

import logging
import numpy
import numpy.lib.polynomial
import matplotlib.pyplot as plt
from .Analyzer  import bookAvgHist,project
from .Constants import Units

def array_mean(a):
    n = float(len(a))
    return sum(a)/n

class SummaryCreator(object):

    def __init__(self, fitSlice):
        object.__init__(self)
        self.msg = logging.getLogger( "Summary" )
        self.msg.setLevel(logging.INFO)
        self.sum = {
            'ini' : {},
            '1st' : {},
            'evt' : {},
            'fin' : {},
            'io'  : {},
            }
        self._fitSlice = []
        for i in fitSlice.split(":"):
            try:
                self._fitSlice += [ int(i) ]
            except ValueError:
                self._fitSlice += [ None   ]
        # text format of the summary
        self.txt = {}
        for i in ( 'ini','1st','evt','fin'):
            self.txt[i] = {}
            for j in ( 'mem', 'cpu', 'allocs' ):
                self.txt[i][j] = {
                    'slice' : [],
                    'comps' : [],
                    }

        # maximal number of top-XXX consumers to display
        self.max = 20

        self.minEvt =  1
        self.maxEvt = -1
        return

    def process(self, dataSetMgr, monCompMgr):

        from .PyRootLib import importRoot
        ROOT = importRoot(batch=True)
        from .PyRootLib import setupRootStyle
        setupRootStyle()
        c = ROOT.TCanvas('c_default')

        for m in [ self.processIni,
                   self.processFirstEvt,
                   self.processEvt,
                   self.processFin,
                   #self.processIo, ## FIXME
                   ]:
            m(dataSetMgr, monCompMgr)
        del c
        return

    def processIni(self, dataSetMgr, monCompMgr):

        self._top_consumers( dataSetMgr, monCompMgr,
                             compTypes = ('alg','algtool','svc'),
                             title = 'initialization',
                             storeName = 'ini',
                             sliceName = 'ini',
                             begSlice  = 0,
                             endSlice  = 1 )
        return

    def processFirstEvt(self, dataSetMgr, monCompMgr):

        ## get top-20 consumers
        self._top_consumers( dataSetMgr, monCompMgr, compTypes = ('alg',),
                             title = '1st event',
                             storeName = 'evt',
                             sliceName = '1st',
                             begSlice  = 0,
                             endSlice  = 1 )
        return

    def processEvt(self, dataSetMgr, monCompMgr):
        from .App import DataSetMgr

        from .PyRootLib import importRoot
        ROOT = importRoot(batch=True)

        ## RootFct = ROOT.TF1
        ## dummyCanvas = ROOT.TCanvas("dummyFitCanvas")
        ## import PyRootLib as prl

        ## self.sum['evt']['histos'] = []
        ## self.sum['evt']['fig']    = []

        ## short-hand
        ms = Units.ms
        kb = Units.kb
        Mb = Units.Mb

        ## get top-20 consumers
        dsNames = DataSetMgr.names()

        _txt = self.txt['evt']

        monComp = monCompMgr['PerfMonSlice']
        yMinCpu = []
        yMaxCpu = []
        yMinIo  = []
        yMaxIo  = []
        for dsName in dsNames:
            data = monComp.data[dsName]
            histos = data['histos'] = {}
            if 'evt' not in data:
                continue
            data = data['evt']
            if data is None:
                continue
            cpu = data['cpu']
            cpu_u = cpu['user']
            cpu_s = cpu['sys']
            cpu_r = cpu['real']
            cpu_c = cpu['cpu']

            dcpu_u= cpu_u[:,2]
            dcpu_s= cpu_s[:,2]
            dcpu_r= cpu_r[:,2]
            dcpu_c= cpu_c[:,2]

            mem   = data['mem']
            vmem  = mem['vmem']
            dvmem = vmem[:,2]
            rss   = mem['rss']
            drss  = rss[:,2]
            mall  = mem['mall']
            dmall = mall[:,2]

            nallocs  = mem['nmall'][:,2]
            nfrees   = mem['nfree'][:,2]

            yMinCpu.append(dcpu_c[self.minEvt:].min() * ms)
            yMaxCpu.append(dcpu_c[self.minEvt:].max() * ms)

            io = monComp.data[dsName]['io']
            io_c = io['w']['cpu']
            yMinIo.append(io_c[self.minEvt:].min() * ms)
            yMaxIo.append(io_c[self.minEvt:].max() * ms)

            ## fill-in some data for ASCII summary
            if dsName == '000':
                _txt['cpu']['slice'] += [
                    "%-20s %10.3f %10.3f %10.3f ms" % (
                        "[u/s/r]",
                        dcpu_u[self.minEvt:].mean() * ms,
                        dcpu_s[self.minEvt:].mean() * ms,
                        dcpu_r[self.minEvt:].mean() * ms,
                        ),
                    ]
                _txt['mem']['slice'] += [
                    "%-20s %10.3f %10.3f %10.3f kB" % (
                        "[dVmem/dRss/dMalloc]",
                        dvmem[self.minEvt:].mean() * kb,
                        drss [self.minEvt:].mean() * kb,
                        dmall[self.minEvt:].mean() * kb,
                        ),
                    ]
                _txt['allocs']['slice'] += [
                    "%-20s %10i %10i" % (
                        "[nallocs/nfrees]",
                        nallocs[self.minEvt:].mean(),
                        nfrees[self.minEvt:].mean(),
                        ),
                    ]

            ## book ROOT histos
            nEntries = len(dataSetMgr[dsName].bins)-1
            minEvt   = dataSetMgr[dsName].bins[self.minEvt]
            maxEvt   = dataSetMgr[dsName].bins[-1]

            hId   = 'cpu_%s.%s' % (monComp.name, dsName)
            hName = 'cpu_%s'    % dsName
            histos[hName] = ROOT.TH1F(
                hId,
                "[%s] CPU usage [%s];%s;%s" % (
                    monComp.name, dsName,
                    "event nbr", "Timing [ms]"
                    ),
                nEntries, minEvt, maxEvt
                )

            hId   = 'vmem_%s.%s' % (monComp.name, dsName)
            hName = 'vmem_%s'    % dsName
            histos[hName] = ROOT.TH1F(
                hId,
                "[%s] VMem usage [%s];%s;%s" % (
                    monComp.name, dsName,
                    "event nbr", "V-Mem [MB]"
                    ),
                nEntries, minEvt, maxEvt
                )

            hId   = 'rss_%s.%s' % (monComp.name, dsName)
            hName = 'rss_%s'    % dsName
            histos[hName] = ROOT.TH1F(
                hId,
                "[%s] RSS usage [%s];%s;%s" % (
                    monComp.name, dsName,
                    "event nbr", "RSS [MB]"
                    ),
                nEntries, minEvt, maxEvt
                )

            hId   = 'io_%s.%s' % (monComp.name, dsName)
            hName = 'io_%s'    % dsName
            histos[hName] = ROOT.TH1F(
                hId,
                "[%s] I/O time [%s];%s;%s" % (
                    monComp.name, dsName,
                    "event nbr", "I/O time [ms]"
                    ),
                nEntries, minEvt, maxEvt
                )
            pass

        yMinIo = min(yMinIo)
        yMaxIo = max(yMaxIo)
        yMinCpu = min(yMinCpu)
        yMaxCpu = max(yMaxCpu)

        def markForLegend(p): setattr(p, '_markedForLegend', True)
        def isMarked(p):      return hasattr(p, '_markedForLegend')

        memLeak = []
        for dsName in dsNames:

            if 'evt' not in monComp.data[dsName]:
                continue

            data = monComp.data[dsName]

            cpu = data['evt']['cpu']
            cpu_c = cpu['cpu']
            dcpu_c= cpu_c[:,2]

            ## CPU
            bins = dataSetMgr[dsName].bins
            xbins= bins[self.minEvt:]
            if 'evt/cpu' not in monComp.figs:
                monComp.figs['evt/cpu'] = plt.figure()
                monComp.figs['evt/cpu'].add_subplot(211)
                monComp.figs['evt/cpu'].add_subplot(212)
            fig = monComp.figs['evt/cpu']
            ax = fig.axes[0]
            pl = ax.plot(
                xbins,
                dcpu_c[self.minEvt:] * ms,
                linestyle = 'steps',
                label = dsName
                )
            ax.grid(True)
            ax.set_title ("CPU time [Begin/End-Event]")
            ax.set_ylabel('CPU time [ms]')
            ax.set_xlabel('Event number' )
            ax.set_ylim((ax.get_ylim()[0]*0.9,
                         ax.get_ylim()[1]*1.1))
            markForLegend(pl[0])

            h,b = numpy.histogram(
                dcpu_c[self.minEvt:] * ms,
                bins  = 20,
                range = (yMinCpu*0.90, yMaxCpu*1.10)
                )
            ax = fig.axes[1]
            pl = ax.plot(b[:-1], h, label=dsName, ls='steps')
            ax.grid(True)
            ax.set_xlabel('CPU time [ms]')
            ax.set_ylim((ax.get_ylim()[0],
                         ax.get_ylim()[1]*1.1))
            markForLegend(pl[0])

            h = data['histos']['cpu_%s' % dsName]
            hAvg = bookAvgHist(h, dcpu_c*ms )
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                cpuTime = dcpu_c[i] * ms
                h.Fill( float(bins[i]), cpuTime )
                hAvg.Fill( cpuTime )

            ## Mem
            mem = data['evt']['mem']
            vmem = mem['vmem']
            dvmem = vmem[:,2]
            dmall = mem['mall'][:,2]

            if 'evt/mem' not in monComp.figs:
                monComp.figs['evt/mem'] = plt.figure()
                monComp.figs['evt/mem'].add_subplot(311)
                monComp.figs['evt/mem'].add_subplot(312)
                monComp.figs['evt/mem'].add_subplot(313)

            fig = monComp.figs['evt/mem']
            ax = fig.axes[0]
            pl = ax.plot( xbins,
                          vmem[self.minEvt:,1] * Mb ,
                          linestyle = 'steps',
                          #'o',
                          label     = dsName )
            ax.set_title ( "Memory usage [Begin/End-Event]" )
            ax.set_ylabel( 'VMem [MB]' )
            ax.set_xlabel( 'Event number' )
            ax.set_ylim( (ax.get_ylim()[0]*0.9,
                          ax.get_ylim()[1]*1.1) )
            ax.grid( True )
            markForLegend( pl[0] )

            ## fit
            #nFit = int(len(bins)/2.)
            #x = bins[-nFit:]
            #y = data['evt']['mem/vmem/1'][-nFit:] * Mb
            begFit = self._fitSlice[0]
            endFit = self._fitSlice[1]
            x = bins[begFit:endFit]
            y = vmem[begFit:endFit,1] * Mb
            coeffs = numpy.lib.polyfit( x, y, deg = 1 )
            memLeak += [ coeffs[0] * kb ]
            ax.plot( x, numpy.lib.polyval( coeffs, x ),
                     '-',
                     color = 'y',
                     lw = 2.
                     )
            if dsName == '000':
                _txt['mem']['slice'] += [
                    "%-20s p[0] = %8.3f MB \tp[1] = %8.3f kB" % (
                       "vmem fit:",
                       coeffs[1] * Mb, coeffs[0] * kb, #
                       )
                    ]

            ax = fig.axes[1]
            pl = ax.plot( xbins,
                          mem['rss'][self.minEvt:,1] * Mb,
                          linestyle = 'steps',
                          label     = dsName )
            ax.set_ylabel( 'RSS [MB]' )
            ax.set_xlabel( 'Event number' )
            ax.set_ylim( (ax.get_ylim()[0]*0.9,
                          ax.get_ylim()[1]*1.1) )
            ax.grid( True )
            markForLegend( pl[0] )

            ax = fig.axes[2]
            pl = ax.plot( xbins,
                          dmall[self.minEvt:] * Mb,
                          linestyle = 'steps',
                          label     = dsName )
            ax.set_ylabel( 'Delta Malloc [MB]' )
            ax.set_xlabel( 'Event number' )
            ax.set_ylim( (ax.get_ylim()[0]*0.9,
                          ax.get_ylim()[1]*1.1) )
            ax.grid( True )
            markForLegend( pl[0] )


            h = data['histos']['vmem_%s' % dsName]
            hAvg = bookAvgHist(h, mem['vmem'][:,1] * Mb)
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                vmem = mem['vmem'][i,1] * Mb
                h.Fill( float(bins[i]), vmem )
                hAvg.Fill( vmem )

            h = data['histos']['rss_%s' % dsName]
            hAvg = bookAvgHist(h, mem['rss'][:,1] * Mb)
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                rss = mem['rss'][i,1] * Mb
                h.Fill( float(bins[i]), rss )
                hAvg.Fill( rss )

            ## I/O
            if 'evt/io' not in monComp.figs:
                monComp.figs['evt/io'] = plt.figure()
                monComp.figs['evt/io'].add_subplot(211)
                monComp.figs['evt/io'].add_subplot(212)
            fig = monComp.figs['evt/io']
            ax = fig.axes[0]
            io_c = data['io']['w']['cpu']
            pl = ax.plot( xbins,
                          io_c[self.minEvt:] * ms,
                          linestyle = 'steps',
                          label = dsName )
            ax.set_title ( "I/O time [CommitOutput]" )
            ax.set_ylabel( 'I/O time [ms]' )
            ax.set_xlabel( 'Event number'  )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )
            ax.grid( True )
            markForLegend( pl[0] )

            yMinIo *= 0.9
            yMaxIo *= 1.1
            yMinIo,yMaxIo = min(yMinIo,yMaxIo), max(yMinIo,yMaxIo)
            h,b = numpy.histogram(
                io_c[self.minEvt:] * ms,
                bins  = 20,
                range = ( yMinIo, yMaxIo )
                )
            ax = fig.axes[1]
            pl = ax.plot( b[:-1], h, label=dsName, ls='steps' )
            ax.set_xlabel( 'I/O time [ms]' )
            ax.set_ylim( (ax.get_ylim()[0],
                          ax.get_ylim()[1]*1.1) )
            ax.grid( True )
            markForLegend( pl[0] )

            h = data['histos']['io_%s' % dsName]
            hAvg = bookAvgHist(h, io_c * ms)
            data['histos'][h.GetName()] = hAvg
            for i in range(len(bins)):
                cpuTime = io_c[i] * ms
                h.Fill( float(bins[i]), cpuTime )
                hAvg.Fill( cpuTime )

            pass # loop over data sets

        ## handle mem-leak text
        ax = monComp.figs['evt/mem'].axes[0]
        ax.text( 0.025, 0.97,
                 "\n".join( "[%s] Leak: %8.3f kB" % ( n, leak )
                            for n,leak in zip( DataSetMgr.labels(),
                                               memLeak ) ),
                 bbox = dict( facecolor='w' ),
                 #color = '',
                 fontweight = 'bold',
                 #fontsize   = 'x-larger',
                 verticalalignment = 'top',
                 transform = ax.transAxes )
        for figName,fig in monComp.figs.items():
            loc = 'best'
            if figName == 'evt/mem':
                loc = 'lower right'
            for ax in fig.axes:
                objs = [ z for z in ax.lines if isMarked(z) ]
                ax.legend( objs, DataSetMgr.labels(),
                           #loc='lower right'
                           #loc='best'
                           loc=loc
                           )


        ## get top-20 consumers
        self._top_consumers( dataSetMgr, monCompMgr, compTypes = ('alg',),
                             title = 'Event loop',
                             storeName = 'evt',
                             sliceName = 'evt',
                             begSlice  = self.minEvt,
                             endSlice  = None )
        return

    def processFin(self, dataSetMgr, monCompMgr):

        self._top_consumers(
            dataSetMgr, monCompMgr, compTypes = ('alg','algtool','svc'),
            title = "finalization",
            storeName = 'fin',
            sliceName = 'fin',
            begSlice  = 0,
            endSlice  = None
            )
        return

    def processIo(self, dataSetMgr, monCompMgr):

        ## short-hand
        ms = Units.ms

        dsNames = dataSetMgr.keys()
        dsNames.sort()

        monComps = [ ]
        cppTypes = {}
        for monComp in monCompMgr.values():

            if monComp.type != 'io':
                continue

            monCompKeys = monComp.data.keys()
            monCompKeys.sort()
            if monCompKeys != dsNames:
                continue
            monComps.append( monComp )
            cppTypes[monComp.name.split("#")[0]] = {}

        if len(monComps) == 0:
            self.msg.debug( "Could not find any io-container to process !" )
            return

        ## get input and/or output files
        inputFiles  = {}
        outputFiles = {}
        for dsName in dsNames:
            d = dataSetMgr[dsName].data
            for k in d.keys():
                if not k.startswith( "meta://inputPoolFiles/" ) and \
                   not k.startswith( "meta://outputPoolFiles/" ):
                    continue
                n,i = k.split( "PoolFiles/" )
                if   n.startswith( "meta://input" ):
                    f = inputFiles
                elif n.startswith( "meta://output"):
                    f = outputFiles
                else:
                    continue
                try:
                    f[dsName][int(i)]= d[k]
                except KeyError:
                    f[dsName] = {}
                    f[dsName][int(i)]= d[k]

        def _findFolders( files, pattern, dsName ):
            d = {}
            cppType = pattern[0]
            sgKey   = pattern[1]
            if dsName in files:
                for file in files[dsName].values():
                    for f in file['data']:
                        n = f['name']
                        pat = n.startswith( cppType+"_" ) and \
                              n.endswith  ( "_"+sgKey   )
                        if pat:
                            size = f['diskSize'] / float( f['nEntries'] )
                            try:
                                d[n] += size
                            except KeyError:
                                d[n]  = size
            keys = d.keys()
            if len(keys) > 1:
                raise RuntimeError ("len(d) > 1: %r" % d)
            if len(keys) == 0:
                return []
            else:
                return [ d.items()[0] ]

        ## helper function
        def io_proj(data, varexp):
            n,h = project(data, id="tmp", varexp=varexp)
            mean = h.GetMean()
            del h
            return mean

        ## try to find the disk-size for each container
        ## regroup everything by container type
        for monComp in monComps:
            monName = monComp.name
            cppType, sgKey = monComp.name.split("#")
            tp_pattern = ( cppType, sgKey )
            for dsName in dsNames:
                if dsNames not in cppTypes[cppType]:
                    cppTypes[cppType][dsName] = {}
                data = monComp.data[dsName]['evt']

                ior = io_proj(data, varexp=monName+".r.cpu()") *ms
                iorr= io_proj(data, varexp=monName+".rr.cpu()")*ms
                iow = io_proj(data, varexp=monName+".w.cpu()") *ms

                ior = ( ior,  io_proj(data, varexp=monName+".r.user")  * ms )
                iorr= ( iorr, io_proj(data, varexp=monName+".rr.user") * ms )
                iow = ( iow,  io_proj(data, varexp=monName+".w.user")  * ms )
                if ior[0] > 0.:
                    d = { 'size' : _findFolders( inputFiles, tp_pattern,
                                                 dsName ),
                          'r'    : ior,
                          'rr'   : iorr,
                          }
                    monComp.data[dsName]['io/in'] = d
                    if len(d['size']) > 0:
                        try:
                            io_in = cppTypes[cppType][dsName]['io/in']
                        except KeyError:
                            cppTypes[cppType][dsName]['io/in'] = {
                                'size' : [], 'r' : [], 'rr' : []
                                }
                            io_in = cppTypes[cppType][dsName]['io/in']
                        io_in['size'] += [ d['size'][0][1] ]
                        io_in['r'   ] += [ ior  ]
                        io_in['rr'  ] += [ iorr ]
                if iow[0] > 0.:
                    d = {
                        'size' : _findFolders(outputFiles, tp_pattern, dsName),
                        'w'    : iow,
                        }
                    monComp.data[dsName]['io/out']= d
                    if len(d['size']) > 0:
                        try:
                            io_out = cppTypes[cppType][dsName]['io/out']
                        except KeyError:
                            cppTypes[cppType][dsName]['io/out'] = {
                                'size' : [], 'w' : []
                                }
                            io_out = cppTypes[cppType][dsName]['io/out']
                        io_out['size'] += [ d['size'][0][1] ]
                        io_out['w'   ] += [ iow  ]
            pass

        self.sum['io']['fig'] = []

        def _createFig( ioKey, speedKey, cppTypes, dsNames, figTitle ):
            #print ":"*80,figTitle
            fig = plt.figure()
            fig.add_subplot(311)
            fig.add_subplot(312)
            fig.add_subplot(313)

            from .App import DataSetMgr
            color = DataSetMgr.colorIter()

            # to hold 'cpu' for each dataSet
            axes = [ [], [], [] ]

            names = {}
            for idx,dsName in enumerate(dsNames):
                table = []
                names[dsName] = []
                for cppType in cppTypes.keys():
                    store = cppTypes[cppType][dsName]
                    if ioKey not in store:
                        continue
                    store = store[ioKey]
                    if len(store['size']) <= 0:
                        continue
                    names[dsName].append( cppType )
                    size = numpy.array(store['size'])
                    cTime= numpy.array([i[0] for i in store[speedKey]])
                    uTime= numpy.array([i[1] for i in store[speedKey]])
                    if speedKey == 'r':
                        cTimeR = numpy.array([i[0] for i in store['rr']])
                    else:
                        cTimeR = cTime
                    cFreq= numpy.array( [1./i for i in cTime] )
                    uFreq= numpy.array( [1./i for i in uTime] )
                    speed = size / cTime
                    #print cppType,store['size'],cTime.mean(),cTime,speed,speed.mean(),cFreq.mean()
                    table.append(
                        ( len(names[dsName]) - 1,
                          size.mean(),
                          cTimeR.mean() * ms,
                          cTime.mean() * ms,
                          uTime.mean() * ms,
                          cFreq.mean() / ms,
                          uFreq.mean() / ms,
                          speed.mean() /1024. * 1e3,
                        ) )
                    pass
                descr = numpy.dtype( {
                    'names'   : ( 'name',  'size',
                                  speedKey+'/cpuRoot',
                                  speedKey+'/cpu',     speedKey+'/user',
                                  speedKey+'/cpuFreq', speedKey+'/userFreq',
                                  'speed', ),
                    'formats' : ( 'int32',   'float64',
                                  'float64', 'float64', 'float64',
                                  'float64', 'float64',
                                  'float64', ),
                    } )
                table = numpy.array( table, dtype = descr )
                nData = len(table)
                if nData == 0:
                    return None

                table.sort( order=('size',) )

                # linewidth for horizontal bars
                lw = 0.01
                nDsNames = float(len(dsNames))

                ## -- processing time
                ax = fig.axes[0]
                ax.set_title( figTitle )
                xlabel = '<Mean> CPU time (user+sys) [ms]'
                if speedKey == 'r':
                    xlabel = '<Mean> CPU time (user+sys) [green=pure ROOT] [ms]'
                ax.set_xlabel( xlabel )

                labels = [ names[dsName][i] for i in table['name'] ]
                data = table[speedKey+'/cpu']
                fc   = next(color)
                pos  = numpy.arange(nData)
                p = ax.barh( pos + (idx*1./nDsNames),
                             width  = data,
                             height = 1./nDsNames,
                             color  = 'r',
                             label  = dsName,
                             lw     = lw )
                ax.grid(True)
                ax.set_yticks( pos + 0.5 )
                ax.set_yticklabels( labels, fontsize =6,
                                    horizontalalignment = 'right' )

                # display 'user' part only
                data = table[speedKey+'/user']
                pos  = numpy.arange(nData)
                p = ax.barh( pos + (idx*1./nDsNames),
                             data,
                             1./nDsNames,
                             align ='edge',
                             color = fc,
                             label = dsName,
                             lw = lw )
                axes[0] += p

                # hack to display the ROOT component (only for reading)
                if speedKey == 'r':
                    data = table[speedKey+'/cpuRoot']
                    pos  = numpy.arange(nData)
                    p = ax.barh( pos + (idx*1./nDsNames),
                                 data,
                                 1./nDsNames,
                                 align ='edge',
                                 color = 'g',
                                 label = dsName,
                                 lw = lw )


                ## -- Speed
                ax = fig.axes[1]
                ax.set_xlabel( '<Mean> Speed (user+sys) [MB/s]' )

                labels = [ names[dsName][i] for i in table['name'] ]
                data = table[speedKey+'/cpu']
                data = table['speed']
                fc   = next(color)
                pos  = numpy.arange(nData)
                p = ax.barh( pos + (idx*1./nDsNames),
                             width  = data,
                             height = 1./nDsNames,
                             color  = 'r',
                             label  = dsName,
                             lw     = lw )
                ax.grid(True)
                ax.set_yticks( pos + 0.5 )
                ax.set_yticklabels( labels, fontsize =6,
                                    horizontalalignment = 'right' )

                # display 'user' part only
                data = table[speedKey+'/user']
                data = table['speed']
                pos  = numpy.arange(nData)
                p = ax.barh( pos + (idx*1./nDsNames),
                             data,
                             1./nDsNames,
                             align ='edge',
                             color = fc,
                             label = dsName,
                             lw = lw )
                axes[1] += p

                ## -- Frequency

                ax = fig.axes[2]
                ax.set_xlabel( '<Mean> Frequency (user+sys) [kHz]' )

                labels = [ names[dsName][i] for i in table['name'] ]
                data = table[speedKey+'/cpuFreq']
                fc   = next(color)
                pos  = numpy.arange(nData)
                p = ax.barh( pos + (idx*1./nDsNames),
                             width  = data,
                             height = 1./nDsNames,
                             color  = 'r',
                             label  = dsName,
                             lw     = lw )
                ax.grid(True)
                ax.set_yticks( pos + 0.5 )
                ax.set_yticklabels( labels, fontsize =6,
                                    horizontalalignment = 'right' )

                # display 'user' part only
                data = table[speedKey+'/userFreq']
                pos  = numpy.arange(nData)
                p = ax.barh( pos + (idx*1./nDsNames),
                             data,
                             1./nDsNames,
                             align ='edge',
                             color = fc,
                             label = dsName,
                             lw = lw )
                axes[2] += p


            for ix, ax in zip(axes, fig.axes):
                ax.legend( ix[::nData], DataSetMgr.labels(), loc='lower right' )
            return fig

        fig = _createFig( 'io/in',  'r',  cppTypes, dsNames,
                          figTitle = '[P->T] transformations' )
        if fig:
            self.sum['io']['fig'] += [ fig ]

        fig = _createFig( 'io/out', 'w',  cppTypes, dsNames,
                          figTitle = '[T->P] transformations' )
        if fig:
            self.sum['io']['fig'] += [ fig ]

        return

    def _top_consumers(self, dataSetMgr, monCompMgr, compTypes,
                       title,
                       storeName,
                       sliceName, begSlice, endSlice):
        from .App import DataSetMgr
        ## fill-in the text file
        _txt = self.txt[sliceName]

        ## short-hand
        ms = Units.ms
        kb = Units.kb
        Mb = Units.Mb
        msg = self.msg

        dsNames = list(dataSetMgr.keys())
        dsNames.sort()

        monComps = [ ]
        for monComp in monCompMgr.values():

            if monComp.type not in compTypes:
                continue

            monCompKeys = list(monComp.data.keys())
            monCompKeys.sort()
            if monCompKeys != dsNames:
                continue
            # remove AthMasterSeq and AthAlgSeq
            if monComp.name in ('AthMasterSeq', 'AthAlgSeq',):
                continue
            monComps.append(monComp)

        if len(monComps) == 0:
            msg.debug("Could not find any monitored component for"
                      " _top_consumers_ analysis !")
            return

        # create figure
        # --------------
        fig = plt.figure()
        fig.add_subplot(411)
        fig.add_subplot(412)
        fig.add_subplot(413)
        fig.add_subplot(414)
        self.sum[sliceName]['fig'] = fig

        # to hold 'cpu' for each dataSet
        axes = [ [], [], [], [] ]

        color = DataSetMgr.colorIter()
        timings = {}

        _monComps = [ m for m in monComps ]
        for idx,dsName in enumerate(dsNames):

            monComps = []
            table = []
            timings[dsName] = []
            for monComp in _monComps:

                if storeName not in monComp.data[dsName]:
                    continue


                store = monComp.data[dsName][storeName]
                if store is None:
                    ## if storeName == 'evt':
                    ##     print "===",dsName,storeName,monComp.name
                    continue
                if len(store['mem']) == 0:
                    ## if storeName == 'evt':
                    ##     print "+++",dsName,storeName,monComp.name
                    continue

                cpu   = store['cpu']
                cpu_u = cpu['user'][:,2]
                cpu_s = cpu['sys'][:,2]
                cpu_c = cpu['cpu'][:,2]
                cpu_r = cpu['real'][:,2]

                mem = store['mem']
                vmem= mem['vmem']
                mall= mem['mall']
                dvmem = vmem[:,2]
                dmall = mall[:,2]

                monComps.append(monComp)
                timings[dsName].append(monComp.name)
                table.append(
                    (len(timings[dsName]) -1,
                     cpu_u[begSlice:endSlice].mean()*ms,
                     cpu_s[begSlice:endSlice].mean()*ms,
                     (cpu_c[begSlice:endSlice]*ms).mean(),
                     (cpu_r[begSlice:endSlice]*ms).mean(),
                     (dvmem[begSlice:endSlice]*kb).mean(),
                     dmall[begSlice:endSlice].mean()*kb,
                     mem['nmall'][begSlice:endSlice,2].mean(),
                     mem['nfree'][begSlice:endSlice,2].mean(),
                     )
                    )
                has_nan = False
                new_value = list(table[-1])
                for ii,_vv in enumerate(new_value):
                    if numpy.isnan(_vv):
                        has_nan = True
                        new_value[ii] = 0
                        pass
                    pass
                if has_nan:
                    msg.warn("NaN in [%s]", monComp.name)
                    msg.warn(" -%s", table[-1])
                    table[-1] = tuple(new_value)
                    msg.warn(" +%s", table[-1])
                    pass

                pass # loop over components

            descr = numpy.dtype( {
                'names'  : ( 'name',
                             'cpu/user', 'cpu/sys', 'cpu/cpuTime', 'cpu/real',
                             'mem', 'malloc', 'nmall', 'nfree' ),
                'formats': ( 'int32',
                             'float64', 'float64', 'float64', 'float64',
                             'float64', 'float64', 'int64', 'int64' )
                } )
            try:
                table = numpy.array( table, dtype = descr )

                nData = min(
                    self.max,
                    len([ m for m in monComps if m.type in compTypes ])
                    )
            except Exception as err:
                #msg.error("table: %s", table)
                msg.error("caught:  %s", err)
                msg.error("context: title=%s store=%s slice=%s beg=%s end=%s",
                          title, storeName, sliceName, begSlice, endSlice)
                nData = 0
                continue

            if nData == 0:
                _warn = msg.warning
                _warn("in top_consumers: no data to plot for [%s]!", title)
                _warn("** this may mean you'd have to re-run perfmon with more details enabled...")
                _warn("dsname=[%s] storename=[%s]", dsName, storeName)
                _warn("no component found with type %s", compTypes)
                continue

            # linewidth for horizontal bars
            lw = 0.01
            names  = timings[dsName]
            nDsNames = float(len(dsNames))

            ## CPU
            table.sort( order=('cpu/cpuTime',) )
            ax = fig.axes[0]
            ax.set_title ( 'Top consumers during [%s]' % title )
            ax.set_xlabel( '<Mean> CPU time (user+sys) [ms]' )

            labels = [ names[i] for i in table['name'][-nData:] ]
            data = table['cpu/cpuTime'][-nData:]
            fc   = next(color)
            pos  = numpy.arange(nData)
            p = ax.barh( pos + (idx*1./nDsNames),
                         width  = data,
                         height = 1./nDsNames,
                         align ='edge',
                         color = 'r',
                         label = dsName,
                         lw    = lw )
            ax.grid(True)
            ax.set_yticks( pos + 0.5 )
            ax.set_yticklabels( labels,
                                fontsize = 6,
                                horizontalalignment = 'right' )

            # display 'user' part only
            data = table['cpu/user'][-nData:]
            pos  = numpy.arange(nData)
            p = ax.barh( pos + (idx*1./nDsNames),
                         data,
                         1./nDsNames,
                         align ='edge',
                         color = fc,
                         label = dsName,
                         lw = lw )
            axes[0] += p

            ## ==> fill ASCII
            if dsName == '000':
                _c = _txt['cpu']['comps']
                for _i in zip( table['cpu/cpuTime'],
                               table['cpu/sys'],
                               [ names[i] for i in table['name'] ] ):
                    if not numpy.isnan(_i[0]):
                        _c += [
                            "[cpu/sys] %10.3f %10.3f (ms) | %s" % _i
                            ]
                    else:
                        self.msg.error( "%s contains weird data !!",_i[-1] )
                _c.reverse()

            ## real-time
            table.sort( order=('cpu/real',) )
            labels = [ names[i] for i in table['name'][-nData:] ]
            ax = fig.axes[1]
            ax.set_xlabel( '<Mean> real time [ms]' )
            data = table['cpu/real'][-nData:]
            pos  = numpy.arange(len(data))
            p = ax.barh( pos + (idx * 1./nDsNames),
                         data,
                         1./nDsNames,
                         align ='edge',
                         color = next(color),
                         label = dsName,
                         lw    = lw )
            ax.grid(True)
            ax.set_yticks( pos + 0.5 )
            ax.set_yticklabels( labels,
                                fontsize = 6,
                                horizontalalignment = 'right' )

            axes[1] += p

            ## virtual memory
            table.sort( order=('mem',) )
            labels = [ names[i] for i in table['name'][-nData:] ]
            ax = fig.axes[2]
            #ax.set_xlabel( '$\Delta \rm{VMem}\ [kB]$' )
            ax.set_xlabel( 'delta VMem [kB]' )
            data = table['mem'][-nData:]
            pos  = numpy.arange(len(data))
            p = ax.barh( pos + (idx * 1./nDsNames),
                         data,
                         1./nDsNames,
                         align ='edge',
                         color = next(color),
                         label = dsName,
                         lw    = lw )
            ax.grid(True)
            ax.set_yticks( pos + 0.5 )
            ax.set_yticklabels( labels,
                                fontsize = 6,
                                horizontalalignment = 'right' )

            axes[2] += p

            ## ==> fill ASCII
            if dsName == '000':
                _c = _txt['mem']['comps']
                for _i in zip( table['mem'],
                               table['malloc'],
                               [ names[i] for i in table['name'] ] ):
                    if not numpy.isnan(_i[0]):
                        _c += [ "dVmem|dMalloc %10.3f %10.3f kB | %s" % _i ]
                    else:
                        self.msg.error( "%s contains weird data !!",_i[-1] )
                _c.reverse()

            ## malloc
            table.sort( order=('malloc',) )
            labels = [ names[i] for i in table['name'][-nData:] ]
            ax = fig.axes[3]
            #ax.set_xlabel( '$\Delta \rm{Malloc}\ [kB]$' )
            ax.set_xlabel( 'delta Malloc [kB]' )
            data = table['malloc'][-nData:]
            pos  = numpy.arange(len(data))
            p = ax.barh( pos + (idx * 1./nDsNames),
                         data,
                         1./nDsNames,
                         align ='edge',
                         color = next(color),
                         label = dsName,
                         lw    = lw )
            ax.grid(True)
            ax.set_yticks( pos + 0.5 )
            ax.set_yticklabels( labels,
                                fontsize = 6,
                                horizontalalignment = 'right' )

            axes[3] += p

            ## n-allocs
            table.sort(order=('nmall',))
            labels = [ names[i] for i in table['name'][-nData:] ]
            ## ==> fill ASCII
            if dsName == '000':
                _c = _txt['allocs']['comps']
                for _i in zip( table['nmall'],
                               table['nfree'],
                               [ names[i] for i in table['name'] ] ):
                    if not numpy.isnan(_i[0]):
                        _c += [ "alloc|free %10i %10i | %s" % _i ]
                    else:
                        self.msg.error( "%s contains weird data !!",_i[-1] )
                _c.reverse()

            pass # loop over data sets

        if nData != 0:

            for ix, ax in zip(axes, fig.axes):
                ax.legend(ix[::nData], DataSetMgr.labels(), loc='lower right')
        else:
            ## msg.error("problem in _top_consumers: null slice array !")
            ## msg.error("dsname=[%s] compname=[%s] storename=[%s]",
            ##           dsName, monComp.name, storeName)
            ## return
            pass

        m = monCompMgr['PerfMonSlice']
        d = m.data['000'][storeName]
        cpu = "%-20s [%10.3f ms    %10.3f ms\t real=  %10.3f ms ]" % (
            "[u/s/r]",
            d['cpu']['user'][begSlice:endSlice,2].mean()*ms,
            d['cpu']['sys'] [begSlice:endSlice,2].mean()*ms,
            d['cpu']['real'][begSlice:endSlice,2].mean()*ms,
            )

        dvmem = d['mem']['vmem'][:,2]
        drss  = d['mem']['rss'][:,2]
        dmall = d['mem']['mall'][:,2]

        mem = "%-20s [%10.3f MB -> %10.3f MB\t delta= %10.3f kB ]\n" \
              "%-20s [%10.3f MB -> %10.3f MB\t delta= %10.3f kB ]\n" \
              "%-20s [%10.3f MB -> %10.3f MB\t delta= %10.3f kB ]\n" \
              "%-20s [%10i    -> %10i \t delta= %10i    ]\n" \
              "%-20s [%10i    -> %10i \t delta= %10i    ]"   % (
            "VMem",
            d['mem']['vmem'][begSlice:endSlice,0].mean()*Mb,
            d['mem']['vmem'][begSlice:endSlice,1].mean()*Mb,
            dvmem[begSlice:endSlice].mean()*kb,
            "RSS",
            d['mem']['rss'] [begSlice:endSlice,0].mean()*Mb,
            d['mem']['rss'] [begSlice:endSlice,1].mean()*Mb,
            drss[begSlice:endSlice].mean()*kb,
            "Malloc",
            d['mem']['mall'] [begSlice:endSlice,0].mean()*Mb,
            d['mem']['mall'] [begSlice:endSlice,1].mean()*Mb,
            dmall[begSlice:endSlice].mean()*kb,
            "nalloc",
            d['mem']['nmall'][begSlice:endSlice,0].mean(),
            d['mem']['nmall'][begSlice:endSlice,1].mean(),
            d['mem']['nmall'][begSlice:endSlice,2].mean(),
            "nfree",
            d['mem']['nfree'][begSlice:endSlice,0].mean(),
            d['mem']['nfree'][begSlice:endSlice,1].mean(),
            d['mem']['nfree'][begSlice:endSlice,2].mean(),
            )

        _txt['cpu']['slice'] += [ cpu ]
        _txt['mem']['slice'] += [ mem ]

        return
