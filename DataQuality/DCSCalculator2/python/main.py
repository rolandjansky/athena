# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import with_statement

from multiprocessing.pool import Pool

import logging; log = logging.getLogger("DCSCalculator2.main")

from DQUtils.db import fetch_iovs
from DQUtils.iov_arrangement import inverse_lblb, run_iovs_from_lblb
from DQUtils.general import timer, get_package_version
from DQUtils.utils import pprint_objects
from DQUtils.logger import init_logger
from DQUtils.sugar import RunLumi, IOVSet

from DQDefects import DefectsDB

from DCSCalculator2.subdetectors import ALL_SYSTEMS, SYSTEM_MAP
from DCSCalculator2.variable import DefectIOV

import DCSCalculator2.config as config

def maketime(n):
    if n < 1000000:
        # Probably an LB.
        return n
    seconds = n / 1e9
    hours   = seconds / 3600; seconds %= 3600
    minutes = seconds / 60;   seconds %= 60
    return "%ih%im%is" % (hours, minutes, seconds)

def print_system_iovs(result):
    if not result:
        log.info("Empty Result.")
        return
    #print result
    ## for channel, result in sorted(result.by_channel.iteritems()):
    ##     def rlbs(Code):
    ##         return maketime(sum(i.length for i in result if i.Code == Code))
            
    ##     args = channel, hash(result), rlbs(3), rlbs(2), rlbs(1), rlbs(0)
    ##     print "    %i Hash: % 09x LBs: G(%4s) Y(%4s)  R(%4s) Gray(%4s)" % args

def run_one(system, lbtime, run_iovs):
    try:
        with timer("Run DCS calculator 2 for %s" % system):
            return system.run(lbtime, run_iovs)
    except Exception:
        log.warning("DCS Calculator failed to run for %s.", system)
        if config.opts.dont_ignore_system_exceptions:
            raise
        log.exception("Continuing. Use -e -X commandline to investigate")
        if config.opts.email_on_failure:
            from DataQualityUtils.panic import panic
            from traceback import format_exc
            runnum = lbtime[0].Run if len(lbtime)>0 else '??????'
            panicmsg = "DCS Calculator failed to execute in run %s for system %s.\n\n%s" % (runnum, system, format_exc())
            panic(panicmsg)
        return None
    except (KeyboardInterrupt, SystemExit):
        raise

def run_parallel(systems, lbtime, run_iovs, N):
    pool = Pool(N if N > 0 else len(systems))
    results = []
    for system in systems:
        result = pool.apply_async(run_one, (system, lbtime, run_iovs))
        results.append(result)
        
    all_results = IOVSet()
    for system, result in zip(systems, map(lambda x: x.get(), results)):
        log.info(" -- result for %s", system)
        print_system_iovs(result)
        if result:
            all_results.extend(result)
        
    return all_results
    
def run_sequential(systems, lbtime, run_iovs):
    result_iovs = IOVSet()
    
    for system in systems:
        log.info(" -- running for %s", system)
        system_result = run_one(system, lbtime, run_iovs)
        print_system_iovs(system_result)
        if system_result:
            result_iovs.extend(system_result)
            
    return result_iovs

def go(iov, systems, db, indb, timewise=False):
    """
    Run the DCS calculator for `run` on the `systems` specified, saving the 
    result to the `database`.
    """
    since, until = iov

    with timer("Read LBLB"):
        # fetch lumi block info
        lblb = fetch_iovs("LBLB", since, until, with_channel=False, database=(indb if indb.startswith('sqlite') else 'COOLONL_TRIGGER/%s' % indb))
        assert lblb, "No luminosity blocks for desired range. [%s, %s)" % (since, until)
        
        # lumi block time info
        lbtime = inverse_lblb(lblb)

        # run_iovs is....?
        if timewise:
            run_iovs = run_iovs_from_lblb(lbtime)
        else:
            run_iovs = run_iovs_from_lblb(lblb)
    
    log.debug("Will process %i LBs over %i runs", len(lblb), len(run_iovs))
    
    log.info("DCSC2 will run over range: %r %s", tuple(run_iovs.range_iov), "(%i lbs)" % len(lblb))
    
    # Initialize the system objects
    # This changes 'systems' from a list of types to a list of subdetector objects
    systems = [system() for system in systems 
               if not getattr(system, "__DISABLED__", False)]
    
    # Run systems sequentially for this range
    result_iovs = run_sequential(systems, lbtime, run_iovs)
    
    # Run parallel is broken because of the use of closures to implement the 
    # defect translation.
    #result_iovs = run_parallel(systems, lbtime, run_iovs, parallel)
    
    if log.isEnabledFor(logging.DEBUG):
        pprint_objects(result_iovs)
    
    log.info("Calculation complete")
    
    if db != "None":
        with timer("write result (%i iovs)" % len(result_iovs)):
            log.debug("Writing result (%i iovs)", len(result_iovs))
            defect_iovs = list(filter(lambda iov: isinstance(iov, DefectIOV), result_iovs))
            if len(defect_iovs) > 0:
                ddb = DefectsDB(db, read_only=False, create=True)
                defect_names = set(i.channel for i in defect_iovs)
                for defect in defect_names:
                    if defect in ddb.defect_id_map:
                        continue
                    ddb.create_defect(defect, "Created by DCSCalculator2")
                with ddb.storage_buffer:
                    for iov in defect_iovs:
                        ddb.insert(iov.channel, iov.since, iov.until,
                                   iov.comment,
                                   'sys:defectcalculator',
                                   iov.present)
        
    args = len(result_iovs), hash(result_iovs)
    log.info("Success. Calculated %i iovs. Result hash: 0x%0x8.", *args)

def main(argv):
    
    optp, opts, args = config.parse_options(argv)

    init_logger(opts.verbose)
    
    log.info("Using %s", get_package_version("DQUtils"))
    log.info("Using %s", get_package_version("DQDefects"))
    log.info("Using %s", get_package_version("DCSCalculator2"))
    
    log.debug("Commandline arguments: %s", argv)
    log.debug("Current configuration: %s", (opts))
    
    if opts.shell_on_exception: 
        import sys
        from IPython.core import ultratb
        sys.excepthook = ultratb.FormattedTB(call_pdb=True)
        #from IPython.Shell import IPShellEmbed
        #ipython_instance = IPShellEmbed(["-pdb"], rc_override=dict(quiet=True))
    
    if opts.systems is None:
        systems = ALL_SYSTEMS
        
    else:
        systems = []
        invalid_systems = []
        for system in opts.systems:
            if system not in SYSTEM_MAP:
                invalid_systems.append(system)
            else:
                systems.append(SYSTEM_MAP[system])
                
        if invalid_systems:
            optp.error("Invalid system(s) specified: {0}. "
                       "Available systems: {1}".format(invalid_systems, SYSTEM_MAP.keys()))
    

    if (opts.run and opts.range) or (not opts.run and not opts.range):
        optp.error("Specify either -r or -R")
    elif opts.run:
        since, until = opts.run, opts.run
    elif opts.range:
        since, until = map(int, opts.range.split("-"))
    
    if opts.lbs:
        if opts.range:
            optp.error("--lbs and --range are incompatible. "
                       "Use --lbs with --run")
        sincelb, untillb = map(int, opts.lbs.split("-"))
        iov = RunLumi(since, sincelb), RunLumi(until, untillb)
    else:
        iov = RunLumi(since, 0), RunLumi(until+1, 0)
    
    go(iov, systems, opts.output_database, opts.input_database, opts.timewise)

