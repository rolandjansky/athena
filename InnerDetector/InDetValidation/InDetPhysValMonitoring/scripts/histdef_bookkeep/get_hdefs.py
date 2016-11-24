#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
make ordered list of .xml histogram definitions for 
 * input histogram title list (one per line)
 * and an input .xml file (containing unordered superset
     of input histo definitions)
use-case: introducing some order in ~ 1k histograms InDetPVM produces
'''

import sys
import string
import re
import argparse

from hist_bookkeep_utils import *

parser = argparse.ArgumentParser(
    description='Get histogram xml blocks for list of id-s')

parser.add_argument('--id',
                    dest='in_histid',
                    type=open,
                    required=True,
                    help='list of histogram id-s, one per line')

parser.add_argument('--xml',
                    dest='in_xmldef',
                    type=open,
                    required=True,
                    help='xml file with histogram definitions')

parser.add_argument('--c',
                    dest='comment',
                    required=False,
                    default='',
                    help='comment to put above each of the histograms')

args = parser.parse_args()
    
##=========================================================
# main
in_histid_name=args.in_histid.name
in_xmldef_name=args.in_xmldef.name

out_xmldef_name=in_xmldef_name.split(".")[0]+"_"+in_histid_name.split(".")[0]+".xml"
out_missingdef_name=in_xmldef_name.split(".")[0]+"_"+in_histid_name.split(".")[0]+"_missing.txt"

print ''
print ' reading input histogram ids in: ', in_histid_name
print ' reading histogram xml definitions in: ', in_xmldef_name
print ' writing histogram definitions to: ', out_xmldef_name
print ' writing histos missing definitions to: ', out_missingdef_name
print ''

out_xmldef=open(out_xmldef_name, 'w')
out_missingdef=open(out_missingdef_name, 'w')

## --------------------------------------------------------
for line in args.in_histid:
    id=re.sub('\n','',line)
    buff=get_hbuff(id,in_xmldef_name)
    if not buff:
        out_missingdef.write(id+'\n')
    else:
        for bline in buff:
            out_xmldef.write(bline)
## --------------------------------------------------------

## --------------------------------------------------------
## all done, clean-up
out_xmldef.close()
out_missingdef.close()

print ''
print 'all done'
print ''
