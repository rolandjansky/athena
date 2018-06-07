#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Transate arbitrary root file into a han config file
@author: ponyisi@utexas.edu
9 Oct 2008
Adapted for physics validation 14 May 2014
"""

from DQConfMakerBase.DQElements import *
from DQConfMakerBase.Helpers import IDHelper, make_thresholds
from DataQualityUtils.hanwriter import writeHanConfiguration
import ROOT

repeatalgorithm = DQAlgorithm(id='RepeatAlgorithm',
                              libname='libdqm_algorithms.so')
worst = DQAlgorithm(id='WorstCaseSummary',libname='libdqm_summaries.so')

### SOME THINGS YOU MIGHT WANT TO EDIT
# Edit this to change what algorithm is applied (AuxAlgName--xxx)
# or to disable printing the number of entries for each reference
#algorithmparameters = [DQAlgorithmParameter('AuxAlgName--Chi2Test_Prob', 1),
algorithmparameters = [DQAlgorithmParameter('AuxAlgName--Chi2Test_Chi2_per_NDF', 1),
                       DQAlgorithmParameter('RepeatAlgorithm--ResultsNEntries', 1)]

# Edit this to change thresholds
thresh = make_thresholds('Chi2_per_NDF', 1.0, 1.50, 'Chi2Thresholds')

def recurse(rdir, dqregion, ignorepath, refs=None, displaystring='Draw=PE', displaystring2D='Draw=COLZ', regex=None, startpath=None, hists=None):
    import re
    for key in rdir.GetListOfKeys():
        cl = key.GetClassName(); rcl = ROOT.TClass.GetClass(cl)
        if ' ' in key.GetName():
            print 'WARNING: cannot have spaces in histogram names for han config; not including %s %s' % (cl, key.GetName())
            continue
        if rcl.InheritsFrom('TH1'):
            if '/' in key.GetName():
                print 'WARNING: cannot have slashes in histogram names, encountered in directory %s, histogram %s' % (rdir.GetPath(), key.GetName())
                continue
            if key.GetName() == 'summary':
                print 'WARNING: cannot have histogram named summary, encountered in %s' % rdir.GetPath()
                continue
            fpath = rdir.GetPath().replace(ignorepath, '')
            name = (fpath + '/' + key.GetName()).lstrip('/')
            #print rdir.GetPath(), ignorepath, name
            if hists:
                match = False
                for hist in hists:
                    if hist.match(name):
                        match = True
                if not match: continue
            elif regex:
                if not regex.match(name): continue
            dqpargs = { 'id' : ('' if fpath else 'top_level/') + name,
                        'algorithm': repeatalgorithm,
                        'inputdatasource': (startpath + '/' if startpath else '') + name,
                        'algorithmparameters': algorithmparameters,
                        #'thresholds': chi2thresh,
                        'thresholds': thresh,
                        }
            if refs:
                dqpargs['references'] = refs
            dqpar = dqregion.newDQParameter( **dqpargs)
            drawstrs = []
            if not options.normalize: drawstrs.append('NoNorm')
            if options.logy and (cl.startswith('TH1') or cl.startswith('TProfile')): drawstrs.append('LogY')
            if options.logy and cl.startswith('TH2'): drawstrs.append('LogZ')
            if cl.startswith('TH1'): drawstrs.append(displaystring)
            if cl.startswith('TProfile'): drawstrs.append(displaystring)
            if cl.startswith('TH2'): drawstrs.append(displaystring2D)
            if options.scaleref != 1: drawstrs.append('ScaleRef=%f' % options.scaleref)
            if options.ratio: drawstrs.append('RatioPad')
            #if options.ratio: drawstrs.append('Ref2DSignif')
            if options.ratio2D: drawstrs.append('Ref2DRatio')

            drawstrs.append('DataName=%s' % options.title)
            dqpar.addAnnotation('display', ','.join(drawstrs))
            
        elif rcl.InheritsFrom('TDirectory'):
            newregion = dqregion.newDQRegion( key.GetName(), algorithm=worst )
            recurse(key.ReadObj(), newregion, ignorepath, refs, displaystring, displaystring2D, regex, startpath, hists)

def prune(dqregion):
    """
    returns True if we should kill this node
    False if we should not
    """
    params = dqregion.getDQParameters()
    if params == None:
        params = []
    subregions = dqregion.getSubRegions()
    if subregions == None:
        subregions = []
    else:
        subregions = subregions[:]
    # kill subregions
    for sr in subregions:
        if sr == None:
            continue
        if prune(sr):
            dqregion.delRelation('DQRegions', sr)
    subregions = dqregion.getSubRegions()
    if subregions == None:
        subregions = []
    if len(subregions) + len(params) == 0:
        return True
    else:
        return False

def paramcount(dqregion):
    params = dqregion.getDQParameters()
    if params == None:
        params = []
    subregions = dqregion.getSubRegions()
    if subregions == None:
        subregions = []
    
    return len(params) + sum([paramcount(region) for region in subregions])

def process(infname, confname, options, refs=None):
    import re
    f = ROOT.TFile.Open(infname, 'READ')
    if not f.IsOpen():
        print 'ERROR: cannot open %s' % infname
        return
    
    top_level = DQRegion(id='topRegion',algorithm=worst)
    print 'Building tree...'
    refpairs = refs.split(',')
    try:
        refdict = dict(_.split(':') for _ in refpairs)
    except Exception, e:
        print e
    dqrs = [DQReference(reference='%s:same_name' % v, id=k)
            for k, v in refdict.items()]
    displaystring = options.drawopt
    if options.refdrawopt:
        displaystring += ',' + (','.join('DrawRef=%s' % _ for _ in options.refdrawopt.split(',')))
    displaystring2D = options.drawopt2D
    if options.drawrefopt2D:
        displaystring2D += ',' + (','.join('DrawRef2D=%s' % _ for _ in options.drawrefopt2D.split(',')))

    if options.startpath:
        topindir = f.Get(options.startpath)
        if not topindir:
            raise ValueError("Path %s doesn't exist in input file" % options.startpath)
        topindirname = f.GetPath() + options.startpath.strip('/')
        startpath = options.startpath.strip('/')
    else:
        topindir = f
        topindirname = f.GetPath()
        startpath = None
    hists = []
    if options.histlistfile:
        hists = [re.compile(line.rstrip('\n')) for line in open(options.histlistfile)]
        if options.pathregex: print "histlistfile given, pathregex is ignored"
    recurse(topindir, top_level, topindirname, dqrs, displaystring, displaystring2D,
            re.compile(options.pathregex), startpath, hists)
    print 'Pruning dead branches...'
    prune(top_level)
    pc = paramcount(top_level)
 
    sublevel = top_level.getSubRegions()[:]
    for x in sublevel:
        top_level.delRelation('DQRegions', x)
        
    print 'Writing output'
    writeHanConfiguration( filename = confname , roots = sublevel)
    return pc

def super_process(fname, options):
    import shutil, os, sys, contextlib
    import ROOT
    han_is_found = (ROOT.gSystem.Load('libDataQualityInterfaces') != 1)
    if not han_is_found:
        print 'ERROR: unable to load offline DQMF; unable to proceed'
        sys.exit(1)
    bname = os.path.basename(fname)

    hanconfig = None
    hanhcfg = None
    hanoutput = None

    failed = False

    @contextlib.contextmanager
    def tmpdir():
        import tempfile
        td = tempfile.mkdtemp()
        yield td
        shutil.rmtree(td)

    with tmpdir() as hantmpdir:
        try:
            print '====> Processing file %s' % (fname)
            print '====> Generating han configuration file'
            hantmpinput = os.path.join(hantmpdir, bname)
            shutil.copyfile(fname, hantmpinput)
            haninput = hantmpinput
            hanconfig = os.path.join(hantmpdir, 'han.config')
            rv = process(hantmpinput, hanconfig, options, options.reffile)
            #shutil.copy(hanconfig, os.getcwd())
            
            # bad hack. rv = number of histogram nodes
            if rv == 0:
                print 'No histograms to display; exiting with code 0'
                sys.exit(0)

            print '====> Compiling han configuration'
            hanhcfg = os.path.join(hantmpdir, 'han.hcfg')
            ROOT.dqi.HanConfig().AssembleAndSave( hanconfig, hanhcfg )
            print '====> Executing han'
            import resource
            memlimit = resource.getrlimit(resource.RLIMIT_AS)
            resource.setrlimit(resource.RLIMIT_AS, (memlimit[1], memlimit[1]))
            hanoutput = haninput.rpartition('.')[0] + '_han.root'

            rv = ROOT.dqi.HanApp().Analyze( hanhcfg, haninput, hanoutput )
            if rv != 0:
                raise Exception('failure in han')
            print '====> Dumping web display output'
            from DataQualityUtils import handimod
            handimod.handiWithComparisons( options.title,
                                           hanoutput,
                                           options.outdir,
                                           '', False, False, 
                                           'https://atlasdqm.web.cern.ch/atlasdqm/js/')
##            print '====> Copying to', hantargetdir
##            hantargetfile = os.path.join(hantargetdir, 'out_han.root')
##            if not os.access(hantargetdir, os.W_OK):
##                try:
##                    os.makedirs(hantargetdir)
##                except Exception, e:
##                    print 'Unable to create %s for some reason: %s' % (hantargetdir, e)
##                    raise Exception('Error during execute')
##            shutil.copy2(hanoutput, hantargetfile)
##            print '====> Cleaning up'
            os.unlink(hanoutput)
        except Exception, e:
            print e
            if 'canonical format' not in str(e):
                failed = True
        finally:
            try:
                if not prebuilt_hcfg:
                    os.unlink(hantmpinput)
                    os.unlink(hanconfig)
                    os.unlink(hanhcfg)
                os.unlink(hanoutput)
            except:
                pass
        
    return not failed

        
if __name__=="__main__":
    import sys, optparse, shutil, os
    os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS']='1'
    parser = optparse.OptionParser(usage='usage: %prog [options] inputfile')
    parser.add_option('--reffile', default=None,
                      help='Reference files to use. Must have same structure as inputfile.  Format: tag1:reffile1.root,tag2:reffile2.root,...')
    parser.add_option('--outdir', default='./handi',
                      help='Directory for web ouptut')
    parser.add_option('--normalize', default=False, action='store_true',
                      help='Normalize reference histograms for display')
    parser.add_option('--title', default='Summary',
                      help='Title for histograms being tested')
    parser.add_option('--drawopt', default='Draw=PE',
                      help='Draw options for tested histograms (only use if you know what you are doing)')
    parser.add_option('--refdrawopt',
                      help='ROOT Draw option for reference histograms (e.g. HIST)')
    parser.add_option('--drawopt2D', default='Draw=COLZ',
                      help='Draw options for tested TH2 histograms (only use if you know what you are doing)')
    parser.add_option('--drawrefopt2D', default=None,
                      help='Draw options for reference TH2 histograms. If nothing is specified, no 2D reference histograms are drawn. If you want to draw both test and reference histo, recommended settings are --drawopt2D="Draw=BOX" --drawrefopt2D="COLZ"')
    parser.add_option('--logy', action='store_true',
                      help='Display on log Y scale')
    parser.add_option('--pathregex', default='.*',
                      help='Specify regex to match histograms, e.g. "(Btag|Jets)"')
    parser.add_option('--startpath', default=None,
                      help='Start from this subdirectory of the file')
    parser.add_option('--histlistfile',
                      help='text file with a list of regexes/histogram names')
    parser.add_option('--scaleref', type=float, default=1,
                      help='Scale references by this value')
    parser.add_option('--Kolmogorov', default=False, action='store_true',
                      help='Run Kolmogorov test instead of Chi2 test')
    parser.add_option('--ratio', default=False, action='store_true',
                      help='Draw histograms with ratio plots')
    parser.add_option('--ratio2D', default=False, action='store_true',
                      help='Draw 2D histograms with ratio plots')


    options, args = parser.parse_args()
    
    if not 1 == len(args):
        parser.print_help()
        sys.exit(1)
    fname = args[0]
    if options.Kolmogorov:
        algorithmparameters = [DQAlgorithmParameter('AuxAlgName--KolmogorovTest_Prob', 1),
                               DQAlgorithmParameter('RepeatAlgorithm--ResultsNEntries', 1)]
        thresh = make_thresholds('P', 0.05, 0.01, 'pThresholds')

    rv = super_process(fname, options)
    if rv == True:
        sys.exit(0)
    else:
        sys.exit(1)    
