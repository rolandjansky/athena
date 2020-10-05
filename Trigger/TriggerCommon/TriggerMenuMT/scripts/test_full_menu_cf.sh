#!/bin/sh

# This is not an ART test, but a unit test

# This is a unit test of HLT Control Flow 


athena.py --imf --threads=1 --evtMax 5  --filesInput /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1 --config-only=config.pkl -c "doWriteBS=False;doWriteRDOTrigger=True;" TriggerMenuMT/full_menu_cf.py
