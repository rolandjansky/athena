# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Minimal python module for CTP fragment access/modification

For C++ implementation see:
https://gitlab.cern.ch/atlas-tdaq-software/CTPfragment
"""

import sys
import eformat
import cppyy

from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()

cppyy.load_library('libTrigByteStreamToolsDict')
cppyy.load_library('libCTPfragment')
from ROOT import CTPdataformat
from ROOT import CTPfragment as _CTPfragment
from ROOT.CTPfragment import getFolderUpdates  # noqa: F401  (import into our namespace)

# Import classes from C++ namespace
FolderEntry = _CTPfragment.FolderEntry
ExtraPayload = _CTPfragment.ExtraPayload

def _versioned(obj, name, version):
   """Helper to return versioned members of CTPdataformat"""
   attr = ''
   v = version
   while v>=0:   # decrement version until found
      attr = '%s_v%d' % (name, v)
      if hasattr(obj,attr):
         break
      v -= 1
   return getattr(obj,attr) 

def _setBits32(bitset, value, shift, mask):
   """Set value in bitset using mask and shifting n bits to the left"""
   v = bitset & (0xffffffff ^ (mask << shift))
   v = v | (value << shift)
   return v

def decodeTriggerBits(info):
   """Return list of bits [0,1,1,0,...] from list of 32-bit trigger words"""
   if type(info)==int:
      info=[info]
   bits=[]
   cnt=0
   for word in info:
      for i in range(32):
         if word&(1<<i):
            bits+=[cnt]
         cnt+=1
   return bits

def encodeTriggerBits(bits,len=1):
   """Return list of trigger words from list of bits"""
   words = [0] * len
   for bit in bits:
      words[bit//32] |= 1<<(bit%32)
   return words

def ctpFormatVersion(rob):
   """Get CTP fragment format version"""
   v = (rob.rod_minor_version() >> CTPdataformat.CTPFormatVersionShift) & CTPdataformat.CTPFormatVersionMask
   return v

def setCtpFormatVersion(rob, version):
   """Set CTP fragment format version"""
   V = _setBits32(rob.rod_minor_version(), version,
                  CTPdataformat.CTPFormatVersionShift, CTPdataformat.CTPFormatVersionMask)
   rob.rod_minor_version(V)

def hltCounter(rob):
   """Get HLT counter"""
   v = ctpFormatVersion(rob)
   return (rob.rod_detev_type() >> _versioned(CTPdataformat,'HltCounterShift',v) & _versioned(CTPdataformat,'HltCounterMask',v))

def setHltCounter(rob, hltCounter):
   """Set HLT counter"""
   v = ctpFormatVersion(rob)
   c = _setBits32(rob.rod_detev_type(), hltCounter, _versioned(CTPdataformat,'HltCounterShift',v), _versioned(CTPdataformat,'HltCounterMask',v))
   rob.rod_detev_type(c)

def lumiBlock(rob):
   return (rob.rod_detev_type() >> CTPdataformat.LumiBlockShift & CTPdataformat.LumiBlockMask)

def setLumiBlock(rob, lb):
   lbits = _setBits32(rob.rod_detev_type(), lb, CTPdataformat.LumiBlockShift, CTPdataformat.LumiBlockMask)
   rob.rod_detev_type(lbits)

def numberExtraPayloadWords(rob):
   """Number extra payload words (this includes the time since last L1A)"""
   v = ctpFormatVersion(rob)
   if v < 2:
      return 0
   else:
      return ((rob.rod_minor_version() >> _versioned(CTPdataformat,'ProgrammableExtraWordsShift',v)
               & _versioned(CTPdataformat,'ProgrammableExtraWordsMask',v)))
   
def numberHltExtraPayloadWords(rob):
   """Number extra payload words (this does NOT include the time since last L1A)"""
   return max(numberExtraPayloadWords(rob)-1, 0)

def hltExtraPayloadWords(rob):
   n = numberHltExtraPayloadWords(rob)
   offset = 0
   if ctpFormatVersion(rob)>=3:
      offset = 1  # for turn counter
   return rob.rod_data()[-numberHltExtraPayloadWords(rob)+offset:] if n>0 else []

def getExtraPayloadObject(rob):
   """Return CTPfragment::ExtraPayload object created from CTP ROB"""
   v = cppyy.gbl.std.vector('unsigned int')()
   for p in hltExtraPayloadWords(rob):
      v.push_back(p)

   x = _CTPfragment.ExtraPayload(v)
   return x
   
def setHltExtraPayloadWords(rob, extraWords):
   
   # Make writable ROB
   wrob = eformat.write.ROBFragment(rob)
   # Copy ROD data, except extra words
   data = [d for d in wrob.rod_data()]
   n = numberHltExtraPayloadWords(rob)   
   if n>0:
      data = data[:-n]
   # Append new extra words and set in ROB
   v = ctpFormatVersion(wrob)
   if v<2:
      setCtpFormatVersion(wrob, 2)   # need at least V2
      data.extend([0])   # time since last L1A
   data.extend(extraWords)
   wrob.rod_data(data)
   
   # Set new payload length (including words reserved by CTP)
   ctp_extras = 1           # time since last L1A
   if v>=3:
      ctp_extras = 2  # turn counter
   V = _setBits32(wrob.rod_minor_version(), len(extraWords)+ctp_extras,
                  _versioned(CTPdataformat,'ProgrammableExtraWordsShift',v),
                  _versioned(CTPdataformat,'ProgrammableExtraWordsMask',v))
   wrob.rod_minor_version(V)

   return wrob.readonly()

def lvl1AcceptBunch(rob):
   """Get position of LVL1-Accept Bunch from ROD Fragment"""

   v = ctpFormatVersion(rob)
   shift = _versioned(CTPdataformat,'L1APositionShift',v)
   if v==0:
      return (rob.rod_detev_type() >> shift) & CTPdataformat.L1APositionMask
   else:
      return (rob.rod_minor_version() >> shift) & CTPdataformat.L1APositionMask

def getTriggerWords(rob,name='TAV'):
   """Get trigger words"""

   name = name.upper()
   v = ctpFormatVersion(rob)
   pos = _versioned(CTPdataformat,name+'pos',min(v,4))
   words = _versioned(CTPdataformat,name+'words',min(v,4))
   # No TAP in RoI ROB
   if v>4 and rob.source_id().module_id()==1:
      pos = _versioned(CTPdataformat,name+'pos',5)
      if name=='TAP':
         return []

   l1abunch = 0 if rob.source_id().module_id()==1 else lvl1AcceptBunch(rob)
   
   data = [d for d in rob.rod_data()]
   pos = l1abunch*_versioned(CTPdataformat,'DAQwordsPerBunch',v) + pos
   return data[pos:pos+words]
   
#
# For testing
#

def main():
   from optparse import OptionParser
   import eformat
   
   parser = OptionParser(usage='%prog FILE')
   parser.add_option('-m', '--moduleid', type='int', action='store', default=0,
                     help='Module ID of CTP fragment [%default]')
   
   (opt, args) = parser.parse_args()  
   if len(args)!=1:
      parser.print_help()
      return 1

   for event in eformat.istream(args[0]):
      ctp_robs = [rob for rob in event.children()
                  if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_CTP
                  and rob.source_id().module_id() == opt.moduleid]

      if len(ctp_robs)==0:
         print("Cannot find CTP ROB with module ID %d" % opt.moduleid)
         continue

      rob = ctp_robs[0]
      fe = _CTPfragment.FolderEntry()
      fe.folderIndex = 1
      fe.lumiBlock = 54

      fe2 = _CTPfragment.FolderEntry()
      fe2.folderIndex = 2
      fe2.lumiBlock = 59

      #x = getExtraPayloadObject(rob) 
      x = _CTPfragment.ExtraPayload()
      x.setL1PSK(255)
      x.updateFolder(fe)
      x.updateFolder(fe2)
      new_ctp_rob = setHltExtraPayloadWords(rob, [d for d in x.serialize()])
      new_event = eformat.write.FullEventFragment()
      new_event.copy_header(event)
      for r in event.children():
         if r.source_id().subdetector_id() != eformat.helper.SubDetector.TDAQ_CTP:
            new_event.append(eformat.write.ROBFragment(r))

      new_event.append(eformat.write.ROBFragment(new_ctp_rob))
   
      event = new_event.readonly()
      #new_ctp_rob = eformat.write.ROBFragment(new_ctp_rob)
      #setHltCounter(new_ctp_rob,100)
      rob = new_ctp_rob
      
      x = getExtraPayloadObject(rob)
      folderUpdates = _CTPfragment.getFolderUpdates(x)
      upd = ''
      for f in folderUpdates:
         upd += ('[%d,%d]' % (f.second.folderIndex,f.second.lumiBlock))

      print("L1ID %10d, LB %4d, Version %d, Bunch %d, HLT counter: %3d, Payload #%d %s L1PSK %d BGK %d COOLUPD %s" % (
         event.lvl1_id(),
         event.lumi_block(),
         ctpFormatVersion(rob),
         lvl1AcceptBunch(rob),
         hltCounter(rob),
         numberHltExtraPayloadWords(rob),
         hltExtraPayloadWords(rob),
         x.getL1PSK(),
         x.getBGK(),
         upd
      ))

if __name__ == "__main__":
  sys.exit(main())

