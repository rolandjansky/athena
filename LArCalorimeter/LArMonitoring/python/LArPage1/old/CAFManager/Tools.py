# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# author: N. Berger (Nicolas.Berger@cern.ch)
#
# Tools.py          
#   Various helper functions/classes

from Logger import Logger, OutputStream
import os, re

class AllowedValues :
  def __init__(self, values) :
    self.m_values = {}
    for value in values : 
      self.m_values[value] = True
  def isAllowed(self, value) :
    return value in self.m_values

def joinFileRecords(l, j) :
  nameList = []
  for f in l :
    nameList.append(f.name)
  return j.join(nameList)

def getRunNumber(fileName) :
  specs = os.path.split(fileName)[1].split('.')
  specsDir = os.path.split(os.path.split(fileName)[0])[1].split("_")
  if len(specs) >= 2 and specs[1].isdigit() :
    runNum = specs[1]
  elif len(specs) >= 3 and specs[0] == 'daq' and specs[1] == 'ATLAS' and specs[2].isdigit() :
    runNum = specs[2]
  elif len(specsDir) >= 2 and specsDir[1].isdigit() :
    runNum = specsDir[1]
  else :
    return None
  return runNum
  
def getProjectTag(fileName) :
  specs = os.path.split(fileName)[1].split('.')
  if len(specs) == 0 : return None
  pt = specs[0]
  if pt[:4] == 'data' : return pt
  if pt[:2] == 'mc' : return pt
  return None

def getRunNumbers(fileList) :
  runs = []
  for f in fileList : 
    run = getRunNumber(f)
    if run == None : continue
    if run in runs : continue
    runs.append(run)
  return runs

def getProjectTags(fileList) :
  pts = []
  for f in fileList : 
    pt = getProjectTag(f)
    if pt == None : continue
    if pt in pts : continue
    pts.append(pt)
  return pts

def getInputRunNumbers(cafInputs) :
  runs = []
  for i in cafInputs :
    inputRuns = getRunNumbers(i)
    for run in inputRuns :
      if not run in runs : runs.append(run)
  return runs

def getInputProjectTags(cafInputs) :
  pts = []
  for i in cafInputs :
    inputTags = getProjectTags(i)
    for pt in inputTags :
      if not pt in pts : pts.append(pt)
  return pts

def stage(inputs, outputDir) :
  if not outputDir : outputDir = os.getenv('WORKDIR')
  from FileAccessor import CastorPlugin
  castor = CastorPlugin(OutputStream())
  stageResult = castor.stageAll(inputs, outputDir)
  if stageResult[0] == False :
    print 'Could not stage some files from CASTOR : keeping the CASTOR paths - Good Luck!'
    return inputs
  inputs = stageResult[1]
  for group in inputs :
    for f in group : os.system('ls -l ' + f)
  return inputs

class StdRegexp(Logger) :
  def __init__(self, exp, stream = None) :
    Logger.__init__(self, stream)
    exp = exp.replace('\*', '*') # allow quoting * : this is useful when the path was given on the command-line
    exp = exp.replace('*', '(.*)')
    self.exp = exp
    self.matcher = re.compile(exp)

  def isvalid(self) :
    return self.matcher.groups > 0
  
  def groups(self) :
    return self.matcher.groups
  
  def regexp(self) :
    return self.exp

  def match(self, value) :  
    matchObj = self.matcher.match(value)
    if matchObj == None : return None
    if matchObj.span() != (0, len(value)) : return None
    return matchObj
  
class RangeRegexp(Logger) :
  def __init__(self, exp, stream = None) :
    Logger.__init__(self, stream)
    self.exp = ''
    
    # regexp of the form foo<2-55>bar : => match foo2bar...foo55bar
    i1 = exp.find('<')
    if i1 < 0 : 
      self.ERROR('RangeRegexp : no < in ' + exp)
    s1 = exp[i1 + 1:]
    self.head = exp[0:i1]
    i2 = s1.find('>')
    if i2 < 0 : 
      self.ERROR('RangeRegexp : no matching > in ' + exp)
      return      
    s2 = s1[0:i2]
    self.tail = s1[i2 + 1:]
    i3 = s2.find('-')
    if i3 < 0 : 
      self.ERROR('RangeRegexp : no matching - in ' + exp)
      return
    n1 = s2[0:i3]
    n2 = s2[i3+1:]
    if not n1.isdigit() or not n2.isdigit() : 
      self.ERROR('RangeRegexp : non-digit limits in ' + suffices[0])        
      return
    self.n1 = int(n1)
    self.n2 = int(n2)
    self.exp = exp
    self.subRegexp = Regexp(self.head + '*' + self.tail)
    self.rangeGroup = Regexp(self.head).groups() # count the groups in the head to find the index of the relevant one

  def isvalid(self) :
    return (self.exp != '')
   
  def groups(self) :
    if not self.isvalid() : return 0
    return self.subRegexp.groups()

  def regexp(self) :
    if not self.isvalid() : return ''
    return self.subRegexp.regexp()

  def match(self, value) :
    if not self.isvalid() : return False
    match = self.subRegexp.match(value)
    if not match : return False
    if len(match.groups()) < self.rangeGroup :
      return self.ERROR('RangeRegexp : group index inconsistency!')
    numStr = match.group(self.rangeGroup + 1) # group(0) is the whole thing, for some reason
    if not numStr.isdigit() : return False
    num = int(numStr)
    if num < self.n1 or num > self.n2 : return False
    return True


class Regexp(Logger) :
   def __init__(self, exp, stream = None) :
     Logger.__init__(self, stream)
     self.worker = RangeRegexp(exp, None)
     if self.worker.isvalid() : 
       self.worker.stream = stream
       return
     self.worker = StdRegexp(exp, None)
     if self.worker.isvalid() :
       self.worker.stream = stream
       return
     self.worker = None
     self.exp = exp
     
   def match(self, value) :
     if self.worker == None : return (value == self.exp)
     return self.worker.match(value)
   
   def groups(self) :
     if not self.worker : return 0
     return self.worker.groups()
   
   def regexp(self) :
    return self.worker.regexp()

class RegexpList(Logger) :

  def __init__(self, exps, stream = None) :
    Logger.__init__(self, stream)
    self.regexps = []
    for exp in exps : self.regexps.append(Regexp(exp, stream))

  def match(self, value) :
    for regexp in self.regexps :
      if regexp.match(value) : return True  
    return False

  def regexps(self) :
    return self.regexps  
