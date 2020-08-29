#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#### This is the dbgHLTResult class for the Debug Stream event analysis

from __future__ import print_function

from PyUtils import RootUtils
ROOT = RootUtils.import_root()
from ROOT import std
import cppyy
from CLIDComps.clidGenerator import clidGenerator

clidg = clidGenerator("")


ActualHLTResult=ROOT.HLT.HLTResult
stringSerializer = ROOT.StringSerializer()

class hltResult(ActualHLTResult):
  def __init__(self):
    super( hltResult, self ).__init__()
    self.as_int_v = std.vector('unsigned int')()
  
  def load(self, rob):
    self.nav_payload = []

    data = list(rob.rod_data())
    self.as_int_v.clear();
    self.as_int_v.reserve(len(data))
    [ self.as_int_v.push_back(i) for i in data ]

    if self.deserialize(self.as_int_v) == False:
      raise Exception('deserialization of the HLT result failed')
    self.__unpack_navigation()

    
  def __unpack_navigation(self):    
    nav_data = list(self.getNavigationResult())
    if len(nav_data) <= 1:
      return
    
    fidx=nav_data[2]
    blob = get_feature_data_blob(nav_data, fidx)
    while len(blob):
      fsize = len(blob)
      clid = blob[0]
      stidx = blob[1]
      slabel =  blob[2]
      lwords = blob[3:3+slabel]
      sdata = len(blob)-slabel-3
      
      label = deserialize_string(lwords)
      self.nav_payload.append( (clidg.getNameFromClid(clid), label, fsize*4, sdata*4, clid, stidx) )
      fidx = fidx+fsize+1
      blob = get_feature_data_blob(nav_data, fidx)

  def appName(self):
    if self.getExtras().size()>1:
      return deserialize_string(self.getExtras())
    else:
      return ''
      
    
def get_feature_data_blob(data, index):
  if index == len(data):
    return []
  size = data[index]
  begin = index
  begin += 1
  end = index
  end +=1
  end += size
  if end <= len(data):
    index = end
    #print ("getting blob of data from: ", begin, " to ", end )
    return data[begin:end]
  else:
    return []
  

def deserialize_string(lwords):
  """Wrapper for the C++ StringSerializer"""
  
  v = cppyy.makeClass('std::vector<unsigned int>')()
  s = cppyy.makeClass('std::string')()
  v.reserve(len(lwords))
  for w in lwords: v.push_back(w)
  stringSerializer.deserialize(v, s)
  return str(s)

  
#############################################################################
# printing utils
#

def print_ranges(l):
  for i in range(len(l)):
    print ("%-16d%16d" % ( (i+1)*32, i*32))

def print_chain(counter, s):
  ch = cppyy.makeClass('HLT::Chain')(s)
  ch.deserialize(s)
  print (".... chain %-3d Counter:%-4d Passed: %d (Raw:%d Prescaled: %d PassThrough:%d) Rerun: %d LastStep: %d Err: %s"\
        % ( counter, ch.getChainCounter(), ch.chainPassed(), ch.chainPassedRaw(), ch.isPrescaled(), ch.isPassedThrough(),\
            ch.isResurrected(), ch.getChainStep(), ch.getErrorCode().str()))

def print_all_chains(blob):
  print ("... chains:")
  for i in range(len(blob)):
    print_chain(i, blob[i])


def print_all_navigation(result):
  print ("... features")
  for f in result.nav_payload:
    print (".... %-52s %6d B " % (f[0]+'#'+f[1], f[2]))
  return


def print_HLTResult(result):    
#  if opt.sizes:
  print ("... Payload size: ", result.as_int_v.size(), " ", (4.0*result.as_int_v.size())/(1024), "kB")

  if result.as_int_v.size() == 0:
    print ("... Payload size is 0")

  # header info

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
  print ('... Version:', version ,' Lvl1Id:',l1id ,' Decision:',acc ,\
        ' PassThrough:',pt,' Status:',status.str(),\
        ' ConverterStatus:', cnvstatus.str(),' LVL:',level,' Signatures:',nosigs,' Bad:',bad,' Truncated:', trunc,\
        ' App:', result.appName())
  print()

  chains_data = list(result.getChainResult())
  nchains = chains_data[0] if chains_data else 0
  nav_data = list(result.getNavigationResult())
  nnav = len(nav_data)
  nver = nav_data[0] if nav_data else 0
  
#  if opt.sizes:
  print ('... tot:', result.as_int_v.size(), ' chains:', nchains, ' chains (expected):', len(chains_data)-1, \
          ' navigation:', nnav, ' navigation (expected):',result.getNavigationResult()[1] if nnav > 1 else "0 or 1")
    
#  if opt.conf:
  try:
    print ("... SMkey: ", result.getConfigSuperMasterKey(), " Prescalers key ", result.getConfigPrescalesKey())
  except:
    print ("... No config info ")

#  if opt.chains:
  print_all_chains(chains_data[1:])
    
    
  print ("... Navigation version: ", nver)
 # if opt.tes:
  if nnav > 3:
    tessize  = result.getNavigationResult()[2]
    tescount = result.getNavigationResult()[3]
    print ("... Number of TEs: ",  tescount, " and size: ", tessize, " ", 4.0*tessize/(1024), "kB")
  else:
    print ("... Cannot print TriggerElement details (not enough navigation data)")

#  if opt.features:
    print_all_navigation(result)

    
##############################################################################
# collect the sizes
def collect_feature_sizes(dest, result):
  for f in result.nav_payload:
    key = f[0]+'#'+f[1]
    if key not in dest:
      dest[key] = 0
    dest[key] += f[2]

    # keyed by type
    key = f[0]
    if key not in dest:
      dest[key] = 0
    dest[key] += f[2]

  if 'Total' not in dest:
    dest['Total'] = 0
  dest['Total'] += 4*result.as_int_v.size()
