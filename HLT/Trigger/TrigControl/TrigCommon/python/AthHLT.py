# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Utilities used in athenaHLT.py
#
from AthenaCommon.Logging import logging
log = logging.getLogger('athenaHLT')

from functools import cache

class CondDB:
   _run2 = 236108
   def __init__(self, run):
      self.run = run
   def db_instance(self):
      if self.run>=self._run2:
         return 'CONDBR2'
      else:
         return 'COMP200'
   def sor_folder(self):
      if self.run>=self._run2:
         return '/TDAQ/RunCtrl/SOR'
      else:
         return '/TDAQ/RunCtrl/SOR_Params'

@cache
def get_sor_params(run_number):
   import pickle
   cool_cache = 'AthHLT.sor.pkl'

   try:
      # Try to load the SOR record from the file cache
      d = pickle.load(open(cool_cache, 'rb'))
      if d['RunNumber'] != run_number:
         raise Exception('Cache does not contain current run')
      log.info('Reading cached SOR record for run %s from %s', run_number, cool_cache)
      return d
   except Exception as e:
      d = {}
      log.verbose('Could not read SOR record from cache: %s' % e)
      log.info('Reading SOR record for run %s from COOL', run_number)

   from CoolConvUtilities import AtlCoolLib
   cdb = CondDB(run_number)
   dbcon = AtlCoolLib.readOpen('COOLONL_TDAQ/%s' % cdb.db_instance())
   folder = dbcon.getFolder(cdb.sor_folder())

   # need to keep sor variable while using payload (cannot do the following in
   # one single line nor overwrite sor). Otherwise: 1) GC comes into play;
   # 2) the object is deleted; 3) since it's a shared_ptr, the internal
   # cool::IObject also gets deleted; 4) payload is not valid any longer
   try:
      sor = folder.findObject(run_number << 32, 0)
   except Exception:
      return None        # This can happen for unknown run numbers

   payload = sor.payload()
   d = {k: payload[k] for k in payload}
   try:
      pickle.dump(d, open(cool_cache, 'wb'))
   except Exception:
      log.info('Could not store SOR record in cache %s', cool_cache)

   return d


@cache
def get_trigconf_keys(run_number):
   """Read HLT keys from COOL"""

   from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil

   cdb = CondDB(run_number)
   db = TriggerCoolUtil.GetConnection(cdb.db_instance())
   run_range = [[run_number,run_number]]
   d = {}
   d['SMK'] = TriggerCoolUtil.getHLTConfigKeys(db, run_range)[run_number]['SMK']
   # First HLT/L1 prescale key used in the run
   d['HLTPSK'] = TriggerCoolUtil.getHLTPrescaleKeys(db, run_range)[run_number]['HLTPSK2'][0][0]
   d['LVL1PSK'] = TriggerCoolUtil.getL1ConfigKeys(db, run_range)[run_number]['LVL1PSK'][0][0]

   return d

#
# Testing (used as ctest)
#
if __name__=='__main__':
   # Can be used as script, e.g.: python -m TrigCommon.AthHLT 327265
   import sys
   if len(sys.argv)>1:
      log.info('SOR parameters: %s', get_sor_params(int(sys.argv[1])))
      sys.exit(0)

   # Unit testing case:
   d = get_sor_params(327265)  # Run-2
   print(d)
   assert(d['DetectorMask']=='0000000000000000c10069fffffffff7')

   d = get_sor_params(216416)  # Run-1
   print(d)
   assert(d['DetectorMask']==281474976710647)

   d = get_trigconf_keys(360026)
   print(d)
   assert(d['SMK']==2749)
   assert(d['LVL1PSK']==15186)
   assert(d['HLTPSK']==17719)
