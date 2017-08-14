#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Front-end script to run JobRunner jobs

__authors__  = ['Juerg Beringer', 'Carl Suster']
__version__ = 'runJobs.py atlas/athena'
__usage__   = """%prog [options] JOBOPTIONTEMPLATE DATASET TASK

Templates: - InDetBeamSpotExample/VertexTemplate.py
           - InDetBeamSpotExample/*Template.py
           - your own template file

See the comment field of jobs (e.g. MON.DB_BEAMSPOT jobs) in the beam spot
summary webpage for real usage examples.
"""

import sys
import glob
import os
import re
import subprocess
import InDetBeamSpotExample
from InDetBeamSpotExample import TaskManager
from InDetBeamSpotExample import DiskUtils

def extract_file_list_legacy(inputdata, options):
    ''' Reads several legacy options to work out what input data to use. '''
    if options.legacy_griduser:
        fs = [inputdata]
    elif options.legacy_fromcastor:
        # INPUTDATA specifies a directory with files
        pattern = options.legacy_filter or (None if options.bytestream else '.*ESD.*')
        fs = DiskUtils.FileSet.from_directory(inputdata).matching(pattern)
    elif os.path.isfile(inputdata):
        # inputdata is a text file with filenames
        fs = DiskUtils.FileSet.from_file_containing_list(inputdata)
    elif options.legacy_runoverdpd:
        # INPUTDATA is filename
        rundir = os.path.join(os.getcwd(), dsname)
        if not os.path.exists(rundir):
            fail('Run ' + dsname + ' (directory ' + rundir + ') not found')
        dpddir = os.path.join(rundir, inputdata)
        if not os.path.exists(dpddir):
            fail('Dataset with name ' + inputdata + ' (directory ' + dpddir + ') not found')
        fs = DiskUtils.FileSet.from_glob(os.path.join(dpddir, '*', '*-dpd.root*'))
    else:
        # INPUTDATA is a directory with files
        pattern = options.legacy_filter or '*.root*'
        fs = DiskUtils.FileSet.from_glob(os.path.join(inputdata, pattern))
    return list(sorted(fs))

def extract_file_list(options):
    ''' Use new flags to work out input file list. '''
    if options.in_directory:
        fs = DiskUtils.FileSet.from_directory(options.in_directory)
    elif options.in_list:
        fs = DiskUtils.FileSet.from_file_containing_list(options.in_list)
    return list(sorted(fs
            .matching(options.f_match)
            .excluding(options.f_exclude)
            ))

def process_flags(options, legacy=False):
    flags = {
            'inputds' : '',
            'bytestream' : options.bytestream,
            'DataSource' : 'geant4' if options.is_mc else 'data',
            'evtmax' : options.evtmax,
            'maxjobs' : options.maxjobs,
            'outputlevel' : options.outputlevel,
            'logmail' : options.users,
            'alignmentfile' : options.alignmentfile,
            'beamspotfile' : options.beamspotfile,
            'autoconfparams' : options.autoconfparams,
            'taskpostprocsteps' : options.postprocsteps,
            'filesperjob' : options.nfiles,
            'lbperjob' : options.lbperjob,
            'batchqueue' : options.batchqueue,
            'gridsite' : options.gridsite,
            'addinputtopoolcatalog' : not (options.bytestream or options.submit == 'grid' or options.legacy_griduser),
            }

    # List of desired output files. For grid jobs, it must specify exactly
    # the expected files (otherwise the grid jobs will fail or not return the
    # desired output). For all other jobs, it can be an inclusive list of
    # possible outputs and JobRunner will return only the actually present
    # output files when asked for output files.
    if options.outputfilelist:
        flags['outputfilelist'] = [ f.strip() for f in options.outputfilelist.split(',') ]
    else:
        flags['outputfilelist'] = ['dpd.root', 'nt.root', 'monitoring.root', 'beamspot.db']

    if legacy:
        flags['griduser'] = options.legacy_griduser
    else:
        flags['griduser'] = '.'.join(['user', options.grid_user or os.getenv('USER')])

    if options.legacy_runoverdpd and not options.lbperjob:
        flags['maxjobs'] = 1

    for s in options.params.split(', '):
        if s:
            try:
                p = s.split('=', 1)
                flags[p[0].strip()] = eval(p[1].strip())
            except:
                print '\nERROR parsing user parameter', p, '- parameter will be ignored'

    return flags

def make_runner(runner_type, flags):
    runner_class = InDetBeamSpotExample.loadClass(runner_type)
    return runner_class(**flags)

if __name__ == '__main__':
    cmd = subprocess.list2cmdline(sys.argv)

    from optparse import OptionParser, OptionGroup
    parser = OptionParser(usage=__usage__, version=__version__)
    parser.add_option('', '--bytestream', dest='bytestream', action='store_true', default=False,
            help='input files are bytestream instead of ROOT/POOL files')
    parser.add_option('-m', '--mc', dest='is_mc', action='store_true', default=False,
            help='input data is from Monte-Carlo instead of data (automatically chooses between COMP200 and OFLP200 / CONDBR2 conditions DBs)')
    parser.add_option('-j', '--maxjobs', dest='maxjobs', type='int', default=0,
            help='max number of jobs (default: 0 ie no maximum)')
    parser.add_option('', '--files-per-job', dest='nfiles', type='int', default=1, metavar='N',
            help='number of files per job (default: 1, set to 0 for single job over all files)')
    parser.add_option('-e', '--maxevents', dest='evtmax', type='int', default=-1,
            help='max number of events per job')
    parser.add_option('', '--lbperjob', dest='lbperjob', type='int', default=0, metavar='N',
            help='number of luminosity blocks per job (default: 0 - no bunching)')
    parser.add_option('-o', '--outputfilelist', dest='outputfilelist', default='', metavar='FILES',
            help='list of desired output files (default: "dpd.root,nt.root,monitoring.root,beamspot.db"; must be specified explicitly for grid)')
    parser.add_option('-k', '--taskdb', dest='taskdb', default='',
            help='TaskManager database (default: from TASKDB or sqlite_file:taskdata.db; set to string None to avoid using a task database)')
    parser.add_option('-l', '--logmail', dest='users', default='', metavar='USERS',
            help='send log mail to specified users (default: no mail)')
    parser.add_option('-z', '--postprocsteps', dest='postprocsteps', default='JobPostProcessing', metavar='STEPS',
            help='Task-level postprocessing steps (Default: JobPostProcessing)')
    parser.add_option('-t', '--test', dest='testonly', action='store_true', default=False,
            help='show only options and input files')
    parser.add_option('-v', '--verbosity', dest='outputlevel', type='int', default=4, metavar='LEVEL',
            help='output level (default:4, where 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)')
    parser.add_option('-p', '--params', dest='params', default='',
            help='job option parameters to pass to job option template')
    parser.add_option('', '--autoconfparams', dest='autoconfparams', default='DetDescrVersion',
            help='comma-separated list of automatically determined parameters (template must include AutoConfFragment.py, default: "DetDescrVersion")')

    # Additional optional files requiring special treatment (other parameters
    # should be passed to the job option template via "-p params")
    parser.add_option('-a', '--alignment-file', dest='alignmentfile', default='', metavar='FILE',
            help='alignment file (default: none)')
    parser.add_option('-b', '--beamspot-file', dest='beamspotfile', default='', metavar='FILE',
            help='beam spot SQLite file (default: none)')

    execopt = OptionGroup(parser, 'Execution Options')
    execopt.add_option('', '--submit', dest='submit', default='lsf', metavar='TYPE',
            choices=['grid', 'lsf', 'shell', 'bg', 'pdsf', 'simple'],
            help='submission type (default: lsf, choices: grid,lsf,shell,bg,pdsf,simple)')
    execopt.add_option('', '--grid-user', dest='grid_user', default=None, metavar='USER',
            help='grid username (default: $USER)')
    execopt.add_option('', '--grid-site', dest='gridsite', default='AUTO', metavar='SITE',
            help='site name where jobs are sent (default: AUTO)')
    execopt.add_option('', '--lsf-submit', dest='lsf_submit', default=False, action='store_true',
            help='submit on the LSF batch system')
    execopt.add_option('-q', '--queue', dest='batchqueue', default='atlasb1',
            help='batch queue (default: atlasb1)')
    parser.add_option_group(execopt)

    inopt = OptionGroup(parser, 'Input File Options',
            "One of these must be specified.")
    inopt.add_option('', '--directory', dest='in_directory', metavar='DIR',
            help='run over all matching files in the directory')
    inopt.add_option('', '--file-list', dest='in_list', metavar='FILE',
            help='run over all matching files in the directory')
    inopt.add_option('', '--dsid', dest='in_dsid', metavar='DSID',
            help='run over a rucio DSID')
    parser.add_option_group(inopt)

    filtopt = OptionGroup(parser, 'Input Filtering Options',
            "Optional filters to select input files.")
    inopt.add_option('', '--match', dest='f_match', default=None, metavar='REGEX',
            help='keep only files matching the pattern')
    inopt.add_option('', '--exclude', dest='f_exclude', default=None, metavar='REGEX',
            help='skip files matching the pattern')
    parser.add_option_group(filtopt)

    # TODO Check if these flags can be removed:
    deprecated = OptionGroup(parser, 'Deprecated Options')
    deprecated.add_option('-c', '--castor', dest='legacy_fromcastor', action='store_true', default=False,
            help='INPUTDATA refers to CASTOR directory')
    deprecated.add_option('', '--prefix', dest='legacy_prefix', default='',
            help='Prefix for reading files from mass storage (ignored)')
    deprecated.add_option('-d', '--dpd', dest='legacy_runoverdpd', action='store_true', default=False,
            help='run over DPD (single job, INPUTDATA is DPD task name)')
    deprecated.add_option('-i', '--interactive', dest='legacy_interactive', action='store_true', default=False,
            help='run interatively (same as -r JobRunner)')
    deprecated.add_option('-f', '--filter', dest='legacy_filter', default='',
            help='use specified pattern to filter input files (default: *.root* for local files, .*ESD.* for castor)')
    deprecated.add_option('-g', '--grid', dest='legacy_griduser', default='',
            help='run on grid (GRIDUSER is user prefix of grid job name, e.g. user09.JuergBeringer; INPUTDATA is grid dataset name)')
    deprecated.add_option('-s', '--gridsite', dest='gridsite', default='AUTO',
            help='deprecated spelling of --grid-site')
    deprecated.add_option('-r', '--runner', dest='legacy_runner', default='LSFJobRunner',
            help='type of JobRunner (default: LSFJobRunner or PandaJobRunner)')
    deprecated.add_option('-w', '--wait', dest='legacy_dowait', action='store_true', default=False,
            help='wait for jobs to complete')
    deprecated.add_option('-n', '--nfiles', dest='nfiles', type='int',
            help='deprecated spelling of --files-per-job')
    parser.add_option_group(deprecated)

    (opts,args) = parser.parse_args()
    if len(args) not in [3, 4]:
        parser.error('wrong number of command line arguments')

    joboptiontemplate = args[0]
    dsname = args[1]
    taskname = args[2]

    legacy_options = len(args) == 4
    if legacy_options:
        print "WARNING: the four-argument invocation of runJobs is deprecated"
        print "WARNING: enabling (imperfect) legacy compatibility mode"
        files = extract_file_list_legacy(args[3], opts)
        grid_mode = bool(opts.legacy_griduser)
        runner_type = opts.legacy_runner
        if grid_mode:
            runner_type = 'PandaJobRunner'
        if opts.legacy_interactive:
            opts.legacy_runner='JobRunner'
    else:
        files = extract_file_list(opts)
        grid_mode = opts.submit == 'grid'
        runner_type = {
                'lsf': 'LSFJobRunner',
                'grid': 'PandaJobRunner',
                'shell': 'ShellJobRunner',
                'bg': 'BackgroundJobRunner',
                'pdsf': 'PDSFJobRunner',
                'simple': 'JobRunner',
                }[opts.submit]
        if grid_mode:
            if not opts.in_dsid:
                sys.exit('ERROR: For grid submission, a DSID must be given')
    if not files:
        sys.exit('ERROR: No input files found')

    flags = process_flags(opts, legacy=legacy_options)
    flags['comment'] = cmd
    flags['inputfiles'] = files
    flags['joboptionpath'] = joboptiontemplate

    if grid_mode:
        flags['inputds'] = files[0]
        if opts.lbperjob:
            sys.exit('ERROR: Bunching per LB not supported for grid jobs')
        if not opts.outputfilelist:
            sys.exit('ERROR: For grid jobs, must specify output files expclitly using option -o (e.g. -o dpd.root)')

    if opts.nfiles < 1 or (opts.legacy_runoverdpd and opts.nfiles == 1):
        # run single job over all files:
        flags['filesperjob'] = len(files)
        if grid_mode:
            sys.exit('ERROR: Must specify number of files per job explicitly when running on grid')

    workdir = os.path.join(os.getcwd(), dsname, taskname)
    flags['jobdir'] = os.path.join(workdir, '%(jobnr)03i')
    if os.path.exists(workdir):
        sys.exit("ERROR: Task %s exists already for dataset %s (directory %s)" % (taskname,dsname,workdir))

    if opts.lbperjob:
        flags['jobname'] = '-'.join([dsname, taskname, 'lb%(jobnr)03i'])
    else:
        flags['jobname'] = '-'.join([dsname, taskname, '%(jobnr)03i'])
        if grid_mode or opts.legacy_runoverdpd:
            flags['jobname'] = dsname + '-' + taskname

    runner = make_runner(runner_type, flags)

    if opts.alignmentfile:
        runner.addFilesToPoolFileCatalog([opts.alignmentfile])

    if grid_mode:
        runner.setParam('outputfileprefix','%(jobname)s-')
        runner.setParam('addinputtopoolcatalog',False)   # input is a grid dataset
        runner.registerToBeCopied('alignmentfile')
        runner.registerToBeCopied('beamspotfile')

    print
    runner.showParams(-1)
    print

    # Temporary warning. TODO: still needed?
    if grid_mode and opts.autoconfparams:
        print "WARNING: Automatic configuration of parameters such as DetDescrVersion doesn't work yet on the grid!"
        print "         Please be sure the values of each of the following parameters are specified explicitly above,"
        print "         unless the defaults in the job option template are correct:\n"
        print "           ", opts.autoconfparams
        print

    print len(files), "input file(s)/dataset found."
    print
    if not opts.testonly:
        runner.configure()
        if opts.taskdb != 'None':
            try:
                with TaskManager.TaskManager(opts.taskdb) as taskman:
                    taskman.addTask(dsname,taskname,joboptiontemplate,runner.getParam('release'),runner.getNJobs(),opts.postprocsteps,comment=cmd)
            except:
                print 'WARNING: Unable to add task to task manager database ' + opts.taskdb
        runner.run()
        if opts.legacy_dowait and not grid_mode:
            if not opts.legacy_interactive: runner.wait()
            print
            print "Job directories in %s for this task:" % workdir
            print
            os.system('ls -l %s' % workdir)
            print
            print "The following output file(s) were produced:"
            print
            print runner.getOutputFiles()
            print
