#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# helper script to generate JSON file of datasets for batch pathena submission

import sys, json
prefix = sys.argv[1]
dslist = sys.argv[2:]
jlist = []
for ds in dslist:
    jlist.append({'inDS': ds, 'outDS': prefix + '.' + ds.split(':')[-1]})
with open('inOutDs.json', 'w') as ofile:
    json.dump(jlist, ofile)
