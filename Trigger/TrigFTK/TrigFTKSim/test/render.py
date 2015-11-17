#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# pip install --user jinja2
# run: ./render.py > TrigFTKSim_TestConfiguration.xml
from jinja2 import Environment, FileSystemLoader
env = Environment(loader=FileSystemLoader('.'))
template = env.get_template('template.xml')

# variables used to render the output XML
max_events = 100
ftk_setup_tag = "Run2TempMapv0"
input_ftkip = "/afs/cern.ch/work/p/ptaylor/public/RTT_Input_Ntuples/InputNtuples/group.trig-daq.558989_005479.EXT0._00016.NTUP_FTKIP.root"
constants_dir = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/"
fit_constants_version = "test.run2.v0.1"
patterns_version = "test.run2.v0.1"
# print to stdout
print template.render(**locals())
