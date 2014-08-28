# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Minimal python module for CTP fragment access/modification

For C++ implementation see:
https://svnweb.cern.ch/trac/atlastdaq/browser/DAQ/DataFlow/CTPfragment/trunk/CTPfragment/
"""

import sys
import eformat
import PyCintex

PyCintex.loadDictionary('TrigByteStreamToolsDict')
CTPdataformat = PyCintex.makeNamespace('CTPdataformat')()
CTP = PyCintex.makeNamespace('CTP')
PyCintex.makeNamespace('CTPdataformat::Helper')


def _versioned(obj, name, version):
   """Helper to return versioned members of CTPdataformat"""
   attr = ''
   v = version
   while v>0:   # decrement version until found
      attr = '%s_v%d' % (name, v)
      if hasattr(obj,attr): break
      v -= 1
   return getattr(obj,attr) 

def _setBits32(bitset, value, shift, mask):
   """Set value in bitset using mask and shifting n bits to the left"""
   v = bitset & (0xffffffff ^ (mask << shift))
   v = v | (value << shift)
   return v

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
   l = _setBits32(rob.rod_detev_type(), lb, CTPdataformat.LumiBlockShift, CTPdataformat.LumiBlockMask)
   rob.rod_detev_type(l)

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
   if ctpFormatVersion(rob)>=3: offset = 1  # for turn counter
   return rob.rod_data()[-numberHltExtraPayloadWords(rob)+offset:] if n>0 else []

def getExtraPayloadObject(rob):
   """Return CTP::ExtraPayload object created from CTP ROB"""
   v = PyCintex.gbl.std.vector('unsigned int')()
   for p in hltExtraPayloadWords(rob):
      v.push_back(p)

   x = PyCintex.makeClass('CTP::ExtraPayload')(v)
   return x
   
def setHltExtraPayloadWords(rob, extraWords):
   
   # Make writable ROB
   wrob = eformat.write.ROBFragment(rob)
   # Copy ROD data, except extra words
   data = [d for d in wrob.rod_data()]
   n = numberHltExtraPayloadWords(rob)   
   if n>0: data = data[:-n]
   # Append new extra words and set in ROB
   v = ctpFormatVersion(wrob)
   if v<2:
      setCtpFormatVersion(wrob, 2)   # need at least V2
      data.extend([0])   # time since last L1A
   data.extend(extraWords)
   wrob.rod_data(data)
   
   # Set new payload length (including words reserved by CTP)
   ctp_extras = 1           # time since last L1A
   if v>=3: ctp_extras = 2  # turn counter
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
      ctp_robs = [rob for rob in event.children() \
                     if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_CTP \
                     and rob.source_id().module_id() == opt.moduleid]

      if len(ctp_robs)==0:
         print "Cannot find CTP ROB with module ID %d" % opt.moduleid
         continue

      rob = ctp_robs[0]
      fe = PyCintex.makeClass('CTP::FolderEntry')()
      fe.folderIndex = 1
      fe.lumiBlock = 54

      fe2 = PyCintex.makeClass('CTP::FolderEntry')()
      fe2.folderIndex = 2
      fe2.lumiBlock = 59

      x = getExtraPayloadObject(rob)
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
      folderUpdates = CTP.getFolderUpdates(x)
      upd = ''
      for f in folderUpdates:
         upd += ('[%d,%d]' % (f.second.folderIndex,f.second.lumiBlock))

      print "L1ID %10d, LB %4d, Version %d, Bunch %d, HLT counter: %3d, Payload #%d %s PSK %d COOLUPD %s" % (
         event.lvl1_id(),
         event.lumi_block(),
         ctpFormatVersion(rob),
         lvl1AcceptBunch(rob),
         hltCounter(rob),
         numberHltExtraPayloadWords(rob),
         hltExtraPayloadWords(rob),
         x.getL1PSK(),
         upd
         )

if __name__ == "__main__":
  sys.exit(main())

