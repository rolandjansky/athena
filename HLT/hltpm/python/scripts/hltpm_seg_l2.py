#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Tue 21 Aug 2007 12:14:52 PM CEST 

# to bypass variable lenght check in OKS (do it before OKS is loaded)
import os, sys
os.environ['OKS_SKIP_MAX_LENGTH_CHECK'] = 'true'

import pm.project
from pm.partition.utils import parse_cmdline, post_process
from pm.segment.l2 import option, gendb

# we modify the default templates file to include hlt specifics
option['template-file']['default'] = 'daq/sw/hlt-templates.data.xml'

import hltpm.common

from hltpm.option import option as hltopt
del hltopt['l2-hlt']
del hltopt['ef-hlt']
del hltopt['hlt-extra-path']
option.update(hltopt)

args = parse_cmdline(gendb.__doc__, option)
d = gendb(**args)

# now we change the stuff we need.
hltpm.common.make_hlt_segment(d, proxy=args['proxy'],
    templates=pm.project.Project(args['template-file'])) 

dbname = os.path.basename(sys.argv[0]).split('.')[:-1]
if len(d['segment']) == 1: dbname = d['segment'][0].id
d['segment'] = post_process(args, d['segment'])

save_db = pm.project.Project(dbname + '.data.xml', d['includes'])
save_db.addObjects(d['segment'])
