#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import argparse
import logging
from TrigHLTRatesFromCool import TrigHltRates
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("read_hlt_rates.py")
log.setLevel( logging.INFO )
log.info('Read Hlt Rates from COOL')

__doc__="""Retrieve HLT rates from COOL"""
parser = argparse.ArgumentParser(description = __doc__)
parser.add_argument('--ratetag', type=str, default="TestTagRateM13", help="COOL tag for rates folder")
parser.add_argument('--chaintag', type=str, default="TestTagChainM13", help="COOL tag for chains folder")
parser.add_argument('--runNumber', type=int, default=319018, help="Run number")
parser.add_argument('--timestamp', type=int, default=1490877280, help="Unix timestap")
args=parser.parse_args()

log.info("Aquiring rates for run %s", args.runNumber)

hltrates = TrigHltRates.TrigHltRates(ratetag=args.ratetag, chaintag=args.chaintag)

# Get rates of all chains for all iovs
return_code, chainratedict = hltrates.getAllRates(runno=args.runNumber)
if return_code < 0:
    log.warning("getAllRates error %s", return_code)
else:
    log.info("getAllRates for %s chains", return_code)

# Get rates chains that has "MU20" in it's name for all iovs
return_code, chainratedict = hltrates.getAllRates(runno=args.runNumber, chains=[".*MU20.*"])
if return_code < 0:
    log.warning("getAllRates error %s", return_code)
else:
    log.info("getAllRates for %s chains", return_code)

# Get rates of all chains at 4th IOV
return_code, chainratedict = hltrates.getRates(runno=args.runNumber, iov=4)
if return_code < 0:
    log.warning("getAllRates error %s", return_code)
else:
    log.info("getAllRates for %s chains", return_code)

# Get Rate for "grp_Cosm.*" and "grp_Cal.*" chains at a certain timestamp
iov = hltrates.getIOV(runno=args.runNumber, timestamp=args.timestamp) # Get iov corresponding to timestamp
return_code, chainratedict = hltrates.getRates(runno=args.runNumber, chains=["grp_Cosm.*","grp_Cal.*"], iov=iov)
if return_code < 0:
    log.warning("getAllRates error %s", return_code)
else:
    log.info("getAllRates for %s chains", return_code)

# Get Rate for "HLT.*" chains 5 minutes after ready4physics
timestamp = hltrates.getTimestamp(runno=args.runNumber, iov=0)
iov = hltrates.getIOV(runno=args.runNumber, timestamp=args.timestamp+5*60)
return_code, chainratedict = hltrates.getRates(runno=args.runNumber, chains=["HLT.*"], iov=iov)
if return_code < 0:
    log.warning("getAllRates error %s", return_code)
else:
    log.info("getAllRates for %s chains", return_code)
