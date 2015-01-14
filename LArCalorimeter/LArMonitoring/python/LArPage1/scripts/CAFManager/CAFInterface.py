# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)    
# 
# CAFInterface: 
#   parses the arguments of command-line scripts and formats
#    appropriate command for the CAFMAnager backend.


import os, sys, time, subprocess, traceback
from CAFManager import CAFManager
from DataClasses import ConfigRecord, JobTypeRecord, FileRecord, JobRecord, FileSpecs, JobTypeSpec
from Logger import Logger, FileLogger, FileStream, OutputStream
from FileAccessor import FileAccessor
from Checkers import FileChecker

logFileName = '/afs/cern.ch/user/l/larcomm/w0/digitMon/output/logs/cafMgr.log'
lockFileName = '/afs/cern.ch/user/l/larcomm/w0/digitMon/output/logs/lock'
masterLogFileName = '/afs/cern.ch/user/l/larcomm/w0/digitMon/output/logs/master.log'


def add(args) :
  log = Logger(OutputStream())
  mgr = CAFManager()
  if not mgr.ok() : return
  simulate = ('-s' in args or '--simulate' in args)
  if '--config' in args :
    idx = args.index('--config')
    if idx + 4 >= len(args) : return log.ERROR('arguments expected after --config')
    releaseDir   = args[idx + 1]
    tmpOutput    = args[idx + 2]
    batchScheme  = args[idx + 3]
    defaultQueue = args[idx + 4]
    mgr.db().addConfig(ConfigRecord(releaseDir, batchScheme, defaultQueue, tmpOutput), simulate=simulate)
    return

  if '--jobType' in args :
    idx = args.index('--jobType')
    if idx + 1 >= len(args) : return log.ERROR('arguments expected after --jobType')
    filePath = args[idx + 1]
    filePars = os.path.split(filePath)
    fileDir  = filePars[0]
    fileName = filePars[1]
    if fileName == '' : return log.ERROR('Empty fileName specified')
    if len(fileName.split('.')) > 1 : fileName = fileName.split('.')[0]
    if fileDir == '' : fileDir = os.getcwd()
    sys.path.append(os.path.join(os.getcwd(), fileDir))
    try: exec('from ' + fileName + ' import job')
    except Exception, inst : 
      traceback.print_exc()
      return log.ERROR('Could not import job from specified file ' + filePath + ' : ' + str(inst))
    mgr.db().addJobType(JobTypeRecord(job.m_name, job.m_transform, job.m_inputs, job.m_outputDir, job.m_outputs, 
			job.m_inputSelector, job.m_namingScheme, job.m_configId, job.m_maxJobs, job.m_cronExec), simulate=simulate)
    return
  if '--file' in args :
    idx = args.index('--file')
    if idx + 2 >= len(args) : return log.ERROR('arguments expected after --file')
    fileName = args[idx + 1]
    fileType = args[idx + 2]
    if fileName == '' : return log.ERROR('Empty fileName specified')
    f = FileRecord(fileName, fileType)
    if not FileChecker(mgr.db(), log.stream).isOK(f) :
      return log.ERROR('Invalid file specifications')
    mgr.db().addFile(f)
    return
  log.ERROR('Invalid object type specified!')


def dump(args, log = Logger(OutputStream())) :
  categories = parseCategories(args, log)
  if len(categories) == 0 :
    return log.ERROR('Must specify at least one category (--configs, --jobTypes, --jobs, or --files)')
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  verbosity = parseVerbosity(args, log)
  mgr = CAFManager()
  if not mgr.ok() : return
  getJobFiles = True
  if verbosity == 0 : getJobFiles = False
  data = retrieveData(mgr, log, categories, selection, typeSelection, statusSelection, nMax, getJobFiles)
  if 'config' in categories :
    log.INFO('-------------------------------------------------------------------')
    log.INFO('Configs: ')
    log.INFO('-------------------------------------------------------------------')
    n = 0
    for config in data['configs'] :
      config.dump(log, verbosity)
      n = n + 1
    log.INFO(str(n) + ' entries')
  if 'jobType' in categories :
    log.INFO('-------------------------------------------------------------------')
    log.INFO('JobTypes: ')
    log.INFO('-------------------------------------------------------------------')
    n = 0
    for jobType in data['jobTypes'] :
      jobType.dump(log, verbosity)
      n = n + 1
    log.INFO(str(n) + ' entries')
  if 'job' in categories :
    log.INFO('-------------------------------------------------------------------')
    log.INFO('Jobs: ')
    log.INFO('-------------------------------------------------------------------')
    n = 0
    for job in data['jobs'] :
      job.dump(log, verbosity)
      n = n + 1
    log.INFO(str(n) + ' entries')
  if 'file' in categories :
    log.INFO('-------------------------------------------------------------------')
    log.INFO('Files: ')
    log.INFO('-------------------------------------------------------------------')
    n = 0
    for f in data['files'] :
      f.dump(log, verbosity)
      n = n + 1
      if verbosity == 0 : continue
      jobIds = mgr.db().getJobIdsForFiles([f])[f]
      log.INFO('  In jobs :')
      for jobId in jobIds :
        job = mgr.db().getJob(jobId)
        log.INFO('    %s' % job.name)
    log.INFO(str(n) + ' entries')


def remove(args) :
  log = Logger(OutputStream())
  simulate = ('-s' in args or '--simulate' in args)
  if not simulate :
    if not testLock(Logger(OutputStream())) : return
    lock(log, args)
  categories = parseCategories(args, log)
  if len(categories) != 1 :
    unlock(log)
    return log.ERROR('Must specify exactly one category to work on (--config, --jobType, --job or --file)')
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  verbosity = parseVerbosity(args, log)
  force = ('-f' in args or '--force' in args)
  mgr = CAFManager()
  if not mgr.ok() : 
    unlock(log)
    return
  data = retrieveData(mgr, log, categories, selection, typeSelection, statusSelection)
  if (not 'files'    in data or data['files']    == []) and \
     (not 'jobs'     in data or data['jobs']     == []) and \
     (not 'jobTypes' in data or data['jobTypes'] == []) and \
     (not 'configs'  in data or data['configs']  == []) : 
    log.INFO('Nothing to remove')
    unlock(log)
    return
  if not simulate and not force : 
    log.INFO('To be removed:')
    if 'files' in data :
      for f in data['files'] : log.INFO('File ' + f.name)
    if 'jobs' in data :
      for j in data['jobs'] : log.INFO('Job ' + j.name)
    if 'jobTypes' in data :
      for jt in data['jobTypes'] : log.INFO('JobType ' + jt.name)
    if 'configs' in data :
      for c in data['configs'] : log.INFO('JobConfig ' + c.name)
    log.INFO("This is not a test! (Otherwise use -s). If you're sure, return [y]")  
    line = sys.stdin.readline()
    if line.strip() != 'y' :
      log.TRACE('Aborted on user second thoughts')
      unlock(mgr)
      return
  if 'config' in categories :
    if len(data['configs']) != 1 :
      unlock(mgr)
      return log.ERROR('Must specify exactly one config id using --id')
    mgr.db().removeConfig(data['config'][0], simulate = simulate)
  if 'jobType' in categories :
    if len(data['jobTypes']) != 1 :
      return log.ERROR('Must specify exactly one jobType id using --id')
    mgr.db().removeJobType(data['jobTypes'][0], simulate = simulate)
  if 'file' in categories :
    for f in data['files'] : mgr.db().removeFile(f, simulate = simulate)
  if 'job' in categories :
    for job in data['jobs'] : mgr.db().removeJob(job, simulate = simulate)
  unlock(mgr)
  return True

def modify(args) :
  log = Logger(OutputStream())
  simulate = ('-s' in args or '--simulate' in args)
  if not simulate :
    if not testLock(Logger(OutputStream())) : return
    lock(log, args)
  categories = parseCategories(args, log)
  if len(categories) != 1 :
    return log.ERROR('Must specify exactly one category to work on (--config, --jobType, --job or --file)')
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  verbosity = parseVerbosity(args, log)
  force = ('-f' in args or '--force' in args)
  properties = parseProperties(args, ['maxJobs', 'inputs', 'inputSelector', 'status', 'outputDir', 'outputs', 'transform', 'releaseDir', 'cronExec', 'configId', 'defaultQueue', 'properties'], log)
  mgr = CAFManager()
  if not mgr.ok() : return
  data = retrieveData(mgr, log, categories, selection, typeSelection, statusSelection)
  if 'job' in categories :
    allowed = ['status', 'outputs', 'properties']
    for job in data['jobs'] :
      if properties['status'] : 
        if not mgr.db().modifyJob(job, status=properties['status'][0], simulate = simulate) : return False
      if properties['outputs'] : 
        if not mgr.db().modifyJob(job, outputs=properties['outputs'], simulate = simulate) : return False
      if properties['properties'] : 
        if not mgr.db().modifyJob(job, properties=properties['properties'][0], simulate = simulate) : return False
  if 'jobType' in categories :    
    allowed = [ 'maxJobs', 'inputs', 'outputDir', 'inputSelector', 'transform', 'cronExec', 'configId' ]
    if len(data['jobTypes']) > 1 :
      log.INFO('Job types affected by this modification:')
      for jobType in data['jobTypes'] : log.INFO('  ' + jobType.name)
      if not simulate and not force :
        log.INFO("This is not a test! (Otherwise use -s). If you're sure, return [y]")  
      line = sys.stdin.readline()
      if line.strip() != 'y' :
        log.INFO('Aborted on user second thoughts')
        unlock(mgr)
        return
    for jobType in data['jobTypes'] :
      if properties['maxJobs'] : 
        if len(properties['maxJobs']) != 1 :
          unlock(mgr)
          return log.ERROR('Expected a single number after maxJobs')
        if not mgr.db().modifyJobType(jobType, maxJobs=properties['maxJobs'][0], simulate = simulate) : 
          unlock(mgr)
          return False
      if properties['inputs'] : 
        if not mgr.db().modifyJobType(jobType, inputs=properties['inputs'][0], simulate = simulate) : 
          unlock(mgr)
          return False
      if properties['inputSelector'] : 
        if not mgr.db().modifyJobType(jobType, inputSelector=properties['inputSelector'][0], simulate = simulate) : 
          unlock(mgr)
          return False
      if properties['outputDir'] : 
        if not mgr.db().modifyJobType(jobType, outputDir=properties['outputDir'][0], simulate = simulate) : 
          unlock(mgr)
          return False
      if properties['transform'] : 
        if not mgr.db().modifyJobType(jobType, transform=properties['transform'][0], simulate = simulate) : 
          unlock(mgr)
          return False
      if properties['cronExec'] : 
        if not mgr.db().modifyJobType(jobType, cronExec=properties['cronExec'][0], simulate = simulate) : 
          unlock(mgr)
          return False
      if properties['configId'] : 
        if not mgr.db().modifyJobType(jobType, configId=properties['configId'][0], simulate = simulate) : 
          unlock(mgr)
          return False
  if 'file' in categories :
    unlock(mgr)
    return self.ERROR('files not handled yet')
  if 'config' in categories :
    allowed = ['releaseDir', 'defaultQueue']
    for config in data['configs'] :
      if properties['releaseDir'] : 
        if not mgr.db().modifyConfig(config, releaseDir=properties['releaseDir'][0], simulate = simulate) : return False
      if properties['defaultQueue'] : 
        if not mgr.db().modifyConfig(config, defaultQueue=properties['defaultQueue'][0], simulate = simulate) : return False
  for p in properties.iterkeys() :
    if not properties[p] : continue 
    if not p in allowed : 
      unlock(mgr)
      return log.ERROR("Cannot set property '" + p + "'")  
  unlock(mgr)
  return True


def query(args) :
  log = Logger(OutputStream())
  categories = parseCategories(args, log)
  if len(categories) != 1 :
    return log.ERROR('Must specify exactly one category to work on (--config, --jobType, --job or --file)')
  properties = parseProperties(args, ['status', 'inputNames', 'outputNames', 'maxJobs', 'inputs', 'inputSelector', 'inputTo', 'outputDir', 'freeFiles', 'allFiles', 'transform', 'releaseDir', 'cronExec', 'configId', 'defaultQueue', 'properties'], log, withValues = False)
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  verbosity = parseVerbosity(args, log)
  simulate = ('-s' in args or '--simulate' in args)
  mgr = CAFManager()
  if not mgr.ok() : return
  data = retrieveData(mgr, log, categories, selection, typeSelection, statusSelection)
  result = None
  if 'job' in categories :
    allowed = [ 'status', 'inputNames', 'outputNames', 'properties' ]
    for job in data['jobs'] :
      if properties['status'] : result = mgr.db().queryJob(job, 'status')
      if properties['inputNames' ] : result = mgr.db().queryJob(job, 'inputNames')
      if properties['outputNames'] : result = mgr.db().queryJob(job, 'outputNames')
      if properties['properties'] : result = mgr.db().queryJob(job, 'properties')
      if result != None : log.INFO(job.name + ' : ' + str(result))
  if 'jobType' in categories :
    allowed = [ 'maxJobs', 'inputs', 'inputSelector', 'outputDir', 'freeFiles', 'allFiles', 'transform', 'cronExec', 'configId' ]
    for jobType in data['jobTypes'] :
      result = None
      if properties['maxJobs'] : result = mgr.db().queryJobType(jobType, 'maxJobs')
      if properties['inputs'] : result = mgr.db().queryJobType(jobType, 'inputs')
      if properties['inputSelector'] : result = mgr.db().queryJobType(jobType, 'inputSelector')
      if properties['outputDir'] : result = mgr.db().queryJobType(jobType, 'outputDir')
      if properties['freeFiles'] : result = mgr.db().queryJobType(jobType, 'freeFiles')
      if properties['allFiles']  : result = mgr.db().queryJobType(jobType, 'allFiles')
      if properties['transform']  : result = mgr.db().queryJobType(jobType, 'transform')
      if properties['cronExec']  : result = mgr.db().queryJobType(jobType, 'cronExec')
      if properties['configId']  : result = mgr.db().queryJobType(jobType, 'configId')
      if result : log.INFO(jobType.name + ' : ' + str(result))
  if 'file' in categories :
    allowed = [ 'inputTo' ]
    for f in data['files'] :
      if properties['inputTo'] : result = mgr.db().queryFile(f, 'inputTo')
      if result : log.INFO(f.name + ' : ' + str(result))
  if 'config' in categories :
    allowed = [ 'releaseDir', 'defaultQueue' ]
    for c in data['configs'] :
      if properties['releaseDir'] : result = mgr.db().queryConfig(c, 'releaseDir')
      if properties['defaultQueue'] : result = mgr.db().queryConfig(c, 'defaultQueue')
      if result : log.INFO(str(c.configId) + ' : ' + str(result))
  for p in properties.iterkeys() :
    if not properties[p] : continue 
    if not p in allowed : return log.ERROR("Property '" + p + "' not handled")  


def admin(args) :
  simulate = ('-s' in args or '--simulate' in args)
  if '--rotateLogs' in args :
    i = 1
    while os.access(logFileName + '.' + str(i) + '.gz', os.F_OK) : i = i + 1
    newFileName = logFileName + '.' + str(i)
    FileLogger(masterLogFileName).TRACE('At ' +  time.asctime() + ' : rotating cafMgr log file to ' + newFileName)
    os.system('mv %s %s && touch %s && gzip %s' % (logFileName, newFileName, logFileName, newFileName))
  if '--backupDBFiles' in args :
    mgr = CAFManager('', FileStream(masterLogFileName))
    if not mgr.ok() : return
    mgr.db().backupFiles()


def run(args) :
  simulate = ('-s' in args or '--simulate' in args)
  if simulate :
    stream = OutputStream()
  else :
    stream = FileStream(logFileName)
  properties = parseProperties(args, ['output-error', 'force-end', 'no-catalog'], Logger(stream), withValues = False)
  if not simulate :
    #if not testLock(Logger(OutputStream())) : return
    masterStream = FileStream(masterLogFileName)
    if not testLock(Logger(masterStream)) : return
    lock(Logger(stream), args)
  tasks = []
  if '--exec' in args or '--cronExec' in args : tasks.append('cronExec')
  if '--scan' in args or '--checkForNewFiles' in args : tasks.append('checkForNewFiles')
  if '--harvest' in args or '--harvestJobs' in args : tasks.append('harvestJobs')
  if '--submit' in args or '--submitJobs' in args : tasks.append('submitJobs')
  sels = parseSelections(args, Logger(stream))
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  mgr = CAFManager(stream = stream)
  if not mgr.ok() : return
  typeSelStr = ''
  if typeSelection != None : typeSelStr = ' for types ' + str(typeSelection)
  extra = ''
  noCatalog = False
  forceEnd = False
  if properties['output-error'] : extra += 'OUTPUT_ERROR'
  if properties['no-catalog']   : noCatalog = True
  submitOptions = []
  if properties['force-end'] : submitOptions += ['force-end']
  mgr.TRACE('')
  mgr.TRACE('-------------------------------------------------------------------')
  mgr.TRACE('Running tasks ' + str(tasks) + typeSelStr + ' at ' + time.asctime())
  mgr.TRACE('-------------------------------------------------------------------')
  nOpsHJ = 0
  nOpsCF = 0
  nOpsSJ = 0
  try:
    if 'cronExec'         in tasks : nOpsCE = mgr.cronExec(typeSelection, simulate=simulate)
    if 'harvestJobs'      in tasks : nOpsHJ = mgr.harvestJobs(extra)
    if 'checkForNewFiles' in tasks : nOpsCF = mgr.checkForNewFiles(typeSelection, simulate=simulate, noCatalog=noCatalog)
    if 'submitJobs'       in tasks : nOpsSJ = mgr.submitJobs(typeSelection, '', simulate=simulate, options=submitOptions)
    if nOpsHJ == None or nOpsSJ == None or nOpsHJ + nOpsSJ > 0 : dump(['', '--jobs', '--scheduled'], log = mgr)
  except Exception, inst:
    mgr.ERROR('Failed with exception : ' + str(inst))
    traceback.print_exc()
  if not simulate : unlock(mgr)


def logFile(args) :
  log = Logger(OutputStream())
  mgr = CAFManager()
  if not mgr.ok() : return
  actions = []
  if '--get' in args : 
    if 'get' in actions : return log.ERROR('multiple --get statements')
    actions.append('get')
  if '--force' in args : 
    if 'force' in actions : return log.ERROR('multiple --force statements')
    actions.append('force')
  if '--test' in args : 
    if 'test' in actions : return log.ERROR('multiple --test statements')
    actions.append('test')
  if '--events' in args : 
    if 'events' in actions : return log.ERROR('multiple --events statements')
    actions.append('events')
  if '--cpuTime' in args : 
    if 'cpuTime' in actions : return log.ERROR('multiple --cpuTime statements')
    actions.append('cpuTime')
  if 'test' in actions :
    action = 'test'
  elif 'events' in actions :
    action = 'events'
  elif 'cpuTime' in actions :
    action = 'cpuTime'
  else :
    action = None
  if not 'get' in actions and action == None : 
    return log.ERROR('Should specify at least one action (--get or --test or --events)')
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  verbosity = parseVerbosity(args, log)
  data = retrieveData(mgr, log, ['job'], selection, typeSelection, statusSelection)
  for job in data['jobs'] :
    output = mgr.getLogFile(job, force='force' in actions, action=action, keep = 'get' in actions)
    if output == False : continue
    if 'test' in actions :
      if output == None and action == 'test' : output = 'No errors'
    log.INFO(job.name + ' : ' + str(output))


def getOutput(args) :
  log = Logger(OutputStream())
  mgr = CAFManager()
  if not mgr.ok() : return
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  properties = parseProperties(args, ['fileName'], log, withValues = True)
  force = False
  if '--force' in args : 
    force = True
  if not properties['fileName'] : return log.ERROR('Must specify a file name')
  data = retrieveData(mgr, log, ['job'], selection, typeSelection, statusSelection)
  for job in data['jobs'] :
    output = mgr.getOutputs(job, properties['fileName'][0], force=force)


def check(args) :
  simulate = ('-s' in args or '--simulate' in args)
  log = Logger(OutputStream())
  mgr = CAFManager()
  if not mgr.ok() : return
  actions = []
  if '--output' in args : 
    if 'output' in actions : return log.ERROR('multiple --output statements')
    actions.append('output')
  if not 'output' in actions : 
    return log.ERROR('Should specify at least one action (--output)')
  sels = parseSelections(args, log)
  if not sels : return False
  (selection, typeSelection, statusSelection, nMax) = sels
  verbosity = parseVerbosity(args, log)
  data = retrieveData(mgr, log, ['job'], selection, typeSelection, statusSelection)
  for job in data['jobs'] :
    if 'output' in actions :
      output = mgr.checkOutputs(job, simulate=simulate)


def ls(args) :
  if len(args) <= 1 : return Logger(OutputStream()).ERROR('Not enough arguments')
  useCat  = ('-c' in args or '--catalog' in args)
  strTime  = ('-l' in args)
  epochTime  = ('--epoch' in args)
  path = args[-1]
  files = FileAccessor(path, OutputStream(), useCastorCatalog=useCat).getFiles()
  for f in files : 
   disp = f
   if epochTime :
     disp = str(FileAccessor(f, OutputStream()).time()) + ' ' + f
   if strTime :
     disp = time.asctime(time.localtime(FileAccessor(f, OutputStream()).time())) + ' ' + f
   Logger(OutputStream()).INFO(disp)


def parseCategories(args, log) :
  # categories
  doCfg  = ('-c' in args or '--configs' in args or '--config' in args)
  doType = ('-t' in args or '--jobTypes' in args or '--jobType' in args)
  doJob  = ('-j' in args or '--jobs' in args or '--job' in args)
  doFile = ('-f' in args or '--files' in args or '--file' in args)
  categories = []
  if doCfg  : categories.append('config')
  if doType : categories.append('jobType')
  if doJob  : categories.append('job')
  if doFile : categories.append('file')
  return categories


def parseVerbosity(args, log) :
  # verbosity
  verbose1 = ('-v' in args or '--verbose' in args)
  verbose2 = ('-vv' in args)
  verbose3 = ('-vvv' in args)
  verbosity = 0
  if verbose3 : 
    verbosity = 3
  else :
    if verbose2 : 
      verbosity = 2
    else :
      if verbose1 : verbosity = 1 
  return verbosity


def parseProperties(args, testFor, log, withValues = True) :
  properties = {}
  for p in testFor :
    if not ('--' + p) in args :
      properties[p] = None
      continue 
    if p in properties : return log.ERROR('Property ' + p + ' specified twice')
    if not withValues :
      properties[p] = True
      continue
    idx = args.index('--' + p)
    if idx + 1 == len(args) :
      val = ''
    else :
     val = args[idx + 1]
     if val[0] == '-' : val = '' # in case it's the next property...
    if val.find(',,') >= 0 :
      val = [ val.replace(',,', ',') ]
    else :
      vals = val.split(',')
      val = []
      for v in vals : 
        if v.isdigit() :
          val.append(int(v))
        else : 
          val.append(v)
    properties[p] = val
  return properties


def parseSelections(args, log) :

  props = parseProperties(args, ['id', 'name', 'batchId', 'type', 'last'], log)

  # individual selections
  selection = {}
  selection['id']      = props['id']
  selection['name']    = props['name']
  selection['batchId'] = props['batchId']
  # type selection
  typeSelection = props['type']

  # status selection
  statusSelection = None
  ss = []
  if '--success' in args : ss.append('success')
  if '--running' in args : ss.append('running')
  if '--scheduled' in args : ss.append('scheduled')
  if '--not-success' in args : ss.append('not success')
  if '--not-running' in args : ss.append('not running')
  if '--not-scheduled' in args : ss.append('not scheduled')
  if '--failed' in args : ss.append('failed')
  if '--invalid' in args : ss.append('invalid')
  if '--missing-output' in args : ss.append('missing output')
  if len(ss) > 0 : statusSelection = ss

  nMax = props['last']
  if nMax == None or len(nMax) == 0 :
    nMax = -1
  else :
    nMax = nMax[0]
  return (selection, typeSelection, statusSelection, nMax)


def retrieveData(mgr, log, categories, selection, typeSelection, statusSelection, nMax = -1, getJobFiles = True) :
  data = {}
  if 'config' in categories :
    data['configs'] = mgr.db().getAllConfigs(selection)
  if 'jobType' in categories :
    if typeSelection != None :
      log.INFO('Using job type selection = ' + str(typeSelection))
    data['jobTypes'] = mgr.db().getAllJobTypes(typeSelection)
  if 'job' in categories :
    if selection['name'] or selection['id'] or selection['batchId'] :
      log.INFO('Using selection = ' + str(selection))
    if typeSelection :
      log.INFO('Using job type selection = ' + str(typeSelection))
    if statusSelection :
      log.INFO('Using status selection = ' + str(statusSelection))   
    data['jobs'] = mgr.db().getAllJobs(selection, typeSelection, statusSelection, nMax, getJobFiles)
  if 'file' in categories :
    if selection != None :
      log.INFO('Using selection = ' + str(selection))
    if statusSelection != None :
      log.INFO('Using status selection = ' + str(statusSelection))
    data['files'] = mgr.db().getAllFiles(selection, statusSelection, nMax)
  return data

def lock(log, args) :
  hnProcess = subprocess.Popen('hostname', stdout=subprocess.PIPE)
  hostName = hnProcess.communicate()[0][:-1]
  pid = os.getpid()
  log.TRACE('Locking ' + lockFileName)
  os.system('echo "Running ' + " ".join(args) + ' on ' + hostName + ', pid = ' + str(pid) + '" > ' + lockFileName)

def testLock(log) :
  if os.access(lockFileName, os.F_OK) : 
    return log.ERROR('Cannot get lock - aborting')
  return True

def unlock(log) :
  log.TRACE('Removing lock file ' + lockFileName)
  os.system('rm -f ' + lockFileName)


def mailErrors(args) :
  f = open(logFileName, 'r')
  for line in f :
    if line[:5] != 'ERROR' : continue
    tokens = line.split(')')
    timeStr = tokens[0][6:]
    print tokens[0], '"'+timeStr+ '"'
    t = None
    try:
      t = time.strptime(timeStr)
    except:
      print 'Bad time format : ' + timeStr
    print 'time = ', t

