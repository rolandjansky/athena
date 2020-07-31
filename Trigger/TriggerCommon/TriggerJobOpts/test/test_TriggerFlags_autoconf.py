#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Test of Trigger config flags autoconfiguration
# Exercised on BS data and MC POOL file formats

from AthenaCommon.Logging import logging
log=logging.getLogger('test_TriggerFlags_autoconf')

import os, glob
artdir = "/eos/atlas/atlascerngroupdisk/data-art/build-output/master/Athena/x86_64-centos7-gcc8-opt/"
nightly = os.listdir(artdir)[-2] # don't take the latest; this may still be running
nightlydir = artdir+nightly+"/"

def getRun2MCFile():
    filen = "myAOD.pool.root"
    artdir_21 = "/eos/atlas/atlascerngroupdisk/data-art/grid-output/21.0/Athena/x86_64-slc6-gcc62-opt/"
    nightly_21 = os.listdir(artdir_21)[-2] # don't take the latest; this may still be running
    if not os.path.isfile(filen):
        tarball = glob.glob(artdir_21+nightly_21+"/Tier0ChainTests/test_q220/user.artprod.*.EXT1.*.tar")[0]
        os.system("tar -xf {} {}".format(tarball,filen))
    return filen

inputfiles = {
    "Run1_Data": "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1",
    "Run2_Data": "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1",
    "Run3_Data": nightlydir+"/TrigP1Test/test_trigP1_v1PhysP1_T0Mon_build/data18_13TeV.00360026.physics_Main.unknown.RAW._lb0151._athenaHLT._0001.data",
    #
    "Run2_MC_r22": nightlydir+"TrigAnalysisTest/test_trigAna_RDOtoAOD_build/AOD.pool.root",
    "Run2_MC_r21": getRun2MCFile(),
    #
    "Run3_MC": nightlydir+"TrigAnalysisTest/test_trigAna_RDOtoAOD_mt1_build/AOD.pool.root",
}

from TriggerJobOpts.TriggerConfigFlags import createTriggerFlags

from AthenaCommon.Constants import DEBUG
from TriggerJobOpts.TriggerConfigFlags import log as tcf_log
tcf_log.setLevel(DEBUG)

def test_TriggerFlags(sample):

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    acf = ConfigFlags.clone()
    acf.Input.Files = [inputfiles[sample]]
    acf.addFlagsCategory("Trigger", createTriggerFlags)
    
    # Test EDMDecodingVersion
    EDMDecode_ref = {
        "Run1": 1,
        "Run2": 2,
        "Run3": 3
    }[sample[:4]]
    log.info("EDMDecodingVersion: expected {}, configured {}".format(EDMDecode_ref, acf.Trigger.EDMDecodingVersion))
    assert( acf.Trigger.EDMDecodingVersion == EDMDecode_ref )

    return
    
if __name__=="__main__":
    log.info("Testing TriggerConfigFlags autoconfiguration")
    import sys
    for sample in sorted(inputfiles.keys()):
        run, source = sample.split('_',1)
        log.info("{} {} input file: {}".format(run, source, inputfiles[sample]))
        try:
            test_TriggerFlags(sample)
        except AssertionError:
            sys.exit(1)
            
    log.info("Done with test")
    sys.exit(0)
