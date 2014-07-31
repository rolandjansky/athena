#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Replace all algorithms in a han configuration file by a new one, then output
new binary configuration file.
"""
#@author: ponyisi@hep.uchicago.edu
#@date: 13 Nov 2008

from DQConfMakerBase.DQElements import *
from DQConfMakerBase.Helpers import IDHelper, make_thresholds
from DQHanConfMaker.hanwriter import writeHanConfiguration
import ROOT

def makeDQObject(obj, newalg, opts):
    name = obj.GetName()
    #algname = obj.GetAlgName()
    #alglib = obj.GetAlgLibName()
    #algref = obj.GetAlgRefName()
    annotations = obj.GetAllAnnotations()
    #print algref
    if opts.keepweights:
        weight = obj.GetWeight()
    else:
        weight = 1
    if isinstance(obj, ROOT.dqi.HanConfigGroup):
        if opts.keepsummaries:
            alg = getAlgorithm(obj)
        else:
            alg = newalg.summaryalg(obj)
        dqreg = DQRegion(id=name, algorithm=alg, weight=weight)
        return dqreg
    else:
        newalgparm = newalg.alg(obj)
        alg = newalgparm[0]
        algpars = newalgparm[1]
        thresholds = newalgparm[2]
        algref = newalgparm[3]
        if algref == None:
            if obj.GetAlgRefName() != '':
                algref = [DQReference(reference=options.fname + ':'+obj.GetAlgRefName())]
            else:
                algref = None
        dqpar = DQParameter(id=name.rpartition('/')[2], algorithm=alg,
                            algorithmparameters=algpars,
                            weight=weight,
                            thresholds=thresholds, references=algref,
                            inputdatasource=name)
        for annotation in annotations:
            if annotation.GetName() == 'inputname':
                continue
            dqpar.addAnnotation(annotation.GetName(), annotation.GetValue())
        if obj.GetIsRegex():
            dqpar.addAnnotation('regex', 1)
        return dqpar
            

def recurse(newalg, obj, dqobj, opts):
    #print obj.GetName()
    if isinstance(obj, ROOT.dqi.HanConfigGroup):
        for x in obj.GetAllAssessors():
            newdqobj = makeDQObject(x, newalg, opts)
            dqobj.addDQParameter(newdqobj)
            recurse(newalg, x, newdqobj, opts)
        for x in obj.GetAllGroups():
            newdqobj = makeDQObject(x, newalg, opts)
            dqobj.addDQRegion(newdqobj)
            recurse(newalg, x, newdqobj, opts)

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
    import sys, tempfile, optparse
    parser = optparse.OptionParser()
    parser.add_option('--keepweights', action='store_true', default=False,
                      help='Keep original check weights from file; default resets them to 1')
    parser.add_option('--keepsummaries', action='store_true', default=False,
                      help='Keep original summary algorithms; default changes them using summaryalg()')
    (options, args) = parser.parse_args()
    options.fname = args[0]

    if len(args) != 3:
        print 'This script replaces all the reference histograms by those of the same'
        print 'name in a new file.'
        print
        print 'usage: %s <inputhcfg> <newalgpy> <outputhcfg> [--keepweights] [--keepsummaries]' % sys.argv[0]
        print ' --> inputhcfg and outputhcfg files are binary configurations'
        print ' --> newalgpy is a python file with an alg() function that'
        print '     returns a configured algorithm in the DQConfMakerSense'
        print ' --> use --keepweights to keep the weights in the original configuration,'
        print '     otherwise they will be reset to 1'
        print ' --> use --keepsummaries to keep the original summary algorithms,'
        print '     otherwise a summaryalg() function is expected in newalgpy'
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
    newalgpy = args[1]

    if newalgpy[-3:] == '.py':
        newalgpy = newalgpy[:-3]

    newalg = __import__(newalgpy, fromlist=['alg'])

    recurse(newalg, infile.Get('top_level'), topreg, options)
    #do_composites(infile)
    
    sublevel = topreg.getSubRegions()[:]
    for x in sublevel:
        topreg.delRelation('DQRegions', x)

    tmp = tempfile.NamedTemporaryFile()
    writeHanConfiguration(filename=tmp.name, roots=sublevel)
    #writeHanConfiguration(filename='output.config', roots=sublevel)
    #import shutil
    #shutil.copy(tmp.name, 'newc_' + args[2].replace('hcfg', 'config'))
    ROOT.dqi.HanConfig().AssembleAndSave( tmp.name, args[2] )
    tmp.close()
    #copy_compalgs(args[0], args[2])
