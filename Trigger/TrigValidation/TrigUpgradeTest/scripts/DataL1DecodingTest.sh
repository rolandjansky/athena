#!/usr/bin/env sh

get_files -xmls LVL1config_Physics_pp_v5.xml
ln -s /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/data15_13TeV.00266904.physics_EnhancedBias.merge.RAW._lb0452._SFO-1._0001.1 input.data

athena.py --threads=1 L1Decoder/decodeBS.py