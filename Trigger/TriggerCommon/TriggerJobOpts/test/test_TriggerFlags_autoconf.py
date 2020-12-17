#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Test of Trigger config flags autoconfiguration
# Exercised on BS data and MC POOL file formats

from AthenaCommon.Logging import logging
log=logging.getLogger('test_TriggerFlags_autoconf')

import os
def get_file_from_art(package, test):
    artdir = "/eos/atlas/atlascerngroupdisk/data-art/build-output/master/Athena/x86_64-centos7-gcc8-opt/"
    if not os.path.isdir(artdir):
        log.warning('Cannot access %s', artdir)
        return None
    nightly = os.listdir(artdir)[-2] # don't take the latest; this may still be running
    nightlydir = artdir + nightly + "/"
    return nightlydir + package + "/" + test + "/AOD.pool.root"

from TrigValTools.TrigValSteering import Input
inputfiles = {
    "Run1_Data": Input.get_input('data_run1').paths[0],
    "Run2_Data": Input.get_input('data').paths[0],
    "Run3_Data": Input.get_input('data_run3').paths[0],
    #
    "Run2_MC_r22": get_file_from_art("TrigAnalysisTest", "test_trigAna_RDOtoAOD_v7Primaries_build"),
    "Run2_MC_r21": '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/v3/myAOD.pool.root',
    #
    "Run3_MC": get_file_from_art("TrigAnalysisTest", "test_trigAna_RDOtoAOD_v1Dev_build")
}

from TriggerJobOpts.TriggerConfigFlags import createTriggerFlags

from AthenaCommon.Constants import DEBUG
from TriggerJobOpts.TriggerConfigFlags import log as tcf_log
tcf_log.setLevel(DEBUG)

def test_TriggerFlags(sample):

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    input_file = inputfiles[sample]
    if not input_file or not os.path.isfile(input_file):
        log.warning('Skipping %s because cannot access the input file', sample)
        return

    acf = ConfigFlags.clone()
    acf.Input.Files = [inputfiles[sample]]
    acf.addFlagsCategory("Trigger", createTriggerFlags)
    
    # Test EDMVersion
    EDMDecode_ref = {
        "Run1": 1,
        "Run2": 2,
        "Run3": 3
    }[sample[:4]]
    log.info("EDMVersion: expected {}, configured {}".format(EDMDecode_ref, acf.Trigger.EDMVersion))
    assert( acf.Trigger.EDMVersion == EDMDecode_ref )

    return
    
if __name__=="__main__":
    log.info("Testing TriggerConfigFlags autoconfiguration")
    import sys
    for sample in inputfiles:
        run, source = sample.split('_',1)
        log.info("{} {} input file: {}".format(run, source, inputfiles[sample]))
        try:
            test_TriggerFlags(sample)
        except AssertionError:
            sys.exit(1)
            
    log.info("Done with test")
    sys.exit(0)
