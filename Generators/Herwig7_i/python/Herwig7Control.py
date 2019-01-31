## \file Herwig7Control.py
## \brief Main python interface for %Herwig7 for preparing the event generation
## \author Daniel Rauch (daniel.rauch@desy.de)
##
## This part of the interface provides functionality for running all the tasks
## necessary to initialize and prepare the event generation.
## More concretely, it handles the read or alternatively the build/integrate/
## mergegrids steps in order to produce the Herwig runfile and all other
## ingredients for a run, possibly also creating a gridpack.
## The event generation itself starting from reading the runfile is handled
## in Herwig7_i/Herwig7.h and src/Herwig7.cxx.

import datetime, os, shutil, subprocess, sys, time

import Herwig7Config as hw7Config
import Herwig7Utils as hw7Utils
# import Herwig7Defaults as HwDefaults

from AthenaCommon import Logging
athMsgLog = Logging.logging.getLogger('Herwig7Control')


## \brief Get path to the `share/Herwig` folder
##
## Try to get it from the `InstallArea` first.
## If this fails fall back to `$HERWIG7_PATH/share/Herwig`
##
def get_share_path():

  cmt_paths  = os.environ.get("CMAKE_PREFIX_PATH")
  cmt_config = os.environ.get("BINARY_TAG")
  
  # trying to get it from the `InstallArea`
  for path in cmt_paths.split(':'):
    path = os.path.join(path, "InstallArea", cmt_config, "share")
    try:
      filelist = os.listdir(path)
    except:
      filelist = []
    if "HerwigDefaults.rpo" in filelist: return(path)

  # falling back to `$HERWIG7_PATH`
  path = os.path.join(os.environ['HERWIG7_PATH'], 'share/Herwig')
  if os.path.isfile(os.path.join(path, 'HerwigDefaults.rpo')):
    return(path)

  # raise exception if none of the two methods work out
  raise RuntimeError(hw7Utils.ansi_format_error('Could not find a valid share/Herwig folder'))


# proper handling with path set in External/Herwig7/cmt/requirements
herwig7_path = os.environ['HERWIG7_PATH']
herwig7_bin_path   = os.path.join(herwig7_path, 'bin')
herwig7_share_path = get_share_path()

herwig7_binary     = os.path.join(herwig7_bin_path, 'Herwig')


## Do the read/run sequence.
##
## This function should provide the read and run step in one go
def run(gen_config):

  ## perform the read step
  do_read(gen_config)

  ## start the event generation
  do_run(gen_config, cleanup_herwig_scratch=False)


## Do the build, integrate, mergegrids and run step in one go
## without creating a gridpack
##
## \param[in] cleanup_herwig_scratch Remove `Herwig-scratch` folder after event generation to save disk space
def matchbox_run(gen_config, integration_jobs, cleanup_herwig_scratch):

  ## perform build/integrate/mergegrids sequence
  do_build_integrate_mergegrids(gen_config, integration_jobs)

  ## start the event generation
  do_run(gen_config, cleanup_herwig_scratch)


## Either do the build, integrate and mergegrids steps and create a gridpack
## or extract it and generate events from it
##
## \param[in] cleanup_herwig_scratch Remove `Herwig-scratch` folder after event generation to save disk space
def matchbox_run_gridpack(gen_config, integration_jobs, gridpack_name, cleanup_herwig_scratch):

  print_integration_logs = True

  gridpack_exists = hasattr(gen_config.runArgs, 'inputGenConfFile')

  ## print start banner including version numbers
  log(message=start_banner())

  if not gridpack_exists:

    ## create infile from jobOption commands
    write_infile(gen_config)

    ## do build/integrate/mergegrids sequence
    xsec, err = do_build_integrate_mergegrids(gen_config, integration_jobs)

    ## compress infile, runfile and process folder to gridpack tarball
    do_compress_gridpack(gen_config.run_name, gridpack_name)

    ## display banner and exit
    log(message=exit_banner(gridpack_name, xsec, err))
    sys.exit(0)

  else:

    ## unpack the gridpack
    do_uncompress_gridpack(gen_config.runArgs.inputGenConfFile)
    athMsgLog.info("Finished unpacking the gridpack")

    ## start the event generation
    do_run(gen_config, cleanup_herwig_scratch)



def do_step(step, command, logfile_name=None):

  athMsgLog.info(hw7Utils.ansi_format_info("Starting Herwig7 '{}' step with command '{}'".format(step, ' '.join(command))))

  logfile = open(logfile_name, 'w') if logfile_name else None
  do = subprocess.Popen(command, stdout=logfile, stderr=logfile)
  do.wait()
  if not do.returncode == 0:
    raise RuntimeError(hw7Utils.ansi_format_error("Some error occured during the '{}' step.".format(step)))

  if logfile:
    athMsgLog.info("Content of {} log file '{}':".format(step, logfile_name))
    athMsgLog.info("")
    with open(logfile_name, 'r') as logfile:
      for line in logfile:
        athMsgLog.info('  {}'.format(line.rstrip('\n')))
    athMsgLog.info("")


def do_abort():
  athMsgLog.info(hw7Utils.ansi_format_info("Doing abort"))
  sys.exit(0)


## Do the read step
def do_read(gen_config):

  ## print start banner including version numbers
  log(message=start_banner())

  ## create infile from JobOption object
  write_infile(gen_config)

  ## copy HerwigDefaults.rpo to the current working directory
  get_default_repository()

  ## call Herwig7 binary to do the read step
  share_path = get_share_path()
  do_step('read', [herwig7_binary, 'read', get_infile_name(gen_config.run_name), '-I', share_path])

## Do the read step and re-use an already existing infile
def do_read_existing_infile(gen_config):

  ## print start banner including version numbers
  log(message=start_banner())

  ## copy HerwigDefaults.rpo to the current working directory
  get_default_repository()

  ## call Herwig7 binary to do the read step
  share_path = get_share_path()
  do_step('read', [herwig7_binary, 'read', gen_config.infile_name, '-I', share_path])


## Do the build step
def do_build(gen_config, integration_jobs):

  ## print start banner including version numbers
  log(message=start_banner())

  ## create infile from JobOption object
  write_infile(gen_config)

  ## copy HerwigDefaults.rpo to the current working directory
  get_default_repository()

  ## call the Herwig7 binary to do the build step
  share_path = get_share_path()
  do_step('build', [herwig7_binary, 'build', get_infile_name(gen_config.run_name), '-I', share_path, '-y '+str(integration_jobs)])


## Do the integrate step for one specific integration job
## \todo provide info about the range
def do_integrate(run_name, integration_job):

  runfile_name = get_runfile_name(run_name)
  # setupfile_name = get_setupfile_name(run_name) if setupfile else None

  integrate_log = run_name+'.integrate'+str(integration_job)+'.log'
  integrate_command = [herwig7_binary,'integrate',runfile_name,'--jobid='+str(integration_job)]
  # if setupfile: integrate_command.append('--setupfile='+setupfile_name)

  do_step('integrate', integrate_command, integrate_log)


## This function provides the mergegrids step
def do_mergegrids(run_name, integration_jobs):

  runfile_name = get_runfile_name(run_name)
  mergegrids_command = [herwig7_binary, 'mergegrids', runfile_name]
  # if setupfile_name: mergegrids_command.append('--setupfile='+setupfile_name)

  do_step('mergegrids', mergegrids_command)

  ## calculate the cross section from the integration logfiles and possibly warn about low accuracy
  xsec, err = hw7Utils.get_cross_section(run_name, integration_jobs)

  return(xsec, err)


## Subsequent build, integrate and mergegrid steps
def do_build_integrate_mergegrids(gen_config, integration_jobs):

  ## run build step
  do_build(gen_config, integration_jobs)

  ## run integration jobs in parallel subprocesses
  runfile_name = get_runfile_name(gen_config.run_name)
  # setupfile_name = get_setupfile_name(run_name) if setupfile else None
  athMsgLog.info(hw7Utils.ansi_format_info('Starting integration with {} jobs'.format(integration_jobs)))

  integration_procs = []
  for integration_job in range(integration_jobs):
    integrate_log = gen_config.run_name+'.integrate'+str(integration_job)+'.log'
    integrate_command = [herwig7_binary,'integrate',runfile_name,'--jobid='+str(integration_job)]
    # if setupfile: integrate_command.append('--setupfile='+setupfile_name)
    integration_procs.append(hw7Utils.Process(integration_job, integrate_command, integrate_log))

  integration_handler = hw7Utils.ProcessHandler(integration_procs, athMsgLog)
  if not integration_handler.success():
    raise RuntimeError(hw7Utils.ansi_format_error('Not all of the integration jobs finished successfully'))

  athMsgLog.info(hw7Utils.ansi_format_ok('All integration jobs finished successfully'))

  ## combine the different integration grids
  xsec, err = do_mergegrids(gen_config.run_name, integration_jobs)

  return(xsec, err)


def do_compress_gridpack(run_name, gridpack_name):

  if not (gridpack_name.endswith('.tar.gz') or gridpack_name.endswith('.tgz')): gridpack_name += '.tar.gz'
  infile_name = get_infile_name(run_name)
  runfile_name = get_runfile_name(run_name)
  do_step('compress', ['tar', 'czf', gridpack_name, infile_name, runfile_name, 'Herwig-scratch'])


def do_uncompress_gridpack(gridpack_name):

  athMsgLog.info("unpacking gridpack '{}'".format(gridpack_name))
  do_step('uncompress', ['tar', 'xzf', gridpack_name])


## \param[in] cleanup_herwig_scratch Remove `Herwig-scratch` folder after event generation to save disk space
def do_run(gen_config, cleanup_herwig_scratch=True):

  ## this is necessary to make Herwig aware of the name of the run file
  gen_config.genSeq.Herwig7.RunFile = get_runfile_name(gen_config.run_name)

  ## overwrite athena's seed for the random number generator
  if gen_config.runArgs.randomSeed is None:
    gen_config.genSeq.Herwig7.UseRandomSeedFromGeneratetf = False
  else:
    gen_config.genSeq.Herwig7.UseRandomSeedFromGeneratetf = True
    gen_config.genSeq.Herwig7.RandomSeedFromGeneratetf = gen_config.runArgs.randomSeed

  ## set matrix element PDF name in the Herwig7 C++ class
  gen_config.genSeq.Herwig7.PDFNameME = gen_config.me_pdf_name

  ## set underlying event PDF name in the Herwig7 C++ class
  gen_config.genSeq.Herwig7.PDFNameMPI = gen_config.mpi_pdf_name

  ## possibly delete Herwig-scratch folder after finishing the event generation
  gen_config.genSeq.Herwig7.CleanupHerwigScratch = cleanup_herwig_scratch

  ## don't break out here so that the job options can be finished and the C++
  ## part of the interface can take over and generate the events
  athMsgLog.info(hw7Utils.ansi_format_info("Returning to the job options and starting the event generation afterwards"))


## Do the run step and re-use an already existing runfile
def do_run_existing_runfile(gen_config):

  ## this is necessary to make Herwig aware of the name of the run file
  gen_config.genSeq.Herwig7.RunFile = gen_config.runfile_name

  ## overwrite athena's seed for the random number generator
  if gen_config.runArgs.randomSeed is None:
    gen_config.genSeq.Herwig7.UseRandomSeedFromGeneratetf = False
  else:
    gen_config.genSeq.Herwig7.UseRandomSeedFromGeneratetf = True
    gen_config.genSeq.Herwig7.RandomSeedFromGeneratetf = gen_config.runArgs.randomSeed

  ## don't break out here so that the job options can be finished and the C++
  ## part of the interface can take over and generate the events
  athMsgLog.info(hw7Utils.ansi_format_info("Returning to the job options and starting the event generation afterwards"))


# utility functions -----------------------------------------------------------


def herwig_version():

  versions = get_software_versions()
  return(' '.join(versions[0].split()[1:]))

def thepeg_version():

  versions = get_software_versions()
  return(' '.join(versions[1].split()[1:]))

def start_banner():

  herwig_version_number = herwig_version()
  thepeg_version_number = thepeg_version()
  herwig_version_space = ' '.join(['' for i in range(14-len(herwig_version_number))])
  thepeg_version_space = ' '.join(['' for i in range(14-len(thepeg_version_number))])

  banner = ''
  banner += "#####################################\n"
  banner += "##   {}   ##\n".format(hw7Utils.ansi_format_ok("---------------------------"))
  banner += "##   {}   ##\n".format(hw7Utils.ansi_format_ok("Starting HERWIG 7 in ATHENA"))
  banner += "##   {}   ##\n".format(hw7Utils.ansi_format_ok("---------------------------"))
  banner += "##                                 ##\n"
  banner += "##   with software versions:       ##\n"
  banner += "##   - Herwig7:    {}{}   ##\n".format(herwig_version_number, herwig_version_space)
  banner += "##   - ThePEG:     {}{}   ##\n".format(thepeg_version_number, thepeg_version_space)
  # banner += "##                                            ##\n"
  # banner += "##   with depencency versions                 ##\n"
  # banner += "##   - GoSam\n"
  # banner += "##     - GoSam-Contrib\n"
  # banner += "##     - GoSam\n"
  # banner += "##   - HJets\n"
  # banner += "##   - MadGraph5_aMC@NLO: \n"
  # banner += "##   - NJet: \n"
  # banner += "##   - OpenLoops: \n"
  # banner += "##   - VBFNLO: \n"
  banner += "##                                 ##\n"
  banner += "#####################################\n"
  return(banner)


def get_software_versions():

  return(subprocess.check_output([herwig7_binary,'--version']).splitlines())
  

def get_infile_name(run_name="Herwig-Matchbox"):

  return('{}.in'.format(run_name))


def get_setupfile_name(run_name="Herwig-Matchbox"):

  return('{}.setupfile.in'.format(run_name))


def get_runfile_name(run_name="Herwig-Matchbox"):

  return('{}.run'.format(run_name) if not run_name.endswith('.run') else run_name)


def write_infile(gen_config, print_infile=True):

  # lock settings to prevent modification from within the job options after infile was written to disk
  gen_config.default_commands.lock()
  gen_config.commands.lock()

  infile_name = get_infile_name(gen_config.run_name)
  if print_infile: athMsgLog.info("")
  athMsgLog.info(hw7Utils.ansi_format_info("Writing infile '{}'".format(infile_name)))
  commands = \
    gen_config.global_pre_commands().splitlines() \
    + gen_config.local_pre_commands().splitlines() \
    + ["",
       "## ================",
       "## Default Commands",
       "## ================"] \
    + str(gen_config.default_commands.commands).splitlines() \
    + ["",
       "## ========================",
       "## Commands from jobOptions",
       "## ========================"] \
    + str(gen_config.commands.commands).splitlines() \
    + gen_config.local_post_commands().splitlines()
  try:
    with open(infile_name, 'w') as infile:
        for command in commands:
          infile.write(command+'\n')
  except:
    raise RuntimeError('Could not write Herwig/Matchbox infile')

  if print_infile:
    athMsgLog.info("")
    for command in commands:
      athMsgLog.info('  {}'.format(command))
    athMsgLog.info("")


def write_setupfile(run_name, commands, print_setupfile=True):

  setupfile_name = get_setupfile_name(run_name)

  if len(commands) > 0:
    if print_setupfile: athMsgLog.info("")
    athMsgLog.info("Writing setupfile '{}'".format(setupfile_name))
    try:
      with open(setupfile_name, 'w') as setupfile:
          for command in commands: setupfile.write(command+'\n')
    except:
      raise RuntimeError('Could not write Herwig/Matchbox setupfile')

    if print_setupfile:
      athMsgLog.info("")
      for command in commands: athMsgLog.info('  {}'.format(command))
      athMsgLog.info("")

  else:
    athMsgLog.info("No setupfile commands given.")


## \brief Copy default repository `HerwigDefaults.rpo` to current working directory
##
def get_default_repository():

  shutil.copy(os.path.join(get_share_path(), 'HerwigDefaults.rpo'), 'HerwigDefaults.rpo')


def log(level='info', message=''):

  if level in ['info', 'warn', 'error']:
    logger = getattr(athMsgLog, level)
    for line in message.splitlines(): logger(line)
  else:
    raise ValueError("Unknown logging level'{}' specified. Possible values are 'info', 'warn' or 'error'".format(level))


def exit_banner(gridpack, cross_section, cross_section_error):

  size = hw7Utils.humanize_bytes(hw7Utils.get_size(gridpack))
  space_size = hw7Utils.get_repeated_pattern(' ', 31-len(size))

  xsec    = '{:f}'.format(cross_section)
  err     = '{:f}'.format(cross_section_error)
  rel_err = '{:.2f}'.format(cross_section_error / cross_section * 100.0)
  space_xsec = hw7Utils.get_repeated_pattern(' ', 37-len(xsec)-len(err)-len(rel_err))

  banner = ''
  space = ' '.join(['' for i in range(70+4+1-len(gridpack))])
  banner += "##########################################################################################\n"
  banner += "##   -------------------------------------------------------------------------------    ##\n"
  banner += "##   {} (size: {}){} ##\n".format(hw7Utils.ansi_format_ok("HERWIG 7 successfully created the gridpack"), size, space_size)
  banner += "##                                                                                      ##\n"
  banner += "##      {}{}      ##\n".format(hw7Utils.ansi_format_info(gridpack), space)
  banner += "##                                                                                      ##\n"
  banner += "##   cross section from integration: {} +/- {} ({}%) nb {} ##\n".format(xsec, err, rel_err, space_xsec)

  if cross_section_error / cross_section > hw7Utils.integration_grids_precision_threshold:
    threshold = '{}%'.format(hw7Utils.integration_grids_precision_threshold*100.0)
    space_threshold = hw7Utils.get_repeated_pattern(' ', 6-len(threshold))
    banner += "##                                                                                      ##\n"
    banner += "##   {}    ##\n".format(hw7Utils.ansi_format_warning("! WARNING: The integration grids only have a low precision (worse than {}){}!".format(threshold, space_threshold)))

  banner += "##                                                                                      ##\n"
  banner += "##   -------------------------------------------------------------------------------    ##\n"
  banner += "##                                                                                      ##\n"
  banner += "##   Please ignore the error                                                            ##\n"
  banner += "##                                                                                      ##\n"
  banner += "##      No such file or directory: 'evgen.root' raised while stating file evgen.root    ##\n"
  banner += "##                                                                                      ##\n"
  banner += "##########################################################################################\n"
  return(banner)
