#! /usr/bin/env python
from __future__ import with_statement
import sys
import os

def _make_jobo(job):
    import tempfile
    jobo = tempfile.NamedTemporaryFile(suffix='-jobo.py')
    import textwrap
    job = textwrap.dedent (job)
    jobo.writelines([l+os.linesep for l in job.splitlines()])
    jobo.flush()
    return jobo

def _run_jobo(job, msg, logfile_name=None):
    import os,atexit,tempfile,shutil,glob,time
    import commands
    sc,out = commands.getstatusoutput ('which athena.py')
    if sc != 0:
        raise RuntimeError ("could not locate 'athena.py'")
    app = out
    jobo = _make_jobo(job)

    sc,out = commands.getstatusoutput ('which sh')
    if sc != 0:
        raise RuntimeError ("could not locate 'sh'")
    sh = out

    if logfile_name is None:
        import os, tempfile
        logfile = tempfile.NamedTemporaryFile (prefix='test_pickling_job_',
                                               suffix='.logfile.txt',
                                               dir=os.getcwd())
    else:
        import os
        if os.path.exists (logfile_name):
            os.remove (logfile_name)
        logfile = open (logfile_name, "w+")

    out = []
    cmd = [sh, app, jobo.name]
    import subprocess as sub
    app_handle = sub.Popen (args=[sh, app, jobo.name],
                            stdout=logfile,
                            stderr=logfile)
    while app_handle.poll() is None:
        time.sleep (5)
        pass
    
    jobo.close()
    sc = app_handle.returncode
    if sc != 0:
        logfile.seek(0)
        for l in logfile: msg.error (l.rstrip())
        msg.error ('='*80)
        msg.error ('problem running jobo')
        raise SystemExit (2)

    logfile.seek (0)
    out = [l.rstrip() for l in logfile]
    return out

def main():
    from optparse import OptionParser
    parser = OptionParser (usage="usage: %prog [options] jobo.py")
    p = parser.add_option
    p("-o",
      "--output",
      dest    = "output",
      default = None,
      help    = "path to a file where to store log's content")
    p("-s", "--save",
      dest    = "pickle",
      default = "my.cfg.pkl",
      help    = "path to a file where to save the generated cfg-pickle")

    (options, args) = parser.parse_args()

    if len (args) > 0:
        jobo_name = [arg for arg in args if arg[0] != "-"]
        pass
    else:
        parser.print_help()
        print "you need to give a joboption name to execute !"
        sys.exit (1)
        pass
    
    from AthenaCommon.Logging import logging
    msg = logging.getLogger ('pickling-cfg-test')
    msg.setLevel (logging.INFO)

    jobo = """\
    for jobo_name in %s:
        include (jobo_name)
    from AthenaCommon.ConfigurationShelve import saveToPickle
    saveToPickle (fileName='%s')
    theApp.exit(0)
    """ % (jobo_name, options.pickle)

    msg.info ("creating pickle file from jobo '%s'...", jobo_name)
    msg.info ("storing configuration into [%s]...", options.pickle)
    wout = _run_jobo (jobo, msg, logfile_name=None)

    jobo = """\
    from AthenaCommon.ConfigurationShelve import loadFromPickle
    loadFromPickle (fileName='my.cfg.pkl')
    """
    msg.info ("trying to load back pickle...")
    msg.info ("logging process into [%s]...", options.output)
    rout = _run_jobo (jobo, msg, logfile_name=options.output)

    msg.info ("bye.")
    return

if __name__ == "__main__":
    main()
    
