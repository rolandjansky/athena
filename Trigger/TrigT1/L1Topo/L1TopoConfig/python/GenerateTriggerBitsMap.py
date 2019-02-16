#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Migration of Davide Gerbaudo script from L1topo validation package https://gitlab.cern.ch/atlas-trigger/l1topo/L1TopoValidation to L1TopoConfig. Example of command to run python GenerateTriggerBitsMap.py 2346 testing.txt

# A script to generate the file used to map the l1topo bit names
#
# davide.gerbaudo@gmail.com
# Aug 2016

import datetime
import os
import sys
from operator import attrgetter
from L1TopoMenu import L1TopoMenu

if __name__=="__main__":

    if len(sys.argv)<3:
        usage = "usage:\n{0:s} <SMK> <output_file>\nexample:\n{0:s} 2346 algorithm_labels.txt".format(sys.argv[0])
        raise StandardError('Missing arguments\n'+usage)
    smk = int(sys.argv[1])
    output_filename = sys.argv[2]
    menu = L1TopoMenu("TRIGGERDB", smk)
    num_expected_bits = 128

    lines = ["{:d}".format(i) for i in range(num_expected_bits)]
    for line in sorted(menu.outputLines, key=attrgetter("counter")):
        lines[int(line.counter)] = line.name
        
    preamble = """# This is an automatically generated file; do not edit
 this file was generated on {timestamp:s}
 for the trigger with SMK {smk:d}
 using the command below:
 {command:s}
""".format(**{'timestamp':str(datetime.date.today()),
                           'smk':smk,
                           'command': ' '.join(sys.argv)})
    with open(output_filename, 'w') as output_file:
        output_file.write(preamble)
        output_file.write('\n'.join(lines))
        output_file.write('\n')
    print "created {:s}".format(output_filename)

