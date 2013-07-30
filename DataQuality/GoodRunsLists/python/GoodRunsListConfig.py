# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class GoodRunsListConfig:
  def __init__(self):
    # -- run cmd options
    self.name                   = "GoodRunsListConfig"                  # -- name set inside GRLB xml output file
    self.querytool     		= "AtlRunQuery.py"			# -- run query tool
    self.listname      		= "default.xml"				# -- name of GRL xml output file
    self.prefix                 = ""                                    # -- prefix for GRL file name
    self.show                   = ""                                    # -- show arguments for atlrunquery
    self.dqfolder		= "DEFECTS"				# -- DQ folder, eg. LBSUMM, SHIFTOFL. Default by AtlRunQuery

    # -- query selections in AtlRunQuery.py
    # -- see Database/CoolRunQuery/python/AtlRunQueryParser.py for full list
    self.querydict = {}
    self.querydict['partition']	 = ["ATLAS"]				# -- DAQ partition
    self.querydict['ptag'] 	 = ["data09*"]			        # -- projecttag: collection filename prefix
    #self.querydict['events']	 = ["1000+"]				# -- number of events per run
    self.querydict['run']	 = ["140541+"]			        # -- run number selection range
    #self.querydict['duration']	 = ["60s+"]				# -- duration of run selection
    self.querydict['time']	 = []					# -- time of run selection, eg.
    self.querydict['detector']	 = [] #['all']				# -- subdetectors turned on, eg. 
    self.querydict['streams']	 = []					# -- trigger output streams, eg. 
    self.querydict['mag']	 = [] #["solenoidon","toroidon"]	# -- magnetic field configuration
    self.querydict['dq']	 = []					# -- selection of data quality flags
    self.querydict['smk']	 = []					# -- super master key, eg. 
    self.querydict['trigger']	 = []					# -- select specific triggers available, eg.
    self.querydict['release']    = []					# -- release used, eg: '15.1.*'
    #self.querydict['ctag']      = ['DetStatusLBSUMM-pass1']		# -- COOL conditions tag to use, eg: 'COMCOND-HLTC-001-00'. Default = head.
    self.querydict['db']         = ['DATA']                             # -- DATA or MC
    self.dqignore	 = []					# DQ defects to ignore.  These will be patched into the dq querydict


  def Summary(self):
    print 'Name         : %s' % (self.name)
    print 'Tool         : %s' % (self.querytool)
    print 'File         : %s' % (self.listname)
    print 'Show         : %s' % (self.show)
    print 'DQ folder    : %s' % (self.dqfolder)
    print 'Query        :'
    print self.getsearchquery()
    return


  def getsearchquery(self):
    cmd = "find "
    ## search arguments
    for key in self.querydict.keys():
      arr = self.querydict[key][:]
      if key == 'dq':
        if len(self.dqignore) > 0:
            toexclude = '\\' + '\\'.join(self.dqignore)
        else:
            toexclude = ''
        print 'to exclude', toexclude
        for i, q in enumerate(arr):
            qsplit = q.split()
            if len(qsplit) > 0:
              arr[i] = ' '.join([qsplit[0] + toexclude] + qsplit[1:])
      for selection in arr:
        if len(selection)>0:
          cmd += "%s %s and " % (key,selection)
    cmd = cmd[:-4]
    ## show arguments
    if len(self.show)>0: 
      cmd += " %s" % (self.show)
    else: cmd += " / show run and events / nodef" 
    ## grl name and filename
    xmlstring = 'xmlfile %s:%s' % (self.listname,self.name)
    cmd += " , %s" % (xmlstring)
    return cmd


  def getparseddqflags(self):
    from CoolRunQuery.AtlRunQueryLib import DQSelector,DQCondition,AtlRunQueryOptions
    from CoolRunQuery.AtlRunQueryParser import ArgumentParser

    ## let atlrunquery do first parse of arguments
    query  = self.getsearchquery()
    ap = ArgumentParser()
    atlqueryarg = self.querytool + " " + ap.ParseArgument( query )
    (options, args) = AtlRunQueryOptions().parse(atlqueryarg)

    ## second step, parse actual dq selections into formulas
    selector = DQSelector(name = 'dataquality', dq = options.dqchannels)
    dqfolder = self.getdqfolder() 

    funcArr = []
    dqconfig = selector.selectors[dqfolder]
    for flag in dqconfig.flags: 
      fncName = dqconfig.flagInterpreter[flag]['passFncName']
      fncName = fncName.replace("\'","")
      funcArr.append(fncName)
    #funcArr  = selector.selectors[dqfolder].passFncName
    #for i in range(len(funcArr)): funcArr[i] = funcArr[i].replace("\'","")

    ## and rebuild string to pass on
    dqchannels = options.dqchannels
    for i in range(len(dqchannels)):
      dqArr = dqchannels[i].split()
      dqArr[1] = funcArr[i]
      dqchannels[i] = ' '.join(dqArr)

    return dqchannels


  def getdqfolder(self):
    return self.dqfolder

  def setdqfolder(self,dqfolder=''):    
    if len(dqfolder)>0: self.dqfolder = dqfolder
    for idx in range( len(self.querydict['dq']) ):
      dqquery = self.querydict['dq'][idx].strip()
      queryArr = dqquery.split()
      ## save ctag
      if len(queryArr)==3:
        ctagArr = queryArr[1].split('#')
        if len(ctagArr)==2:
          queryArr[1] = '%s#%s' % (self.dqfolder,ctagArr[1])
        else: queryArr[1] = self.dqfolder 
      ## simple
      elif len(queryArr)==2:
        queryArr.insert(1,self.dqfolder)
      dqquery = '%s %s %s' % (queryArr[0],queryArr[1],queryArr[2])
      self.querydict['dq'][idx] = dqquery
    return

  def setdqctag(self,dqctag):
    if len(dqctag)==0: return
    for idx in range( len(self.querydict['dq']) ):
      dqquery = self.querydict['dq'][idx].strip()
      queryArr = dqquery.split()
      ## save ctag
      if len(queryArr)==3:
        ctagArr = queryArr[1].split('#')
        queryArr[1] = '%s#%s' % (ctagArr[0],dqctag)
      ## simple
      elif len(queryArr)==2:
        dqfoldtag = '%s#%s' % (self.dqfolder,dqctag)
        queryArr.insert(1,dqfoldtag)
      dqquery = '%s %s %s' % (queryArr[0],queryArr[1],queryArr[2])
      self.querydict['dq'][idx] = dqquery
    return

  def setctag(self,ctag):
    if len(ctag)>0: self.querydict['ctag'] = ctag

  def getctag(self):
    return self.querydict.get('ctag', '')

  def setPrefix(self,newprefix):
    self.prefix = newprefix
    self.listname = self.prefix+self.listname

