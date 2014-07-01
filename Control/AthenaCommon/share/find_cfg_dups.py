#!/usr/bin/env python

""" a script to try to find configurables which could be merged together
    (based on their properties)
"""

import sys
import AthenaCommon.CfgMergerLib as cfgmerger

def _make_jobo(joboname, run_full_job=False):
    import tempfile
    jobo = tempfile.NamedTemporaryFile(suffix='-jobo.py')
    import textwrap
    job = textwrap.dedent("""\
    import sys, os
    print '::: analyzing [%(joboname)s]...'
    sys.stdout = open(os.devnull,'w')
    sys.stderr = open(os.devnull,'w')
    include ('%(joboname)s')
    sys.stdout = sys.__stdout__
    sys.stderr = sys.__stderr__

    joboname = '%(joboname)s'
    import AthenaCommon.CfgMergerLib as cfg_merger
    oname = os.path.basename (joboname)
    oname = 'cfg_dup_report_'+oname.replace('/','_')+'.pkl'
    dups = cfg_merger.analyze(oname=oname)

    if %(run_full_job)s:
        oname = oname.replace('.pkl','.josvc.ascii')
        include('AthenaServices/AthenaJobOptionsSvc_jobOptions.py')
        jobo_ascii_table = oname.replace('.josvc.ascii', '.jobosvc.full.cxx.ascii')
        svcMgr.JobOptionsSvc.PythonizedCatalogue = jobo_ascii_table
        
        #theApp.run()
        #print '::: dumping jobo svc content into ['+oname+']...'
        #cfg_merger.dump_josvc_content(oname)
        #print '::: dumping jobo svc content into ['+oname+']... [done]'
        #theApp.finalize()
    else:
        theApp.exit()
    """% {'joboname':joboname,
          'run_full_job':run_full_job})
    jobo.write(job)
    jobo.flush()
    return jobo

def _run_jobo(job, run_full_job=False):

    import commands
    sc,out = commands.getstatusoutput('which athena.py')
    if sc != 0:
        raise RuntimeError("could not locate 'athena.py'")
    app = out
    jobo = _make_jobo(job, run_full_job)
    
    sc,out = commands.getstatusoutput('which sh')
    if sc != 0:
        raise RuntimeError ("could not locate 'sh'")
    sh = out

    out = []
    cmd = [sh, app, jobo.name]
    import subprocess as sub
    rc = sub.check_call(args=[sh, app, jobo.name])

    return rc

def main():
    if len(sys.argv) < 1:
        raise RuntimeError("you have to give a joboptions name")

    run_full_jobo = True
    idx = None
    for i,arg in enumerate(sys.argv):
        if '--fast' == arg:
            idx = i
            run_full_jobo = False
    if idx is not None:
        del sys.argv[idx]

    joboname = sys.argv[1]
    print "::: joboname [%s]" % joboname

    rc = _run_jobo(joboname, run_full_jobo)
    
"""
NOTES:
 AtlasTrackSummaryTool does funny things and replace the method `name` with a string attribute `name`
"""

if __name__ == "__main__":
    sc = main()
    sys.exit(sc)
    
