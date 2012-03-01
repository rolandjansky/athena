if ('sampleList' in dir()) or ('sampleFile' in dir()):
  #################################################################################################
  # Provide input for the FileStager here
  #################################################################################################
  
  ## import filestager tool
  from FileStager.FileStagerTool import FileStagerTool
  
  if ('sampleList' in dir()):
    stagetool = FileStagerTool(sampleList=sampleList)
  elif ('sampleFile' in dir()):
    print "FileStager() : Processing sample file : %s" % sampleFile
    stagetool = FileStagerTool(sampleFile=sampleFile)
  
  ## Configure rf copy command used by the stager; default is 'lcg-cp -v --vo altas -t 1200'
  stagetool.CpCommand = "rfcp"
  stagetool.CpArguments = []
  stagetool.OutfilePrefix = ""
  stagetool.checkGridProxy = False
  #stagetool.LogfileDir = "./"
  
  #################################################################################################
  # Configure the FileStager -- no need to change these lines
  #################################################################################################
  
  ## get Reference to existing Athena job
  from AthenaCommon.AlgSequence import AlgSequence
  thejob = AlgSequence()
  
  ## check if collection names begin with "gridcopy"
  print "FileStager() : doStaging ?", stagetool.DoStaging()
  
  ## Import file stager algorithm
  from FileStager.FileStagerConf import FileStagerAlg
  
  ## filestageralg needs to be the first algorithm added to the thejob.
  if stagetool.DoStaging():
     thejob += FileStagerAlg('FileStager')
     thejob.FileStager.InputCollections = stagetool.GetSampleList()
     #thejob.FileStager.PipeLength = 2
     #thejob.FileStager.VerboseStager = True
     #thejob.FileStager.KeepLogfiles = True
     thejob.FileStager.LogfileDir    = stagetool.LogfileDir
     thejob.FileStager.BaseTmpdir    = stagetool.GetTmpdir()
     thejob.FileStager.InfilePrefix  = stagetool.InfilePrefix
     thejob.FileStager.OutfilePrefix = stagetool.OutfilePrefix
     thejob.FileStager.CpCommand     = stagetool.CpCommand
     #thejob.FileStager.CpArguments   = stagetool.CpArguments
     thejob.FileStager.FirstFileAlreadyStaged = stagetool.StageFirstFile
     thejob.FileStager.StoreStatistics = False
  
  #################################################################################################
  # Pass collection names to EventSelector
  #################################################################################################
  
  ## set input collections
  ic = []
  if stagetool.DoStaging():
    ic = stagetool.GetStageCollections()
  else:
    ic = stagetool.GetSampleList()
  
  ## assume we're dealing with AODs, else ESDs
  poolESDInput = False
  if len(ic)>0:
    if ic[0].find('ESD')>0: poolESDInput = True
  
  ## default: EventSelector
  try:
    svcMgr = theApp.serviceMgr()
    svcMgr.EventSelector.InputCollections = ic
    #svcMgr.EventSelector.SkipBadFiles = True
  except Exception,inst:
    pass
  
  ## else athenaCommonFlags
  if not poolESDInput:
    try:
      ## the Input AOD File(s)
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.PoolAODInput = ic
    except Exception,inst:
      pass
  else:
    try:
      ## the Input ESD File(s)
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.PoolESDInput = ic
    except Exception,inst:
      pass
  
  
