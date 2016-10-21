# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
athenaMT event modifier to test HLT prescale update.

Usage: athenaMT -Z TrigP1Test.PrescaleChange ...

Configuraton of this module via:
  $DBCONN           - DB connection (default: TRIGGERDBATN)
  ./prescales.txt   - Text file with prescale keys (one per file) to cycle through
"""

import os
import itertools
import logging
import eformat
import atexit

from TrigByteStreamTools import trigbs_replaceLB
from TrigByteStreamTools import CTPfragment

class Config:
  """Configuration options for this module"""
  hltCounterChange = {3:3,    # first number: LB on which to change CTP bits, second number: new value for CTP bits
                      6:6,
                      8:8}
  
class Store:
  """Global variables"""
  lastCounterChange = 0

# This is also used in share/testPrescaleChange_postconfig.py
partitionName = '%s_%s_%s' % (os.getenv('AtlasProject','AtlasHLT'),
                              os.path.basename(os.getenv('AtlasBaseDir','')),
                              os.getenv('AtlasVersion','rel_0'))

log = logging.getLogger(__name__)
log.info('Will perform HLT prescale udpate on these LBs: %s' % sorted(Config.hltCounterChange.values()))

dbconn = os.getenv('DBCONN','TRIGGERDBATN')
try:
    psk = [int(s) for s in open('prescales.txt','r').readlines()]
except Exception as e:
    log.error('Cannot read prescale keys: %s' % e)
    psk = None
    
log.info('List of prescale keys in prescales.txt: %s' % psk)
if psk!=None: psk = itertools.cycle(psk)

#
# Use TestTriggerDBWriter (from tdaq release) to fill the PSk table in the DB
# Simulate the user input via stdin redirection
#

@atexit.register
def clearPSK():
    """Clear the PSK table in the trigger DB"""
    
    open('input.txt','w').write('4\n')
    log.info('Clearing HLT PSK table for partition %s' % partitionName)
    os.system("TDAQ_PARTITION=%s TestTriggerDBWriter %s < input.txt > TestTriggerDBWriter_clear.log" % (partitionName,dbconn))
    
def writePSK(lb, psk):
    """Write one new LB/PSK set to the trigger DB"""
    
    open('input.txt','w').write('2\n%d %d\n1\n' % (lb,psk))
    logfile = 'TestTriggerDBWriter_%d_%d.log' % (lb,psk)    
    log.info('Changing HLT PSK to %d on LB %d (see %s)' % (psk,lb,logfile))
    os.system("TDAQ_PARTITION=%s TestTriggerDBWriter %s < input.txt > %s" % (partitionName,dbconn,logfile))

def modify(event):

    event = eformat.write.FullEventFragment(event)
    
    # Modify LB and HLT counter in CTP fragment
    newevt = trigbs_replaceLB.modify(event)
    lb = newevt.lumi_block()

    # Set new prescale key
    if lb in Config.hltCounterChange and psk!=None:
        Store.lastCounterChange = Config.hltCounterChange.pop(lb)
        writePSK(Store.lastCounterChange, psk.next())


    # Find CTP ROBs
    ctp_robs = [rob for rob in newevt.children() if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_CTP]

    for ctp_rob in ctp_robs:
        CTPfragment.setHltCounter(ctp_rob, Store.lastCounterChange)
    
    return newevt.readonly()

# Done once on importing this module
clearPSK()

