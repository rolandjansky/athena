#!/bin/sh

# This is not an ART test, but a unit test

# This is a unit test checking that chains are assigned to 
# the correct slice. Configuration will fail if e.g.
# an egamma chain is placed in the muon slice.
#
# Combined slice is not handled because it has to import
# all the others anyway
#
# The configuration will exit early, just after attempting to
# generate a dummy config for each chain in the tested slice


athena.py -c "setMenu='LS2_v1';doWriteBS=False;doWriteRDOTrigger=True;fpeAuditor=True; doEmptyMenu=True; do${1}Slice=True; from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.Test.doDummyChainConfig=True" --filesInput=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1 --threads=1 TriggerJobOpts/runHLT_standalone.py --config-only=config.pkl
