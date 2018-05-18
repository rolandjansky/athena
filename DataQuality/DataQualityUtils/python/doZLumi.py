# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                   

def getRun(fname):
    import ROOT
    fin = ROOT.TFile.Open(fname)
    if not fin: return None
    runname = None
    for key in fin.GetListOfKeys():
        if key.GetName().startswith('run_'):
            runname = key.GetName()
            break
    if runname: return int(runname[4:])
    else: return None

def copyPlot(infname, outfname):
    import ROOT
    run = getRun(outfname)
    if not run: return
    fin = ROOT.TFile.Open(infname, 'READ')
    if not fin: return
    fout = ROOT.TFile.Open(outfname, 'UPDATE')
    if not fout: return
    for objname in ['z_lumi']:
        obj = fin.Get(objname)
        if obj:
            d = fout.Get('run_%d/GLOBAL/DQTGlobalWZFinder' % run)
            if d:
                d.WriteTObject(obj)
    fin.Close(); fout.Close()
            
def makeGRL(run, defect, fname):
    from ROOT import TString
    import DQUtils, DQDefects
    import os

    tag = 'HEAD'
    runs = [run]
    print 'Query run information...',
    from DQUtils.db import fetch_iovs
    dbinstance = 'CONDBR2'
    eor = fetch_iovs('EOR', (min(runs) << 32) | 1,
                     (max(runs) << 32) | 0xFFFFFFFF,
                     with_channel=False, what=[], database='COOLONL_TDAQ/%s' % dbinstance)
    eor = eor.trim_iovs
    eor = DQUtils.IOVSet(iov for iov in eor if iov.since.run in runs)
    print 'done'
    print 'Query defects...',
    ddb = DQDefects.DefectsDB('COOLOFL_GLOBAL/%s' % dbinstance, tag=tag)
    ignores = set([_ for _ in ddb.defect_names if 'UNCHECKED' in _])
    try:
        defectiovs = ddb.retrieve(since = min(runs) << 32 | 1,
                                  until = max(runs) << 32 | 0xffffffff,
                                  channels = [defect],
                                  evaluate_full = False,
                                  ignore=ignores)
    except Exception, e:
        print e
        raise
    print 'Doing exclusions...',
    okiovs = eor.logical_and(eor, defectiovs.logical_not())
    print 'done'

    print 'Generating GRL...',
    data = DQUtils.grl.make_grl(okiovs, '', '2.1')
    with open(fname, 'w') as outf:
        outf.write(data)

def go(fname):
    import subprocess, os, shutil
    if 'DQ_STREAM' in os.environ:
        if (os.environ.get('DQPRODUCTION', '0') == '1'
            and os.environ['DQ_STREAM'] != 'physics_Main'):
            return
    if 'DISPLAY' in os.environ: del os.environ['DISPLAY']
    runno = getRun(fname)
    print 'Seen run', runno
    grlcmd = []
    if runno >= 325000:
        makeGRL(runno, 'PHYS_StandardGRL_All_Good', 'grl.xml')
        grlcmd = ['--grl', 'grl.xml']
    else:
        print 'Run number', runno, 'not 2017 data'

    subprocess.call(['dqt_zlumi_compute_lumi.py', fname, '--out', 'zlumiraw.root', '--dblivetime', '--plotdir', ''] + grlcmd)
    subprocess.call(['dqt_zlumi_alleff_HIST.py', fname, '--out', 'zlumieff.root', '--plotdir', ''])
    subprocess.call(['dqt_zlumi_combine_lumi.py', 'zlumiraw.root', 'zlumieff.root', 'zlumi.root'])
    subprocess.call(['dqt_zlumi_display_z_rate.py', 'zlumi.root', '--plotdir', ''])
    copyPlot('zlumi.root', fname)
    if os.path.isfile('zlumi.root_zrate.csv'):
        shutil.move('zlumi.root_zrate.csv', 'zrate.csv')
