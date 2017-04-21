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
parser.add_argument('--ratetag',type=str,default="TestTagRateM13",help="COOL tag for rates folder")
parser.add_argument('--chaintag',type=str,default="TestTagChainM13",help="COOL tag for chains folder")
parser.add_argument('--runNumber',type=int,default=319018,help="RunNumber")
parser.add_argument('--timestamp',type=int,default=1490877280,help="Unix timestap")
args=parser.parse_args()

log.info("Aquiring rates for run %s",args.runNumber)

hltrates = TrigHltRates.TrigHltRates(ratetag=args.ratetag,chaintag=args.chaintag)

return_code,chains,rates = hltrates.getAllRatesAtT(runno=args.runNumber,timestamp=args.timestamp)
if return_code < 0:
    log.warning("getAllRatesAtT error %s",return_code)
else:
    log.info("getAllRatesAtT for %s chains",return_code)
return_code,rates = hltrates.getRatesAtT(runno=319018,timestamp=1490877280,selected_chains=["total","grp_Calibration","recording"])
if return_code < 0:
    log.warning("getAllRatesAtT error %s",return_code)
else:
    log.info("getRatesAtT for selected %s chains",return_code)
return_code,chains,rates,iovs = hltrates.getAllRates(runno=319018)
return_code,rates,iovs = hltrates.getRates(runno=319018,selected_chains=["total","grp_Calibration","recording"])
return_code,rates,iovs = hltrates.getRates(runno=319018,selected_chains=["total","grp_Calibration","wrong_chain_name","recording"])
