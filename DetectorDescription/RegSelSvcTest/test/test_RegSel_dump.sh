#!/bin/sh
# art-type: build
# art-ci: master

athena --threads=1 -c 'EvtMax=10;enableViews=True;FilesInput="root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data16_13TeV.00309640.physics_EnhancedBias.merge.RAW/data16_13TeV.00309640.physics_EnhancedBias.merge.RAW._lb0628._SFO-1._0001.1"' RegSelSvcTest/dumpRegSelSvc.py