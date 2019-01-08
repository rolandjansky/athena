# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# @file    PyDumper.SgDumpLib
# @purpose API for the sg-dump script
# @author  Sebastien Binet <binet@cern.ch>
# @date    August 2009

from __future__ import with_statement
import sys
import os

__doc__ = """\
API for the sg-dump script (which dumps an ASCII representation of events in
POOL or RAW files
"""
__author__ = "Sebastien Binet <binet@cern.ch>"
__version__= "$Revision$"

__all__ = [
    'run_sg_dump',
    ]

def _make_jobo(job):
    import tempfile
    jobo = tempfile.NamedTemporaryFile(suffix='-jobo.py')
    import textwrap
    job = textwrap.dedent (job)
    jobo.writelines([l+os.linesep for l in job.splitlines()])
    jobo.flush()
    return jobo

def _gen_jobo(dct):
    import textwrap
    if dct['input-type'] == 'ANY':
        job = textwrap.dedent("""\
        # automatically generated joboptions file

        # percolate through the autoconfiguration...

        # input files configuration
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
        _input_files = %(input-files)s
        acf.FilesInput = _input_files
        del _input_files

        from RecExConfig.InputFilePeeker import inputFileSummary
        file_geo = inputFileSummary.get('geometry')
        if file_geo and file_geo.find('-GEO-') >= 0:
            inputFileSummary['geometry'] = 'ATLAS-R1-2012-02-00-00'

        from RecExConfig.RecFlags import rec
        rec.AutoConfiguration = ['everything']
        import RecExConfig.AutoConfiguration as auto
        auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


        # import the rec flags...
        from RecExConfig.RecFlags import rec
        # check we have been configured to read...
        for item in ('readRDO',
                     #'readBS' ?!?!
                     'readESD',
                     'readAOD',
                     'readTAG',
                     ):
            value = getattr (rec, item)()
            getattr (rec, item).set_Value_and_Lock(value)

        for item in ('doCBNT', 'doWriteBS',
                     'doWriteRDO', 'doWriteESD',
                     'doWriteAOD', 'doWriteTAG', 'doWriteTAGCOM',
                     'doESD',
                     'doAOD',
                     'doDPD',
                     'doEgamma',  # avoid dict clash
                     ):
            getattr (rec, item).set_Value_and_Lock(False)

        # disable the time consuming stuff we don't care about
        for item in ('doDumpTDS', 'doDumpTES',
                     'doMonitoring',
                     'doHist',
                     'doNameAuditor', 'doDetailedAuditor',
                     'doSGAuditor',
                     'doPerfMon', 'doDetailedPerfMon',
                     ):
            getattr (rec, item).set_Value_and_Lock(False)

        # events to process
        acf.EvtMax = %(evts)s
        acf.SkipEvents = %(skip)s

        # prevent AthFile from using the cache
        import PyUtils.AthFile as af
        af.server.flush_cache()
        try:
            af.server.disable_pers_cache()
        except AttributeError: # backward compat...
            pass
            
        # main jobos
        include ('RecExCond/RecExCommon_flags.py')
        # FIXME: work-around for bug #56185
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.makeRIO.all_setOff()
        # FIXME -- end
        include ('RecExCommon/RecExCommon_topOptions.py')

        svcMgr.GeoModelSvc.IgnoreTagDifference = True

        # adding the python dumper algorithm
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()
        from %(pyalg_pkg)s import %(pyalg_cls)s as pyalg
        job += pyalg('pyalg',
                     ofile='%(ofile-name)s',
                     exclude='%(exclude)s',
                     OutputLevel=Lvl.INFO)
        """) % dct
    else:
        dct['is-input-pool'] = dct['input-type'] != 'BS'
        job = textwrap.dedent("""\
        # automatically generated joboptions file

        # input files configuration
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
        _input_files = %(input-files)s
        acf.FilesInput = _input_files
        del _input_files

        from RecExConfig.InputFilePeeker import inputFileSummary
        file_geo = inputFileSummary.get('geometry')
        if file_geo and file_geo.find('-GEO-') >= 0:
            inputFileSummary['geometry'] = 'ATLAS-R1-2012-02-00-00'

        # disable (most of) auto-cfg
        #from RecExConfig.RecFlags import rec
        #rec.AutoConfiguration.set_Value_and_Lock([
        #    'ProjectName',
        #    'RealOrSim',
        #    'FieldAndGeo',
        #    'BeamType',
        #    'ConditionsTag',
        #    'TriggerStream',
        #    'LumiFlags',
        #    ])
        
        # import the rec flags...
        from RecExConfig.RecFlags import rec
        for item in ('doCBNT', 'doWriteBS',
                     'doWriteRDO', 'doWriteESD',
                     'doWriteAOD', 'doWriteTAG', 'doWriteTAGCOM',
                     'doESD', 'doAOD',
                     'doEgamma',  # avoid dict clash
                     ):
            getattr(rec, item).set_Value_and_Lock(False)

        # disable the time consuming stuff we don't give a damn about
        for item in ('doDumpTDS', 'doDumpTES',
                     'doMonitoring',
                     'doHist',
                     'doNameAuditor', 'doDetailedAuditor',
                     'doSGAuditor',
                     'doPerfMon', 'doDetailedPerfMon',
                     ):
            getattr(rec, item).set_Value_and_Lock(False)

        # tell RecExCommon which input file(s) type we are dealing with
        #getattr (rec, 'read%(input-type)s').set_Value_and_Lock(True)
        globals()['read%(input-type)s'] = True

        # input files configuration
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
        _input_files = %(input-files)s
        if %(is-input-pool)s:
            globalflags.InputFormat = 'pool'
            _item = 'Pool%(input-type)sInput'
            globals()[_item] = _input_files
            setattr(acf, _item, _input_files)
            del _item
        else:
            globalflags.InputFormat = 'bytestream'
            BSRDOInput = _input_files
            getattr(acf, 'BSRDOInput').set_Value_and_Lock(_input_files)
            getattr(acf, 'PoolRDOInput').set_Value_and_Lock([])
            rec.readRDO.set_Value_and_Lock(True)
            readRDO = True
        del _input_files

        # events to process
        acf.EvtMax = %(evts)s
        acf.SkipEvents = %(skip)s
    
        # prevent AthFile from using the cache
        import PyUtils.AthFile as af
        af.server.flush_cache()
        try:
            af.server.disable_pers_cache()
        except AttributeError: # backward compat...
            pass
        
        # main jobos
        include ('RecExCond/RecExCommon_flags.py')
        include ('RecExCommon/RecExCommon_topOptions.py')

        svcMgr.GeoModelSvc.IgnoreTagDifference = True

        # adding the python dumper algorithm
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()
        from %(pyalg_pkg)s import %(pyalg_cls)s as pyalg
        job += pyalg('pyalg',
                     ofile='%(ofile-name)s',
                     exclude='%(exclude)s',
                     OutputLevel=Lvl.INFO)
        """) % dct
        
    return job

def _run_jobo(job, msg, options):
    import os,atexit,tempfile,shutil,glob
    # capture current directory's content
    keep_files = [os.path.abspath(item)
                  for item in os.listdir(os.getcwd())]
    keep_files.append (os.path.abspath(options.oname))
    keep_files.append (os.path.abspath("%s.log"%options.oname))
    
    def _cleanup(keep_files):
        errors = []
        for item in os.listdir(os.getcwd()):
            item = os.path.abspath(item)
            if os.path.basename(item).startswith(('.__afs',
                                                  '.__nfs')):
                # don't care about freakingly sticky metadata files
                continue
            
            if item in keep_files:
                continue
            try:
                if   os.path.isfile (item): os.remove (item)
                elif os.path.islink (item): os.unlink (item)
                elif os.path.isdir  (item): shutil.rmtree (item)
                else:
                    msg.warning ("don't know what kind of stuff this is: %s",
                                 item)
            except Exception, err:
                errors.append ("%s"%err)
                pass
        if len(errors)>0:
            msg.error ("problem during workdir clean-up")
            map (msg.error, errors)
        else:
            msg.debug ("workdir clean-up [ok]")
        return

    if options.do_clean_up:
        atexit.register (_cleanup, keep_files)
    
    import commands
    sc,out = commands.getstatusoutput ('which athena.py')
    if sc != 0:
        msg.error("could not locate 'athena.py':\n%s", out)
        return sc, out
    app = out
    jobo = _make_jobo(job)

    if options.use_recex_links:
        sc,out = commands.getstatusoutput ('RecExCommon_links.sh')
        if sc != 0:
            msg.error("could not run 'RecExCommon_links.sh':\n%s"%out)
            return sc, out
        msg.info ('installed RecExCommon links')
    
    sc,out = commands.getstatusoutput ('which sh')
    if sc != 0:
        msg.error("could not locate 'sh':\n%s",out)
        return sc, out
    sh = out

    from time import time
    logfile = tempfile.NamedTemporaryFile(prefix='sg_dumper_job_',
                                          suffix='.logfile.txt',
                                          dir=os.getcwd())

    # do not require $HOME to be available for ROOT
    # see bug #82096
    # https://savannah.cern.ch/bugs/index.php?82096
    env = dict(os.environ)
    env['ROOTENV_NO_HOME'] = os.getenv('ROOTENV_NO_HOME', '1')
    
    out = []
    athena_opts = []
    if options.athena_opts:
        import shlex
        athena_opts = shlex.split(options.athena_opts)
    cmd = [sh, app,] + athena_opts + [jobo.name,]
    import subprocess as sub
    app_handle = sub.Popen (args=cmd,
                            stdout=logfile,
                            stderr=logfile,
                            env=env)
    pos = 0
    import time, re
    pat = re.compile (r'^Py:pyalg .*')
    evt_pat = re.compile (
        r'^Py:pyalg .*? ==> processing event [[](?P<evtnbr>\d*?)[]].*'
        )
    def _monitor(pos):
        logfile.flush()
        _watcher = open (logfile.name, 'r'); _watcher.seek (0, 2) # end of file
        end = _watcher.tell();               _watcher.seek (pos)
        mon = [l for l in _watcher
               if pat.match(l)]
        _watcher.seek (end)
        pos = _watcher.tell()

        for l in mon:
            if l.count ('==> initialize...'):
                msg.info ('athena initialized')
            if evt_pat.match(l):
                ievt = evt_pat.match(l).group('evtnbr')
                out.append(ievt)
                msg.info ('processed event [%s]', ievt)
            if l.count ('==> finalize...'):
                msg.info ('athena finalized')
                
        return pos

    while app_handle.poll() is None:
        pos = _monitor(pos)
        time.sleep (5)
        pass
    _monitor(pos)
    
    jobo.close()
    sc = app_handle.returncode
    if sc != 0:
        logfile.seek(0)
        msg.error ('='*80)
        from cStringIO import StringIO
        err = StringIO()
        for l in logfile:
            print l,
            print >> err, l,
        msg.error ('='*80)
        msg.error ('problem running jobo')
        return sc, err.getvalue()

    logfile.seek (0)
    from cStringIO import StringIO
    out = StringIO()
    for l in logfile:
        if pat.match(l):
            print >> out, l,
    return sc, out.getvalue()

def run_sg_dump(files, output,
                nevts=-1,
                skip=0,
                dump_jobo=False,
                use_recex_links=True,
                pyalg_cls='PyDumper.PyComps:PySgDumper',
                exclude='',
                file_type=None,
                do_clean_up=False,
                athena_opts=None,
                msg=None):
    """API for the sg-dump script.
     `files` a list of input filenames to be dumped by SgDump
     `output` the name of the output (ASCII) file
     `nevts`  the number of events to dump (default: -1 ie all)
     `skip`   the number of events to skip at the start (default: 0)
     `dump_jobo` switch to store or not the automatically generated jobo (put
                 the name of the jobo output name in there if you want to keep
                 it)
     `use_recex_links` switch to run RecExCommon_links and thus a local db replica
     `pyalg_cls` the fully qualified name of the PyAthena.Alg class to process the file(s) content (PySgDumper or DataProxyLoader)
     `exclude`: comma-separated list of glob patterns for keys/types to ignore.
     `file_type` the input file's type (RDO,BS,ESD,AOD,DPD or ANY)
     `do_clean_up` flag to enable the attempt at removing all the files sg-dump
                   produces during the course of its execution
     `athena_opts` a space-separated list of athena command-line options (e.g '--perfmon --stdcmalloc --nprocs=-1')
     `msg`    a logging.Logger instance

     returns the exit code of the sub-athena process
    """

    if msg is None:
        import PyUtils.Logging as L
        msg = L.logging.getLogger('sg-dumper')
        msg.setLevel(L.logging.INFO)

    if isinstance(files, basestring):
        files = files.split()
        
    if not isinstance(files, (list,tuple)):
        err = "'files' needs to be a list (or tuple) of file names"
        msg.error(err)
        raise TypeError(err)

    if not isinstance(output, basestring):
        err = "'output' needs to be a filename"
        msg.error(err)
        raise TypeError(err)

    _allowed_values = ('PyDumper.PyComps:PySgDumper',
                       'PyDumper.PyComps:DataProxyLoader')
    if not (pyalg_cls in _allowed_values):
        err = "'pyalg_cls' allowed values are: %s. got: [%s]" % (
            _allowed_values,
            pyalg_cls)
        msg.error(err)
        raise ValueError(err)
    pyalg_pkg,pyalg_cls = pyalg_cls.split(':')

    _allowed_values = ('rdo', 'bs', 'esd', 'aod', 'dpd', 'any')
    if file_type is None:
        file_type = 'any'
    if not (file_type.lower() in _allowed_values):
        err = "'file_type' allowed values are: %s. got: [%s]" % (
            _allowed_values,
            file_type)
        msg.error(err)
        raise ValueError(err)
    file_type = file_type.lower()
    
    jobo = _gen_jobo({
        'ofile-name' : output,
        'input-files': files,
        'evts' :       nevts,
        'skip' :       skip,
        'exclude' :    exclude,
        'pyalg_pkg':   pyalg_pkg,
        'pyalg_cls':   pyalg_cls,
        'input-type':  file_type.upper(),
        })

    msg.info(':'*40)
    msg.info('input files:     %s', files)
    msg.info('events:          %s', nevts)
    msg.info('skip:            %s', skip)
    msg.info('out (ascii):     %s', output)
    msg.info('use recex links: %s', use_recex_links)
    msg.info('pyalg-class:     %s:%s', pyalg_pkg, pyalg_cls)
    msg.info('file_type:       %s', file_type)
    msg.info('exclude:         %s', exclude)
    
    if dump_jobo and isinstance(dump_jobo, basestring):
        try:
            with open(dump_jobo, 'w') as f:
                f.write(jobo)
        except Exception, err:
            msg.warning('problem while dumping joboption file to [%s]:\n%s',
                        dump_jobo, err)

    from collections import namedtuple
    Options = namedtuple('Options',
                         'oname use_recex_links do_clean_up athena_opts')
    opts = Options(oname=output,
                   use_recex_links=use_recex_links,
                   do_clean_up=do_clean_up,
                   athena_opts=athena_opts)
    
    sc, out = 1, "<N/A>"
    msg.info('running dumper...')
    sc,out = _run_jobo(jobo, msg, opts)
    msg.info('dumper done')
    if output != os.devnull:
        msg.info('writing logfile: %s.log', output)
        try:
            with open('%s.log'%output, 'w') as f:
                for l in out.splitlines():
                    print >> f, l
                print >> f, "### EOF ###"
            
        except Exception, err:
            msg.warning('problem writing out logfile [%s.log]:\n%s',
                        output, err)

    msg.info('bye.')
    msg.info(':'*40)
    return sc, out
