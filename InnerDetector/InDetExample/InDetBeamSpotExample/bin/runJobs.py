#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Front-end script to run JobRunner jobs

__author__  = 'Juerg Beringer'
__version__ = 'runJobs.py atlas/athena'
__usage__   = """%prog [options] JOBOPTIONTEMPLATE DATASET TASK INPUTDATA

Templates:  - InDetBeamSpotExample/ESDToDPDTemplate.py
            - InDetBeamSpotExample/BSToDPDTemplate.py
            - InDetBeamSpotExample/VertexTemplate.py
            - InDetBeamSpotExample/TrackChi2Template.py
            - InDetBeamSpotExample/MonitoringTemplate.py
            - your own template file

Examples:   - runJobs InDetBeamSpotExample/ESDToDPDTemplate.py 105039 DPD 105039/ESD
            - runJobs -d InDetBeamSpotExample/VertexTemplate.py 105039 VTX DPD
            - runJobs -d -q all.64bit.q -r PDSFJobRunner InDetBeamSpotExample/TrackChi2Template.py 105039 TRK DPD
            - runJobs -o dpd.root -g user09.YourName -p 'DetDescrVersion="ATLAS-GEO-02-01-00"' \\
                      InDetBeamSpotExample/ESDToDPDTemplate.py 105039 DPD \\
                      mc08.105039.J0_pythia_jetjetNU.recon.ESD.e372_s490_r603/"""


import sys
import glob
import os
import re
import InDetBeamSpotExample
from InDetBeamSpotExample import TaskManager
from InDetBeamSpotExample import DiskUtils

# Create a properly quoted string of the command line to save
qargv = [ ]
for s in sys.argv:
    if re.search('\s|\*|\?',s):   # any white space or special characters in word so we need quoting?
        if "'" in s:
            qargv.append('"%s"' % re.sub('"',"'",s))
        else:
            qargv.append("'%s'" % re.sub("'",'"',s))
    else:
        qargv.append(s)
cmd = ' '.join(qargv)

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('', '--bytestream', dest='bytestream', action='store_true', default=False, help='input files are bytestream instead of ROOT/POOL files')
parser.add_option('-m', '--mc', dest='isMc', action='store_true', default=False, help='input data is from Monte-Carlo instead of data (automatically chooses between COMP200 and OFLP200 / CONDBR2 conditions DBs)')
parser.add_option('-j', '--maxjobs', dest='maxjobs', type='int', default=0, help='max number of jobs (default: 0 ie no maximum)')
parser.add_option('-n', '--nfiles', dest='nfiles', type='int', default=1, help='number of files per job (default: 1, set to 0 for single job over all files)') 
parser.add_option('-e', '--maxevents', dest='evtmax', type='int', default=-1, help='max number of events per job')
parser.add_option('', '--lbperjob', dest='lbperjob', type='int', default=0, help='number of luminosity blocks per job (default: 0 - no bunching)') 
parser.add_option('-o', '--outputfilelist', dest='outputfilelist', default='', help='list of desired output files (default: "dpd.root,nt.root,monitoring.root,beamspot.db"; must be specified explicitly for grid)')
parser.add_option('-k', '--taskdb', dest='taskdb', default='', help='TaskManager database (default: from TASKDB or sqlite_file:taskdata.db; set to string None to avoid using a task database)')
parser.add_option('-l', '--logmail', dest='users', default='', help='send log mail to specified users (default: no mail)')
parser.add_option('-q', '--queue', dest='batchqueue', default='atlasb1', help='batch queue (default: atlasb1)')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='run interatively (same as -r JobRunner)')
parser.add_option('-g', '--grid', dest='griduser', default='', help='run on grid (GRIDUSER is user prefix of grid job name, e.g. user09.JuergBeringer; INPUTDATA is grid dataset name)')
parser.add_option('-s', '--gridsite', dest='gridsite', default='AUTO', help='site name where jobs are sent (default:AUTO)')
parser.add_option('-r', '--runner', dest='runner', default='LSFJobRunner', help='type of JobRunner (class name, default: LSFJobRunner)')
parser.add_option('-w', '--wait', dest='dowait', action='store_true', default=False, help='wait for jobs to complete')
parser.add_option('-z', '--postprocsteps', dest='postprocsteps', default='JobPostProcessing', help='Task-level postprocessing steps (Default: JobPostProcessing)')
parser.add_option('-t', '--test', dest='testonly', action='store_true', default=False, help='show only options and input files')
parser.add_option('-v', '--verbosity', dest='outputlevel', type='int', default=4, help='output level (default:4, where 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)')
parser.add_option('-p', '--params', dest='params', default='', help='job option parameters to pass to job option template')
parser.add_option('', '--autoconfparams', dest='autoconfparams', default='DetDescrVersion', help='comma-separated list of automatically determined parameters (template must include AutoConfFragment.py, default: "DetDescrVersion")')
# Additional optional files requiring special treatment (other parameters should be passed
# to the job option template via "-p params")
parser.add_option('-a', '--alignment-file', dest='alignmentfile', default='', help='alignment file (default: none)')
parser.add_option('-b', '--beamspot-file', dest='beamspotfile', default='', help='beam spot SQLite file (default: none)')

# TODO check if these flags can be removed:
parser.add_option('-c', '--castor', dest='fromcastor', action='store_true', default=False, help='INPUTDATA refers to CASTOR directory')
parser.add_option('', '--prefix', dest='prefix', default='', help='Prefix for reading files from mass storage (Default: determine from filename (`\'\') ')
parser.add_option('-d', '--dpd', dest='runoverdpd', action='store_true', default=False, help='run over DPD (single job, INPUTDATA is DPD task name)')

# TODO this takes regular expressions in some cases and globs in others which is inconsistent:
parser.add_option('-f', '--filter', dest='filter', default='', help='use specified pattern to filter input files (default: *.root* for local files, .*ESD.* for castor)')

(options,args) = parser.parse_args()
if len(args) != 4:
    parser.error('wrong number of command line arguments')

userparams = { }
for s in options.params.split(', '):
    if s:
        try:
            p = s.split('=',1)
            userparams[p[0].strip()] = eval(p[1].strip())
        except:
            print '\nERROR parsing user parameter',p,'- parameter will be ignored'

joboptiontemplate = args[0]
dsname = args[1]
taskname = args[2]
inputdata = args[3]
filter = options.filter

# Set input data type
if options.isMc:
    DataSource = 'geant4'
else:
    DataSource = 'data'

# Set input dataset and files
if options.griduser:
    inputDS = inputdata
    files = [ inputdata ]
    options.runner = 'PandaJobRunner'

else:
    inputDS = ''
    if options.fromcastor:
        # INPUTDATA specifies a directory with files
        pattern = options.filter or (None if options.bytestream else '.*ESD.*')
        fs = DiskUtils.FileSet.from_directory(inputdata).matching(pattern)
    elif os.path.isfile(inputdata):
        # inputdata is a text file with filenames
        fs = DiskUtils.FileSet.from_file_containing_list(inputdata)
    elif options.runoverdpd:
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
        pattern = options.filter or '*.root*'
        fs = DiskUtils.FileSet.from_glob(os.path.join(inputdata, pattern))
    files = sorted(fs)


if options.interactive:
    options.runner='JobRunner'

if not files:
    sys.exit('ERROR: No input files found')

if options.runoverdpd and not options.lbperjob:
    options.maxjobs = 1

if options.lbperjob and options.griduser:
    sys.exit('ERROR: Bunching per LB not supported for grid jobs')

if options.nfiles<1 or (options.runoverdpd and options.nfiles==1):
    options.nfiles = len(files)   # run single job over all files
    if options.griduser:
        sys.exit('ERROR: Must specify number of files per job explicity when running on grid')

# List of desired output files. For grid jobs, it must specify exactly
# the expected files (otherwise the grid jobs will fail or not return the
# desired output). For all other jobs, it can be an inclusive list of
# possible outputs and JobRunner will return only the actually present
# output files when asked for output files.
if not options.outputfilelist:
    if options.griduser:
        sys.exit('ERROR: For grid jobs, must specify output files expclitly using option -o (e.g. -o dpd.root)')
    else:
        outputfilelist = ['dpd.root', 'nt.root', 'monitoring,root', 'beamspot.db']
else:
    outputfilelist = [ f.strip() for f in options.outputfilelist.split(',') ]

workdir = os.getcwd()+'/'+dsname+'/'+taskname
if os.path.exists(workdir):
    sys.exit("ERROR: Task %s exists already for dataset %s (directory %s)" % (taskname,dsname,workdir))

if options.lbperjob:
    jobname=dsname+'-'+taskname+'-lb%(jobnr)03i'
else:
    jobname=dsname+'-'+taskname+'-%(jobnr)03i'
    if options.runoverdpd or options.griduser:
        jobname=dsname+'-'+taskname

runnerClass = InDetBeamSpotExample.loadClass(options.runner)
runner = runnerClass(inputfiles=files,
                     inputds=inputDS,
                     bytestream=options.bytestream,
                     filesperjob=options.nfiles,
                     lbperjob=options.lbperjob,
                     joboptionpath=joboptiontemplate,
                     batchqueue=options.batchqueue,
                     jobname=jobname,
                     griduser=options.griduser,
                     gridsite=options.gridsite,
                     jobdir=workdir+'/%(jobnr)03i',
                     addinputtopoolcatalog=not (options.bytestream or options.griduser),
                     evtmax=options.evtmax,
                     maxjobs=options.maxjobs,
                     outputlevel=options.outputlevel,
                     logmail=options.users,
                     outputfilelist=outputfilelist,
                     alignmentfile=options.alignmentfile,
                     beamspotfile=options.beamspotfile,
                     autoconfparams=options.autoconfparams,
                     taskpostprocsteps=options.postprocsteps,
                     DataSource=DataSource,
                     comment=cmd,
                     **userparams)

if options.alignmentfile:
    runner.addFilesToPoolFileCatalog([options.alignmentfile])

if options.griduser:
    runner.setParam('outputfileprefix','%(jobname)s-')
    runner.setParam('addinputtopoolcatalog',False)   # input is a grid dataset
    runner.registerToBeCopied('alignmentfile')
    runner.registerToBeCopied('beamspotfile')

print
runner.showParams()
print

# Temporary warning
if options.griduser and options.autoconfparams:
    print "WARNING: Automatic configuration of parameters such as DetDescrVersion doesn't work yet on the grid!"
    print "         Please be sure the values of each of the following parameters are specified explicitly above,"
    print "         unless the defaults in the job option template are correct:\n"
    print "           ", options.autoconfparams
    print

print len(files),"input file(s)/input dataset found."
print
if not options.testonly:
    runner.configure()
    if options.taskdb!='None':
        try:
            with TaskManager.TaskManager(options.taskdb) as taskman:
                taskman.addTask(dsname,taskname,joboptiontemplate,runner.getParam('release'),runner.getNJobs(),options.postprocsteps,comment=cmd)
        except:
            print 'WARNING: Unable to add task to task manager database '+options.taskdb
    runner.run()
    if options.dowait and not options.griduser:
        if not options.interactive: runner.wait()
        print
        print "Job directories in %s for this task:" % workdir
        print
        os.system('ls -l %s' % workdir)
        print
        print "The following output file(s) were produced:"
        print
        print runner.getOutputFiles()
        print
