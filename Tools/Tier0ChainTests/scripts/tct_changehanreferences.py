#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Replace all references in a han configuration file by a new file, then output
new configuration.
"""
#@author: ponyisi@hep.uchicago.edu
#@date: 13 Nov 2008

from DQConfMakerBase.DQElements import *
from DQConfMakerBase.Helpers import IDHelper, make_thresholds
from DQHanConfMaker.hanwriter import writeHanConfiguration
import ROOT

fcache = {}

def test_obj_existence(newref, name):
    prune, dum, tail = newref.rpartition(':')
    if prune in fcache:
        f = fcache[prune]
    else:
        print 'Opening', prune
        f = ROOT.TFile.Open(prune)
        fcache[prune] = f
    import os
    o = f.Get(os.path.join(tail, name))
    if o == None:
        #print name, 'FAIL', f, os.path.join(tail, name)
        #f.Close()
        return False
    else:
        #print name, 'PASS', f, os.path.join(tail, name)
        #f.Close()
        return True

def makeDQObject(obj, newref, prefix, opts):
    name = obj.GetName()
    if opts.hanformat:
        if prefix == '':
            newrefname = name.rpartition('/')[2]
        else:
            newrefname = prefix + '/' + name.rpartition('/')[2]
        #print newrefname
    else:
        newrefname = name
    algname = obj.GetAlgName()
    alglib = obj.GetAlgLibName()
    algref = obj.GetAlgRefName()
    if algref != '' or (opts.forceref and not isinstance(obj, ROOT.dqi.HanConfigGroup)):
        if test_obj_existence(newref, newrefname):
            algref = [DQReference(reference=newref+newrefname)]
        elif algref != '' and opts.fallback:
            algref = [DQReference(reference=sys.argv[1]+':'+algref)]
        else:
            algref = None
            print 'Unable to find a good reference for', name
    else:
        algref = None
    algpars = [DQAlgorithmParameter(name=x.GetName(), value=x.GetValue())
               for x in obj.GetAllAlgPars()]
    thresholds = [{'green': DQThreshold(name=x.GetName(), value=x.GetGreen()),
                   'red': DQThreshold(name=x.GetName(), value=x.GetRed())}
                  for x in obj.GetAllAlgLimits()]
    #alglimitsitr = obj.GetAllAlgLimits()
    annotations = obj.GetAllAnnotations()
    alg = DQAlgorithm(id=algname, libname=alglib)
    #print algref
    if isinstance(obj, ROOT.dqi.HanConfigGroup):
        dqreg = DQRegion(id=name, algorithm=alg, weight=obj.GetWeight())
        return dqreg
    else:
        dqpar = DQParameter(id=name.rpartition('/')[2], algorithm=alg,
                            algorithmparameters=algpars,
                            weight=obj.GetWeight(),
                            thresholds=thresholds, references=algref,
                            inputdatasource=name)
        anndict = {}
        for annotation in annotations:
            if annotation.GetName() == 'inputname':
                continue
            anndict[annotation.GetName()] = annotation.GetValue()
        if options.nonorm:
            if 'display' not in anndict:
                anndict['display'] = 'NoNorm'
            else:
                if 'NoNorm' not in anndict['display']:
                    anndict['display'] += ',NoNorm'
        for annkey, annval in anndict.items():
            dqpar.addAnnotation(annkey, annval)
        if obj.GetIsRegex():
            dqpar.addAnnotation('regex', 1)
        return dqpar
            

def recurse(fname, obj, dqobj, opts, prefix = ''):
    #print obj.GetName()
    if isinstance(obj, ROOT.dqi.HanConfigGroup):
        if prefix == '/top_level':
            newprefix = obj.GetName()
        else:
            newprefix = prefix + '/' + obj.GetName()
        for x in obj.GetAllAssessors():
            newdqobj = makeDQObject(x, fname, newprefix,
                                    opts)
            dqobj.addDQParameter(newdqobj)
            recurse(fname, x, newdqobj, opts, newprefix)
        for x in obj.GetAllGroups():
            newdqobj = makeDQObject(x, fname, newprefix,
                                    opts)
            dqobj.addDQRegion(newdqobj)
            recurse(fname, x, newdqobj, opts, newprefix)

def getAlgorithm(obj):
    alg = DQAlgorithm(id=obj.GetAlgName(), libname=obj.GetAlgLibName())
    return alg

def do_composites(f):
    pass
##     rv = []
##     for k in f.GetListOfKeys():
##         cl = ROOT.TClass.GetClass(k.GetClassName())
##         if cl.InheritsFrom('dqi::HanConfigCompAlg'):
##             obj = k.ReadObj()
##             print obj.GetName()
##             nobj = CompositeAlgorithm(id=k.GetName(),
##                                       subalgorithms=[DQAlgorithm(id=x) for x in 'GatherData'])

def copy_compalgs(inf, out):
    fin = ROOT.TFile(inf, 'READ')
    fout = ROOT.TFile(out, 'UPDATE')
    fout.cd()
    for k in fin.GetListOfKeys():
        cl = ROOT.TClass.GetClass(k.GetClassName())
        if cl.InheritsFrom('dqi::HanConfigCompAlg'):
            obj = k.ReadObj()
            #fout.cd()
            obj.Write()
    fin.Close()
    fout.Close()
    

if __name__ == '__main__':
    import sys, tempfile, optparse, urllib, os
    parser = optparse.OptionParser()
    parser.add_option('--hanformat', action='store_true', default=False,
                      help='Reference file is DQMF output')
    parser.add_option('--forceref', action='store_true', default=False,
                      help="Add reference to all checks, even if original didn't have one")
    parser.add_option('--fallback', action='store_true', default=False,
                      help='Fall back to original reference if new reference is not available')
    parser.add_option('--nonorm', action='store_true', default=False,
                      help='Do not normalize references')
    (options, args) = parser.parse_args()

    if len(args) != 3:
        print 'This script replaces all the reference histograms by those of the same'
        print 'name in a new file.'
        print
        print 'usage: %s <inputhcfg> <newrefpath> <outputhcfg> [--hanformat] [--forceref] [--fallback] [--nonorm]' % sys.argv[0]
        print ' --> inputhcfg and outputhcfg files are binary configurations'
        print ' --> newrefpath is the new reference file you want to use in'
        print '     the format rootfile.root:top_path'
        print '     for example newref.root:run_92226'
        print ' --> use --hanformat if the new reference file was produced by DQMF'
        print ' --> use --forceref if you want to add references to all histograms, '
        print '     not just the ones that already have references'
        print ' --> use --fallback if you want to use the existing reference if no new'
        print '     reference is available'
        print " --> use --nonorm if you don't want to normalize the references to data"
        print '     histogram area'
        sys.exit(3)
    
    if ROOT.gSystem.Load('libDataQualityInterfaces') != 0:
        print 'Cannot load libDataQualityInterfaces.so.  Perhaps you need to do some CMT magic.'
        sys.exit(4)
        
    infile = ROOT.TFile.Open(args[0], 'READ')
    if infile == None:
        print 'cannot find', args[0]
        sys.exit(1)
    topgroup = infile.Get('top_level')
    if topgroup == None:
        print 'cannot find top_level configuration in config file'
        sys.exit(2)
    topalg = getAlgorithm(topgroup)
    topreg = DQRegion(id='topRegion', algorithm=topalg)
    newref = args[1]
    urlname = newref.rsplit(':',1)[0]
    localnewreffile, dum = urllib.urlretrieve(newref.rsplit(':',1)[0])
    localnewref = newref.replace(urlname, localnewreffile)
    patchref = localnewref
    if ':' not in patchref:
        patchref += ':'
    if not options.hanformat and patchref[-1] != '/':
        patchref += '/'

    recurse(patchref, infile.Get('top_level'), topreg, options)
    do_composites(infile)
    
    sublevel = topreg.getSubRegions()[:]
    for x in sublevel:
        topreg.delRelation('DQRegions', x)

    tmp = tempfile.NamedTemporaryFile()
    writeHanConfiguration(filename=tmp.name, roots=sublevel)
    #writeHanConfiguration(filename='output.config', roots=sublevel)
    import shutil
    #shutil.copy(tmp.name, 'newc_' + args[2].replace('hcfg', 'config'))
    ROOT.dqi.HanConfig().AssembleAndSave( tmp.name, args[2] )
    tmp.close()
    copy_compalgs(args[0], args[2])
    if localnewref != newref:
        os.unlink(localnewreffile)
