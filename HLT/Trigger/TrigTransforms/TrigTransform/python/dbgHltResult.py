#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#### This is the dbgHLTResult class for the Debug Stream event analysis

from __future__ import print_function
import logging
import ROOT

from CLIDComps.clidGenerator import clidGenerator
clidg = clidGenerator("")

msg = logging.getLogger('PyJobTransforms.' + __name__)


class hltResult(ROOT.HLT.HLTResult):
  def __init__(self):
    super( hltResult, self ).__init__()
    self.hltResultVector = ROOT.std.vector('unsigned int')()


  def load(self, rob):
    self.nav_payload = []
    data = list(rob.rod_data())
    self.hltResultVector.clear()
    self.hltResultVector.reserve(len(data))
    [ self.hltResultVector.push_back(i) for i in data ]

    if not self.deserialize(self.hltResultVector):
      raise Exception('deserialization of the HLT result failed')
    self.__unpackNavigation()

    
  def __unpackNavigation(self):    
    navData = list(self.getNavigationResult())
    if len(navData) <= 1:
      msg.debug("Navigation data not available")
      return
    
    featureIdx = navData[2]
    blob = getFeatureDataBlob(navData, featureIdx)
  
    while len(blob):
      featureSize = len(blob)
      clid = blob[0]
      stidx = blob[1]
      slabel = blob[2]
      lwords = blob[3:3+slabel]
      sdata = len(blob) - slabel - 3
      
      label = deserializeString(lwords)
      self.nav_payload.append((clidg.getNameFromClid(clid), label, featureSize * 4, sdata * 4, clid, stidx))
      featureIdx = featureIdx + featureSize + 1
      blob = getFeatureDataBlob(navData, featureIdx)


  def appName(self):
    if self.getExtras().size() > 1:
      return deserializeString(self.getExtras())
    else:
      return ''
      
    
def getFeatureDataBlob(data, index):
  if index == len(data):
    msg.debug("Index out of range")
    return []

  size = data[index]
  begin = index + 1
  end = index + size + 1

  if end <= len(data):
    index = end
    msg.debug("Getting blob of data from: {0} to {1}".format(begin, end))
    return data[begin:end]
  else:
    msg.debug("Data out of range")
    return []


def deserializeString(lwords):
  """Wrapper for the C++ StringSerializer"""
  
  v = ROOT.std.vector['unsigned int']()
  s = ROOT.std.string()
  v.reserve(len(lwords))
  for w in lwords: 
    v.push_back(w)
    
  stringSerializer = ROOT.StringSerializer()
  stringSerializer.deserialize(v, s)

  return str(s)

  
#############################################################################
# Printing utils used for debugging

def print_ranges(l):
  for i in range(len(l)):
    print('{0:-16d}{1:16d}'.format((i+1)*32, i*32))


def print_chain(counter, s):
  ch = ROOT.HLT.Chain(s)
  ch.deserialize(s)
  print('.... chain {0:-3d} Counter: {1:-4d} Passed: {2} (Raw: {3} Prescaled: {4} PassThrough: {5}) Rerun: {6} LastStep: {7} Err: {8}'
        .format( counter, ch.getChainCounter(), ch.chainPassed(), ch.chainPassedRaw(), ch.isPrescaled(), \
            ch.isPassedThrough(), ch.isResurrected(), ch.getChainStep(), ch.getErrorCode().str()))


def print_all_chains(blob):
  print('... chains:')
  for i in range(len(blob)):
    print_chain(i, blob[i])


def print_all_navigation(result):
  print('... features')
  for f in result.nav_payload:
    print('.... {0:-52s} {1:6d} B '.format(f[0]+'#'+f[1], f[2]))
  return


def print_HLTResult(result):    
  # If opt.sizes:
  print('... Payload size: {0} {1}kB'.format(result.as_int_v.size(), (4.0*result.as_int_v.size())/(1024)))

  if result.as_int_v.size() == 0:
    print('... Payload size is 0')

  # Header info
  version = result.getHLTResultClassVersion()
  l1id = result.getLvl1Id()
  acc = result.isAccepted()
  pt = result.isPassThrough()
  status = result.getHLTStatus()
  cnvstatus = result.getLvlConverterStatus()
  level = result.getHLTLevel()
  nosigs = result.getNumOfSatisfiedSigs()
  bad = result.isCreatedOutsideHLT()
  trunc = result.isHLTResultTruncated()
  print('... Version:', version ,' Lvl1Id:',l1id ,' Decision:',acc ,\
        ' PassThrough:',pt,' Status:',status.str(),\
        ' ConverterStatus:', cnvstatus.str(),' LVL:',level, \
        ' Signatures:',nosigs,' Bad:',bad,' Truncated:', trunc,\
        ' App:', result.appName())
  print()

  chains_data = list(result.getChainResult())
  nchains = chains_data[0] if chains_data else 0
  nav_data = list(result.getNavigationResult())
  nnav = len(nav_data)
  nver = nav_data[0] if nav_data else 0

  # If opt.sizes:
  print('... tot:', result.as_int_v.size(), ' chains:', nchains, ' chains (expected):', len(chains_data)-1, \
          ' navigation:', nnav, ' navigation (expected):',result.getNavigationResult()[1] if nnav > 1 else "0 or 1")

  # If opt.conf:
  print('... SMkey: {0} Prescalers key {1}'.format(result.getConfigSuperMasterKey(), result.getConfigPrescalesKey()))

  # If opt.chains:
  print_all_chains(chains_data[1:])
    
  print('... Navigation version: ', nver)

  # If opt.tes:
  if nnav > 3:
    tessize = result.getNavigationResult()[2]
    tescount = result.getNavigationResult()[3]
    print('... Number of TEs: {0} and size: {1} {2}kB'.format(tescount, tessize, 4.0*tessize/(1024)))
  else:
    print('... Cannot print TriggerElement details (not enough navigation data)')

  # If opt.features:
  print_all_navigation(result)


##############################################################################
# Collect the sizes
def collect_feature_sizes(dest, result):
  for f in result.nav_payload:
    key = f[0] + '#' + f[1]
    if key not in dest:
      dest[key] = 0
    dest[key] += f[2]

    # Keyed by type
    key = f[0]
    if key not in dest:
      dest[key] = 0
    dest[key] += f[2]

  if 'Total' not in dest:
    dest['Total'] = 0
  dest['Total'] += 4*result.as_int_v.size()
