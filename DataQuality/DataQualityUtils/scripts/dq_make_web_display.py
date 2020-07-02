#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Transate arbitrary root file into a han config file with the "GatherData" algorithm
@author: ponyisi@hep.uchicago.edu
9 Oct 2008
Adapted for fast physics monitoring 14 April 2011
"""
from __future__ import print_function

#HANDIR='/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/fastphysmon/1'

from DQConfMakerBase.DQElements import DQReference, DQRegion, DQAlgorithm
from DQHanConfMaker.hanwriter import writeHanConfiguration
import ROOT

gatherdata = DQAlgorithm(id='GatherData', 
                         libname='libdqm_algorithms.so')
worst = DQAlgorithm(id='WorstCaseSummary',libname='libdqm_summaries.so')

def recurse(rdir, dqregion, ignorepath, reffile=None):
##    print [x.__class__ for x in toList(dqregion.getSubRegions()) ]
    for key in rdir.GetListOfKeys():
        cl = key.GetClassName(); rcl = ROOT.TClass.GetClass(cl)
        #print key.GetName(), cl
        if ' ' in key.GetName():
            print('WARNING: cannot have spaces in histogram names for han config; not including %s %s' % (cl, key.GetName()))
            continue
        if rcl.InheritsFrom('TH1'):
            if '/' in key.GetName():
                print('WARNING: cannot have slashes in histogram names, encountered in directory %s, histogram %s' % (rdir.GetPath(), key.GetName()))
                continue
            if key.GetName() == 'summary':
                print('WARNING: cannot have histogram named summary, encountered in %s' % rdir.GetPath())
                continue
            name = rdir.GetPath().replace(ignorepath, '') + '/' + key.GetName()
            dqpargs = { 'id' :name,
                        'algorithm': gatherdata,
                        'inputdatasource': name,
                        }
            if reffile:
                dqpargs['references'] = DQReference(reference='%s:same_name' % reffile)
            dqpar = dqregion.newDQParameter( **dqpargs)
            dqpar.addAnnotation('display', 'NoNorm')
            
        elif rcl.InheritsFrom('TDirectory'):
            #print key.GetName()
            newregion = dqregion.newDQRegion( key.GetName(), algorithm=worst )
            recurse(key.ReadObj(), newregion, ignorepath, reffile)

def prune(dqregion):
    """
    returns True if we should kill this node
    False if we should not
    """
    params = dqregion.getDQParameters()
    if params is None:
        params = []
    subregions = dqregion.getSubRegions()
    if subregions is None:
        subregions = []
    else:
        subregions = subregions[:]
    # kill subregions
    for sr in subregions:
        if sr is None:
            continue
        if prune(sr):
            dqregion.delRelation('DQRegions', sr)
    subregions = dqregion.getSubRegions()
    if subregions is None:
        subregions = []
    if len(subregions) + len(params) == 0:
        return True
    else:
        return False

def paramcount(dqregion):
    params = dqregion.getDQParameters()
    if params is None:
        params = []
    subregions = dqregion.getSubRegions()
    if subregions is None:
        subregions = []
    
    return len(params) + sum([paramcount(region) for region in subregions])

def process(infname, confname, reffile=None):
    f = ROOT.TFile(infname, 'READ')
    if not f.IsOpen():
        print('ERROR: cannot open %s' % infname)
        return
    
    top_level = DQRegion(id='topRegion',algorithm=worst)
    print('Building tree...')
    recurse(f, top_level, f.GetPath(), reffile)
    print('Pruning dead branches...')
    prune(top_level)
    pc = paramcount(top_level)
 
    sublevel = top_level.getSubRegions()[:]
    for x in sublevel:
        top_level.delRelation('DQRegions', x)
        
    print('Writing output')
    writeHanConfiguration( filename = confname , roots = sublevel)
    return pc

def super_process(fname, options):
    import shutil, os, sys, contextlib
    import ROOT
    han_is_found = (ROOT.gSystem.Load('libDataQualityInterfaces') == 0)
    if not han_is_found:
        print('ERROR: unable to load offline DQMF; unable to proceed')
        sys.exit(1)
    bname = os.path.basename(fname)

    run = options.run
    hanconfig = None
    hanhcfg = None
    hanoutput = None

    failed = False
    prebuilt_hcfg = False

    @contextlib.contextmanager
    def tmpdir():
        import tempfile
        td = tempfile.mkdtemp()
        yield td
        shutil.rmtree(td)

    with tmpdir() as hantmpdir:
        try:
            print('====> Processing file %s' % (fname))
            print('====> Generating han configuration file')
            hantmpinput = os.path.join(hantmpdir, bname)
            shutil.copyfile(fname, hantmpinput)
            haninput = hantmpinput
            hanconfig = os.path.join(hantmpdir, 'han.config')
            rv = process(hantmpinput, hanconfig, options.reffile)
            # bad hack. rv = number of histogram nodes
            if rv == 0:
                print('No histograms to display; exiting with code 0')
                sys.exit(0)

            print('====> Compiling han configuration')
            hanhcfg = os.path.join(hantmpdir, 'han.hcfg')
    ##         os.system('han-config-gen.exe %s' % hanconfig)
            ROOT.dqi.HanConfig().AssembleAndSave( hanconfig, hanhcfg )

            print('====> Executing han')
            import resource
            memlimit = resource.getrlimit(resource.RLIMIT_AS)
            resource.setrlimit(resource.RLIMIT_AS, (memlimit[1], memlimit[1]))
            hanoutput = haninput.rpartition('.')[0] + '_han.root'

            rv = ROOT.dqi.HanApp().Analyze( hanhcfg, haninput, hanoutput, '' )
    ##         rv = os.system('han.exe %s %s' % (hanhcfg, hantmpinput))
            if rv != 0:
                raise Exception('failure in han')
            hantargetdir = os.path.join(options.webdir, str(options.iteration),
                                        options.dispname, 'run_%s' % run)
            print('====> Copying to', hantargetdir)
            hantargetfile = os.path.join(hantargetdir, 'run_%s_han.root' % run)
            if not os.access(hantargetdir, os.W_OK):
                try:
                    os.makedirs(hantargetdir)
                except Exception as e:
                    print('Unable to create %s for some reason: %s' % (hantargetdir, e))
                    raise Exception('Error during execute')
            shutil.copy2(hanoutput, hantargetfile)
            print('====> Cleaning up')
        except Exception as e:
            print(e)
            if 'canonical format' not in str(e):
                failed = True
        finally:
            try:
                if not prebuilt_hcfg:
                    os.unlink(hantmpinput)
                    os.unlink(hanconfig)
                    os.unlink(hanhcfg)
                os.unlink(hanoutput)
            except Exception:
                pass
        
    return not failed

        
if __name__=="__main__":
    import sys, optparse
    parser = optparse.OptionParser(usage='usage: %prog [options] inputfile run')
    parser.add_option('--webdir', default='/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/fastphysmon',
                      help='Change directory to store web display files')
    parser.add_option('--dispname', default='Summary',
                      help='Set "stream" name on web display')
    parser.add_option('--iteration', default=1, type='int',
                      help='Set iteration number for web display')
    parser.add_option('--reffile', default=None,
                      help='Reference file to use. Must have same structure as inputfile')

    options, args = parser.parse_args()
    
    if not 2 == len(args):
        parser.print_help()
        sys.exit(1)
    fname = args[0]
    try:
        run = int(args[1])
        options.run = run
    except ValueError:
        parser.print_help()
        print('Specified run', args[1], 'doesn\'t seem to be an integer')
        sys.exit(1)

    rv = super_process(fname, options)
    if rv:
        sys.exit(0)
    else:
        sys.exit(1)    
