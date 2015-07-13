# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# FileAccessor.py          
#   Provides an interface to transparently access files in various storage types 
#   (implemented so far: UNIX FS, CASTOR through nsls and CASTOR through catalog)

import os, glob, time, stat
from subprocess import Popen, PIPE 
from Logger import Logger 
from Tools import Regexp

castorCatalogFileName = 'atlas.atlcal.last'
castorCatalogUpdateInterval = 4*60*60; # 4h in seconds

class FileAccessor(Logger) :
  def __init__(self, filePath, stream = None, minimumAge = 0, useCastorCatalog = False) :
    Logger.__init__(self, stream)
    self.m_path = filePath
    self.minimumAge = minimumAge
    self.plugins = []
    if useCastorCatalog :
      self.plugins.append(CastorCatalogPlugin(stream))
    else :
      self.plugins.append(CastorPlugin(stream))
    self.plugins.append(FileSystemPlugin(stream))
  
  def getFiles(self) :
    for p in self.plugins :
      if p.canHandle(self.m_path) : 
        #self.INFO('Using ' + p.__class__.__name__ + ' plugin')
        return p.expandPath(self.m_path)
    return None

  def moveToPermanent(self, perm, config, zipIt = False) :
    if not self.m_path.startswith(config.tmpOutputDir) :
      return self.ERROR('Trying to move from non-temp output directory ' + self.m_path)
    if not self.access() :
      self.INFO('Skipping non-existent output file ' + self.m_path)
      return True
    self.INFO('Moving ' + self.m_path)
    for p in self.plugins :
      if p.canHandle(perm) : 
        return p.moveTo(self.m_path, perm, zipIt)
    return False

  def moveTo(self, perm) :
    if not self.access() :
      self.INFO('Skipping non-existent file ' + self.m_path)
      return True
    for p in self.plugins :
      if p.canHandle(perm) : 
        return p.moveTo(self.m_path, perm)
    return False

  def makedirs(self, perm) :
    if FileAccessor(perm).access() : return True
    if not perm.startswith('/castor') :
      if os.access(perm, os.W_OK) : return True
      os.makedirs(perm)
    else :
      result = Popen('rfmkdir -p ' + perm, shell=True, stdout=PIPE, stderr=PIPE).communicate()
      if result[1] != '' : return self.ERROR('Could not rfmkdir ' + perm)
    return True
      
  def stdForm(self) :
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.stdForm(self.m_path)
    return None
      
  def remove(self) : # no error if file non-existent
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.remove(self.m_path)
    self.ERROR('no plugins available to handle path ' + self.m_path)
    return None
  
  def rmdir(self) : # no error if dir non-existent (but error if non-empty)
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.rmdir(self.m_path)
    self.ERROR('no plugins available to handle path ' + self.m_path)
    return None

  def recursiveremove(self) : # no error if non-existent
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.recursiveremove(self.m_path)
    self.ERROR('no plugins available to handle path ' + self.m_path)
    return None

  def access(self) :
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.access(self.m_path)
    self.ERROR('no plugins available to handle path ' + self.m_path)
    return None

  def copy(self, toPath) :
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.copy(self.m_path, toPath)
    self.ERROR('no plugins available to handle path ' + self.m_path)
    return None

  def time(self) :
    for p in self.plugins :
      if p.canHandle(self.m_path) : return p.time(self.m_path)
    self.ERROR('no plugins available to handle path ' + self.m_path)
    return None

class AccessorPlugin(Logger) :
  def __init__(self, stream) :
    Logger.__init__(self, stream)
  
  def executeShellCmd(self, cmd, descr = '', verbose = False) :
    try: 
      p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
      result = p.communicate()
    except Exception, inst:
      if verbose : self.TRACE('result of ' + cmd + ' = ' + str(result))
      if descr != '' : self.ERROR(descr + ' failed with exception : ' + str(inst))
      return None
    if verbose : self.TRACE('result of ' + cmd + ' = ' + str(result))
    if result[1] != '' : 
      if descr != '' : self.ERROR(descr + ' failed with error : ' + str(result[1]))
      return None
    if p.returncode != 0 :
      return self.ERROR(descr + ' failed with exit code : ' + str(p.returncode))
    if result[0] == None : return ''
    return result[0]

  def zipFile(self, name) :
    result = self.executeShellCmd('mv ' + name + ' ' + name[:-3] + '; gzip ' + name[:-3], 
                                  'Zipping file')
    return result


class FileSystemPlugin(AccessorPlugin) :
  def __init__(self, stream = None) :
    AccessorPlugin.__init__(self, stream)
    
  def canHandle(self, path) :
    return True
  
  def expandPath(self, path) :
    files = glob.glob(path)
    return files
  
  def moveTo(self, fromPath, toPath, zipIt = False) :
    if os.access(toPath, os.W_OK) : return True
    os.makedirs(toPath) # this is the recursive mkdir
    if zipIt :
      result = self.zipFile(fromPath)
      if not result : return False
    result = self.executeShellCmd('mv ' + fromPath + ' ' + toPath, 
                                'Moving output to permanent location')
    return (result != None)
    
  def copy(self, fromPath, toPath) :
    if not os.access(toPath, os.W_OK) : return False
    result = self.executeShellCmd('cp ' + fromPath + ' ' + toPath, 
                                'Copying ' + fromPath + ' to ' + toPath)
    return (result != None)

  def stdForm(self, path) :
    return path

  def remove(self, path) :
    if not os.access(path, os.F_OK) : return True
    try: os.remove(path)
    except Exception, inst: return self.ERROR('Could not remove ' + path + ' : ' + str(inst))
    return True
    
  def rmdir(self, path) :
    try: os.rmdir(path)
    except Exception, inst: return self.ERROR('Could not rmdir ' + path + ' : ' + str(inst))
    return True
    
  def recursiveremove(self, path) :
    result = self.executeShellCmd('rm -rf ' + path, 'Removing ' + path)
    return (result != None)
    
  def access(self, path) :
    return os.access(path, os.F_OK)

  def time(self, path) :
    return os.stat(path)[stat.ST_MTIME]

class CastorBasePlugin(AccessorPlugin) :
  def __init__(self, stream = None) :
    AccessorPlugin.__init__(self, stream)
  
  def canHandle(self, path) :
    path = self.castorPath(path)
    if path != None : return True
    return False
  
  def castorPath(self, path) :  # formats the path in a unique way : /castor/...
    idx = path.find('/castor/')
    if idx > 0 : path = path[idx:]
    if path.startswith('/castor/cern.ch/') : return path
    return None

  def moveTo(self, fromPath, toPath, zipIt = False) : # fromPath is FS-based
    if not self.access(toPath) :
      if self.executeShellCmd('rfmkdir -p ' + toPath, 'Could not rfmkdir ' + toPath) == None : return False
    if zipIt :
      result = self.zipFile(fromPath)
      if not result : return False
    result = self.executeShellCmd('rfcp ' + fromPath + ' ' + toPath + ' && rm -r ' + fromPath, 
                                  'Moving output to permanent CASTOR location')
    return (result != None)
  
  def copy(self, fromPath, toPath) : # toPath is FS-based
    if self.executeShellCmd('rfcp ' + self.castorPath(fromPath) + ' ' + toPath, \
      'Could not stage ' + fromPath + ' to ' + toPath) == None : return False
    return True

  def stdForm(self, path) :
   castor = self.castorPath(path)
   if castor == None : return None
   return 'rfio:' + castor
  
  def remove(self, path) :
    result = self.executeShellCmd('nsrm -f ' + self.castorPath(path), 'Removing file ' + self.castorPath(path))
    if result == None :
      self.ERROR('could not remove file ' + self.castorPath(path))
      return False
    return True
  
  def rmdir(self, path) :
    files = os.path.join(path, '*')
    if len(self.expandPath(files)) > 0 : return ERROR('Cannot remove non-empty dir ' + path)
    result = self.executeShellCmd('nsrm -fr ' + self.castorPath(path), 'Removing dir ' + self.castorPath(path))
    return (result != None)
  
  def recursiveremove(self, path) :
    result = self.executeShellCmd('nsrm -rf ' + self.castorPath(path), 'Removing ' + self.castorPath(path))
    return (result != None)

  def stageAll(self, pathLL, toPath) :
    if not toPath : return (False, None) 
    self.INFO('Before staging: free disk on ' + toPath)
    os.system('df -h ' + toPath)
    result = True
    newLL = []
    successes = []
    for group in pathLL :
      newGroup = []
      for f in group :         
        self.INFO('Staging ' + f)
        if not self.canHandle(f) :
          newGroup.append(f)
          continue
        if not self.copy(f, toPath) : 
          result = False
          break
        newFile = os.path.join(toPath, os.path.split(f)[1])
        self.INFO('Successfully staged ' + newFile)
        successes.append(newFile)
        newGroup.append(newFile)
      newLL.append(newGroup)
    if not result :
      for f in successes :
        self.executeShellCmd('rm -f ' + f)
      newLL = pathLL
    self.INFO('After staging : free disk on ' + toPath)
    os.system('df -h ' + toPath)
    return (result, newLL)
    


class CastorPlugin(CastorBasePlugin) :
  def __init__(self, stream = None) :
    CastorBasePlugin.__init__(self, stream)
  
  def expandPath(self, path) :
    path = self.castorPath(path)
    paths = path.split('/')
    if len(paths) < 4 : return []
    return self.recursiveExpand([ '/castor/cern.ch' ], paths[3:])
  
  def recursiveExpand(self, combs, suffices) :
    #print 'Recurse : ' + combs[0] + ' ' + str(suffices)
    if len(suffices) == 0 : return combs
    regexp = Regexp(suffices[0])
    # no-regexp case
    if regexp.groups == 0 :
      return self.recursiveExpand([ comb + '/' + suffices[0] for comb in combs ], suffices[1:])
    newCombs = []
    for comb in combs :
      result = self.executeShellCmd('nsls ' + comb)       
      branches = result.split('\n')[:-1]
      #print 'Branches : ' + str(branches)
      nCalls = 0
      for branch in branches :
        newComb = comb + '/' + branch
        nCalls = nCalls + 1
        if not regexp.match(branch) : 
          #TRACE(newComb + ' rejected!')
          continue
        #print '===> Add', newComb
        newCombs.append(newComb)
    #print 'nCalls = ', nCalls, combs
    if len(newCombs) == 0 : return []
    return self.recursiveExpand(newCombs, suffices[1:])

  def access(self, path) :
    result = self.executeShellCmd('nsls ' + self.castorPath(path))
    if result == None : return False
    return True

  def time(self, path) :
    result = self.executeShellCmd('nsls -ld ' + self.castorPath(path))
    if result == None : return -1
    tokens = result.split()
    # problem: this may be Month/day/time or Month/day/year
    month = tokens[5]
    day = tokens[6]
    if tokens[7].find(':') > 0 :
      hhmm = tokens[7]
      current = time.localtime()
      year = time.localtime()[0]
      hypo1 = time.strptime(month + ' ' + day + ' ' + str(year), '%b %d %Y')
      if time.mktime(hypo1) > time.mktime(current) :
        year = year - 1
      year = str(year)
    else :
      hhmm = '00:00'
      year = tokens[7]
    t = time.strptime(month + ' ' + day + ' ' + year + ' ' + hhmm, '%b %d %Y %H:%M')
    #return month + ' ' + day + ' ' + year + ' ' + hhmm + ' ' + str(int(time.mktime(t)))
    return int(time.mktime(t))


class CastorCatalogPlugin(CastorBasePlugin) :
  def __init__(self, stream = None) :
    CastorBasePlugin.__init__(self, stream)
  
  def castorPath(self, path) :  # formats the path in a unique way : /castor/...
    idx = path.find('/castor/')
    if idx > 0 : path = path[idx:]
    if path.startswith('/castor/cern.ch/') : return path
    return None

  def expandPath(self, path) :
    path = self.castorPath(path)
    if not self.getCatalog() : return []
    return self.findInCatalog(path)

  def catalogDir(self) :
    tmpDir = os.getenv("TMPDIR")
    if not tmpDir : tmpDir = '/tmp'
    return tmpDir

  def catalogLocation(self) :
    return os.path.join(self.catalogDir(), castorCatalogFileName)

  def getCatalog(self) :
    return self.downloadCatalog()  # wget -N does exactly what we need without more checks...
    #self.INFO('Checking for catalog ' + castorCatalogFileName)
    #if not os.access(self.catalogLocation(), os.R_OK) : 
    #  self.INFO('Catalog not found, downloading new one...')
    #  if not self.downloadCatalog() : return False
    #else :
    #  stats = os.stat(self.catalogLocation())
    #  if time.time() - stats.st_ctime > castorCatalogUpdateInterval :
    #    self.INFO('Catalog age is ' + str(int(time.time() - stats.st_ctime)/3600) + ' hours, downloading new one')
    #    if not self.downloadCatalog() : return False
    #    self.INFO('Download successful')
    #return True
      
  def downloadCatalog(self) :
    self.INFO('Retrieving catalog (location = ' + self.catalogLocation() + ')')
    cmd = 'cd ' + self.catalogDir() + '; wget -N http://castor.web.cern.ch/castor/DiskPoolDump/' + castorCatalogFileName
    result = self.executeShellCmd(cmd) # , '', True)
    if not os.access(self.catalogLocation(), os.R_OK) : 
      self.TRACE('Unable to retrieve catalog, retrying (command = ' + cmd + ')')
      result = self.executeShellCmd(cmd)
      if not os.access(self.catalogLocation(), os.R_OK) : 
        self.TRACE('Contents of /tmp/larcomm: ' + str(self.executeShellCmd('ls -l /tmp/larcomm')))
        return self.ERROR('Unable to download catalog after 2 tries, aborting')
    self.INFO('Got catalog')
    return True
 
  def findInCatalog(self, path) :
    if not os.access(self.catalogLocation(), os.R_OK) :
      return self.ERROR('Unable to access catalog ' + self.catalogLocation() + ', aborting')
    f = open(self.catalogLocation(), 'r')
    regexp = Regexp(path)
    matches = []
    i = 0
    for line in f :
      i = i + 1
      if i % 500000 == 0 : self.INFO('Processing line ' + str(i))
      fields = line.split(' ')
      if len(fields) == 0 : continue
      if regexp.match(fields[0]) : 
        ts = time.strptime(fields[13] + " " + fields[14] + " " + fields[15]  + " " + fields[16], "%d %b %Y %H:%M.%S")
        matches.append((fields[0], time.mktime(ts)))
    #matches.sort(key=lambda f: f[1])
    matches.sort()
    return [f[0] for f in matches]

  def access(self, path) :
    found = self.findInCatalog(path)
    if len(found) >= 1 : return True
    return False

