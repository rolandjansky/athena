#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @file   TrigSlimVal.py
# @brief  Checks the output of TrigSlimValidation run logs
# @author Ben Smith <bcsmith@fas.harvard.edu>

import os
import sys
from sets import Set

# Global variables
variables = Set()
failures = list()
testsRun = 0

# variable class
class Variable:
  name = ""
  string = ""

  def __init__(self, name, string):
    self.name = name
    self.string = string
  def __repr__(self):
    return 'Variable \'' + self.name + '\' from string \'' + self.string + '\''

# comparison class
class Comparison:
  name = "Comparison"
  string = ""

  def __init__(self, name, string):
    self.name = name
    self.string = string
  def __repr__(self):
    return 'Comparison \'' + self.name + '\' with definition \'' + self.string + '\''

  def passes(self, test, event):
    # replace words with variable values
    words = self.string.split(' ')
    comparison = '='
    currside = 0
    leftVal = 0
    rightVal = 0
    operation = '+'
    allVals = list()
    for word in words:
      # check if its an operation
      if len(word) == 1 and not word.isdigit():
        if word == '+':
          operation = '+'
          allVals.append('+')
        elif word == '-':
          operation = '-'
          allVals.append('-')
        elif word == '=':
          operation = '+'
          comparison = '='
          allVals.append('=')
          currside = 1
        elif word == '<':
          operation = '+'
          comparison = '<'
          allVals.append('<')
          currside = 1
        elif word == '>':
          operation = '+'
          comparison = '>'
          allVals.append('>')
          currside = 1
        else:
          return 'Error: cannot parse word ' + word + ' from comparison ' + self.string
        continue
      # replace the word with variable value
      val = ''
      if word.startswith('BASE.'):
        val = test.getBaseVal(word.replace('BASE.', ''), event)
      elif word.startswith('MOD.'):
        val = test.getModifiedVal(word.replace('MOD.', ''), event)
      elif word.isdigit():
        val = int(word)
      if isinstance(val, basestring) and val.count('not defined in') > 0:
        val = 0
      if not isinstance(val, int):
        return 'ERROR: Cannot parse word ' + word + ' from comparison ' + self.string
      
      allVals.append('%d' % val)
      if operation == '-':
        val = val * -1
      if currside == 0:
        leftVal += val
      else:
        rightVal += val
    
    if leftVal == 0 and rightVal == 0:
      return 'PASSED'
    if comparison == '=' and leftVal == rightVal:
      return 'PASSED'
    if comparison == '<' and leftVal < rightVal:
      return 'PASSED'
    if comparison == '>' and leftVal > rightVal:
      return 'PASSED'
      
    message = 'FAILED comparison \'' + self.name +'\': ' + self.string
    message += ' ( '
    for x in allVals:
      message += x + ' '
    message += ')'
    message += ' Event: %d' % event
    return message

# test class
class Test:
  name = "TestClass"
  alterableVariables = Set()
  comparisons = Set()
  baseValDictArray = list()
  modifiedValDictArray = list()

  def __init__(self, name):
    self.name = name
  def __repr__(self):
    str = 'Test ' + self.name
    str += '\n\tAlterableVariables: ' + self.alterableVariables.__repr__()
    str += '\n\tComparisons: ' + self.comparisons.__repr__()
    str += '\n\tBase value dictionary array: ' + self.baseValDictArray.__repr__()
    str += '\n\tModified value dictionary array: ' + self.modifiedValDictArray.__repr__()
    return str

  def getBaseVal(self, name, event):
    if not len(self.baseValDictArray) > event:
      return 'No entries in the base value dictionary for event %d' % event
    baseValDict = self.baseValDictArray[event];
    v = getVariable(name)
    if not isinstance(v, Variable):
      return v
    if not v in baseValDict:
      return 'Variable ' + name + ' not defined in the base value dictionary'
    return baseValDict[v]

  def getModifiedVal(self, name, event):
    if not len(self.modifiedValDictArray) > event:
      return 'No entries in the modified value dictionary for event %d' % event
    modifiedValDict = self.modifiedValDictArray[event];
    v = getVariable(name)
    if not isinstance(v, Variable):
      return v
    if not v in modifiedValDict:
      return 'Variable ' + name + ' not defined in the modified value dictionary'
    return modifiedValDict[v]

  def runTests(self):
    global testsRun
    global failures
    
    testsRun = testsRun + 1
    # loop through events and check each one
    if not len(self.baseValDictArray) == len(self.modifiedValDictArray):
      return 'Initialization error: base and modified value dictionary arrays have different sizes'
    i = 0
    for i in range(0, len(self.baseValDictArray)):
      # check that the non-alterable variables have not changed
      for v in variables.difference(map(getVariable, self.alterableVariables)):
        if isinstance(self.getBaseVal(v.name, i),basestring):
          continue
        c = Comparison('UnalterableVariable: ' + v.name, 'BASE.' + v.name + ' = ' + 'MOD.' + v.name)
        val = c.passes(self, i)
        if not val == 'PASSED':
          failures.append(self.name + ': ' + val)
          return val
      # check that the comparisons pass
      for c in self.comparisons:
        val = c.passes(self, i)
        if not val == 'PASSED':
          failures.append(self.name + ': ' + val)
          return val
    # if we made it this far, then we passed!
    return 'PASSED'

# variable functions
def getVariable(name):
  for v in variables:
    if v.name == name:
      return v
  return 'Variable ' + name + ' not found'

def addVariable(name, string):
  if isinstance(getVariable(name), Variable):
    return 'Could not add variable ' + name + '... Variable already exists'
  v = Variable(name, string)
  variables.add(v)
  return v

# parsing functions
def matchLine(line, valDict):
  # ignore blank lines
  if len(line) == 0:
    return ''
  # check if any of the variable strings appear in the line
  for v in variables:
    if line.count(v.string) > 0:
      if v in valDict: 
        return 'Wanted to assign line ' + line + ' to var ' + v.name + ' but ' + v.name + ' is already assigned!'
      # split the line by the colon
      words = line.split(':')
      if not len(words) == 2:
        message = 'Wanted to assign line ' + line + ' to var ' + v.name
        message = message + ' but line has wrong num (' + words.length() + ') of words'
        return messsage
      val = int(words[1].replace(' ', '').replace('\n', ''))
      valDict[v] = val
      return ''
  return 'Unable to match line ' + line
  
def parseEvent(log, valDict):
  for line in log:
    errorVal = matchLine(line, valDict)
    if not errorVal == '': 
      return errorVal 
  return '' 

def parseLog(logFile, valDictArray):
  if not os.path.exists(logFile):
    return 'File ' + logFile + ' does not exist'
  file = open(logFile, 'r')

  # split the log file by event by looking for the lines
  startBlock = 'START of TrigSlimValidation DUMP'
  endBlock = 'END of TrigSlimValidation DUMP'
  log = list()
  inBlock = False
  eventNum = 1

  for line in file.readlines():
    # if we see a start block, prepare to read an event
    if line.count(startBlock) > 0:
      if inBlock:
        return 'Wanted to start event %d, but event %d never closed' % (eventNum, eventNum - 1)
      inBlock = True
      log[:] = []
      continue
    if line.count(endBlock) > 0:
    # if we see an end block, close event and parse it
      if not inBlock:
        return 'Wanted to end event %d, but event was never opened!' % (eventNum)
      valDict = dict()
      errorVal = parseEvent(log, valDict)
      valDictArray.append(valDict)
      if not errorVal == '':
        return errorVal
      inBlock = False
      eventNum = eventNum + 1
      continue
    # if we're in an event block, add the log line
    # otherwise, just ignore the line
    if inBlock:
      log.append(line)
  file.close()

  return ''

def setupTest(test, baseLog, modifiedLog):
  test.baseValDictArray = list()
  errorVal = parseLog(baseLog, test.baseValDictArray)
  if not errorVal == '':
    print '\nParsing the base log ' +  baseLog + ' for test',
    print test.name + ' failed with the following error:'
    print errorVal
    print 'This is unrecoverable... exiting!\n'
    exit()
  test.modifiedValDictArray = list()
  errorVal = parseLog(modifiedLog, test.modifiedValDictArray)
  if not errorVal == '':
    print '\nParsing the modified log ' +  modifiedLog + ' for test',
    print test.name + ' failed with the following error:'
    print errorVal
    print 'This is unrecoverable... exiting!\n'
    exit()


############## TrigSlimValidation specific code #################
def buildObjectList(log, key):
  s = Set()

  if not os.path.exists(log):
    return 'File ' + log + ' does not exist'
  file = open(log, 'r')
  for line in file.readlines():
    if line.count(key) > 0:
      for word in line.split(' '):
        if word.count(':') > 0:
          s.add(word.replace(':', ''))
          break
  return s

def buildStreamRemovedTest(stream, baseLog, modLog, features):
  RSTest = Test('RS' + stream)
  setupTest(RSTest, baseLog, modLog)
  RSTest.alterableVariables = Set(['elements', 'intermediates', 'stream'+stream, 'links', 'features', 'featureless', 'ghosts', 'seeds', 'seededBy', 'sameRoI','longest', 'unassociated'])
  RSTest.alterableVariables.add('unassociatedgroup')
  # All features can change, as entire streams are removed
  for f in features:
    RSTest.alterableVariables.add('feature'+f)
  RSTest.comparisons = Set([Comparison('StreamRemoved', 'MOD.stream' + stream + ' < BASE.stream' + stream)])
  return RSTest

def buildFeatureRemovedTest(feature, baseLog, modLog):
  RFTest = Test('RF'+feature)
  setupTest(RFTest, baseLog, modLog)
  RFTest.alterableVariables = Set(['features','featureless','links','feature'+feature])
  RFTest.comparisons = Set([Comparison('DistinctFeatures', 'BASE.features = MOD.features + 1'),
                        Comparison('FeatureLinks', 'BASE.links < 1 + MOD.links + BASE.feature'+feature),
                        Comparison('FeatureRemoved', 'MOD.feature'+feature+' = 0')])
  return RFTest
  
def buildSqueezeTest(baseLog, modLog, streams, features):
  SqueezeTest = Test('Squeeze')
  setupTest(SqueezeTest, baseLog, modLog)
  SqueezeTest.alterableVariables = Set(['intermediates', 'seeds', 'seededBy', 'sameRoI', 'elements', 'unassociated', 'longest', 'shortest', 'links','unassociatedgroup'])
  for s in streams:
    SqueezeTest.alterableVariables.add('stream'+s)
  SqueezeTest.comparisons = Set([Comparison('IntermeidateElements','MOD.intermediates = 0'),
                             Comparison('Elements', 'MOD.elements + BASE.intermediates = BASE.elements'),
                             Comparison('LongestChain', 'MOD.longest = 3'),
                             Comparison('ShortestChain', 'MOD.shortest < 4')])
  # When you squeeze, you can actually increase the number of occurrences of a given feature
  # if you remove a node with multiple children.  Thus, we need to check that the number
  # of occurrences of each feature is the same or larger
  for f in features:
    SqueezeTest.alterableVariables.add('feature'+f)
    SqueezeTest.comparisons.add(Comparison('FeatureIncrease'+f, 'MOD.feature'+f+' + 1 > BASE.feature' + f)) 
  return SqueezeTest

def buildNoSlimTest(baseLog, modLog):
  # No Slim Test
  # Nothing changes, so this is super easy!
  NoSlimTest = Test("NoSlimTest")
  setupTest(NoSlimTest, baseLog, m)
  return NoSlimTest

def buildCombinedTest(test1, test2):
  test = Test(test1.name + test2.name)
  test.alterableVariables = test1.alterableVariables.union(test2.alterableVariables)
  test.comparisons = test1.comparisons.union(test2.comparisons)
  test.baseValDictArray = test1.baseValDictArray
  test.modifiedValDictArray = test1.modifiedValDictArray
  # There are some special rules for combining RF and squeeze tests
  # Basically, squeeze tests require that the number of occurrences of a feature
  # stays the same or increases.  We need to remove this required when we combine
  # with an RF test, but only for the feature that was removed
  toRemove = Set()
  if test1.name.count('RF') > 0:
    feature = test1.name.replace('RF', '')
    for c in test.comparisons:
      if c.name.startswith('FeatureIncrease') and c.name.count(feature) > 0:
        toRemove.add(c)
  if test2.name.count('RF') > 0:
    feature = test2.name.replace('RF', '')
    for c in test.comparisons:
      if c.name.startswith('FeatureIncrease') and c.name.count(feature) > 0:
        toRemove.add(c)
  test.comparisons = test.comparisons.difference(toRemove)
  return test
  
# Begin main

def main():

  # read command line options
  from optparse import OptionParser
  parser = OptionParser(usage = "usage: %prog [options]",
                       description = "Compares output of TrigSlimValAlg on slimmed and unslimmed AODs to validate TrigNavigationSlimming.")
  parser.add_option("-b", "--base", action = "store", dest = "baseLog", default = "TrigSlimValidation_Base.log", help = "Base log file")
  parser.add_option("-m", "--modified", action = "append", dest = "modifiedLog", default = list(), help = "Modified log file")
  parser.add_option("-t", "--test", action = "append", dest = "testType", default = list(), help = "Test to run")
  parser.add_option("-f", "--feature", action = "append", dest = "removedFeature", default = list(), help = "Removed feature (if needed by test)")
  parser.add_option("-s", "--stream", action = "append", dest = "removedStream", default = list(), help = "Removed stream (if needed by test)")

  (opts, args) = parser.parse_args()

  if len(args) > 0:
    print
    print 'Unable to parse option: ' + args[0]
    print 'This is unrecoverable... exiting!\n'
    print
    exit()

  # Populate all variables
  baseLog = opts.baseLog
  modifiedLogs = opts.modifiedLog
  testTypes = opts.testType
  removedFeatures = opts.removedFeature
  removedStreams = opts.removedStream
  if len(modifiedLogs) == 0 and len(testTypes) == 0:
    print
    print 'You must specify either a modified log (with -m) or a test type (with -t)'
    print
    exit()
  if len(modifiedLogs) == 0:
    for t in testTypes:
      modifiedLog = 'TrigSlimValidation_' + t + '.log'
      print
      print 'Assuming modified log file at ' + modifiedLog + ' based upon test type ' + t
      print
      modifiedLogs.append(modifiedLog)
  if len(testTypes) == 0:
    for l in modifiedLogs:
      frontRemoved = l.split('_')
      if not len(frontRemoved) == 2:
        print
        print 'Unable to determine implicit test type from modified log', 
        print l + '... you should supply it explicity with -t'
        print
        exit()
      backRemoved = frontRemoved[1].split('.')
      if not len(backRemoved) == 2:
        print
        print 'Unable to determine implicit test type from modified log', 
        print l + '... you should supply it explicity with -t'
        print
        exit()
      t = backRemoved[0]
      print
      print 'Assuming test type ' + t + ' based upon modified log file ' + l
      print
      testTypes.append(t)

  # Build feature, stream, and group lists
  features = buildObjectList(baseLog, 'Occurrences of feature')
  if not isinstance(features, Set):
    print
    print 'Unable to build feature list due to the following error:'
    print features
    print 'This is unrecoverable... exiting!\n'
    exit()
  streams = buildObjectList(baseLog, 'Elements associated with stream')
  if not isinstance(streams, Set):
    print
    print 'Unable to build stream list due to the following error:'
    print streams
    print 'This is unrecoverable... exiting!\n'
    exit()
  groups = buildObjectList(baseLog, 'Elements associated with group')
  if not isinstance(groups, Set):
    print
    print 'Unable to build group list due to the following error:'
    print groups
    print 'This is unrecoverable... exiting!\n'
    exit()

  # Define needed variables
  addVariable('elements', 'Number of trigger elements:')
  addVariable('features', 'Number of distinct features:')
  addVariable('intermediates', 'Number of intermediate trigger elements:')
  addVariable('featureless', 'Number of featureless trigger elements:')
  addVariable('ghosts', 'Number of ghost trigger elements:')
  addVariable('RoIs', 'Number of RoIs:')
  addVariable('longest', 'Longest chain:')
  addVariable('shortest', 'Shortest chain:')
  addVariable('links', 'Number of feature links:')
  addVariable('seeds', 'Number of seeds relations:')
  addVariable('seededBy', 'Number of seeded by relations:')
  addVariable('sameRoI', 'Number of same RoI relations:')

  # add feature, stream, and group variables
  for f in features:
    addVariable('feature'+f, 'Occurrences of feature ' + f + ':')
  for s in streams:
    addVariable('stream'+s, 'Elements associated with stream ' + s + ':')
  for g in groups:
    addVariable('group'+g, 'Elements associated with group ' + g + ':')
  addVariable('unassociated', 'Elements unassociated with any stream:')
  addVariable('unassociatedgroup', 'Elements unassociated with any group:')

  # Define some useful tests
  AllTests = list()

  for t,m in zip(testTypes,modifiedLogs):
    if t == 'NoSlim':
      AllTests.append(buildNoSlimTest(baseLog, m))
    if t == 'RF':
      if len(removedFeatures) == 0:
        print
        print "Could not find feature to remove for test " + t + ' with modified log file ' + m
        print
        exit()
      AllTests.append(buildFeatureRemovedTest(removedFeatures.pop(0), baseLog, m))
    if t == 'RS':
      if len(removedStreams) == 0:
        print
        print "Could not find stream to remove for test " + t + ' with modified log file ' + m
        print
        exit()
      AllTests.append(buildStreamRemovedTest(removedStreams.pop(0), baseLog, m, features))
    if t == 'Squeeze':
      AllTests.append(buildSqueezeTest(baseLog, m, streams, features)) 
    if t == 'SqueezeRF':
      if len(removedFeatures) == 0:
        print
        print "Could not find feature to remove for test " + t + ' with modified log file ' + m
        print
        exit()
      SqueezeTest = buildSqueezeTest(baseLog, m, streams, features)
      RFTest = buildFeatureRemovedTest(removedFeatures.pop(0), baseLog, m)
      AllTests.append(buildCombinedTest(SqueezeTest, RFTest))

  # Check the tests
  print
  print 'Running tests...'
  print

  for t in AllTests:
    print t.name + ': ' + t.runTests()

  print
  print 'Tests complete!'
  print

  # Print the final report
  print
  print 'Final report:'
  print 'Tests passed: %d/%d' % (testsRun - len(failures), testsRun)
  print 'Tests failed: %d/%d' % (len(failures), testsRun)
  print

  if len(failures) > 0:
    print
    print "trigslimval.py ERROR: Failures found"
    print
    print 'Failures:'
    for f in failures:
      print f
    print
    print

if __name__ == "__main__":
   sys.exit(main())

