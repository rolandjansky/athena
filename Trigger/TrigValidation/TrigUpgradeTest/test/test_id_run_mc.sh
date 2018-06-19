#!/bin/sh
# art-type: build
# art-ci: master

athena.py --threads=1 --evtMax=10 --filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000001.pool.root.1" -c 'doCalo=False;' TrigUpgradeTest/IDCalo.py
