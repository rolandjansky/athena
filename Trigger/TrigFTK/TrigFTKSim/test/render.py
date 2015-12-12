#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# pip install --user jinja2
# run: ./render.py > TrigFTKSim_TestConfiguration.xml
from jinja2 import Environment, FileSystemLoader
env = Environment(loader=FileSystemLoader('.'))
template = env.get_template('template.xml')

# variables used to render the output XML
max_events = 3000
ftk_setup_tag = "Run2TempMapv0"
input_ftkip = "/afs/cern.ch/work/p/ptaylor/public/RTT_Input_Ntuples/InputNtuples/user.jahreda.mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_RDO.e3698_s2608_s2183_r7192.FTK_RDO.v6_EXT0/NTUP_FTKIP.root"
constants_dir = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/"
fit_constants_version = "test.run2.v0.1"
patterns_version = "test.run2.v0.1"
# print to stdout
print template.render(**locals())
