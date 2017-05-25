# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                   

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
            d = fout.Get('run_%d/GLOBAL/DQTGlobalWZFinder')
            d.WriteTOjbect(obj)
    fin.Close(); fout.Close()
            

def go(fname):
    import subprocess, os, shutil
    if 'DQ_STREAM' in os.environ:
        if (os.environ.get('DQPRODUCTION', '0') == '1'
            and os.environ['DQ_STREAM'] != 'physics_Main'):
            return
    if 'DISPLAY' in os.environ: del os.environ['DISPLAY']
    runno = getRun(fname)
    print 'Seen run', runno
    subprocess.call(['dqt_zlumi_compute_lumi.py', fname, '--out', 'zlumiraw.root', '--plotdir', ''])
    subprocess.call(['dqt_zlumi_alleff_HIST.py', fname, '--out', 'zlumieff.root'])
    subprocess.call(['dqt_zlumi_combine_lumi.py', 'zlumiraw.root', 'zlumieff.root', 'zlumi.root'])
    subprocess.call(['dqt_zlumi_display_z_rate.py', 'zlumi.root'])
    copyPlot('zlumi.root', fname)
    shutil.move('zlumi.root_zrate.csv', 'zrate.csv')
