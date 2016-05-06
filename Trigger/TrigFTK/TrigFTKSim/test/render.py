#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# pip install --user jinja2
# run: ./render.py > TrigFTKSim_TestConfiguration.xml
from jinja2 import Environment, FileSystemLoader
env = Environment(loader=FileSystemLoader('.'))
template = env.get_template('template.xml')

# variables used to render the output XML
max_events = 500
ftk_setup_tag = "Run2TempMapv0"
input_ftkip = "/afs/cern.ch/work/p/ptaylor/public/RTT_Input_Ntuples/InputNtuples/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7579/NTUP_FTKIP.root"
input_RDO = "/afs/cern.ch/work/p/ptaylor/public/RTT_Input_Ntuples/InputNtuples/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7579/RDO.07663480._000079.pool.root.1"
constants_dir = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/"
fit_constants_version = "test.run2.v0.1"
patterns_version = "test.run2.v0.1"
# print to stdout
print template.render(**locals())
